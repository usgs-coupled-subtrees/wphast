#include "StdAfx.h"
#include "WellCreateAction.h"

CWellCreateAction::CWellCreateAction(CWPhastDoc* pDoc, const CWellSchedule &well)
: m_pDoc(pDoc)
, m_well(well)
{
	this->m_pWellActor = CWellActor::New();
	ASSERT(this->m_pWellActor->IsA("CWellActor"));

	this->m_pWellActor->SetWell(well, this->m_pDoc->GetUnits());
}

CWellCreateAction::~CWellCreateAction(void)
{
	ASSERT(this->m_pWellActor);
	if (this->m_pWellActor) {
		this->m_pWellActor->Delete();
		this->m_pWellActor = 0;
	}
}

void CWellCreateAction::Execute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->Add(this->m_pWellActor);
}

void CWellCreateAction::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->UnAdd(this->m_pWellActor);
}

