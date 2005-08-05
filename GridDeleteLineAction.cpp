#include "StdAfx.h"
#include "GridDeleteLineAction.h"

#include "WPhastDoc.h"
#include "GridLODActor.h"

CGridDeleteLineAction::CGridDeleteLineAction(CGridLODActor *pGridLODActor, CWPhastDoc *pWPhastDoc, int nGridIndex, int nPlaneIndex, double dValue, bool bSkipFirstExecute)
: m_pGridLODActor(pGridLODActor)
, m_pWPhastDoc(pWPhastDoc)
, m_nGridIndex(nGridIndex)
, m_nPlaneIndex(nPlaneIndex)
, m_dValue(dValue)
, m_bSkipFirstExecute(bSkipFirstExecute)
{
	ASSERT(this->m_pGridLODActor && this->m_pGridLODActor->IsA("CGridLODActor"));
}

CGridDeleteLineAction::~CGridDeleteLineAction(void)
{
	ASSERT(this->m_pGridLODActor && this->m_pGridLODActor->IsA("CGridLODActor"));
}

void CGridDeleteLineAction::Execute()
{
	if (!this->m_bSkipFirstExecute)
	{
		this->m_pGridLODActor->DeleteLine(this->m_nGridIndex, this->m_nPlaneIndex);
	}
	this->m_bSkipFirstExecute = false;

	// do this always
	//
	this->m_pWPhastDoc->UpdateGridDomain();
}

void CGridDeleteLineAction::UnExecute()
{
	// void CGridLODActor::InsertLine(int nGridIndex, double dValue);
	//this->m_pGridLODActor->InsertLine(this->m_nGridIndex, this->m_dValue);
	this->m_pWPhastDoc->UpdateGridDomain();
}
