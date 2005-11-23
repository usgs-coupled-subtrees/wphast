#pragma once

#include "Action.h"

#include "HeadIC.h"

class CICHeadZoneActor;

class CSetHeadICAction : public CAction
{
public:
	CSetHeadICAction(CICHeadZoneActor* pICHeadZoneActor, CTreeCtrl* pTreeCtrl, const CHeadIC& newHeadIC);
	virtual ~CSetHeadICAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void SetData(const CHeadIC& rHeadIC);

protected:
	CICHeadZoneActor* m_pICHeadZoneActor;
	CTreeCtrl*        m_pTreeCtrl;
	CHeadIC           m_oldHeadIC;
	CHeadIC           m_newHeadIC;
};
