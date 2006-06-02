#include "StdAfx.h"
#include "SetUnitsAction.h"

#include "WPhastDoc.h"
#include "WPhastView.h"


CSetUnitsAction::CSetUnitsAction(CWPhastDoc* pDoc, const CUnits& newUnits)
: m_pDoc(pDoc)
, m_originalUnits(pDoc->GetUnits())
, m_newUnits(newUnits)
{
}

CSetUnitsAction::~CSetUnitsAction(void)
{
}

void CSetUnitsAction::Execute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetUnits(this->m_newUnits);
}

void CSetUnitsAction::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetUnits(this->m_originalUnits);
}
