#include "StdAfx.h"
#include "SetPrintFreqAction.h"

#include "WPhastDoc.h"
#include "WPhastView.h"


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
	this->m_pDoc->SetPrintFrequency(this->m_newPrintFreq);
}

void CSetPrintFreqAction::UnExecute(void)
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetPrintFrequency(this->m_oriPrintFreq);
}
