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

	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar);
	virtual void InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent);
	virtual void Edit(CTreeCtrl* pTreeCtrl);
	virtual void Remove(CPropertyTreeControlBar* pTreeControlBar);
	virtual void UnRemove(CPropertyTreeControlBar* pTreeControlBar);
	virtual void Add(CWPhastDoc *pWPhastDoc);
	virtual void Remove(CWPhastDoc *pWPhastDoc);

	void UpdateTree(CPropertyTreeControlBar* pTreeControlBar, int nStressPeriod);

	void Edit(CTreeCtrl* pTreeCtrl, int nStressPeriod = 1);
	void Serialize(bool bStoring, hid_t loc_id, const CUnits& units);
	void SerializeStressPeriod(int nStressPeriod, bool bStoring, hid_t loc_id);

	void AddStressPeriod(CPropertyTreeControlBar* pTreeControlBar, int nStressPeriod);
	void RemoveStressPeriod(CPropertyTreeControlBar* pTreeControlBar, int nStressPeriod);

	CString GetTreeHeading(void)const;

	void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, const CBC& crBC);
	void Update(CTreeCtrl* pTreeCtrl, int nStressPeriod);
	int GetStressPeriodCount(void)const;

protected:
	CBCZoneActor(void);
	virtual ~CBCZoneActor(void);
	//void ReInsertStressPeriod(CPropertyTreeControlBar* pTreeControlBar, int nStressPeriod);

protected:
	CBC m_bc;
	std::vector<CBC>       m_vecBC;
	std::vector<HTREEITEM> m_vecHTI;
	std::vector<HTREEITEM> m_vecHParent;
	std::vector<HTREEITEM> m_vecHInsertAfter;

private:
	CBCZoneActor(const CBCZoneActor&);  // Not implemented.
	void operator=(const CBCZoneActor&);  // Not implemented.
public:
	CBC GetBC(int nStressPeriod = 1)const;
	void SetBC(const CBC& rBC, int nStressPeriod = 1);

	HTREEITEM GetHTreeItem(int nStressPeriod)const;
	void SetHTreeItem(HTREEITEM htItem, int nStressPeriod = 1);
};
