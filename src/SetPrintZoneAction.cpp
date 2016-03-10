#include "StdAfx.h"
#include "SetPrintZoneAction.h"

#include "PrintZoneActor.h"

CSetPrintZoneAction::CSetPrintZoneAction(CPrintZoneActor* pPrintZoneActor, CTreeCtrl* pTreeCtrl, const CPrintZone& newPrintZone, LPCTSTR desc)
	: m_pPrintZoneActor(pPrintZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldPrintZone(m_pPrintZoneActor->GetData())
	, m_oldDesc(m_pPrintZoneActor->GetDesc())
	, m_newPrintZone(newPrintZone)
	, m_newDesc(desc)
{
}

CSetPrintZoneAction::~CSetPrintZoneAction(void)
{
}

void CSetPrintZoneAction::Execute(void)
{
	this->SetPrintZone(this->m_newPrintZone, this->m_newDesc);
}

void CSetPrintZoneAction::UnExecute(void)
{
	this->SetPrintZone(this->m_oldPrintZone, this->m_oldDesc);
}

void CSetPrintZoneAction::SetPrintZone(const CPrintZone& rPrintZone, std::string& rDesc)
{
	this->m_pPrintZoneActor->SetData(rPrintZone);
	this->m_pPrintZoneActor->SetDesc(rDesc.c_str());
	this->m_pPrintZoneActor->Update(this->m_pTreeCtrl, this->m_pPrintZoneActor->GetHTreeItem());
}

