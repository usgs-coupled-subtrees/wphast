#include "StdAfx.h"
#include "SetFlowOnlyAction.h"

#include <map>
#include "WPhastDoc.h"
#include "WPhastView.h"

CSetFlowOnlyAction::CSetFlowOnlyAction(CWPhastDoc* pDoc, const CFlowOnly& flowOnly)
: m_pDoc(pDoc)
, m_oriFlowOnly(pDoc->GetFlowOnly())
, m_newFlowOnly(flowOnly)
{
}

CSetFlowOnlyAction::~CSetFlowOnlyAction(void)
{
}

void CSetFlowOnlyAction::Execute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetFlowOnly(this->m_newFlowOnly);
}

void CSetFlowOnlyAction::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetFlowOnly(this->m_oriFlowOnly);
}
