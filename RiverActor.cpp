#include "StdAfx.h"
#include "RiverActor.h"

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
{
	this->m_pPoints         = vtkPoints::New();
	this->m_pTransformUnits = vtkTransform::New();
	this->m_pTransformScale = vtkTransform::New();
	this->m_pTransformUnits->Identity();
	this->m_pTransformScale->Identity();

	this->m_pCellPicker     = vtkCellPicker::New();
	this->m_pCellPicker->SetTolerance(0.001);
	this->m_pCellPicker->PickFromListOn();
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
	double pt[3];
	vtkIdType np = this->m_pPoints->GetNumberOfPoints();
	ASSERT(np == this->m_listSphereSource.size());
	ASSERT(np == this->m_listActor.size());
	ASSERT(np == this->m_listPolyDataMapper.size());
	std::list<vtkSphereSource*>::iterator iterSphereSource = this->m_listSphereSource.begin();
	for (vtkIdType i = 0; i < np; ++i, ++iterSphereSource)
	{
		if (iterSphereSource == this->m_listSphereSource.end()) break;
		this->m_pPoints->GetPoint(i, pt);
		this->m_pTransformScale->TransformPoint(pt, pt);
		this->m_pTransformUnits->TransformPoint(pt, pt);
		(*iterSphereSource)->SetCenter(pt[0], pt[1], pt[2]);
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
		(*iterActor)->Delete();
	}
	this->m_listActor.clear();
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
}
