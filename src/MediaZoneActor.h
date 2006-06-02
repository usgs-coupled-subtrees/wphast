#pragma once
#include "ZoneActor.h"
#include "GridElt.h"

class CPropertyTreeControlBar;
class CUnits;
class CWPhastDoc;

class CMediaZoneActor : public CZoneActor
{
public:
	vtkTypeRevisionMacro(CMediaZoneActor,CZoneActor);
	static CMediaZoneActor *New();
	static void Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CGridElt& gridElt, LPCTSTR desc);

	virtual void InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter);

	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent);
	virtual void Edit(CTreeCtrl* pTreeCtrl);
	virtual void Remove(CPropertyTreeControlBar* pTreeControlBar);
	virtual void UnRemove(CPropertyTreeControlBar* pTreeControlBar);

	virtual void Add(CWPhastDoc *pWPhastDoc);
	virtual void Remove(CWPhastDoc *pWPhastDoc);

	void Serialize(bool bStoring, hid_t loc_id, const CUnits& units);

	static const char szHeading[];

protected:
	CMediaZoneActor(void);
	virtual ~CMediaZoneActor(void);
protected:
	CGridElt m_grid_elt;

private:
	CMediaZoneActor(const CMediaZoneActor&);  // Not implemented.
	void operator=(const CMediaZoneActor&);  // Not implemented.
public:
	CGridElt GetGridElt(void)const;
	////void SetGridElt(const CGridElt& rGridElt, const CUnits& rUnits);
	void SetGridElt(const CGridElt& rGridElt);

	CGridElt GetData(void)const;
	void SetData(const CGridElt& rGridElt);	

	HTREEITEM GetHTreeItem(void)const;
};
