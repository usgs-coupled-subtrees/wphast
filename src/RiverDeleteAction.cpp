#include "StdAfx.h"
#include "RiverDeleteAction.h"

#include "WPhastDoc.h"
#include "RiverActor.h"

CRiverDeleteAction::CRiverDeleteAction(CWPhastDoc* pDoc, CRiverActor* pRiverActor)
: m_pDoc(pDoc)
, m_pRiverActor(pRiverActor)
{
	ASSERT_VALID(this->m_pDoc);
	ASSERT(this->m_pRiverActor && this->m_pRiverActor->IsA("CRiverActor"));
}

CRiverDeleteAction::~CRiverDeleteAction(void)
{
	ASSERT(this->m_pRiverActor && this->m_pRiverActor->IsA("CRiverActor"));
}

void CRiverDeleteAction::Execute()
{
	ASSERT_VALID(this->m_pDoc);	
	this->m_pDoc->Remove(this->m_pRiverActor);
}

void CRiverDeleteAction::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->UnRemove(this->m_pRiverActor);
}

