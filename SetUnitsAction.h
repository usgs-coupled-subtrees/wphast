#pragma once
#include "Action.h"

#include "Units.h"

class CWPhastDoc;

class CSetUnitsAction :
	public CAction
{
public:
	CSetUnitsAction(CWPhastDoc* pDoc, const CUnits& newUnits);
	virtual ~CSetUnitsAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc* m_pDoc;
	CUnits      m_originalUnits;
	CUnits      m_newUnits;
};
