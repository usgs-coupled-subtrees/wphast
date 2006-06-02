#pragma once
#include "Action.h"

#include "WPhastDoc.h"
#include "River.h"
#include "RiverActor.h"

class CRiverCreateAction : public CAction
{
public:
	CRiverCreateAction(CWPhastDoc* pDoc, const CRiver &river, CTreeCtrlNode nodeInsertAfter = CTreeCtrlNode(TVI_LAST, NULL));
	CRiverCreateAction(CWPhastDoc* pDoc, CRiverActor *pRiverActor, CTreeCtrlNode nodeInsertAfter = CTreeCtrlNode(TVI_LAST, NULL));
	~CRiverCreateAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc  *m_pDoc;
	CRiverActor *m_pRiverActor;
	CRiver       m_river;

	HTREEITEM m_hInsertAfter;
	DWORD_PTR m_dwInsertAfter;
	CTreeCtrlNode m_nodeParent;
};
