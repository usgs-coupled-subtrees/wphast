#include "StdAfx.h"
#include "SetPrintInputAction.h"

#include "WPhastDoc.h"

CSetPrintInputAction::CSetPrintInputAction(CWPhastDoc* pDoc, const CPrintInput& newPrintInput)
: m_pDoc(pDoc)
, m_oriPrintInput(pDoc->GetPrintInput())
, m_newPrintInput(newPrintInput)
{
}

CSetPrintInputAction::~CSetPrintInputAction(void)
{
}

void CSetPrintInputAction::Execute(void)
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetPrintInput(this->m_newPrintInput);
}

void CSetPrintInputAction::UnExecute(void)
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetPrintInput(this->m_oriPrintInput);
}
