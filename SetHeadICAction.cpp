#include "StdAfx.h"
#include "SetHeadICAction.h"

#include "ICZoneActor.h"

CSetHeadICAction::CSetHeadICAction(CICZoneActor* pICZoneActor, CTreeCtrl* pTreeCtrl, const CHeadIC& newHeadIC)
	: m_pICZoneActor(pICZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldHeadIC(pICZoneActor->GetHeadIC())
	, m_newHeadIC(newHeadIC)
{
}

CSetHeadICAction::~CSetHeadICAction(void)
{
}

void CSetHeadICAction::Execute(void)
{
	this->SetHeadIC(this->m_newHeadIC);
}

void CSetHeadICAction::UnExecute(void)
{
	this->SetHeadIC(this->m_oldHeadIC);
}

void CSetHeadICAction::SetHeadIC(const CHeadIC& rHeadIC)
{
	this->m_pICZoneActor->SetHeadIC(rHeadIC);
	this->m_pICZoneActor->Update(this->m_pTreeCtrl, this->m_pICZoneActor->GetHTreeItem());
}

