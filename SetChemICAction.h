#pragma once

#include "Action.h"

#include "ChemIC.h"

class CICChemZoneActor;

class CSetChemICAction : public CAction
{
public:
	CSetChemICAction(CICChemZoneActor* pICChemZoneActor, CTreeCtrl* pTreeCtrl, const CChemIC& newChemIC);
	virtual ~CSetChemICAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void SetData(const CChemIC& chemIC);

protected:
	CICChemZoneActor* m_pICChemZoneActor;
	CTreeCtrl*        m_pTreeCtrl;
	CChemIC           m_oldChemIC;
	CChemIC           m_newChemIC;
};
