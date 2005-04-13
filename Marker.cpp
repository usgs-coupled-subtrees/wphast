#include "StdAfx.h"
#include "Marker.h"

#include <vtkLineSource.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyDataMapper.h>

#include <vtkObjectFactory.h>
vtkCxxRevisionMacro(CMarker, "$Revision$");
vtkStandardNewMacro(CMarker);


CMarker::CMarker(void)
: m_horzLine(0)
, m_vertLine(0)
, m_append(0)
, m_mapper(0)
{
	this->Point[0] = this->Point[1] = this->Point[2] = 0.0;

	this->m_horzLine = vtkLineSource::New();
	this->m_vertLine = vtkLineSource::New();

	this->m_append = vtkAppendPolyData::New();
	this->m_append->AddInput(this->m_horzLine->GetOutput());
	this->m_append->AddInput(this->m_vertLine->GetOutput());

    this->m_mapper = vtkPolyDataMapper::New();
	this->m_mapper->SetInput(this->m_append->GetOutput());

	this->SetMapper(this->m_mapper);
}

CMarker::~CMarker(void)
{
	this->m_horzLine->Delete();
	this->m_vertLine->Delete();
	this->m_append->Delete();
	this->m_mapper->Delete();
}

void CMarker::Modified()
{
	this->m_vertLine->SetPoint1(this->Point[0], -1e30, this->Point[2]);
	this->m_vertLine->SetPoint2(this->Point[0], +1e30, this->Point[2]);

	this->m_horzLine->SetPoint1(-1e30, this->Point[1], this->Point[2]);
	this->m_horzLine->SetPoint2(+1e30, this->Point[1], this->Point[2]);

	this->Superclass::Modified();
}