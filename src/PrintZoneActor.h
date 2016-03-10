#pragma once
#include "ZoneActor.h"
// COMMENT: {2/2/2016 5:41:42 PM}#include "srcinput/Zone_budget.h"
#include "PrintZone.h"
#include "srcWedgeSource.h"

class CPropertyTreeControlBar;
class CUnits;
class CWPhastDoc;

class CPrintZoneActor : public CZoneActor
{
public:
	vtkTypeRevisionMacro(CPrintZoneActor,CZoneActor);
	static CPrintZoneActor *New();
// COMMENT: {2/2/2016 5:55:43 PM}	static void Create(CWPhastDoc* pWPhastDoc, const Zone_budget& zone_budget);
	static void Create(CWPhastDoc* pWPhastDoc, const CPrintZone& printZone);

// COMMENT: {2/2/2016 5:56:15 PM}	void SetData(const Zone_budget& zone_budget);
// COMMENT: {2/2/2016 5:56:15 PM}	Zone_budget GetData(void)const;
	void SetData(const CPrintZone& printZone);
	CPrintZone GetData(void)const;

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
	CPrintZoneActor(void);
	virtual ~CPrintZoneActor(void);

protected:
// COMMENT: {2/19/2009 9:32:44 PM}	Polyhedron* polyh;
// COMMENT: {2/2/2016 5:53:15 PM}	Zone_budget m_zone_budget;
	CPrintZone m_printZone;
	static double s_color[3];
	static vtkProperty* s_Property;
	static vtkProperty* s_OutlineProperty;

private:
	CPrintZoneActor(const CPrintZoneActor&);  // Not implemented.
	void operator=(const CPrintZoneActor&);  // Not implemented.

public:
	virtual Polyhedron*& GetPolyhedron(void);
	HTREEITEM GetHTreeItem(void)const;

private:
	class StaticInit
	{
	public:
		StaticInit()
		{
			if (CPrintZoneActor::s_Property == 0)
			{
				CPrintZoneActor::s_Property = vtkProperty::New();
				CPrintZoneActor::s_Property->SetColor(CPrintZoneActor::s_color);
				CPrintZoneActor::s_Property->SetOpacity(0.1);
			}
			if (CPrintZoneActor::s_OutlineProperty == 0)
			{
				CPrintZoneActor::s_OutlineProperty = vtkProperty::New();
				CPrintZoneActor::s_OutlineProperty->SetAmbient(1.0);
				CPrintZoneActor::s_OutlineProperty->SetDiffuse(1.0);
				CPrintZoneActor::s_OutlineProperty->SetColor(CPrintZoneActor::s_color);
				CPrintZoneActor::s_OutlineProperty->SetEdgeColor(CPrintZoneActor::s_color);	
				CPrintZoneActor::s_OutlineProperty->SetAmbientColor(CPrintZoneActor::s_color);
				CPrintZoneActor::s_OutlineProperty->SetRepresentationToWireframe();
			}
		}
		~StaticInit()
		{
			if (CPrintZoneActor::s_Property != 0)
			{
				CPrintZoneActor::s_Property->Delete();
				CPrintZoneActor::s_Property = 0;
			}
			if (CPrintZoneActor::s_OutlineProperty != 0)
			{
				CPrintZoneActor::s_OutlineProperty->Delete();
				CPrintZoneActor::s_OutlineProperty = 0;
			}
		}
	};
};
