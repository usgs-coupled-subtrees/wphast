#pragma once
#include "Action.h"

class CWPhastDoc;

class CSetScaleAction :
	public CAction
{
public:
	CSetScaleAction(CWPhastDoc* pDoc, float xScale, float yScale, float zScale);
	virtual ~CSetScaleAction(void);

	virtual void Execute();
	virtual void UnExecute();
	void Apply(float xScale, float yScale, float zScale);

protected:
	CWPhastDoc* m_pDoc;
	float m_Scale[3];
	float m_OriginalScale[3];
};
