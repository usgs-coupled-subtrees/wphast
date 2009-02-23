#include "StdAfx.h"
#include "ICHeadZoneActor.h"

#include "resource.h"
#include "PropertyTreeControlBar.h"
#include "ICHeadSpreadPropertyPage.h"

#include "property.h"
#include "Global.h"

#include "WPhastDoc.h"
#include "SetHeadICAction.h"
#include "ZoneCreateAction.h"
#include "FlowOnly.h"
#include "Protect.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
#include <vtkPropAssembly.h>

vtkCxxRevisionMacro(CICHeadZoneActor, "$Revision$");
vtkStandardNewMacro(CICHeadZoneActor);

const char CICHeadZoneActor::szHeading[] = "ICHead";
vtkFloatingPointType CICHeadZoneActor::s_color[3] = {0., 0., 0};
vtkProperty* CICHeadZoneActor::s_Property = 0;
vtkProperty* CICHeadZoneActor::s_OutlineProperty = 0;

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CICHeadZoneActor::CICHeadZoneActor(void)
{
	static StaticInit init; // constructs/destructs s_Property/s_OutlineProperty

	// cube
	this->CubeActor->SetProperty(CICHeadZoneActor::s_Property);

	// outline
	this->OutlineActor->SetProperty(CICHeadZoneActor::s_OutlineProperty);
}

CICHeadZoneActor::~CICHeadZoneActor(void)
{
}

void CICHeadZoneActor::Create(CWPhastDoc* pWPhastDoc, const CHeadIC& headIC, LPCTSTR desc)
{
	CZoneCreateAction<CICHeadZoneActor>* pAction = new CZoneCreateAction<CICHeadZoneActor>(
		pWPhastDoc,
		headIC.polyh,
		desc
		);
	pAction->GetZoneActor()->SetData(headIC);
	pWPhastDoc->Execute(pAction);
}

CHeadIC CICHeadZoneActor::GetData(void)const
{
	return this->m_headIC;
}

void CICHeadZoneActor::SetData(const CHeadIC& rHeadIC)
{
	CProtect p(this->m_headIC.polyh);
	this->m_headIC = rHeadIC;
}

void CICHeadZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();

	HTREEITEM htiIC = pTreeControlBar->GetICHeadNode();;

	this->InsertAt(pTreeCtrl, htiIC, hInsertAfter);
	//{{HACK
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
	//}}HACK
}

void CICHeadZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	CZoneActor::Update(pTreeCtrl, htiParent);

	pTreeCtrl->SetItemText(htiParent, this->GetNameDesc());

	// head
	if (this->m_headIC.head)
	{
		static_cast<Cproperty*>(this->m_headIC.head)->Insert(pTreeCtrl, htiParent, "head");
	}
}

void CICHeadZoneActor::Serialize(bool bStoring, hid_t loc_id, const CUnits& units)
{
	CZoneActor::Serialize(bStoring, loc_id);

	if (bStoring)
	{
		// store head_ic
		this->m_headIC.Serialize(bStoring, loc_id);
	}
	else
	{
		// load head_ic
		this->m_headIC.Serialize(bStoring, loc_id);

		this->SetUnits(units);
	}
}

void CICHeadZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	CString str;
	str.Format(_T("%s Properties"), this->GetName());
	CPropertySheet props(str);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	ASSERT(this->GetData().polyh);

	CICHeadSpreadPropertyPage icHeadProps;
	icHeadProps.SetProperties(this->GetData());		
	icHeadProps.SetDesc(this->GetDesc());		

	props.AddPage(&icHeadProps);
	if (props.DoModal() == IDOK)
	{
		CHeadIC headIC;
		icHeadProps.GetProperties(headIC);
		ASSERT(headIC.polyh);
		pDoc->Execute(new CSetHeadICAction(this, pTreeCtrl, headIC, icHeadProps.GetDesc()));
	}
}

void CICHeadZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyIC())
	{
		pPropAssembly->AddPart(this);
		if (!pWPhastDoc->GetPropCollection()->IsItemPresent(pPropAssembly))
		{
			pWPhastDoc->GetPropCollection()->AddItem(pPropAssembly);
		}
	}
#ifdef _DEBUG
	else ASSERT(FALSE);
#endif
}

void CICHeadZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyIC())
	{
		pPropAssembly->RemovePart(this);
		// VTK HACK
		// This is req'd because ReleaseGraphicsResources is not called when
		// vtkPropAssembly::RemovePart(vtkProp *prop) is called
		pWPhastDoc->ReleaseGraphicsResources(this);
	}
#ifdef _DEBUG
	else ASSERT(FALSE);
#endif
}

void CICHeadZoneActor::SetStaticColor(COLORREF cr)
{
	CICHeadZoneActor::s_color[0] = (double)GetRValue(cr)/255.;
	CICHeadZoneActor::s_color[1] = (double)GetGValue(cr)/255.;
	CICHeadZoneActor::s_color[2] = (double)GetBValue(cr)/255.;	
	if (CICHeadZoneActor::s_Property)
	{
		CICHeadZoneActor::s_Property->SetColor(CICHeadZoneActor::s_color);
	}
	if (CICHeadZoneActor::s_OutlineProperty)
	{
		CICHeadZoneActor::s_OutlineProperty->SetColor(CICHeadZoneActor::s_color);
		CICHeadZoneActor::s_OutlineProperty->SetEdgeColor(CICHeadZoneActor::s_color);	
		CICHeadZoneActor::s_OutlineProperty->SetAmbientColor(CICHeadZoneActor::s_color);
	}
}

Polyhedron*& CICHeadZoneActor::GetPolyhedron(void)
{
	return this->m_headIC.polyh;
}
