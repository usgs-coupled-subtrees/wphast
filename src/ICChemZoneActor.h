#pragma once
#include "ICZoneActor.h"

class CICChemZoneActor : public CICZoneActor
{
public:
	vtkTypeRevisionMacro(CICChemZoneActor,CICZoneActor);
	static CICChemZoneActor *New();
	static void Create(CWPhastDoc* pWPhastDoc, const CChemIC& chemIC, LPCTSTR desc);

	CChemIC GetData(void)const;
	void SetData(const CChemIC& rChemIC);
	Polyhedron*& GetPolyhedron(void);

	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter = TVI_LAST);
	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent);
	virtual void Edit(CTreeCtrl* pTreeCtrl);
	virtual void Add(CWPhastDoc *pWPhastDoc);
	virtual void Remove(CWPhastDoc *pWPhastDoc);

	static const char szHeading[];

	static void SetStaticColor(COLORREF cr);

	void Serialize(bool bStoring, hid_t loc_id, const CUnits& units);

protected:
	CICChemZoneActor(void);
	virtual ~CICChemZoneActor(void);
	CChemIC m_chemIC;
	static vtkFloatingPointType s_color[3];
	static vtkProperty* s_Property;
	static vtkProperty* s_OutlineProperty;

private:
	CICChemZoneActor(const CICChemZoneActor&);  // Not implemented.
	void operator=(const CICChemZoneActor&);  // Not implemented.
private:
	class StaticInit
	{
	public:
		StaticInit() {
			if (CICChemZoneActor::s_Property == 0)
			{
				CICChemZoneActor::s_Property = vtkProperty::New();
				CICChemZoneActor::s_Property->SetColor(CICChemZoneActor::s_color);
				CICChemZoneActor::s_Property->SetOpacity(0.3);
			}
			if (CICChemZoneActor::s_OutlineProperty == 0)
			{
				CICChemZoneActor::s_OutlineProperty = vtkProperty::New();
				CICChemZoneActor::s_OutlineProperty->SetAmbient(1.0);
				CICChemZoneActor::s_OutlineProperty->SetColor(CICChemZoneActor::s_color);
				CICChemZoneActor::s_OutlineProperty->SetEdgeColor(CICChemZoneActor::s_color);	
				CICChemZoneActor::s_OutlineProperty->SetAmbientColor(CICChemZoneActor::s_color);
				CICChemZoneActor::s_OutlineProperty->SetRepresentationToWireframe();
			}
		}
		~StaticInit()
		{
			if (CICChemZoneActor::s_Property != 0)
			{
				CICChemZoneActor::s_Property->Delete();
				CICChemZoneActor::s_Property = 0;
			}
			if (CICChemZoneActor::s_OutlineProperty != 0)
			{
				CICChemZoneActor::s_OutlineProperty->Delete();
				CICChemZoneActor::s_OutlineProperty = 0;
			}
		}
	};
};
