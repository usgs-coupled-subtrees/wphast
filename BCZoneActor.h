#pragma once
#include "ZoneActor.h"

#include "BC.h"
#include <vector>

class CWPhastDoc;

class CBCZoneActor : public CZoneActor
{
public:
	vtkTypeRevisionMacro(CBCZoneActor,CZoneActor);
	static CBCZoneActor *New();
	static void Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CBC& bc);

	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter = TVI_LAST);
	virtual void InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent);
	virtual void Edit(CTreeCtrl* pTreeCtrl);
	//virtual void Remove(CPropertyTreeControlBar* pTreeControlBar);
	//virtual void UnRemove(CPropertyTreeControlBar* pTreeControlBar);
	virtual void Add(CWPhastDoc *pWPhastDoc);
	virtual void Remove(CWPhastDoc *pWPhastDoc);

	void UpdateTree(CPropertyTreeControlBar* pTreeControlBar);

	void Edit(CTreeCtrl* pTreeCtrl, int nStressPeriod = 1);
	void Serialize(bool bStoring, hid_t loc_id, const CUnits& units);

	static const char szHeading[];

	CString GetTreeHeading(void)const;

	void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, const CBC& crBC);
	void Update(CTreeCtrl* pTreeCtrl);

protected:
	CBCZoneActor(void);
	virtual ~CBCZoneActor(void);

protected:
	CBC m_bc;
// COMMENT: {4/13/2005 5:20:04 PM}	std::vector<HTREEITEM> m_vecHTI;
// COMMENT: {4/13/2005 5:20:04 PM}	std::vector<HTREEITEM> m_vecHParent;
// COMMENT: {4/13/2005 5:20:04 PM}	std::vector<HTREEITEM> m_vecHInsertAfter;

private:
	CBCZoneActor(const CBCZoneActor&);  // Not implemented.
	void operator=(const CBCZoneActor&);  // Not implemented.
public:
	CBC GetBC(void)const;
	void SetBC(const CBC& rBC);

	CBC GetData(void)const;
	void SetData(const CBC& rBC);

	HTREEITEM GetHTreeItem(void)const;
	void SetHTreeItem(HTREEITEM htItem);
};
