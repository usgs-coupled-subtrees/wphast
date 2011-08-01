#include "StdAfx.h"
#include "GridDeleteLineAction.h"

#include "WPhastDoc.h"
#include "GridActor.h"

CGridDeleteLineAction::CGridDeleteLineAction(CGridActor *pGridActor, CWPhastDoc *pWPhastDoc, int nGridIndex, int nPlaneIndex, double dValue, bool bSkipFirstExecute)
: m_pGridActor(pGridActor)
, m_pWPhastDoc(pWPhastDoc)
, m_nGridIndex(nGridIndex)
, m_nPlaneIndex(nPlaneIndex)
, m_dValue(dValue)
, m_bSkipFirstExecute(bSkipFirstExecute)
{
	ASSERT(this->m_pGridActor && this->m_pGridActor->IsA("CGridActor"));
}

CGridDeleteLineAction::~CGridDeleteLineAction(void)
{
	ASSERT(this->m_pGridActor && this->m_pGridActor->IsA("CGridActor"));
}

void CGridDeleteLineAction::Execute()
{
	if (!this->m_bSkipFirstExecute)
	{
		VERIFY(this->m_pGridActor->DeleteLine(this->m_nGridIndex, this->m_nPlaneIndex));
	}
	this->m_bSkipFirstExecute = false;

	// do this always
	//
	this->m_pWPhastDoc->UpdateGridDomain();
}

void CGridDeleteLineAction::UnExecute()
{
	VERIFY(this->m_pGridActor->InsertLine(this->m_nGridIndex, this->m_dValue) != -1);
	this->m_pWPhastDoc->UpdateGridDomain();
}
