#include "StdAfx.h"
#include "AxesActor.h"

#include <vtkAxes.h>
#include <vtkTubeFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>

#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(CAxesActor, "$Revision$");
vtkStandardNewMacro(CAxesActor);

// Note: No header files should follow the next three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CAxesActor::CAxesActor(void)
: m_pAxes(0)
, m_pTubeFilter(0)
, m_pPolyDataMapper(0)
, m_DefaultSize(1.0)
, m_DefaultTubeDiameter(1.0)
{
	this->m_DefaultPositions[0] = 0;
	this->m_DefaultPositions[1] = 1;
	this->m_DefaultPositions[2] = 0;
	this->m_DefaultPositions[3] = 1;
	this->m_DefaultPositions[4] = 0;
	this->m_DefaultPositions[5] = 1;

	this->m_NormalizedPosition[0] = 0;
	this->m_NormalizedPosition[1] = 0;
	this->m_NormalizedPosition[2] = 0;

	this->m_pAxes = vtkAxes::New();
	this->m_pAxes->SetOrigin(0, 0, 0);

	this->m_pTubeFilter = vtkTubeFilter::New();
	this->m_pTubeFilter->SetInput(this->m_pAxes->GetOutput());
	this->m_pTubeFilter->SetNumberOfSides(10);

	this->m_pPolyDataMapper = vtkPolyDataMapper::New();
	this->m_pPolyDataMapper->SetInput(this->m_pTubeFilter->GetOutput());

	this->SetMapper(this->m_pPolyDataMapper);
	this->SetPickable(0);

	// match Model Viewer's colors
	this->m_LookupTable = vtkLookupTable::New();
	this->m_LookupTable->SetNumberOfColors(3);
	this->m_LookupTable->SetTableValue(0, 1, 0, 0, 1);
	this->m_LookupTable->SetTableValue(1, 0, 1, 0, 1);
	this->m_LookupTable->SetTableValue(2, 0, 0, 1, 1);
	this->m_LookupTable->Build();
	this->m_pPolyDataMapper->SetLookupTable(this->m_LookupTable);
	this->m_pPolyDataMapper->SetScalarRange(0, 0.5);
}

CAxesActor::~CAxesActor(void)
{
	this->m_pAxes->Delete();
	this->m_pTubeFilter->Delete();
	this->m_pPolyDataMapper->Delete();
	this->m_LookupTable->Delete();
}

void CAxesActor::SetDefaultPositions(const double p[6])
{
	this->m_DefaultPositions[0] = p[0];
	this->m_DefaultPositions[1] = p[1];
	this->m_DefaultPositions[2] = p[2];
	this->m_DefaultPositions[3] = p[3];
	this->m_DefaultPositions[4] = p[4];
	this->m_DefaultPositions[5] = p[5];

	float xo = this->m_NormalizedPosition[0]*(p[1]-p[0]) + p[0];
	float yo = this->m_NormalizedPosition[1]*(p[3]-p[2]) + p[2];
	float zo = this->m_NormalizedPosition[2]*(p[5]-p[4]) + p[4];

	this->m_pAxes->SetOrigin(xo, yo, zo);
}

void CAxesActor::SetDefaultSize(float s)
{
	this->m_pAxes->SetScaleFactor(this->GetNormalizedSize() * s);
	this->m_DefaultSize = s;
}

void CAxesActor::SetDefaultTubeDiameter(float d)
{
	this->m_pTubeFilter->SetRadius(this->GetNormalizedTubeDiameter() * d / 2);
	this->m_DefaultTubeDiameter = d;
}

float CAxesActor::GetNormalizedSize(void)const
{
	return (this->m_pAxes->GetScaleFactor() / this->m_DefaultSize);
}

float CAxesActor::GetNormalizedTubeDiameter(void) const
{
	return (2 * this->m_pTubeFilter->GetRadius() / this->m_DefaultTubeDiameter);
}

const float *CAxesActor::GetNormalizedPosition(void)const
{
	return this->m_NormalizedPosition;
}

int CAxesActor::GetRepresentation(void)const
{
	if (this->m_pPolyDataMapper->GetInput() == this->m_pTubeFilter->GetOutput())
	{
		return 0;    // tube
	}
	else
	{
		return 1;    // line
	}
}
