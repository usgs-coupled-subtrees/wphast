#include "StdAfx.h"
#include "SetTimeControlAction.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

CSetTimeControlAction::CSetTimeControlAction(CWPhastDoc* pDoc, const CTimeControl2& newTimeControl)
: m_pDoc(pDoc)
, m_originalTimeControl(pDoc->GetTimeControl2())
, m_newTimeControl(newTimeControl)
{
}

CSetTimeControlAction::~CSetTimeControlAction(void)
{
}

void CSetTimeControlAction::Execute(void)
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetTimeControl2(this->m_newTimeControl);
}

void CSetTimeControlAction::UnExecute(void)
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetTimeControl2(this->m_originalTimeControl);
}
