#include "StdAfx.h"
#include "PropertyTreeControlBar.h"
#include "resource.h"

#include "PrintFreq.h"
#include "PrintFreqPropertyPage.h"

#include <windowsx.h>                            // GET_X_LPARAM GET_Y_LPARAM
#undef GetNextSibling                            // defined in windowsx.h

//// #include "ZoneLODActor.h"
#include "GridLODActor.h"
#include "WPhastDoc.h"
#include "WPhastView.h"
#include "MediaPropertyPage.h"
#include "GridPropertyPage.h"
#include "ZoneActor.h"
#include "WellActor.h"
#include "RiverActor.h"
#include "MediaZoneActor.h"
#include "ICZoneActor.h"
#include "BCZoneActor.h"
#include "Units.h"
#include "NewModel.h"
#include "FlowOnly.h"
#include "TimeControl2.h"
#include "Units1PropertyPage.h"
#include "Units2PropertyPage.h"
#include "BoxPropertiesDialogBar.h"
#include "Global.h"

// Actions
#include "ResizeGridAction.h"
#include "SetUnitsAction.h"
#include "ZoneRemoveAction.h"
#include "WellDeleteAction.h"


#include <vtkWin32RenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkPropCollection.h>
#include <vtkAxes.h>
#include <vtkTubeFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkInteractorStyle.h> 
#include <vtkBoxWidget.h>
#include <vtkPropAssembly.h>

static const TCHAR szPeriodFormat[]        = _T("Simulation Period %d");
static const TCHAR szFLOW_ONLY[]           = _T("FLOW_ONLY");
static const TCHAR szSTEADY_FLOW[]         = _T("STEADY_FLOW");
static const TCHAR szFREE_SURFACE_BC[]     = _T("FREE_SURFACE_BC");
static const TCHAR szUNITS[]               = _T("UNITS");
static const TCHAR szGRID[]                = _T("GRID");
static const TCHAR szMEDIA[]               = _T("MEDIA");
static const TCHAR szBOUNDARY_CONDITIONS[] = _T("BOUNDARY_CONDITIONS");
static const TCHAR szINITIAL_CONDITIONS[]  = _T("INITIAL_CONDITIONS");
static const TCHAR szTIME_CONTROL[]        = _T("TIME_CONTROL");
static const TCHAR szPRINT_FREQUENCY[]     = _T("PRINT_FREQUENCY");

static const TCHAR szWELLS[]               = _T("WELLS");
static const TCHAR szRIVERS[]              = _T("RIVERS");

static const TCHAR szZoneFormat[]          = _T("Zone %d");


static const int BC_INDEX              = 0;
static const int PRINT_FREQUENCY_INDEX = 1;
static const int TIME_CONTROL_INDEX    = 2;


// vtkProp3D

IMPLEMENT_DYNAMIC(CPropertyTreeControlBar, baseCPropertyTreeControlBar);

BEGIN_MESSAGE_MAP(CPropertyTreeControlBar, CSizingControlBarCFVS7)
	ON_WM_CREATE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROPERTY_TREE, OnSelChanged)
	ON_NOTIFY(NM_CLICK, IDC_PROPERTY_TREE, OnNMClk)	
	ON_NOTIFY(NM_DBLCLK, IDC_PROPERTY_TREE, OnNMDblClk)	
	ON_NOTIFY(TVN_KEYDOWN, IDC_PROPERTY_TREE, OnKeyDown)		
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()
/**
NM_CLICK
NM_CUSTOMDRAW
NM_DBLCLK
NM_KILLFOCUS
NM_RCLICK
NM_RDBLCLK
NM_RETURN
NM_SETCURSOR
NM_SETFOCUS

TVN_BEGINDRAG
TVN_BEGINLABELEDIT
TVN_BEGINRDRAG
TVN_DELETEITEM
TVN_ENDLABELEDIT
TVN_GETDISPINFO
TVN_GETINFOTIP
TVN_ITEMEXPANDED
TVN_ITEMEXPANDING
TVN_KEYDOWN
TVN_SELCHANGED <--
TVN_SELCHANGING
TVN_SETDISPINFO
TVN_SINGLEEXPAND
**/

CPropertyTreeControlBar::CPropertyTreeControlBar(void)
: m_bSelectingProp(false)
, m_nNextZone(0)
, m_pImageList(NULL)
{
}

CPropertyTreeControlBar::~CPropertyTreeControlBar(void)
{
}

CWPhastDoc* CPropertyTreeControlBar::GetDocument(void)const
{
	if (CFrameWnd *pFrame = reinterpret_cast<CFrameWnd*>(AfxGetApp()->m_pMainWnd)) {
		ASSERT_KINDOF(CFrameWnd, pFrame);
		ASSERT_VALID(pFrame);
		if (CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument())) {
			ASSERT_KINDOF(CWPhastDoc, pDoc);
			ASSERT_VALID(pDoc);
			return pDoc;
		}
	}
	return 0;
}

void CPropertyTreeControlBar::OnSelChanged(NMHDR* pNMHDR, LRESULT* /*pResult*/)
{
	if (this->m_bSelectingProp) return;

	NMTREEVIEW *pNMTREEVIEW = (LPNMTREEVIEW)pNMHDR;
	CTreeCtrlNode item(pNMTREEVIEW->itemNew.hItem, this->GetTreeCtrlEx());

	// MEDIA
	//
	if (item.IsNodeAncestor(this->m_nodeMedia))
	{
		if (item != this->m_nodeMedia)
		{
			while (item.GetParent() != this->m_nodeMedia)
			{
				item = item.GetParent();
				if (!item) break;
			}
			if (item.GetData())
			{
				if (CZoneActor* pZone = CZoneActor::SafeDownCast((vtkObject*)item.GetData()))
				{

					CFrameWnd *pFrame = reinterpret_cast<CFrameWnd*>(AfxGetApp()->m_pMainWnd);
					ASSERT_KINDOF(CFrameWnd, pFrame);
					ASSERT_VALID(pFrame);

					CWPhastView* pView = reinterpret_cast<CWPhastView*>(pFrame->GetActiveView());
					ASSERT_KINDOF(CWPhastView, pView);
					ASSERT_VALID(pView);

					pZone->Select(pView);
				}
			}
		}
		//{{
		else
		{
            if (CWPhastDoc *pDoc = this->GetDocument()) {
				pDoc->ClearSelection();
			}
		}
		//}}
		return;
	}

	///{{{{{
	// Rivers
	//
	CTreeCtrlNode child = this->GetRiversNode().GetChild();
	for (; child; child = child.GetNextSibling())
	{
		if (CRiverActor *pActor = CRiverActor::SafeDownCast(reinterpret_cast<vtkObject*>(child.GetData())))
		{
			pActor->ClearSelection();
			pActor->SetEnabled(0);
		}
	}
	if (item.IsNodeAncestor(this->GetRiversNode()) && item != this->GetRiversNode())
	{
		if ((item != this->GetRiversNode()) && (item.GetParent() != this->GetRiversNode()))
		{
			CTreeCtrlNode riverNode = item.GetParent();
			if (riverNode.GetData())
			{
				if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(reinterpret_cast<vtkObject*>(riverNode.GetData())))
				{
					pRiverActor->SelectPoint(riverNode.GetIndex(item));
				}
				else
				{
					ASSERT(FALSE);
				}
			}
		}
	}


	if (m_wndTree.GetItemData(pNMTREEVIEW->itemNew.hItem))
	{
		HTREEITEM hParent = m_wndTree.GetParentItem(pNMTREEVIEW->itemNew.hItem);
		// if (pNMTREEVIEW->itemNew.hItem == m_htiGrid || hParent == m_htiGrid) {
		// if (pNMTREEVIEW->itemNew.hItem == m_nodeGrid || hParent == m_nodeGrid) {
// COMMENT: {7/15/2004 1:28:10 PM}		if (this->m_nodeGrid.IsNodeAncestor(CTreeCtrlNode(pNMTREEVIEW->itemNew.hItem, &this->m_wndTree))) {
// COMMENT: {7/15/2004 1:28:10 PM}			//CGridLODActor* pZone = reinterpret_cast<CGridLODActor*>(m_wndTree.GetItemData(pNMTREEVIEW->itemNew.hItem));
// COMMENT: {7/15/2004 1:28:10 PM}		}


		if (hParent == m_nodeMedia || hParent == this->m_nodeBC || hParent == this->m_nodeIC)
		{

			CZoneActor* pZone = reinterpret_cast<CZoneActor*>(m_wndTree.GetItemData(pNMTREEVIEW->itemNew.hItem));

			CFrameWnd *pFrame = reinterpret_cast<CFrameWnd*>(AfxGetApp()->m_pMainWnd);
			ASSERT_KINDOF(CFrameWnd, pFrame);
			ASSERT_VALID(pFrame);

			CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
			ASSERT_KINDOF(CWPhastDoc, pDoc);
			ASSERT_VALID(pDoc);

			CWPhastView* pView = reinterpret_cast<CWPhastView*>(pFrame->GetActiveView());
			ASSERT_KINDOF(CWPhastView, pView);
			ASSERT_VALID(pView);

			pZone->Select(pView);
		}
		else if (hParent == this->m_nodeWells)
		{
			// if (CWellActor* pWell = reinterpret_cast<CWellActor*>(item.GetData()))
			if (item.GetData())
			{
				if (vtkProp* pProp = vtkProp::SafeDownCast(reinterpret_cast<vtkObject*>(item.GetData())))
				{
					if (CWPhastDoc* pDoc = this->GetDocument())
					{
						pDoc->Notify(this, WPN_SELCHANGED, 0, pProp);
					}
				}
			}
		}
		else if (hParent == this->m_nodeRivers)
		{
			if (item.GetData())
			{
				if (vtkProp* pProp = vtkProp::SafeDownCast(reinterpret_cast<vtkObject*>(item.GetData())))
				{
					if (CWPhastDoc* pDoc = this->GetDocument())
					{
						pDoc->Notify(this, WPN_SELCHANGED, 0, pProp);
					}
				}
			}
		}
		else
		{
			if (CWPhastDoc *pDoc = this->GetDocument()) {
				pDoc->ClearSelection();
			}

			// ASSERT(FALSE);
			// Update StatusBar
			//
			if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
				CString status;
				status.LoadString(AFX_IDS_IDLEMESSAGE);
				pWnd->SetWindowText(status);
			}
		}		
	}
	else
	{
		//if (pNMTREEVIEW->itemNew.hItem == this->m_nodeMedia || pNMTREEVIEW->itemNew.hItem == this->m_nodeBC || pNMTREEVIEW->itemNew.hItem == this->m_nodeIC) {
            if (CWPhastDoc *pDoc = this->GetDocument()) {
				pDoc->ClearSelection();
			}
		//}
	}
}

void CPropertyTreeControlBar::OnNMClk(NMHDR* pNMHDR, LRESULT* pResult)
{
	TVHITTESTINFO ht = {0};

	DWORD dwpos = ::GetMessagePos();
	ht.pt.x = GET_X_LPARAM(dwpos);
	ht.pt.y = GET_Y_LPARAM(dwpos);

	::MapWindowPoints(HWND_DESKTOP, pNMHDR->hwndFrom, &ht.pt, 1);
	this->m_wndTree.HitTest(&ht);

	if (!ht.hItem) return;
	if (!(TVHT_ONITEMSTATEICON & ht.flags)) return;


	// determine next state
	//
	CTreeCtrlNode item(ht.hItem, &this->m_wndTree);
	UINT nNewState;
	switch (this->GetNodeCheck(item)) {
		case BST_UNCHECKED:
			// currently unchecked
			nNewState = BST_CHECKED;
			break;
		case BST_CHECKED:
			// currently checked
			nNewState = BST_UNCHECKED;
			break;
		default:
			ASSERT(FALSE);
			nNewState = BST_CHECKED;
			break;
	}

	// set new state
	//
	this->SetNodeCheck(item, nNewState);

	// re-render
	//
	if (CWPhastDoc *pDoc = this->GetDocument())
	{
// COMMENT: {9/8/2004 5:10:45 PM}		// HACK {{
// COMMENT: {9/8/2004 5:10:45 PM}		if (item == this->GetWellsNode())
// COMMENT: {9/8/2004 5:10:45 PM}		{
// COMMENT: {9/8/2004 5:10:45 PM}			pDoc->GetPropAssemblyWells()->SetVisibility(nNewState == BST_CHECKED);
// COMMENT: {9/8/2004 5:10:45 PM}			pDoc->Notify(this, WPN_VISCHANGED, 0, pDoc->GetPropAssemblyWells());
// COMMENT: {9/8/2004 5:10:45 PM}		}
// COMMENT: {9/8/2004 5:10:45 PM}		// HACK }}

		pDoc->UpdateAllViews(0);
	}

// COMMENT: {7/13/2004 4:08:16 PM}	if (CWPhastDoc *pDoc = this->GetDocument()) {
// COMMENT: {7/13/2004 4:08:16 PM}		if (item == this->m_nodeMedia) {
// COMMENT: {7/13/2004 4:08:16 PM}			vtkPropAssembly *pPropAssembly = pDoc->GetPropAssemblyMedia();
// COMMENT: {7/13/2004 4:08:16 PM}			if (nNewState == BST_CHECKED) {
// COMMENT: {7/13/2004 4:08:16 PM}				pPropAssembly->SetVisibility(1);
// COMMENT: {7/13/2004 4:08:16 PM}			}
// COMMENT: {7/13/2004 4:08:16 PM}			else {
// COMMENT: {7/13/2004 4:08:16 PM}				pPropAssembly->SetVisibility(0);
// COMMENT: {7/13/2004 4:08:16 PM}			}
// COMMENT: {7/13/2004 4:08:16 PM}			// set check mark
// COMMENT: {7/13/2004 4:08:16 PM}			item.SetState(INDEXTOSTATEIMAGEMASK(nNewState + 1), TVIS_STATEIMAGEMASK);
// COMMENT: {7/13/2004 4:08:16 PM}		}
// COMMENT: {7/13/2004 4:08:16 PM}		pDoc->UpdateAllViews(0);
// COMMENT: {7/13/2004 4:08:16 PM}	}
}

UINT CPropertyTreeControlBar::GetNodeCheck(CTreeCtrlNode node)const
{
	switch (node.GetState(TVIS_STATEIMAGEMASK) & TVIS_STATEIMAGEMASK) {
		case (INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1)):
			// currently unchecked
			return BST_UNCHECKED;
			break;
		case (INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1)):
			// currently checked
			return BST_CHECKED;
			break;
		default:
			ASSERT(FALSE);
			break;
	}
	return BST_INDETERMINATE;
}

void CPropertyTreeControlBar::SetNodeCheck(CTreeCtrlNode node, UINT nCheckState)
{
	ASSERT(nCheckState == BST_CHECKED || nCheckState == BST_UNCHECKED);

	vtkPropAssembly *pPropAssembly = 0;
	CWPhastDoc *pDoc = 0;

	if (node == this->GetMediaNode()) {
		if (!((pDoc = this->GetDocument()) && (pPropAssembly = pDoc->GetPropAssemblyMedia()))) {
			return;
		}
	}
	else if (node == this->GetICNode()) {
		if (!((pDoc = this->GetDocument()) && (pPropAssembly = pDoc->GetPropAssemblyIC()))) {
			return;
		}
	}
	else if (node == this->GetBCNode()) {
		if (!((pDoc = this->GetDocument()) && (pPropAssembly = pDoc->GetPropAssemblyBC()))) {
			return;
		}
	}
	else if (node == this->GetWellsNode())
	{
// COMMENT: {9/8/2004 5:11:03 PM}		// HACK {{
// COMMENT: {9/8/2004 5:11:03 PM}		// set check mark
// COMMENT: {9/8/2004 5:11:03 PM}		node.SetState(INDEXTOSTATEIMAGEMASK(nCheckState + 1), TVIS_STATEIMAGEMASK);
// COMMENT: {9/8/2004 5:11:03 PM}		return;
// COMMENT: {9/8/2004 5:11:03 PM}		// HACK }}
		if (!((pDoc = this->GetDocument()) && (pPropAssembly = pDoc->GetPropAssemblyWells()))) {
			return;
		}
	}
	else if (node == this->GetRiversNode())
	{
		if (!((pDoc = this->GetDocument()) && (pPropAssembly = pDoc->GetPropAssemblyRivers()))) {
			return;
		}
	}
	else if (node == this->GetGridNode()) {
		if (CWPhastDoc *pDoc = this->GetDocument()) {
			pDoc->OnViewGrid();
		}
		return;
	}
	else {
		ASSERT(
			node.GetParent() == this->GetMediaNode()  ||
            node.GetParent() == this->GetICNode()     ||
            node.GetParent() == this->GetBCNode()     ||
            node.GetParent() == this->GetWellsNode()  ||
            node.GetParent() == this->GetRiversNode()
				);
	}

	// set check mark
	node.SetState(INDEXTOSTATEIMAGEMASK(nCheckState + 1), TVIS_STATEIMAGEMASK);

	if (pPropAssembly)
	{
		// set assembly visibility
		pPropAssembly->SetVisibility(nCheckState == BST_CHECKED);
	}
	else
	{
		// set individual actor visibility
		ASSERT(node.GetData());
		if (node.GetData())
		{
// COMMENT: {6/15/2005 1:49:44 PM}			if (vtkActor* pActor = vtkActor::SafeDownCast((vtkObject*)node.GetData()))
// COMMENT: {6/15/2005 1:49:44 PM}			{
// COMMENT: {6/15/2005 1:49:44 PM}				pActor->SetVisibility(nCheckState == BST_CHECKED);
// COMMENT: {6/15/2005 1:49:44 PM}			}
// COMMENT: {6/15/2005 1:49:44 PM}			if (CRiverActor* pActor = CRiverActor::SafeDownCast((vtkObject*)node.GetData()))
// COMMENT: {6/15/2005 1:49:44 PM}			{
// COMMENT: {6/15/2005 1:49:44 PM}				pActor->SetVisibility(nCheckState == BST_CHECKED);
// COMMENT: {6/15/2005 1:49:44 PM}			}
			if (vtkProp3D* pProp3D = vtkProp3D::SafeDownCast((vtkObject*)node.GetData()))
			{
				pProp3D->SetVisibility(nCheckState == BST_CHECKED);
			}
		}
	}
}

void CPropertyTreeControlBar::SetBCCheck(UINT nCheckState)
{
	this->SetNodeCheck(this->m_nodeBC, nCheckState);
}

void CPropertyTreeControlBar::SetICCheck(UINT nCheckState)
{
	this->SetNodeCheck(this->m_nodeIC, nCheckState);
}

void CPropertyTreeControlBar::SetMediaCheck(UINT nCheckState)
{
	this->SetNodeCheck(this->m_nodeMedia, nCheckState);
}

UINT CPropertyTreeControlBar::GetBCCheck(void)
{
	return this->GetNodeCheck(this->GetBCNode());
}

UINT CPropertyTreeControlBar::GetICCheck(void)
{
	return this->GetNodeCheck(this->GetICNode());
}

UINT CPropertyTreeControlBar::GetMediaCheck(void)
{
	return this->GetNodeCheck(this->GetMediaNode());
}

void CPropertyTreeControlBar::OnNMDblClk(NMHDR* pNMHDR, LRESULT* pResult)
{
	TRACE("OnNMDblClk\n");

	//{{HACK
	// If creating a new zone cancel now 
	// if this isn't here a leak occurs iff the
	// item double-clicked is already selected
	{
		CFrameWnd *pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		ASSERT_VALID(pFrame);

		CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
		ASSERT_VALID(pDoc);

		POSITION pos = pDoc->GetFirstViewPosition();
		while (pos != NULL) {
			CWPhastView *pView = (CWPhastView*) pDoc->GetNextView(pos);
			ASSERT_VALID(pView);
			if (pView->CreatingNewZone()) {
				pView->CancelNewZone();
			}
		}
	}
	//}}HACK

	TVHITTESTINFO ht = {0};

	DWORD dwpos = ::GetMessagePos();
	ht.pt.x = GET_X_LPARAM(dwpos);
	ht.pt.y = GET_Y_LPARAM(dwpos);

	::MapWindowPoints(HWND_DESKTOP, pNMHDR->hwndFrom, &ht.pt, 1);
	this->m_wndTree.HitTest(&ht);

	if (!ht.hItem) return;
	if (!(TVHT_ONITEM & ht.flags)) return;

	CTreeCtrlNode item = this->GetTreeCtrlEx()->GetSelectedItem();
	CTreeCtrlNode parent = item.GetParent();
	HTREEITEM hItem = item;
	HTREEITEM hParent = parent;

	// FLOW_ONLY
	//
	if (item.IsNodeAncestor(this->m_nodeFlowOnly))
	{
		CFlowOnly *pFlowOnly = reinterpret_cast<CFlowOnly*>(this->m_nodeFlowOnly.GetData());
		ASSERT(pFlowOnly);
#if defined(_DEBUG)
		CFrameWnd *pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		ASSERT_VALID(pFrame);
		// CWPhastDoc* pDoc = dynamic_cast<CWPhastDoc*>(pFrame->GetActiveDocument()); // needs /GR
		CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
		ASSERT_VALID(pDoc);
		CNewModel *pModel = pDoc->GetModel();
		ASSERT(pFlowOnly == &pDoc->GetModel()->m_flowOnly);
#endif
		//{{ HACK
		CTreeCtrlNode nodeMedia = this->GetMediaNode();
		int nCount = nodeMedia.GetChildCount();

		CMediaZoneActor *pMediaZoneActor = NULL;
		for (int i = 0; i < nCount; ++i)
		{
			if (CMediaZoneActor *pZone = CMediaZoneActor::SafeDownCast((vtkObject*)nodeMedia.GetChildAt(i).GetData()))
			{
				pMediaZoneActor = pZone;
				///pFlowOnly->Edit(&this->m_wndTree, pZone);
				///*pResult = TRUE;
				///return;
				break;
			}
		}

		CTreeCtrlNode nodeIC = this->GetICNode();
		nCount = nodeIC.GetChildCount();

		CICZoneActor *pICZoneActor = NULL;
		for (int i = 0; i < nCount; ++i)
		{
			if (CICZoneActor *pZone = CICZoneActor::SafeDownCast((vtkObject*)nodeIC.GetChildAt(i).GetData()))
			{
				if (pZone->GetType() == CICZoneActor::IC_CHEM)
				{
					pICZoneActor = pZone;
					break;
				}
			}
		}
		ASSERT(pMediaZoneActor && pICZoneActor);
		pFlowOnly->Edit(&this->m_wndTree, pMediaZoneActor, pICZoneActor);
		//}} HACK
		///pFlowOnly->Edit(&this->m_wndTree);
		*pResult = TRUE;
		return;
	}


	// FREE_SURFACE
	//
	if (item.IsNodeAncestor(this->m_nodeFreeSurface)) {
		CFreeSurface *pFreeSurface = reinterpret_cast<CFreeSurface*>(this->m_nodeFreeSurface.GetData());
		ASSERT(pFreeSurface);
#if defined(_DEBUG)
		CFrameWnd *pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		ASSERT_VALID(pFrame);
		// CWPhastDoc* pDoc = dynamic_cast<CWPhastDoc*>(pFrame->GetActiveDocument()); // needs /GR
		CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
		ASSERT_VALID(pDoc);
		ASSERT(pFreeSurface == &pDoc->GetModel()->m_freeSurface);
#endif
		pFreeSurface->Edit(&this->m_wndTree);
		*pResult = TRUE;
		return;
	}

	// STEADY_FLOW
	//
	if (item.IsNodeAncestor(this->m_nodeSteadyFlow)) {
		CSteadyFlow* pSteadyFlow = reinterpret_cast<CSteadyFlow*>(this->m_nodeSteadyFlow.GetData());
		ASSERT(pSteadyFlow);
#if defined(_DEBUG)
		CFrameWnd *pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		ASSERT_VALID(pFrame);
		CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
		ASSERT_VALID(pDoc);
		ASSERT(pSteadyFlow == &pDoc->GetModel()->m_steadyFlow);
#endif
		pSteadyFlow->Edit(&this->m_wndTree);
		*pResult = TRUE;
		return;
	}


	// UNITS
	//
	if (item.IsNodeAncestor(this->m_nodeUnits)) {
		if (this->m_nodeUnits.GetData()) {
			CUnits* pUnits = (CUnits*)this->m_nodeUnits.GetData();
			pUnits->Edit(&this->m_wndTree);
			*pResult = TRUE;
			return;
		}
		return;
	}

	// WELLS
	//
	if (item.IsNodeAncestor(this->m_nodeWells))
	{
		if (item != this->m_nodeWells)
		{
			while (item.GetParent() != this->m_nodeWells)
			{
				item = item.GetParent();
				if (!item) break;
			}
			if (item.GetData())
			{
				if (CWellActor* pWellActor = CWellActor::SafeDownCast((vtkObject*)item.GetData()))
				{
					pWellActor->Edit(this->GetDocument());
					*pResult = TRUE;
					return;
				}
			}
		}
		return;
	}

	// RIVERS
	//
	if (item.IsNodeAncestor(this->m_nodeRivers))
	{
		if (item != this->m_nodeRivers)
		{
			while (item.GetParent() != this->m_nodeRivers)
			{
				item = item.GetParent();
				if (!item) break;
			}
			if (item.GetData())
			{
				// ASSERT(FALSE); // TODO
				::AfxMessageBox("Not currently supported");
			}
		}
		return;
	}


	// GRID
	//
	if (item.IsNodeAncestor(this->m_nodeGrid)) {
		// TODO: pGrid.Edit(&this->m_wndTree);
		if (CGridLODActor* pGrid = CGridLODActor::SafeDownCast((vtkObject*)m_nodeGrid.GetData())) {
			CFrameWnd *pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
			ASSERT_VALID(pFrame);
			CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
			ASSERT_VALID(pDoc);

			// show property sheet
			//
			CPropertySheet propSheet("Define grid", ::AfxGetMainWnd());
			CGridPropertyPage gridPage;

			CGrid gridx, gridy, gridz;
			pGrid->GetGrid(gridx, gridy, gridz);
			gridPage.m_grid[0] = gridx;
			gridPage.m_grid[1] = gridy;
			gridPage.m_grid[2] = gridz;

			gridPage.SetApply(pDoc);
			propSheet.AddPage(&gridPage);
			
			// support undo
			//
			CResizeGridAction* pAction = new CResizeGridAction(pDoc, gridPage.m_grid, pGrid);

			switch (propSheet.DoModal()) {
				case IDOK:
					////pDoc->SetFlowOnly(gridPage.m_bFlowOnly); // TODO: make undoable
					pAction->Reset(gridPage.m_grid);
					pDoc->Execute(pAction);
					break;
				case IDCANCEL:
					pAction->UnExecute();
					delete pAction;
					break;
				default:
					ASSERT(FALSE);
			}
			*pResult = TRUE;
			return;
		}
	}

	// MEDIA
	//
	if (item.IsNodeAncestor(this->m_nodeMedia)) {
		if (item != this->m_nodeMedia) {
			while (item.GetParent() != this->m_nodeMedia) {
				item = item.GetParent();
				if (!item) break;
			}
			if (item.GetData()) {
				if (CZoneActor* pZone = CZoneActor::SafeDownCast((vtkObject*)item.GetData())) {
					pZone->Edit(&this->m_wndTree);
					*pResult = TRUE;
					return;
				}
			}
		}
		return;
	}


	// BOUNDARY_CONDITIONS (first stress period)
	//
	if (item.IsNodeAncestor(this->m_nodeBC)) {
		if (item != this->m_nodeBC) {
			while (item.GetParent() != this->m_nodeBC) {
				item = item.GetParent();
				if (!item) break;
			}
			if (item.GetData()) {
				if (CZoneActor* pZone = CZoneActor::SafeDownCast((vtkObject*)item.GetData())) {
					pZone->Edit(&this->m_wndTree);
					*pResult = TRUE;
					return;
				}
			}
		}
		return;
	}

	// INITIAL_CONDITIONS
	//
	if (item.IsNodeAncestor(this->m_nodeIC)) {
		if (item != this->m_nodeIC) {
			while (item.GetParent() != this->m_nodeIC) {
				item = item.GetParent();
				if (!item) break;
			}
			if (item.GetData()) {
				if (CZoneActor* pZone = CZoneActor::SafeDownCast((vtkObject*)item.GetData())) {
					pZone->Edit(&this->m_wndTree);
					*pResult = TRUE;
					return;
				}
			}
		}
		return;
	}

	// PRINT_FREQUENCY
	//
	if (item.IsNodeAncestor(this->m_nodePF)) {
		if (this->m_nodePF.GetData()) {
			CPrintFreq* pPF = (CPrintFreq*)this->m_nodePF.GetData();
			pPF->Edit(&this->m_wndTree);
			*pResult = TRUE;
			return;
		}
		return;
	}


	// TIME_CONTROL
	//
	if (item.IsNodeAncestor(this->m_nodeTimeControl2))
	{
		if (this->m_nodeTimeControl2.GetData())
		{
			CTimeControl2* pTC = (CTimeControl2*)this->m_nodeTimeControl2.GetData();
			pTC->Edit(&this->m_wndTree);
			*pResult = TRUE;
			return;
		}
		return;
	}

// COMMENT: {4/8/2005 7:08:44 PM}	// Handle stress periods
// COMMENT: {4/8/2005 7:08:44 PM}	int nStressPeriodCount = this->GetStressPeriodCount();
// COMMENT: {4/8/2005 7:08:44 PM}	int nStressPeriod = 0;
// COMMENT: {4/8/2005 7:08:44 PM}	for (int i = 2; i <= nStressPeriodCount; ++i) {
// COMMENT: {4/8/2005 7:08:44 PM}		if (item.IsNodeAncestor(this->GetStressPeriodNode(i))) {
// COMMENT: {4/8/2005 7:08:44 PM}			if (item != this->GetStressPeriodNode(i)) {
// COMMENT: {4/8/2005 7:08:44 PM}				nStressPeriod = i;
// COMMENT: {4/8/2005 7:08:44 PM}			}
// COMMENT: {4/8/2005 7:08:44 PM}			break;
// COMMENT: {4/8/2005 7:08:44 PM}		}
// COMMENT: {4/8/2005 7:08:44 PM}	}
// COMMENT: {4/8/2005 7:08:44 PM}	if (nStressPeriod > 1) {
// COMMENT: {4/8/2005 7:08:44 PM}		// BOUNDARY_CONDITIONS
// COMMENT: {4/8/2005 7:08:44 PM}		//
// COMMENT: {4/8/2005 7:08:44 PM}		if (item.IsNodeAncestor(this->GetBCNode(nStressPeriod))) {
// COMMENT: {4/8/2005 7:08:44 PM}			if (item != this->GetBCNode(nStressPeriod)) {
// COMMENT: {4/8/2005 7:08:44 PM}				while (item.GetParent() != this->GetBCNode(nStressPeriod)) {
// COMMENT: {4/8/2005 7:08:44 PM}					item = item.GetParent();
// COMMENT: {4/8/2005 7:08:44 PM}					if (!item) break;
// COMMENT: {4/8/2005 7:08:44 PM}				}
// COMMENT: {4/8/2005 7:08:44 PM}				if (item && item.GetData()) {
// COMMENT: {4/8/2005 7:08:44 PM}					if (CBCZoneActor* pZone = CBCZoneActor::SafeDownCast((vtkObject*)item.GetData())) {
// COMMENT: {4/8/2005 7:08:44 PM}						pZone->Edit(&this->m_wndTree, nStressPeriod);
// COMMENT: {4/8/2005 7:08:44 PM}						*pResult = TRUE;
// COMMENT: {4/8/2005 7:08:44 PM}						return;
// COMMENT: {4/8/2005 7:08:44 PM}					}
// COMMENT: {4/8/2005 7:08:44 PM}				}
// COMMENT: {4/8/2005 7:08:44 PM}			}
// COMMENT: {4/8/2005 7:08:44 PM}			TRACE("BC not found\n");
// COMMENT: {4/8/2005 7:08:44 PM}			return;
// COMMENT: {4/8/2005 7:08:44 PM}		}
// COMMENT: {4/8/2005 7:08:44 PM}
// COMMENT: {4/8/2005 7:08:44 PM}		// TIME_CONTROL
// COMMENT: {4/8/2005 7:08:44 PM}		//
// COMMENT: {4/8/2005 7:08:44 PM}		if (item.IsNodeAncestor(this->GetTimeControlNode(nStressPeriod))) {
// COMMENT: {4/8/2005 7:08:44 PM}			if (this->GetTimeControlNode(nStressPeriod).GetData()) {
// COMMENT: {4/8/2005 7:08:44 PM}				CTimeControl* pTC = (CTimeControl*)this->GetTimeControlNode(nStressPeriod).GetData();
// COMMENT: {4/8/2005 7:08:44 PM}				pTC->EditMultiple(this);
// COMMENT: {4/8/2005 7:08:44 PM}				*pResult = TRUE;
// COMMENT: {4/8/2005 7:08:44 PM}				return;
// COMMENT: {4/8/2005 7:08:44 PM}			}
// COMMENT: {4/8/2005 7:08:44 PM}			return;
// COMMENT: {4/8/2005 7:08:44 PM}		}
// COMMENT: {4/8/2005 7:08:44 PM}		::AfxMessageBox("Edit Node Greater than simulation 1 BUT NOT FOUND(time_control, bc)");
// COMMENT: {4/8/2005 7:08:44 PM}		*pResult = TRUE;
// COMMENT: {4/8/2005 7:08:44 PM}		return;
// COMMENT: {4/8/2005 7:08:44 PM}	}

	//// handle zones
	////
	//if (hItem && m_wndTree.GetItemData(hItem)) {
	//	if (CZoneActor* pZone = CZoneActor::SafeDownCast((vtkObject*)m_wndTree.GetItemData(hItem))) {
	//		pZone->Edit(&this->m_wndTree);
	//		*pResult = TRUE;
	//		return;
	//	}
	//}
}

//vtkPropCollection* CPropertyTreeControlBar::GetPropCollection() const
//{
//	/// ASSERT(this->m_pPropCollection != 0);
//	return this->m_pPropCollection;
//}

//float* CPropertyTreeControlBar::GetGridBounds()
//{
//	if (this->m_pGridLODActor) {
//		return this->m_pGridLODActor->GetBounds();
//	}
//	else {
//		ASSERT(FALSE);
//	}
//	return 0;
//}

///{{{

HBITMAP FAR PASCAL CreateColorBitmap(int cx, int cy)
{
    HBITMAP hbm;
    HDC hdc;

    hdc = GetDC(NULL);

    //
    // on a multimonitor system with mixed bitdepths
    // always use a 32bit bitmap for our work buffer
    // this will prevent us from losing colors when
    // blting to and from the screen.  this is mainly
    // important for the drag & drop offscreen buffers.
    //
// COMMENT: {9/8/2004 10:48:49 PM}    if (!(GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE) &&
// COMMENT: {9/8/2004 10:48:49 PM}		TRUE &&
// COMMENT: {9/8/2004 10:48:49 PM}		TRUE)
// COMMENT: {9/8/2004 10:48:49 PM}// COMMENT: {9/8/2004 8:42:09 PM}        GetSystemMetrics(SM_CMONITORS) > 1 &&
// COMMENT: {9/8/2004 10:48:49 PM}// COMMENT: {9/8/2004 8:42:09 PM}        GetSystemMetrics(SM_SAMEDISPLAYFORMAT) == 0)
// COMMENT: {9/8/2004 10:48:49 PM}    {
// COMMENT: {9/8/2004 10:48:49 PM}        LPVOID p;
// COMMENT: {9/8/2004 10:48:49 PM}#ifndef UNIX
// COMMENT: {9/8/2004 10:48:49 PM}        BITMAPINFO bi = {sizeof(BITMAPINFOHEADER), cx, cy, 1, 32};
// COMMENT: {9/8/2004 10:48:49 PM}#else
// COMMENT: {9/8/2004 10:48:49 PM}        BITMAPINFO bi;
// COMMENT: {9/8/2004 10:48:49 PM}        bi.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER); 
// COMMENT: {9/8/2004 10:48:49 PM}        bi.bmiHeader.biWidth    = cx;
// COMMENT: {9/8/2004 10:48:49 PM}        bi.bmiHeader.biHeight   = cy;
// COMMENT: {9/8/2004 10:48:49 PM}        bi.bmiHeader.biPlanes   = 1 ;
// COMMENT: {9/8/2004 10:48:49 PM}        bi.bmiHeader.biBitCount = 32;
// COMMENT: {9/8/2004 10:48:49 PM}#endif
// COMMENT: {9/8/2004 10:48:49 PM}        hbm = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &p, NULL, 0);
// COMMENT: {9/8/2004 10:48:49 PM}    }
// COMMENT: {9/8/2004 10:48:49 PM}    else
    {
        hbm = CreateCompatibleBitmap(hdc, cx, cy);
    }

    ReleaseDC(NULL, hdc);
    return hbm;
}
#include <uxtheme.h>
#include <tmschema.h>
HIMAGELIST CreateCheckBoxImagelist(HIMAGELIST himl, BOOL fTree, BOOL fUseColorKey, BOOL fMirror)
{
    int cxImage, cyImage;
    HBITMAP hbm;
    HBITMAP hbmTemp;
    COLORREF clrMask;
	HDC hdcDesk = ::GetDC(NULL);
    HDC hdc;
    RECT rc;
    int nImages = fTree ? 3 : 2;

    if (!hdcDesk)
        return NULL;

	hdc = ::CreateCompatibleDC(hdcDesk);
	::ReleaseDC(NULL, hdcDesk);

    if (!hdc)
        return NULL;

    // Must protect against ImageList_GetIconSize failing in case app
    // gave us a bad himl
	if (himl && ::ImageList_GetIconSize(himl, &cxImage, &cyImage)) {
        // cxImage and cyImage are okay
    } else {
		cxImage = /*SRC g_cxSmIcon SRC*/::GetSystemMetrics(SM_CXSMICON);
		cyImage = /*SRC g_cySmIcon SRC*/::GetSystemMetrics(SM_CYSMICON);
    }

	himl = ::ImageList_Create(cxImage, cyImage, ILC_MASK, 0, nImages);
	hbm = ::CreateColorBitmap(cxImage * nImages, cyImage);

    if (fUseColorKey)
    {
        clrMask = RGB(255,000,255); // magenta
		if (clrMask == /*SRC g_clrWindow SRC*/::GetSysColor(COLOR_WINDOW))
            clrMask = RGB(000,000,255); // blue
    }
    else
    {
        clrMask = /*SRC g_clrWindow SRC*/::GetSysColor(COLOR_WINDOW);
    }

    // fill
// COMMENT: {9/8/2004 8:42:56 PM}    hbmTemp = SelectObject(hdc, hbm);
	hbmTemp = (HBITMAP)::SelectObject(hdc, hbm);

    rc.left = rc.top = 0;
    rc.bottom = cyImage;
    rc.right = cxImage * nImages;
    // SRC FillRectClr(hdc, &rc, clrMask);
	::FillRect(hdc, &rc, ::CreateSolidBrush(clrMask));

    rc.right = cxImage;
    // now draw the real controls on
	::InflateRect(&rc, -/*SRC g_cxEdge SRC*/::GetSystemMetrics(SM_CXEDGE), -/*SRC g_cyEdge SRC*/::GetSystemMetrics(SM_CYEDGE));
    rc.right++;
    rc.bottom++;

    if (fTree)
		::OffsetRect(&rc, cxImage, 0);

//{{
	HTHEME hTheme = ::OpenThemeData(NULL, L"Button");
	if (hTheme)
	{
		VERIFY(::DrawThemeBackground(hTheme,
			hdc,
			BP_CHECKBOX,
			CBS_UNCHECKEDNORMAL,
			&rc,
			NULL) == S_OK);

// COMMENT: {9/8/2004 10:42:49 PM}		//{{
// COMMENT: {9/8/2004 10:42:49 PM}		RECT rc2;
// COMMENT: {9/8/2004 10:42:49 PM}		rc2.left = rc2.top = 0;
// COMMENT: {9/8/2004 10:42:49 PM}		rc2.bottom = cyImage;
// COMMENT: {9/8/2004 10:42:49 PM}		rc2.right = cxImage * nImages;
// COMMENT: {9/8/2004 10:42:49 PM}
// COMMENT: {9/8/2004 10:42:49 PM}		VERIFY(::DrawThemeIcon(hTheme,
// COMMENT: {9/8/2004 10:42:49 PM}			hdc,
// COMMENT: {9/8/2004 10:42:49 PM}			BP_CHECKBOX,
// COMMENT: {9/8/2004 10:42:49 PM}			CBS_UNCHECKEDNORMAL,
// COMMENT: {9/8/2004 10:42:49 PM}			&rc2,
// COMMENT: {9/8/2004 10:42:49 PM}			himl,
// COMMENT: {9/8/2004 10:42:49 PM}			1) == S_OK);
// COMMENT: {9/8/2004 10:42:49 PM}		//}}
	}
	else
	{
		::DrawFrameControl(hdc, &rc, DFC_BUTTON, DFCS_BUTTONCHECK | DFCS_FLAT | 
			(fUseColorKey? 0 : /*SRC DFCS_TRANSPARENT SRC*/0));
	}
//}}

	::OffsetRect(&rc, cxImage, 0);
    // [msadek]; For the mirrored case, there is an off-by-one somewhere in MirrorIcon() or System API.
    // Since I will not be touching MirrorIcon() by any mean and no chance to fix a system API,
    // let's compensate for it here.
    if(fMirror)
    {
        OffsetRect(&rc, -1, 0);  
    }

// COMMENT: {4/6/2005 6:59:30 PM}	if (hTheme)
// COMMENT: {4/6/2005 6:59:30 PM}	{
// COMMENT: {4/6/2005 6:59:30 PM}		::DrawThemeBackground(hTheme,
// COMMENT: {4/6/2005 6:59:30 PM}			hdc,
// COMMENT: {4/6/2005 6:59:30 PM}			BP_CHECKBOX,
// COMMENT: {4/6/2005 6:59:30 PM}			CBS_CHECKEDNORMAL,
// COMMENT: {4/6/2005 6:59:30 PM}			&rc,
// COMMENT: {4/6/2005 6:59:30 PM}			NULL);
// COMMENT: {4/6/2005 6:59:30 PM}	}
// COMMENT: {4/6/2005 6:59:30 PM}	else
	{
		::DrawFrameControl(hdc, &rc, DFC_BUTTON, DFCS_BUTTONCHECK | DFCS_FLAT | DFCS_CHECKED | 
			(fUseColorKey? 0 : /*SRC DFCS_TRANSPARENT SRC*/0));
	}

	::SelectObject(hdc, hbmTemp);

    if (fUseColorKey)
    {
		::ImageList_AddMasked(himl, hbm, clrMask);
    }
    else
    {
		::ImageList_Add(himl, hbm, NULL);
    }

// COMMENT: {9/8/2004 8:40:16 PM}    if(fMirror)
// COMMENT: {9/8/2004 8:40:16 PM}    {
// COMMENT: {9/8/2004 8:40:16 PM}        HICON hIcon = ImageList_ExtractIcon(0, himl, nImages-1);
// COMMENT: {9/8/2004 8:40:16 PM}        MirrorIcon(&hIcon, NULL);
// COMMENT: {9/8/2004 8:40:16 PM}        ImageList_ReplaceIcon(himl, nImages-1, hIcon);
// COMMENT: {9/8/2004 8:40:16 PM}    }

	//{{
	::CloseThemeData(hTheme);
	//}}

	::DeleteDC(hdc);
	::DeleteObject( hbm );
    return himl;
}
///}}}

int CPropertyTreeControlBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBarCFVS7::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndTree.Create(WS_CHILD|WS_VISIBLE|
		TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT|TVS_SHOWSELALWAYS,
		CRect(0, 0, 0, 0), this, IDC_PROPERTY_TREE))
	{
		TRACE0("Failed to create instant bar child\n");
		return -1;		// fail to create
	}
	m_wndTree.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

// COMMENT: {4/6/2005 6:49:11 PM}	// setup image list
// COMMENT: {4/6/2005 6:49:11 PM}	//
// COMMENT: {4/6/2005 6:49:11 PM}	this->m_pImageList = new CImageList();
// COMMENT: {4/6/2005 6:49:11 PM}	ASSERT_VALID(this->m_pImageList);
// COMMENT: {4/6/2005 6:49:11 PM}#if defined(_DEBUG)
// COMMENT: {4/6/2005 6:49:11 PM}	this->m_pImageList->Create(IDB_CHECKS_TREE, 14, 0, RGB(255, 0, 255));
// COMMENT: {4/6/2005 6:49:11 PM}#else
// COMMENT: {4/6/2005 6:49:11 PM}	// this->m_pImageList->Create(IDB_EYES_TREE3, 16, 0, RGB(255, 0, 255));
// COMMENT: {4/6/2005 6:49:11 PM}	this->m_pImageList->Create(IDB_CHECKS_TREE, 14, 0, RGB(255, 0, 255));
// COMMENT: {4/6/2005 6:49:11 PM}#endif
// COMMENT: {4/6/2005 6:49:11 PM}
// COMMENT: {4/6/2005 6:49:11 PM}#if 0
	this->m_pImageList = new CImageList();
	this->m_pImageList->Attach(CreateCheckBoxImagelist(this->m_pImageList->GetSafeHandle(), TRUE, FALSE, FALSE));
// COMMENT: {4/6/2005 6:49:14 PM}#endif

	this->m_wndTree.SetImageList(this->m_pImageList, LVSIL_STATE);

	
	// bring the tooltips to front
	CWnd* pTT = FromHandle((HWND) m_wndTree.SendMessage(TVM_GETTOOLTIPS));
	if (pTT != NULL) {
		pTT->SetWindowPos(&wndTopMost, 0, 0, 0, 0,
		SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	}
	return 0;
}

void CPropertyTreeControlBar::SetUnits(CUnits* pUnits)
{
	pUnits->Insert(&this->m_wndTree, this->m_nodeUnits);
	ASSERT(reinterpret_cast<CUnits*>(this->m_nodeUnits.GetData()) == pUnits);
}

void CPropertyTreeControlBar::SetFlowOnly(CFlowOnly* pFlowOnly)
{
	pFlowOnly->Insert(&this->m_wndTree, this->m_nodeFlowOnly);
}

void CPropertyTreeControlBar::SetFreeSurface(CFreeSurface *pFreeSurface)
{
	pFreeSurface->Insert(&this->m_wndTree, this->m_nodeFreeSurface);
}

void CPropertyTreeControlBar::SetSteadyFlow(CSteadyFlow *pSteadyFlow)
{
	pSteadyFlow->Insert(&this->m_wndTree, this->m_nodeSteadyFlow);
}

void CPropertyTreeControlBar::SetModel(CNewModel* pModel)
{
	this->SetFlowOnly(&pModel->m_flowOnly);
	this->SetFreeSurface(&pModel->m_freeSurface);
	this->SetSteadyFlow(&pModel->m_steadyFlow);
}

void CPropertyTreeControlBar::SetTimeControl2(CTimeControl2* pTimeControl2)
{
	pTimeControl2->Insert(&this->m_wndTree, this->m_nodeTimeControl2);
}

// COMMENT: {4/11/2005 1:10:33 PM}void CPropertyTreeControlBar::SetTimeControl(const CTimeControl& timeControl, int nStressPeriod)
// COMMENT: {4/11/2005 1:10:33 PM}{
// COMMENT: {4/11/2005 1:10:33 PM}	CTreeCtrlNode nodeTC = this->GetTimeControlNode(nStressPeriod);
// COMMENT: {4/11/2005 1:10:33 PM}	ASSERT(nodeTC.GetData());
// COMMENT: {4/11/2005 1:10:33 PM}	if (CTimeControl* pTimeControl = (CTimeControl*)(nodeTC.GetData())) {
// COMMENT: {4/11/2005 1:10:33 PM}		(*pTimeControl) = timeControl;
// COMMENT: {4/11/2005 1:10:33 PM}		pTimeControl->Insert(&this->m_wndTree, pTimeControl->GetTreeItem());
// COMMENT: {4/11/2005 1:10:33 PM}	}
// COMMENT: {4/11/2005 1:10:33 PM}}

void CPropertyTreeControlBar::SetPrintFrequency(CPrintFreq* pPrintFreq)
{
	pPrintFreq->Insert(&this->m_wndTree, this->m_nodePF);
}

// COMMENT: {4/11/2005 1:10:15 PM}void CPropertyTreeControlBar::SetPrintFrequency(const CPrintFreq& printFreq, int nStressPeriod)
// COMMENT: {4/11/2005 1:10:15 PM}{
// COMMENT: {4/11/2005 1:10:15 PM}	CTreeCtrlNode nodePF = this->GetPrintFrequencyNode(nStressPeriod);
// COMMENT: {4/11/2005 1:10:15 PM}	ASSERT(nodePF.GetData());
// COMMENT: {4/11/2005 1:10:15 PM}	if (CPrintFreq* pPrintFreq = (CPrintFreq*)(nodePF.GetData())) {
// COMMENT: {4/11/2005 1:10:15 PM}		(*pPrintFreq) = printFreq;
// COMMENT: {4/11/2005 1:10:15 PM}		pPrintFreq->Insert(&this->m_wndTree, pPrintFreq->GetTreeItem());
// COMMENT: {4/11/2005 1:10:15 PM}	}
// COMMENT: {4/11/2005 1:10:15 PM}}

void CPropertyTreeControlBar::AddZone(CZoneActor* pZone)
{
	this->m_bSelectingProp = TRUE; // HACK
	ASSERT(pZone);
	pZone->Insert(this);
	this->m_wndTree.RedrawWindow();
	++this->m_nNextZone;
	this->m_bSelectingProp = FALSE; // HACK
}

void CPropertyTreeControlBar::RemoveZone(CZoneActor* pZone)
{
	ASSERT(pZone);
	pZone->Remove(this);
	m_wndTree.RedrawWindow();
}

// COMMENT: {4/8/2005 7:02:52 PM}CTreeCtrlNode CPropertyTreeControlBar::GetBCNode(int nStressPeriod)
// COMMENT: {4/8/2005 7:02:52 PM}{
// COMMENT: {4/8/2005 7:02:52 PM}	ASSERT(nStressPeriod > 0);
// COMMENT: {4/8/2005 7:02:52 PM}	ASSERT(nStressPeriod <= this->GetStressPeriodCount());
// COMMENT: {4/8/2005 7:02:52 PM}
// COMMENT: {4/8/2005 7:02:52 PM}	if (nStressPeriod == 1)	return this->m_nodeBC;
// COMMENT: {4/8/2005 7:02:52 PM}
// COMMENT: {4/8/2005 7:02:52 PM}	CTreeCtrlNode item = this->GetStressPeriodNode(nStressPeriod).GetChildAt(BC_INDEX);
// COMMENT: {4/8/2005 7:02:52 PM}	ASSERT(item.GetText().Compare(this->m_nodeBC.GetText()) == 0);
// COMMENT: {4/8/2005 7:02:52 PM}	return item;
// COMMENT: {4/8/2005 7:02:52 PM}}
CTreeCtrlNode CPropertyTreeControlBar::GetBCNode(void)
{
	return this->m_nodeBC;
}

// COMMENT: {4/8/2005 7:00:15 PM}CTreeCtrlNode CPropertyTreeControlBar::GetTimeControlNode(int nStressPeriod)
// COMMENT: {4/8/2005 7:00:15 PM}{
// COMMENT: {4/8/2005 7:00:15 PM}	ASSERT(nStressPeriod > 0);
// COMMENT: {4/8/2005 7:00:15 PM}	ASSERT(nStressPeriod <= this->GetStressPeriodCount());
// COMMENT: {4/8/2005 7:00:15 PM}
// COMMENT: {4/8/2005 7:00:15 PM}	if (nStressPeriod == 1)	return this->m_nodeTimeControl;
// COMMENT: {4/8/2005 7:00:15 PM}
// COMMENT: {4/8/2005 7:00:15 PM}	CTreeCtrlNode item = this->GetStressPeriodNode(nStressPeriod).GetChildAt(TIME_CONTROL_INDEX);
// COMMENT: {4/8/2005 7:00:15 PM}	ASSERT(item.GetText().Compare(this->m_nodeTimeControl.GetText()) == 0);
// COMMENT: {4/8/2005 7:00:15 PM}	return item;
// COMMENT: {4/8/2005 7:00:15 PM}}
CTreeCtrlNode CPropertyTreeControlBar::GetTimeControlNode(void)
{
	return this->m_nodeTimeControl;
}

CTreeCtrlNode CPropertyTreeControlBar::GetTimeControl2Node(void)
{
	return this->m_nodeTimeControl2;
}

// COMMENT: {4/8/2005 7:03:15 PM}CTreeCtrlNode CPropertyTreeControlBar::GetPrintFrequencyNode(int nStressPeriod)
// COMMENT: {4/8/2005 7:03:15 PM}{
// COMMENT: {4/8/2005 7:03:15 PM}	ASSERT(nStressPeriod > 0);
// COMMENT: {4/8/2005 7:03:15 PM}	ASSERT(nStressPeriod <= this->GetStressPeriodCount());
// COMMENT: {4/8/2005 7:03:15 PM}
// COMMENT: {4/8/2005 7:03:15 PM}	if (nStressPeriod == 1)	return this->m_nodePF;
// COMMENT: {4/8/2005 7:03:15 PM}
// COMMENT: {4/8/2005 7:03:15 PM}	CTreeCtrlNode item = this->GetStressPeriodNode(nStressPeriod).GetChildAt(PRINT_FREQUENCY_INDEX);
// COMMENT: {4/8/2005 7:03:15 PM}	ASSERT(item.GetText().Compare(this->m_nodePF.GetText()) == 0);
// COMMENT: {4/8/2005 7:03:15 PM}	return item;
// COMMENT: {4/8/2005 7:03:15 PM}}
CTreeCtrlNode CPropertyTreeControlBar::GetPrintFrequencyNode(void)
{
	return this->m_nodePF;
}


// COMMENT: {4/11/2005 1:09:44 PM}CTimeControl* CPropertyTreeControlBar::GetTimeControl(int nStressPeriod)
// COMMENT: {4/11/2005 1:09:44 PM}{
// COMMENT: {4/11/2005 1:09:44 PM}	CTreeCtrlNode node = this->GetTimeControlNode(nStressPeriod);
// COMMENT: {4/11/2005 1:09:44 PM}	ASSERT(node.GetData());
// COMMENT: {4/11/2005 1:09:44 PM}	return (CTimeControl*)node.GetData();
// COMMENT: {4/11/2005 1:09:44 PM}}
CTimeControl2* CPropertyTreeControlBar::GetTimeControl2(void)
{
	CTreeCtrlNode node = this->GetTimeControl2Node();
	ASSERT(node.GetData());
	return (CTimeControl2*)node.GetData();
}

// COMMENT: {4/11/2005 1:08:48 PM}CPrintFreq* CPropertyTreeControlBar::GetPrintFrequency(int nStressPeriod)
// COMMENT: {4/11/2005 1:08:48 PM}{
// COMMENT: {4/11/2005 1:08:48 PM}	CTreeCtrlNode node = this->GetPrintFrequencyNode(nStressPeriod);
// COMMENT: {4/11/2005 1:08:48 PM}	ASSERT(node.GetData());
// COMMENT: {4/11/2005 1:08:48 PM}	return (CPrintFreq*)node.GetData();
// COMMENT: {4/11/2005 1:08:48 PM}}
CPrintFreq* CPropertyTreeControlBar::GetPrintFrequency(void)
{
	CTreeCtrlNode node = this->GetPrintFrequencyNode();
	ASSERT(node.GetData());
	return (CPrintFreq*)node.GetData();
}

/**
CTreeCtrlNode CPropertyTreeControlBar::_InsertStressPeriod(const CTimeControl& timeControl, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
}
**/

// COMMENT: {4/8/2005 6:52:24 PM}CTreeCtrlNode CPropertyTreeControlBar::AddStressPeriod(const CTimeControl& timeControl)
// COMMENT: {4/8/2005 6:52:24 PM}{
// COMMENT: {4/8/2005 6:52:24 PM}	ASSERT(FALSE); // FIXME
// COMMENT: {4/8/2005 6:52:24 PM}	return CTreeCtrlNode();
// COMMENT: {4/8/2005 6:52:24 PM}
// COMMENT: {4/8/2005 6:52:24 PM}	CTimeControl* pTimeControl = new CTimeControl(timeControl);
// COMMENT: {4/8/2005 6:52:24 PM}
// COMMENT: {4/8/2005 6:52:24 PM}	int nStressPeriod = this->GetStressPeriodCount() + 1;
// COMMENT: {4/8/2005 6:52:24 PM}
// COMMENT: {4/8/2005 6:52:24 PM}	CString str;
// COMMENT: {4/8/2005 6:52:24 PM}	str.Format(szPeriodFormat, nStressPeriod);
// COMMENT: {4/8/2005 6:52:24 PM}	CTreeCtrlNode nodeSP = this->m_wndTree.InsertItem(str);
// COMMENT: {4/8/2005 6:52:24 PM}
// COMMENT: {4/8/2005 6:52:24 PM}	/*
// COMMENT: {4/8/2005 6:52:24 PM}	HTREEITEM InsertItem(
// COMMENT: {4/8/2005 6:52:24 PM}	LPCTSTR lpszItem,
// COMMENT: {4/8/2005 6:52:24 PM}	HTREEITEM hParent = TVI_ROOT,
// COMMENT: {4/8/2005 6:52:24 PM}	HTREEITEM hInsertAfter = TVI_LAST 
// COMMENT: {4/8/2005 6:52:24 PM}	);
// COMMENT: {4/8/2005 6:52:24 PM}	*/
// COMMENT: {4/8/2005 6:52:24 PM}
// COMMENT: {4/8/2005 6:52:24 PM}	// BOUNDARY_CONDITIONS
// COMMENT: {4/8/2005 6:52:24 PM}	//
// COMMENT: {4/8/2005 6:52:24 PM}	CTreeCtrlNode nodeBC = m_wndTree.InsertItem(szBOUNDARY_CONDITIONS, nodeSP);
// COMMENT: {4/8/2005 6:52:24 PM}	int nCount = this->m_nodeBC.GetChildCount();
// COMMENT: {4/8/2005 6:52:24 PM}	for (int i = 0; i < nCount; ++i) {
// COMMENT: {4/8/2005 6:52:24 PM}		CTreeCtrlNode child = this->m_nodeBC.GetChildAt(i);
// COMMENT: {4/8/2005 6:52:24 PM}		ASSERT(child.GetData());
// COMMENT: {4/8/2005 6:52:24 PM}		if (child.GetData()) {
// COMMENT: {4/8/2005 6:52:24 PM}			if (CBCZoneActor* pZone = CBCZoneActor::SafeDownCast((vtkObject*)child.GetData())) {
// COMMENT: {4/8/2005 6:52:24 PM}				pZone->AddStressPeriod(this, nStressPeriod);
// COMMENT: {4/8/2005 6:52:24 PM}			}
// COMMENT: {4/8/2005 6:52:24 PM}			else {
// COMMENT: {4/8/2005 6:52:24 PM}				ASSERT(FALSE);
// COMMENT: {4/8/2005 6:52:24 PM}			}
// COMMENT: {4/8/2005 6:52:24 PM}		}
// COMMENT: {4/8/2005 6:52:24 PM}	}
// COMMENT: {4/8/2005 6:52:24 PM}
// COMMENT: {4/8/2005 6:52:24 PM}	// PRINT_FREQUENCY
// COMMENT: {4/8/2005 6:52:24 PM}	//
// COMMENT: {4/8/2005 6:52:24 PM}	CTreeCtrlNode nodePF = this->m_wndTree.InsertItem(szPRINT_FREQUENCY, nodeSP);
// COMMENT: {4/8/2005 6:52:24 PM}	CPrintFreq* pPrintFreq = new CPrintFreq();
// COMMENT: {4/8/2005 6:52:24 PM}	(*pPrintFreq) = (*this->GetPrintFrequency(nStressPeriod - 1));
// COMMENT: {4/8/2005 6:52:24 PM}	pPrintFreq->Insert(&this->m_wndTree, nodePF);
// COMMENT: {4/8/2005 6:52:24 PM}
// COMMENT: {4/8/2005 6:52:24 PM}	// TIME_CONTROL
// COMMENT: {4/8/2005 6:52:24 PM}	//
// COMMENT: {4/8/2005 6:52:24 PM}	CTreeCtrlNode nodeTimeControl = this->m_wndTree.InsertItem(szTIME_CONTROL, nodeSP);
// COMMENT: {4/8/2005 6:52:24 PM}	pTimeControl->Insert(&this->m_wndTree, nodeTimeControl);
// COMMENT: {4/8/2005 6:52:24 PM}
// COMMENT: {4/8/2005 6:52:24 PM}	return nodeSP;
// COMMENT: {4/8/2005 6:52:24 PM}}

// COMMENT: {4/8/2005 6:52:34 PM}void CPropertyTreeControlBar::RemoveStressPeriod(int nStressPeriod)
// COMMENT: {4/8/2005 6:52:34 PM}{
// COMMENT: {4/8/2005 6:52:34 PM}	CTreeCtrlNode nodeSP = this->GetStressPeriodNode(nStressPeriod);
// COMMENT: {4/8/2005 6:52:34 PM}	ASSERT(nodeSP);
// COMMENT: {4/8/2005 6:52:34 PM}	if (nodeSP) {
// COMMENT: {4/8/2005 6:52:34 PM}#ifdef _DEBUG
// COMMENT: {4/8/2005 6:52:34 PM}		CString str;
// COMMENT: {4/8/2005 6:52:34 PM}		str.Format(szPeriodFormat, nStressPeriod);
// COMMENT: {4/8/2005 6:52:34 PM}		ASSERT(nodeSP.GetText().Compare(str) == 0);
// COMMENT: {4/8/2005 6:52:34 PM}#endif
// COMMENT: {4/8/2005 6:52:34 PM}		// free TIME_CONTROL 
// COMMENT: {4/8/2005 6:52:34 PM}		CTreeCtrlNode nodeTC = this->GetTimeControlNode(nStressPeriod);
// COMMENT: {4/8/2005 6:52:34 PM}		ASSERT(nodeTC.GetData());
// COMMENT: {4/8/2005 6:52:34 PM}		if (CTimeControl* pTimeControl = (CTimeControl*)(nodeTC.GetData())) {
// COMMENT: {4/8/2005 6:52:34 PM}			delete pTimeControl;
// COMMENT: {4/8/2005 6:52:34 PM}			nodeTC.SetData(0);
// COMMENT: {4/8/2005 6:52:34 PM}		}
// COMMENT: {4/8/2005 6:52:34 PM}		nodeTC.Delete();
// COMMENT: {4/8/2005 6:52:34 PM}
// COMMENT: {4/8/2005 6:52:34 PM}		// PRINT_FREQUENCY
// COMMENT: {4/8/2005 6:52:34 PM}		//		
// COMMENT: {4/8/2005 6:52:34 PM}		CTreeCtrlNode nodePF = this->GetPrintFrequencyNode(nStressPeriod);
// COMMENT: {4/8/2005 6:52:34 PM}		ASSERT(nodePF.GetData());
// COMMENT: {4/8/2005 6:52:34 PM}		if (CPrintFreq* pPrintFreq = (CPrintFreq*)(nodePF.GetData())) {
// COMMENT: {4/8/2005 6:52:34 PM}			delete pPrintFreq;
// COMMENT: {4/8/2005 6:52:34 PM}			nodePF.SetData(0);
// COMMENT: {4/8/2005 6:52:34 PM}		}
// COMMENT: {4/8/2005 6:52:34 PM}		nodePF.Delete();
// COMMENT: {4/8/2005 6:52:34 PM}
// COMMENT: {4/8/2005 6:52:34 PM}		// BOUNDARY_CONDITIONS
// COMMENT: {4/8/2005 6:52:34 PM}		//
// COMMENT: {4/8/2005 6:52:34 PM}		CTreeCtrlNode nodeBC = this->GetBCNode(nStressPeriod);
// COMMENT: {4/8/2005 6:52:34 PM}		int nCount = this->m_nodeBC.GetChildCount();
// COMMENT: {4/8/2005 6:52:34 PM}		for (int i = 0; i < nCount; ++i) {
// COMMENT: {4/8/2005 6:52:34 PM}			CTreeCtrlNode child = this->m_nodeBC.GetChildAt(i);
// COMMENT: {4/8/2005 6:52:34 PM}			ASSERT(child.GetData());
// COMMENT: {4/8/2005 6:52:34 PM}			if (child.GetData()) {
// COMMENT: {4/8/2005 6:52:34 PM}				if (CBCZoneActor* pZone = CBCZoneActor::SafeDownCast((vtkObject*)child.GetData())) {
// COMMENT: {4/8/2005 6:52:34 PM}					pZone->RemoveStressPeriod(this, nStressPeriod);
// COMMENT: {4/8/2005 6:52:34 PM}				}
// COMMENT: {4/8/2005 6:52:34 PM}				else {
// COMMENT: {4/8/2005 6:52:34 PM}					ASSERT(FALSE);
// COMMENT: {4/8/2005 6:52:34 PM}				}
// COMMENT: {4/8/2005 6:52:34 PM}			}
// COMMENT: {4/8/2005 6:52:34 PM}		}
// COMMENT: {4/8/2005 6:52:34 PM}		ASSERT(nodeBC.GetChildCount() == 0);
// COMMENT: {4/8/2005 6:52:34 PM}		nodeBC.Delete();
// COMMENT: {4/8/2005 6:52:34 PM}	}
// COMMENT: {4/8/2005 6:52:34 PM}	ASSERT(nodeSP.GetChildCount() == 0);
// COMMENT: {4/8/2005 6:52:34 PM}	nodeSP.Delete();
// COMMENT: {4/8/2005 6:52:34 PM}}


// COMMENT: {4/8/2005 6:49:05 PM}int CPropertyTreeControlBar::GetStressPeriodCount(void)
// COMMENT: {4/8/2005 6:49:05 PM}{
// COMMENT: {4/8/2005 6:49:05 PM}	int nCount = 0;
// COMMENT: {4/8/2005 6:49:05 PM}	CTreeCtrlNode node = this->m_nodeSP1;
// COMMENT: {4/8/2005 6:49:05 PM}	for (; node; node = node.GetNextSibling()) {
// COMMENT: {4/8/2005 6:49:05 PM}		++nCount;
// COMMENT: {4/8/2005 6:49:05 PM}#ifdef _DEBUG
// COMMENT: {4/8/2005 6:49:05 PM}		CString str;
// COMMENT: {4/8/2005 6:49:05 PM}		str.Format(szPeriodFormat, nCount);
// COMMENT: {4/8/2005 6:49:05 PM}		ASSERT(node.GetText().Compare(str) == 0);
// COMMENT: {4/8/2005 6:49:05 PM}#endif
// COMMENT: {4/8/2005 6:49:05 PM}	}
// COMMENT: {4/8/2005 6:49:05 PM}	return nCount;
// COMMENT: {4/8/2005 6:49:05 PM}}

// COMMENT: {4/8/2005 6:49:11 PM}CTreeCtrlNode CPropertyTreeControlBar::GetStressPeriodNode(int nStressPeriod)
// COMMENT: {4/8/2005 6:49:11 PM}{
// COMMENT: {4/8/2005 6:49:11 PM}	ASSERT(0 < nStressPeriod && nStressPeriod <= this->GetStressPeriodCount());
// COMMENT: {4/8/2005 6:49:11 PM}
// COMMENT: {4/8/2005 6:49:11 PM}	CTreeCtrlNode nodeSP = this->m_nodeSP1; // Stress Period 1
// COMMENT: {4/8/2005 6:49:11 PM}	for (int i = 1; i < nStressPeriod; ++i) {
// COMMENT: {4/8/2005 6:49:11 PM}		nodeSP = nodeSP.GetNextSibling(); // Stress Period i+1
// COMMENT: {4/8/2005 6:49:11 PM}		ASSERT(nodeSP);
// COMMENT: {4/8/2005 6:49:11 PM}	}
// COMMENT: {4/8/2005 6:49:11 PM}#ifdef _DEBUG
// COMMENT: {4/8/2005 6:49:11 PM}	CString str;
// COMMENT: {4/8/2005 6:49:11 PM}	str.Format(szPeriodFormat, nStressPeriod);
// COMMENT: {4/8/2005 6:49:11 PM}	ASSERT(nodeSP.GetText().Compare(str) == 0);
// COMMENT: {4/8/2005 6:49:11 PM}#endif
// COMMENT: {4/8/2005 6:49:11 PM}	return nodeSP;
// COMMENT: {4/8/2005 6:49:11 PM}}


//CTreeCtrl* CPropertyTreeControlBar::GetTreeCtrl()
//{
//	return &this->m_wndTree;
//}

void CPropertyTreeControlBar::SelectGridNode(void)
{
// COMMENT: {1/8/2004 7:27:58 PM}	ASSERT(!m_bSelectingProp);
// COMMENT: {1/8/2004 7:27:58 PM}	m_bSelectingProp = true;
// COMMENT: {1/8/2004 7:27:58 PM}
// COMMENT: {1/8/2004 7:27:58 PM}	this->m_nodeGrid.Select();
// COMMENT: {1/8/2004 7:27:58 PM}
// COMMENT: {1/8/2004 7:27:58 PM}	ASSERT(m_bSelectingProp);
// COMMENT: {1/8/2004 7:27:58 PM}	m_bSelectingProp = false;
	this->SelectWithoutNotification(this->m_nodeGrid);
}

BOOL CPropertyTreeControlBar::SelectWithoutNotification(HTREEITEM htItem)
{
	ASSERT(!m_bSelectingProp);
	m_bSelectingProp = true;

	BOOL bValue = this->GetTreeCtrlEx()->SelectItem(htItem);

	ASSERT(m_bSelectingProp);
	m_bSelectingProp = false;

	return bValue;
}

#ifdef WPHAST_DEPRECATED
// COMMENT: {10/23/2003 8:08:28 PM}HTREEITEM CPropertyTreeControlBar::SelectProp(vtkProp3D* pProp)
// COMMENT: {10/23/2003 8:08:28 PM}{
// COMMENT: {10/23/2003 8:08:28 PM}	ASSERT(!m_bSelectingProp);
// COMMENT: {10/23/2003 8:08:28 PM}	m_bSelectingProp = true;
// COMMENT: {10/23/2003 8:08:28 PM}	HTREEITEM hItem = 0;
// COMMENT: {10/23/2003 8:08:28 PM}
// COMMENT: {10/23/2003 8:08:28 PM}	// foreach root
// COMMENT: {10/23/2003 8:08:28 PM}	//
// COMMENT: {10/23/2003 8:08:28 PM}	HTREEITEM hRoot = m_wndTree.GetRootItem();
// COMMENT: {10/23/2003 8:08:28 PM}	while (hRoot != NULL) {
// COMMENT: {10/23/2003 8:08:28 PM}		if ((void*)m_wndTree.GetItemData(hRoot) == (void*)pProp) {
// COMMENT: {10/23/2003 8:08:28 PM}			m_wndTree.SelectItem(hRoot);
// COMMENT: {10/23/2003 8:08:28 PM}			ASSERT(m_bSelectingProp);
// COMMENT: {10/23/2003 8:08:28 PM}			m_bSelectingProp = false;
// COMMENT: {10/23/2003 8:08:28 PM}			hItem = hRoot;
// COMMENT: {10/23/2003 8:08:28 PM}			return hItem;
// COMMENT: {10/23/2003 8:08:28 PM}		}			
// COMMENT: {10/23/2003 8:08:28 PM}		// foreach child
// COMMENT: {10/23/2003 8:08:28 PM}		//
// COMMENT: {10/23/2003 8:08:28 PM}		HTREEITEM hChildItem = m_wndTree.GetChildItem(hRoot);
// COMMENT: {10/23/2003 8:08:28 PM}		while (hChildItem != NULL) {
// COMMENT: {10/23/2003 8:08:28 PM}			if ((void*)m_wndTree.GetItemData(hChildItem) == (void*)pProp) {
// COMMENT: {10/23/2003 8:08:28 PM}				m_wndTree.SelectItem(hChildItem);
// COMMENT: {10/23/2003 8:08:28 PM}				ASSERT(m_bSelectingProp);
// COMMENT: {10/23/2003 8:08:28 PM}				m_bSelectingProp = false;
// COMMENT: {10/23/2003 8:08:28 PM}				hItem = hChildItem;
// COMMENT: {10/23/2003 8:08:28 PM}				return hItem;
// COMMENT: {10/23/2003 8:08:28 PM}			}			
// COMMENT: {10/23/2003 8:08:28 PM}			hChildItem = m_wndTree.GetNextItem(hChildItem, TVGN_NEXT);
// COMMENT: {10/23/2003 8:08:28 PM}		}
// COMMENT: {10/23/2003 8:08:28 PM}		hRoot = m_wndTree.GetNextItem(hRoot, TVGN_NEXT);
// COMMENT: {10/23/2003 8:08:28 PM}	}
// COMMENT: {10/23/2003 8:08:28 PM}	ASSERT(m_bSelectingProp);
// COMMENT: {10/23/2003 8:08:28 PM}	m_bSelectingProp = false;
// COMMENT: {10/23/2003 8:08:28 PM}	return hItem;
// COMMENT: {10/23/2003 8:08:28 PM}}
#endif // WPHAST_DEPRECATED


void CPropertyTreeControlBar::SetGridLODActor(CGridLODActor* pGridLODActor)
{
	ASSERT(pGridLODActor != NULL);
	//pGridLODActor->Insert(&m_wndTree, m_htiGrid);
	//m_wndTree.SetItemData(m_htiGrid, (DWORD_PTR)pGridLODActor);
	pGridLODActor->Insert(&m_wndTree, m_nodeGrid);
	m_wndTree.RedrawWindow();
}

void CPropertyTreeControlBar::DeleteContents()
{
// COMMENT: {4/11/2005 1:07:44 PM}	// free time_controls and print_freqs
// COMMENT: {4/11/2005 1:07:44 PM}	//
// COMMENT: {4/11/2005 1:07:44 PM}	int nStressPeriods = this->GetStressPeriodCount();
// COMMENT: {4/11/2005 1:07:44 PM}	for (int i = 2; i <= nStressPeriods; ++i) {
// COMMENT: {4/11/2005 1:07:44 PM}		// TIME_CONTROL 
// COMMENT: {4/11/2005 1:07:44 PM}		CTreeCtrlNode nodeTC = this->GetTimeControlNode(i);
// COMMENT: {4/11/2005 1:07:44 PM}		ASSERT(nodeTC.GetData());
// COMMENT: {4/11/2005 1:07:44 PM}		if (CTimeControl* pTimeControl = (CTimeControl*)(nodeTC.GetData())) {
// COMMENT: {4/11/2005 1:07:44 PM}			delete pTimeControl;
// COMMENT: {4/11/2005 1:07:44 PM}			nodeTC.SetData(0);
// COMMENT: {4/11/2005 1:07:44 PM}		}
// COMMENT: {4/11/2005 1:07:44 PM}		// PRINT_FREQUENCY 
// COMMENT: {4/11/2005 1:07:44 PM}		CTreeCtrlNode nodePF = this->GetPrintFrequencyNode(i);
// COMMENT: {4/11/2005 1:07:44 PM}		ASSERT(nodePF.GetData());
// COMMENT: {4/11/2005 1:07:44 PM}		if (CPrintFreq* pPrintFreq = (CPrintFreq*)(nodePF.GetData())) {
// COMMENT: {4/11/2005 1:07:44 PM}			delete pPrintFreq;
// COMMENT: {4/11/2005 1:07:44 PM}			nodePF.SetData(0);
// COMMENT: {4/11/2005 1:07:44 PM}		}
// COMMENT: {4/11/2005 1:07:44 PM}	}
	this->m_wndTree.DeleteAllItems();

	// reset next zone
	this->m_nNextZone = 0;

	// populate static properties
	this->m_nodeFlowOnly    = this->m_wndTree.InsertItem(szFLOW_ONLY          );
	this->m_nodeSteadyFlow  = this->m_wndTree.InsertItem(szSTEADY_FLOW        );
	this->m_nodeFreeSurface = this->m_wndTree.InsertItem(szFREE_SURFACE_BC    );
	this->m_nodeUnits       = this->m_wndTree.InsertItem(szUNITS              );
	this->m_nodeGrid        = this->m_wndTree.InsertItem(szGRID               );
	this->m_nodeMedia       = this->m_wndTree.InsertItem(szMEDIA              );
	this->m_nodeIC          = this->m_wndTree.InsertItem(szINITIAL_CONDITIONS );


// COMMENT: {4/11/2005 7:50:38 PM}	// add first simulation period
// COMMENT: {4/11/2005 7:50:38 PM}	CString str;
// COMMENT: {4/11/2005 7:50:38 PM}	str.Format(szPeriodFormat, 1);
// COMMENT: {4/11/2005 7:50:38 PM}	this->m_nodeSP1         = this->m_wndTree.InsertItem(str);

	// populate the first simulation period
	//
// COMMENT: {4/8/2005 6:45:59 PM}	this->m_nodeBC          = this->m_wndTree.InsertItem(szBOUNDARY_CONDITIONS, this->m_nodeSP1);
// COMMENT: {4/8/2005 6:45:59 PM}	this->m_nodeWells       = this->m_wndTree.InsertItem(szWELLS,               this->m_nodeSP1);
// COMMENT: {4/8/2005 6:45:59 PM}	this->m_nodePF          = this->m_wndTree.InsertItem(szPRINT_FREQUENCY,     this->m_nodeSP1);
// COMMENT: {4/8/2005 6:45:59 PM}	this->m_nodeTimeControl = this->m_wndTree.InsertItem(szTIME_CONTROL,        this->m_nodeSP1);
	this->m_nodeBC           = this->m_wndTree.InsertItem(szBOUNDARY_CONDITIONS );
	this->m_nodeWells        = this->m_wndTree.InsertItem(szWELLS               );
	this->m_nodeRivers       = this->m_wndTree.InsertItem(szRIVERS              );
	this->m_nodePF           = this->m_wndTree.InsertItem(szPRINT_FREQUENCY     );
// COMMENT: {4/22/2005 3:55:31 PM}	this->m_nodeTimeControl = this->m_wndTree.InsertItem(szTIME_CONTROL        );
	this->m_nodeTimeControl2 = this->m_wndTree.InsertItem(szTIME_CONTROL        );

	// set initial checkmark states (eyes)
	this->m_wndTree.SetItemState(this->m_nodeMedia,  INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	this->m_wndTree.SetItemState(this->m_nodeGrid,   INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	this->m_wndTree.SetItemState(this->m_nodeIC,     INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	this->m_wndTree.SetItemState(this->m_nodeBC,     INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	this->m_wndTree.SetItemState(this->m_nodeWells,  INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	this->m_wndTree.SetItemState(this->m_nodeRivers, INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);


// COMMENT: {4/11/2005 1:07:32 PM}	ASSERT(this->GetStressPeriodCount() == 1);
}

void CPropertyTreeControlBar::Update(IObserver* pSender, LPARAM lHint, CObject* pHint, vtkObject* pObject)
{
	ASSERT(pSender != this);

	switch (lHint)
	{
	case WPN_NONE:
		TRACE("WARNING unexpected\n");
		break;
	case WPN_SELCHANGED:
		if (vtkProp* pProp = vtkProp::SafeDownCast(pObject))
		{
			if (CZoneActor* pZoneActor = CZoneActor::SafeDownCast(pProp))
			{
				ASSERT(FALSE); // untested
			}
			else if (CWellActor* pWellActor = CWellActor::SafeDownCast(pProp))
			{
				CTreeCtrlNode wells = this->GetWellsNode();
				bool bFound = false;
				for (int i = 0; i < wells.GetChildCount(); ++i)
				{
					CTreeCtrlNode well = wells.GetChildAt(i);
					if (well.GetData())
					{
						CWellActor *pWellActor = (CWellActor*)well.GetData();
						if (pWellActor == pObject)
						{
							well.Select();
							bFound = true;
							break;
						}
					}
				}
				ASSERT(bFound);
				if (!bFound) wells.Select();
			}
			else if (CRiverActor* pRiverActor = CRiverActor::SafeDownCast(pProp))
			{
				CTreeCtrlNode rivers = this->GetRiversNode();
				bool bFound = false;
				for (int i = 0; i < rivers.GetChildCount(); ++i)
				{
					CTreeCtrlNode riv = rivers.GetChildAt(i);
					if (riv.GetData())
					{
						CRiverActor *pRiverActor = (CRiverActor*)riv.GetData();
						if (pRiverActor == pObject)
						{
							riv.Select();
							bFound = true;
							break;
						}
					}
				}
				ASSERT(bFound);
				if (!bFound) rivers.Select();
			}
			else
			{
				ASSERT(FALSE); // untested
			}
		}
		break;

	case WPN_VISCHANGED:
		ASSERT(FALSE);
		break;

	default:
		ASSERT(FALSE);
	}
}


//void CPropertyTreeControlBar::SetGrid(grid* x, grid* y, grid* z)
//{
//	ASSERT(this->m_pPropCollection);
//	ASSERT(this->m_pGridLODActor);
//	ASSERT(this->m_pAxes);
//	ASSERT(this->m_pAxesTubeFilter);
//	ASSERT(this->m_pAxesActor);
//
//	// set the grid
//	//
//	this->m_pGridLODActor->SetGrid(x, y, z);
//	this->m_pGridLODActor->SetPickable(0);
//	// add to collection
//	this->m_pPropCollection->AddItem(this->m_pGridLODActor);
//	this->m_pGridLODActor->Insert(&this->m_wndTree, this->m_nodeGrid);
//	m_wndTree.RedrawWindow();
//
//
//	// set the axes
//	//
//	float bounds[6];
//	this->m_pGridLODActor->GetBounds(bounds);
//	this->m_pAxes->SetScaleFactor((bounds[1] - bounds[0])/5);
//	this->m_pAxesTubeFilter->SetRadius(this->m_pAxes->GetScaleFactor()/25.0);
//	// add to collection
//	this->m_pPropCollection->AddItem(this->m_pAxesActor);
//
//	/******
//	if (this->m_pimpl->m_vectorDefaultZones.empty()) {
//		// create default media zones (not undoable)
//		//
//		float* scale = this->GetScale();
//		CZoneCreateAction<CMediaZone> action(
//			this,
//			_T("(Default)"),
//			bounds[0]/scale[0],
//			bounds[1]/scale[0],
//			bounds[2]/scale[1],
//			bounds[3]/scale[1],
//			bounds[4]/scale[2],
//			bounds[5]/scale[2]
//			);
//		action.GetZone()->SetPickable(0);
//		action.Execute();
//		this->AddDefaultZone(action.GetZone());
//	}
//
//	// resize all default zones
//	//
//	std::vector<CZone*>::iterator iter = this->m_pimpl->m_vectorDefaultZones.begin();
//	for(; iter != this->m_pimpl->m_vectorDefaultZones.end(); ++iter) {
//		(*iter)->SetBounds(bounds);
//	}
//	******/
//}

//void CPropertyTreeControlBar::SetScale(float x, float y, float z)
//{
//	ASSERT(x != 0);
//	ASSERT(y != 0);
//	ASSERT(z != 0);
//
//	/****
//	// Update scale page
//	//
//	this->m_pScalePage->m_XScale = m_Scale[0];
//	this->m_pScalePage->m_YScale = m_Scale[1];
//	this->m_pScalePage->m_ZScale = m_Scale[2];
//	if (this->m_pScalePage->GetSafeHwnd()) {
//		this->m_pScalePage->UpdateData(FALSE);
//	}
//	****/
//	float scale[3];
//	scale[0] = x;
//	scale[1] = y;
//	scale[2] = z;
//
//	// set scale for the grid and all zones
//	//
//	if (vtkPropCollection* pCollection = this->GetPropCollection()) {
//		pCollection->InitTraversal();
//		for (int i = 0; i < pCollection->GetNumberOfItems(); ++i) {
//			vtkProp* prop = pCollection->GetNextProp();
//			if (CGridLODActor *pGridActor = CGridLODActor::SafeDownCast(prop)) {
//				pGridActor->SetScale(scale);
//			}
//			if (CZone *pZone = CZone::SafeDownCast(prop)) {
//				pZone->SetScale(scale);
//			}
//		}
//	}
//
//
//	// set scale for the axes
//	//
//	float bounds[6];
//	this->m_pGridLODActor->GetBounds(bounds);
//	this->m_pAxes->SetScaleFactor((bounds[1] - bounds[0])/5);
//	this->m_pAxesTubeFilter->SetRadius(this->m_pAxes->GetScaleFactor()/25.0);
//
//	/****
//	POSITION pos = this->GetFirstViewPosition();
//	while (pos != NULL) {
//		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
//
//		// resize the selection bounding box
//		//
//		if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( pView->GetRenderWindowInteractor()->GetPicker() )) {
//			if (vtkProp3D* prop = picker->GetProp3D()) {
//				//if (CZoneLODActor* pActor = CZoneLODActor::SafeDownCast(prop)) {
//				//	pActor->Pick(pView->GetRenderer(), pView->GetRenderWindowInteractor());
//				//}
//				if (CZone *pZone = CZone::SafeDownCast(prop)) {
//					pZone->Pick(pView->GetRenderer(), pView->GetRenderWindowInteractor());
//				}
//
//			}
//		}
//
//		// resize the Box Widget
//		//
//		if (pView->GetBoxWidget()->GetProp3D()) {
//			pView->GetBoxWidget()->PlaceWidget();
//		}
//	}
//	****/
//}

void CPropertyTreeControlBar::OnDestroy()
{
	CSizingControlBarCFVS7::OnDestroy();

	// Add your message handler code here

	// free time_controls
	// Used only to show no memory leaks on exit
	//
	ASSERT(::IsWindow(this->m_wndTree.m_hWnd));
	if (::IsWindow(this->m_wndTree.m_hWnd)) {
		this->DeleteContents();
	}

	// free image list
	delete this->m_pImageList;
}

CString CPropertyTreeControlBar::GetNextZoneName(void)const
{
	static CString str;
	str.Format(szZoneFormat, this->m_nNextZone);
	return str;
}

int CPropertyTreeControlBar::GetNextWellNumber(void)const
{
	CTreeCtrlNode wells = this->GetWellsNode();

	int nNext = 1;
	int n = wells.GetChildCount();
	for (int i = 0; i < n; ++i)
	{ 
		CTreeCtrlNode child = wells.GetChildAt(i);
		// TODO
	}
	return nNext;
}

void CPropertyTreeControlBar::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	TRACE("OnKeyDown\n");
	NMTVKEYDOWN *pNMTVKEYDOWN = (LPNMTVKEYDOWN)pNMHDR;

	if (pNMTVKEYDOWN->wVKey == VK_DELETE)
	{
		CTreeCtrlNode sel = this->m_wndTree.GetSelectedItem();

		// MEDIA
		//
		if (sel.IsNodeAncestor(this->m_nodeMedia))
		{
			if (sel != this->m_nodeMedia)
			{
				while (sel.GetParent() != this->m_nodeMedia)
				{
					sel = sel.GetParent();
					if (!sel) break;
				}
				if (sel.GetData())
				{
					if (CZoneActor* pZone = CZoneActor::SafeDownCast((vtkObject*)sel.GetData()))
					{
						if (pZone->GetDefault())
						{
							::AfxMessageBox(_T("The default MEDIA zone cannot be deleted."));
						}
						else
						{
							CTreeCtrlNode parent = sel.GetParent();
							if (CWPhastDoc* pDoc = this->GetDocument())
							{
								parent.Select();
								pDoc->Execute(new CZoneRemoveAction(pDoc, pZone, this));
							}
						}
						*pResult = TRUE;
						return;
					}
				}
			}
			return;
		}

		// WELLS
		//
		if (sel.IsNodeAncestor(this->GetWellsNode()))
		{
			if (sel != this->GetWellsNode())
			{
				while (sel.GetParent() != this->GetWellsNode())
				{
					sel = sel.GetParent();
					if (!sel) break;
				}
				if (sel && sel.GetData())
				{
					if (CWellActor* pWell = CWellActor::SafeDownCast((vtkObject*)sel.GetData()))
					{
						CTreeCtrlNode parent = sel.GetParent();
						if (CWPhastDoc* pDoc = this->GetDocument())
						{
							VERIFY(parent.Select());
							pDoc->Execute(new CWellDeleteAction(pDoc, pWell));
						}
						*pResult = TRUE;
						return;
					}
				}
			}
			return;
		}


		// INITIAL_CONDITIONS
		//
		if (sel.IsNodeAncestor(this->m_nodeIC))
		{
			if (sel != this->m_nodeIC)
			{
				while (sel.GetParent() != this->m_nodeIC)
				{
					sel = sel.GetParent();
					if (!sel) break;
				}
				if (sel.GetData())
				{
					if (CICZoneActor* pZone = CICZoneActor::SafeDownCast((vtkObject*)sel.GetData()))
					{
						if (pZone->GetDefault())
						{
							if (pZone->GetType() == CICZoneActor::IC_HEAD)
							{
								::AfxMessageBox(_T("The default HEAD_IC zone cannot be deleted."));
							}
							else if (pZone->GetType() == CICZoneActor::IC_CHEM)
							{
								::AfxMessageBox(_T("The default CHEMISTRY_IC zone cannot be deleted."));
							}
						}
						else
						{
							CTreeCtrlNode parent = sel.GetParent();
							if (CWPhastDoc* pDoc = this->GetDocument())
							{
								parent.Select();
								pDoc->Execute(new CZoneRemoveAction(pDoc, pZone, this));
							}
						}
						*pResult = TRUE;
						return;
					}
				}
			}
			return;
		}

		// BOUNDARY_CONDITIONS (first stress period)
		//
		if (sel.IsNodeAncestor(this->m_nodeBC))
		{
			if (sel != this->m_nodeBC)
			{
				while (sel.GetParent() != this->m_nodeBC)
				{
					sel = sel.GetParent();
					if (!sel) break;
				}
				if (sel.GetData())
				{
					if (CZoneActor* pZone = CZoneActor::SafeDownCast((vtkObject*)sel.GetData()))
					{
						ASSERT(!pZone->GetDefault()); // no default BCs
						CTreeCtrlNode parent = sel.GetParent();
						if (CWPhastDoc* pDoc = this->GetDocument())
						{
							parent.Select();
							pDoc->Execute(new CZoneRemoveAction(pDoc, pZone, this));
						}
						*pResult = TRUE;
						return;
					}
				}
			}
			return;
		}

		// TIME_CONTROL
		//
		if (sel.IsNodeAncestor(this->m_nodeTimeControl))
		{
			if (this->m_nodeTimeControl.GetData())
			{
				CTimeControl* pTC = (CTimeControl*)this->m_nodeTimeControl.GetData();
				::AfxMessageBox("Deleting TIME_CONTROL not currently implemented");
				*pResult = TRUE;
				return;
			}
			return;
		}


// COMMENT: {4/11/2005 1:06:40 PM}		// Handle stress periods greater than 1
// COMMENT: {4/11/2005 1:06:40 PM}		int nStressPeriodCount = this->GetStressPeriodCount();
// COMMENT: {4/11/2005 1:06:40 PM}		int nStressPeriod = 0;
// COMMENT: {4/11/2005 1:06:40 PM}		for (int i = 2; i <= nStressPeriodCount; ++i) {
// COMMENT: {4/11/2005 1:06:40 PM}			if (sel.IsNodeAncestor(this->GetStressPeriodNode(i))) {
// COMMENT: {4/11/2005 1:06:40 PM}				if (sel != this->GetStressPeriodNode(i)) {
// COMMENT: {4/11/2005 1:06:40 PM}					// TODO: May want to implement deleting stressperiods
// COMMENT: {4/11/2005 1:06:40 PM}					nStressPeriod = i;
// COMMENT: {4/11/2005 1:06:40 PM}				}
// COMMENT: {4/11/2005 1:06:40 PM}				break;
// COMMENT: {4/11/2005 1:06:40 PM}			}
// COMMENT: {4/11/2005 1:06:40 PM}		}
// COMMENT: {4/11/2005 1:06:40 PM}		if (nStressPeriod > 1) {
// COMMENT: {4/11/2005 1:06:40 PM}			// BOUNDARY_CONDITIONS
// COMMENT: {4/11/2005 1:06:40 PM}			//
// COMMENT: {4/11/2005 1:06:40 PM}			if (sel.IsNodeAncestor(this->GetBCNode(nStressPeriod))) {
// COMMENT: {4/11/2005 1:06:40 PM}				if (sel != this->GetBCNode(nStressPeriod)) {
// COMMENT: {4/11/2005 1:06:40 PM}					while (sel.GetParent() != this->GetBCNode(nStressPeriod)) {
// COMMENT: {4/11/2005 1:06:40 PM}						sel = sel.GetParent();
// COMMENT: {4/11/2005 1:06:40 PM}						if (!sel) break;
// COMMENT: {4/11/2005 1:06:40 PM}					}
// COMMENT: {4/11/2005 1:06:40 PM}					if (sel && sel.GetData()) {
// COMMENT: {4/11/2005 1:06:40 PM}						if (CBCZoneActor* pZone = CBCZoneActor::SafeDownCast((vtkObject*)sel.GetData())) {
// COMMENT: {4/11/2005 1:06:40 PM}							::AfxMessageBox(_T("Boundary conditions can only be deleted from the first simulation period."));
// COMMENT: {4/11/2005 1:06:40 PM}							//CTreeCtrlNode parent = sel.GetParent();
// COMMENT: {4/11/2005 1:06:40 PM}							//ASSERT(!pZone->GetDefault()); // no default BCs
// COMMENT: {4/11/2005 1:06:40 PM}							//if (CWPhastDoc* pDoc = this->GetDocument()) {
// COMMENT: {4/11/2005 1:06:40 PM}							//	pDoc->Execute(new CZoneRemoveAction(pDoc, pZone, this));
// COMMENT: {4/11/2005 1:06:40 PM}							//	parent.Select();
// COMMENT: {4/11/2005 1:06:40 PM}							//}
// COMMENT: {4/11/2005 1:06:40 PM}							*pResult = TRUE;
// COMMENT: {4/11/2005 1:06:40 PM}							return;
// COMMENT: {4/11/2005 1:06:40 PM}						}
// COMMENT: {4/11/2005 1:06:40 PM}					}
// COMMENT: {4/11/2005 1:06:40 PM}				}
// COMMENT: {4/11/2005 1:06:40 PM}				ASSERT(FALSE);
// COMMENT: {4/11/2005 1:06:40 PM}				TRACE("BC not found\n");
// COMMENT: {4/11/2005 1:06:40 PM}				return;
// COMMENT: {4/11/2005 1:06:40 PM}			}
// COMMENT: {4/11/2005 1:06:40 PM}
// COMMENT: {4/11/2005 1:06:40 PM}			// TIME_CONTROL
// COMMENT: {4/11/2005 1:06:40 PM}			//
// COMMENT: {4/11/2005 1:06:40 PM}			if (sel.IsNodeAncestor(this->GetTimeControlNode(nStressPeriod))) {
// COMMENT: {4/11/2005 1:06:40 PM}				if (this->GetTimeControlNode(nStressPeriod).GetData()) {
// COMMENT: {4/11/2005 1:06:40 PM}					CTimeControl* pTC = (CTimeControl*)this->GetTimeControlNode(nStressPeriod).GetData();
// COMMENT: {4/11/2005 1:06:40 PM}					::AfxMessageBox("Deleting TIME_CONTROL not currently implemented");
// COMMENT: {4/11/2005 1:06:40 PM}					*pResult = TRUE;
// COMMENT: {4/11/2005 1:06:40 PM}					return;
// COMMENT: {4/11/2005 1:06:40 PM}				}
// COMMENT: {4/11/2005 1:06:40 PM}				return;
// COMMENT: {4/11/2005 1:06:40 PM}			}
// COMMENT: {4/11/2005 1:06:40 PM}			::AfxMessageBox("Delete Node Greater than simulation 1 BUT NOT FOUND(time_control, bc)");
// COMMENT: {4/11/2005 1:06:40 PM}			*pResult = TRUE;
// COMMENT: {4/11/2005 1:06:40 PM}			return;
// COMMENT: {4/11/2005 1:06:40 PM}		}
	}
}

void CPropertyTreeControlBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	// Note: NOT called when mouse down occurs inside treectrl
	//{{HACK
	// If creating a new zone cancel now 
	{
		CFrameWnd *pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		ASSERT_VALID(pFrame);

		CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
		ASSERT_VALID(pDoc);

		POSITION pos = pDoc->GetFirstViewPosition();
		while (pos != NULL) {
			CWPhastView *pView = (CWPhastView*) pDoc->GetNextView(pos);
			ASSERT_VALID(pView);
			if (pView->CreatingNewZone()) {
				pView->CancelNewZone();
			}
		}
	}
	//}}HACK

	CSizingControlBarCFVS7::OnLButtonDown(nFlags, point);
}
