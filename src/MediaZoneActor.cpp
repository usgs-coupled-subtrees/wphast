#include "StdAfx.h"
#include "resource.h"
#include "MediaZoneActor.h"
#include "PropertyTreeControlBar.h"
#include "MediaPropertyPage.h"
#include "MediaSpreadPropertyPage.h"
#include "property.h"

#include "WPhastDoc.h"
#include "SetMediaAction.h"
#include "ZoneCreateAction.h"
#include "FlowOnly.h"
#include "Protect.h"
#include "Global.h"

//#include "MediaProps.h"
#include "MediaPropsPage.h"
#include "MediaPropsPage2.h"
#include "TreePropSheetEx/TreePropSheetEx.h"
#include "GridKeyword.h"


#include <vtkPropAssembly.h>
#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro

vtkCxxRevisionMacro(CMediaZoneActor, "$Revision$");
vtkStandardNewMacro(CMediaZoneActor);

const char CMediaZoneActor::szHeading[] = "Media";
double CMediaZoneActor::s_color[3] = {0., 0., 0.};
vtkProperty* CMediaZoneActor::s_Property = 0;
vtkProperty* CMediaZoneActor::s_OutlineProperty = 0;


// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMediaZoneActor::CMediaZoneActor(void)
{
	static StaticInit init; // constructs/destructs s_Property/s_OutlineProperty

	// cube
	this->CubeActor->SetProperty(CMediaZoneActor::s_Property);

	// outline
	this->OutlineActor->SetProperty(CMediaZoneActor::s_OutlineProperty);
}

CMediaZoneActor::~CMediaZoneActor(void)
{
}

void CMediaZoneActor::Create(CWPhastDoc* pWPhastDoc, const CGridElt& gridElt, LPCTSTR desc)
{
	ASSERT(gridElt.polyh && ::AfxIsValidAddress(gridElt.polyh, sizeof(Polyhedron)));
	CGridKeyword gk;
	pWPhastDoc->GetGridKeyword(gk);
	CZoneCreateAction<CMediaZoneActor>* pAction = new CZoneCreateAction<CMediaZoneActor>(
		pWPhastDoc,
		gridElt.polyh,
		gk.m_grid_origin,
		gk.m_grid_angle,
		desc
		);
	pAction->GetZoneActor()->SetGridElt(gridElt);
	pWPhastDoc->Execute(pAction);
}

void CMediaZoneActor::Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc)
{
	CZoneActor::Serialize(bStoring, loc_id, pWPhastDoc);

	if (bStoring)
	{
		// store grid_elt
		this->m_grid_elt.Serialize(bStoring, loc_id);
	}
	else
	{
		// load grid_elt
		this->m_grid_elt.Serialize(bStoring, loc_id);

		if (this->GetDefault() && this->m_grid_elt.tortuosity == 0)
		{
			this->m_grid_elt.tortuosity          = new Cproperty();
			this->m_grid_elt.tortuosity->type    = PROP_FIXED;
			this->m_grid_elt.tortuosity->count_v = 1;
			this->m_grid_elt.tortuosity->v[0]    = 1.0;
		}

		// color
		if (CMediaZoneActor::s_Property)
		{
			this->CubeActor->SetProperty(CMediaZoneActor::s_Property);
		}
	}
}

void CMediaZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
	HTREEITEM htiMedia = pTreeControlBar->GetMediaNode();
	this->InsertAt(pTreeCtrl, htiMedia, hInsertAfter);
	//{{HACK
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
	//}}HACK
}

void CMediaZoneActor::InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	this->m_hti = pTreeCtrl->InsertItem(this->GetNameDesc(), hParent, hInsertAfter);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti); // might want to move this -- this causes the tree control to redraw
	this->Update(pTreeCtrl, this->m_hti);
}

void CMediaZoneActor::Remove(CPropertyTreeControlBar* pTreeControlBar)
{
	CZoneActor::Remove(pTreeControlBar);
}

void CMediaZoneActor::UnRemove(CPropertyTreeControlBar* pTreeControlBar)
{
	HTREEITEM htiMedia = pTreeControlBar->GetMediaNode();
	ASSERT(this->m_hParent      == htiMedia);
	ASSERT(this->m_hInsertAfter != 0);

	vtkPropAssembly *pPropAssembly = pTreeControlBar->GetDocument()->GetPropAssemblyMedia();
	pPropAssembly->AddPart(this);

	CZoneActor::UnRemove(pTreeControlBar);
}

void CMediaZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	CZoneActor::Update(pTreeCtrl, htiParent);

	CString format;

	CGridElt relative(this->m_grid_elt);
	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	if (pFrame)
	{
		ASSERT_VALID(pFrame);
		CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
		ASSERT_VALID(pDoc);

		CGlobal::PathsAbsoluteToRelative(pDoc->GetDefaultPathName(), pDoc, relative);
	}

	// active
	if (relative.active)
	{
		static_cast<Cproperty*>(relative.active)->Insert(pTreeCtrl, htiParent, "active");
	}
	else if (this->GetDefault())
	{
		Cproperty prop(1);
		prop.Insert(pTreeCtrl, htiParent, "active");
	}

	// kx
	if (relative.kx)
	{
		static_cast<Cproperty*>(relative.kx)->Insert(pTreeCtrl, htiParent, "Kx");
	}

	// ky
	if (relative.ky)
	{
		static_cast<Cproperty*>(relative.ky)->Insert(pTreeCtrl, htiParent, "Ky");
	}

	// kz
	if (relative.kz)
	{
		static_cast<Cproperty*>(relative.kz)->Insert(pTreeCtrl, htiParent, "Kz");
	}

	// porosity
	if (relative.porosity)
	{
		static_cast<Cproperty*>(relative.porosity)->Insert(pTreeCtrl, htiParent, "porosity");
	}

	// storage
	if (relative.storage)
	{
		static_cast<Cproperty*>(relative.storage)->Insert(pTreeCtrl, htiParent, "specific_storage");
	}

	// alpha_long
	if (relative.alpha_long)
	{
		static_cast<Cproperty*>(relative.alpha_long)->Insert(pTreeCtrl, htiParent, "long_dispersivity");
	}

	// alpha_horizontal
	if (relative.alpha_horizontal)
	{
		static_cast<Cproperty*>(relative.alpha_horizontal)->Insert(pTreeCtrl, htiParent, "horizontal_dispersivity");
	}

	// alpha_vertical
	if (relative.alpha_vertical)
	{
		static_cast<Cproperty*>(relative.alpha_vertical)->Insert(pTreeCtrl, htiParent, "vertical_dispersivity");
	}

	// tortuosity
	if (relative.tortuosity)
	{
		static_cast<Cproperty*>(relative.tortuosity)->Insert(pTreeCtrl, htiParent, "tortuosity");
	}

	// shell
	if (relative.shell)
	{
		CString format;
		format.Format("shell %g %g %g", relative.shell_width[0], relative.shell_width[1], relative.shell_width[2]);
		pTreeCtrl->InsertItem(format, htiParent);
	}
}

void CMediaZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	CString str;
	str.Format(_T("%s Properties"), this->GetName());
	CPropertySheet props(str);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	ASSERT(this->GetData().polyh);

	// set default as active
	//
	CGridElt elt(this->m_grid_elt);
	if (this->GetDefault())
	{
		if (!elt.active || elt.active->type == PROP_UNDEFINED)
		{
			Cproperty prop(1);
			Cproperty::CopyProperty(&elt.active, &prop);
		}
	}

	CMediaPropsPage2 mediaProps;
	mediaProps.SetProperties(elt);
	mediaProps.SetDesc(this->GetDesc());
	mediaProps.SetUnits(pDoc->GetUnits());
	if (this->GetDefault())
	{
		mediaProps.SetDefault(true);
		mediaProps.SetFlowOnly(bool(pDoc->GetFlowOnly()));
	}
	props.AddPage(&mediaProps);

	if (props.DoModal() == IDOK)
	{
		CGridElt GridElt;
		mediaProps.GetProperties(GridElt);
		ASSERT(GridElt.polyh);

		pDoc->Execute(new CSetMediaAction(this, pTreeCtrl, GridElt, mediaProps.GetDesc()));
	}
}

CGridElt CMediaZoneActor::GetGridElt(void)const
{
	return this->m_grid_elt;
}

CGridElt CMediaZoneActor::GetData(void)const
{
	return this->GetGridElt();
}

void CMediaZoneActor::SetGridElt(const CGridElt& rGridElt)
{
	// NOTE: Use SetBounds to set the zone
	//
	CProtect p(this->m_grid_elt.polyh);
	this->m_grid_elt = rGridElt;
}


void CMediaZoneActor::SetData(const CGridElt& rGridElt)
{
	this->SetGridElt(rGridElt);
}

HTREEITEM CMediaZoneActor::GetHTreeItem(void)const
{
	return this->m_hti;
}

void CMediaZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyMedia())
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

void CMediaZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyMedia())
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

void CMediaZoneActor::SetStaticColor(COLORREF cr)
{
	CMediaZoneActor::s_color[0] = (double)GetRValue(cr)/255.;
	CMediaZoneActor::s_color[1] = (double)GetGValue(cr)/255.;
	CMediaZoneActor::s_color[2] = (double)GetBValue(cr)/255.;	
	if (CMediaZoneActor::s_Property)
	{
		CMediaZoneActor::s_Property->SetColor(CMediaZoneActor::s_color);
	}
	if (CMediaZoneActor::s_OutlineProperty)
	{
		CMediaZoneActor::s_OutlineProperty->SetColor(CMediaZoneActor::s_color);
		CMediaZoneActor::s_OutlineProperty->SetEdgeColor(CMediaZoneActor::s_color);
		CMediaZoneActor::s_OutlineProperty->SetAmbientColor(CMediaZoneActor::s_color);
	}
}

Polyhedron*& CMediaZoneActor::GetPolyhedron(void)
{
	return this->m_grid_elt.polyh;
}
