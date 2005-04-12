#pragma once

#include "Action.h"

#include "PrintFreq.h"

class CWPhastDoc;

class CSetPrintFreqAction :
	public CAction
{
public:
// COMMENT: {4/11/2005 1:41:01 PM}	CSetPrintFreqAction(CWPhastDoc* pDoc, const CPrintFreq& newPrintFreq, int nStressPeriod);
	CSetPrintFreqAction(CWPhastDoc* pDoc, const CPrintFreq& newPrintFreq);
	~CSetPrintFreqAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*  m_pDoc;
	CPrintFreq   m_oriPrintFreq;
	CPrintFreq   m_newPrintFreq;
// COMMENT: {4/11/2005 1:40:53 PM}	int          m_nStressPeriod;
};
