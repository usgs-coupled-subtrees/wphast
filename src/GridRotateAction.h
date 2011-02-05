#pragma once

#pragma once
#include "Action.h"

#include "GridKeyword.h"

class CWPhastDoc;
class CGridActor;


class CGridRotateAction : public CAction
{
public:
	CGridRotateAction(CWPhastDoc* pDoc, CGridActor* pActor, const CGridKeyword& gridKeyword);
	virtual ~CGridRotateAction(void);

	virtual void Execute();
	virtual void UnExecute();

	void Apply(const CGridKeyword& gridKeyword);

protected:
	CWPhastDoc     *WPhastDoc;
	CGridKeyword    OriginalGridKeyword;
	CGridKeyword    NewGridKeyword;
};
