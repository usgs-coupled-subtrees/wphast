#pragma once
#include "Action.h"

class CWPhastDoc;

class CSetScaleAction : public CAction
{
public:
	CSetScaleAction(CWPhastDoc* pDoc, double xScale, double yScale, double zScale);
	virtual ~CSetScaleAction(void);

	virtual void Execute();
	virtual void UnExecute();
	void Apply(double xScale, double yScale, double zScale);

protected:
	CWPhastDoc* m_pDoc;
	double m_Scale[3];
	double m_OriginalScale[3];
};
