#pragma once

#include "Action.h"

#include "HeadIC.h"

class CICZoneActor;

class CSetHeadICAction :
	public CAction
{
public:
	CSetHeadICAction(CICZoneActor* pICZoneActor, CTreeCtrl* pTreeCtrl, const CHeadIC& newHeadIC);
	virtual ~CSetHeadICAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void SetHeadIC(const CHeadIC& rHeadIC);

protected:
	CICZoneActor* m_pICZoneActor;
	CTreeCtrl*    m_pTreeCtrl;
	CHeadIC       m_oldHeadIC;
	CHeadIC       m_newHeadIC;
};
