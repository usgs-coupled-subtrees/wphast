#include "StdAfx.h"
#include "SetBCAction.h"

#include "BCZoneActor.h"

CSetBCAction::CSetBCAction(CBCZoneActor* pBCZoneActor, CTreeCtrl* pTreeCtrl, const CBC& newBC)
	: m_pBCZoneActor(pBCZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldBC(pBCZoneActor->GetBC())
	, m_newBC(newBC)
{
}

CSetBCAction::~CSetBCAction(void)
{
}

void CSetBCAction::Execute(void)
{
	this->SetBC(this->m_newBC);
}

void CSetBCAction::UnExecute(void)
{
	this->SetBC(this->m_oldBC);
}

void CSetBCAction::SetBC(const CBC& rBC)
{
	this->m_pBCZoneActor->SetBC(rBC);
	this->m_pBCZoneActor->Update(this->m_pTreeCtrl);
}

