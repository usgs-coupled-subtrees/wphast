#pragma once
#include "Action.h"

#include "GridKeyword.h"

class CWPhastDoc;
class CGridLODActor;


class CGridSetAction : public CAction
{
public:
	CGridSetAction(CWPhastDoc* pDoc, CGridLODActor* pActor, const CGridKeyword& gridKeyword);
	virtual ~CGridSetAction(void);

	virtual void Execute();
	virtual void UnExecute();

	void Apply(const CGridKeyword& gridKeyword);

protected:
	CWPhastDoc     *m_pDoc;
	CGridLODActor  *m_pGridLODActor;
	CGridKeyword    m_origGridKeyword;
	CGridKeyword    m_newGridKeyword;
};
