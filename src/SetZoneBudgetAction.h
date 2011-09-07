#pragma once

#include "Action.h"

#include "srcinput/Zone_budget.h"

class CZoneFlowRateZoneActor;

class CSetZoneBudgetAction : public CAction
{
public:
	CSetZoneBudgetAction(CZoneFlowRateZoneActor* pZFRZoneActor, CTreeCtrl* pTreeCtrl, const Zone_budget& newZB);
	virtual ~CSetZoneBudgetAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void SetData(const Zone_budget& rZB);

protected:
	CZoneFlowRateZoneActor* ZFRZoneActor;
	CTreeCtrl*              TreeCtrl;
	Zone_budget             OldZone_budget;
	Zone_budget             NewOldZone_budget;
};
