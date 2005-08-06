#pragma once

#include "Action.h"

class CWPhastDoc;
class CGridLODActor;

class CGridInsertLineAction : public CAction
{
public:
	CGridInsertLineAction(CGridLODActor *pGridLODActor, CWPhastDoc *pWPhastDoc, int nGridIndex, int nPlaneIndex, double dValue, bool bSkipFirstExecute = false);
	~CGridInsertLineAction(void);
	virtual void Execute();
	virtual void UnExecute();

protected:
	CGridLODActor *m_pGridLODActor;
	CWPhastDoc    *m_pWPhastDoc;
	int            m_nGridIndex;
	int            m_nPlaneIndex;
	double         m_dValue;
	bool           m_bSkipFirstExecute;
};
