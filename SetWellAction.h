#pragma once
#include "Action.h"

#include "WellSchedule.h"

class CWellActor;
class CWPhastDoc;

class CSetWellAction : public CAction
{
public:
	CSetWellAction(CWellActor *pWellActor, const CWellSchedule& newWell, CWPhastDoc *pWPhastDoc);
	virtual ~CSetWellAction(void);

	virtual void Execute(void);
	virtual void UnExecute(void);

protected:
	void SetWell(const CWellSchedule &well);

protected:
	CWellActor    *m_pWellActor;
	CWellSchedule  m_oldWell;
	CWellSchedule  m_newWell;
	CWPhastDoc    *m_pWPhastDoc;
};
