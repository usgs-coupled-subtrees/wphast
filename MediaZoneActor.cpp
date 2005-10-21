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

#include <vtkPropAssembly.h>


#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro

vtkCxxRevisionMacro(CMediaZoneActor, "$Revision$");
vtkStandardNewMacro(CMediaZoneActor);

CMediaZoneActor::CMediaZoneActor(void)
{
	ASSERT(this->m_grid_elt.zone);
	this->m_pZone = this->m_grid_elt.zone;
}

CMediaZoneActor::~CMediaZoneActor(void)
{
}

void CMediaZoneActor::Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CGridElt& gridElt)
{
	CZoneCreateAction<CMediaZoneActor>* pAction = new CZoneCreateAction<CMediaZoneActor>(
		pWPhastDoc,
		pWPhastDoc->GetNextZoneName(),
		zone.x1,
		zone.x2,
		zone.y1,
		zone.y2,
		zone.z1,
		zone.z2
		);
	pAction->GetZoneActor()->SetGridElt(gridElt);
	pWPhastDoc->Execute(pAction);
}

void CMediaZoneActor::Serialize(bool bStoring, hid_t loc_id, const CUnits& units)
{
	ASSERT(this->GetName()); // must have name

	CZoneActor::Serialize(bStoring, loc_id);

	if (bStoring)
	{
		// store grid_elt
		this->m_grid_elt.Serialize(bStoring, loc_id);
	}
	else
	{
		// load grid_elt
		this->m_grid_elt.Serialize(bStoring, loc_id);
		this->SetUnits(units);
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
	this->m_hti = pTreeCtrl->InsertItem(this->GetName(), hParent, hInsertAfter);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti); // might want to move this -- this causes the tree control to redraw
	this->Update(pTreeCtrl, this->m_hti);
}

void CMediaZoneActor::Remove(CPropertyTreeControlBar* pTreeControlBar)
{
// COMMENT: {7/15/2004 3:36:40 PM}	if (CWPhastDoc *pDoc = pTreeControlBar->GetDocument()) {
// COMMENT: {7/15/2004 3:36:40 PM}		pDoc->RemoveProp3D(this);
// COMMENT: {7/15/2004 3:36:40 PM}		if (vtkPropAssembly *pPropAssembly = pDoc->GetPropAssemblyMedia()) {
// COMMENT: {7/15/2004 3:36:40 PM}			pPropAssembly->RemovePart(this);
// COMMENT: {7/15/2004 3:36:40 PM}			//{{ VTK HACK
// COMMENT: {7/15/2004 3:36:40 PM}			// This is req'd because ReleaseGraphicsResources is not called when
// COMMENT: {7/15/2004 3:36:40 PM}			// vtkPropAssembly::RemovePart(vtkProp *prop) is called
// COMMENT: {7/15/2004 3:36:40 PM}			pDoc->ReleaseGraphicsResources(this);
// COMMENT: {7/15/2004 3:36:40 PM}			//}} VTK HACK
// COMMENT: {7/15/2004 3:36:40 PM}		}
// COMMENT: {7/15/2004 3:36:40 PM}		pDoc->UpdateAllViews(0);
// COMMENT: {7/15/2004 3:36:40 PM}	}
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
	CString format;

	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiParent))
	{
		pTreeCtrl->DeleteItem(hChild);
	}

	// active
	if (this->m_grid_elt.active)
	{
		static_cast<Cproperty*>(this->m_grid_elt.active)->Insert(pTreeCtrl, htiParent, "active");
	}
	else if (this->GetDefault())
	{
		Cproperty prop(1);
		prop.Insert(pTreeCtrl, htiParent, "active");
	}

	// kx
	if (this->m_grid_elt.kx)
	{
		static_cast<Cproperty*>(this->m_grid_elt.kx)->Insert(pTreeCtrl, htiParent, "Kx");
	}

	// ky
	if (this->m_grid_elt.ky)
	{
		static_cast<Cproperty*>(this->m_grid_elt.ky)->Insert(pTreeCtrl, htiParent, "Ky");
	}

	// kz
	if (this->m_grid_elt.kz)
	{
		static_cast<Cproperty*>(this->m_grid_elt.kz)->Insert(pTreeCtrl, htiParent, "Kz");
	}

	// porosity
	if (this->m_grid_elt.porosity)
	{
		static_cast<Cproperty*>(this->m_grid_elt.porosity)->Insert(pTreeCtrl, htiParent, "porosity");
	}

	// storage
	if (this->m_grid_elt.storage)
	{
		static_cast<Cproperty*>(this->m_grid_elt.storage)->Insert(pTreeCtrl, htiParent, "specific_storage");
	}

	// alpha_long
	if (this->m_grid_elt.alpha_long)
	{
		static_cast<Cproperty*>(this->m_grid_elt.alpha_long)->Insert(pTreeCtrl, htiParent, "long_dispersivity");
	}

	// alpha_horizontal
	if (this->m_grid_elt.alpha_horizontal)
	{
		static_cast<Cproperty*>(this->m_grid_elt.alpha_horizontal)->Insert(pTreeCtrl, htiParent, "horizontal_dispersivity");
	}

	// alpha_vertical
	if (this->m_grid_elt.alpha_vertical)
	{
		static_cast<Cproperty*>(this->m_grid_elt.alpha_vertical)->Insert(pTreeCtrl, htiParent, "vertical_dispersivity");
	}

#ifdef _DEBUG
	//{{TEST DRAG DROP
	HTREEITEM hLastItem = pTreeCtrl->GetChildItem(htiParent);
	while (pTreeCtrl->GetNextSiblingItem(hLastItem))
	{
		hLastItem = pTreeCtrl->GetNextSiblingItem(hLastItem);
	}
	hLastItem = pTreeCtrl->InsertItem("TEST", hLastItem);
	hLastItem = pTreeCtrl->InsertItem("TEST", hLastItem);
	hLastItem = pTreeCtrl->InsertItem("TEST", hLastItem);
	//}}TEST DRAG DROP
#endif
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

	// set default as active
	//
	CGridElt elt(this->m_grid_elt);
	if (this->GetDefault())
	{
		if (!elt.active || elt.active->type == UNDEFINED)
		{
			Cproperty prop(1);
			Cproperty::CopyProperty(&elt.active, &prop);
		}
	}

	CMediaSpreadPropertyPage mediaSpreadProps;
	mediaSpreadProps.SetProperties(elt);
	if (this->GetDefault())
	{
		mediaSpreadProps.SetDefault(true);
		mediaSpreadProps.SetFlowOnly(bool(pDoc->GetFlowOnly()));
	}
	props.AddPage(&mediaSpreadProps);

	if (props.DoModal() == IDOK)
	{
		CGridElt grid_elt;
		mediaSpreadProps.GetProperties(grid_elt);
		pDoc->Execute(new CSetMediaAction(this, pTreeCtrl, grid_elt));
	}
}


CGridElt CMediaZoneActor::GetGridElt(void)const
{
	return this->m_grid_elt;
}

//void CMediaZoneActor::SetGridElt(const CGridElt& rGridElt, const CUnits& rUnits)
//{
//	// NOTE: use set bounds to set the zone
//	//
//
//	ASSERT(this->m_grid_elt.zone);
//	CZone zoneSave(*this->m_grid_elt.zone);
//
//	this->m_grid_elt = rGridElt;
//	(*this->m_grid_elt.zone) = zoneSave;
//
//	/** REMOVED SEPT 10
//	this->SetBounds(
//		this->m_grid_elt.zone->x1,
//		this->m_grid_elt.zone->x2,
//		this->m_grid_elt.zone->y1,
//		this->m_grid_elt.zone->y2,
//		this->m_grid_elt.zone->z1,
//		this->m_grid_elt.zone->z2,
//		rUnits
//		);
//	REMOVED SEPT 10 **/
//}

void CMediaZoneActor::SetGridElt(const CGridElt& rGridElt)
{
	// NOTE: Use SetBounds to set the zone
	//

	ASSERT(this->m_grid_elt.zone);
	CZone zoneSave(*this->m_grid_elt.zone);

	this->m_grid_elt = rGridElt;
	(*this->m_grid_elt.zone) = zoneSave;
	this->m_pZone = this->m_grid_elt.zone;
}

HTREEITEM CMediaZoneActor::GetHTreeItem(void)const
{
	return this->m_hti;
}

void CMediaZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyMedia()) {
		pPropAssembly->AddPart(this);
		if (!pWPhastDoc->GetPropCollection()->IsItemPresent(pPropAssembly)) {
			pWPhastDoc->GetPropCollection()->AddItem(pPropAssembly);
		}
	}
#ifdef _DEBUG
	else ASSERT(FALSE);
#endif
}

void CMediaZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyMedia()) {
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
