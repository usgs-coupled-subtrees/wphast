#include "StdAfx.h"
#include "SetTimeControlAction.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

// COMMENT: {4/11/2005 1:46:04 PM}CSetTimeControlAction::CSetTimeControlAction(CWPhastDoc* pDoc, const CTimeControl& newTimeControl, int nStressPeriod)
// COMMENT: {4/11/2005 1:46:04 PM}: m_pDoc(pDoc)
// COMMENT: {4/11/2005 1:46:04 PM}, m_originalTimeControl(pDoc->GetTimeControl(nStressPeriod))
// COMMENT: {4/11/2005 1:46:04 PM}, m_newTimeControl(newTimeControl)
// COMMENT: {4/11/2005 1:46:04 PM}, m_nStressPeriod(nStressPeriod)
// COMMENT: {4/11/2005 1:46:04 PM}{
// COMMENT: {4/11/2005 1:46:04 PM}}
CSetTimeControlAction::CSetTimeControlAction(CWPhastDoc* pDoc, const CTimeControl& newTimeControl)
: m_pDoc(pDoc)
, m_originalTimeControl(pDoc->GetTimeControl())
, m_newTimeControl(newTimeControl)
{
}

CSetTimeControlAction::~CSetTimeControlAction(void)
{
}

void CSetTimeControlAction::Execute(void)
{
	ASSERT_VALID(this->m_pDoc);
// COMMENT: {4/11/2005 1:46:37 PM}	this->m_pDoc->SetTimeControl(this->m_newTimeControl, this->m_nStressPeriod);
	this->m_pDoc->SetTimeControl(this->m_newTimeControl);
}

void CSetTimeControlAction::UnExecute(void)
{
	ASSERT_VALID(this->m_pDoc);
// COMMENT: {4/11/2005 1:46:41 PM}	this->m_pDoc->SetTimeControl(this->m_originalTimeControl, this->m_nStressPeriod);
	this->m_pDoc->SetTimeControl(this->m_originalTimeControl);
}
