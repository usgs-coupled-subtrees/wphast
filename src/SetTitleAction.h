#pragma once

#include "Action.h"

#include "Title.h"

class CWPhastDoc;


class CSetTitleAction : public CAction
{
public:
	CSetTitleAction(CWPhastDoc* pDoc, const CTitle& t);
	~CSetTitleAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc*  WPhastDoc;
	CTitle       OriginalTitle;
	CTitle       NewTitle;
};
