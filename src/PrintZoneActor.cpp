#include "StdAfx.h"
#include "resource.h"
#include "PrintZoneActor.h"
#include "PropertyTreeControlBar.h"
#include "ZoneCreateAction.h"
#include "PrintLocsPropsPage.h"
#include "property.h"

#include "WPhastDoc.h"
#include "Protect.h"
#include "Global.h"
#include "GridKeyword.h"
#include "WPhastMacros.h"
#include "SetPrintZoneAction.h"

#include <vtkPropAssembly.h>
#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro

vtkCxxRevisionMacro(CPrintZoneActor, "$Revision$");
vtkStandardNewMacro(CPrintZoneActor);

const char CPrintZoneActor::szHeading[] = "PrintLocations";
double CPrintZoneActor::s_color[3] = {0., 0., 0.};
vtkProperty* CPrintZoneActor::s_Property = 0;
vtkProperty* CPrintZoneActor::s_OutlineProperty = 0;

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPrintZoneActor::CPrintZoneActor(void)
{
	static StaticInit init; // constructs/destructs s_Property/s_OutlineProperty

	// cube
	this->CubeActor->SetProperty(CPrintZoneActor::s_Property);

	// outline
	this->OutlineActor->SetProperty(CPrintZoneActor::s_OutlineProperty);
}

CPrintZoneActor::~CPrintZoneActor(void)
{
}

Polyhedron*& CPrintZoneActor::GetPolyhedron(void)
{
	return this->m_printZone.polyh;
}

void CPrintZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter)
{
	ASSERT(FALSE);  // should never be called
}

void CPrintZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	CZoneActor::Update(pTreeCtrl, htiParent);

	CPrintZone relative(this->m_printZone);
	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	if (pFrame)
	{
		ASSERT_VALID(pFrame);
		CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
		ASSERT_VALID(pDoc);

		CGlobal::PathsAbsoluteToRelative(pDoc->GetDefaultPathName(), pDoc, relative);
	}

	// print
	if (relative.print)
	{
		static_cast<Cproperty*>(relative.print)->Insert(pTreeCtrl, htiParent, "print");
	}
}

void CPrintZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	CPrintLocsPropsPage page;
	page.SetProperties(this->m_printZone);
	page.SetDesc(this->GetDesc());

	CString str;
	str.Format(_T("%s Properties"), this->GetName());
	CPropertySheet props(str);
	props.AddPage(&page);

	if (props.DoModal() == IDOK)
	{
		CPrintZone printZone;
		page.GetProperties(printZone);
		ASSERT(printZone.polyh);

		pDoc->Execute(new CSetPrintZoneAction(this, pTreeCtrl, printZone, page.GetDesc()));
	}
}

void CPrintZoneActor::InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	this->m_hti = pTreeCtrl->InsertItem(this->GetNameDesc(), hParent, hInsertAfter);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti); // might want to move this -- this causes the tree control to redraw
	this->Update(pTreeCtrl, this->m_hti);
}

void CPrintZoneActor::Remove(CPropertyTreeControlBar* pTreeControlBar)
{
	CZoneActor::Remove(pTreeControlBar);
}

void CPrintZoneActor::UnRemove(CPropertyTreeControlBar* pTreeControlBar)
{
	HTREEITEM htiZFR = pTreeControlBar->GetZoneFlowRatesNode();
	ASSERT(this->m_hParent      == htiZFR);
	ASSERT(this->m_hInsertAfter != 0);

	vtkPropAssembly *pPropAssembly = pTreeControlBar->GetDocument()->GetPropAssemblyPrintLocations();
	pPropAssembly->AddPart(this);

	CZoneActor::UnRemove(pTreeControlBar);
}

void CPrintZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyPrintLocations())
	{
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(this));
		pPropAssembly->AddPart(this);
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(this));
		pWPhastDoc->AddPropAssembly(pPropAssembly);
	}
#ifdef _DEBUG
	else ASSERT(FALSE);
#endif
}

void CPrintZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyPrintLocations())
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

void CPrintZoneActor::SetStaticColor(COLORREF cr)
{
	CPrintZoneActor::s_color[0] = (double)GetRValue(cr)/255.;
	CPrintZoneActor::s_color[1] = (double)GetGValue(cr)/255.;
	CPrintZoneActor::s_color[2] = (double)GetBValue(cr)/255.;	
	if (CPrintZoneActor::s_Property)
	{
		CPrintZoneActor::s_Property->SetColor(CPrintZoneActor::s_color);
	}
	if (CPrintZoneActor::s_OutlineProperty)
	{
		CPrintZoneActor::s_OutlineProperty->SetColor(CPrintZoneActor::s_color);
		CPrintZoneActor::s_OutlineProperty->SetEdgeColor(CPrintZoneActor::s_color);
		CPrintZoneActor::s_OutlineProperty->SetAmbientColor(CPrintZoneActor::s_color);
	}
}

void CPrintZoneActor::SetData(const CPrintZone& printZone)
{
	// NOTE: Use SetBounds to set the zone
	//
	CProtect protect(this->m_printZone.polyh);
	this->m_printZone = printZone;
}

CPrintZone CPrintZoneActor::GetData(void)const
{
	return this->m_printZone;
}

void CPrintZoneActor::Create(CWPhastDoc* pWPhastDoc, const CPrintZone& printZone)
{
	ASSERT(printZone.polyh && ::AfxIsValidAddress(printZone.polyh, sizeof(Polyhedron)));

	CGridKeyword gk;
	pWPhastDoc->GetGridKeyword(gk);
	CZoneCreateAction<CPrintZoneActor>* pAction = new CZoneCreateAction<CPrintZoneActor>(
		pWPhastDoc,
		printZone.polyh,
		gk.m_grid_origin,
		gk.m_grid_angle,
		""
		);
	pAction->GetZoneActor()->SetData(printZone);
	pWPhastDoc->Execute(pAction);
}

void CPrintZoneActor::Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc)
{
	CZoneActor::Serialize(bStoring, loc_id, pWPhastDoc);

	if (bStoring)
	{
		// store zone_budget
		this->m_printZone.Serialize(bStoring, loc_id);
	}
	else
	{
		// load zone_budget
		this->m_printZone.Serialize(bStoring, loc_id);

		// color
		if (CPrintZoneActor::s_Property)
		{
			this->CubeActor->SetProperty(CPrintZoneActor::s_Property);
		}
	}
}

HTREEITEM CPrintZoneActor::GetHTreeItem(void)const
{
	ASSERT(this->m_hti);
	return this->m_hti;
}
