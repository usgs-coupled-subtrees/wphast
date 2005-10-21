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
	static CICZoneActor *New();
	static void Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CHeadIC& headIC);
	static void Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CChemIC& chemIC);
	enum IC_TYPE {
		IC_UNDEF,
		IC_HEAD,
		IC_CHEM
	};

	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter = TVI_LAST);
	virtual void InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent);
	virtual void Edit(CTreeCtrl* pTreeCtrl);
	virtual LPCTSTR GetSerialName(void); // const;
	virtual void SetSerialName(LPCTSTR name);
	virtual void Add(CWPhastDoc *pWPhastDoc);
	virtual void Remove(CWPhastDoc *pWPhastDoc);

	void Serialize(bool bStoring, hid_t loc_id, const CUnits& units);

	CICZoneActor::IC_TYPE GetType(void)const;

protected:
	CICZoneActor(void);
	virtual ~CICZoneActor(void);

protected:
	IC_TYPE m_type;
	CHeadIC m_headIC;
	CChemIC m_chemIC;
	std::string m_serialName;

private:
	CICZoneActor(const CICZoneActor&);  // Not implemented.
	void operator=(const CICZoneActor&);  // Not implemented.
public:
	CHeadIC GetHeadIC(void)const;
	void SetHeadIC(const CHeadIC& rHeadIC);

	CChemIC GetChemIC(void)const;
	void SetChemIC(const CChemIC& chemIC);

	HTREEITEM GetHTreeItem(void)const;
};
