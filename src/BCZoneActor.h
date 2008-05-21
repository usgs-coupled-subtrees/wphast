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
	static void Create(CWPhastDoc* pWPhastDoc, const CBC& bc, LPCTSTR desc);

	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter = TVI_LAST);
	virtual void InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
	virtual void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent);
	virtual void Edit(CTreeCtrl* pTreeCtrl);

	virtual void Add(CWPhastDoc *pWPhastDoc);
	virtual void Remove(CWPhastDoc *pWPhastDoc);

	void UpdateTree(CPropertyTreeControlBar* pTreeControlBar);

	void Edit(CTreeCtrl* pTreeCtrl, int nStressPeriod = 1);
	void Serialize(bool bStoring, hid_t loc_id, const CUnits& units);


	static const char szHeading[];

	static void SetStaticColor(int bc_type, COLORREF cr); 

	CString GetTreeHeading(void)const;

	void Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, const CBC& crBC);
	void Update(CTreeCtrl* pTreeCtrl);

	void UpdateProperty();

protected:
	CBCZoneActor(void);
	virtual ~CBCZoneActor(void);

protected:
	CBC m_bc;
	static vtkFloatingPointType s_color[3][3];
	static vtkProperty* s_Property[3];
	static vtkProperty* s_OutlineProperty[3];

private:
	CBCZoneActor(const CBCZoneActor&);  // Not implemented.
	void operator=(const CBCZoneActor&);  // Not implemented.

public:
	CBC GetBC(void)const;
	void SetBC(const CBC& rBC);

	CBC GetData(void)const;
	void SetData(const CBC& rBC);

	virtual Polyhedron*& GetPolyhedron(void);

	HTREEITEM GetHTreeItem(void)const;
	void SetHTreeItem(HTREEITEM htItem);

private:
	class StaticInit
	{
	public:
		StaticInit()
		{
			for (int i = 0; i < 3; ++i)
			{
				if (CBCZoneActor::s_Property[i] == 0)
				{
					CBCZoneActor::s_Property[i] = vtkProperty::New();
					CBCZoneActor::s_Property[i]->SetColor(CBCZoneActor::s_color[i]);
					CBCZoneActor::s_Property[i]->SetOpacity(0.3);
				}
				if (CBCZoneActor::s_OutlineProperty[i] == 0)
				{
					CBCZoneActor::s_OutlineProperty[i] = vtkProperty::New();
					CBCZoneActor::s_OutlineProperty[i]->SetAmbient(1.0);
					CBCZoneActor::s_OutlineProperty[i]->SetColor(CBCZoneActor::s_color[i]);
					CBCZoneActor::s_OutlineProperty[i]->SetEdgeColor(CBCZoneActor::s_color[i]);	
					CBCZoneActor::s_OutlineProperty[i]->SetAmbientColor(CBCZoneActor::s_color[i]);
					CBCZoneActor::s_OutlineProperty[i]->SetRepresentationToWireframe();
				}

			}
		}
		~StaticInit()
		{
			for (int i = 0; i < 3; ++i)
			{
				if (CBCZoneActor::s_Property[i] != 0)
				{
					CBCZoneActor::s_Property[i]->Delete();
					CBCZoneActor::s_Property[i] = 0;
				}
				if (CBCZoneActor::s_OutlineProperty[i] != 0)
				{
					CBCZoneActor::s_OutlineProperty[i]->Delete();
					CBCZoneActor::s_OutlineProperty[i] = 0;
				}
			}
		}
	};
};
