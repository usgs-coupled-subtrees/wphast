#include "StdAfx.h"
#include "RiverActor.h"

#include <strstream>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkCellPicker.h>
#include "Units.h"

vtkCxxRevisionMacro(CRiverActor, "$Revision: 244 $");
vtkStandardNewMacro(CRiverActor);


CRiverActor::CRiverActor(void)
: m_fRadius(1.0)
, Interactor(0)
{
	this->m_pPoints         = vtkPoints::New();
	this->m_pTransformUnits = vtkTransform::New();
	this->m_pTransformScale = vtkTransform::New();
	this->m_pTransformUnits->Identity();
	this->m_pTransformScale->Identity();

	this->m_pCellPicker     = vtkCellPicker::New();
	this->m_pCellPicker->SetTolerance(0.001);
	this->m_pCellPicker->PickFromListOn();

	this->EventCallbackCommand = vtkCallbackCommand::New();
	this->EventCallbackCommand->SetClientData(this); 
	this->EventCallbackCommand->SetCallback(CRiverActor::ProcessEvents);
}

CRiverActor::~CRiverActor(void)
{
	this->ClearPoints();

	this->m_pPoints->Delete();
	this->m_pTransformUnits->Delete();
	this->m_pTransformScale->Delete();
	this->m_pCellPicker->Delete();
}

vtkIdType CRiverActor::InsertNextPoint(double x, double y, double z)
{
	vtkIdType id = this->m_pPoints->InsertNextPoint(x, y, z);

	vtkSphereSource *pSphereSource = vtkSphereSource::New();
	pSphereSource->SetPhiResolution(10);
	pSphereSource->SetThetaResolution(10);
	pSphereSource->SetRadius(this->m_fRadius);

	double pt[3];
	pt[0] = x; pt[1] = y; pt[2] = z;
	this->m_pTransformScale->TransformPoint(pt, pt);
	this->m_pTransformUnits->TransformPoint(pt, pt);
	pSphereSource->SetCenter(pt[0], pt[1], pt[2]);

	vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
	pPolyDataMapper->SetInput(pSphereSource->GetOutput());

	vtkActor *pActor = vtkActor::New();
	pActor->SetMapper(pPolyDataMapper);
	pActor->GetProperty()->SetColor(0., 0., 1.);

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
		pTubeFilter->SetRadius(this->m_fRadius / 2.0);
		vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
		pPolyDataMapper->SetInput(pTubeFilter->GetOutput());
		vtkActor *pActor = vtkActor::New();
		pActor->SetMapper(pPolyDataMapper);
		pActor->GetProperty()->SetColor(0., 1., 1.);
		this->AddPart(pActor);
		this->m_pCellPicker->AddPickList(pActor);

		this->m_listLineSource.push_back(pLineSource);
		this->m_listTubeFilter.push_back(pTubeFilter);
		this->m_listLinePolyDataMapper.push_back(pPolyDataMapper);
		this->m_listLineActor.push_back(pActor);

		this->m_pTransformScale->TransformPoint(prev_pt, prev_pt);
		this->m_pTransformUnits->TransformPoint(prev_pt, prev_pt);
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
	this->m_pPoints->InsertPoint(id, x, y, z);

	vtkSphereSource *pSphereSource = vtkSphereSource::New();
	pSphereSource->SetPhiResolution(10);
	pSphereSource->SetThetaResolution(10);
	pSphereSource->SetRadius(this->m_fRadius);

	double pt[3];
	pt[0] = x; pt[1] = y; pt[2] = z;
	this->m_pTransformScale->TransformPoint(pt, pt);
	this->m_pTransformUnits->TransformPoint(pt, pt);
	pSphereSource->SetCenter(pt[0], pt[1], pt[2]);

	vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
	pPolyDataMapper->SetInput(pSphereSource->GetOutput());

	vtkActor *pActor = vtkActor::New();
	pActor->SetMapper(pPolyDataMapper);
	pActor->GetProperty()->SetColor(0., 0., 1.);

    this->m_pCellPicker->AddPickList(pActor);
	this->AddPart(pActor);

	if (this->m_pPoints->GetNumberOfPoints() > 1)
	{
		vtkLineSource *pLineSource = vtkLineSource::New();
		vtkTubeFilter *pTubeFilter = vtkTubeFilter::New();
		pTubeFilter->SetNumberOfSides(8);
		pTubeFilter->SetInput(pLineSource->GetOutput());
		pTubeFilter->SetRadius(this->m_fRadius / 2.0);
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
	double pt[3];
	vtkIdType np = this->m_pPoints->GetNumberOfPoints();
	for (vtkIdType i = 0; i < np; ++i)
	{
		this->m_pPoints->GetPoint(i, pt);
		pt[2] = z;
		this->m_pPoints->SetPoint(i, pt);
	}
	this->UpdatePoints();
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

	ASSERT(np - 1 == this->m_listLineSource.size());

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

void CRiverActor::SetRadius(float radius)
{
	this->m_fRadius = radius;
	std::list<vtkSphereSource*>::iterator iterSphereSource = this->m_listSphereSource.begin();
	for (; iterSphereSource != this->m_listSphereSource.end(); ++iterSphereSource)
	{
		(*iterSphereSource)->SetRadius(this->m_fRadius);
	}

	std::list<vtkTubeFilter*>::iterator iterTubeFilter = this->m_listTubeFilter.begin();
	for (; iterTubeFilter != this->m_listTubeFilter.end(); ++iterTubeFilter)
	{
		(*iterTubeFilter)->SetRadius(this->m_fRadius / 2.0);
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
		this->Interactor->RemoveObserver(this->EventCallbackCommand);
	}

	this->Interactor = i;

	if(i)
	{
		i->AddObserver(vtkCommand::EnterEvent, 
			this->EventCallbackCommand, 
			1);

		i->AddObserver(vtkCommand::LeaveEvent, 
			this->EventCallbackCommand, 
			1);

		i->AddObserver(vtkCommand::MouseMoveEvent, 
			this->EventCallbackCommand, 
			1);
	}
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
		TRACE("MouseMoveEvent\n");
		{
			int X = self->Interactor->GetEventPosition()[0];
			int Y = self->Interactor->GetEventPosition()[1];

			// Okay, we can process this. Try to pick handles first;
			// if no handles picked, then pick the bounding box.
			vtkRenderer *ren = self->Interactor->FindPokedRenderer(X,Y);
// COMMENT: {6/10/2005 2:55:30 PM}			if ( ren != this->CurrentRenderer )
// COMMENT: {6/10/2005 2:55:30 PM}			{
// COMMENT: {6/10/2005 2:55:30 PM}				this->State = vtkBoxWidget::Outside;
// COMMENT: {6/10/2005 2:55:30 PM}				return;
// COMMENT: {6/10/2005 2:55:30 PM}			}
			vtkAssemblyPath *path;
			self->m_pCellPicker->Pick(X, Y, 0.0, ren);
			path = self->m_pCellPicker->GetPath();
			if ( path != NULL )
			{
				TRACE("Actor found\n");
				// ASSERT(path->GetFirstNode()->GetProp()->IsA("vtkAssembly"));
				ostrstream oss;
				path->GetFirstNode()->GetProp()->PrintSelf(oss, 4);
				oss << ends;
				TRACE("\n");
				afxDump << "vtkAssemblyPath{{\n" << oss.str() << "vtkAssemblyPath}}\n";
				oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
				if (vtkActor* pActor = vtkActor::SafeDownCast(path->GetFirstNode()->GetProp()))
				{
					pActor->GetProperty()->SetColor(1., 0., 0.);
					self->Interactor->Render();
				}
			}
		}
		break;

	case vtkCommand::LeftButtonPressEvent: 
		break;

	case vtkCommand::LeftButtonReleaseEvent:
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
		break;

	case vtkCommand::KeyReleaseEvent: 
		break;

	case vtkCommand::CharEvent:  
		break;

	case vtkCommand::DeleteEvent:
		break;
	}
}

void CRiverActor::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	this->m_pPoints->PrintSelf(os, indent.GetNextIndent());
}
