#include "StdAfx.h"
#include "WellDeleteAction.h"

#include <map>
#include "WPhastDoc.h"
#include "WellActor.h"

CWellDeleteAction::CWellDeleteAction(CWPhastDoc* pDoc, CWellActor* pWellActor)
: m_pDoc(pDoc)
, m_pWellActor(pWellActor)
{
	ASSERT_VALID(this->m_pDoc);
	ASSERT(this->m_pWellActor && this->m_pWellActor->IsA("CWellActor"));
}

CWellDeleteAction::~CWellDeleteAction(void)
{
	ASSERT(this->m_pWellActor && this->m_pWellActor->IsA("CWellActor"));
}

void CWellDeleteAction::Execute()
{
	ASSERT_VALID(this->m_pDoc);	
	this->m_pDoc->Remove(this->m_pWellActor);
}

void CWellDeleteAction::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->UnRemove(this->m_pWellActor);
}

