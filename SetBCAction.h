#pragma once

#include "Action.h"

#include "BC.h"

class CBCZoneActor;

class CSetBCAction : public CAction
{
public:
	CSetBCAction(CBCZoneActor* pBCZoneActor, CTreeCtrl* pTreeCtrl, const CBC& newBC, int nStressPeriod);
	virtual ~CSetBCAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void SetBC(const CBC& rBC);

protected:
	CBCZoneActor*   m_pBCZoneActor;
	CTreeCtrl*      m_pTreeCtrl;
	CBC             m_oldBC;
	CBC             m_newBC;
	int             m_nStressPeriod;
};
