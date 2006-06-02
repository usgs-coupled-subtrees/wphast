#pragma once
#include "Action.h"

class CWPhastDoc;

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CSetScaleAction : public CAction
{
public:
	CSetScaleAction(CWPhastDoc* pDoc, vtkFloatingPointType xScale, vtkFloatingPointType yScale, vtkFloatingPointType zScale);
	virtual ~CSetScaleAction(void);

	virtual void Execute();
	virtual void UnExecute();
	void Apply(vtkFloatingPointType xScale, vtkFloatingPointType yScale, vtkFloatingPointType zScale);

protected:
	CWPhastDoc* m_pDoc;
	vtkFloatingPointType m_Scale[3];
	vtkFloatingPointType m_OriginalScale[3];
};
