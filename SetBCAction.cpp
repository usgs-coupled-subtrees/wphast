#include "StdAfx.h"
#include "SetBCAction.h"

#include "BCZoneActor.h"

CSetBCAction::CSetBCAction(CBCZoneActor* pBCZoneActor, CTreeCtrl* pTreeCtrl, const CBC& newBC, LPCTSTR desc)
	: m_pBCZoneActor(pBCZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldBC(pBCZoneActor->GetBC())
	, m_oldDesc(pBCZoneActor->GetDesc())
	, m_newBC(newBC)
	, m_newDesc(desc)
{
}

CSetBCAction::~CSetBCAction(void)
{
}

void CSetBCAction::Execute(void)
{
	this->SetBC(this->m_newBC, this->m_newDesc);
}

void CSetBCAction::UnExecute(void)
{
	this->SetBC(this->m_oldBC, this->m_oldDesc);
}

void CSetBCAction::SetBC(const CBC& rBC, std::string& rDesc)
{
	this->m_pBCZoneActor->SetBC(rBC);
	this->m_pBCZoneActor->SetDesc(rDesc.c_str());
	this->m_pBCZoneActor->Update(this->m_pTreeCtrl);
}

