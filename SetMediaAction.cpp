#include "StdAfx.h"
#include "SetMediaAction.h"

#include "MediaZoneActor.h"

CSetMediaAction::CSetMediaAction(CMediaZoneActor* pMediaZoneActor, CTreeCtrl* pTreeCtrl, const CGridElt& newGridElt)
	: m_pMediaZoneActor(pMediaZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldGridElt(pMediaZoneActor->GetGridElt())
	, m_newGridElt(newGridElt)
{
}

CSetMediaAction::~CSetMediaAction(void)
{
}

void CSetMediaAction::Execute(void)
{
	this->SetGridElt(this->m_newGridElt);
}

void CSetMediaAction::UnExecute(void)
{
	this->SetGridElt(this->m_oldGridElt);
}

void CSetMediaAction::SetGridElt(const CGridElt& rGridElt)
{
	this->m_pMediaZoneActor->SetGridElt(rGridElt);
	this->m_pMediaZoneActor->Update(this->m_pTreeCtrl, this->m_pMediaZoneActor->GetHTreeItem());
}

