#include "StdAfx.h"
#include "SetTimeControl2Action.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

CSetTimeControl2Action::CSetTimeControl2Action(CWPhastDoc* pDoc, const CTimeControl2& newTimeControl2)
: m_pDoc(pDoc)
, m_originalTimeControl2(pDoc->GetTimeControl2())
, m_newTimeControl2(newTimeControl2)
{
}

CSetTimeControl2Action::~CSetTimeControl2Action(void)
{
}

void CSetTimeControl2Action::Execute(void)
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetTimeControl2(this->m_newTimeControl2);
}

void CSetTimeControl2Action::UnExecute(void)
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetTimeControl2(this->m_originalTimeControl2);
}
