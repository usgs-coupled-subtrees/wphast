#pragma once
#include "Action.h"

#include "SteadyFlow.h"

class CWPhastDoc;

class CSetSteadyFlowAction :
	public CAction
{
public:
	CSetSteadyFlowAction(CWPhastDoc* pDoc, const CSteadyFlow& steadyFlow);
	virtual ~CSetSteadyFlowAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc* m_pDoc;
	CSteadyFlow   m_oriSteadyFlow;
	CSteadyFlow   m_newSteadyFlow;
};
