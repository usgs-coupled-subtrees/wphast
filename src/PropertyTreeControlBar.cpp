#include "StdAfx.h"
#include "PropertyTreeControlBar.h"
#include "resource.h"

#include "PrintFreq.h"
#include "PrintFreqPropertyPage.h"

#include <sstream>
#include <windowsx.h>       // GET_X_LPARAM GET_Y_LPARAM
#undef GetNextSibling       // defined in windowsx.h
#undef GetPrevSibling       // defined in windowsx.h



//// #include "ZoneLODActor.h"
#include "GridActor.h"
#include "WPhastDoc.h"
#include "WPhastView.h"
#include "MediaPropertyPage.h"
#include "GridPropertyPage.h"
#include "RiverPropertySheet.h"
#include "SetRiverAction.h"
#include "RiverDeleteAction.h"
#include "RiverDeletePointAction.h"
#include "DragDropAction.h"
#include "ZoneCreateAction.h"


#include "ModelessPropertySheet.h"
#include "GridPropertyPage2.h"
#include "BCTypeDialog.h"

#include "ZoneActor.h"
#include "WellActor.h"
#include "RiverActor.h"
#include "MediaZoneActor.h"
#include "ICZoneActor.h"
#include "ICHeadZoneActor.h"
#include "ICChemZoneActor.h"
#include "BCZoneActor.h"
#include "Units.h"
#include "NewModel.h"
#include "FlowOnly.h"
#include "SolutionMethod.h"
#include "TimeControl2.h"
#include "Units1PropertyPage.h"
#include "Units2PropertyPage.h"
#include "BoxPropertiesDialogBar.h"
#include "Global.h"
#include "RiverPropertyPage2.h"

// Actions
#include "ResizeGridAction.h"
#include "SetUnitsAction.h"
#include "ZoneRemoveAction.h"
#include "WellDeleteAction.h"
#include "WellCreateAction.h"
#include "RiverCreateAction.h"

#include <vtkWin32RenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkPropCollection.h>
#include <vtkAxes.h>
#include <vtkTubeFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkInteractorStyle.h> 
#include <vtkBoxWidget.h>
#include <vtkPropAssembly.h>

//static const TCHAR szPeriodFormat[]        = _T("Simulation Period %d");
static const TCHAR szFLOW_ONLY[]           = _T("SOLUTE_TRANSPORT");
static const TCHAR szSTEADY_FLOW[]         = _T("STEADY_FLOW");
static const TCHAR szFREE_SURFACE_BC[]     = _T("FREE_SURFACE_BC");
static const TCHAR szSOLUTION_METHOD[]     = _T("SOLUTION_METHOD");
static const TCHAR szUNITS[]               = _T("UNITS");
static const TCHAR szGRID[]                = _T("GRID");
static const TCHAR szMEDIA[]               = _T("MEDIA");
static const TCHAR szINITIAL_CONDITIONS[]  = _T("INITIAL_CONDITIONS");
static const TCHAR szIC_HEAD[]             = _T("HEAD_IC");
static const TCHAR szIC_CHEM[]             = _T("CHEMISTRY_IC");
static const TCHAR szBOUNDARY_CONDITIONS[] = _T("BOUNDARY_CONDITIONS");
static const TCHAR szWELLS[]               = _T("WELLS");
static const TCHAR szRIVERS[]              = _T("RIVERS");
static const TCHAR szPRINT_INITIAL[]       = _T("PRINT_INITIAL");
static const TCHAR szPRINT_FREQUENCY[]     = _T("PRINT_FREQUENCY");
static const TCHAR szTIME_CONTROL[]        = _T("TIME_CONTROL");

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
	ON_NOTIFY(TVN_BEGINDRAG, IDC_PROPERTY_TREE, OnBeginDrag)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, &CPropertyTreeControlBar::OnUpdateEditProperties)
	ON_COMMAND(ID_EDIT_PROPERTIES, &CPropertyTreeControlBar::OnEditProperties)
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
, m_pImageList(NULL)
{
	CString str;
	str.Format("WPhast:%d", _getpid());

	this->m_cfPID     = (CLIPFORMAT)::RegisterClipboardFormat(str);

	ASSERT(CGridElt::clipFormat >= 0xC000);
	ASSERT(CHeadIC::clipFormat >= 0xC000);
	ASSERT(CChemIC::clipFormat >= 0xC000);
	ASSERT(CBC::clipFormat >= 0xC000);
	ASSERT(CZoneActor::clipFormat >= 0xC000);
}

CPropertyTreeControlBar::~CPropertyTreeControlBar(void)
{
}

CWPhastDoc* CPropertyTreeControlBar::GetDocument(void)const
{
	if (CFrameWnd *pFrame = reinterpret_cast<CFrameWnd*>(AfxGetApp()->m_pMainWnd))
	{
		ASSERT_KINDOF(CFrameWnd, pFrame);
		ASSERT_VALID(pFrame);
		if (CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument()))
		{
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

	if (m_wndTree.GetItemData(pNMTREEVIEW->itemNew.hItem))
	{
		HTREEITEM hParent = m_wndTree.GetParentItem(pNMTREEVIEW->itemNew.hItem);

		if (hParent == m_nodeMedia || hParent == this->m_nodeBC || hParent == this->m_nodeICHead || hParent == this->m_nodeICChem)
		{

			CZoneActor* pZone = reinterpret_cast<CZoneActor*>(m_wndTree.GetItemData(pNMTREEVIEW->itemNew.hItem));
			if (CWPhastDoc* pDoc = this->GetDocument())
			{
				// Notify listeners
				//
				pDoc->Notify(this, WPN_SELCHANGED, 0, pZone);
			}
		}
		else if (hParent == this->m_nodeWells)
		{
			// if (CWellActor* pWell = reinterpret_cast<CWellActor*>(item.GetData()))
			if (item.GetData())
			{
				if (vtkProp* pProp = vtkProp::SafeDownCast(reinterpret_cast<vtkObject*>(item.GetData())))
				{
					// Notify listeners
					//
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
					// Notify listeners
					//
					if (CWPhastDoc* pDoc = this->GetDocument())
					{
						pDoc->Notify(this, WPN_SELCHANGED, 0, pProp);
					}
				}
			}
		}
		else
		{
			// Notify listeners
			//
			if (CWPhastDoc *pDoc = this->GetDocument())
			{
				pDoc->Notify(this, WPN_SELCHANGED, 0, 0);
			}

			// Update StatusBar
			//
			if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
			{
				CString status;
				status.LoadString(AFX_IDS_IDLEMESSAGE);
				pWnd->SetWindowText(status);
			}
		}		
	}
	else
	{
		// Notify listeners
		//
		if (CWPhastDoc *pDoc = this->GetDocument())
		{
			pDoc->Notify(this, WPN_SELCHANGED, 0, 0);
		}

		// Note: MessageBar should be a listener for WPN_SELCHANGED
		// Update StatusBar
		//
		if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
		{
			CString status;
			status.LoadString(AFX_IDS_IDLEMESSAGE);
			pWnd->SetWindowText(status);
		}

		// River point selection
		//
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
	CTreeCtrlNode item(ht.hItem, this->GetTreeCtrlEx());
	UINT nNewState;
	switch (this->GetNodeCheck(item))
	{
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
		pDoc->UpdateAllViews(0);
	}
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

	if (node == this->GetMediaNode())
	{
		if (!((pDoc = this->GetDocument()) && (pPropAssembly = pDoc->GetPropAssemblyMedia())))
		{
			return;
		}
	}
	else if (node == this->GetICNode())
	{
		if (!((pDoc = this->GetDocument()) && (pPropAssembly = pDoc->GetPropAssemblyIC())))
		{
			return;
		}
	}
	else if (node == this->GetBCNode())
	{
		if (!((pDoc = this->GetDocument()) && (pPropAssembly = pDoc->GetPropAssemblyBC())))
		{
			return;
		}
	}
	else if (node == this->GetWellsNode())
	{
		if (!((pDoc = this->GetDocument()) && (pPropAssembly = pDoc->GetPropAssemblyWells())))
		{
			return;
		}
	}
	else if (node == this->GetRiversNode())
	{
		if (!((pDoc = this->GetDocument()) && (pPropAssembly = pDoc->GetPropAssemblyRivers())))
		{
			return;
		}
	}
	else if (node == this->GetGridNode())
	{
		if (CWPhastDoc *pDoc = this->GetDocument())
		{
			pDoc->OnViewGrid();
		}
		return;
	}
	else
	{
		ASSERT(
			node.GetParent() == this->GetMediaNode()  ||
// COMMENT: {10/24/2005 3:22:20 PM}            node.GetParent() == this->GetICNode()     ||
			//{{
            node.GetParent() == this->GetICHeadNode() ||
			node.GetParent() == this->GetICChemNode() ||
			//}}
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

	// Cancel any modes currently running
	//
	if (true)
	{
		CFrameWnd *pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		ASSERT_VALID(pFrame);

		CWPhastDoc* pDoc = this->GetDocument();
		ASSERT_VALID(pDoc);

		POSITION pos = pDoc->GetFirstViewPosition();
		while (pos != NULL)
		{
			CWPhastView *pView = reinterpret_cast<CWPhastView*>(pDoc->GetNextView(pos));
			ASSERT_VALID(pView);
			pView->CancelMode();
		}
	}

	TVHITTESTINFO ht = {0};

	DWORD dwpos = ::GetMessagePos();
	ht.pt.x = GET_X_LPARAM(dwpos);
	ht.pt.y = GET_Y_LPARAM(dwpos);

	::MapWindowPoints(HWND_DESKTOP, pNMHDR->hwndFrom, &ht.pt, 1);
	this->m_wndTree.HitTest(&ht);

	if (!ht.hItem) return;
	if ((TVHT_ONITEMSTATEICON & ht.flags))
	{
		// user double-clicked on checkmark
		//
		this->OnNMClk(pNMHDR, pResult);
		*pResult = TRUE;
		return;
	}
	if (!(TVHT_ONITEM & ht.flags)) return;

	///this->EditSelection(pResult);
	//{{
	if (this->IsNodeEditable(this->GetTreeCtrlEx()->GetSelectedItem(), true)) {
		*pResult = TRUE;
	}
	//}}
}

bool CPropertyTreeControlBar::IsNodeEditable(CTreeCtrlNode editNode, bool bDoEdit)
{
	CTreeCtrlNode item = editNode;
	CTreeCtrlNode parent = item.GetParent();
	HTREEITEM hItem = item;
	HTREEITEM hParent = parent;


	// FLOW_ONLY
	//
	if (item.IsNodeAncestor(this->m_nodeFlowOnly))
	{
		CFlowOnly *pFlowOnly = reinterpret_cast<CFlowOnly*>(this->m_nodeFlowOnly.GetData());
		if (!pFlowOnly) return false;  // can occur on failed import

		//{{ HACK
		CTreeCtrlNode nodeMedia = this->GetMediaNode();
		int nCount = nodeMedia.GetChildCount();

		CMediaZoneActor *pMediaZoneActor = NULL;
		for (int i = 0; i < nCount; ++i)
		{
			if (CMediaZoneActor *pZone = CMediaZoneActor::SafeDownCast((vtkObject*)nodeMedia.GetChildAt(i).GetData()))
			{
				pMediaZoneActor = pZone;
				break;
			}
		}
		if (bDoEdit)
		{
			pFlowOnly->Edit(&this->m_wndTree, pMediaZoneActor, 0);
		}
		//}} HACK
		return true;
	}


	// FREE_SURFACE
	//
	if (item.IsNodeAncestor(this->m_nodeFreeSurface))
	{
		CFreeSurface *pFreeSurface = reinterpret_cast<CFreeSurface*>(this->m_nodeFreeSurface.GetData());
		if (!pFreeSurface) return false;  // can occur on failed import

		if (bDoEdit)
		{
			pFreeSurface->Edit(&this->m_wndTree);
		}
		return true;
	}

	// SOLUTION_METHOD
	//
	if (item.IsNodeAncestor(this->m_nodeSolutionMethod))
	{
		CSolutionMethod *pSolutionMethod = reinterpret_cast<CSolutionMethod*>(this->m_nodeSolutionMethod.GetData());
		if (!pSolutionMethod) return false;  // can occur on failed import

		if (bDoEdit)
		{
			pSolutionMethod->Edit(&this->m_wndTree);
		}
		return true;
	}

	// STEADY_FLOW
	//
	if (item.IsNodeAncestor(this->m_nodeSteadyFlow))
	{
		CSteadyFlow* pSteadyFlow = reinterpret_cast<CSteadyFlow*>(this->m_nodeSteadyFlow.GetData());
		if (!pSteadyFlow) return false;  // can occur on failed import

		if (bDoEdit)
		{
			pSteadyFlow->Edit(&this->m_wndTree);
		}
		return true;
	}


	// UNITS
	//
	if (item.IsNodeAncestor(this->m_nodeUnits))
	{
		if (this->m_nodeUnits.GetData())
		{
			CUnits* pUnits = reinterpret_cast<CUnits*>(this->m_nodeUnits.GetData());
			if (bDoEdit)
			{
				pUnits->Edit(&this->m_wndTree);
			}
			return true;
		}
		return false;
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
					if (bDoEdit)
					{
						pWellActor->Edit(this->GetDocument());
					}
					return true;
				}
			}
		}
		return false;
	}

	// RIVERS
	//
	if (item.IsNodeAncestor(this->m_nodeRivers))
	{
		if (item != this->m_nodeRivers)
		{
			CTreeCtrlNode ptNode = item;
			while (item.GetParent() != this->m_nodeRivers)
			{
				item = item.GetParent();
				if (!item) break;
			}

			// determine point being edited
			//
			int point = 0;
			if (ptNode != item)
			{
				while (ptNode.GetParent() != item)
				{
					ptNode = ptNode.GetParent();
					if (!ptNode) break;
				}
				point = item.GetIndex(ptNode);
			}

			if (item.GetData())
			{
				if (CRiverActor* pRiverActor = CRiverActor::SafeDownCast((vtkObject*)item.GetData()))
				{
					if (bDoEdit)
					{
						CRiver river = pRiverActor->GetRiver();

						CString str;
						str.Format(_T("River %d Properties"), river.n_user);
						CPropertySheet sheet(str);
						
						CRiverPropertyPage2 page;
						sheet.AddPage(&page);

						if (CWPhastDoc* pWPhastDoc = this->GetDocument())
						{
							page.SetProperties(river);
							page.SetFlowOnly(bool(pWPhastDoc->GetFlowOnly()));
							page.SetUnits(pWPhastDoc->GetUnits());
							if (point) page.SetPoint(point);

							std::set<int> riverNums;
							pWPhastDoc->GetUsedRiverNumbers(riverNums);

							// remove this river number from used list
							std::set<int>::iterator iter = riverNums.find(river.n_user);
							ASSERT(iter != riverNums.end());
							if (iter != riverNums.end())
							{
								riverNums.erase(iter);
							}
							page.SetUsedRiverNumbers(riverNums);

							if (sheet.DoModal() == IDOK)
							{
								CRiver river;
								page.GetProperties(river);
								pWPhastDoc->Execute(new CSetRiverAction(pRiverActor, river, pWPhastDoc));
							}
						}
					}
					return true;
				}
			}
		}
		return false;
	}


	// GRID
	//
	if (item.IsNodeAncestor(this->m_nodeGrid))
	{
		if (CGridActor* pGridActor = CGridActor::SafeDownCast((vtkObject*)m_nodeGrid.GetData()))
		{
			CFrameWnd *pFrame = reinterpret_cast<CFrameWnd*>(AfxGetApp()->m_pMainWnd);
			ASSERT_VALID(pFrame);
			CWPhastDoc* pWPhastDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
			ASSERT_VALID(pWPhastDoc);

			if (bDoEdit)
			{
				pWPhastDoc->Edit(pGridActor);
			}
			return true;
		}
		return false;
	}

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
					if (bDoEdit)
					{
						pZone->Edit(&this->m_wndTree);
					}
					return true;
				}
			}
		}
		return false;
	}

	// BOUNDARY_CONDITIONS
	//
	if (item.IsNodeAncestor(this->m_nodeBC))
	{
		if (item != this->m_nodeBC)
		{
			while (item.GetParent() != this->m_nodeBC)
			{
				item = item.GetParent();
				if (!item) break;
			}
			if (item.GetData())
			{
				if (CZoneActor* pZone = CZoneActor::SafeDownCast((vtkObject*)item.GetData()))
				{
					if (bDoEdit)
					{
						pZone->Edit(&this->m_wndTree);
					}
					return true;
				}
			}
		}
		return false;
	}

	// INITIAL_CONDITIONS
	//
	if (item.IsNodeAncestor(this->m_nodeIC))
	{
		if (item != this->m_nodeIC && item != this->m_nodeICHead && item != this->m_nodeICChem)
		{
			while (item.GetParent() != this->m_nodeICHead && item.GetParent() != this->m_nodeICChem)
			{
				item = item.GetParent();
				if (!item) break;
			}
			if (item.GetData())
			{
				if (CZoneActor* pZone = CZoneActor::SafeDownCast((vtkObject*)item.GetData()))
				{
					if (bDoEdit)
					{
						pZone->Edit(&this->m_wndTree);
					}
					return true;
				}
			}
		}
		return false;
	}

	// PRINT_INITIAL
	//
	if (item.IsNodeAncestor(this->m_nodePrintInput))
	{
		if (this->m_nodePrintInput.GetData())
		{
			CPrintInput* pPI = (CPrintInput*)this->m_nodePrintInput.GetData();
			if (bDoEdit)
			{
				pPI->Edit(&this->m_wndTree);
			}
			return true;
		}
		return false;
	}

	// PRINT_FREQUENCY
	//
	if (item.IsNodeAncestor(this->m_nodePF))
	{
		if (this->m_nodePF.GetData())
		{
			CPrintFreq* pPF = (CPrintFreq*)this->m_nodePF.GetData();
			if (bDoEdit)
			{
				pPF->Edit(&this->m_wndTree);
			}
			return true;
		}
		return false;
	}


	// TIME_CONTROL
	//
	if (item.IsNodeAncestor(this->m_nodeTimeControl2))
	{
		if (this->m_nodeTimeControl2.GetData())
		{
			CTimeControl2* pTC = (CTimeControl2*)this->m_nodeTimeControl2.GetData();
			if (bDoEdit)
			{
				pTC->Edit(&this->m_wndTree);
			}
			return true;
		}
		return false;
	}

	return false;
}


HBITMAP FAR PASCAL CreateColorBitmap(int cx, int cy)
{
    HBITMAP hbm;
    HDC hdc;

    hdc = GetDC(NULL);
    hbm = CreateCompatibleBitmap(hdc, cx, cy);

    ReleaseDC(NULL, hdc);
    return hbm;
}

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
	{
        return NULL;
	}

	hdc = ::CreateCompatibleDC(hdcDesk);
	::ReleaseDC(NULL, hdcDesk);

    if (!hdc)
        return NULL;

	if (himl && ::ImageList_GetIconSize(himl, &cxImage, &cyImage))
	{
        // cxImage and cyImage are okay
    }
	else
	{
		cxImage = ::GetSystemMetrics(SM_CXSMICON);
		cyImage = ::GetSystemMetrics(SM_CYSMICON);
    }

	himl = ::ImageList_Create(cxImage, cyImage, ILC_COLORDDB | ILC_MASK, 0, nImages);
	hbm = ::CreateColorBitmap(cxImage * nImages, cyImage);

    if (fUseColorKey)
    {
        clrMask = RGB(255,000,255); // magenta
		if (clrMask == ::GetSysColor(COLOR_WINDOW))
		{
            clrMask = RGB(000,000,255); // blue
		}
    }
    else
    {
        clrMask = ::GetSysColor(COLOR_WINDOW);
    }

    // fill
	hbmTemp = (HBITMAP)::SelectObject(hdc, hbm);

    rc.left = rc.top = 0;
    rc.bottom = cyImage;
    rc.right = cxImage * nImages;
	::FillRect(hdc, &rc, ::CreateSolidBrush(clrMask));

    rc.right = cxImage;

	::InflateRect(&rc, -::GetSystemMetrics(SM_CXEDGE), -::GetSystemMetrics(SM_CYEDGE));
    rc.right++;
    rc.bottom++;

    if (fTree)
	{
		::OffsetRect(&rc, cxImage, 0);
	}

	HTHEME hTheme = ::g_xpStyle.IsAppThemed() ? ::g_xpStyle.OpenThemeData(NULL, L"Button") : NULL;
	if (hTheme)
	{
		VERIFY(::g_xpStyle.DrawThemeBackground(hTheme,
			hdc,
			BP_CHECKBOX,
			CBS_UNCHECKEDNORMAL,
			&rc,
			NULL) == S_OK);
	}
	else
	{
		VERIFY(::DrawFrameControl(hdc,
			&rc,
			DFC_BUTTON, DFCS_BUTTONCHECK | DFCS_FLAT 
			));
	}

	::OffsetRect(&rc, cxImage, 0);
    if (fMirror)
    {
		// MirrorIcon has off by one bug
		::OffsetRect(&rc, -1, 0);  
    }

	if (hTheme)
	{
		VERIFY(::g_xpStyle.DrawThemeBackground(hTheme,
			hdc,
			BP_CHECKBOX,
			CBS_CHECKEDNORMAL,
			&rc,
			NULL) == S_OK);
	}
	else
	{
		VERIFY(::DrawFrameControl(hdc,
			&rc,
			DFC_BUTTON, DFCS_BUTTONCHECK | DFCS_FLAT | DFCS_CHECKED
			));
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

	if (hTheme)
	{
		VERIFY(::g_xpStyle.CloseThemeData(hTheme) == S_OK);
	}

	::DeleteDC(hdc);
	::DeleteObject(hbm);
    return himl;
}

int CPropertyTreeControlBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBarCFVS7::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!this->m_wndTree.Create(WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
		CRect(0, 0, 0, 0), this, IDC_PROPERTY_TREE))
	{
		TRACE0("Failed to create instant bar child\n");
		return -1;		// fail to create
	}
	this->m_wndTree.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	this->m_OleDropTarget.Register(&this->m_wndTree);
	this->m_OleDropTarget.SetHandler(this);


	this->m_pImageList = new CImageList();
	this->m_pImageList->Attach(CreateCheckBoxImagelist(this->m_pImageList->GetSafeHandle(), TRUE, FALSE, FALSE));

	this->m_wndTree.SetImageList(this->m_pImageList, LVSIL_STATE);
	
	// bring the tooltips to front
	CWnd* pTT = FromHandle((HWND) m_wndTree.SendMessage(TVM_GETTOOLTIPS));
	if (pTT != NULL)
	{
		pTT->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
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

void CPropertyTreeControlBar::SetSolutionMethod(CSolutionMethod *pSolutionMethod)
{
	pSolutionMethod->Insert(&this->m_wndTree, this->m_nodeSolutionMethod);
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
	this->SetPrintInput(&pModel->m_printInput);
	this->SetPrintFrequency(&pModel->m_printFreq);
	this->SetSolutionMethod(&pModel->m_solutionMethod);
}

void CPropertyTreeControlBar::SetTimeControl2(CTimeControl2* pTimeControl2)
{
	if (pTimeControl2)
	{
		pTimeControl2->Insert(&this->m_wndTree, this->m_nodeTimeControl2);
	}
}

void CPropertyTreeControlBar::SetPrintFrequency(CPrintFreq* pPrintFreq)
{
	if (pPrintFreq)
	{
		pPrintFreq->Insert(&this->m_wndTree, this->m_nodePF);
	}
}

void CPropertyTreeControlBar::SetPrintInput(CPrintInput* pPrintInput)
{
	if (pPrintInput)
	{
		pPrintInput->Insert(&this->m_wndTree, this->m_nodePrintInput);
	}
}

void CPropertyTreeControlBar::AddZone(CZoneActor* pZone, HTREEITEM hInsertAfter)
{
	this->m_bSelectingProp = TRUE; // HACK
	ASSERT(pZone);
	pZone->Insert(this, hInsertAfter);
	this->m_wndTree.RedrawWindow();
	this->m_bSelectingProp = FALSE; // HACK
}

void CPropertyTreeControlBar::RemoveZone(CZoneActor* pZone)
{
	ASSERT(pZone);
	pZone->Remove(this);
	m_wndTree.RedrawWindow();
}

CTimeControl2* CPropertyTreeControlBar::GetTimeControl2(void)
{
	CTreeCtrlNode node = this->GetTimeControl2Node();
	ASSERT(node.GetData());
	return (CTimeControl2*)node.GetData();
}

CPrintFreq* CPropertyTreeControlBar::GetPrintFrequency(void)
{
	CTreeCtrlNode node = this->GetPrintFrequencyNode();
	ASSERT(node.GetData());
	return (CPrintFreq*)node.GetData();
}

void CPropertyTreeControlBar::SelectGridNode(void)
{
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

void CPropertyTreeControlBar::SetGridActor(CGridActor* pGridActor)
{
	ASSERT(pGridActor != NULL);
	pGridActor->Insert(&m_wndTree, m_nodeGrid);
	m_wndTree.RedrawWindow();
}

void CPropertyTreeControlBar::DeleteContents()
{
	this->m_wndTree.DeleteAllItems();

	// populate static properties
	this->m_nodeFlowOnly       = this->m_wndTree.InsertItem(szFLOW_ONLY          );
	this->m_nodeSteadyFlow     = this->m_wndTree.InsertItem(szSTEADY_FLOW        );
	this->m_nodeFreeSurface    = this->m_wndTree.InsertItem(szFREE_SURFACE_BC    );
	this->m_nodeSolutionMethod = this->m_wndTree.InsertItem(szSOLUTION_METHOD    );
	this->m_nodeUnits          = this->m_wndTree.InsertItem(szUNITS              );
	this->m_nodeGrid           = this->m_wndTree.InsertItem(szGRID               );
	this->m_nodeMedia          = this->m_wndTree.InsertItem(szMEDIA              );
	this->m_nodeIC             = this->m_wndTree.InsertItem(szINITIAL_CONDITIONS );
	this->m_nodeICHead         = this->m_nodeIC.AddTail(szIC_HEAD);
	this->m_nodeICChem         = this->m_nodeIC.AddTail(szIC_CHEM);

	// populate the first simulation period
	//
	this->m_nodeBC           = this->m_wndTree.InsertItem(szBOUNDARY_CONDITIONS );
	this->m_nodeWells        = this->m_wndTree.InsertItem(szWELLS               );
	this->m_nodeRivers       = this->m_wndTree.InsertItem(szRIVERS              );
	this->m_nodePrintInput   = this->m_wndTree.InsertItem(szPRINT_INITIAL       );
	this->m_nodePF           = this->m_wndTree.InsertItem(szPRINT_FREQUENCY     );
	this->m_nodeTimeControl2 = this->m_wndTree.InsertItem(szTIME_CONTROL        );

	// set initial checkmark states (eyes)
	this->m_wndTree.SetItemState(this->m_nodeMedia,  INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	this->m_wndTree.SetItemState(this->m_nodeGrid,   INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	this->m_wndTree.SetItemState(this->m_nodeIC,     INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	this->m_wndTree.SetItemState(this->m_nodeBC,     INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	this->m_wndTree.SetItemState(this->m_nodeWells,  INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	this->m_wndTree.SetItemState(this->m_nodeRivers, INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
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
			this->m_bSelectingProp = TRUE;
			if (CZoneActor* pZoneActor = CZoneActor::SafeDownCast(pProp))
			{
				bool bFound = false;
				CTreeCtrlNode parent = this->GetMediaNode();
				for (int i = 0; !bFound && i < parent.GetChildCount(); ++i)
				{
					CTreeCtrlNode node = parent.GetChildAt(i);
					if (node.GetData())
					{
						CZoneActor *pZoneActor = (CZoneActor*)node.GetData();
						if (pZoneActor == pObject)
						{
							node.Select();
							bFound = true;
							break;
						}
					}
				}
				parent = this->GetBCNode();
				for (int i = 0; !bFound && i < parent.GetChildCount(); ++i)
				{
					CTreeCtrlNode node = parent.GetChildAt(i);
					if (node.GetData())
					{
						CZoneActor *pZoneActor = (CZoneActor*)node.GetData();
						if (pZoneActor == pObject)
						{
							node.Select();
							bFound = true;
							break;
						}
					}
				}
				parent = this->GetICHeadNode();
				for (int i = 0; !bFound && i < parent.GetChildCount(); ++i)
				{
					CTreeCtrlNode node = parent.GetChildAt(i);
					if (node.GetData())
					{
						CZoneActor *pZoneActor = (CZoneActor*)node.GetData();
						if (pZoneActor == pObject)
						{
							node.Select();
							bFound = true;
							break;
						}
					}
				}
				parent = this->GetICChemNode();
				for (int i = 0; !bFound && i < parent.GetChildCount(); ++i)
				{
					CTreeCtrlNode node = parent.GetChildAt(i);
					if (node.GetData())
					{
						CZoneActor *pZoneActor = (CZoneActor*)node.GetData();
						if (pZoneActor == pObject)
						{
							node.Select();
							bFound = true;
							break;
						}
					}
				}
				ASSERT(bFound);
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
			this->m_bSelectingProp = FALSE;
		}
		else if (pObject == 0)
		{
			this->m_bSelectingProp = FALSE;
			this->ClearSelection();
		}
		break;

	case WPN_VISCHANGED:
		ASSERT(FALSE);
		break;

	case WPN_SCALE_CHANGED:
		break;

	default:
		ASSERT(FALSE);
	}
}

void CPropertyTreeControlBar::OnDestroy()
{
	CSizingControlBarCFVS7::OnDestroy();

	// Add your message handler code here

	// free time_controls
	// Used only to show no memory leaks on exit
	//
	ASSERT(::IsWindow(this->m_wndTree.m_hWnd));
	if (::IsWindow(this->m_wndTree.m_hWnd))
	{
		this->DeleteContents();
	}

	// free image list
	delete this->m_pImageList;
}

void CPropertyTreeControlBar::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	TRACE("OnKeyDown\n");
	NMTVKEYDOWN *pNMTVKEYDOWN = (LPNMTVKEYDOWN)pNMHDR;

	if (pNMTVKEYDOWN->wVKey == VK_DELETE)
	{
		// code moved to OnEditClear
		ASSERT(FALSE);
	}
	else if (pNMTVKEYDOWN->wVKey == VK_SPACE)
	{
		CTreeCtrlNode item = this->GetTreeCtrlEx()->GetSelectedItem();
		if (!item) return;

		// determine next state
		//
		UINT nNewState;
		switch (this->GetNodeCheck(item))
		{
			case BST_UNCHECKED:
				// currently unchecked
				nNewState = BST_CHECKED;
				break;
			case BST_CHECKED:
				// currently checked
				nNewState = BST_UNCHECKED;
				break;
			default:
				return;
				break;
		}

		// set new state
		//
		this->SetNodeCheck(item, nNewState);

		// re-render
		//
		if (CWPhastDoc *pDoc = this->GetDocument())
		{
			pDoc->UpdateAllViews(0);
		}

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

void CPropertyTreeControlBar::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: Add your message handler code here
}

void CPropertyTreeControlBar::ClearSelection(void)
{
	this->SelectWithoutNotification(0);
}

bool CPropertyTreeControlBar::IsNodeDraggable(CTreeCtrlNode dragNode, COleDataSource &oleDataSource)
{
	std::ostringstream oss;

	if (dragNode.GetParent() == this->GetMediaNode())
	{
		if (dragNode.GetData())
		{
			if (CMediaZoneActor* pZone = CMediaZoneActor::SafeDownCast((vtkObject*)dragNode.GetData()))
			{
				if (!pZone->GetDefault())
				{
					CGridElt elt = pZone->GetGridElt();
					oss << "MEDIA\n";
					oss << elt;
					oss << "\n";

					std::string s = oss.str();
					HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
					LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

					::lstrcpy(pData, s.c_str());
					::GlobalUnlock(hGlobal);

					oleDataSource.CacheGlobalData(this->m_cfPID, hGlobal);
					oleDataSource.CacheGlobalData(CF_TEXT, hGlobal);

					return true;
				}
			}
		}
	}

	if (dragNode.GetParent() == this->GetBCNode())
	{
		if (dragNode.GetData())
		{
			if (CBCZoneActor* pZone = CBCZoneActor::SafeDownCast((vtkObject*)dragNode.GetData()))
			{
				ASSERT(!pZone->GetDefault());

				CBC b = pZone->GetBC();
				oss << b;
				oss << "\n";

				std::string s = oss.str();
				HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
				LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

				::lstrcpy(pData, s.c_str());
				::GlobalUnlock(hGlobal);

				oleDataSource.CacheGlobalData(this->m_cfPID, hGlobal);
				oleDataSource.CacheGlobalData(CF_TEXT, hGlobal);

				return true;
			}
		}
	}

	if (dragNode.GetParent() == this->GetICHeadNode())
	{
		if (dragNode.GetData())
		{
			if (CICHeadZoneActor *pZone = CICHeadZoneActor::SafeDownCast((vtkObject*)dragNode.GetData()))
			{
				if (!pZone->GetDefault())
				{
					oss << pZone->GetData();
					oss << "\n";

					std::string s = oss.str();
					HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
					LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

					::lstrcpy(pData, s.c_str());
					::GlobalUnlock(hGlobal);

					oleDataSource.CacheGlobalData(this->m_cfPID, hGlobal);
					oleDataSource.CacheGlobalData(CF_TEXT, hGlobal);

					return true;
				}
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		else
		{
			ASSERT(FALSE);
		}
	}

	if (dragNode.GetParent() == this->GetICChemNode())
	{
		if (dragNode.GetData())
		{
			if (CICChemZoneActor *pZone = CICChemZoneActor::SafeDownCast((vtkObject*)dragNode.GetData()))
			{
				if (!pZone->GetDefault())
				{
					oss << pZone->GetData();
					oss << "\n";

					std::string s = oss.str();
					HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
					LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

					::lstrcpy(pData, s.c_str());
					::GlobalUnlock(hGlobal);

					oleDataSource.CacheGlobalData(this->m_cfPID, hGlobal);
					oleDataSource.CacheGlobalData(CF_TEXT, hGlobal);

					return true;
				}
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		else
		{
			ASSERT(FALSE);
		}
	}

	if (dragNode.GetParent() == this->GetWellsNode())
	{
		if (dragNode.GetData())
		{
			if (CWellActor *pWellActor = CWellActor::SafeDownCast((vtkObject*)dragNode.GetData()))
			{
				oss << pWellActor->GetWell();

				std::string s = oss.str();
				HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
				LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

				::lstrcpy(pData, s.c_str());
				::GlobalUnlock(hGlobal);

				oleDataSource.CacheGlobalData(this->m_cfPID, hGlobal);
				oleDataSource.CacheGlobalData(CF_TEXT, hGlobal);

				return true;
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		else
		{
			ASSERT(FALSE);
		}
	}

	if (dragNode.GetParent() == this->GetRiversNode())
	{
		if (dragNode.GetData())
		{
			if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast((vtkObject*)dragNode.GetData()))
			{
				oss << pRiverActor->GetRiver();

				std::string s = oss.str();
				HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
				LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

				::lstrcpy(pData, s.c_str());
				::GlobalUnlock(hGlobal);

				oleDataSource.CacheGlobalData(this->m_cfPID, hGlobal);
				oleDataSource.CacheGlobalData(CF_TEXT, hGlobal);

				return true;
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		else
		{
			ASSERT(FALSE);
		}
	}

	return false;
}

void CPropertyTreeControlBar::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	TRACE("OnBeginDrag\n");

	NMTREEVIEW* pNMTreeView = (NMTREEVIEW*)pNMHDR;

	CTreeCtrlNode dragNode(pNMTreeView->itemNew.hItem, &this->m_wndTree);
	dragNode.Select();

	COleDataSource oleDataSource;

	if (!this->IsNodeDraggable(dragNode, oleDataSource))
	{
		return;
	}

	this->m_dragNode = dragNode;

	DROPEFFECT dwDropEffect = oleDataSource.DoDragDrop(DROPEFFECT_MOVE);

	if ((dwDropEffect & DROPEFFECT_MOVE) == DROPEFFECT_MOVE)
	{
		// see PasteString
		// remove the node being dragged ???
	}
	else if ((dwDropEffect & DROPEFFECT_NONE) == DROPEFFECT_NONE)
	{
		// do nothing
	}
	else
	{
		TRACE("WARNING: Unexpected DropEffect\n");
	}

	this->m_dragNode = CTreeCtrlNode();
	this->m_wndTree.SetInsertMark(0);

}

DROPEFFECT CPropertyTreeControlBar::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	TRACE("CPropertyTreeControlBar::OnDragEnter\n");
	ASSERT_VALID(this);

	if (pDataObject->IsDataAvailable(this->m_cfPID) == TRUE)
	{
		return DROPEFFECT_MOVE;
	}
	return DROPEFFECT_NONE;
}

DROPEFFECT CPropertyTreeControlBar::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	TRACE("CPropertyTreeControlBar::OnDragOver\n");
	ASSERT_VALID(this);

	if (pDataObject->IsDataAvailable(this->m_cfPID) == TRUE)
	{
		bool bHasDefault = false;
		CTreeCtrlNode parentNode = this->m_dragNode.GetParent();
		if (parentNode == this->GetMediaNode())
		{
			bHasDefault = true;
		}
		if (parentNode == this->GetICHeadNode())
		{
			bHasDefault = true;
		}
		if (parentNode == this->GetICChemNode())
		{
			bHasDefault = true;
		}

		CTreeCtrlNode hitNode = this->m_wndTree.HitTest(point);
		if ( hitNode && hitNode.IsNodeAncestor(parentNode) && (!bHasDefault || hitNode != parentNode) )
		{
			if (hitNode == parentNode)
			{
				// drop location will be the first position under the heading
				//
				ASSERT(!bHasDefault);
				this->m_wndTree.SetInsertMark(hitNode);
			}
			else
			{
				// the hitNode is a descendant of a sibling of the dragNode
				//
				CTreeCtrlNode sibling = hitNode;
				while (sibling)
				{
					if (sibling.GetParent() == parentNode) break;
					sibling = sibling.GetParent();
				}
				ASSERT(sibling);

				// find last visible node
				//
				CTreeCtrlNode last_visible = sibling;
				while (last_visible.HasChildren() && (last_visible.GetState(TVIS_EXPANDED) & TVIS_EXPANDED) != 0)
				{
					last_visible = last_visible.GetLastChild();
				}
				ASSERT(last_visible);
				this->m_wndTree.SetInsertMark(last_visible);
			}
			return DROPEFFECT_MOVE;
		}
		else
		{
			// erase insertion mark
			//
			this->m_wndTree.SetInsertMark(0);
		}
	}
	return DROPEFFECT_NONE;
}

BOOL CPropertyTreeControlBar::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	TRACE("CPropertyTreeControlBar::OnDrop\n");

	if (pDataObject->IsDataAvailable(this->m_cfPID) == TRUE)
	{
		bool bHasDefault = false;
		CTreeCtrlNode parentNode = this->m_dragNode.GetParent();
		if (parentNode == this->GetMediaNode())
		{
			bHasDefault = true;
		}
		if (parentNode == this->GetICHeadNode())
		{
			bHasDefault = true;
		}
		if (parentNode == this->GetICChemNode())
		{
			bHasDefault = true;
		}

		CTreeCtrlNode hitNode = this->m_wndTree.HitTest(point);
		if ( hitNode && hitNode.IsNodeAncestor(parentNode) && (!bHasDefault || hitNode != parentNode) )
		{
			CTreeCtrlNode dropNode;
			if (hitNode == parentNode)
			{
				// drop location will be the first position under the heading
				//
				ASSERT(!bHasDefault);
				dropNode = CTreeCtrlNode(TVI_FIRST, this->GetTreeCtrlEx());
			}
			else
			{
				// the hitNode is a descendant of a sibling of the dragNode
				//
				CTreeCtrlNode sibling = hitNode;
				while (sibling)
				{
					if (sibling.GetParent() == parentNode) break;
					sibling = sibling.GetParent();
				}
				ASSERT(sibling);
				dropNode = sibling;
			}

			if (dropNode && dropNode != this->m_dragNode && this->m_dragNode.GetPrevSibling() != dropNode)
			{
				ASSERT(dropNode == TVI_FIRST || dropNode.GetParent() == parentNode);
				if (CZoneActor::SafeDownCast((vtkObject*)this->m_dragNode.GetData()))
				{
					if (CWPhastDoc* pDoc = this->GetDocument())
					{
						CAction* pAction = new CDragDropAction<CZoneActor>(this, this->m_dragNode, dropNode);
						if (pAction)
						{
							pDoc->Execute(pAction);
							return DROPEFFECT_NONE;
						}
					}
				}
				if (CWellActor::SafeDownCast((vtkObject*)this->m_dragNode.GetData()))
				{
					if (CWPhastDoc* pDoc = this->GetDocument())
					{
						CAction* pAction = new CDragDropAction<CWellActor>(this, this->m_dragNode, dropNode);
						if (pAction)
						{
							pDoc->Execute(pAction);
							return DROPEFFECT_NONE;
						}
					}
				}
				if (CRiverActor::SafeDownCast((vtkObject*)this->m_dragNode.GetData()))
				{
					if (CWPhastDoc* pDoc = this->GetDocument())
					{
						CAction* pAction = new CDragDropAction<CRiverActor>(this, this->m_dragNode, dropNode);
						if (pAction)
						{
							pDoc->Execute(pAction);
							return DROPEFFECT_NONE;
						}
					}
				}
			}
		}
	}
	return DROPEFFECT_NONE;
}

void CPropertyTreeControlBar::OnDragLeave(CWnd* pWnd)
{
	TRACE("CPropertyTreeControlBar::OnDragLeave\n");
}

bool CPropertyTreeControlBar::IsNodeCopyable(CTreeCtrlNode copyNode, COleDataSource *pOleDataSource)
{
	std::ostringstream oss;

	if (copyNode.GetParent() == this->GetMediaNode())
	{
		if (copyNode.GetData())
		{
			if (CMediaZoneActor* pZone = CMediaZoneActor::SafeDownCast((vtkObject*)copyNode.GetData()))
			{
				if (pOleDataSource)
				{
					CGridElt elt = pZone->GetData();

					//{{ Why?
					if (pZone->GetDefault() && elt.active == NULL)
					{
						elt.active = new Cproperty(1);
					}
					//}} Why?

					// CF_TEXT
					//
					oss << "MEDIA\n";
					oss << elt;
					oss << "\n";

					std::string s = oss.str();
					HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
					LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

					::lstrcpy(pData, s.c_str());
					::GlobalUnlock(hGlobal);

					pOleDataSource->CacheGlobalData(CF_TEXT, hGlobal);

					// copy Media clip format
					//
					CSharedFile globFile;
					CArchive ar(&globFile, CArchive::store);
					CGridElt elt2 = pZone->GetData();
					elt.Serialize(ar);

					ar.Close();

					pOleDataSource->CacheGlobalData(CGridElt::clipFormat, globFile.Detach());

					// Description (CZoneActor::clipFormat)
					//
					if (pZone->GetDesc())
					{
						CSharedFile sf;
						CArchive arch(&sf, CArchive::store);
						CString desc(pZone->GetDesc());
						arch << desc;
						arch.Close();
						pOleDataSource->CacheGlobalData(CZoneActor::clipFormat, sf.Detach());
					}
				}
				return true;
			}
		}
	}

	if (copyNode.GetParent() == this->GetICHeadNode())
	{
		if (copyNode.GetData())
		{
			if (CICHeadZoneActor* pZone = CICHeadZoneActor::SafeDownCast((vtkObject*)copyNode.GetData()))
			{
				if (pOleDataSource)
				{
					// CF_TEXT
					//
					oss << pZone->GetData();
					oss << "\n";

					std::string s = oss.str();
					HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
					LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

					::lstrcpy(pData, s.c_str());
					::GlobalUnlock(hGlobal);

					pOleDataSource->CacheGlobalData(CF_TEXT, hGlobal);

					// copy HeadIC clip format
					//
					CSharedFile globFile;
					CArchive ar(&globFile, CArchive::store);
					pZone->GetData().Serialize(ar);
					ar.Close();

					pOleDataSource->CacheGlobalData(CHeadIC::clipFormat, globFile.Detach());

					// Description (CZoneActor::clipFormat)
					//
					if (pZone->GetDesc())
					{
						CSharedFile sf;
						CArchive arch(&sf, CArchive::store);
						CString desc(pZone->GetDesc());
						arch << desc;
						arch.Close();
						pOleDataSource->CacheGlobalData(CZoneActor::clipFormat, sf.Detach());
					}
				}
				return true;
			}
		}
	}

	if (copyNode.GetParent() == this->GetICChemNode())
	{
		if (copyNode.GetData())
		{
			if (CICChemZoneActor* pZone = CICChemZoneActor::SafeDownCast((vtkObject*)copyNode.GetData()))
			{
				if (pOleDataSource)
				{
					// CF_TEXT
					//
					oss << pZone->GetData();
					oss << "\n";

					std::string s = oss.str();
					HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
					LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

					::lstrcpy(pData, s.c_str());
					::GlobalUnlock(hGlobal);

					pOleDataSource->CacheGlobalData(CF_TEXT, hGlobal);

					// copy ChemIC clip format
					//
					CSharedFile globFile;
					CArchive ar(&globFile, CArchive::store);
					pZone->GetData().Serialize(ar);
					ar.Close();

					pOleDataSource->CacheGlobalData(CChemIC::clipFormat, globFile.Detach());

					// Description (CZoneActor::clipFormat)
					//
					if (pZone->GetDesc())
					{
						CSharedFile sf;
						CArchive arch(&sf, CArchive::store);
						CString desc(pZone->GetDesc());
						arch << desc;
						arch.Close();
						pOleDataSource->CacheGlobalData(CZoneActor::clipFormat, sf.Detach());
					}
				}
				return true;
			}
		}
	}

	if (copyNode.GetParent() == this->GetBCNode())
	{
		if (copyNode.GetData())
		{
			if (CBCZoneActor* pZone = CBCZoneActor::SafeDownCast((vtkObject*)copyNode.GetData()))
			{
				if (pOleDataSource)
				{
					CBC bc = pZone->GetBC();

					// CF_TEXT
					//
					oss << bc;
					oss << "\n";

					std::string s = oss.str();
					HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
					LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

					::lstrcpy(pData, s.c_str());
					::GlobalUnlock(hGlobal);

					pOleDataSource->CacheGlobalData(CF_TEXT, hGlobal);

					// copy BC clip format
					//
					CSharedFile globFile;
					CArchive ar(&globFile, CArchive::store);
					bc.Serialize(ar);
					ar.Close();

					pOleDataSource->CacheGlobalData(CBC::clipFormat, globFile.Detach());

					// Description (CZoneActor::clipFormat)
					//
					if (pZone->GetDesc())
					{
						CSharedFile sf;
						CArchive arch(&sf, CArchive::store);
						CString desc(pZone->GetDesc());
						arch << desc;
						arch.Close();
						pOleDataSource->CacheGlobalData(CZoneActor::clipFormat, sf.Detach());
					}
				}
				return true;
			}
		}
	}

	if (copyNode.GetParent() == this->GetWellsNode())
	{
		if (copyNode.GetData())
		{
			if (CWellActor* pWellActor = CWellActor::SafeDownCast((vtkObject*)copyNode.GetData()))
			{
				if (pOleDataSource)
				{
					CWellSchedule well = pWellActor->GetWell();

					// CF_TEXT
					//
					oss << well;

					std::string s = oss.str();
					HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
					LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

					::lstrcpy(pData, s.c_str());
					::GlobalUnlock(hGlobal);

					pOleDataSource->CacheGlobalData(CF_TEXT, hGlobal);

					// copy BC clip format
					//
					CSharedFile globFile;
					CArchive ar(&globFile, CArchive::store);
					well.Serialize(ar);
					ar.Close();

					pOleDataSource->CacheGlobalData(CWellSchedule::clipFormat, globFile.Detach());
				}
				return true;
			}
		}
	}

	if (copyNode.GetParent() == this->GetRiversNode())
	{
		if (copyNode.GetData())
		{
			if (CRiverActor* pRiverActor = CRiverActor::SafeDownCast((vtkObject*)copyNode.GetData()))
			{
				if (pOleDataSource)
				{
					CRiver river = pRiverActor->GetRiver();

					// CF_TEXT
					//
					oss << river;

					std::string s = oss.str();
					HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, s.length() + 1);
					LPTSTR pData = (LPTSTR)::GlobalLock(hGlobal);

					::lstrcpy(pData, s.c_str());
					::GlobalUnlock(hGlobal);

					pOleDataSource->CacheGlobalData(CF_TEXT, hGlobal);

					// copy BC clip format
					//
					CSharedFile globFile;
					CArchive ar(&globFile, CArchive::store);
					river.Serialize(ar);
					ar.Close();

					pOleDataSource->CacheGlobalData(CRiver::clipFormat, globFile.Detach());
				}
				return true;
			}
		}
	}

	return false;
}

CLIPFORMAT CPropertyTreeControlBar::GetNativeClipFormat(void)const
{
	CLIPFORMAT type = 0;
	type = this->GetZoneClipFormat();
	if (type)
	{
		return type;
	}

	COleDataObject dataObject;
	if (dataObject.AttachClipboard())
	{
		if (dataObject.IsDataAvailable(CWellSchedule::clipFormat))
		{
			type = CWellSchedule::clipFormat;
		}
		if (dataObject.IsDataAvailable(CRiver::clipFormat))
		{
			type = CRiver::clipFormat;
		}
	}
	return type;
}

CLIPFORMAT CPropertyTreeControlBar::GetZoneClipFormat(void)const
{
	CLIPFORMAT type = 0;

	COleDataObject dataObject;
	if (dataObject.AttachClipboard())
	{
		if (dataObject.IsDataAvailable(CGridElt::clipFormat))
		{
			type = CGridElt::clipFormat;
		}
		else if (dataObject.IsDataAvailable(CHeadIC::clipFormat))
		{
			type = CHeadIC::clipFormat;
		}
		else if (dataObject.IsDataAvailable(CChemIC::clipFormat))
		{
			type = CChemIC::clipFormat;
		}
		else if (dataObject.IsDataAvailable(CBC::clipFormat))
		{
			type = CBC::clipFormat;
		}
	}
	return type;
}

bool CPropertyTreeControlBar::IsNodePasteable(CTreeCtrlNode pasteNode, bool bDoPaste)
{
	if (!this->GetNativeClipFormat())
	{
		return false;
	}

	if (pasteNode.IsNodeAncestor(this->GetMediaNode()))
	{
		return this->IsNodePasteable<CMediaZoneActor, CGridElt>(this->GetMediaNode(), pasteNode, bDoPaste);
	}
	else if (pasteNode.IsNodeAncestor(this->GetICHeadNode()))
	{
		return this->IsNodePasteable<CICHeadZoneActor, CHeadIC>(this->GetICHeadNode(), pasteNode, bDoPaste);
	}
	else if (pasteNode.IsNodeAncestor(this->GetICChemNode()))
	{
		return this->IsNodePasteable<CICChemZoneActor, CChemIC>(this->GetICChemNode(), pasteNode, bDoPaste);
	}
	else if (pasteNode.IsNodeAncestor(this->GetBCNode()))
	{
		return this->IsNodePasteable<CBCZoneActor, CBC>(this->GetBCNode(), pasteNode, bDoPaste);
	}
	else if (pasteNode.IsNodeAncestor(this->GetWellsNode()))
	{
		return this->IsNodePasteableWell(pasteNode, bDoPaste);
	}
	else if (pasteNode.IsNodeAncestor(this->GetRiversNode()))
	{
		return this->IsNodePasteableRiver(pasteNode, bDoPaste);
	}
	return false;
}

bool CPropertyTreeControlBar::IsNodePasteableWell(CTreeCtrlNode pasteNode, bool bDoPaste)
{
	CTreeCtrlNode headNode = this->GetWellsNode();

	CLIPFORMAT type = this->GetNativeClipFormat();
	if (type == CWellSchedule::clipFormat && pasteNode.IsNodeAncestor(headNode))
	{
		if (bDoPaste)
		{
			COleDataObject dataObject;
			VERIFY(dataObject.AttachClipboard());
			HGLOBAL hGlob = dataObject.GetGlobalData(type);
			if (hGlob == NULL) return false;

			CSharedFile globFile;
			globFile.SetHandle(hGlob, FALSE);
			CArchive ar(&globFile, CArchive::load);

			CWellSchedule well;
			well.Serialize(ar);
			ar.Close();

			if (CWPhastDoc *pWPhastDoc = this->GetDocument())
			{
				CTreeCtrlNode after(TVI_LAST, this->GetTreeCtrlEx());
				if (pasteNode != headNode)
				{
					while (pasteNode.GetParent() != headNode)
					{
						pasteNode = pasteNode.GetParent();
						if (!pasteNode) break;
					}
					if (pasteNode && pasteNode.GetParent() == headNode)
					{
						after = pasteNode;
					}
				}
				well.n_user = pWPhastDoc->GetNextWellNumber();
				CWellCreateAction* pAction = new CWellCreateAction(pWPhastDoc, well, after);
				pWPhastDoc->Execute(pAction);
			}
		}
		return true;
	}
	return false;
}

bool CPropertyTreeControlBar::IsNodePasteableRiver(CTreeCtrlNode pasteNode, bool bDoPaste)
{
	CTreeCtrlNode headNode = this->GetRiversNode();

	CLIPFORMAT type = this->GetNativeClipFormat();
	if (type == CRiver::clipFormat && pasteNode.IsNodeAncestor(headNode))
	{
		if (bDoPaste)
		{
			COleDataObject dataObject;
			VERIFY(dataObject.AttachClipboard());
			HGLOBAL hGlob = dataObject.GetGlobalData(type);
			if (hGlob == NULL) return false;

			CSharedFile globFile;
			globFile.SetHandle(hGlob, FALSE);
			CArchive ar(&globFile, CArchive::load);

			CRiver river;
			river.Serialize(ar);
			ar.Close();

			if (CWPhastDoc *pWPhastDoc = this->GetDocument())
			{
				CTreeCtrlNode after(TVI_LAST, this->GetTreeCtrlEx());
				if (pasteNode != headNode)
				{
					while (pasteNode.GetParent() != headNode)
					{
						pasteNode = pasteNode.GetParent();
						if (!pasteNode) break;
					}
					if (pasteNode && pasteNode.GetParent() == headNode)
					{
						after = pasteNode;
					}
				}
				river.n_user = pWPhastDoc->GetNextRiverNumber();
				CRiverCreateAction* pAction = new CRiverCreateAction(pWPhastDoc, river, after);
				pWPhastDoc->Execute(pAction);
			}
		}
		return true;
	}
	return false;
}

// COMMENT: {4/11/2006 6:55:07 PM}template<typename ZT, typename DT>
// COMMENT: {4/11/2006 6:55:07 PM}bool GetBCType(CWPhastDoc *pWPhastDoc, CZoneCreateAction<ZT>* pAction);

template<typename ZT, typename DT>
bool CPropertyTreeControlBar::IsNodePasteable(CTreeCtrlNode headNode, CTreeCtrlNode pasteNode, bool bDoPaste)
{
	CLIPFORMAT type = this->GetZoneClipFormat();
	if (type && pasteNode.IsNodeAncestor(headNode))
	{
		if (bDoPaste)
		{
			COleDataObject dataObject;
			VERIFY(dataObject.AttachClipboard());
			HGLOBAL hGlob = dataObject.GetGlobalData(type);
			if (hGlob == NULL) return false;

			CSharedFile globFile;
			globFile.SetHandle(hGlob, FALSE);
			CArchive ar(&globFile, CArchive::load);

			DT data;
			CZone zone;
			if (type == DT::clipFormat)
			{
				//{{HACK
				// all should be null or non-null
				if (data.zone == NULL)
				{
					data.zone = new CZone();
				}
				//}}HACK
				data.Serialize(ar);
				zone = *data.zone;
			}
			else if (type == CGridElt::clipFormat)
			{
				CGridElt elt;
				elt.Serialize(ar);
				zone = *elt.zone;
			}
			else if (type == CHeadIC::clipFormat)
			{
				CHeadIC headIC;
				headIC.zone = new CZone();
				headIC.Serialize(ar);
				zone = *headIC.zone;
			}
			else if (type == CChemIC::clipFormat)
			{
				CChemIC chemIC;
				chemIC.zone = new CZone();
				chemIC.Serialize(ar);
				zone = *chemIC.zone;
			}
			else if (type == CBC::clipFormat)
			{
				CBC bc;
				bc.Serialize(ar);
				zone = *bc.zone;
			}
			else
			{
				ASSERT(FALSE);
			}
			ar.Close();

			if (CWPhastDoc *pWPhastDoc = this->GetDocument())
			{
				CTreeCtrlNode after(TVI_LAST, this->GetTreeCtrlEx());
				if (pasteNode != headNode)
				{
					while (pasteNode.GetParent() != headNode)
					{
						pasteNode = pasteNode.GetParent();
						if (!pasteNode) break;
					}
					if (pasteNode && pasteNode.GetParent() == headNode)
					{
						after = pasteNode;
					}
				}
				// Description (CZoneActor::clipFormat)
				//
				CString strDesc;
				if (HGLOBAL hDesc = dataObject.GetGlobalData(CZoneActor::clipFormat))
				{
					CSharedFile descFile;
					descFile.SetHandle(hDesc, FALSE);
					CArchive descArch(&descFile, CArchive::load);
					descArch >> strDesc;
				}
				CZoneCreateAction<ZT>* pAction = new CZoneCreateAction<ZT>(
					pWPhastDoc,
					pWPhastDoc->GetNextZoneName(),
					zone.x1,
					zone.x2,
					zone.y1,
					zone.y2,
					zone.z1,
					zone.z2,
					strDesc.IsEmpty() ? NULL : strDesc,
					after
					);
				if (headNode == this->GetBCNode())
				{
					return GetBCType<ZT, DT>(pWPhastDoc, pAction, type, data);
				}
				if (type == DT::clipFormat)
				{
					pAction->GetZoneActor()->SetData(data);
				}
				pWPhastDoc->Execute(pAction);
			}
		}
		return true;
	}
	return false;
}

template<typename ZT, typename DT>
bool GetBCType(CWPhastDoc *pWPhastDoc, CZoneCreateAction<ZT>* pAction, CLIPFORMAT type, DT& data)
{
	ASSERT(FALSE); // this should never be called; only needed to compile
	delete pAction;
	return false;
}

// specialization for CBCZoneActor, CBC
template<>
bool GetBCType<CBCZoneActor, CBC>(CWPhastDoc *pWPhastDoc, CZoneCreateAction<CBCZoneActor>* pAction, CLIPFORMAT cb_type, CBC& data)
{
	// get type of boundary condition
	CBCTypeDialog dlg;
	if (cb_type == CBC::clipFormat)
	{
		dlg.bc_type = data.bc_type;
	}
// COMMENT: {4/11/2006 9:53:05 PM}	::MessageBeep(-1);
// COMMENT: {4/11/2006 9:53:05 PM}	::Sleep(500);
// COMMENT: {4/11/2006 9:53:05 PM}	::MessageBeep(MB_ICONASTERISK);
// COMMENT: {4/11/2006 9:53:05 PM}	::Sleep(500);
	::MessageBeep(MB_ICONEXCLAMATION);
// COMMENT: {4/11/2006 9:53:09 PM}	::Sleep(500);
// COMMENT: {4/11/2006 9:53:09 PM}	::MessageBeep(MB_ICONHAND);
// COMMENT: {4/11/2006 9:53:09 PM}	::Sleep(500);
// COMMENT: {4/11/2006 9:53:09 PM}	::MessageBeep(MB_ICONQUESTION);
// COMMENT: {4/11/2006 9:53:09 PM}	::Sleep(500);
// COMMENT: {4/11/2006 9:53:09 PM}	::MessageBeep(MB_OK);	
// COMMENT: {4/11/2006 9:53:09 PM}	::Sleep(500);
	if (dlg.DoModal() != IDOK)
	{
		delete pAction;
		return true;
	}

	CBC bc;
	bc.bc_type = dlg.bc_type;    // SPECIFIED, FLUX, LEAKY
	if (cb_type == CBC::clipFormat)
	{
		if (bc.bc_type == data.bc_type)
		{
			// copied type is the same as pasted type
			bc = data;
		}
	}
	ASSERT(bc.bc_type == SPECIFIED || bc.bc_type == FLUX || bc.bc_type == LEAKY);
	pAction->GetZoneActor()->SetData(bc);
	pWPhastDoc->Execute(pAction);
	return true;
}


// COMMENT: {4/11/2006 6:36:10 PM}template<typename ZT, typename DT>
// COMMENT: {4/11/2006 6:36:10 PM}void CTimeSeries<ZT, DT>::ForceBC(DT data, int bc_type)
// COMMENT: {4/11/2006 6:36:10 PM}{
// COMMENT: {4/11/2006 6:36:10 PM}	CBC bc;
// COMMENT: {4/11/2006 6:36:10 PM}	bc.bc_type = bc_type;
// COMMENT: {4/11/2006 6:36:10 PM}
// COMMENT: {4/11/2006 6:36:10 PM}}
// COMMENT: {4/11/2006 6:36:10 PM}
// COMMENT: {4/11/2006 6:36:10 PM}// specialization for CBCZoneActor
// COMMENT: {4/11/2006 6:36:10 PM}template<>
// COMMENT: {4/11/2006 6:36:10 PM}HTREEITEM CTimeSeries<CBCZoneActor>::InsertItem(CTreeCtrl* pTreeCtrl, LPCTSTR lpszHeading, HTREEITEM hParent /* = TVI_ROOT*/, HTREEITEM hInsertAfter /* = TVI_LAST*/)const
// COMMENT: {4/11/2006 6:36:10 PM}{
// COMMENT: {4/11/2006 6:36:10 PM}	HTREEITEM item = 0;
// COMMENT: {4/11/2006 6:36:10 PM}	if (!pTreeCtrl || this->size() == 0) return item;
// COMMENT: {4/11/2006 6:36:10 PM}
// COMMENT: {4/11/2006 6:36:10 PM}	item = pTreeCtrl->InsertItem(lpszHeading, hParent, hInsertAfter);
// COMMENT: {4/11/2006 6:36:10 PM}	CTimeSeries<Cproperty>::const_iterator iter = this->begin();
// COMMENT: {4/11/2006 6:36:10 PM}	for (; iter != this->end(); ++iter)
// COMMENT: {4/11/2006 6:36:10 PM}	{
// COMMENT: {4/11/2006 6:36:10 PM}		ASSERT((*iter).second.type != UNDEFINED);
// COMMENT: {4/11/2006 6:36:10 PM}		if ((*iter).second.type == UNDEFINED) continue;
// COMMENT: {4/11/2006 6:36:10 PM}
// COMMENT: {4/11/2006 6:36:10 PM}		CString str;
// COMMENT: {4/11/2006 6:36:10 PM}		if ((*iter).first.input)
// COMMENT: {4/11/2006 6:36:10 PM}		{
// COMMENT: {4/11/2006 6:36:10 PM}			str.Format("%g %s", (*iter).first.value, (*iter).first.input);
// COMMENT: {4/11/2006 6:36:10 PM}		}
// COMMENT: {4/11/2006 6:36:10 PM}		else
// COMMENT: {4/11/2006 6:36:10 PM}		{
// COMMENT: {4/11/2006 6:36:10 PM}			str.Format("%g", (*iter).first.value);
// COMMENT: {4/11/2006 6:36:10 PM}		}
// COMMENT: {4/11/2006 6:36:10 PM}		iter->second.Insert(pTreeCtrl, item, str);
// COMMENT: {4/11/2006 6:36:10 PM}	}
// COMMENT: {4/11/2006 6:36:10 PM}	return item;
// COMMENT: {4/11/2006 6:36:10 PM}}

void CPropertyTreeControlBar::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	if (CTreeCtrlEx *pTreeCtrlEx = this->GetTreeCtrlEx())
	{
		CTreeCtrlNode dragNode = pTreeCtrlEx->GetSelectedItem();
		if (this->IsNodeCopyable(dragNode, NULL))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CPropertyTreeControlBar::OnEditCopy()
{
	if (CTreeCtrlEx *pTreeCtrlEx = this->GetTreeCtrlEx())
	{
		CWaitCursor wait;

		CTreeCtrlNode dragNode = pTreeCtrlEx->GetSelectedItem();
		COleDataSource* pSrcItem = new COleDataSource();

		if (pSrcItem && this->IsNodeCopyable(dragNode, pSrcItem))
		{
			pSrcItem->SetClipboard();
		}
		else
		{
			delete pSrcItem;
			// sound warning message
			::MessageBeep(MB_ICONEXCLAMATION);
		}
	}
}

void CPropertyTreeControlBar::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	if (CTreeCtrlEx *pTreeCtrlEx = this->GetTreeCtrlEx())
	{
		CTreeCtrlNode pasteNode = pTreeCtrlEx->GetSelectedItem();
		if (pasteNode)
		{
			if (this->IsNodePasteable(pasteNode, false))
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
}

void CPropertyTreeControlBar::OnEditPaste()
{
	if (CTreeCtrlEx *pTreeCtrlEx = this->GetTreeCtrlEx())
	{
		CTreeCtrlNode pasteNode = pTreeCtrlEx->GetSelectedItem();
		if (pasteNode)
		{
			if (!(this->IsNodePasteable(pasteNode, true)))
			{
				// sound warning message
				::MessageBeep(MB_ICONEXCLAMATION);
			}
		}
	}
}

void CPropertyTreeControlBar::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	if (CTreeCtrlEx *pTreeCtrlEx = this->GetTreeCtrlEx())
	{
		CTreeCtrlNode dragNode = pTreeCtrlEx->GetSelectedItem();
		COleDataSource source;
		if (this->IsNodeDraggable(dragNode, source) && this->IsNodeCopyable(dragNode, NULL))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CPropertyTreeControlBar::OnEditCut()
{
	if (CTreeCtrlEx *pTreeCtrlEx = this->GetTreeCtrlEx())
	{
		CWaitCursor wait;
		CTreeCtrlNode cutNode = pTreeCtrlEx->GetSelectedItem();

		COleDataSource source;
		if (this->IsNodeDraggable(cutNode, source))
		{
			COleDataSource* pOleDataSource = new COleDataSource();
			if (pOleDataSource && this->IsNodeCopyable(cutNode, pOleDataSource))
			{
				pOleDataSource->SetClipboard();

				this->OnEditClear();
			}
			else
			{
				delete pOleDataSource;
				// sound warning message
				::MessageBeep(MB_ICONEXCLAMATION);
			}
		}
		else
		{
			// sound warning message
			::MessageBeep(MB_ICONEXCLAMATION);
		}
	}
}

void CPropertyTreeControlBar::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CPropertyTreeControlBar::OnEditClear()
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
// COMMENT: {5/29/2007 6:47:02 PM}							parent.Select();
							pDoc->Execute(new CZoneRemoveAction(pDoc, pZone, this));
						}
					}
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
// COMMENT: {5/29/2007 6:49:02 PM}						VERIFY(parent.Select());
						pDoc->Execute(new CWellDeleteAction(pDoc, pWell));
					}
					return;
				}
			}
		}
		return;
	}

	// RIVERS
	//
	if (sel.IsNodeAncestor(this->GetRiversNode()))
	{
		if (sel != this->GetRiversNode())
		{
			CTreeCtrlNode ptNode = sel;
			while (sel.GetParent() != this->GetRiversNode())
			{
				sel = sel.GetParent();
				if (!sel) break;
			}

			// determine if point is being deleted
			//
			int point = sel.GetIndex(ptNode);

			if (sel && sel.GetData())
			{
				if (CRiverActor* pRiverActor = CRiverActor::SafeDownCast((vtkObject*)sel.GetData()))
				{
					CTreeCtrlNode parent = sel.GetParent();
					if (CWPhastDoc* pDoc = this->GetDocument())
					{
						if (point >= 0)
						{
							if (point == 0 || point == pRiverActor->GetPointCount() - 1)
							{
								::AfxMessageBox("The first and the last river points cannot be deleted.");
							}
							else
							{
								VERIFY(ptNode.GetParent().Select());
								pDoc->Execute(new CRiverDeletePointAction(pRiverActor, pDoc, point));
							}
						}
						else
						{
							VERIFY(parent.Select());
							pDoc->Execute(new CRiverDeleteAction(pDoc, pRiverActor));
						}							
					}
					return;
				}
			}
		}
		return;
	}

	// HEAD_IC
	//
	if (sel.IsNodeAncestor(this->m_nodeICHead))
	{
		if (sel != this->m_nodeICHead)
		{
			while (sel.GetParent() != this->m_nodeICHead)
			{
				sel = sel.GetParent();
				if (!sel) break;
			}
			if (sel.GetData())
			{
				if (CICHeadZoneActor* pZone = CICHeadZoneActor::SafeDownCast((vtkObject*)sel.GetData()))
				{
					if (pZone->GetDefault())
					{
						::AfxMessageBox(_T("The default HEAD_IC zone cannot be deleted."));
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
					return;
				}
			}
		}
		return;
	}

	// CHEMISTRY_IC
	//
	if (sel.IsNodeAncestor(this->m_nodeICChem))
	{
		if (sel != this->m_nodeICChem)
		{
			while (sel.GetParent() != this->m_nodeICChem)
			{
				sel = sel.GetParent();
				if (!sel) break;
			}
			if (sel.GetData())
			{
				if (CICChemZoneActor* pZone = CICChemZoneActor::SafeDownCast((vtkObject*)sel.GetData()))
				{
					if (pZone->GetDefault())
					{
						::AfxMessageBox(_T("The default CHEMISTRY_IC zone cannot be deleted."));
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
					return;
				}
			}
		}
		return;
	}

	// TIME_CONTROL
	//
	if (sel.IsNodeAncestor(this->m_nodeTimeControl2))
	{
		::AfxMessageBox("TIME_CONTROL cannot be deleted.");
		return;
	}
}

void CPropertyTreeControlBar::OnUpdateEditProperties(CCmdUI *pCmdUI)
{
	if (CTreeCtrlEx *pTreeCtrlEx = this->GetTreeCtrlEx())
	{
		CTreeCtrlNode node = pTreeCtrlEx->GetSelectedItem();
		if (this->IsNodeEditable(node, false))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CPropertyTreeControlBar::OnEditProperties()
{
	if (CTreeCtrlEx *pTreeCtrlEx = this->GetTreeCtrlEx())
	{
		CTreeCtrlNode node = pTreeCtrlEx->GetSelectedItem();
		this->IsNodeEditable(node, true);
	}
}
