#include "StdAfx.h"
#include "SetChemICAction.h"

#include "ICChemZoneActor.h"

CSetChemICAction::CSetChemICAction(CICChemZoneActor* pICChemZoneActor, CTreeCtrl* pTreeCtrl, const CChemIC& newChemIC, LPCTSTR desc)
	: m_pICChemZoneActor(pICChemZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldChemIC(pICChemZoneActor->GetData())
	, m_oldDesc(pICChemZoneActor->GetDesc())
	, m_newChemIC(newChemIC)
	, m_newDesc(desc)

{
}

CSetChemICAction::~CSetChemICAction(void)
{
}

void CSetChemICAction::Execute(void)
{
	this->SetData(this->m_newChemIC, this->m_newDesc);
}

void CSetChemICAction::UnExecute(void)
{
	this->SetData(this->m_oldChemIC, this->m_oldDesc);
}

void CSetChemICAction::SetData(const CChemIC& chemIC, std::string& rDesc)
{
	this->m_pICChemZoneActor->SetData(chemIC);
	this->m_pICChemZoneActor->SetDesc(rDesc.c_str());
	this->m_pICChemZoneActor->Update(this->m_pTreeCtrl, this->m_pICChemZoneActor->GetHTreeItem());
}
