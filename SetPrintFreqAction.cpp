#include "StdAfx.h"
#include "SetPrintFreqAction.h"

#include "WPhastDoc.h"
#include "WPhastView.h"


CSetPrintFreqAction::CSetPrintFreqAction(CWPhastDoc* pDoc, const CPrintFreq& newPrintFreq, int nStressPeriod)
: m_pDoc(pDoc)
, m_oriPrintFreq(pDoc->GetPrintFrequency(nStressPeriod))
, m_newPrintFreq(newPrintFreq)
, m_nStressPeriod(nStressPeriod)
{
}

CSetPrintFreqAction::~CSetPrintFreqAction(void)
{
}

void CSetPrintFreqAction::Execute(void)
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetPrintFrequency(this->m_newPrintFreq, this->m_nStressPeriod);
}

void CSetPrintFreqAction::UnExecute(void)
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetPrintFrequency(this->m_oriPrintFreq, this->m_nStressPeriod);
}
