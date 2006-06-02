#pragma once

#include "Action.h"

#include "PrintInput.h"

class CWPhastDoc;

class CSetPrintInputAction : public CAction
{
public:
	CSetPrintInputAction(CWPhastDoc* pDoc, const CPrintInput& newPrintInput);
	~CSetPrintInputAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*  m_pDoc;
	CPrintInput  m_oriPrintInput;
	CPrintInput  m_newPrintInput;
};
