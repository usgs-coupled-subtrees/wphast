#include "StdAfx.h"
#include "SetPrintFreqAction.h"

#include "WPhastDoc.h"
#include "WPhastView.h"


// COMMENT: {4/11/2005 1:39:49 PM}CSetPrintFreqAction::CSetPrintFreqAction(CWPhastDoc* pDoc, const CPrintFreq& newPrintFreq, int nStressPeriod)
// COMMENT: {4/11/2005 1:39:49 PM}: m_pDoc(pDoc)
// COMMENT: {4/11/2005 1:39:49 PM}, m_oriPrintFreq(pDoc->GetPrintFrequency(nStressPeriod))
// COMMENT: {4/11/2005 1:39:49 PM}, m_newPrintFreq(newPrintFreq)
// COMMENT: {4/11/2005 1:39:49 PM}, m_nStressPeriod(nStressPeriod)
// COMMENT: {4/11/2005 1:39:49 PM}{
// COMMENT: {4/11/2005 1:39:49 PM}}
CSetPrintFreqAction::CSetPrintFreqAction(CWPhastDoc* pDoc, const CPrintFreq& newPrintFreq)
: m_pDoc(pDoc)
, m_oriPrintFreq(pDoc->GetPrintFrequency())
, m_newPrintFreq(newPrintFreq)
{
}

CSetPrintFreqAction::~CSetPrintFreqAction(void)
{
}

void CSetPrintFreqAction::Execute(void)
{
	ASSERT_VALID(this->m_pDoc);
// COMMENT: {4/11/2005 1:40:22 PM}	this->m_pDoc->SetPrintFrequency(this->m_newPrintFreq, this->m_nStressPeriod);
	this->m_pDoc->SetPrintFrequency(this->m_newPrintFreq);
}

void CSetPrintFreqAction::UnExecute(void)
{
	ASSERT_VALID(this->m_pDoc);
// COMMENT: {4/11/2005 1:40:29 PM}	this->m_pDoc->SetPrintFrequency(this->m_oriPrintFreq, this->m_nStressPeriod);
	this->m_pDoc->SetPrintFrequency(this->m_oriPrintFreq);
}
