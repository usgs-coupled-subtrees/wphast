#pragma once
#include "Action.h"

#include "GridKeyword.h"

class CWPhastDoc;
class CGridActor;


class CGridSetAction : public CAction
{
public:
	CGridSetAction(CWPhastDoc* pDoc, CGridActor* pActor, const CGridKeyword& gridKeyword);
	virtual ~CGridSetAction(void);

	virtual void Execute();
	virtual void UnExecute();

	void Apply(const CGridKeyword& gridKeyword);

protected:
	CWPhastDoc     *m_pDoc;
	CGridActor     *m_pGridActor;
	CGridKeyword    m_origGridKeyword;
	CGridKeyword    m_newGridKeyword;
};
