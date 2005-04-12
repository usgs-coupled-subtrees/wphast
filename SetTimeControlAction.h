#pragma once

#include "Action.h"

#include "TimeControl.h"

class CWPhastDoc;

class CSetTimeControlAction :
	public CAction
{
public:
// COMMENT: {4/11/2005 1:47:15 PM}	CSetTimeControlAction(CWPhastDoc* pDoc, const CTimeControl& newTimeControl, int nStressPeriod);
	CSetTimeControlAction(CWPhastDoc* pDoc, const CTimeControl& newTimeControl);
	virtual ~CSetTimeControlAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*  m_pDoc;
	CTimeControl m_originalTimeControl;
	CTimeControl m_newTimeControl;
// COMMENT: {4/11/2005 1:47:23 PM}	int          m_nStressPeriod;
};
