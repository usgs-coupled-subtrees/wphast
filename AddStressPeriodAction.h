#pragma once
#include "Action.h"

#include "TimeControl.h"
class CWPhastDoc;

class CAddStressPeriodAction : public CAction
{
public:
	CAddStressPeriodAction(CWPhastDoc* pDoc, const CTimeControl& tc);
	~CAddStressPeriodAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*  m_pDoc;
	CTimeControl m_tc;
	int m_nStressPeriod;
};
