#pragma once
#include "Action.h"

#include "WPhastDoc.h"
#include "WellSchedule.h"
#include "WellActor.h"


class CWellCreateAction : public CAction
{
public:
	CWellCreateAction(CWPhastDoc* pDoc, const CWellSchedule &well);
	~CWellCreateAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc   *m_pDoc;
	CWellActor   *m_pWellActor;
	CWellSchedule m_well;
};
