// SpecifiedHeadPropsPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "SpecifiedHeadPropsPage.h"

#include "Global.h"


// CSpecifiedHeadPropsPage dialog

const TCHAR PSZ_HEAD[]     = _T("Head");
const TCHAR PSZ_SOLUTION[] = _T("Solution");

const int SELECTED = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CSpecifiedHeadPropsPage, CPropsPropertyPage)

CSpecifiedHeadPropsPage::CSpecifiedHeadPropsPage()
	: CPropsPropertyPage(CSpecifiedHeadPropsPage::IDD)
	, HeadSeries(this, false, false)
	, SolutionSeries(this, false, true)
{
	TRACE("In %s\n", __FUNCTION__);

	// load property descriptions
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,   IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sHeadRTF,          IDR_BC_SPECIFIED_HEAD_RTF);
	CGlobal::LoadRTFString(this->m_sSolutionAssocRTF, IDR_BC_SPECIFIED_SOL_ASSOC_RTF);
	CGlobal::LoadRTFString(this->m_sSolutionFixRTF,   IDR_BC_SPECIFIED_SOL_FIX_RTF);	
	CGlobal::LoadRTFString(this->m_sSolTypeRTF,       IDR_BC_SOL_TYPE_RTF);
	CGlobal::LoadRTFString(this->m_sExtCellsRTF,      IDR_BC_SPECIFIED_EXT_CELLS_RTF);

	// init properties
	this->SetFlowOnly(false);
	this->SetDefault(false);

	// init data
	this->BC.bc_type = BC_info::BC_SPECIFIED;

	TRACE("Out %s\n", __FUNCTION__);
}

CSpecifiedHeadPropsPage::~CSpecifiedHeadPropsPage()
{
	TRACE("In %s\n", __FUNCTION__);
	TRACE("Out %s\n", __FUNCTION__);
}

void CSpecifiedHeadPropsPage::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s %s\n", __FUNCTION__, pDX->m_bSaveAndValidate ? _T("TRUE") : _T("FALSE"));
	CPropsPropertyPage::DoDataExchange(pDX);

	// time series grid controls
	DDX_GridControl(pDX, IDC_GRID_HEAD,     this->HeadSeries.grid);
	DDX_GridControl(pDX, IDC_GRID_SOLUTION, this->SolutionSeries.grid);

	if (this->TreeCtrl.GetCount() == 0)
	{
		// setup tree
		this->HeadSeries.treeitem     = this->TreeCtrl.InsertItem(PSZ_HEAD,     TVI_ROOT, TVI_LAST);
		this->SolutionSeries.treeitem = this->TreeCtrl.InsertItem(PSZ_SOLUTION, TVI_ROOT, TVI_LAST);

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
	}

	// description
	::DDX_Text(pDX, IDC_DESC_EDIT, this->Description);

	// exterior cells only
	if (pDX->m_bSaveAndValidate)
	{
		if (this->IsDlgButtonChecked(IDC_CHECK_EXTERIOR_ONLY) == BST_CHECKED)
		{
			switch (this->GetCheckedRadioButton(IDC_RADIO_EC_ALL, IDC_RADIO_EC_Z))
			{
			case IDC_RADIO_EC_ALL:
				this->BC.face_defined = TRUE;
				this->BC.face         = 11;
				this->BC.cell_face    = CF_ALL;
				break;
			case IDC_RADIO_EC_X:
				this->BC.face_defined = TRUE;
				this->BC.face         = 0;
				this->BC.cell_face    = CF_X;
				break;
			case IDC_RADIO_EC_Y:
				this->BC.face_defined = TRUE;
				this->BC.face         = 1;
				this->BC.cell_face    = CF_Y;
				break;
			case IDC_RADIO_EC_Z:
				this->BC.face_defined = TRUE;
				this->BC.face         = 2;
				this->BC.cell_face    = CF_Z;
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}
		else
		{
			this->BC.face_defined = FALSE;
			this->BC.face         = -1;
			this->BC.cell_face    = CF_UNKNOWN;
		}
	}
	else
	{
		switch (this->BC.cell_face)
		{
		case CF_UNKNOWN:
			this->CheckDlgButton(IDC_CHECK_EXTERIOR_ONLY, BST_UNCHECKED);
			this->CheckRadioButton(IDC_RADIO_EC_ALL, IDC_RADIO_EC_Z, IDC_RADIO_EC_ALL);
			ASSERT(this->BC.face == -1);
			ASSERT(this->BC.face_defined == FALSE);
			break;
		case CF_ALL:
			this->CheckDlgButton(IDC_CHECK_EXTERIOR_ONLY, BST_CHECKED);
			this->CheckRadioButton(IDC_RADIO_EC_ALL, IDC_RADIO_EC_Z, IDC_RADIO_EC_ALL);
			ASSERT(this->BC.face == 11);
			ASSERT(this->BC.face_defined == TRUE);
			break;
		case CF_X:
			this->CheckDlgButton(IDC_CHECK_EXTERIOR_ONLY, BST_CHECKED);
			this->CheckRadioButton(IDC_RADIO_EC_ALL, IDC_RADIO_EC_Z, IDC_RADIO_EC_X);
			ASSERT(this->BC.face == 0);
			ASSERT(this->BC.face_defined == TRUE);
			break;
		case CF_Y:
			this->CheckDlgButton(IDC_CHECK_EXTERIOR_ONLY, BST_CHECKED);
			this->CheckRadioButton(IDC_RADIO_EC_ALL, IDC_RADIO_EC_Z, IDC_RADIO_EC_Y);
			ASSERT(this->BC.face == 1);
			ASSERT(this->BC.face_defined == TRUE);
			break;
		case CF_Z:
			this->CheckDlgButton(IDC_CHECK_EXTERIOR_ONLY, BST_CHECKED);
			this->CheckRadioButton(IDC_RADIO_EC_ALL, IDC_RADIO_EC_Z, IDC_RADIO_EC_Z);
			ASSERT(this->BC.face == 2);
			ASSERT(this->BC.face_defined == TRUE);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		this->OnBnClickedExteriorOnly();
	}

	// solution type
	if (pDX->m_bSaveAndValidate)
	{
		this->BC.bc_solution_type = ST_UNDEFINED;
		if (this->IsDlgButtonChecked(IDC_FIXED_RADIO) == BST_CHECKED)
		{
			this->BC.bc_solution_type = ST_FIXED;
		}
		if (this->IsDlgButtonChecked(IDC_ASSOC_RADIO) == BST_CHECKED)
		{
			this->BC.bc_solution_type = ST_ASSOCIATED;
		}
	}
	else
	{
		if (this->BC.bc_solution_type == ST_FIXED)
		{
			this->CheckRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO, IDC_FIXED_RADIO);
		}
		else
		{
			this->CheckRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO, IDC_ASSOC_RADIO);
		}
	}

	// time series
	if (pDX->m_bSaveAndValidate)
	{
		this->DDV_SoftValidate();
	}
	this->DDX_Series(pDX);

	TRACE("Out %s %s\n", __FUNCTION__, pDX->m_bSaveAndValidate ? _T("TRUE") : _T("FALSE"));
}

void CSpecifiedHeadPropsPage::DDV_SoftValidate()
{
	// description
	CDataExchange dx(this, TRUE);
	::DDX_Text(&dx, IDC_DESC_EDIT, this->Description);

	// exterior cells only
	if (this->IsDlgButtonChecked(IDC_CHECK_EXTERIOR_ONLY) == BST_CHECKED)
	{
		switch (this->GetCheckedRadioButton(IDC_RADIO_EC_ALL, IDC_RADIO_EC_Z))
		{
		case IDC_RADIO_EC_ALL:
			this->BC.face_defined = TRUE;
			this->BC.face         = 11;
			this->BC.cell_face    = CF_ALL;
			break;
		case IDC_RADIO_EC_X:
			this->BC.face_defined = TRUE;
			this->BC.face         = 0;
			this->BC.cell_face    = CF_X;
			break;
		case IDC_RADIO_EC_Y:
			this->BC.face_defined = TRUE;
			this->BC.face         = 1;
			this->BC.cell_face    = CF_Y;
			break;
		case IDC_RADIO_EC_Z:
			this->BC.face_defined = TRUE;
			this->BC.face         = 2;
			this->BC.cell_face    = CF_Z;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	else
	{
		this->BC.face_defined = FALSE;
		this->BC.face         = -1;
		this->BC.cell_face    = CF_UNKNOWN;
	}

	// solution type
	this->BC.bc_solution_type = ST_UNDEFINED;
	if (this->IsDlgButtonChecked(IDC_FIXED_RADIO) == BST_CHECKED)
	{
		this->BC.bc_solution_type = ST_FIXED;
	}
	if (this->IsDlgButtonChecked(IDC_ASSOC_RADIO) == BST_CHECKED)
	{
		this->BC.bc_solution_type = ST_ASSOCIATED;
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
		else
		{
			ASSERT(FALSE);
		}
	}
}

void CSpecifiedHeadPropsPage::DDX_Series(CDataExchange* pDX)
{
	if (this->ItemDDX)
	{
		if (pDX->m_bSaveAndValidate)
		{
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
			if (this->ItemDDX == this->HeadSeries.treeitem)
			{
				this->HeadSeries.DDX_Series(pDX);
				this->SolutionSeries.grid.ShowWindow(SW_HIDE);
				this->HeadSeries.grid.ShowWindow(SW_SHOW);
			}
			else if (this->ItemDDX == this->SolutionSeries.treeitem)
			{
				this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);
				this->HeadSeries.grid.ShowWindow(SW_HIDE);
				this->SolutionSeries.grid.ShowWindow(SW_SHOW);
			}
		}
	}
}

void CSpecifiedHeadPropsPage::SetProperties(const CBC& rBC)
{
	TRACE("In %s\n", __FUNCTION__);

	// copy grid_elt
	this->BC = rBC;

	// time series
	this->HeadSeries.SetSeries(rBC.m_bc_head);
	this->SolutionSeries.SetSeries(rBC.m_bc_solution);

	TRACE("Out %s\n", __FUNCTION__);
}

void CSpecifiedHeadPropsPage::GetProperties(CBC& rBC)const
{
	TRACE("In %s\n", __FUNCTION__);

	rBC = this->BC;

	// series
	this->HeadSeries.GetSeries(rBC.m_bc_head);
	this->SolutionSeries.GetSeries(rBC.m_bc_solution);

	TRACE("Out %s\n", __FUNCTION__);
}

BEGIN_MESSAGE_MAP(CSpecifiedHeadPropsPage, CPropsPropertyPage)
	// IDC_GRID_HEAD
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_HEAD, OnEndLabelEditFlux)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_HEAD, OnSelChangedFlux)

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

	// IDC_CHECK_EXTERIOR_ONLY
	ON_BN_CLICKED(IDC_CHECK_EXTERIOR_ONLY, OnBnClickedExteriorOnly)

	// solution type radios
	ON_BN_CLICKED(IDC_FIXED_RADIO, OnBnClickedSolutionRadios)
	ON_BN_CLICKED(IDC_ASSOC_RADIO, OnBnClickedSolutionRadios)
	ON_BN_SETFOCUS(IDC_FIXED_RADIO, OnBnSetfocusFixedRadio)
	ON_BN_SETFOCUS(IDC_ASSOC_RADIO, OnBnSetfocusAssocRadio)

	// DDX failure
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	ON_BN_SETFOCUS(IDC_CHECK_EXTERIOR_ONLY, &CSpecifiedHeadPropsPage::OnBnSetfocusCheckExteriorOnly)
	ON_BN_SETFOCUS(IDC_RADIO_EC_ALL, &CSpecifiedHeadPropsPage::OnBnSetfocusRadioEcAll)
	ON_BN_SETFOCUS(IDC_RADIO_EC_X, &CSpecifiedHeadPropsPage::OnBnSetfocusRadioEcX)
	ON_BN_SETFOCUS(IDC_RADIO_EC_Y, &CSpecifiedHeadPropsPage::OnBnSetfocusRadioEcY)
	ON_BN_SETFOCUS(IDC_RADIO_EC_Z, &CSpecifiedHeadPropsPage::OnBnSetfocusRadioEcZ)
END_MESSAGE_MAP()


// CSpecifiedHeadPropsPage message handlers

void CSpecifiedHeadPropsPage::OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->HeadSeries.OnEndLabelEdit(pnmgv->iRow, pnmgv->iColumn);
	TRACE("Out %s\n", __FUNCTION__);
}

void CSpecifiedHeadPropsPage::OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result)
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

void CSpecifiedHeadPropsPage::SetPropertyDescription()
{
	if (this->ItemDDX)
	{
		CString strItem = this->TreeCtrl.GetItemText(this->ItemDDX);

		COMPARE_SET(PSZ_HEAD, this->m_sHeadRTF);
		if (strItem.Compare(PSZ_SOLUTION) == 0)
		{
			this->OnBnClickedSolutionRadios();
		}
	}
}

void CSpecifiedHeadPropsPage::OnEnSetfocusDescEdit()
{
	TRACE("In %s\n", __FUNCTION__);
	this->RichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
	TRACE("Out %s\n", __FUNCTION__);
}

void CSpecifiedHeadPropsPage::OnSelChangedFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->HeadSeries.OnSelChanged(pnmgv->iRow, pnmgv->iColumn);

	TRACE("Out %s\n", __FUNCTION__);
}

void CSpecifiedHeadPropsPage::OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->SolutionSeries.OnSelChanged(pnmgv->iRow, pnmgv->iColumn);

	TRACE("Out %s\n", __FUNCTION__);
}

LRESULT CSpecifiedHeadPropsPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	CWnd* pFocus = (CWnd*)wParam;
	if (pFocus && pFocus->GetSafeHwnd())
	{
		ASSERT_KINDOF(CWnd, pFocus);
		pFocus->SetFocus();
	}
	return 0;
}

void CSpecifiedHeadPropsPage::OnBnClickedButtonXYZ()
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
	}
}

void CSpecifiedHeadPropsPage::SetUnits(const CUnits &u)
{
	this->HeadSeries.SetUnits(u);
	this->SolutionSeries.SetUnits(u);
}

void CSpecifiedHeadPropsPage::OnBnClickedExteriorOnly()
{
	if (this->IsDlgButtonChecked(IDC_CHECK_EXTERIOR_ONLY) == BST_CHECKED)
	{
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_EC_ALL))
		{
			pWnd->EnableWindow(TRUE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_EC_X))
		{
			pWnd->EnableWindow(TRUE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_EC_Y))
		{
			pWnd->EnableWindow(TRUE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_EC_Z))
		{
			pWnd->EnableWindow(TRUE);
		}
	}
	else
	{
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_EC_ALL))
		{
			pWnd->EnableWindow(FALSE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_EC_X))
		{
			pWnd->EnableWindow(FALSE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_EC_Y))
		{
			pWnd->EnableWindow(FALSE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_EC_Z))
		{
			pWnd->EnableWindow(FALSE);
		}
	}
}

void CSpecifiedHeadPropsPage::OnCbnSelchangeComboProptype()
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
		else
		{
			ASSERT(FALSE);
		}
	}
}

void CSpecifiedHeadPropsPage::OnBnClickedCheckMixture()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->HeadSeries.treeitem)
		{
			ASSERT(FALSE);
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

void CSpecifiedHeadPropsPage::OnBnClickedSolutionRadios()
{
	if (this->GetCheckedRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO) == IDC_ASSOC_RADIO)
	{
		this->RichEditCtrl.SetWindowText(this->m_sSolutionAssocRTF.c_str());
	}
	else if (this->GetCheckedRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO) == IDC_FIXED_RADIO)
	{
		this->RichEditCtrl.SetWindowText(this->m_sSolutionFixRTF.c_str());
	}
}

void CSpecifiedHeadPropsPage::OnBnSetfocusCheckExteriorOnly()
{
	this->RichEditCtrl.SetWindowText(this->m_sExtCellsRTF.c_str());
}

void CSpecifiedHeadPropsPage::OnBnSetfocusRadioEcAll()
{
	this->RichEditCtrl.SetWindowText(this->m_sExtCellsRTF.c_str());
}

void CSpecifiedHeadPropsPage::OnBnSetfocusRadioEcX()
{
	this->RichEditCtrl.SetWindowText(this->m_sExtCellsRTF.c_str());
}

void CSpecifiedHeadPropsPage::OnBnSetfocusRadioEcY()
{
	this->RichEditCtrl.SetWindowText(this->m_sExtCellsRTF.c_str());
}

void CSpecifiedHeadPropsPage::OnBnSetfocusRadioEcZ()
{
	this->RichEditCtrl.SetWindowText(this->m_sExtCellsRTF.c_str());
}

void CSpecifiedHeadPropsPage::OnBnSetfocusFixedRadio()
{
	this->RichEditCtrl.SetWindowText(this->m_sSolutionFixRTF.c_str());
}

void CSpecifiedHeadPropsPage::OnBnSetfocusAssocRadio()
{
	this->RichEditCtrl.SetWindowText(this->m_sSolutionAssocRTF.c_str());
}
