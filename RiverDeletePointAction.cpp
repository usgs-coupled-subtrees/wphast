#include "StdAfx.h"
#include "RiverDeletePointAction.h"

#include "RiverActor.h"



CRiverDeletePointAction::CRiverDeletePointAction(CRiverActor* pRiverActor, vtkIdType id)
: m_pRiverActor(pRiverActor)
, m_id(id)
{
	ASSERT(this->m_pRiverActor && this->m_pRiverActor->IsA("CRiverActor"));
	ASSERT(this->m_pRiverActor->GetRiverPoint(this->m_id));
	if (this->m_pRiverActor && this->m_pRiverActor->GetRiverPoint(this->m_id))
	{
		CRiverPoint pt(*this->m_pRiverActor->GetRiverPoint(this->m_id));
		ASSERT(pt.x_defined && pt.y_defined);
		this->m_riverPoint = pt;
	}
}

CRiverDeletePointAction::~CRiverDeletePointAction(void)
{
	ASSERT(this->m_pRiverActor && this->m_pRiverActor->IsA("CRiverActor"));
}

void CRiverDeletePointAction::Execute()
{
	this->m_pRiverActor->DeletePoint(this->m_id);
}

void CRiverDeletePointAction::UnExecute()
{
	this->m_pRiverActor->InsertPoint(this->m_id - 1, this->m_riverPoint);
}

