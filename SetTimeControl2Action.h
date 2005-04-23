#pragma once

#include "Action.h"

#include "TimeControl2.h"

class CWPhastDoc;

class CSetTimeControl2Action :
	public CAction
{
public:
	CSetTimeControl2Action(CWPhastDoc* pDoc, const CTimeControl2& newTimeControl2);
	virtual ~CSetTimeControl2Action(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*     m_pDoc;
	CTimeControl2   m_originalTimeControl2;
	CTimeControl2   m_newTimeControl2;
};
