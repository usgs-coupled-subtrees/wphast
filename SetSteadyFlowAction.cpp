#include "StdAfx.h"
#include "SetSteadyFlowAction.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

CSetSteadyFlowAction::CSetSteadyFlowAction(CWPhastDoc* pDoc, const CSteadyFlow& steadyFlow)
: m_pDoc(pDoc)
, m_oriSteadyFlow(pDoc->GetSteadyFlow())
, m_newSteadyFlow(steadyFlow)
{
}

CSetSteadyFlowAction::~CSetSteadyFlowAction(void)
{
}

void CSetSteadyFlowAction::Execute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetSteadyFlow(this->m_newSteadyFlow);
}

void CSetSteadyFlowAction::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetSteadyFlow(this->m_oriSteadyFlow);
}
