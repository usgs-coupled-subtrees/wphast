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
	, ThicknessProperty(this, true, false)
	, HydCondProperty(this, true, false)
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

	// time series grid controls
	DDX_GridControl(pDX, IDC_GRID_HEAD,     this->HeadSeries.grid);
	DDX_GridControl(pDX, IDC_GRID_SOLUTION, this->SolutionSeries.grid);

	if (this->TreeCtrl.GetCount() == 0)
	{
		// setup tree
		this->HeadSeries.treeitem      = this->TreeCtrl.InsertItem(PSZ_HEAD,      TVI_ROOT, TVI_LAST);
		this->SolutionSeries.treeitem  = this->TreeCtrl.InsertItem(PSZ_SOLUTION,  TVI_ROOT, TVI_LAST);
		this->ThicknessProperty.treeitem = this->TreeCtrl.InsertItem(PSZ_THICKNESS, TVI_ROOT, TVI_LAST);
		this->HydCondProperty.treeitem   = this->TreeCtrl.InsertItem(PSZ_HYD_COND,  TVI_ROOT, TVI_LAST);

		// setup tree selection
		this->ItemDDX = this->HeadSeries.treeitem;
		this->TreeCtrl.SelectItem(this->ItemDDX);
		this->TreeCtrl.ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);

		// initialize time series grids
		this->HeadSeries.InitializeGrid(pDX);
		this->SolutionSeries.InitializeGrid(pDX);

		// set points grid for each propety
		this->HeadSeries.SetPointsGrid(&this->PointsGrid);
		this->SolutionSeries.SetPointsGrid(&this->PointsGrid);
		this->ThicknessProperty.SetPointsGrid(&this->PointsGrid);
		this->HydCondProperty.SetPointsGrid(&this->PointsGrid);
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

	// properties
	if (pDX->m_bSaveAndValidate)
	{
		this->DDV_SoftValidate();
	}
	this->DDX_Series(pDX);
	this->DDX_Single(pDX);

	TRACE("Out %s\n", __FUNCTION__);
}

void CLeakyPropsPage::DDV_SoftValidate()
{
	// description
	CDataExchange dx(this, TRUE);
	::DDX_Text(&dx, IDC_DESC_EDIT, this->Description);

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

	if (this->ItemDDX)
	{
		// time series
		if (this->ItemDDX == this->HeadSeries.treeitem)
		{
			this->HeadSeries.DDV_SoftValidate();
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.DDV_SoftValidate();
		}
		// properties
		else if (this->ItemDDX == this->ThicknessProperty.treeitem)
		{
			this->ThicknessProperty.DDV_SoftValidate();
		}
		else if (this->ItemDDX == this->HydCondProperty.treeitem)
		{
			this->HydCondProperty.DDV_SoftValidate();
		}
	}
}

void CLeakyPropsPage::DDX_Series(CDataExchange* pDX)
{
	if (this->ItemDDX)
	{
		if (pDX->m_bSaveAndValidate)
		{
			// time series
			if (this->ItemDDX == this->HeadSeries.treeitem)
			{
				this->HeadSeries.DDX_Series(pDX);
				ASSERT(!this->SolutionSeries.grid.IsWindowVisible());
				ASSERT(this->HeadSeries.grid.IsWindowVisible());
			}
			else if (this->ItemDDX == this->SolutionSeries.treeitem)
			{
				this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);
				ASSERT(this->SolutionSeries.grid.IsWindowVisible());
				ASSERT(!this->HeadSeries.grid.IsWindowVisible());
			}
		}
		else
		{
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
		}
	}
}

void CLeakyPropsPage::DDX_Single(CDataExchange* pDX)
{
	if (this->ItemDDX)
	{
		// properties
		if (this->ItemDDX == this->ThicknessProperty.treeitem)
		{
			this->ThicknessProperty.DDX_Single(pDX);

			this->HeadSeries.grid.ShowWindow(SW_HIDE);
			this->SolutionSeries.grid.ShowWindow(SW_HIDE);
			if (!pDX->m_bSaveAndValidate)
			{
				CGridTimeSeries::ShowSingleProperty(this, SW_SHOW);
			}
		}
		else if (this->ItemDDX == this->HydCondProperty.treeitem)
		{
			this->HydCondProperty.DDX_Single(pDX);

			this->HeadSeries.grid.ShowWindow(SW_HIDE);
			this->SolutionSeries.grid.ShowWindow(SW_HIDE);
			if (!pDX->m_bSaveAndValidate)
			{
				CGridTimeSeries::ShowSingleProperty(this, SW_SHOW);
			}
		}
	}
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
	this->ThicknessProperty.SetProperty(rBC.bc_thick);
	this->HydCondProperty.SetProperty(rBC.bc_k);

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
	this->ThicknessProperty.GetProperty(rBC.bc_thick);
	ASSERT(rBC.bc_thick);
	this->HydCondProperty.GetProperty(rBC.bc_k);
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
	ON_BN_SETFOCUS(IDC_CHECK_FACE, OnBnSetfocusCheckFace)
	ON_BN_SETFOCUS(IDC_FACE_X_RADIO, OnBnSetfocusCheckFace)
	ON_BN_SETFOCUS(IDC_FACE_Y_RADIO, OnBnSetfocusCheckFace)
	ON_BN_SETFOCUS(IDC_FACE_Z_RADIO, OnBnSetfocusCheckFace)

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

#define COMPARE_SET(S, R) \
do { \
	if (strItem.Compare(S) == 0) { \
		this->RichEditCtrl.SetWindowText(R.c_str()); \
	} \
} while (0)

void CLeakyPropsPage::SetPropertyDescription()
{
	if (this->ItemDDX)
	{
		CString strItem = this->TreeCtrl.GetItemText(this->ItemDDX);

		COMPARE_SET(PSZ_HEAD,     this->m_sHeadRTF);
		COMPARE_SET(PSZ_SOLUTION, this->m_sAssocSolutionRTF);
	}
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
		else if (this->ItemDDX == this->ThicknessProperty.treeitem)
		{
			this->ThicknessProperty.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->HydCondProperty.treeitem)
		{
			this->HydCondProperty.OnBnClickedButtonXYZ();
		}
	}
}

void CLeakyPropsPage::SetUnits(const CUnits &u)
{
	this->HeadSeries.SetUnits(u);
	this->SolutionSeries.SetUnits(u);
	this->ThicknessProperty.SetUnits(u);
	this->HydCondProperty.SetUnits(u);
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
		else if (this->ItemDDX == this->ThicknessProperty.treeitem)
		{
			this->ThicknessProperty.OnCbnSelchangeComboProptype();
		}
		else if (this->ItemDDX == this->HydCondProperty.treeitem)
		{
			this->HydCondProperty.OnCbnSelchangeComboProptype();
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
	this->OnBnSetfocusCheckFace();
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
		else if (this->ItemDDX == this->ThicknessProperty.treeitem)
		{
			this->ThicknessProperty.OnBnClickedCheckMixture();
		}
		else if (this->ItemDDX == this->HydCondProperty.treeitem)
		{
			this->HydCondProperty.OnBnClickedCheckMixture();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

void CLeakyPropsPage::OnBnSetfocusCheckFace()
{
	this->RichEditCtrl.SetWindowText(this->m_sFaceRTF.c_str());
}
