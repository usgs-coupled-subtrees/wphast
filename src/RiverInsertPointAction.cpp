#include "StdAfx.h"
#include "RiverInsertPointAction.h"

#include "WPhastDoc.h"
#include "RiverActor.h"


CRiverInsertPointAction::CRiverInsertPointAction(CRiverActor* pRiverActor, vtkIdType id, double x, double y, double z, bool bSkipFirstExecute)
: m_pRiverActor(pRiverActor)
, m_id(id)
, m_x(x)
, m_y(y)
, m_z(z)
, m_bSkipFirstExecute(bSkipFirstExecute)
{
	ASSERT(this->m_pRiverActor && this->m_pRiverActor->IsA("CRiverActor"));
}

CRiverInsertPointAction::~CRiverInsertPointAction(void)
{
	ASSERT(this->m_pRiverActor && this->m_pRiverActor->IsA("CRiverActor"));
}

void CRiverInsertPointAction::Execute()
{
	if (!this->m_bSkipFirstExecute)
	{
		this->m_pRiverActor->InsertPoint(this->m_id, this->m_x, this->m_y, this->m_z);
	}
	this->m_bSkipFirstExecute = false;
}

void CRiverInsertPointAction::UnExecute()
{
	this->m_pRiverActor->DeletePoint(this->m_id + 1);
}

