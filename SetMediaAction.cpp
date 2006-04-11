#include "StdAfx.h"
#include "SetMediaAction.h"

#include "MediaZoneActor.h"

CSetMediaAction::CSetMediaAction(CMediaZoneActor* pMediaZoneActor, CTreeCtrl* pTreeCtrl, const CGridElt& newGridElt, LPCTSTR desc)
	: m_pMediaZoneActor(pMediaZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldGridElt(pMediaZoneActor->GetGridElt())
	, m_oldDesc(pMediaZoneActor->GetDesc())
	, m_newGridElt(newGridElt)
	, m_newDesc(desc)

{
}

CSetMediaAction::~CSetMediaAction(void)
{
}

void CSetMediaAction::Execute(void)
{
	this->SetGridElt(this->m_newGridElt, this->m_newDesc);
}

void CSetMediaAction::UnExecute(void)
{
	this->SetGridElt(this->m_oldGridElt, this->m_oldDesc);
}

void CSetMediaAction::SetGridElt(const CGridElt& rGridElt, std::string& rDesc)
{
	this->m_pMediaZoneActor->SetGridElt(rGridElt);
	this->m_pMediaZoneActor->SetDesc(rDesc.c_str());
	this->m_pMediaZoneActor->Update(this->m_pTreeCtrl, this->m_pMediaZoneActor->GetHTreeItem());
}

