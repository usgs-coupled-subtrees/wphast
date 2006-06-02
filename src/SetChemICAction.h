#pragma once

#include "Action.h"

#include "ChemIC.h"

class CICChemZoneActor;

class CSetChemICAction : public CAction
{
public:
	CSetChemICAction(CICChemZoneActor* pICChemZoneActor, CTreeCtrl* pTreeCtrl, const CChemIC& newChemIC, LPCTSTR desc);
	virtual ~CSetChemICAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void SetData(const CChemIC& chemIC, std::string& rDesc);

protected:
	CICChemZoneActor* m_pICChemZoneActor;
	CTreeCtrl*        m_pTreeCtrl;
	CChemIC           m_oldChemIC;
	CChemIC           m_newChemIC;
	std::string     m_oldDesc;
	std::string     m_newDesc;
};
