#pragma once

#include "BCZoneActor.h"

class CBCSpecHeadZoneActor : public CBCZoneActor
{
public:
	vtkTypeRevisionMacro(CBCSpecHeadZoneActor,CBCZoneActor);
	static CBCSpecHeadZoneActor *New();


	////virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter = TVI_LAST);
	virtual void InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent);
	virtual void Edit(CTreeCtrl* pTreeCtrl);

	////virtual void Add(CWPhastDoc *pWPhastDoc);
	////virtual void Remove(CWPhastDoc *pWPhastDoc);

protected:
	CBCSpecHeadZoneActor(void);
	virtual ~CBCSpecHeadZoneActor(void);

};
