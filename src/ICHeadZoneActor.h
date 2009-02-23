#pragma once
#include "ICZoneActor.h"

class CICHeadZoneActor : public CICZoneActor
{
public:
	vtkTypeRevisionMacro(CICHeadZoneActor,CICZoneActor);
	static CICHeadZoneActor *New();
	static void Create(CWPhastDoc* pWPhastDoc, const CHeadIC& headIC, LPCTSTR desc);

	CHeadIC GetData(void)const;
	void SetData(const CHeadIC& rHeadIC);
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
	CICHeadZoneActor(void);
	virtual ~CICHeadZoneActor(void);
	CHeadIC m_headIC;
	static vtkFloatingPointType s_color[3];
	static vtkProperty* s_Property;
	static vtkProperty* s_OutlineProperty;	

private:
	CICHeadZoneActor(const CICHeadZoneActor&);  // Not implemented.
	void operator=(const CICHeadZoneActor&);  // Not implemented.
private:
	class StaticInit
	{
	public:
		StaticInit()
		{
			if (CICHeadZoneActor::s_Property == 0)
			{
				CICHeadZoneActor::s_Property = vtkProperty::New();
				CICHeadZoneActor::s_Property->SetColor(CICHeadZoneActor::s_color);
				CICHeadZoneActor::s_Property->SetOpacity(0.3);
// COMMENT: {6/30/2008 3:52:59 PM}				CICHeadZoneActor::s_Property->SetAmbient(1.0);
// COMMENT: {6/30/2008 3:52:59 PM}				CICHeadZoneActor::s_Property->SetDiffuse(1.0);
			}
			if (CICHeadZoneActor::s_OutlineProperty == 0)
			{
				CICHeadZoneActor::s_OutlineProperty = vtkProperty::New();
				CICHeadZoneActor::s_OutlineProperty->SetAmbient(1.0);
				CICHeadZoneActor::s_OutlineProperty->SetDiffuse(1.0);
				CICHeadZoneActor::s_OutlineProperty->SetColor(CICHeadZoneActor::s_color);
				CICHeadZoneActor::s_OutlineProperty->SetEdgeColor(CICHeadZoneActor::s_color);	
				CICHeadZoneActor::s_OutlineProperty->SetAmbientColor(CICHeadZoneActor::s_color);
				CICHeadZoneActor::s_OutlineProperty->SetRepresentationToWireframe();
			}
		}
		~StaticInit()
		{
			if (CICHeadZoneActor::s_Property != 0)
			{
				CICHeadZoneActor::s_Property->Delete();
				CICHeadZoneActor::s_Property = 0;
			}
			if (CICHeadZoneActor::s_OutlineProperty != 0)
			{
				CICHeadZoneActor::s_OutlineProperty->Delete();
				CICHeadZoneActor::s_OutlineProperty = 0;
			}
		}
	};
};
