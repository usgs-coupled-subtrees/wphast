#pragma once
#include "ICZoneActor.h"

class CICHeadZoneActor : public CICZoneActor
{
public:
	vtkTypeRevisionMacro(CICHeadZoneActor,CICZoneActor);
	static CICHeadZoneActor *New();
	static void Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CHeadIC& headIC, LPCTSTR desc);

	CHeadIC GetData(void)const;
	void SetData(const CHeadIC& rHeadIC);

	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter = TVI_LAST);
	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent);
	virtual void Edit(CTreeCtrl* pTreeCtrl);
	virtual void Add(CWPhastDoc *pWPhastDoc);
	virtual void Remove(CWPhastDoc *pWPhastDoc);

	static const char szHeading[];

	static void SetStaticColor(COLORREF cr);

	void Serialize(bool bStoring, hid_t loc_id, const CUnits& units);

protected:
	CICHeadZoneActor(void);
	virtual ~CICHeadZoneActor(void);
	CHeadIC m_headIC;
	static vtkFloatingPointType s_color[3];
	static vtkProperty* s_Property;

private:
	CICHeadZoneActor(const CICHeadZoneActor&);  // Not implemented.
	void operator=(const CICHeadZoneActor&);  // Not implemented.
private:
	class StaticInit
	{
	public:
		StaticInit() {
			if (CICHeadZoneActor::s_Property == 0) {
				CICHeadZoneActor::s_Property = vtkProperty::New();
				CICHeadZoneActor::s_Property->SetColor(CICHeadZoneActor::s_color);
				CICHeadZoneActor::s_Property->SetOpacity(0.3);
			}
		}
		~StaticInit() {
			if (CICHeadZoneActor::s_Property != 0) {
				CICHeadZoneActor::s_Property->Delete();
				CICHeadZoneActor::s_Property = 0;
			}
		}
	};
};
