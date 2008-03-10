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

	static void SetStaticColor(COLORREF cr);

protected:
	CMediaZoneActor(void);
	virtual ~CMediaZoneActor(void);
protected:
	CGridElt m_grid_elt;
	static vtkFloatingPointType s_color[3];
	static vtkProperty* s_Property;

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

private:
	class StaticInit
	{
	public:
		StaticInit() {
			if (CMediaZoneActor::s_Property == 0) {
				CMediaZoneActor::s_Property = vtkProperty::New();
				CMediaZoneActor::s_Property->SetColor(CMediaZoneActor::s_color);
				CMediaZoneActor::s_Property->SetOpacity(0.3);
// COMMENT: {2/27/2008 8:14:45 PM}#ifdef USE_WEDGE
// COMMENT: {2/27/2008 8:14:45 PM}				CMediaZoneActor::s_Property->SetRepresentationToWireframe();
// COMMENT: {2/27/2008 8:14:45 PM}				CMediaZoneActor::s_Property->SetColor(1.f, 1.f, 1.f);
// COMMENT: {2/27/2008 8:14:45 PM}				CMediaZoneActor::s_Property->SetOpacity(1.0);
// COMMENT: {2/27/2008 8:14:45 PM}#endif
			}
		}
		~StaticInit() {
			if (CMediaZoneActor::s_Property != 0) {
				CMediaZoneActor::s_Property->Delete();
				CMediaZoneActor::s_Property = 0;
			}
		}
	};
};
