// FluxPropsPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "FluxPropsPage2.h"

#include "Global.h"


// CFluxPropsPage2 dialog

const TCHAR PSZ_FLUX[]     = _T("Flux");
const TCHAR PSZ_SOLUTION[] = _T("Solution");

const int SELECTED = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CFluxPropsPage2, CPropsPropertyPage)

CFluxPropsPage2::CFluxPropsPage2()
	: CPropsPropertyPage(CFluxPropsPage2::IDD)
	, FluxSeries(this, false, false)
	, SolutionSeries(this, false, true)
{
	TRACE("In %s\n", __FUNCTION__);

	// load property descriptions
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,   IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sAssocSolutionRTF, IDR_BC_FLUX_ASSOC_SOL_RTF);
	CGlobal::LoadRTFString(this->m_sFluxRTF,          IDR_BC_FLUX_FLUX_RTF);
	CGlobal::LoadRTFString(this->m_sFaceRTF,          IDR_BC_FLUX_FACE_RTF);

	// init properties
	this->SetFlowOnly(false);
	this->SetDefault(false);

	TRACE("Out %s\n", __FUNCTION__);
}

CFluxPropsPage2::~CFluxPropsPage2()
{
	TRACE("In %s\n", __FUNCTION__);
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);
	CPropsPropertyPage::DoDataExchange(pDX);

	// time series grid controls
	DDX_GridControl(pDX, IDC_GRID_FLUX,     this->FluxSeries.grid);
	DDX_GridControl(pDX, IDC_GRID_SOLUTION, this->SolutionSeries.grid);

	if (this->TreeCtrl.GetCount() == 0)
	{
		// setup tree
		this->FluxSeries.treeitem     = this->TreeCtrl.InsertItem(PSZ_FLUX,     TVI_ROOT, TVI_LAST);
		this->SolutionSeries.treeitem = this->TreeCtrl.InsertItem(PSZ_SOLUTION, TVI_ROOT, TVI_LAST);

		// setup tree selection
		this->ItemDDX = this->FluxSeries.treeitem;
		this->TreeCtrl.SelectItem(this->ItemDDX);
		this->TreeCtrl.ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);

		// initialize time series grids
		this->FluxSeries.InitializeGrid(pDX);
		this->SolutionSeries.InitializeGrid(pDX);

		// set points grid for each propety
		this->FluxSeries.SetPointsGrid(&this->PointsGrid);
		this->SolutionSeries.SetPointsGrid(&this->PointsGrid);
	}

	// description
	::DDX_Text(pDX, IDC_DESC_EDIT, this->Description);

	// face
	if (pDX->m_bSaveAndValidate)
	{
		if (this->IsDlgButtonChecked(IDC_CHECK_FACE))
		{
			if (this->IsDlgButtonChecked(IDC_FACE_X_RADIO))
			{
				this->BC.face_defined = TRUE;
				this->BC.face         = 0;
			}
			if (this->IsDlgButtonChecked(IDC_FACE_Y_RADIO))
			{
				this->BC.face_defined = TRUE;
				this->BC.face         = 1;
			}
			if (this->IsDlgButtonChecked(IDC_FACE_Z_RADIO))
			{
				this->BC.face_defined = TRUE;
				this->BC.face         = 2;
			}
		}
	}
	else
	{
		// face
		switch(this->BC.face)
		{
		case 0: // x
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_X_RADIO);
			break;
		case 1: // y
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_Y_RADIO);
			break;
		case 2: // z
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_Z_RADIO);
			break;
		default: // x
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_X_RADIO);
		}
		if (this->BC.face_defined)
		{
			this->CheckDlgButton(IDC_CHECK_FACE, BST_CHECKED);
		}
		else
		{
			this->CheckDlgButton(IDC_CHECK_FACE, BST_UNCHECKED);
		}
		this->OnBnClickedCheckFace();
	}

	// time series
	if (pDX->m_bSaveAndValidate)
	{
		this->DDV_SoftValidate();
	}
	this->DDX_Series(pDX);

// COMMENT: {7/13/2009 8:01:38 PM}	if (pDX->m_bSaveAndValidate)
// COMMENT: {7/13/2009 8:01:38 PM}	{
// COMMENT: {7/13/2009 8:01:38 PM}		if (this->ItemDDX == this->FluxSeries.treeitem)
// COMMENT: {7/13/2009 8:01:38 PM}		{
// COMMENT: {7/13/2009 8:01:38 PM}			this->FluxSeries.DDX_Series(pDX);
// COMMENT: {7/13/2009 8:01:38 PM}			ASSERT(!this->SolutionSeries.grid.IsWindowVisible());
// COMMENT: {7/13/2009 8:01:38 PM}			ASSERT(this->FluxSeries.grid.IsWindowVisible());
// COMMENT: {7/13/2009 8:01:38 PM}		}
// COMMENT: {7/13/2009 8:01:38 PM}		else if (this->ItemDDX == this->SolutionSeries.treeitem)
// COMMENT: {7/13/2009 8:01:38 PM}		{
// COMMENT: {7/13/2009 8:01:38 PM}			this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);
// COMMENT: {7/13/2009 8:01:38 PM}			ASSERT(this->SolutionSeries.grid.IsWindowVisible());
// COMMENT: {7/13/2009 8:01:38 PM}			ASSERT(!this->FluxSeries.grid.IsWindowVisible());
// COMMENT: {7/13/2009 8:01:38 PM}		}
// COMMENT: {7/13/2009 8:01:38 PM}	}
// COMMENT: {7/13/2009 8:01:38 PM}	else
// COMMENT: {7/13/2009 8:01:38 PM}	{
// COMMENT: {7/13/2009 8:01:38 PM}		if (this->ItemDDX == this->FluxSeries.treeitem)
// COMMENT: {7/13/2009 8:01:38 PM}		{
// COMMENT: {7/13/2009 8:01:38 PM}			this->FluxSeries.DDX_Series(pDX);
// COMMENT: {7/13/2009 8:01:38 PM}			this->SolutionSeries.grid.ShowWindow(SW_HIDE);
// COMMENT: {7/13/2009 8:01:38 PM}			this->FluxSeries.grid.ShowWindow(SW_SHOW);
// COMMENT: {7/13/2009 8:01:38 PM}		}
// COMMENT: {7/13/2009 8:01:38 PM}		else if (this->ItemDDX == this->SolutionSeries.treeitem)
// COMMENT: {7/13/2009 8:01:38 PM}		{
// COMMENT: {7/13/2009 8:01:38 PM}			this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);
// COMMENT: {7/13/2009 8:01:38 PM}			this->FluxSeries.grid.ShowWindow(SW_HIDE);
// COMMENT: {7/13/2009 8:01:38 PM}			this->SolutionSeries.grid.ShowWindow(SW_SHOW);
// COMMENT: {7/13/2009 8:01:38 PM}		}
// COMMENT: {7/13/2009 8:01:38 PM}	}

	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::DDV_SoftValidate()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->FluxSeries.treeitem)
		{
			this->FluxSeries.DDV_SoftValidate();
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.DDV_SoftValidate();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

void CFluxPropsPage2::DDX_Series(CDataExchange* pDX)
{
	if (this->ItemDDX)
	{
		if (pDX->m_bSaveAndValidate)
		{
			if (this->ItemDDX == this->FluxSeries.treeitem)
			{
				this->FluxSeries.DDX_Series(pDX);
				ASSERT(!this->SolutionSeries.grid.IsWindowVisible());
				ASSERT(this->FluxSeries.grid.IsWindowVisible());
			}
			else if (this->ItemDDX == this->SolutionSeries.treeitem)
			{
				this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);
				ASSERT(this->SolutionSeries.grid.IsWindowVisible());
				ASSERT(!this->FluxSeries.grid.IsWindowVisible());
			}
		}
		else
		{
			if (this->ItemDDX == this->FluxSeries.treeitem)
			{
				this->FluxSeries.DDX_Series(pDX);
				this->SolutionSeries.grid.ShowWindow(SW_HIDE);
				this->FluxSeries.grid.ShowWindow(SW_SHOW);
			}
			else if (this->ItemDDX == this->SolutionSeries.treeitem)
			{
				this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);
				this->FluxSeries.grid.ShowWindow(SW_HIDE);
				this->SolutionSeries.grid.ShowWindow(SW_SHOW);
			}
		}
	}
}

void CFluxPropsPage2::SetProperties(const CBC& rBC)
{
	TRACE("In %s\n", __FUNCTION__);

	// copy grid_elt
	this->BC = rBC;

	// time series
	this->FluxSeries.SetSeries(rBC.m_bc_flux);
	this->SolutionSeries.SetSeries(rBC.m_bc_solution);

	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::GetProperties(CBC& rBC)const
{
	TRACE("In %s\n", __FUNCTION__);

	rBC = this->BC;

	// series
	this->FluxSeries.GetSeries(rBC.m_bc_flux);
	this->SolutionSeries.GetSeries(rBC.m_bc_solution);

	TRACE("Out %s\n", __FUNCTION__);
}

BEGIN_MESSAGE_MAP(CFluxPropsPage2, CPropsPropertyPage)
	// IDC_GRID_FLUX
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_FLUX, OnEndLabelEditFlux)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_FLUX, OnSelChangedFlux)

	// IDC_GRID_SOLUTION
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_SOLUTION, OnSelChangedSolution)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_SOLUTION, OnEndLabelEditSolution)

// COMMENT: {7/13/2009 7:26:43 PM}	// IDC_PROP_TREE
// COMMENT: {7/13/2009 7:26:43 PM}	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTreeSelChanging)
// COMMENT: {7/13/2009 7:26:43 PM}	ON_NOTIFY(TVN_SELCHANGED,  IDC_PROP_TREE, OnTreeSelChanged)

	// IDC_DESC_EDIT
	ON_EN_SETFOCUS(IDC_DESC_EDIT, OnEnSetfocusDescEdit)

	// IDC_BUTTON_XYZ
	ON_BN_CLICKED(IDC_BUTTON_XYZ, OnBnClickedButtonXYZ)

	//// IDC_COMBO_PROPTYPE
	//ON_CBN_SELCHANGE(IDC_COMBO_PROPTYPE, OnCbnSelchangeComboProptype)

	// IDC_CHECK_MIXTURE
	ON_BN_CLICKED(IDC_CHECK_MIXTURE, OnBnClickedCheckMixture)

	// IDC_CHECK_FACE
	ON_BN_CLICKED(IDC_CHECK_FACE, OnBnClickedCheckFace)

	// DDX failure
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
END_MESSAGE_MAP()


// CFluxPropsPage2 message handlers

void CFluxPropsPage2::OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->FluxSeries.OnEndLabelEdit(pnmgv->iRow, pnmgv->iColumn);
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->SolutionSeries.OnEndLabelEdit(pnmgv->iRow, pnmgv->iColumn);
	TRACE("Out %s\n", __FUNCTION__);
}

// COMMENT: {7/13/2009 7:25:54 PM}void CFluxPropsPage2::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
// COMMENT: {7/13/2009 7:25:54 PM}{
// COMMENT: {7/13/2009 7:25:54 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:25:54 PM}	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
// COMMENT: {7/13/2009 7:25:54 PM}	this->ItemDDX = pTvn->itemOld.hItem;
// COMMENT: {7/13/2009 7:25:54 PM}	if (this->ItemDDX)
// COMMENT: {7/13/2009 7:25:54 PM}	{
// COMMENT: {7/13/2009 7:25:54 PM}		this->DDV_SoftValidate();
// COMMENT: {7/13/2009 7:25:54 PM}// COMMENT: {7/13/2009 7:25:02 PM}		if (this->ItemDDX == this->FluxSeries.treeitem)
// COMMENT: {7/13/2009 7:25:54 PM}// COMMENT: {7/13/2009 7:25:02 PM}		{
// COMMENT: {7/13/2009 7:25:54 PM}// COMMENT: {7/13/2009 7:25:02 PM}			this->FluxSeries.DDV_SoftValidate();
// COMMENT: {7/13/2009 7:25:54 PM}// COMMENT: {7/13/2009 7:25:02 PM}		}
// COMMENT: {7/13/2009 7:25:54 PM}// COMMENT: {7/13/2009 7:25:02 PM}		else if (this->ItemDDX == this->SolutionSeries.treeitem)
// COMMENT: {7/13/2009 7:25:54 PM}// COMMENT: {7/13/2009 7:25:02 PM}		{
// COMMENT: {7/13/2009 7:25:54 PM}// COMMENT: {7/13/2009 7:25:02 PM}			this->SolutionSeries.DDV_SoftValidate();
// COMMENT: {7/13/2009 7:25:54 PM}// COMMENT: {7/13/2009 7:25:02 PM}		}
// COMMENT: {7/13/2009 7:25:54 PM}
// COMMENT: {7/13/2009 7:25:54 PM}		//{{{6/26/2009 5:12:11 PM}
// COMMENT: {7/13/2009 7:25:54 PM}		// force CInPlaceXXX to lose focus
// COMMENT: {7/13/2009 7:25:54 PM}		this->TreeCtrl.SetFocus();
// COMMENT: {7/13/2009 7:25:54 PM}		//}}{6/26/2009 5:12:11 PM}
// COMMENT: {7/13/2009 7:25:54 PM}
// COMMENT: {7/13/2009 7:25:54 PM}		if (!this->UpdateData(TRUE))
// COMMENT: {7/13/2009 7:25:54 PM}		{
// COMMENT: {7/13/2009 7:25:54 PM}			// notify which control caused failure
// COMMENT: {7/13/2009 7:25:54 PM}			//
// COMMENT: {7/13/2009 7:25:54 PM}			CWnd* pFocus = CWnd::GetFocus();
// COMMENT: {7/13/2009 7:25:54 PM}			this->PostMessage(UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);
// COMMENT: {7/13/2009 7:25:54 PM}
// COMMENT: {7/13/2009 7:25:54 PM}			// disallow change
// COMMENT: {7/13/2009 7:25:54 PM}			//
// COMMENT: {7/13/2009 7:25:54 PM}			*pResult = TRUE;
// COMMENT: {7/13/2009 7:25:54 PM}			TRACE("Out %s Disallowed\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:25:54 PM}			return;
// COMMENT: {7/13/2009 7:25:54 PM}		}
// COMMENT: {7/13/2009 7:25:54 PM}	}
// COMMENT: {7/13/2009 7:25:54 PM}	*pResult = 0;
// COMMENT: {7/13/2009 7:25:54 PM}	TRACE("Out Allowed %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:25:54 PM}}

#define COMPARE_SET(S, R) \
do { \
	if (strItem.Compare(S) == 0) { \
		this->RichEditCtrl.SetWindowText(R.c_str()); \
	} \
} while (0)

// COMMENT: {7/13/2009 7:30:01 PM}void CFluxPropsPage2::OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
// COMMENT: {7/13/2009 7:30:01 PM}{
// COMMENT: {7/13/2009 7:30:01 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:30:01 PM}	UNREFERENCED_PARAMETER(pResult);
// COMMENT: {7/13/2009 7:30:01 PM}	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
// COMMENT: {7/13/2009 7:30:01 PM}	this->ItemDDX = pTvn->itemNew.hItem;
// COMMENT: {7/13/2009 7:30:01 PM}	if (this->ItemDDX)
// COMMENT: {7/13/2009 7:30:01 PM}	{
// COMMENT: {7/13/2009 7:30:01 PM}		this->UpdateData(FALSE);
// COMMENT: {7/13/2009 7:30:01 PM}
// COMMENT: {7/13/2009 7:30:01 PM}		// update property description
// COMMENT: {7/13/2009 7:30:01 PM}		//
// COMMENT: {7/13/2009 7:30:01 PM}		if (this->TreeCtrl.GetSafeHwnd())
// COMMENT: {7/13/2009 7:30:01 PM}		{
// COMMENT: {7/13/2009 7:30:01 PM}			CString strItem = this->TreeCtrl.GetItemText(this->ItemDDX);
// COMMENT: {7/13/2009 7:30:01 PM}
// COMMENT: {7/13/2009 7:30:01 PM}			COMPARE_SET(PSZ_FLUX,     this->m_sFluxRTF);
// COMMENT: {7/13/2009 7:30:01 PM}			COMPARE_SET(PSZ_SOLUTION, this->m_sAssocSolutionRTF);
// COMMENT: {7/13/2009 7:30:01 PM}		}
// COMMENT: {7/13/2009 7:30:01 PM}	}
// COMMENT: {7/13/2009 7:30:01 PM}	if (this->TreeCtrl.GetSafeHwnd())
// COMMENT: {7/13/2009 7:30:01 PM}	{
// COMMENT: {7/13/2009 7:30:01 PM}		this->TreeCtrl.SetFocus();
// COMMENT: {7/13/2009 7:30:01 PM}	}
// COMMENT: {7/13/2009 7:30:01 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:30:01 PM}}

void CFluxPropsPage2::SetPropertyDescription()
{
	if (this->ItemDDX)
	{
		CString strItem = this->TreeCtrl.GetItemText(this->ItemDDX);

		COMPARE_SET(PSZ_FLUX,     this->m_sFluxRTF);
		COMPARE_SET(PSZ_SOLUTION, this->m_sAssocSolutionRTF);
	}
}

void CFluxPropsPage2::OnEnSetfocusDescEdit()
{
	TRACE("In %s\n", __FUNCTION__);
	this->RichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnSelChangedFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->FluxSeries.OnSelChanged(pnmgv->iRow, pnmgv->iColumn);

	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->SolutionSeries.OnSelChanged(pnmgv->iRow, pnmgv->iColumn);

	TRACE("Out %s\n", __FUNCTION__);
}

LRESULT CFluxPropsPage2::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	CWnd* pFocus = (CWnd*)wParam;
	if (pFocus && pFocus->GetSafeHwnd())
	{
		ASSERT_KINDOF(CWnd, pFocus);
		pFocus->SetFocus();
	}
	return 0;
}

void CFluxPropsPage2::OnBnClickedButtonXYZ()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->FluxSeries.treeitem)
		{
			this->FluxSeries.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.OnBnClickedButtonXYZ();
		}
	}
}

void CFluxPropsPage2::SetUnits(const CUnits &u)
{
	this->FluxSeries.SetUnits(u);
	this->SolutionSeries.SetUnits(u);
}

void CFluxPropsPage2::OnBnClickedCheckFace()
{
	if (this->IsDlgButtonChecked(IDC_CHECK_FACE))
	{
		if (CWnd *pWnd = this->GetDlgItem(IDC_FACE_X_RADIO))
		{
			pWnd->EnableWindow(TRUE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_FACE_Y_RADIO))
		{
			pWnd->EnableWindow(TRUE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_FACE_Z_RADIO))
		{
			pWnd->EnableWindow(TRUE);
		}
	}
	else
	{
		if (CWnd *pWnd = this->GetDlgItem(IDC_FACE_X_RADIO))
		{
			pWnd->EnableWindow(FALSE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_FACE_Y_RADIO))
		{
			pWnd->EnableWindow(FALSE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_FACE_Z_RADIO))
		{
			pWnd->EnableWindow(FALSE);
		}
	}
}

void CFluxPropsPage2::OnBnClickedCheckMixture()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->FluxSeries.treeitem)
		{
			this->FluxSeries.OnBnClickedCheckMixture();
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.OnBnClickedCheckMixture();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}
