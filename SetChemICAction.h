#pragma once

#include "Action.h"

#include "ChemIC.h"

class CICZoneActor;

class CSetChemICAction :
	public CAction
{
public:
	CSetChemICAction(CICZoneActor* pICZoneActor, CTreeCtrl* pTreeCtrl, const CChemIC& newChemIC);
	virtual ~CSetChemICAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void SetChemIC(const CChemIC& chemIC);

protected:
	CICZoneActor* m_pICZoneActor;
	CTreeCtrl*    m_pTreeCtrl;
	CChemIC       m_oldChemIC;
	CChemIC       m_newChemIC;
};
