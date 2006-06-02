#pragma once
#include "ZoneActor.h"
#include "HeadIC.h"
#include "ChemIC.h"

class CPropertyTreeControlBar;
class CUnits;
class CWPhastDoc;

class CICZoneActor : public CZoneActor
{
public:
	vtkTypeRevisionMacro(CICZoneActor,CZoneActor);

	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter = TVI_LAST) = 0;
	virtual void InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent) = 0;
	virtual void Edit(CTreeCtrl* pTreeCtrl) = 0;
	virtual void Add(CWPhastDoc *pWPhastDoc) = 0;
	virtual void Remove(CWPhastDoc *pWPhastDoc) = 0;

protected:
	CICZoneActor(void);
	virtual ~CICZoneActor(void);

public:
	HTREEITEM GetHTreeItem(void)const;
};
