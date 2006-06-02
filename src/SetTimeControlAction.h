#pragma once

#include "Action.h"

#include "TimeControl2.h"

class CWPhastDoc;

class CSetTimeControlAction :
	public CAction
{
public:
	CSetTimeControlAction(CWPhastDoc* pDoc, const CTimeControl2& newTimeControl);
	virtual ~CSetTimeControlAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*  m_pDoc;
	CTimeControl2 m_originalTimeControl;
	CTimeControl2 m_newTimeControl;
};
