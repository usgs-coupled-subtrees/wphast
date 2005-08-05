#pragma once

#include "Action.h"

#include <vtkIdType.h>

class CWPhastDoc;
class CGridLODActor;

class CGridDeleteLineAction : public CAction
{
public:
	CGridDeleteLineAction(CGridLODActor *pGridLODActor, CWPhastDoc *pWPhastDoc, int nGridIndex, int nPlaneIndex, double dValue, bool bSkipFirstExecute = false);
	~CGridDeleteLineAction(void);
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
