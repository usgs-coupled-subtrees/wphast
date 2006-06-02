#pragma once

#include "Action.h"

#include "BC.h"

class CBCZoneActor;

class CSetBCAction : public CAction
{
public:
	CSetBCAction(CBCZoneActor* pBCZoneActor, CTreeCtrl* pTreeCtrl, const CBC& newBC, LPCTSTR desc);
	virtual ~CSetBCAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void SetBC(const CBC& rBC, std::string& rDesc);

protected:
	CBCZoneActor*   m_pBCZoneActor;
	CTreeCtrl*      m_pTreeCtrl;
	CBC             m_oldBC;
	CBC             m_newBC;
	std::string     m_oldDesc;
	std::string     m_newDesc;
};
