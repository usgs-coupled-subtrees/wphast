#include "StdAfx.h"
#include "SetTimeControlAction.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

CSetTimeControlAction::CSetTimeControlAction(CWPhastDoc* pDoc, const CTimeControl& newTimeControl, int nStressPeriod)
: m_pDoc(pDoc)
, m_originalTimeControl(pDoc->GetTimeControl(nStressPeriod))
, m_newTimeControl(newTimeControl)
, m_nStressPeriod(nStressPeriod)
{
}

CSetTimeControlAction::~CSetTimeControlAction(void)
{
}

void CSetTimeControlAction::Execute(void)
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetTimeControl(this->m_newTimeControl, this->m_nStressPeriod);
}

void CSetTimeControlAction::UnExecute(void)
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetTimeControl(this->m_originalTimeControl, this->m_nStressPeriod);
}
