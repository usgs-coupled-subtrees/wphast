#include "StdAfx.h"
#include "SetChemICAction.h"

#include "ICZoneActor.h"

CSetChemICAction::CSetChemICAction(CICZoneActor* pICZoneActor, CTreeCtrl* pTreeCtrl, const CChemIC& newChemIC)
	: m_pICZoneActor(pICZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldChemIC(pICZoneActor->GetChemIC())
	, m_newChemIC(newChemIC)
{
}

CSetChemICAction::~CSetChemICAction(void)
{
}

void CSetChemICAction::Execute(void)
{
	this->SetChemIC(this->m_newChemIC);
}

void CSetChemICAction::UnExecute(void)
{
	this->SetChemIC(this->m_oldChemIC);
}

void CSetChemICAction::SetChemIC(const CChemIC& chemIC)
{
	this->m_pICZoneActor->SetChemIC(chemIC);
	this->m_pICZoneActor->Update(this->m_pTreeCtrl, this->m_pICZoneActor->GetHTreeItem());
}
