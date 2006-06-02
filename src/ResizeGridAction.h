#pragma once
#include "Action.h"

class CWPhastDoc;
#include "GridLODActor.h"

class CResizeGridAction : public CAction
{
public:
	CResizeGridAction(CWPhastDoc* pDoc, CGrid* grids, CGridLODActor* pActor = 0);
	virtual ~CResizeGridAction(void);

	void Reset(CGrid* grids);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void Store(CGridLODActor* pActor);
	CWPhastDoc* m_pDoc;
	CGrid m_Grid[3];
	CGrid m_OriginalGrid[3];
};
