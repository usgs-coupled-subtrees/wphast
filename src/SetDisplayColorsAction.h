#pragma once
#include "Action.h"
#include "DisplayColors.h"

class CWPhastDoc;

class CSetDisplayColorsAction :	public CAction
{
public:
	CSetDisplayColorsAction(CWPhastDoc* pDoc, const CDisplayColors& dc);
	~CSetDisplayColorsAction(void);

	virtual void Execute();
	virtual void UnExecute();
	void Apply(const CDisplayColors& dc);
	void Cancel();

protected:
	CWPhastDoc* WPhastDoc;
	CDisplayColors DisplayColors;
	CDisplayColors OriginalDisplayColors;
};
