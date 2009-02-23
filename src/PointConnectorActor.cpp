#include "StdAfx.h"
#include "PointConnectorActor.h"

#include <strstream>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkCellPicker.h>
#include <vtkProp3DCollection.h>

#include "Utilities.h"
#include "Units.h"
#include "GridKeyword.h"
#include "PropertyTreeControlBar.h"
#include "DelayRedraw.h"
#include "TreeMemento.h"
#include "MainFrm.h"
#include "Global.h"

vtkCxxRevisionMacro(CPointConnectorActor, "$Revision: 244 $");
vtkStandardNewMacro(CPointConnectorActor);

// COMMENT: {10/8/2008 1:53:49 PM}const char CPointConnectorActor::szHeading[] = "Rivers";
const bool bShowGhostPoint = true;

CPointConnectorActor::CPointConnectorActor(void)
: Radius(1.0)
, Z(0.0)
, Interactor(0)
, CurrentRenderer(0)
, Enabled(0)
, CurrentHandle(0)
, CurrentSource(0)
, CurrentId(-1)
, SelectedHandleProperty(0)
, EnabledHandleProperty(0)
, State(CPointConnectorActor::None)
, Cursor3D(0)
, Cursor3DMapper(0)
, Cursor3DActor(0)
, ConnectingLineSource(0)
, ConnectingMapper(0)
, ConnectingActor(0)
, LineCellPicker(0)
, CellPicker(0)
, TreeMemento(0)
, GhostSphereSource(0)
, GhostPolyDataMapper(0)
, GhostActor(0)
, GridAngle(0.0)
{
// COMMENT: {9/17/2008 8:22:29 PM}	static StaticInit init; // constructs/destructs s_HandleProperty s_ConnectorProperty

	for (size_t i = 0; i < 3; ++i)
	{
		this->GridOrigin[i] = 0.0;
	}

	this->Points         = vtkPoints::New();
	this->TransformUnits = vtkTransform::New();
	this->TransformScale = vtkTransform::New();
	this->TransformGrid  = vtkTransform::New();
	this->TransformUnits->Identity();
	this->TransformScale->Identity();
	this->TransformGrid->Identity();

	this->CellPicker     = vtkCellPicker::New();
	this->CellPicker->SetTolerance(0.001);
	this->CellPicker->PickFromListOn();

	this->LineCellPicker = vtkCellPicker::New();
	this->LineCellPicker->SetTolerance(0.001);
	this->LineCellPicker->PickFromListOn();

	static int count = 0;
	TRACE("CPointConnectorActor ctor %d \n", count);
	++count;

	this->EventCallbackCommand = vtkCallbackCommand::New();
	this->EventCallbackCommand->SetClientData(this);
	this->EventCallbackCommand->SetCallback(CPointConnectorActor::ProcessEvents);

	if (bShowGhostPoint)
	{
		this->GhostSphereSource = vtkSphereSource::New();
		this->GhostSphereSource->SetPhiResolution(10);
		this->GhostSphereSource->SetThetaResolution(10);
		this->GhostSphereSource->SetRadius(this->Radius);

		this->GhostPolyDataMapper = vtkPolyDataMapper::New();
		this->GhostPolyDataMapper->SetInput(this->GhostSphereSource->GetOutput());

		this->GhostActor = vtkActor::New();
		this->GhostActor->SetMapper(this->GhostPolyDataMapper);
		this->GhostActor->GetProperty()->SetColor(1, 1, 1);
		this->GhostActor->GetProperty()->SetOpacity(0.33);
		this->GhostActor->VisibilityOff();
		this->AddPart(this->GhostActor);
	}

	this->CreateDefaultProperties();

#ifdef __CPPUNIT__
	// this->SetDebug(1);
#endif
}

CPointConnectorActor::~CPointConnectorActor(void)
{
	static int count = 0;
	TRACE("CPointConnectorActor dtor %d \n", count);
	++count;

	if (this->Interactor)
	{
		// this is req'd since messages will continue to be sent
		// after deletion and this->Interactor == 0xcdcdcdcd or
		// this->Interactor->SubjectHelper == 0xcdcdcdcd
		this->Interactor->RemoveObserver(this->EventCallbackCommand);
	}
	this->EventCallbackCommand->Delete();    this->EventCallbackCommand   = 0;

	this->ClearPoints();

	this->Points->Delete();               this->Points              = 0;
	this->TransformUnits->Delete();       this->TransformUnits      = 0;
	this->TransformScale->Delete();       this->TransformScale      = 0;
	this->TransformGrid->Delete();        this->TransformGrid       = 0;
	this->CellPicker->Delete();           this->CellPicker          = 0;
	this->LineCellPicker->Delete();       this->LineCellPicker      = 0;

	this->SelectedHandleProperty->Delete();  this->SelectedHandleProperty = 0;
	this->EnabledHandleProperty->Delete();   this->EnabledHandleProperty  = 0;

	if (this->Cursor3D)
	{
		this->Cursor3D->Delete();
		this->Cursor3D = 0;
	}
	if (this->Cursor3DMapper)
	{
		this->Cursor3DMapper->Delete();
		this->Cursor3DMapper = 0;
	}
	if (this->Cursor3DActor)
	{
		this->Cursor3DActor->Delete();
		this->Cursor3DActor = 0;
	}

	if (this->ConnectingLineSource)
	{
		this->ConnectingLineSource->Delete();
		this->ConnectingLineSource = 0;
	}
	if (this->ConnectingMapper)
	{
		this->ConnectingMapper->Delete();
		this->ConnectingMapper = 0;
	}
	if (this->ConnectingActor)
	{
		this->ConnectingActor->Delete();
		this->ConnectingActor = 0;
	}
	if (this->TreeMemento)
	{
		delete this->TreeMemento;
		this->TreeMemento = 0;
	}
	if (bShowGhostPoint)
	{
		if (this->GhostSphereSource)
		{
			this->GhostSphereSource->Delete();
			this->GhostSphereSource = 0;
		}
		if (this->GhostPolyDataMapper)
		{
			this->GhostPolyDataMapper->Delete();
			this->GhostPolyDataMapper = 0;
		}
		if (this->GhostActor)
		{
			this->GhostActor->Delete();
			this->GhostActor = 0;
		}
	}

}

#pragma optimize( "g", off )	// Seems that the compiler skips the sNext[] calculation
								// assuming that the values aren't used within vtkMath::Normalize
vtkIdType CPointConnectorActor::InsertNextPoint(double x, double y, double z)
{
	// only insert point if different enough from previous
	// point otherwise an error will occur in vtkTubeFilter
	//
	if (vtkIdType np = this->Points->GetNumberOfPoints())
	{
		vtkFloatingPointType p[3];
		vtkFloatingPointType pNext[3];
		vtkFloatingPointType sNext[3];
		this->Points->GetPoint(np - 1, p);
		pNext[0] = x; pNext[1] = y; pNext[2] = z;
		for (int i = 0; i < 3; ++i)
		{
			sNext[i] = pNext[i] - p[i];
		}
		if (vtkMath::Normalize(sNext) == 0.0)
		{
			TRACE("Coincident points in CPointConnectorActor...can't compute normals\n");
			return -1; // error
		}
	}

	// append point to this->Points
	//
	double pt[3];
	pt[0] = x; pt[1] = y; pt[2] = z;
	this->TransformGrid->TransformPoint(pt, pt);
	this->TransformScale->TransformPoint(pt, pt);
	this->TransformUnits->TransformPoint(pt, pt);
	vtkIdType id = this->Points->InsertNextPoint(x, y, z);

	// add vtk objects
	//
	this->AddGraphicPoint();

	// update vtk objects
	//
	this->UpdatePoints();

	return id;
}
#pragma optimize( "", on )

void CPointConnectorActor::SetUnits(const CUnits &units, bool bUpdatePoints)
{
	double scale_h = units.horizontal.input_to_si;
	double scale_v = units.vertical.input_to_si;
	if (this->GetCoordinateSystem() == PHAST_Transform::MAP)
	{
		scale_h = units.map_horizontal.input_to_si;
	}
	if (this->GetCoordinateSystem() == PHAST_Transform::MAP)
	{
		scale_v = units.map_vertical.input_to_si;
	}
	this->TransformUnits->Identity();
	this->TransformUnits->Scale(scale_h, scale_h, scale_v);

	this->HorizonalUnits = units.horizontal.defined ? units.horizontal.input : units.horizontal.si;
	this->VerticalUnits = units.vertical.defined ? units.vertical.input : units.vertical.si;

	CGlobal::MinimizeLengthUnits(this->HorizonalUnits);
	CGlobal::MinimizeLengthUnits(this->VerticalUnits);

	if (this->GetCoordinateSystem() == PHAST_Transform::MAP)
	{
		this->HorizonalUnits = units.map_horizontal.defined ? units.map_horizontal.input : units.map_horizontal.si;
		this->VerticalUnits = units.map_vertical.defined ? units.map_vertical.input : units.map_vertical.si;
	}

	if (bUpdatePoints)
	{
		this->UpdatePoints();
	}
}

void CPointConnectorActor::SetGridKeyword(const CGridKeyword &gridKeyword, bool bUpdatePoints)
{
	this->TransformGrid->Identity();
	if (this->GetCoordinateSystem() == PHAST_Transform::MAP)
	{
		this->TransformGrid->RotateZ(-gridKeyword.m_grid_angle);
		this->TransformGrid->Translate(-gridKeyword.m_grid_origin[0], -gridKeyword.m_grid_origin[1], -gridKeyword.m_grid_origin[2]);
	}

	if (bUpdatePoints)
	{
		this->UpdatePoints();
	}
}

void CPointConnectorActor::SetScale(float x, float y, float z)
{
	this->SetScale(double(x), double(y), double(z));
}

void CPointConnectorActor::SetZ(double z)
{
	this->Z = z;

	double pt[3];
	vtkIdType np = this->Points->GetNumberOfPoints();
	for (vtkIdType i = 0; i < np; ++i)
	{
		this->Points->GetPoint(i, pt);
		pt[2] = this->Z;
		this->Points->SetPoint(i, pt);
	}
	this->UpdatePoints();
}

double CPointConnectorActor::GetZ(void)const
{
	return this->Z;
}

void CPointConnectorActor::SetScale(double x, double y, double z)
{
	vtkFloatingPointType point[3];
	if (this->State == CPointConnectorActor::CreatingRiver)
	{
		if (this->Points->GetNumberOfPoints())
		{
			this->ConnectingLineSource->GetPoint1(point);
			this->TransformScale->GetInverse()->TransformPoint(point, point);
		}
	}
	this->TransformScale->Identity();
	this->TransformScale->Scale(x, y, z);
	if (this->State == CPointConnectorActor::CreatingRiver)
	{
		if (this->Points->GetNumberOfPoints())
		{
			this->TransformScale->TransformPoint(point, point);
			this->ConnectingLineSource->SetPoint1(point);
		}
		this->Interactor->Render();
	}
	this->UpdatePoints();
}

void CPointConnectorActor::UpdatePoints(void)
{
	double pt[3]; double prev_pt[3];
	vtkIdType np = this->Points->GetNumberOfPoints();
	ASSERT(np == this->SphereSources.size());
	ASSERT(np == this->Actors.size());
	ASSERT(np == this->PolyDataMappers.size());

	ASSERT(np - 1 == this->LineSources.size() || this->LineSources.size() == 0);

	std::list<vtkSphereSource*>::iterator iterSphereSource = this->SphereSources.begin();
	std::list<vtkLineSource*>::iterator iterLineSource = this->LineSources.begin();
	for (vtkIdType i = 0; i < np; ++i, ++iterSphereSource)
	{
		this->Points->GetPoint(i, pt);
		this->TransformGrid->TransformPoint(pt, pt);
		this->TransformScale->TransformPoint(pt, pt);
		this->TransformUnits->TransformPoint(pt, pt);

		if (iterSphereSource != this->SphereSources.end())
		{
			(*iterSphereSource)->SetCenter(pt[0], pt[1], pt[2]);
		}
		if (i > 0)
		{
			if (iterLineSource != this->LineSources.end())
			{
				(*iterLineSource)->SetPoint1(prev_pt[0], prev_pt[1], prev_pt[2]);
				(*iterLineSource)->SetPoint2(pt[0], pt[1], pt[2]);
				++iterLineSource;
			}
		}
		prev_pt[0] = pt[0];   prev_pt[1] = pt[1];   prev_pt[2] = pt[2];
	}
}

void CPointConnectorActor::ClearPoints(void)
{
	this->CurrentHandle = NULL;

	this->Points->Initialize();
	ASSERT(this->Points->GetNumberOfPoints() == 0);

	std::list<vtkSphereSource*>::iterator iterSphereSource = this->SphereSources.begin();
	for(; iterSphereSource != this->SphereSources.end(); ++iterSphereSource)
	{
		(*iterSphereSource)->Delete();
	}
	this->SphereSources.clear();

	std::list<vtkPolyDataMapper*>::iterator iterPolyDataMapper = this->PolyDataMappers.begin();
	for(; iterPolyDataMapper != this->PolyDataMappers.end(); ++iterPolyDataMapper)
	{
		(*iterPolyDataMapper)->Delete();
	}
	this->PolyDataMappers.clear();

	std::list<vtkActor*>::iterator iterActor = this->Actors.begin();
	for(; iterActor != this->Actors.end(); ++iterActor)
	{
		this->RemovePart(*iterActor);
		this->CellPicker->DeletePickList(*iterActor);
		(*iterActor)->Delete();
	}
	this->Actors.clear();

	std::list<vtkLineSource*>::iterator iterLineSource = this->LineSources.begin();
	for(; iterLineSource != this->LineSources.end(); ++iterLineSource)
	{
		(*iterLineSource)->Delete();
	}
	this->LineSources.clear();

	std::list<vtkTubeFilter*>::iterator iterTubeFilter = this->TubeFilters.begin();
	for(; iterTubeFilter != this->TubeFilters.end(); ++iterTubeFilter)
	{
		(*iterTubeFilter)->Delete();
	}
	this->TubeFilters.clear();

	std::list<vtkPolyDataMapper*>::iterator iterLinePolyDataMapper = this->LinePolyDataMappers.begin();
	for(; iterLinePolyDataMapper != this->LinePolyDataMappers.end(); ++iterLinePolyDataMapper)
	{
		(*iterLinePolyDataMapper)->Delete();
	}
	this->LinePolyDataMappers.clear();

	std::list<vtkActor*>::iterator iterLineActor = this->LineActors.begin();
	for(; iterLineActor != this->LineActors.end(); ++iterLineActor)
	{
		this->RemovePart(*iterLineActor);
		this->LineCellPicker->DeletePickList(*iterLineActor);
		(*iterLineActor)->Delete();
	}
	this->LineActors.clear();
}

vtkFloatingPointType CPointConnectorActor::GetRadius(void)const
{
	return this->Radius;
}

void CPointConnectorActor::SetRadius(vtkFloatingPointType radius)
{
	this->Radius = radius;
	std::list<vtkSphereSource*>::iterator iterSphereSource = this->SphereSources.begin();
	for (; iterSphereSource != this->SphereSources.end(); ++iterSphereSource)
	{
		(*iterSphereSource)->SetRadius(this->Radius);
	}

	std::list<vtkTubeFilter*>::iterator iterTubeFilter = this->TubeFilters.begin();
	for (; iterTubeFilter != this->TubeFilters.end(); ++iterTubeFilter)
	{
		(*iterTubeFilter)->SetRadius(this->Radius / 2.0);
	}
	if (bShowGhostPoint)
	{
		if (this->GhostSphereSource)
		{
			this->GhostSphereSource->SetRadius(this->Radius);
		}
	}
}

void CPointConnectorActor::SetInteractor(vtkRenderWindowInteractor* i)
{
	if (i == this->Interactor)
	{
		return;
	}

	// if we already have an Interactor then stop observing it
	if (this->Interactor)
	{
		this->Interactor->RemoveObserver(this->EventCallbackCommand);
	}

	this->Interactor = i;

	if (i)
	{
		this->SetEnabled(1);
	}

	this->Modified();
}

//----------------------------------------------------------------------------
void CPointConnectorActor::ProcessEvents(vtkObject* vtkNotUsed(object),
									   unsigned long event,
									   void* clientdata,
									   void* vtkNotUsed(calldata))
{
	CPointConnectorActor* self
		= reinterpret_cast<CPointConnectorActor *>( clientdata );

	TRACE("CPointConnectorActor::ProcessEvents Interactor= %p\n", self->Interactor);

	switch(event)
	{
	case vtkCommand::ExposeEvent:
		TRACE("vtkCommand::ExposeEvent\n");
		break;

	case vtkCommand::ConfigureEvent:
		TRACE("vtkCommand::ConfigureEvent\n");
		break;

	case vtkCommand::EnterEvent:
		TRACE("vtkCommand::EnterEvent\n");
		break;

	case vtkCommand::LeaveEvent:
		TRACE("vtkCommand::LeaveEvent\n");
		break;

	case vtkCommand::TimerEvent:
		TRACE("vtkCommand::TimerEvent\n");
		break;

	case vtkCommand::MouseMoveEvent:
		TRACE("vtkCommand::MouseMoveEvent\n");
		self->OnMouseMove();
		break;

	case vtkCommand::LeftButtonPressEvent:
		TRACE("vtkCommand::LeftButtonPressEvent\n");
		self->OnLeftButtonDown();
		break;

	case vtkCommand::LeftButtonReleaseEvent:
		TRACE("vtkCommand::LeftButtonReleaseEvent\n");
		self->OnLeftButtonUp();
		break;

	case vtkCommand::MiddleButtonPressEvent:
		TRACE("vtkCommand::MiddleButtonPressEvent\n");
		break;

	case vtkCommand::MiddleButtonReleaseEvent:
		TRACE("vtkCommand::MiddleButtonReleaseEvent\n");
		break;

	case vtkCommand::RightButtonPressEvent:
		TRACE("vtkCommand::RightButtonPressEvent\n");
		break;

	case vtkCommand::RightButtonReleaseEvent:
		TRACE("vtkCommand::RightButtonReleaseEvent\n");
		break;

	case vtkCommand::KeyPressEvent:
		TRACE("vtkCommand::KeyPressEvent\n");
		self->OnKeyPress();
		break;

	case vtkCommand::KeyReleaseEvent:
		TRACE("vtkCommand::KeyReleaseEvent\n");
		break;

	case vtkCommand::CharEvent:
		TRACE("vtkCommand::CharEvent\n");
		break;

	case vtkCommand::DeleteEvent:
		TRACE("vtkCommand::DeleteEvent\n");
		self->Interactor = 0;
		break;

	default:
		TRACE("ProcessEvents = default\n");
		break;
	}
}

void CPointConnectorActor::OnLeftButtonDown()
{
	if (!this->Interactor) return;

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if ( ren != this->CurrentRenderer )
	{
		return;
	}

	if (this->State == CPointConnectorActor::None)
	{
		// move point
		//
		vtkAssemblyPath *path;
		this->CellPicker->Pick(X, Y, 0.0, ren);
		path = this->CellPicker->GetPath();
		if (path != NULL)
		{
			if (vtkActor* pActor = vtkActor::SafeDownCast(path->GetFirstNode()->GetProp()))
			{
				this->HighlightHandle(path->GetFirstNode()->GetProp());
				this->Points->GetPoint(this->GetCurrentPointId(), this->WorldPointXYPlane);
				this->EventCallbackCommand->SetAbortFlag(1);
				this->State = CPointConnectorActor::MovingPoint;
				this->InvokeEvent(CPointConnectorActor::StartMovePointEvent, NULL);
				this->Interactor->Render();
				return;
			}
		}

		// create new point
		//
		this->LineCellPicker->Pick(X, Y, 0.0, ren);
		path = this->LineCellPicker->GetPath();
		if (path != NULL)
		{
			if (vtkActor* pActor = vtkActor::SafeDownCast(path->GetFirstNode()->GetProp()))
			{
				std::list<vtkActor*>::iterator iterActor = this->LineActors.begin();
				for (vtkIdType id = 0; iterActor != this->LineActors.end(); ++id, ++iterActor)
				{
					if (pActor == *iterActor)
					{
						// add and select point
						this->Update();
						this->InsertPoint(id, this->WorldPointXYPlane[0], this->WorldPointXYPlane[1], this->WorldPointXYPlane[2]);
						this->SelectPoint(id + 1);

						// notify listeners
						this->InvokeEvent(CPointConnectorActor::InsertPointEvent, NULL);
						break;
					}
				}
				this->EventCallbackCommand->SetAbortFlag(1);
			}
		}
	}

	if (this->State == CPointConnectorActor::CreatingRiver)
	{
		this->Update();
		this->Cursor3DActor->SetPosition(this->WorldSIPoint[0], this->WorldSIPoint[1], this->WorldSIPoint[2]);
		this->Interactor->Render();
	}
}

void CPointConnectorActor::OnMouseMove()
{
	if (!this->Interactor) return;

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if ( ren != this->CurrentRenderer )
	{
		return;
	}

	if (bShowGhostPoint)
	{
		if (this->State == CPointConnectorActor::None)
		{
			vtkAssemblyPath *path;
			this->CellPicker->Pick(X, Y, 0.0, ren);
			path = this->CellPicker->GetPath();

			if (path == NULL && !(::GetAsyncKeyState(VK_LBUTTON) < 0) && !(::GetAsyncKeyState(VK_RBUTTON) < 0) && !(::GetAsyncKeyState(VK_MBUTTON) < 0))
			{
				this->LineCellPicker->Pick(X, Y, 0.0, ren);
				path = this->LineCellPicker->GetPath();
				if (path)
				{
					TRACE("found\n");
				}
				this->LineCellPicker->Pick(X, Y, 0.0, ren);
				path = this->LineCellPicker->GetPath();
				if (path != NULL)
				{
					if (vtkActor* pActor = vtkActor::SafeDownCast(path->GetFirstNode()->GetProp()))
					{
						this->Update();
						this->GhostSphereSource->SetCenter(this->WorldSIPoint[0], this->WorldSIPoint[1], this->WorldSIPoint[2]);
						this->GhostActor->VisibilityOn();
						this->Interactor->Render();
					}
				}
				else
				{
					this->GhostActor->VisibilityOff();
					this->Interactor->Render();
				}
			}
			else
			{
				this->GhostActor->VisibilityOff();
				this->Interactor->Render();
			}
		}
	}

	if (this->State == CPointConnectorActor::MovingPoint)
	{
		// update WorldPointXYPlane
		//
		this->Update();
// COMMENT: {2/9/2009 10:25:03 PM}		//{{
// COMMENT: {2/9/2009 10:25:03 PM}		double pt[3];
// COMMENT: {2/9/2009 10:25:03 PM}		this->TransformGrid->GetInverse()->TransformPoint(this->WorldPointXYPlane, pt);
// COMMENT: {2/9/2009 10:25:03 PM}		this->TransformScale->GetInverse()->TransformPoint(pt, pt);
// COMMENT: {2/9/2009 10:25:03 PM}		this->TransformUnits->GetInverse()->TransformPoint(pt, pt);
// COMMENT: {2/9/2009 10:25:03 PM}		//}}
		this->Points->SetPoint(this->CurrentId, this->WorldPointXYPlane);
		this->UpdatePoints();

		this->EventCallbackCommand->SetAbortFlag(1);
		this->InvokeEvent(CPointConnectorActor::MovingPointEvent, NULL);
		this->Interactor->Render();
	}

	if (this->State == CPointConnectorActor::CreatingRiver)
	{
		this->Update();
		this->Cursor3DActor->SetPosition(this->WorldSIPoint[0], this->WorldSIPoint[1], this->WorldSIPoint[2]);
		this->Cursor3DActor->VisibilityOn();
		if (this->Points->GetNumberOfPoints())
		{
			this->ConnectingLineSource->SetPoint2(this->WorldSIPoint[0], this->WorldSIPoint[1], this->WorldSIPoint[2]);
			this->ConnectingActor->VisibilityOn();
		}
		this->EventCallbackCommand->SetAbortFlag(1);
		this->Interactor->Render();
	}
}

void CPointConnectorActor::OnLeftButtonUp()
{
	if (!this->Interactor) return;

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if ( ren != this->CurrentRenderer )
	{
		return;
	}

	if (this->State == CPointConnectorActor::MovingPoint)
	{
		// update WorldPointXYPlane
		//
		this->Update();
		this->Points->SetPoint(this->CurrentId, this->WorldPointXYPlane);
		this->UpdatePoints();

		this->HighlightHandle(NULL);
		this->EventCallbackCommand->SetAbortFlag(1);
		this->State = CPointConnectorActor::None;
		this->InvokeEvent(CPointConnectorActor::EndMovePointEvent, NULL);
		this->Interactor->Render();
	}

	if (this->State == CPointConnectorActor::CreatingRiver)
	{
		this->Update();
		if (this->InsertNextPoint(this->WorldPointXYPlane[0], this->WorldPointXYPlane[1], this->WorldPointXYPlane[2]) != -1)
		{
			this->UpdatePoints();
			this->ConnectingLineSource->SetPoint1(this->WorldSIPoint[0], this->WorldSIPoint[1], this->WorldSIPoint[2]);
			this->ConnectingLineSource->SetPoint2(this->WorldSIPoint[0], this->WorldSIPoint[1], this->WorldSIPoint[2]);
			this->Interactor->Render();
		}
	}
}

void CPointConnectorActor::OnKeyPress()
{
	if (!this->Interactor) return;

	if (this->State == CPointConnectorActor::CreatingRiver)
	{
		char* keysym = this->Interactor->GetKeySym();
		if (::strcmp(keysym, "Escape") == 0)
		{
			this->CancelNew();
		}
		else
		{
			this->EndNew();
		}
		this->EventCallbackCommand->SetAbortFlag(1);
	}
}

int CPointConnectorActor::HighlightHandle(vtkProp *prop)
{
	// first unhighlight anything picked
	if (this->CurrentHandle)
	{
		if (this->Enabled)
		{
			this->CurrentHandle->SetProperty(this->EnabledHandleProperty);
		}
		else
		{
			this->CurrentHandle->SetProperty(this->GetHandleProperty());
		}
	}

	this->CurrentHandle = (vtkActor *)prop;

	if (this->CurrentHandle)
	{
		this->CurrentHandle->SetProperty(this->SelectedHandleProperty);
		std::list<vtkActor*>::iterator iterActor = this->Actors.begin();
		std::list<vtkSphereSource*>::iterator iterSource = this->SphereSources.begin();
		for (vtkIdType i = 0; iterActor != this->Actors.end(); ++i, ++iterActor, ++iterSource)
		{
			if (this->CurrentHandle == *iterActor)
			{
				this->CurrentSource = *iterSource;
				this->CurrentId = i;
				return i;
			}
		}
	}
	return -1;
}

void CPointConnectorActor::CreateDefaultProperties()
{
  this->SelectedHandleProperty = vtkProperty::New();
  this->SelectedHandleProperty->SetOpacity(0.7);
  this->SelectedHandleProperty->SetColor(1, 0, 0);

  this->EnabledHandleProperty = vtkProperty::New();
  this->EnabledHandleProperty->SetOpacity(0.7);
  this->EnabledHandleProperty->SetColor(1, 1, 1);
}

void CPointConnectorActor::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	this->Points->PrintSelf(os, indent.GetNextIndent());
}

void CPointConnectorActor::SetEnabled(int enabling)
{
	if (!this->Interactor)
	{
		vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
		return;
	}

	if ( enabling ) //------------------------------------------------------------
	{
		vtkDebugMacro(<<"Enabling CPointConnectorActor");

		if ( this->Enabled ) //already enabled, just return
		{
			return;
		}

		if ( ! this->CurrentRenderer )
		{
			this->CurrentRenderer = this->Interactor->FindPokedRenderer(
				this->Interactor->GetLastEventPosition()[0],
				this->Interactor->GetLastEventPosition()[1]);
			if (this->CurrentRenderer == NULL)
			{
				return;
			}
		}

		this->Enabled = 1;

		// listen to the following events
		vtkRenderWindowInteractor *i = this->Interactor;
		i->AddObserver(vtkCommand::MouseMoveEvent,
			this->EventCallbackCommand, 10);
		i->AddObserver(vtkCommand::LeftButtonPressEvent,
			this->EventCallbackCommand, 10);
		i->AddObserver(vtkCommand::LeftButtonReleaseEvent,
			this->EventCallbackCommand, 10);
		i->AddObserver(vtkCommand::KeyPressEvent,
			this->EventCallbackCommand, 10);
		i->AddObserver(vtkCommand::DeleteEvent,       // this doesn't seem to be working
			this->EventCallbackCommand, 10);

		std::list<vtkActor*>::iterator iterActor = this->Actors.begin();
		for(; iterActor != this->Actors.end(); ++iterActor)
		{
			(*iterActor)->SetProperty(this->EnabledHandleProperty);
		}
	}

	else //disabling-------------------------------------------------------------
	{
		vtkDebugMacro(<<"Disabling river");

		if ( ! this->Enabled ) //already disabled, just return
		{
			return;
		}

		this->Enabled = 0;

		// don't listen for events any more
		this->Interactor->RemoveObserver(this->EventCallbackCommand);

		this->CurrentHandle = NULL;
		this->CurrentRenderer = NULL;

		std::list<vtkActor*>::iterator iterActor = this->Actors.begin();
		for(; iterActor != this->Actors.end(); ++iterActor)
		{
			(*iterActor)->SetProperty(this->GetHandleProperty());
		}
	}
	this->Interactor->Render();
}

void CPointConnectorActor::Update()
{
	if (!this->CurrentRenderer) return;

	double zPos = this->Z * this->TransformScale->GetMatrix()->GetElement(2, 2) * this->TransformUnits->GetMatrix()->GetElement(2, 2);
	CUtilities::GetWorldPointAtFixedPlane(this->Interactor, this->CurrentRenderer, 2, zPos, this->WorldPointXYPlane);
	for (int i = 0; i < 3; ++i)
	{
		this->WorldSIPoint[i] = this->WorldPointXYPlane[i];
	}

	// UN-SCALE
	this->TransformGrid->GetInverse()->TransformPoint(this->WorldPointXYPlane, this->WorldPointXYPlane);
	this->TransformScale->GetInverse()->TransformPoint(this->WorldPointXYPlane, this->WorldPointXYPlane);
	this->TransformUnits->GetInverse()->TransformPoint(this->WorldPointXYPlane, this->WorldPointXYPlane);

	// this is probably wrong (should use same point in and out and add TransformGrid)
	this->TransformScale->GetInverse()->TransformPoint(this->WorldSIPoint, this->WorldScaledUnitPoint);
	this->TransformUnits->GetInverse()->TransformPoint(this->WorldSIPoint, this->WorldScaledUnitPoint);

	// update status bar
	((CMainFrame*)::AfxGetMainWnd())->UpdateGrid(
		this->WorldPointXYPlane[0],
		this->WorldPointXYPlane[1],
		this->WorldPointXYPlane[2],
		this->HorizonalUnits,
		this->VerticalUnits
		);

	this->WorldPointXYPlane[2] = this->Z;
	this->WorldSIPoint[2] = zPos;
}

void CPointConnectorActor::Add(CPropertyTreeControlBar *pTree, HTREEITEM hInsertAfter)
{
	// no-op
}

void CPointConnectorActor::UnAdd(CPropertyTreeControlBar *pTree)
{
	if (this->Node)
	{
		CTreeCtrlNode node = this->Node.GetParent();
		node.Select();
		VERIFY(this->Node.Delete());
	}
}

void CPointConnectorActor::Remove(CPropertyTreeControlBar *pTree)
{
	ASSERT(this->TreeMemento == 0);
	this->TreeMemento = new CTreeMemento(this->Node);
	this->Node.Delete();
}

void CPointConnectorActor::UnRemove(CPropertyTreeControlBar *pTree)
{
	ASSERT(this->TreeMemento != 0);
	this->Node = this->TreeMemento->Restore();
	delete this->TreeMemento;
	this->TreeMemento = 0;

	this->Node.SetData((DWORD_PTR)this);
	VERIFY(this->Node.Select());
}

void CPointConnectorActor::Update(CTreeCtrlNode node)
{
	// no-op
}

vtkActor* CPointConnectorActor::GetPoint(int index)
{
	std::list<vtkActor*>::iterator iterActor = this->Actors.begin();
	for (int i = 0; iterActor != this->Actors.end(); ++i, ++iterActor)
	{
		if (i == index)
		{
			return *iterActor;
		}
	}
	ASSERT(FALSE);
	return NULL;
}

void CPointConnectorActor::SelectPoint(int index)
{
	this->HighlightHandle(NULL);
	this->HighlightHandle(this->GetPoint(index));
	this->CurrentSource = NULL;
	if (this->Interactor)
	{
		this->Interactor->Render();
	}
}

void CPointConnectorActor::ClearSelection(void)
{
	this->HighlightHandle(NULL);
}

vtkIdType CPointConnectorActor::GetCurrentPointId(void)const
{
	return this->CurrentId;
}

double* CPointConnectorActor::GetCurrentPointPosition(void)
{
	return this->WorldPointXYPlane;
}

size_t CPointConnectorActor::GetPointCount(void)const
{
	return this->Actors.size();
}

void CPointConnectorActor::GetCurrentPointPosition(double x[3])const
{
	x[0] = this->WorldPointXYPlane[0];
	x[1] = this->WorldPointXYPlane[1];
	x[2] = this->WorldPointXYPlane[2];
}

void CPointConnectorActor::MovePoint(vtkIdType id, double x, double y)
{
	// update points object
	//
	if (this->Points)
	{
		double pt[3];
		this->Points->GetPoint(id, pt); // get previous z-value
		pt[0] = x;
		pt[1] = y;
		this->Points->SetPoint(id, pt);
		this->UpdatePoints();
		if (this->Interactor)
		{
			this->Interactor->Render();
		}
	}
}

LPCTSTR CPointConnectorActor::GetName(void)const
{
	return this->Name.c_str();
}

void CPointConnectorActor::SetVisibility(int visibility)
{
	vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Visibility to " << visibility);
	if (this->Visibility != visibility)
	{
		std::list<vtkActor*>::iterator it = this->Actors.begin();
		for (; it != this->Actors.end(); ++it)
		{
			(*it)->SetVisibility(visibility);
		}

		std::list<vtkActor*>::iterator itLine = this->LineActors.begin();
		for (; itLine != this->LineActors.end(); ++itLine)
		{
			(*itLine)->SetVisibility(visibility);
		}

		this->Visibility = visibility;
		this->Modified();
	}
}

int CPointConnectorActor::GetVisibility(void)
{
	vtkDebugMacro(<< this->GetClassName() << " (" << this << "): returning Visibility of " << this->Visibility );
	return this->Visibility;
}

void CPointConnectorActor::ScaleFromBounds(vtkFloatingPointType bounds[6])
{
	// set radius
	//
	vtkFloatingPointType defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
	this->SetRadius(defaultAxesSize * 0.085 /* / sqrt(scale[0] * scale[1]) */ );

	if (this->Cursor3D)
	{
		// set size of 3D cursor
		//
		vtkFloatingPointType dim = (bounds[1] - bounds[0]) / 20.0;
		this->Cursor3D->SetModelBounds(-dim, dim, -dim, dim, -dim, dim);
	}
}

void CPointConnectorActor::CancelNew()
{
	if (this->State == CPointConnectorActor::CreatingRiver)
	{
		ASSERT(this->Cursor3D != 0);
		ASSERT(this->Cursor3DMapper != 0);
		ASSERT(this->Cursor3DActor != 0);
		if (this->Cursor3DActor)
		{
			ASSERT(this->GetParts()->IsItemPresent(this->Cursor3DActor));
			this->RemovePart(this->Cursor3DActor);
			this->Cursor3DActor->Delete();
			this->Cursor3DActor = 0;
		}
		if (this->Cursor3DMapper)
		{
			this->Cursor3DMapper->Delete();
			this->Cursor3DMapper = 0;
		}
		if (this->Cursor3D)
		{
			this->Cursor3D->Delete();
			this->Cursor3D = 0;
		}

		ASSERT(this->ConnectingLineSource != 0);
		ASSERT(this->ConnectingMapper != 0);
		ASSERT(this->ConnectingActor != 0);
		if (this->ConnectingActor)
		{
			ASSERT(this->GetParts()->IsItemPresent(this->ConnectingActor));
			this->RemovePart(this->ConnectingActor);
			this->ConnectingActor->Delete();
			this->ConnectingActor = 0;
		}
		if (this->ConnectingMapper)
		{
			this->ConnectingMapper->Delete();
			this->ConnectingMapper = 0;
		}
		if (this->ConnectingLineSource)
		{
			this->ConnectingLineSource->Delete();
			this->ConnectingLineSource = 0;
		}
		this->Interactor->Render();
		this->InvokeEvent(CPointConnectorActor::CancelNewEvent, NULL);
	}
	this->SetEnabled(0);
	this->State = CPointConnectorActor::None;
}

void CPointConnectorActor::EndNew()
{
	if (this->State == CPointConnectorActor::CreatingRiver)
	{
		ASSERT(this->Cursor3D != 0);
		ASSERT(this->Cursor3DMapper != 0);
		ASSERT(this->Cursor3DActor != 0);
		if (this->Cursor3DActor)
		{
			ASSERT(this->GetParts()->IsItemPresent(this->Cursor3DActor));
			this->RemovePart(this->Cursor3DActor);
			this->Cursor3DActor->Delete();
			this->Cursor3DActor = 0;
		}
		if (this->Cursor3DMapper)
		{
			this->Cursor3DMapper->Delete();
			this->Cursor3DMapper = 0;
		}
		if (this->Cursor3D)
		{
			this->Cursor3D->Delete();
			this->Cursor3D = 0;
		}

		ASSERT(this->ConnectingLineSource != 0);
		ASSERT(this->ConnectingMapper != 0);
		ASSERT(this->ConnectingActor != 0);
		if (this->ConnectingActor)
		{
			ASSERT(this->GetParts()->IsItemPresent(this->ConnectingActor));
			this->RemovePart(this->ConnectingActor);
			this->ConnectingActor->Delete();
			this->ConnectingActor = 0;
		}
		if (this->ConnectingMapper)
		{
			this->ConnectingMapper->Delete();
			this->ConnectingMapper = 0;
		}
		if (this->ConnectingLineSource)
		{
			this->ConnectingLineSource->Delete();
			this->ConnectingLineSource = 0;
		}
		this->Interactor->Render();
		this->InvokeEvent(CPointConnectorActor::EndNewEvent, NULL);
	}
	this->SetEnabled(0);
	this->State = CPointConnectorActor::None;
}

void CPointConnectorActor::InsertPoint(vtkIdType id, double x, double y, double z)
{
	// move existing points after inserted point
	//
	double prev[3];
	vtkIdType count = this->Points->GetNumberOfPoints();
	for (vtkIdType j = count; j > id; --j)
	{
		this->Points->GetPoint(j - 1, prev);
		this->Points->InsertPoint(j, prev);
	}

	this->Points->InsertPoint(id + 1, x, y, z);
	this->AddGraphicPoint();
	this->UpdatePoints();

	if (this->Interactor)
	{
		this->Interactor->Render();
	}
}

void CPointConnectorActor::AddGraphicPoint(void)
{
	// create new point
	//
	vtkSphereSource *pSphereSource = vtkSphereSource::New();
	pSphereSource->SetPhiResolution(10);
	pSphereSource->SetThetaResolution(10);
	pSphereSource->SetRadius(this->Radius);
	this->SphereSources.push_back(pSphereSource);

	vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
	pPolyDataMapper->SetInput(pSphereSource->GetOutput());
	this->PolyDataMappers.push_back(pPolyDataMapper);

	vtkLODActor *pActor = vtkLODActor::New();
	pActor->SetMapper(pPolyDataMapper);
	if (this->Enabled)
	{
		pActor->SetProperty(this->EnabledHandleProperty);
	}
	else
	{
		pActor->SetProperty(this->GetHandleProperty());
	}
	this->Actors.push_back(pActor);
	this->CellPicker->AddPickList(pActor);
	this->AddPart(pActor);

	if (this->Points->GetNumberOfPoints() > 1)
	{
		// create new connecting tube
		//
		vtkLineSource *pLineSource = vtkLineSource::New();
		this->LineSources.push_back(pLineSource);

		vtkTubeFilter *pTubeFilter = vtkTubeFilter::New();
		pTubeFilter->SetNumberOfSides(8);
		pTubeFilter->SetInput(pLineSource->GetOutput());
		pTubeFilter->SetRadius(this->Radius / 2.0);
		this->TubeFilters.push_back(pTubeFilter);

		vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
		pPolyDataMapper->SetInput(pTubeFilter->GetOutput());
		this->LinePolyDataMappers.push_back(pPolyDataMapper);

		vtkLODActor *pActor = vtkLODActor::New();
		pActor->SetMapper(pPolyDataMapper);
		pActor->SetProperty(this->GetConnectorProperty());
		this->LineActors.push_back(pActor);
		this->LineCellPicker->AddPickList(pActor);
		this->AddPart(pActor);
	}
}

void CPointConnectorActor::DeletePoint(vtkIdType id)
{
	ASSERT(id < this->Points->GetNumberOfPoints());

	// reset color
	//
	vtkActor *pActor = this->GetPoint(id);
	if (this->Enabled)
	{
		pActor->SetProperty(this->EnabledHandleProperty);
	}
	else
	{
		pActor->SetProperty(this->GetHandleProperty());
	}

	double next[3];
	vtkIdType count = this->Points->GetNumberOfPoints();
	for (vtkIdType j = id; j < count; ++j)
	{
		this->Points->GetPoint(j + 1, next);
		this->Points->InsertPoint(j, next);
	}
	this->Points->SetNumberOfPoints(count - 1);

	this->DeleteGraphicPoint();
	this->UpdatePoints();
	if (this->Interactor)
	{
		this->Interactor->Render();
	}
}

void CPointConnectorActor::DeleteGraphicPoint(void)
{
	// remove last point
	//
	vtkSphereSource *pSphereSource = this->SphereSources.back();
	pSphereSource->Delete();
	this->SphereSources.pop_back();

	vtkPolyDataMapper *pPolyDataMapper = this->PolyDataMappers.back();
	pPolyDataMapper->Delete();
	this->PolyDataMappers.pop_back();

	vtkActor *pActor = this->Actors.back();
	this->CellPicker->DeletePickList(pActor);
	this->RemovePart(pActor);
	pActor->Delete();
	this->Actors.pop_back();

	if (true)
	{
		vtkLineSource *pLineSource = this->LineSources.back();
		pLineSource->Delete();
		this->LineSources.pop_back();

		vtkTubeFilter *pTubeFilter = this->TubeFilters.back();
		pTubeFilter->Delete();
		this->TubeFilters.pop_back();

		vtkPolyDataMapper *pPolyDataMapper = this->LinePolyDataMappers.back();
		pPolyDataMapper->Delete();
		this->LinePolyDataMappers.pop_back();

		vtkActor *pActor = this->LineActors.back();
		this->LineCellPicker->DeletePickList(pActor);
		this->RemovePart(pActor);
		pActor->Delete();
		this->LineActors.pop_back();
	}
}

#ifdef WIN32
BOOL CPointConnectorActor::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (this->Enabled)
	{
		::SetCursor(::AfxGetApp()->LoadCursor(IDC_NULL));
		return TRUE;
	}
	return FALSE;
}
#endif // WIN32
