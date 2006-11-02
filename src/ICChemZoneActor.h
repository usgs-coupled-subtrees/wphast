#pragma once
#include "ICZoneActor.h"

class CICChemZoneActor : public CICZoneActor
{
public:
	vtkTypeRevisionMacro(CICChemZoneActor,CICZoneActor);
	static CICChemZoneActor *New();
	static void Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CChemIC& chemIC, LPCTSTR desc);

	CChemIC GetData(void)const;
	void SetData(const CChemIC& rChemIC);

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

private:
	CICChemZoneActor(const CICChemZoneActor&);  // Not implemented.
	void operator=(const CICChemZoneActor&);  // Not implemented.
private:
	class StaticInit
	{
	public:
		StaticInit() {
			if (CICChemZoneActor::s_Property == 0) {
				CICChemZoneActor::s_Property = vtkProperty::New();
				CICChemZoneActor::s_Property->SetColor(CICChemZoneActor::s_color);
				CICChemZoneActor::s_Property->SetOpacity(0.3);
			}
		}
		~StaticInit() {
			if (CICChemZoneActor::s_Property != 0) {
				CICChemZoneActor::s_Property->Delete();
				CICChemZoneActor::s_Property = 0;
			}
		}
	};
};
