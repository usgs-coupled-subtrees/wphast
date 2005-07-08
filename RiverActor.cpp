#include "StdAfx.h"
#include "RiverActor.h"

#include <strstream>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkCellPicker.h>
#include <vtkProp3DCollection.h>

#include "Units.h"
#include "PropertyTreeControlBar.h"
#include "DelayRedraw.h"

vtkCxxRevisionMacro(CRiverActor, "$Revision: 244 $");
vtkStandardNewMacro(CRiverActor);


CRiverActor::CRiverActor(void)
: m_Radius(1.0)
, m_Z(0.0)
, Interactor(0)
, CurrentRenderer(0)
, Enabled(0)
, CurrentHandle(0)
, CurrentSource(0)
, CurrentId(-1)
, HandleProperty(0)
, SelectedHandleProperty(0)
, EnabledHandleProperty(0)
, State(CRiverActor::None)
, m_pCursor3D(0)
, m_pCursor3DMapper(0)
, m_pCursor3DActor(0)
, ConnectingLineSource(0)
, ConnectingMapper(0)
, ConnectingActor(0)
, m_pLineCellPicker(0)
, m_pCellPicker(0)
{
	this->m_pPoints         = vtkPoints::New();
	this->m_pTransformUnits = vtkTransform::New();
	this->m_pTransformScale = vtkTransform::New();
	this->m_pTransformUnits->Identity();
	this->m_pTransformScale->Identity();

	this->m_pCellPicker     = vtkCellPicker::New();
	this->m_pCellPicker->SetTolerance(0.001);
	this->m_pCellPicker->PickFromListOn();

	this->m_pLineCellPicker     = vtkCellPicker::New();
	this->m_pLineCellPicker->SetTolerance(0.001);
	this->m_pLineCellPicker->PickFromListOn();

	static int count = 0;
	TRACE("CRiverActor ctor %d \n", count);
	++count;

	this->EventCallbackCommand = vtkCallbackCommand::New();
	this->EventCallbackCommand->SetClientData(this); 
	this->EventCallbackCommand->SetCallback(CRiverActor::ProcessEvents);

	this->CreateDefaultProperties();

#ifdef __CPPUNIT__
	// this->SetDebug(1);
#endif
}

CRiverActor::~CRiverActor(void)
{
	static int count = 0;
	TRACE("CRiverActor dtor %d \n", count);
	++count;

	if (this->Interactor)
	{
		this->Interactor->RemoveObserver(this->EventCallbackCommand);
		this->Interactor->UnRegister(this);
	}
	this->EventCallbackCommand->Delete();    this->EventCallbackCommand   = 0;

	this->ClearPoints();

	this->m_pPoints->Delete();               this->m_pPoints              = 0;
	this->m_pTransformUnits->Delete();       this->m_pTransformUnits      = 0;
	this->m_pTransformScale->Delete();       this->m_pTransformScale      = 0;
	this->m_pCellPicker->Delete();           this->m_pCellPicker          = 0;
	this->m_pLineCellPicker->Delete();       this->m_pLineCellPicker      = 0;	

	this->HandleProperty->Delete();          this->HandleProperty         = 0;
	this->SelectedHandleProperty->Delete();  this->SelectedHandleProperty = 0;
	this->EnabledHandleProperty->Delete();   this->EnabledHandleProperty  = 0;

	if (this->m_pCursor3D)
	{
		this->m_pCursor3D->Delete();
		this->m_pCursor3D = 0;
	}
	if (this->m_pCursor3DMapper)
	{
		this->m_pCursor3DMapper->Delete();
		this->m_pCursor3DMapper = 0;
	}
	if (this->m_pCursor3DActor)
	{
		this->m_pCursor3DActor->Delete();
		this->m_pCursor3DActor = 0;
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
}

vtkIdType CRiverActor::InsertNextPoint(double x, double y, double z)
{
	// only insert point if different enough from last point
	// otherwise an error will occur in vtkTubeFilter
	//
	//vtkIdType np = this->m_pPoints->GetNumberOfPoints();
	if (vtkIdType np = this->m_pPoints->GetNumberOfPoints())
	{
		vtkFloatingPointType p[3];
		vtkFloatingPointType pNext[3];
		vtkFloatingPointType sNext[3];
		this->m_pPoints->GetPoint(np - 1, p);
		pNext[0] = x, pNext[1] = y, pNext[2] = z;
		for (int i = 0; i < 3; ++i) 
		{
			sNext[i] = pNext[i] - p[i];
		}
		if (vtkMath::Normalize(sNext) == 0.0)
		{
			TRACE("Coincident points in CRiverActor...can't compute normals\n");
			///return np;
			return -1;
		}
	}

	vtkIdType id = this->m_pPoints->InsertNextPoint(x, y, z);

	vtkSphereSource *pSphereSource = vtkSphereSource::New();
	pSphereSource->SetPhiResolution(10);
	pSphereSource->SetThetaResolution(10);
	pSphereSource->SetRadius(this->m_Radius);

	double pt[3];
	pt[0] = x; pt[1] = y; pt[2] = z;
	this->m_pTransformScale->TransformPoint(pt, pt);
	this->m_pTransformUnits->TransformPoint(pt, pt);
	pSphereSource->SetCenter(pt[0], pt[1], pt[2]);

	vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
	pPolyDataMapper->SetInput(pSphereSource->GetOutput());

	vtkActor *pActor = vtkActor::New();
#ifdef __CPPUNIT__
	if (id == 0)
	{
		// pActor->SetDebug(1);
	}
#endif

	pActor->SetMapper(pPolyDataMapper);

	if (this->Enabled)
	{
		pActor->SetProperty(this->EnabledHandleProperty);
	}
	else
	{
		pActor->SetProperty(this->HandleProperty);
	}	

    this->m_pCellPicker->AddPickList(pActor);
	this->AddPart(pActor);

	if (this->m_pPoints->GetNumberOfPoints() > 1)
	{
		ASSERT(id > 0);
		double prev_pt[3];
		this->m_pPoints->GetPoint(id - 1, prev_pt);

		vtkLineSource *pLineSource = vtkLineSource::New();
		vtkTubeFilter *pTubeFilter = vtkTubeFilter::New();
		pTubeFilter->SetNumberOfSides(8);
		pTubeFilter->SetInput(pLineSource->GetOutput());
		pTubeFilter->SetRadius(this->m_Radius / 2.0);
		vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
		pPolyDataMapper->SetInput(pTubeFilter->GetOutput());
		vtkActor *pActor = vtkActor::New();
		pActor->SetMapper(pPolyDataMapper);
		pActor->GetProperty()->SetColor(0., 1., 1.);

		this->m_pLineCellPicker->AddPickList(pActor);
		this->AddPart(pActor);

		this->m_listLineSource.push_back(pLineSource);
		this->m_listTubeFilter.push_back(pTubeFilter);
		this->m_listLinePolyDataMapper.push_back(pPolyDataMapper);
		this->m_listLineActor.push_back(pActor);

		this->m_pTransformScale->TransformPoint(prev_pt, prev_pt);
		this->m_pTransformUnits->TransformPoint(prev_pt, prev_pt);
		//{{
		ASSERT( !(((prev_pt[0] - pt[0]) == 0.0) && ((prev_pt[1] - pt[1]) == 0.0)) );
		//}}
		pLineSource->SetPoint1(prev_pt[0], prev_pt[1], prev_pt[2]);
		pLineSource->SetPoint2(pt[0], pt[1], pt[2]);
	}

	this->m_listSphereSource.push_back(pSphereSource);
	this->m_listPolyDataMapper.push_back(pPolyDataMapper);
	this->m_listActor.push_back(pActor);

	return id;
}

void CRiverActor::InsertPoint(vtkIdType id, double x, double y, double z)
{
	ASSERT(FALSE); // sync with other InsertPoint

	this->m_pPoints->InsertPoint(id, x, y, z);

	vtkSphereSource *pSphereSource = vtkSphereSource::New();
	pSphereSource->SetPhiResolution(10);
	pSphereSource->SetThetaResolution(10);
	pSphereSource->SetRadius(this->m_Radius);

	double pt[3];
	pt[0] = x; pt[1] = y; pt[2] = z;
	this->m_pTransformScale->TransformPoint(pt, pt);
	this->m_pTransformUnits->TransformPoint(pt, pt);
	pSphereSource->SetCenter(pt[0], pt[1], pt[2]);

	vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
	pPolyDataMapper->SetInput(pSphereSource->GetOutput());

	vtkActor *pActor = vtkActor::New();
	pActor->SetMapper(pPolyDataMapper);
	pActor->SetProperty(this->HandleProperty);

    this->m_pCellPicker->AddPickList(pActor);
	this->AddPart(pActor);

	if (this->m_pPoints->GetNumberOfPoints() > 1)
	{
		vtkLineSource *pLineSource = vtkLineSource::New();
		vtkTubeFilter *pTubeFilter = vtkTubeFilter::New();
		pTubeFilter->SetNumberOfSides(8);
		pTubeFilter->SetInput(pLineSource->GetOutput());
		pTubeFilter->SetRadius(this->m_Radius / 2.0);
		vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
		pPolyDataMapper->SetInput(pTubeFilter->GetOutput());
		vtkActor *pActor = vtkActor::New();
		pActor->SetMapper(pPolyDataMapper);
		this->AddPart(pActor);

		this->m_listLineSource.push_back(pLineSource);
		this->m_listTubeFilter.push_back(pTubeFilter);
		this->m_listLinePolyDataMapper.push_back(pPolyDataMapper);
		this->m_listLineActor.push_back(pActor);

		double prev_pt[3];
		this->m_pPoints->GetPoint(id - 1, prev_pt);
		this->m_pTransformScale->TransformPoint(prev_pt, prev_pt);
		this->m_pTransformUnits->TransformPoint(prev_pt, prev_pt);
		//{{
		ASSERT( !(((prev_pt[0] - pt[0]) == 0.0) && ((prev_pt[1] - pt[1]) == 0.0)) );
		//}}
		pLineSource->SetPoint1(prev_pt[0], prev_pt[1], prev_pt[2]);
		pLineSource->SetPoint2(pt[0], pt[1], pt[2]);
	}

	this->m_listSphereSource.push_back(pSphereSource);
	this->m_listPolyDataMapper.push_back(pPolyDataMapper);
	this->m_listActor.push_back(pActor);
}

void CRiverActor::SetUnits(const CUnits &units)
{
	this->m_pTransformUnits->Identity();
	this->m_pTransformUnits->Scale(units.horizontal.input_to_si, units.horizontal.input_to_si, units.vertical.input_to_si);
	this->UpdatePoints();
}

void CRiverActor::SetScale(float x, float y, float z)
{
	this->SetScale(double(x), double(y), double(z));
}

void CRiverActor::SetZ(double z)
{
	this->m_Z = z;

	double pt[3];
	vtkIdType np = this->m_pPoints->GetNumberOfPoints();
	for (vtkIdType i = 0; i < np; ++i)
	{
		this->m_pPoints->GetPoint(i, pt);
		pt[2] = this->m_Z;
		this->m_pPoints->SetPoint(i, pt);
	}
	this->UpdatePoints();
}

double CRiverActor::GetZ(void)const
{
	return this->m_Z;
}

void CRiverActor::SetScale(double x, double y, double z)
{
	this->m_pTransformScale->Identity();
	this->m_pTransformScale->Scale(x, y, z);
	this->UpdatePoints();
}

void CRiverActor::UpdatePoints(void)
{
	double pt[3]; double prev_pt[3];
	vtkIdType np = this->m_pPoints->GetNumberOfPoints();
	ASSERT(np == this->m_listSphereSource.size());
	ASSERT(np == this->m_listActor.size());
	ASSERT(np == this->m_listPolyDataMapper.size());

	ASSERT(np - 1 == this->m_listLineSource.size() || this->m_listLineSource.size() == 0);

	std::list<vtkSphereSource*>::iterator iterSphereSource = this->m_listSphereSource.begin();
	std::list<vtkLineSource*>::iterator iterLineSource = this->m_listLineSource.begin();
	for (vtkIdType i = 0; i < np; ++i, ++iterSphereSource)
	{
		this->m_pPoints->GetPoint(i, pt);
		this->m_pTransformScale->TransformPoint(pt, pt);
		this->m_pTransformUnits->TransformPoint(pt, pt);

		if (iterSphereSource != this->m_listSphereSource.end())
		{
			(*iterSphereSource)->SetCenter(pt[0], pt[1], pt[2]);
		}
		if (i > 0)
		{
			if (iterLineSource != this->m_listLineSource.end())
			{
				(*iterLineSource)->SetPoint1(prev_pt[0], prev_pt[1], prev_pt[2]);
				(*iterLineSource)->SetPoint2(pt[0], pt[1], pt[2]);
				++iterLineSource;
			}
		}
		prev_pt[0] = pt[0];   prev_pt[1] = pt[1];   prev_pt[2] = pt[2];
	}
}

void CRiverActor::ClearPoints(void)
{
	this->m_pPoints->Initialize();
	ASSERT(this->m_pPoints->GetNumberOfPoints() == 0);

	std::list<vtkSphereSource*>::iterator iterSphereSource = this->m_listSphereSource.begin();
	for(; iterSphereSource != this->m_listSphereSource.end(); ++iterSphereSource)
	{
		(*iterSphereSource)->Delete();
	}
	this->m_listSphereSource.clear();

	std::list<vtkPolyDataMapper*>::iterator iterPolyDataMapper = this->m_listPolyDataMapper.begin();
	for(; iterPolyDataMapper != this->m_listPolyDataMapper.end(); ++iterPolyDataMapper)
	{
		(*iterPolyDataMapper)->Delete();
	}
	this->m_listPolyDataMapper.clear();

	std::list<vtkActor*>::iterator iterActor = this->m_listActor.begin();
	for(; iterActor != this->m_listActor.end(); ++iterActor)
	{
		this->RemovePart(*iterActor);
		this->m_pCellPicker->DeletePickList(*iterActor);
		(*iterActor)->Delete();
	}
	this->m_listActor.clear();

	std::list<vtkLineSource*>::iterator iterLineSource = this->m_listLineSource.begin();
	for(; iterLineSource != this->m_listLineSource.end(); ++iterLineSource)
	{
		(*iterLineSource)->Delete();
	}
	this->m_listLineSource.clear();

	std::list<vtkTubeFilter*>::iterator iterTubeFilter = this->m_listTubeFilter.begin();
	for(; iterTubeFilter != this->m_listTubeFilter.end(); ++iterTubeFilter)
	{
		(*iterTubeFilter)->Delete();
	}
	this->m_listTubeFilter.clear();

	std::list<vtkPolyDataMapper*>::iterator iterLinePolyDataMapper = this->m_listLinePolyDataMapper.begin();
	for(; iterLinePolyDataMapper != this->m_listLinePolyDataMapper.end(); ++iterLinePolyDataMapper)
	{
		(*iterLinePolyDataMapper)->Delete();
	}
	this->m_listLinePolyDataMapper.clear();

	std::list<vtkActor*>::iterator iterLineActor = this->m_listLineActor.begin();
	for(; iterLineActor != this->m_listLineActor.end(); ++iterLineActor)
	{
		this->RemovePart(*iterLineActor);
		this->m_pLineCellPicker->DeletePickList(*iterLineActor);
		(*iterLineActor)->Delete();
	}
	this->m_listLineActor.clear();
}

void CRiverActor::SetRiver(const CRiver &river, const CUnits &units)
{
	this->m_river = river;

	char buffer[40];
	::sprintf(buffer, "River %d", this->m_river.n_user);
	this->m_serialName = buffer;

	this->ClearPoints();
	std::list<CRiverPoint>::iterator iter = this->m_river.m_listPoints.begin();
	for (; iter != this->m_river.m_listPoints.end(); ++iter)
	{
		this->InsertNextPoint((*iter).x, (*iter).y, (*iter).z);
	}
	this->SetUnits(units);
}

CRiver CRiverActor::GetRiver(void)const
{
	return this->m_river;
}

vtkFloatingPointType CRiverActor::GetRadius(void)const
{
	return this->m_Radius;
}

void CRiverActor::SetRadius(vtkFloatingPointType radius)
{
	this->m_Radius = radius;
	std::list<vtkSphereSource*>::iterator iterSphereSource = this->m_listSphereSource.begin();
	for (; iterSphereSource != this->m_listSphereSource.end(); ++iterSphereSource)
	{
		(*iterSphereSource)->SetRadius(this->m_Radius);
	}

	std::list<vtkTubeFilter*>::iterator iterTubeFilter = this->m_listTubeFilter.begin();
	for (; iterTubeFilter != this->m_listTubeFilter.end(); ++iterTubeFilter)
	{
		(*iterTubeFilter)->SetRadius(this->m_Radius / 2.0);
	}
}

void CRiverActor::SetInteractor(vtkRenderWindowInteractor* i)
{
	if (i == this->Interactor)
	{
		return;
	}

	// if we already have an Interactor then stop observing it
	if (this->Interactor)
	{
		this->Interactor->UnRegister(this);
		this->Interactor->RemoveObserver(this->EventCallbackCommand);
	}

	this->Interactor = i;
	this->Interactor->Register(this);

	if (i)
	{
		this->SetEnabled(1);
	}

// COMMENT: {6/13/2005 3:54:53 PM}	if (i)
// COMMENT: {6/13/2005 3:54:53 PM}	{
// COMMENT: {6/13/2005 3:54:53 PM}		i->AddObserver(vtkCommand::LeftButtonPressEvent, 
// COMMENT: {6/13/2005 3:54:53 PM}			this->EventCallbackCommand, 
// COMMENT: {6/13/2005 3:54:53 PM}			1);
// COMMENT: {6/13/2005 3:54:53 PM}
// COMMENT: {6/13/2005 3:54:53 PM}		i->AddObserver(vtkCommand::LeftButtonReleaseEvent, 
// COMMENT: {6/13/2005 3:54:53 PM}			this->EventCallbackCommand, 
// COMMENT: {6/13/2005 3:54:53 PM}			1);
// COMMENT: {6/13/2005 3:54:53 PM}
// COMMENT: {6/13/2005 3:54:53 PM}		i->AddObserver(vtkCommand::MouseMoveEvent, 
// COMMENT: {6/13/2005 3:54:53 PM}			this->EventCallbackCommand, 
// COMMENT: {6/13/2005 3:54:53 PM}			1);
// COMMENT: {6/13/2005 3:54:53 PM}	}
	this->Modified();
}

//----------------------------------------------------------------------------
void CRiverActor::ProcessEvents(vtkObject* vtkNotUsed(object), 
									   unsigned long event,
									   void* clientdata, 
									   void* vtkNotUsed(calldata))
{
	CRiverActor* self 
		= reinterpret_cast<CRiverActor *>( clientdata );

	switch(event)
	{
	case vtkCommand::ExposeEvent:
		break;

	case vtkCommand::ConfigureEvent:
		break;

	case vtkCommand::EnterEvent:  
		TRACE("EnterEvent\n");
		break;

	case vtkCommand::LeaveEvent:
		TRACE("LeaveEvent\n");
		break;

	case vtkCommand::TimerEvent: 
		break;

	case vtkCommand::MouseMoveEvent:
		self->OnMouseMove();
		break;

	case vtkCommand::LeftButtonPressEvent: 
		self->OnLeftButtonDown();
		break;

	case vtkCommand::LeftButtonReleaseEvent:
		self->OnLeftButtonUp();
		break;

	case vtkCommand::MiddleButtonPressEvent:
		break;

	case vtkCommand::MiddleButtonReleaseEvent:
		break;

	case vtkCommand::RightButtonPressEvent:
		break;

	case vtkCommand::RightButtonReleaseEvent: 
		break;

	case vtkCommand::KeyPressEvent:
		self->OnKeyPress();
		break;

	case vtkCommand::KeyReleaseEvent: 
		break;

	case vtkCommand::CharEvent:  
		break;

	case vtkCommand::DeleteEvent:
		break;
	}
}

void CRiverActor::OnLeftButtonDown()
{
	if (!this->Interactor) return;

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if ( ren != this->CurrentRenderer )
	{
		return;
	}

	if (this->State == CRiverActor::None)
	{
		vtkAssemblyPath *path;
		this->m_pCellPicker->Pick(X, Y, 0.0, ren);
		path = this->m_pCellPicker->GetPath();
		if (path != NULL)
		{
			if (vtkActor* pActor = vtkActor::SafeDownCast(path->GetFirstNode()->GetProp()))
			{
				this->HighlightHandle(path->GetFirstNode()->GetProp());
				this->m_pPoints->GetPoint(this->GetCurrentPointId(), this->m_WorldPointXYPlane);
				this->EventCallbackCommand->SetAbortFlag(1);
				this->State = CRiverActor::MovingPoint;
				this->InvokeEvent(CRiverActor::StartMovePointEvent, NULL);
				this->Interactor->Render();
				return;
			}
		}

		//{{
		this->m_pLineCellPicker->Pick(X, Y, 0.0, ren);
		path = this->m_pLineCellPicker->GetPath();
		if (path != NULL)
		{
			if (vtkActor* pActor = vtkActor::SafeDownCast(path->GetFirstNode()->GetProp()))
			{
				// this->m_pPoints->GetPoint(i, pt);
				std::list<vtkActor*>::iterator iterActor = this->m_listLineActor.begin();
				std::list<vtkLineSource*>::iterator iterSource = this->m_listLineSource.begin();
				for (vtkIdType i = 0; iterActor != this->m_listLineActor.end(); ++i, ++iterActor, ++iterSource)
				{
					if (pActor == *iterActor)
					{
						this->Update();

						vtkIdType before = this->m_pPoints->GetNumberOfPoints();
						/////////this->m_pPoints->InsertNextPoint(this->m_WorldPointXYPlane);
						//{{
						double prev[3];
						vtkIdType count = this->m_pPoints->GetNumberOfPoints();
						for (vtkIdType j = count; j > i; --j)
						{
							this->m_pPoints->GetPoint(j - 1, prev);
							this->m_pPoints->InsertPoint(j, prev);
						}
						this->m_pPoints->InsertPoint(i + 1, this->m_WorldPointXYPlane);
						//}}
						vtkIdType after = this->m_pPoints->GetNumberOfPoints();

						vtkSphereSource *pSphereSource = vtkSphereSource::New();
						pSphereSource->SetPhiResolution(10);
						pSphereSource->SetThetaResolution(10);
						pSphereSource->SetRadius(this->m_Radius);

						vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
						pPolyDataMapper->SetInput(pSphereSource->GetOutput());

						vtkActor *pActor = vtkActor::New();
						pActor->SetMapper(pPolyDataMapper);

						ASSERT(this->Enabled);
						//if (this->Enabled)
						{
							pActor->SetProperty(this->EnabledHandleProperty);
						}
						//else
						{
							pActor->SetProperty(this->HandleProperty);
						}	

						this->m_pCellPicker->AddPickList(pActor);
						this->AddPart(pActor);

						{
							vtkLineSource *pLineSource = vtkLineSource::New();
							vtkTubeFilter *pTubeFilter = vtkTubeFilter::New();
							pTubeFilter->SetNumberOfSides(8);
							pTubeFilter->SetInput(pLineSource->GetOutput());
							pTubeFilter->SetRadius(this->m_Radius / 2.0);
							vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
							pPolyDataMapper->SetInput(pTubeFilter->GetOutput());
							vtkActor *pActor = vtkActor::New();
							pActor->SetMapper(pPolyDataMapper);
							pActor->GetProperty()->SetColor(0., 1., 1.);

							this->m_pLineCellPicker->AddPickList(pActor);
							this->AddPart(pActor);

							this->m_listLineSource.push_back(pLineSource);
							this->m_listTubeFilter.push_back(pTubeFilter);
							this->m_listLinePolyDataMapper.push_back(pPolyDataMapper);
							this->m_listLineActor.push_back(pActor);

							///////this->m_pTransformScale->TransformPoint(prev_pt, prev_pt);
							///////this->m_pTransformUnits->TransformPoint(prev_pt, prev_pt);
							//{{
							///////ASSERT( !(((prev_pt[0] - pt[0]) == 0.0) && ((prev_pt[1] - pt[1]) == 0.0)) );
							//}}
							///////pLineSource->SetPoint1(prev_pt[0], prev_pt[1], prev_pt[2]);
							///////pLineSource->SetPoint2(pt[0], pt[1], pt[2]);
						}

						this->m_listSphereSource.push_back(pSphereSource);
						this->m_listPolyDataMapper.push_back(pPolyDataMapper);
						this->m_listActor.push_back(pActor);
						//}}
						this->UpdatePoints();
						this->Interactor->Render();
						break;
					}
				}
				///::AfxMessageBox("CRiverActor::OnLeftButtonDown on line actor");
				this->EventCallbackCommand->SetAbortFlag(1);
			}
		}
		//}}
	}

	if (this->State == CRiverActor::CreatingRiver)
	{
		this->Update();
		this->m_pCursor3DActor->SetPosition(this->WorldSIPoint[0], this->WorldSIPoint[1], this->WorldSIPoint[2]);
		this->Interactor->Render();
	}
}

void CRiverActor::OnMouseMove()
{
	if (!this->Interactor) return;

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if ( ren != this->CurrentRenderer )
	{
		return;
	}

	//if (this->CurrentHandle && this->CurrentSource)
	if (this->State == CRiverActor::MovingPoint)
	{
		// update m_WorldPointXYPlane
		//
		this->Update();
		this->m_pPoints->SetPoint(this->CurrentId, this->m_WorldPointXYPlane);
		this->UpdatePoints();

		this->EventCallbackCommand->SetAbortFlag(1);
		this->InvokeEvent(CRiverActor::MovingPointEvent, NULL);
		this->Interactor->Render();
	}

	if (this->State == CRiverActor::CreatingRiver)
	{
		this->Update();
		this->m_pCursor3DActor->SetPosition(this->WorldSIPoint[0], this->WorldSIPoint[1], this->WorldSIPoint[2]);
		this->m_pCursor3DActor->VisibilityOn();
		if (this->m_pPoints->GetNumberOfPoints())
		{
			this->ConnectingLineSource->SetPoint2(this->WorldSIPoint[0], this->WorldSIPoint[1], this->WorldSIPoint[2]);
			this->ConnectingActor->VisibilityOn();
		}
		this->EventCallbackCommand->SetAbortFlag(1);
		this->Interactor->Render();
	}
}

void CRiverActor::OnLeftButtonUp()
{
	if (!this->Interactor) return;

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if ( ren != this->CurrentRenderer )
	{
		return;
	}

// COMMENT: {6/14/2005 2:38:22 PM}	vtkAssemblyPath *path;
// COMMENT: {6/14/2005 2:38:22 PM}	this->m_pCellPicker->Pick(X, Y, 0.0, ren);
// COMMENT: {6/14/2005 2:38:22 PM}	path = this->m_pCellPicker->GetPath();
// COMMENT: {6/14/2005 2:38:22 PM}	if (path != NULL)
// COMMENT: {6/14/2005 2:38:22 PM}	{
// COMMENT: {6/14/2005 2:38:22 PM}		if (vtkActor* pActor = vtkActor::SafeDownCast(path->GetFirstNode()->GetProp()))
// COMMENT: {6/14/2005 2:38:22 PM}		{
// COMMENT: {6/14/2005 2:38:22 PM}			this->HighlightHandle(NULL);
// COMMENT: {6/14/2005 2:38:22 PM}			this->EventCallbackCommand->SetAbortFlag(1);
// COMMENT: {6/14/2005 2:38:22 PM}			this->InvokeEvent(CRiverActor::EndMovePointEvent, NULL);
// COMMENT: {6/14/2005 2:38:22 PM}			this->Interactor->Render();
// COMMENT: {6/14/2005 2:38:22 PM}		}
// COMMENT: {6/14/2005 2:38:22 PM}	}

	// if (this->CurrentHandle && this->CurrentSource)
	if (this->State == CRiverActor::MovingPoint)
	{
		// update m_WorldPointXYPlane
		//
		this->Update();
		this->m_pPoints->SetPoint(this->CurrentId, this->m_WorldPointXYPlane);
		UpdatePoints();

		this->HighlightHandle(NULL);
		this->EventCallbackCommand->SetAbortFlag(1);
		this->State = CRiverActor::None;
		this->InvokeEvent(CRiverActor::EndMovePointEvent, NULL);
		this->Interactor->Render();
	}

	if (this->State == CRiverActor::CreatingRiver)
	{
		this->Update();
		if (this->InsertNextPoint(this->m_WorldPointXYPlane[0], this->m_WorldPointXYPlane[1], this->m_WorldPointXYPlane[2]) != -1)
		{
			//{{
			CRiverPoint rivpt;
			rivpt.x = this->m_WorldPointXYPlane[0]; rivpt.x_defined = TRUE;
			rivpt.y = this->m_WorldPointXYPlane[1]; rivpt.y_defined = TRUE;
			this->m_river.m_listPoints.push_back(rivpt);
			//}}
			this->UpdatePoints();
			this->ConnectingLineSource->SetPoint1(this->WorldSIPoint[0], this->WorldSIPoint[1], this->WorldSIPoint[2]);
			this->ConnectingLineSource->SetPoint2(this->WorldSIPoint[0], this->WorldSIPoint[1], this->WorldSIPoint[2]);
			this->Interactor->Render();
		}
	}
}

void CRiverActor::OnKeyPress()
{
	if (!this->Interactor) return;

	if (this->State == CRiverActor::CreatingRiver)
	{
		char* keysym = this->Interactor->GetKeySym();		
		// if (::strcmp(keysym, "Escape") == 0)
		{
			this->EndNewRiver();
		}
		this->Interactor->Render();
	}
}

int CRiverActor::HighlightHandle(vtkProp *prop)
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
			this->CurrentHandle->SetProperty(this->HandleProperty);
		}
	}

	this->CurrentHandle = (vtkActor *)prop;

	if (this->CurrentHandle)
	{
		this->CurrentHandle->SetProperty(this->SelectedHandleProperty);
		std::list<vtkActor*>::iterator iterActor = this->m_listActor.begin();
		std::list<vtkSphereSource*>::iterator iterSource = this->m_listSphereSource.begin();
		for (vtkIdType i = 0; iterActor != this->m_listActor.end(); ++i, ++iterActor, ++iterSource)
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

void CRiverActor::CreateDefaultProperties()
{
  // Handle properties
  this->HandleProperty = vtkProperty::New();
  this->HandleProperty->SetColor(0, 0, 1);

  this->SelectedHandleProperty = vtkProperty::New();
  this->SelectedHandleProperty->SetColor(1, 0, 0);

  this->EnabledHandleProperty = vtkProperty::New();
  this->EnabledHandleProperty->SetColor(1, 1, 1);
}

void CRiverActor::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	this->m_pPoints->PrintSelf(os, indent.GetNextIndent());
}

void CRiverActor::SetEnabled(int enabling)
{
	if (!this->Interactor)
	{
		vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
		return;
	}

	if ( enabling ) //------------------------------------------------------------
	{
		vtkDebugMacro(<<"Enabling river");

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

		std::list<vtkActor*>::iterator iterActor = this->m_listActor.begin();
		for(; iterActor != this->m_listActor.end(); ++iterActor)
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

		std::list<vtkActor*>::iterator iterActor = this->m_listActor.begin();
		for(; iterActor != this->m_listActor.end(); ++iterActor)
		{
			(*iterActor)->SetProperty(this->HandleProperty);
		}
	}

	this->Interactor->Render();
}

void CRiverActor::Update()
{
	if (!this->CurrentRenderer) return;

	int i;

	// get the position of the mouse cursor in screen/window coordinates
	// (pixel)
	vtkRenderer *renderer = this->CurrentRenderer;
	int* pos = this->Interactor->GetEventPosition();

	// get the focal point in world coordinates
	//
	vtkCamera *camera = renderer->GetActiveCamera();	
	vtkFloatingPointType cameraFP[4];
	camera->GetFocalPoint((vtkFloatingPointType*)cameraFP); cameraFP[3] = 1.0;

	// Convert the focal point coordinates to display (or screen) coordinates.
	//
	renderer->SetWorldPoint(cameraFP);
	renderer->WorldToDisplay();
	vtkFloatingPointType *displayCoords = renderer->GetDisplayPoint();

	// Convert the selection point into world coordinates.
	//
	renderer->SetDisplayPoint(pos[0], pos[1], displayCoords[2]);
	renderer->DisplayToWorld();
	vtkFloatingPointType *worldCoords = renderer->GetWorldPoint();
	if ( worldCoords[3] == 0.0 )
	{
		ASSERT(FALSE);
		return;
	}
	vtkFloatingPointType PickPosition[3];
	for (i = 0; i < 3; ++i)
	{
		PickPosition[i] = worldCoords[i] / worldCoords[3];
	}

	double pt[3];
// COMMENT: {6/21/2005 7:11:19 PM}	double zOrig = this->m_Z;
	pt[2] = this->m_Z;

	this->m_pTransformScale->TransformPoint(pt, pt);
	this->m_pTransformUnits->TransformPoint(pt, pt);

	double zPos = pt[2];

	if ( camera->GetParallelProjection() )
	{
		double* cameraDOP = camera->GetDirectionOfProjection();
		double t = (zPos - PickPosition[2]) / cameraDOP[2];
		for (i = 0; i < 2; ++i)
		{
			this->m_WorldPointXYPlane[i] = PickPosition[i] + t * cameraDOP[i];
			this->WorldSIPoint[i] = PickPosition[i] + t * cameraDOP[i];
		}
	}
	else
	{
		double *cameraPos = camera->GetPosition();
		double t = (zPos - cameraPos[2]) / (PickPosition[2] - cameraPos[2]);
		for (i = 0; i < 2; ++i)
		{
			this->m_WorldPointXYPlane[i] = cameraPos[i] + t * (PickPosition[i] - cameraPos[i]);
			this->WorldSIPoint[i] = cameraPos[i] + t * (PickPosition[i] - cameraPos[i]);
		}
	}
// COMMENT: {6/13/2005 7:49:38 PM}	this->m_WorldPointXYPlane[2] = zPos;

	// UN-SCALE
	//
	this->m_pTransformScale->GetInverse()->TransformPoint(this->m_WorldPointXYPlane, this->m_WorldPointXYPlane);
	this->m_pTransformUnits->GetInverse()->TransformPoint(this->m_WorldPointXYPlane, this->m_WorldPointXYPlane);

	this->m_pTransformScale->GetInverse()->TransformPoint(this->WorldSIPoint, this->WorldScaledUnitPoint);
	this->m_pTransformUnits->GetInverse()->TransformPoint(this->WorldSIPoint, this->WorldScaledUnitPoint);

// COMMENT: {6/21/2005 7:11:23 PM}	this->m_WorldPointXYPlane[2] = zOrig;
	this->m_WorldPointXYPlane[2] = this->m_Z;
	this->WorldSIPoint[2] = pt[2];
}

void CRiverActor::Add(CPropertyTreeControlBar *pTree)
{
	CTreeCtrlNode node = pTree->GetRiversNode();
	this->m_node = node.InsertAfter(_T(""), TVI_LAST);
	this->Update(this->m_node);
	this->m_node.SetData((DWORD_PTR)this);
	if (this->GetVisibility())
	{
		this->m_node.SetState(INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	}
	else
	{
		this->m_node.SetState(INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1), TVIS_STATEIMAGEMASK);
	}
	VERIFY(this->m_node.Select());
}

void CRiverActor::UnAdd(CPropertyTreeControlBar *pTree)
{
	if (this->m_node)
	{
		CTreeCtrlNode node = this->m_node.GetParent();
		node.Select();
		VERIFY(this->m_node.Delete());
	}
}

void CRiverActor::Remove(CPropertyTreeControlBar *pTree)
{
	ASSERT(FALSE); // TODO
}

void CRiverActor::UnRemove(CPropertyTreeControlBar *pTree)
{
	ASSERT(FALSE); // TODO
}

void CRiverActor::Update(CTreeCtrlNode node)
{
	// delay the refresh
	//
	CDelayRedraw redraw(node.GetWnd());

	// store expanded states
	bool bMainExpanded = false;
	if (node.HasChildren())
	{
		bMainExpanded = ((node.GetState(TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
	}

	// remove all previous items
	//
	while (node.HasChildren())
	{
		node.GetChild().Delete();
	}

	CString strItem;
	if (this->m_river.description.empty())
	{
		strItem.Format("River %d", this->m_river.n_user);
	}
	else
	{
		strItem.Format("River %d %s", this->m_river.n_user, this->m_river.description.c_str());
	}
	node.SetText(strItem);

	std::list<CRiverPoint>::const_iterator it = this->m_river.m_listPoints.begin();
	for (; it != this->m_river.m_listPoints.end(); ++it)
	{
		strItem.Format("point %g   %g", it->x, it->y);
		CTreeCtrlNode pointBranch = node.AddTail(strItem);
		if (pointBranch)
		{
			if (!it->m_riverSchedule.empty())
			{
				CTimeSeries<CRiverState>::const_iterator iterState = it->m_riverSchedule.begin();
				CTreeCtrlNode headNode;
				CTreeCtrlNode solutionNode;
				for (; iterState != it->m_riverSchedule.end(); ++iterState)
				{
					CString strTime;
					Ctime time(iterState->first);
					CRiverState rate(iterState->second);

					strTime.Format("%g", time.value);
					if (time.type == UNITS)
					{
						strTime += " ";
						strTime += time.input;
					}

					if (rate.head_defined)
					{
						if (!headNode)
						{
							headNode = pointBranch.AddHead("head");
						}
						strItem.Format(" %g", rate.head);
						headNode.AddHead(strTime + strItem);
					}

					if (rate.solution_defined)
					{
						if (!solutionNode)
						{
							solutionNode = pointBranch.AddHead("solution");
						}
						strItem.Format(" %d", rate.solution);
						solutionNode.AddHead(strTime + strItem);
					}
				}
			}

			if (it->width_defined)
			{
				strItem.Format("width %g", it->width);
                pointBranch.AddHead(strItem);
			}
			if (it->k_defined)
			{
				strItem.Format("bed_hydraulic_conductivity %g", it->k);
                pointBranch.AddHead(strItem);
			}
			if (it->thickness_defined)
			{
				strItem.Format("bed_thickness %g", it->thickness);
                pointBranch.AddHead(strItem);
			}
			if (it->depth_defined)
			{
				strItem.Format("depth %g", it->depth);
                pointBranch.AddHead(strItem);
			}
		}
	}

	if (bMainExpanded)
	{
		node.Expand(TVE_EXPAND);
	}
}

vtkActor* CRiverActor::GetPoint(int index)
{
	std::list<vtkActor*>::iterator iterActor = this->m_listActor.begin();
	for (int i = 0; iterActor != this->m_listActor.end(); ++i, ++iterActor)
	{
		if (i == index)
		{
			return *iterActor;
		}
	}
	ASSERT(FALSE);
	return NULL;
}

void CRiverActor::SelectPoint(int index)
{
	this->HighlightHandle(NULL);
	this->HighlightHandle(this->GetPoint(index));
	this->CurrentSource = NULL;
	if (this->Interactor)
	{
		this->Interactor->Render();
	}
}

void CRiverActor::ClearSelection(void)
{
	this->HighlightHandle(NULL);
}

vtkIdType CRiverActor::GetCurrentPointId(void)const
{
	return this->CurrentId;
}

double* CRiverActor::GetCurrentPointPosition(void)
{
	return this->m_WorldPointXYPlane;
}

size_t CRiverActor::GetPointCount(void)const
{
	return this->m_listActor.size();
}

void CRiverActor::GetCurrentPointPosition(double x[3])const
{
	x[0] = this->m_WorldPointXYPlane[0];
	x[1] = this->m_WorldPointXYPlane[1];
	x[2] = this->m_WorldPointXYPlane[2];
}

void CRiverActor::MovePoint(vtkIdType id, double x, double y)
{
	if (this->m_pPoints)
	{
		double pt[3];
		this->m_pPoints->GetPoint(id, pt);
		pt[0] = x;
		pt[1] = y;
		this->m_pPoints->SetPoint(id, pt);
		this->UpdatePoints();
		if (this->Interactor)
		{
			this->Interactor->Render();
		}
	}

	std::list<CRiverPoint>::iterator iter = this->m_river.m_listPoints.begin();
	for (vtkIdType i = 0; iter != this->m_river.m_listPoints.end(); ++i, ++iter)
	{
		if (id == i)
		{
			(*iter).x = x;
			(*iter).y = y;
			break;
		}
	}
	this->Update(this->m_node);
}

LPCTSTR CRiverActor::GetSerialName(void)const
{
	return this->m_serialName.c_str();
}

void CRiverActor::Serialize(bool bStoring, hid_t loc_id, const CUnits &units)
{
	if (bStoring)
	{
		this->m_river.Serialize(bStoring, loc_id);
	}
	else
	{
		CRiver river;
		river.Serialize(bStoring, loc_id);
		this->SetRiver(river, units);
	}
}

void CRiverActor::SetVisibility(int visibility)
{
	vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Visibility to " << visibility);
	if (this->Visibility != visibility)
	{
		std::list<vtkActor*>::iterator it = this->m_listActor.begin();
		for (; it != this->m_listActor.end(); ++it)
		{
			(*it)->SetVisibility(visibility);
		}

		std::list<vtkActor*>::iterator itLine = this->m_listLineActor.begin();
		for (; itLine != this->m_listLineActor.end(); ++itLine)
		{
			(*itLine)->SetVisibility(visibility);
		}

		this->Visibility = visibility;
		this->Modified();
	}
}

int CRiverActor::GetVisibility(void)
{
	vtkDebugMacro(<< this->GetClassName() << " (" << this << "): returning Visibility of " << this->Visibility );
	return this->Visibility;
}

void CRiverActor::ScaleFromBounds(vtkFloatingPointType bounds[6])
{
	// set radius
	//
	vtkFloatingPointType defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
	this->SetRadius(defaultAxesSize * 0.085 /* / sqrt(scale[0] * scale[1]) */ );

	if (this->m_pCursor3D)
	{
		// set size of 3D cursor
		//
		vtkFloatingPointType dim = (bounds[1] - bounds[0]) / 20.0;
		this->m_pCursor3D->SetModelBounds(-dim, dim, -dim, dim, -dim, dim);
	}
}

CRiverActor* CRiverActor::StartNewRiver(vtkRenderWindowInteractor* pRenderWindowInteractor)
{
	CRiverActor* pRiverActor = CRiverActor::New();

	// 3D Cursor
	//
	pRiverActor->m_pCursor3D = vtkCursor3D::New();
	pRiverActor->m_pCursor3D->XShadowsOff();
	pRiverActor->m_pCursor3D->YShadowsOff();
	pRiverActor->m_pCursor3D->ZShadowsOff();
	pRiverActor->m_pCursor3D->OutlineOff();

	pRiverActor->m_pCursor3DMapper = vtkPolyDataMapper::New();
	pRiverActor->m_pCursor3DMapper->SetInput(pRiverActor->m_pCursor3D->GetOutput());
	
	pRiverActor->m_pCursor3DActor = vtkActor::New();
	pRiverActor->m_pCursor3DActor->SetMapper(pRiverActor->m_pCursor3DMapper);
	pRiverActor->m_pCursor3DActor->SetPosition(0, 0, 0);
	pRiverActor->m_pCursor3DActor->VisibilityOff();
	pRiverActor->m_pCursor3DActor->GetProperty()->SetColor(0, 0, 1);

	pRiverActor->AddPart(pRiverActor->m_pCursor3DActor);

	pRiverActor->SetInteractor(pRenderWindowInteractor);
	pRiverActor->SetEnabled(1);

	pRiverActor->State = CRiverActor::CreatingRiver;

	// instantiate the line connector
	//
	ASSERT(pRiverActor->ConnectingLineSource == 0);
	ASSERT(pRiverActor->ConnectingMapper == 0);
	ASSERT(pRiverActor->ConnectingActor == 0);

	pRiverActor->ConnectingLineSource = vtkLineSource::New();

	pRiverActor->ConnectingMapper = vtkPolyDataMapper::New();
	pRiverActor->ConnectingMapper->SetInput(pRiverActor->ConnectingLineSource->GetOutput());

	pRiverActor->ConnectingActor = vtkActor::New();
	pRiverActor->ConnectingActor->SetMapper(pRiverActor->ConnectingMapper);
	pRiverActor->ConnectingActor->GetProperty()->SetColor(0., 1., 1.);
	pRiverActor->ConnectingActor->VisibilityOff();

	pRiverActor->AddPart(pRiverActor->ConnectingActor);
	pRiverActor->InvokeEvent(CRiverActor::StartNewRiverEvent, NULL);

	return pRiverActor;
}

void CRiverActor::CancelNewRiver()
{
	if (this->State == CRiverActor::CreatingRiver)
	{
		ASSERT(this->m_pCursor3D != 0);
		ASSERT(this->m_pCursor3DMapper != 0);
		ASSERT(this->m_pCursor3DActor != 0);
		if (this->m_pCursor3DActor)
		{
			ASSERT(this->GetParts()->IsItemPresent(this->m_pCursor3DActor));
			this->RemovePart(this->m_pCursor3DActor);
			this->m_pCursor3DActor->Delete();
			this->m_pCursor3DActor = 0;
		}
		if (this->m_pCursor3DMapper)
		{
			this->m_pCursor3DMapper->Delete();
			this->m_pCursor3DMapper = 0;
		}
		if (this->m_pCursor3D)
		{
			this->m_pCursor3D->Delete();
			this->m_pCursor3D = 0;
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
		this->InvokeEvent(CRiverActor::CancelNewRiverEvent, NULL);
	}
	this->SetEnabled(0);
	this->State = CRiverActor::None;
}

void CRiverActor::EndNewRiver()
{
	if (this->State == CRiverActor::CreatingRiver)
	{
		ASSERT(this->m_pCursor3D != 0);
		ASSERT(this->m_pCursor3DMapper != 0);
		ASSERT(this->m_pCursor3DActor != 0);
		if (this->m_pCursor3DActor)
		{
			ASSERT(this->GetParts()->IsItemPresent(this->m_pCursor3DActor));
			this->RemovePart(this->m_pCursor3DActor);
			this->m_pCursor3DActor->Delete();
			this->m_pCursor3DActor = 0;
		}
		if (this->m_pCursor3DMapper)
		{
			this->m_pCursor3DMapper->Delete();
			this->m_pCursor3DMapper = 0;
		}
		if (this->m_pCursor3D)
		{
			this->m_pCursor3D->Delete();
			this->m_pCursor3D = 0;
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
		this->InvokeEvent(CRiverActor::EndNewRiverEvent, NULL);
	}
	this->SetEnabled(0);
	this->State = CRiverActor::None;
}
