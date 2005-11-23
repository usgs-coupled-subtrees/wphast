#include "StdAfx.h"
#include "SetChemICAction.h"

#include "ICChemZoneActor.h"

CSetChemICAction::CSetChemICAction(CICChemZoneActor* pICChemZoneActor, CTreeCtrl* pTreeCtrl, const CChemIC& newChemIC)
	: m_pICChemZoneActor(pICChemZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldChemIC(pICChemZoneActor->GetData())
	, m_newChemIC(newChemIC)
{
}

CSetChemICAction::~CSetChemICAction(void)
{
}

void CSetChemICAction::Execute(void)
{
	this->SetData(this->m_newChemIC);
}

void CSetChemICAction::UnExecute(void)
{
	this->SetData(this->m_oldChemIC);
}

void CSetChemICAction::SetData(const CChemIC& chemIC)
{
	this->m_pICChemZoneActor->SetData(chemIC);
	this->m_pICChemZoneActor->Update(this->m_pTreeCtrl, this->m_pICChemZoneActor->GetHTreeItem());
}
