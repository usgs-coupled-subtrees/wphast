#pragma once
#include "Action.h"

#include "WPhastDoc.h"
#include "WellActor.h"


class CWellDeleteAction : public CAction
{
public:
	CWellDeleteAction(CWPhastDoc* pDoc, CWellActor* pWellActor);
	~CWellDeleteAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc   *m_pDoc;
	CWellActor   *m_pWellActor;
	// Maybe add some kind of class based on the Memento design pattern
};
