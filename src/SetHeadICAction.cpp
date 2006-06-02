#include "StdAfx.h"
#include "SetHeadICAction.h"

#include "ICHeadZoneActor.h"

CSetHeadICAction::CSetHeadICAction(CICHeadZoneActor* pICHeadZoneActor, CTreeCtrl* pTreeCtrl, const CHeadIC& newHeadIC, LPCTSTR desc)
	: m_pICHeadZoneActor(pICHeadZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldHeadIC(pICHeadZoneActor->GetData())
	, m_oldDesc(pICHeadZoneActor->GetDesc())
	, m_newHeadIC(newHeadIC)
	, m_newDesc(desc)
{
}

CSetHeadICAction::~CSetHeadICAction(void)
{
}

void CSetHeadICAction::Execute(void)
{
	this->SetData(this->m_newHeadIC, this->m_newDesc);
}

void CSetHeadICAction::UnExecute(void)
{
	this->SetData(this->m_oldHeadIC, this->m_oldDesc);
}

void CSetHeadICAction::SetData(const CHeadIC& rHeadIC, std::string& rDesc)
{
	this->m_pICHeadZoneActor->SetData(rHeadIC);
	this->m_pICHeadZoneActor->SetDesc(rDesc.c_str());
	this->m_pICHeadZoneActor->Update(this->m_pTreeCtrl, this->m_pICHeadZoneActor->GetHTreeItem());
}

