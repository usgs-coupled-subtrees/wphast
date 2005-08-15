#pragma once

#include "Action.h"
#include "GridLineMoveMemento.h"

class CWPhastDoc;
class CGridActor;

class CGridMoveLineAction : public CAction
{
public:
	CGridMoveLineAction(CGridActor *pGridActor, CWPhastDoc *pWPhastDoc, GridLineMoveMemento memento, bool bSkipFirstExecute = false);
	~CGridMoveLineAction(void);
	virtual void Execute();
	virtual void UnExecute();

protected:
	CGridActor          *m_pGridActor;
	CWPhastDoc          *m_pWPhastDoc;
	GridLineMoveMemento  m_memento;
	bool                 m_bSkipFirstExecute;
};
