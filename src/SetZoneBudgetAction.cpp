#include "StdAfx.h"
#include "SetZoneBudgetAction.h"

#include "ZoneFlowRateZoneActor.h"

CSetZoneBudgetAction::CSetZoneBudgetAction(CZoneFlowRateZoneActor* pZFRZoneActor, CTreeCtrl* pTreeCtrl, const Zone_budget& newZB)
: ZFRZoneActor(pZFRZoneActor)
, TreeCtrl(pTreeCtrl)
, OldZone_budget(pZFRZoneActor->GetData())
, NewOldZone_budget(newZB)
{
}

CSetZoneBudgetAction::~CSetZoneBudgetAction(void)
{
}

void CSetZoneBudgetAction::Execute(void)
{
	this->SetData(this->NewOldZone_budget);
}

void CSetZoneBudgetAction::UnExecute(void)
{
	this->SetData(this->OldZone_budget);
}

void CSetZoneBudgetAction::SetData(const Zone_budget& rZB)
{
	this->ZFRZoneActor->SetData(rZB);
	this->ZFRZoneActor->Update(this->TreeCtrl, this->ZFRZoneActor->GetHTreeItem());
}

