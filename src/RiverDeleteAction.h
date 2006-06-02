#pragma once
#include "Action.h"

class CWPhastDoc;
class CRiverActor;

class CRiverDeleteAction : public CAction
{
public:
	CRiverDeleteAction(CWPhastDoc* pDoc, CRiverActor* pRiverActor);
	~CRiverDeleteAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc   *m_pDoc;
	CRiverActor  *m_pRiverActor;
};
