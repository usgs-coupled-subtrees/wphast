#pragma once

#include "Action.h"

#include "PrintFreq.h"

class CWPhastDoc;

class CSetPrintFreqAction :
	public CAction
{
public:
	CSetPrintFreqAction(CWPhastDoc* pDoc, const CPrintFreq& newPrintFreq, int nStressPeriod);
	~CSetPrintFreqAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*  m_pDoc;
	CPrintFreq   m_oriPrintFreq;
	CPrintFreq   m_newPrintFreq;
	int          m_nStressPeriod;
};
