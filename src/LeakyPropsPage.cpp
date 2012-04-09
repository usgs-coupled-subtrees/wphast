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
const TCHAR PSZ_ELEVATION[] = _T("Elevation");


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
	, ElevationProperty(this, true, false)
	, bSkipFaceValidation(false)
	, FreeSurface(false)
{
	TRACE("In %s\n", __FUNCTION__);

	// load property descriptions
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,   IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sHeadRTF,          IDR_BC_LEAKY_HEAD_RTF);
	CGlobal::LoadRTFString(this->m_sThicknessRTF,     IDR_BC_LEAKY_THICKNESS_RTF);
	CGlobal::LoadRTFString(this->m_sHydCondRTF,       IDR_BC_LEAKY_HYD_COND_RTF);
	CGlobal::LoadRTFString(this->m_sAssocSolutionRTF, IDR_BC_LEAKY_ASSOC_SOL_RTF);
	CGlobal::LoadRTFString(this->m_sElevationRTF,     IDR_BC_LEAKY_ELEVATION_RTF);
	CGlobal::LoadRTFString(this->m_sFaceRTF,          IDR_BC_LEAKY_FACE_RTF);
	CGlobal::LoadRTFString(this->m_sUseMapZRTF,       IDR_BC_LEAKY_USE_MAP_Z_RTF);

	// init properties
	this->SetFlowOnly(false);
	this->SetDefault(false);

	// init data
	this->BC.bc_type = BC_info::BC_LEAKY;

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
		this->HeadSeries.treeitem          = this->TreeCtrl.InsertItem(PSZ_HEAD,      TVI_ROOT, TVI_LAST);
		this->SolutionSeries.treeitem      = this->TreeCtrl.InsertItem(PSZ_SOLUTION,  TVI_ROOT, TVI_LAST);
		this->ThicknessProperty.treeitem   = this->TreeCtrl.InsertItem(PSZ_THICKNESS, TVI_ROOT, TVI_LAST);
		this->HydCondProperty.treeitem     = this->TreeCtrl.InsertItem(PSZ_HYD_COND,  TVI_ROOT, TVI_LAST);
		this->ElevationProperty.treeitem   = this->TreeCtrl.InsertItem(PSZ_ELEVATION, TVI_ROOT, TVI_LAST);

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
		this->ElevationProperty.SetPointsGrid(&this->PointsGrid);
	}

	// description
	::DDX_Text(pDX, IDC_DESC_EDIT, this->Description);

	// face
	if (pDX->m_bSaveAndValidate)
	{
		//if (this->IsDlgButtonChecked(IDC_CHECK_FACE))
		if (!this->bSkipFaceValidation)
		{
			if (this->IsDlgButtonChecked(IDC_FACE_X_RADIO))
			{
				this->BC.face_defined = TRUE;
				this->BC.face         = 0;
			}
			else if (this->IsDlgButtonChecked(IDC_FACE_Y_RADIO))
			{
				this->BC.face_defined = TRUE;
				this->BC.face         = 1;
			}
			else if (this->IsDlgButtonChecked(IDC_FACE_Z_RADIO))
			{
				this->BC.face_defined = TRUE;
				this->BC.face         = 2;
			}
			else
			{
				pDX->PrepareCtrl(IDC_FACE_X_RADIO);
				::AfxMessageBox("No face has been defined");
				pDX->Fail();
			}
		}
	}
	else
	{
		//if (this->BC.face_defined)
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
			}
		}
	}

	// z_coordinate_system
	if (pDX->m_bSaveAndValidate)
	{
		if (this->IsDlgButtonChecked(IDC_USE_MAP_COOR_Z) == BST_CHECKED)
		{
			this->BC.bc_z_coordinate_system_user = PHAST_Transform::MAP;
		}
		else
		{
			this->BC.bc_z_coordinate_system_user = PHAST_Transform::GRID;
		}
	}
	else
	{
		switch(this->BC.bc_z_coordinate_system_user)
		{
		case PHAST_Transform::MAP:
			this->CheckDlgButton(IDC_USE_MAP_COOR_Z, BST_CHECKED);
			break;
		case PHAST_Transform::GRID:
			this->CheckDlgButton(IDC_USE_MAP_COOR_Z, BST_UNCHECKED);
			break;
		default:
			ASSERT(FALSE);
			this->CheckDlgButton(IDC_USE_MAP_COOR_Z, BST_UNCHECKED);
			break;
		}
	}

	// solution type
	if (pDX->m_bSaveAndValidate)
	{
		this->BC.bc_solution_type = ST_ASSOCIATED;
	}

	// properties
	if (pDX->m_bSaveAndValidate)
	{
		this->DDV_SoftValidate();
	}
	this->DDX_Series(pDX);
	this->DDX_Single(pDX);

	if (pDX->m_bSaveAndValidate && this->LastValidation)
	{
		// check req'd properties
		//

		this->HeadSeries.GetSeries(this->BC.m_bc_head);
		if (this->BC.m_bc_head.size() == 0)
		{
			this->ItemDDX = this->HeadSeries.treeitem;
			this->TreeCtrl.SelectItem(this->ItemDDX);
			CDataExchange dx(this, FALSE);
			this->DDX_Series(&dx);
			this->DDX_Series(pDX);
		}

		this->SolutionSeries.GetSeries(this->BC.m_bc_solution);
		if (this->BC.m_bc_solution.size() == 0)
		{
			this->ItemDDX = this->SolutionSeries.treeitem;
			this->TreeCtrl.SelectItem(this->ItemDDX);
			CDataExchange dx(this, FALSE);
			this->DDX_Series(&dx);
			this->DDX_Series(pDX);
		}

		this->ThicknessProperty.GetProperty(this->BC.bc_thick);
		if (this->BC.bc_thick == 0)
		{
			this->ItemDDX = this->ThicknessProperty.treeitem;
			this->TreeCtrl.SelectItem(this->ItemDDX);
			CDataExchange dx(this, FALSE);
			this->DDX_Single(&dx);
			this->DDX_Single(pDX);
		}

		this->HydCondProperty.GetProperty(this->BC.bc_k);
		if (this->BC.bc_k == 0)
		{
			this->ItemDDX = this->HydCondProperty.treeitem;
			this->TreeCtrl.SelectItem(this->ItemDDX);
			CDataExchange dx(this, FALSE);
			this->DDX_Single(&dx);
			this->DDX_Single(pDX);
		}

		ASSERT(this->BC.face_defined);
		this->ElevationProperty.GetProperty(this->BC.bc_z_user);
		if (this->BC.bc_z_user == 0 && (this->BC.face == 2) && this->FreeSurface)
		{
			this->ItemDDX = this->ElevationProperty.treeitem;
			this->TreeCtrl.SelectItem(this->ItemDDX);
			CDataExchange dx(this, FALSE);
			this->DDX_Single(&dx);
			this->DDX_Single(pDX);
		}
	}

	TRACE("Out %s\n", __FUNCTION__);
}

void CLeakyPropsPage::DDV_SoftValidate()
{
	// description
	CDataExchange dx(this, TRUE);
	::DDX_Text(&dx, IDC_DESC_EDIT, this->Description);

	// face
	//if (this->IsDlgButtonChecked(IDC_CHECK_FACE))
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
		else if (this->ItemDDX == this->ElevationProperty.treeitem)
		{
			this->ElevationProperty.DDV_SoftValidate();
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
				this->HeadSeries.DDX_Series(pDX, false);
				ASSERT(!this->SolutionSeries.grid.IsWindowVisible());
				ASSERT(this->HeadSeries.grid.IsWindowVisible());
			}
			else if (this->ItemDDX == this->SolutionSeries.treeitem)
			{
				this->SolutionSeries.DDX_Series(pDX, false);
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
				this->SolutionSeries.DDX_Series(pDX);

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
			this->ThicknessProperty.DDX_Single(pDX, this->LastValidation);

			this->HeadSeries.grid.ShowWindow(SW_HIDE);
			this->SolutionSeries.grid.ShowWindow(SW_HIDE);
			if (!pDX->m_bSaveAndValidate)
			{
				CGridTimeSeries::ShowSingleProperty(this, SW_SHOW);
			}
		}
		else if (this->ItemDDX == this->HydCondProperty.treeitem)
		{
			this->HydCondProperty.DDX_Single(pDX, this->LastValidation);

			this->HeadSeries.grid.ShowWindow(SW_HIDE);
			this->SolutionSeries.grid.ShowWindow(SW_HIDE);
			if (!pDX->m_bSaveAndValidate)
			{
				CGridTimeSeries::ShowSingleProperty(this, SW_SHOW);
			}
		}
		else if (this->ItemDDX == this->ElevationProperty.treeitem)
		{
			this->ElevationProperty.DDX_Single(pDX, this->LastValidation);

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
	this->ElevationProperty.SetProperty(rBC.bc_z_user);

	TRACE("Out %s\n", __FUNCTION__);
}

void CLeakyPropsPage::GetProperties(CBC& rBC)const
{
	TRACE("In %s\n", __FUNCTION__);
	rBC = this->BC;
	TRACE("Out %s\n", __FUNCTION__);
}

BEGIN_MESSAGE_MAP(CLeakyPropsPage, CPropsPropertyPage)
	// override
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTreeSelChanging)

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
	ON_BN_SETFOCUS(IDC_USE_MAP_COOR_Z, &CLeakyPropsPage::OnBnSetfocusUseMapCoorZ)
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

		COMPARE_SET(PSZ_HEAD,      this->m_sHeadRTF);
		COMPARE_SET(PSZ_SOLUTION,  this->m_sAssocSolutionRTF);
		COMPARE_SET(PSZ_THICKNESS, this->m_sThicknessRTF);
		COMPARE_SET(PSZ_HYD_COND,  this->m_sHydCondRTF);
		COMPARE_SET(PSZ_ELEVATION, this->m_sElevationRTF);
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
		else if (this->ItemDDX == this->ElevationProperty.treeitem)
		{
			this->ElevationProperty.OnBnClickedButtonXYZ();
		}
	}
}

void CLeakyPropsPage::SetUnits(const CUnits &u)
{
	this->HeadSeries.SetUnits(u);
	this->SolutionSeries.SetUnits(u);
	this->ThicknessProperty.SetUnits(u);
	this->HydCondProperty.SetUnits(u);
	this->ElevationProperty.SetUnits(u);
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
		else if (this->ItemDDX == this->ElevationProperty.treeitem)
		{
			this->ElevationProperty.OnCbnSelchangeComboProptype();
		}
	}
}

void CLeakyPropsPage::OnBnClickedCheckFace()
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
		else if (this->ItemDDX == this->ElevationProperty.treeitem)
		{
			this->ElevationProperty.OnBnClickedCheckMixture();
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

void CLeakyPropsPage::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	this->bSkipFaceValidation = true;
	CPropsPropertyPage::OnTreeSelChanging(pNotifyStruct, pResult);
	this->bSkipFaceValidation = false;
}

void CLeakyPropsPage::OnBnSetfocusUseMapCoorZ()
{
	this->RichEditCtrl.SetWindowText(this->m_sUseMapZRTF.c_str());
}
