#include "StdAfx.h"
#include "SetRiverAction.h"

#include "RiverActor.h"
#include "WPhastDoc.h"


CSetRiverAction::CSetRiverAction(CRiverActor *pRiverActor, const CRiver& newRiver, CWPhastDoc *pWPhastDoc)
: m_pRiverActor(pRiverActor)
, m_oldRiver(pRiverActor->GetRiver())
, m_newRiver(newRiver)
, m_pWPhastDoc(pWPhastDoc)
, m_Z(pRiverActor->GetZ())
{
	ASSERT(pRiverActor);
	ASSERT_VALID(pWPhastDoc);
}

CSetRiverAction::~CSetRiverAction(void)
{
}

void CSetRiverAction::Execute(void)
{
	this->SetRiver(this->m_newRiver);
}

void CSetRiverAction::UnExecute(void)
{
	this->SetRiver(this->m_oldRiver);
}

void CSetRiverAction::SetRiver(const CRiver &river)
{
	this->m_pRiverActor->SetRiver(river, this->m_pWPhastDoc->GetUnits());
	this->m_pRiverActor->SetZ(this->m_Z);
	CTreeCtrlNode top = this->m_pRiverActor->GetTreeCtrlNode().GetFirstVisible();
	this->m_pRiverActor->Update(this->m_pRiverActor->GetTreeCtrlNode());
	top.SelectSetFirstVisible();
	this->m_pWPhastDoc->Select(this->m_pRiverActor);
	//{{
	////this->m_pWPhastDoc->Notify(0, WPN_SELCHANGED, 0, this->m_pRiverActor);
	//}}
}
