#pragma once
#include "Action.h"

#include "FlowOnly.h"

class CWPhastDoc;

class CSetFlowOnlyAction :
	public CAction
{
public:
	CSetFlowOnlyAction(CWPhastDoc* pDoc, const CFlowOnly& flowOnly);
	virtual ~CSetFlowOnlyAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc* m_pDoc;
	CFlowOnly   m_oriFlowOnly;
	CFlowOnly   m_newFlowOnly;
};
