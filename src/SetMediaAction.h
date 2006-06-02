#pragma once

#include "Action.h"

#include "GridElt.h"

class CMediaZoneActor;

class CSetMediaAction : public CAction
{
public:
	CSetMediaAction(CMediaZoneActor* pMediaZoneActor, CTreeCtrl* pTreeCtrl, const CGridElt& newGridElt, LPCTSTR desc);
	virtual ~CSetMediaAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void SetGridElt(const CGridElt& rGridElt, std::string& rDesc);

protected:
	CMediaZoneActor* m_pMediaZoneActor;
	CTreeCtrl*       m_pTreeCtrl;
	CGridElt         m_oldGridElt;
	CGridElt         m_newGridElt;
	std::string      m_oldDesc;
	std::string      m_newDesc;
};
