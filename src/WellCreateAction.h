#pragma once
#include "Action.h"

#include "Tree.h"
#include "WellSchedule.h"

class CWPhastDoc;
class CWellActor;

class CWellCreateAction : public CAction
{
public:
	CWellCreateAction(CWPhastDoc* pDoc, const CWellSchedule &well, CTreeCtrlNode nodeInsertAfter = CTreeCtrlNode(TVI_LAST, NULL));
	~CWellCreateAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc   *m_pDoc;
	CWellActor   *m_pWellActor;
	CWellSchedule m_well;

	HTREEITEM m_hInsertAfter;
	DWORD_PTR m_dwInsertAfter;
	CTreeCtrlNode m_nodeParent;
};
