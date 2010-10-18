#pragma once
#include "ZoneActor.h"
#include "srcinput/Zone_budget.h"
#include "srcWedgeSource.h"

class CPropertyTreeControlBar;
class CUnits;
class CWPhastDoc;

class CZoneFlowRateZoneActor : public CZoneActor
{
public:
	vtkTypeRevisionMacro(CZoneFlowRateZoneActor,CZoneActor);
	static CZoneFlowRateZoneActor *New();
	static void Create(CWPhastDoc* pWPhastDoc, const Zone_budget& zone_budget);

	void SetData(const Zone_budget& zone_budget);
	Zone_budget GetData(void)const;

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
	CZoneFlowRateZoneActor(void);
	virtual ~CZoneFlowRateZoneActor(void);

protected:
// COMMENT: {2/19/2009 9:32:44 PM}	Polyhedron* polyh;
	Zone_budget m_zone_budget;
	static double s_color[3];
	static vtkProperty* s_Property;
	static vtkProperty* s_OutlineProperty;

private:
	CZoneFlowRateZoneActor(const CZoneFlowRateZoneActor&);  // Not implemented.
	void operator=(const CZoneFlowRateZoneActor&);  // Not implemented.

public:
	virtual Polyhedron*& GetPolyhedron(void);
	HTREEITEM GetHTreeItem(void)const;

private:
	class StaticInit
	{
	public:
		StaticInit()
		{
			if (CZoneFlowRateZoneActor::s_Property == 0)
			{
				CZoneFlowRateZoneActor::s_Property = vtkProperty::New();
				CZoneFlowRateZoneActor::s_Property->SetColor(CZoneFlowRateZoneActor::s_color);
				CZoneFlowRateZoneActor::s_Property->SetOpacity(0.3);
			}
			if (CZoneFlowRateZoneActor::s_OutlineProperty == 0)
			{
				CZoneFlowRateZoneActor::s_OutlineProperty = vtkProperty::New();
				CZoneFlowRateZoneActor::s_OutlineProperty->SetAmbient(1.0);
				CZoneFlowRateZoneActor::s_OutlineProperty->SetDiffuse(1.0);
				CZoneFlowRateZoneActor::s_OutlineProperty->SetColor(CZoneFlowRateZoneActor::s_color);
				CZoneFlowRateZoneActor::s_OutlineProperty->SetEdgeColor(CZoneFlowRateZoneActor::s_color);	
				CZoneFlowRateZoneActor::s_OutlineProperty->SetAmbientColor(CZoneFlowRateZoneActor::s_color);
				CZoneFlowRateZoneActor::s_OutlineProperty->SetRepresentationToWireframe();
			}
		}
		~StaticInit()
		{
			if (CZoneFlowRateZoneActor::s_Property != 0)
			{
				CZoneFlowRateZoneActor::s_Property->Delete();
				CZoneFlowRateZoneActor::s_Property = 0;
			}
			if (CZoneFlowRateZoneActor::s_OutlineProperty != 0)
			{
				CZoneFlowRateZoneActor::s_OutlineProperty->Delete();
				CZoneFlowRateZoneActor::s_OutlineProperty = 0;
			}
		}
	};
};
