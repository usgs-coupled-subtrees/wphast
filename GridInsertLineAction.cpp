#include "StdAfx.h"
#include "GridInsertLineAction.h"

#include "WPhastDoc.h"
#include "GridLODActor.h"

CGridInsertLineAction::CGridInsertLineAction(CGridLODActor *pGridLODActor, CWPhastDoc *pWPhastDoc, int nGridIndex, int nPlaneIndex, double dValue, bool bSkipFirstExecute)
: m_pGridLODActor(pGridLODActor)
, m_pWPhastDoc(pWPhastDoc)
, m_nGridIndex(nGridIndex)
, m_nPlaneIndex(nPlaneIndex)
, m_dValue(dValue)
, m_bSkipFirstExecute(bSkipFirstExecute)
{
	ASSERT(this->m_bSkipFirstExecute || this->m_nPlaneIndex != -1);
	ASSERT(this->m_pGridLODActor && this->m_pGridLODActor->IsA("CGridLODActor"));
}

CGridInsertLineAction::~CGridInsertLineAction(void)
{
}

void CGridInsertLineAction::Execute()
{
	if (!this->m_bSkipFirstExecute)
	{
		this->m_nPlaneIndex = this->m_pGridLODActor->InsertLine(this->m_nGridIndex, this->m_dValue);
		ASSERT(this->m_nPlaneIndex != -1);
	}
	this->m_bSkipFirstExecute = false;

	// do this always
	//
	this->m_pWPhastDoc->UpdateGridDomain();
}

void CGridInsertLineAction::UnExecute()
{
	ASSERT(this->m_nPlaneIndex != -1);
	VERIFY(this->m_pGridLODActor->DeleteLine(this->m_nGridIndex, this->m_nPlaneIndex));
	this->m_pWPhastDoc->UpdateGridDomain();
}
