#include "StdAfx.h"
#include "SetHeadICAction.h"

#include "ICHeadZoneActor.h"

CSetHeadICAction::CSetHeadICAction(CICHeadZoneActor* pICHeadZoneActor, CTreeCtrl* pTreeCtrl, const CHeadIC& newHeadIC)
	: m_pICHeadZoneActor(pICHeadZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldHeadIC(pICHeadZoneActor->GetData())
	, m_newHeadIC(newHeadIC)
{
}

CSetHeadICAction::~CSetHeadICAction(void)
{
}

void CSetHeadICAction::Execute(void)
{
	this->SetData(this->m_newHeadIC);
}

void CSetHeadICAction::UnExecute(void)
{
	this->SetData(this->m_oldHeadIC);
}

void CSetHeadICAction::SetData(const CHeadIC& rHeadIC)
{
	this->m_pICHeadZoneActor->SetData(rHeadIC);
	this->m_pICHeadZoneActor->Update(this->m_pTreeCtrl, this->m_pICHeadZoneActor->GetHTreeItem());
}

