#include "StdAfx.h"
#include "GridInsertLineAction.h"

#include "WPhastDoc.h"
#include "GridActor.h"

CGridInsertLineAction::CGridInsertLineAction(CGridActor *pGridActor, CWPhastDoc *pWPhastDoc, int nGridIndex, int nPlaneIndex, double dValue, bool bSkipFirstExecute)
: m_pGridActor(pGridActor)
, m_pWPhastDoc(pWPhastDoc)
, m_nGridIndex(nGridIndex)
, m_nPlaneIndex(nPlaneIndex)
, m_dValue(dValue)
, m_bSkipFirstExecute(bSkipFirstExecute)
{
	ASSERT(this->m_bSkipFirstExecute || this->m_nPlaneIndex != -1);
	ASSERT(this->m_pGridActor && this->m_pGridActor->IsA("CGridActor"));
}

CGridInsertLineAction::~CGridInsertLineAction(void)
{
}

void CGridInsertLineAction::Execute()
{
	if (!this->m_bSkipFirstExecute)
	{
		this->m_nPlaneIndex = this->m_pGridActor->InsertLine(this->m_nGridIndex, this->m_dValue);
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
	VERIFY(this->m_pGridActor->DeleteLine(this->m_nGridIndex, this->m_nPlaneIndex));
	this->m_pWPhastDoc->UpdateGridDomain();
}
