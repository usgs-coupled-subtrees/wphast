#pragma once

#include "Action.h"

class CWPhastDoc;
class CGridActor;

class CGridInsertLineAction : public CAction
{
public:
	CGridInsertLineAction(CGridActor *pGridActor, CWPhastDoc *pWPhastDoc, int nGridIndex, int nPlaneIndex, double dValue, bool bSkipFirstExecute = false);
	~CGridInsertLineAction(void);
	virtual void Execute();
	virtual void UnExecute();

protected:
	CGridActor *m_pGridActor;
	CWPhastDoc    *m_pWPhastDoc;
	int            m_nGridIndex;
	int            m_nPlaneIndex;
	double         m_dValue;
	bool           m_bSkipFirstExecute;
};
