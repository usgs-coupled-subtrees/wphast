#pragma once

#include "Action.h"

#include "TimeControl.h"

class CWPhastDoc;

class CSetTimeControlAction :
	public CAction
{
public:
	CSetTimeControlAction(CWPhastDoc* pDoc, const CTimeControl& newTimeControl, int nStressPeriod);
	virtual ~CSetTimeControlAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*  m_pDoc;
	CTimeControl m_originalTimeControl;
	CTimeControl m_newTimeControl;
	int          m_nStressPeriod;
};
