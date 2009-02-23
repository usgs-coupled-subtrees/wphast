#pragma once
#include "ZoneActor.h"
#include "GridElt.h"
#include "srcWedgeSource.h"

class CPropertyTreeControlBar;
class CUnits;
class CWPhastDoc;

class CMediaZoneActor : public CZoneActor
{
public:
	vtkTypeRevisionMacro(CMediaZoneActor,CZoneActor);
	static CMediaZoneActor *New();
	static void Create(CWPhastDoc* pWPhastDoc, const CGridElt& gridElt, LPCTSTR desc);

	virtual void InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter);

	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent);
	virtual void Edit(CTreeCtrl* pTreeCtrl);
	virtual void Remove(CPropertyTreeControlBar* pTreeControlBar);
	virtual void UnRemove(CPropertyTreeControlBar* pTreeControlBar);

	virtual void Add(CWPhastDoc *pWPhastDoc);
	virtual void Remove(CWPhastDoc *pWPhastDoc);

	void Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc);

	static const char szHeading[];

	static void SetStaticColor(COLORREF cr);

protected:
	CMediaZoneActor(void);
	virtual ~CMediaZoneActor(void);
protected:
	CGridElt m_grid_elt;
	static vtkFloatingPointType s_color[3];
	static vtkProperty* s_Property;
	static vtkProperty* s_OutlineProperty;

private:
	CMediaZoneActor(const CMediaZoneActor&);  // Not implemented.
	void operator=(const CMediaZoneActor&);  // Not implemented.
public:
	CGridElt GetGridElt(void)const;
	void SetGridElt(const CGridElt& rGridElt);

	CGridElt GetData(void)const;
	void SetData(const CGridElt& rGridElt);

	virtual Polyhedron*& GetPolyhedron(void);

	HTREEITEM GetHTreeItem(void)const;

private:
	class StaticInit
	{
	public:
		StaticInit()
		{
			if (CMediaZoneActor::s_Property == 0)
			{
				CMediaZoneActor::s_Property = vtkProperty::New();
				CMediaZoneActor::s_Property->SetColor(CMediaZoneActor::s_color);
				CMediaZoneActor::s_Property->SetOpacity(0.3);
// COMMENT: {6/30/2008 3:50:21 PM}				CMediaZoneActor::s_Property->SetAmbient(1.0);
// COMMENT: {6/30/2008 3:45:40 PM}				CMediaZoneActor::s_Property->SetDiffuse(1.0);
			}
			if (CMediaZoneActor::s_OutlineProperty == 0)
			{
				CMediaZoneActor::s_OutlineProperty = vtkProperty::New();
				CMediaZoneActor::s_OutlineProperty->SetAmbient(1.0);
				CMediaZoneActor::s_OutlineProperty->SetDiffuse(1.0);
				CMediaZoneActor::s_OutlineProperty->SetColor(CMediaZoneActor::s_color);
				CMediaZoneActor::s_OutlineProperty->SetEdgeColor(CMediaZoneActor::s_color);	
				CMediaZoneActor::s_OutlineProperty->SetAmbientColor(CMediaZoneActor::s_color);
				CMediaZoneActor::s_OutlineProperty->SetRepresentationToWireframe();
			}
		}
		~StaticInit()
		{
			if (CMediaZoneActor::s_Property != 0)
			{
				CMediaZoneActor::s_Property->Delete();
				CMediaZoneActor::s_Property = 0;
			}
			if (CMediaZoneActor::s_OutlineProperty != 0)
			{
				CMediaZoneActor::s_OutlineProperty->Delete();
				CMediaZoneActor::s_OutlineProperty = 0;
			}
		}
	};
};
