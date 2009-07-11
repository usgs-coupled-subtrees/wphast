// LeakyPropsPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "LeakyPropsPage.h"

#include "Global.h"


// CLeakyPropsPage dialog

const TCHAR PSZ_THICKNESS[] = _T("Thickness");
const TCHAR PSZ_HYD_COND[]  = _T("Hydraulic conductivity");
const TCHAR PSZ_HEAD[]      = _T("Head");
const TCHAR PSZ_SOLUTION[]  = _T("Solution");

const int SELECTED = 0;
const int SINGLE = 0;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CLeakyPropsPage, CPropsPropertyPage)

CLeakyPropsPage::CLeakyPropsPage()
	: CPropsPropertyPage(CLeakyPropsPage::IDD)
	, HeadSeries(this, false, false)
	, SolutionSeries(this, false, true)
	, ThicknessSeries(this, true, false)
	, HydCondSeries(this, true, false)
{
	TRACE("In %s\n", __FUNCTION__);

	// load property descriptions
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,   IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sHeadRTF,          IDR_BC_LEAKY_HEAD_RTF);
	CGlobal::LoadRTFString(this->m_sThicknessRTF,     IDR_BC_LEAKY_THICKNESS_RTF);
	CGlobal::LoadRTFString(this->m_sHydCondRTF,       IDR_BC_LEAKY_HYD_COND_RTF);
	CGlobal::LoadRTFString(this->m_sAssocSolutionRTF, IDR_BC_LEAKY_ASSOC_SOL_RTF);
	CGlobal::LoadRTFString(this->m_sFaceRTF,          IDR_BC_LEAKY_FACE_RTF);

	// init properties
	this->SetFlowOnly(false);
	this->SetDefault(false);

	TRACE("Out %s\n", __FUNCTION__);
}

CLeakyPropsPage::~CLeakyPropsPage()
{
	TRACE("In %s\n", __FUNCTION__);
	TRACE("Out %s\n", __FUNCTION__);
}

void CLeakyPropsPage::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);
	CPropsPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	DDX_Control(pDX, IDC_PROP_TREE, this->TreeCtrl);

	if (this->TreeCtrl.GetCount() == 0)
	{
		this->HeadSeries.treeitem      = this->TreeCtrl.InsertItem(PSZ_HEAD,      TVI_ROOT, TVI_LAST);
		this->SolutionSeries.treeitem  = this->TreeCtrl.InsertItem(PSZ_SOLUTION,  TVI_ROOT, TVI_LAST);
		this->ThicknessSeries.treeitem = this->TreeCtrl.InsertItem(PSZ_THICKNESS, TVI_ROOT, TVI_LAST);
		this->HydCondSeries.treeitem   = this->TreeCtrl.InsertItem(PSZ_HYD_COND,  TVI_ROOT, TVI_LAST);

		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);

		this->ItemDDX = this->HeadSeries.treeitem;
		this->TreeCtrl.SelectItem(this->ItemDDX);

		this->TreeCtrl.ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);
	}

	DDX_GridControl(pDX, IDC_POINTS_GRID,   this->PointsGrid);
	DDX_GridControl(pDX, IDC_GRID_HEAD,     this->HeadSeries.grid);
	DDX_GridControl(pDX, IDC_GRID_SOLUTION, this->SolutionSeries.grid);

	this->HeadSeries.InitializeGrid(pDX);
	this->SolutionSeries.InitializeGrid(pDX);

	this->HeadSeries.SetPointsGrid(&this->PointsGrid);
	this->SolutionSeries.SetPointsGrid(&this->PointsGrid);
	this->ThicknessSeries.SetPointsGrid(&this->PointsGrid);
	this->HydCondSeries.SetPointsGrid(&this->PointsGrid);

	// description
	::DDX_Text(pDX, IDC_DESC_EDIT, this->Description);

	if (pDX->m_bSaveAndValidate)
	{
		// face
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

		// time series
		if (this->ItemDDX == this->HeadSeries.treeitem)
		{
			this->HeadSeries.DDV_SoftValidate();
			this->HeadSeries.DDX_Series(pDX);
			ASSERT(!this->SolutionSeries.grid.IsWindowVisible());
			ASSERT(this->HeadSeries.grid.IsWindowVisible());
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.DDV_SoftValidate();
			this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);
			ASSERT(this->SolutionSeries.grid.IsWindowVisible());
			ASSERT(!this->HeadSeries.grid.IsWindowVisible());
		}
		else if (this->ItemDDX == this->ThicknessSeries.treeitem)
		{
			this->ThicknessSeries.DDV_SoftValidate();
			this->ThicknessSeries.DDX_Single(pDX, true);
			ASSERT(!this->SolutionSeries.grid.IsWindowVisible());
			ASSERT(!this->HeadSeries.grid.IsWindowVisible());
		}
		else if (this->ItemDDX == this->HydCondSeries.treeitem)
		{
			this->HydCondSeries.DDV_SoftValidate();
			this->HydCondSeries.DDX_Single(pDX, true);
			ASSERT(!this->SolutionSeries.grid.IsWindowVisible());
			ASSERT(!this->HeadSeries.grid.IsWindowVisible());
		}

#ifdef _DEBUG
		for (int nCol = 0; nCol < this->HeadSeries.grid.GetColumnCount(); ++nCol)
		{
			afxDump << "Column " << nCol << " = " << this->HeadSeries.grid.GetColumnWidth(nCol) << "\n";
		}
#endif

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

		// time series
		if (this->ItemDDX == this->HeadSeries.treeitem)
		{
			this->HeadSeries.DDX_Series(pDX);

			this->SolutionSeries.grid.ShowWindow(SW_HIDE);
			CGridTimeSeries::ShowSingleProperty(this, SW_HIDE);
			this->HeadSeries.grid.ShowWindow(SW_SHOW);
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);

			this->HeadSeries.grid.ShowWindow(SW_HIDE);
			CGridTimeSeries::ShowSingleProperty(this, SW_HIDE);
			this->SolutionSeries.grid.ShowWindow(SW_SHOW);
		}
		else if (this->ItemDDX == this->ThicknessSeries.treeitem)
		{
			this->ThicknessSeries.DDX_Single(pDX);

			this->HeadSeries.grid.ShowWindow(SW_HIDE);
			this->SolutionSeries.grid.ShowWindow(SW_HIDE);
			CGridTimeSeries::ShowSingleProperty(this, SW_SHOW);
		}
		else if (this->ItemDDX == this->HydCondSeries.treeitem)
		{
			this->HydCondSeries.DDX_Single(pDX);

			this->HeadSeries.grid.ShowWindow(SW_HIDE);
			this->SolutionSeries.grid.ShowWindow(SW_HIDE);
			CGridTimeSeries::ShowSingleProperty(this, SW_SHOW);
		}

	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CLeakyPropsPage::SetProperties(const CBC& rBC)
{
	TRACE("In %s\n", __FUNCTION__);

	// copy grid_elt
	this->BC = rBC;

	// time series
	this->HeadSeries.SetSeries(rBC.m_bc_head);
	this->SolutionSeries.SetSeries(rBC.m_bc_solution);
	// single properties
	this->ThicknessSeries.SetProperty(rBC.bc_thick);
	this->HydCondSeries.SetProperty(rBC.bc_k);

	TRACE("Out %s\n", __FUNCTION__);
}

void CLeakyPropsPage::GetProperties(CBC& rBC)const
{
	TRACE("In %s\n", __FUNCTION__);

	rBC = this->BC;

	// series
	this->HeadSeries.GetSeries(rBC.m_bc_head);
	this->SolutionSeries.GetSeries(rBC.m_bc_solution);

	// single properties
	this->ThicknessSeries.GetProperty(rBC.bc_thick);
	ASSERT(rBC.bc_thick);
	this->HydCondSeries.GetProperty(rBC.bc_k);
	ASSERT(rBC.bc_k);

	TRACE("Out %s\n", __FUNCTION__);
}

BEGIN_MESSAGE_MAP(CLeakyPropsPage, CPropsPropertyPage)
	// IDC_GRID_HEAD
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_HEAD, OnEndLabelEditFlux)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_HEAD,    OnSelChangedFlux)

	// IDC_GRID_SOLUTION
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_SOLUTION, OnEndLabelEditSolution)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_SOLUTION, OnSelChangedSolution)

	// IDC_PROP_TREE
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGED,  IDC_PROP_TREE, OnTreeSelChanged)

	// IDC_DESC_EDIT
	ON_EN_SETFOCUS(IDC_DESC_EDIT, OnEnSetfocusDescEdit)

	// IDC_BUTTON_XYZ
	ON_BN_CLICKED(IDC_BUTTON_XYZ, OnBnClickedButtonXYZ)

	// IDC_COMBO_PROPTYPE
	ON_CBN_SELCHANGE(IDC_COMBO_PROPTYPE, OnCbnSelchangeComboProptype)

	// IDC_CHECK_MIXTURE
	ON_BN_CLICKED(IDC_CHECK_MIXTURE, OnBnClickedCheckMixture)

	// IDC_CHECK_FACE
	ON_BN_CLICKED(IDC_CHECK_FACE, OnBnClickedCheckFace)

	// DDX failure
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
END_MESSAGE_MAP()


// CLeakyPropsPage message handlers

void CLeakyPropsPage::OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->HeadSeries.OnEndLabelEdit(pnmgv->iRow, pnmgv->iColumn);
	TRACE("Out %s\n", __FUNCTION__);
}

void CLeakyPropsPage::OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->SolutionSeries.OnEndLabelEdit(pnmgv->iRow, pnmgv->iColumn);
	TRACE("Out %s\n", __FUNCTION__);
}

void CLeakyPropsPage::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemOld.hItem;
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->HeadSeries.treeitem)
		{
			this->HeadSeries.DDV_SoftValidate();
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.DDV_SoftValidate();
		}

		//{{{6/26/2009 5:12:11 PM}
		// force CInPlaceXXX to lose focus
		this->TreeCtrl.SetFocus();
		//}}{6/26/2009 5:12:11 PM}

		if (!this->UpdateData(TRUE))
		{
			// notify which control caused failure
			//
			CWnd* pFocus = CWnd::GetFocus();
			this->PostMessage(UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

			// disallow change
			//
			*pResult = TRUE;
			TRACE("Out %s Disallowed\n", __FUNCTION__);
			return;
		}
	}
	*pResult = 0;
	TRACE("Out Allowed %s\n", __FUNCTION__);
}

#define COMPARE_SET(S, R) \
do { \
	if (strItem.Compare(S) == 0) { \
		this->RichEditCtrl.SetWindowText(R.c_str()); \
	} \
} while (0)

void CLeakyPropsPage::OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	UNREFERENCED_PARAMETER(pResult);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemNew.hItem;
	if (this->ItemDDX)
	{
		this->UpdateData(FALSE);

		// update property description
		//
		if (this->TreeCtrl.GetSafeHwnd())
		{
			CString strItem = this->TreeCtrl.GetItemText(this->ItemDDX);

			COMPARE_SET(PSZ_HEAD,     this->m_sHeadRTF);
			COMPARE_SET(PSZ_SOLUTION, this->m_sAssocSolutionRTF);
		}
	}
	if (this->TreeCtrl.GetSafeHwnd())
	{
		this->TreeCtrl.SetFocus();
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CLeakyPropsPage::OnEnSetfocusDescEdit()
{
	TRACE("In %s\n", __FUNCTION__);
	this->RichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
	TRACE("Out %s\n", __FUNCTION__);
}

void CLeakyPropsPage::OnSelChangedFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->HeadSeries.OnSelChanged(pnmgv->iRow, pnmgv->iColumn);

	TRACE("Out %s\n", __FUNCTION__);
}

void CLeakyPropsPage::OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->SolutionSeries.OnSelChanged(pnmgv->iRow, pnmgv->iColumn);

	TRACE("Out %s\n", __FUNCTION__);
}

LRESULT CLeakyPropsPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	CWnd* pFocus = (CWnd*)wParam;
	if (pFocus && pFocus->GetSafeHwnd())
	{
		ASSERT_KINDOF(CWnd, pFocus);
		pFocus->SetFocus();
	}
	return 0;
}

void CLeakyPropsPage::OnBnClickedButtonXYZ()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->HeadSeries.treeitem)
		{
			this->HeadSeries.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->ThicknessSeries.treeitem)
		{
			this->ThicknessSeries.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->HydCondSeries.treeitem)
		{
			this->HydCondSeries.OnBnClickedButtonXYZ();
		}
	}
}

void CLeakyPropsPage::SetUnits(const CUnits &u)
{
	this->HeadSeries.SetUnits(u);
	this->SolutionSeries.SetUnits(u);
	this->ThicknessSeries.SetUnits(u);
	this->HydCondSeries.SetUnits(u);
}

void CLeakyPropsPage::OnCbnSelchangeComboProptype()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->HeadSeries.treeitem)
		{
			ASSERT(FALSE);
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			ASSERT(FALSE);
		}
		else if (this->ItemDDX == this->ThicknessSeries.treeitem)
		{
			this->ThicknessSeries.OnCbnSelchangeComboProptype();
		}
		else if (this->ItemDDX == this->HydCondSeries.treeitem)
		{
			this->HydCondSeries.OnCbnSelchangeComboProptype();
		}
	}
}

void CLeakyPropsPage::OnBnClickedCheckFace()
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

void CLeakyPropsPage::OnBnClickedCheckMixture()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->HeadSeries.treeitem)
		{
			this->HeadSeries.OnBnClickedCheckMixture();
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.OnBnClickedCheckMixture();
		}
		else if (this->ItemDDX == this->ThicknessSeries.treeitem)
		{
			this->ThicknessSeries.OnBnClickedCheckMixture();
		}
		else if (this->ItemDDX == this->HydCondSeries.treeitem)
		{
			this->HydCondSeries.OnBnClickedCheckMixture();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}
