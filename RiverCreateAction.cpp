#include "StdAfx.h"
#include "RiverCreateAction.h"

CRiverCreateAction::CRiverCreateAction(CWPhastDoc* pDoc, const CRiver &river)
: m_pDoc(pDoc)
, m_river(river)
{
	ASSERT_VALID(this->m_pDoc);

	this->m_pRiverActor = CRiverActor::New();
	ASSERT(this->m_pRiverActor->IsA("CRiverActor"));

	this->m_pRiverActor->SetRiver(river, this->m_pDoc->GetUnits());
#ifdef _DEBUG
	this->m_pRiverActor->SetRiver(river, this->m_pDoc->GetUnits());
#endif
}

CRiverCreateAction::~CRiverCreateAction(void)
{
	ASSERT(this->m_pRiverActor);
	if (this->m_pRiverActor)
	{
		this->m_pRiverActor->Delete();
		this->m_pRiverActor = 0;
	}
}

void CRiverCreateAction::Execute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->Add(this->m_pRiverActor);
}

void CRiverCreateAction::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->UnAdd(this->m_pRiverActor);
}
