#pragma once

#include "Action.h"
#include "GridLineMoveMemento.h"

class CWPhastDoc;
class CGridLODActor;

class CGridMoveLineAction : public CAction
{
public:
	CGridMoveLineAction(CGridLODActor *pGridLODActor, CWPhastDoc *pWPhastDoc, GridLineMoveMemento memento, bool bSkipFirstExecute = false);
	~CGridMoveLineAction(void);
	virtual void Execute();
	virtual void UnExecute();

protected:
	CGridLODActor       *m_pGridLODActor;
	CWPhastDoc          *m_pWPhastDoc;
	GridLineMoveMemento  m_memento;
	//int            m_nGridIndex;
	//int            m_nPlaneIndex;
	//double         m_dValue;
	bool           m_bSkipFirstExecute;
};
