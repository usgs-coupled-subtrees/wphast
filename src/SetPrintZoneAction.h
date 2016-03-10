#pragma once

#include "Action.h"

#include "PrintZone.h"

class CPrintZoneActor;

class CSetPrintZoneAction : public CAction
{
public:
	CSetPrintZoneAction(CPrintZoneActor* pPrintZoneActor, CTreeCtrl* pTreeCtrl, const CPrintZone& printZone, LPCTSTR desc);
	virtual ~CSetPrintZoneAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void SetPrintZone(const CPrintZone& rPrintZone, std::string& rDesc);

protected:
	CPrintZoneActor* m_pPrintZoneActor;
	CTreeCtrl*       m_pTreeCtrl;
	CPrintZone       m_oldPrintZone;
	CPrintZone       m_newPrintZone;
	std::string      m_oldDesc;
	std::string      m_newDesc;
};
