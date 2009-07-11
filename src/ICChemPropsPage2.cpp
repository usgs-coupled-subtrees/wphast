// ICChemPropsPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ICChemPropsPage2.h"

#include "Global.h"

// CICChemPropsPage2 dialog

const TCHAR PSZ_SOLUTION[]           = _T("Solution");
const TCHAR PSZ_EQUILIBRIUM_PHASES[] = _T("Equilibrium phases");
const TCHAR PSZ_EXCHANGE[]           = _T("Exchange");
const TCHAR PSZ_SURFACE[]            = _T("Surface");
const TCHAR PSZ_GAS_PHASE[]          = _T("Gas phase");
const TCHAR PSZ_SOLID_SOLUTIONS[]    = _T("Solid solutions");
const TCHAR PSZ_KINETICS[]           = _T("Kinetics");

const int SELECTED = 0;
const int SINGLE = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CICChemPropsPage2, CPropsPropertyPage)

CICChemPropsPage2::CICChemPropsPage2()
	: CPropsPropertyPage(CICChemPropsPage2::IDD)
	, SolutionProperty(this, true, true)
	, EquilibriumPhasesProperty(this, true, true)
	, SurfaceProperty(this, true, true)
	, ExchangeProperty(this, true, true)
	, GasPhaseProperty(this, true, true)
	, SolidSolutionsProperty(this, true, true)
	, KineticsProperty(this, true, true)
	, Default(false)
	, FlowOnly(false)
{
	TRACE("In %s\n", __FUNCTION__);

	// load property descriptions
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,       IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sSolutionRTF,          IDR_CHEM_IC_SOLUTION_RTF);
	CGlobal::LoadRTFString(this->m_sEquilibriumPhasesRTF, IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF);
	CGlobal::LoadRTFString(this->m_sSurfaceRTF,           IDR_CHEM_IC_SURFACE_RTF);
	CGlobal::LoadRTFString(this->m_sExchangeRTF,          IDR_CHEM_IC_EXCHANGE_RTF);
	CGlobal::LoadRTFString(this->m_sGasPhaseRTF,          IDR_CHEM_IC_GAS_PHASE_RTF);
	CGlobal::LoadRTFString(this->m_sSolidSolutionsRTF,    IDR_CHEM_IC_SOLID_SOLUTIONS_RTF);
	CGlobal::LoadRTFString(this->m_sKineticsRTF,          IDR_CHEM_IC_KINETICS_RTF);

	TRACE("Out %s\n", __FUNCTION__);
}

CICChemPropsPage2::~CICChemPropsPage2()
{
	TRACE("In %s\n", __FUNCTION__);
	TRACE("Out %s\n", __FUNCTION__);
}

void CICChemPropsPage2::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);
	CPropsPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	DDX_Control(pDX, IDC_PROP_TREE, this->TreeCtrl);

	if (this->TreeCtrl.GetCount() == 0)
	{
		this->SolutionProperty.treeitem          = this->TreeCtrl.InsertItem(PSZ_SOLUTION,           TVI_ROOT, TVI_LAST);
		this->EquilibriumPhasesProperty.treeitem = this->TreeCtrl.InsertItem(PSZ_EQUILIBRIUM_PHASES, TVI_ROOT, TVI_LAST);
		this->SurfaceProperty.treeitem           = this->TreeCtrl.InsertItem(PSZ_EXCHANGE,           TVI_ROOT, TVI_LAST);
		this->ExchangeProperty.treeitem          = this->TreeCtrl.InsertItem(PSZ_SURFACE,            TVI_ROOT, TVI_LAST);
		this->GasPhaseProperty.treeitem          = this->TreeCtrl.InsertItem(PSZ_GAS_PHASE,          TVI_ROOT, TVI_LAST);
		this->SolidSolutionsProperty.treeitem    = this->TreeCtrl.InsertItem(PSZ_SOLID_SOLUTIONS,    TVI_ROOT, TVI_LAST);
		this->KineticsProperty.treeitem          = this->TreeCtrl.InsertItem(PSZ_KINETICS,           TVI_ROOT, TVI_LAST);

		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);

		this->ItemDDX = this->SolutionProperty.treeitem;
		this->TreeCtrl.SelectItem(this->ItemDDX);

		this->TreeCtrl.ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);
	}

	DDX_GridControl(pDX, IDC_POINTS_GRID, this->PointsGrid);

	this->SolutionProperty.SetPointsGrid(&this->PointsGrid);
	this->EquilibriumPhasesProperty.SetPointsGrid(&this->PointsGrid);
	this->SurfaceProperty.SetPointsGrid(&this->PointsGrid);
	this->ExchangeProperty.SetPointsGrid(&this->PointsGrid);
	this->GasPhaseProperty.SetPointsGrid(&this->PointsGrid);
	this->SolidSolutionsProperty.SetPointsGrid(&this->PointsGrid);
	this->KineticsProperty.SetPointsGrid(&this->PointsGrid);

	// description
	::DDX_Text(pDX, IDC_DESC_EDIT, this->Description);

	if (pDX->m_bSaveAndValidate)
	{
		// properties
		if (this->ItemDDX == this->SolutionProperty.treeitem)
		{
			this->SolutionProperty.DDV_SoftValidate();
			this->SolutionProperty.DDX_Single(pDX, this->Default);
		}
		else if (this->ItemDDX == this->EquilibriumPhasesProperty.treeitem)
		{
			this->EquilibriumPhasesProperty.DDV_SoftValidate();
			this->EquilibriumPhasesProperty.DDX_Single(pDX, false);
		}
		else if (this->ItemDDX == this->SurfaceProperty.treeitem)
		{
			this->SurfaceProperty.DDV_SoftValidate();
			this->SurfaceProperty.DDX_Single(pDX, false);
		}
		else if (this->ItemDDX == this->ExchangeProperty.treeitem)
		{
			this->ExchangeProperty.DDV_SoftValidate();
			this->ExchangeProperty.DDX_Single(pDX, false);
		}
		else if (this->ItemDDX == this->GasPhaseProperty.treeitem)
		{
			this->GasPhaseProperty.DDV_SoftValidate();
			this->GasPhaseProperty.DDX_Single(pDX, false);
		}
		else if (this->ItemDDX == this->SolidSolutionsProperty.treeitem)
		{
			this->SolidSolutionsProperty.DDV_SoftValidate();
			this->SolidSolutionsProperty.DDX_Single(pDX, false);
		}
		else if (this->ItemDDX == this->KineticsProperty.treeitem)
		{
			this->KineticsProperty.DDV_SoftValidate();
			this->KineticsProperty.DDX_Single(pDX, false);
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	else
	{
		// properties
		CGridTimeSeries::ShowSingleProperty(this, SW_SHOW);

		// properties
		if (this->ItemDDX == this->SolutionProperty.treeitem)
		{
			this->SolutionProperty.DDX_Single(pDX);
		}
		else if (this->ItemDDX == this->EquilibriumPhasesProperty.treeitem)
		{
			this->EquilibriumPhasesProperty.DDX_Single(pDX);
		}
		else if (this->ItemDDX == this->SurfaceProperty.treeitem)
		{
			this->SurfaceProperty.DDX_Single(pDX);
		}
		else if (this->ItemDDX == this->ExchangeProperty.treeitem)
		{
			this->ExchangeProperty.DDX_Single(pDX);
		}
		else if (this->ItemDDX == this->GasPhaseProperty.treeitem)
		{
			this->GasPhaseProperty.DDX_Single(pDX);
		}
		else if (this->ItemDDX == this->SolidSolutionsProperty.treeitem)
		{
			this->SolidSolutionsProperty.DDX_Single(pDX);
		}
		else if (this->ItemDDX == this->KineticsProperty.treeitem)
		{
			this->KineticsProperty.DDX_Single(pDX);
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CICChemPropsPage2::SetProperties(const CChemIC& rChemIC)
{
	TRACE("In %s\n", __FUNCTION__);

	// copy grid_elt
	this->ChemIC = rChemIC;

	// single properties
	this->SolutionProperty.SetProperty(rChemIC.solution);
	this->EquilibriumPhasesProperty.SetProperty(rChemIC.equilibrium_phases);
	this->SurfaceProperty.SetProperty(rChemIC.surface);
	this->ExchangeProperty.SetProperty(rChemIC.exchange);
	this->GasPhaseProperty.SetProperty(rChemIC.gas_phase);
	this->SolidSolutionsProperty.SetProperty(rChemIC.solid_solutions);
	this->KineticsProperty.SetProperty(rChemIC.kinetics);

	TRACE("Out %s\n", __FUNCTION__);
}

void CICChemPropsPage2::GetProperties(CChemIC& rChemIC)const
{
	TRACE("In %s\n", __FUNCTION__);

	rChemIC = this->ChemIC;

	// single properties
	this->SolutionProperty.GetProperty(rChemIC.solution);
	this->EquilibriumPhasesProperty.GetProperty(rChemIC.equilibrium_phases);
	this->SurfaceProperty.GetProperty(rChemIC.surface);
	this->ExchangeProperty.GetProperty(rChemIC.exchange);
	this->GasPhaseProperty.GetProperty(rChemIC.gas_phase);
	this->SolidSolutionsProperty.GetProperty(rChemIC.solid_solutions);
	this->KineticsProperty.GetProperty(rChemIC.kinetics);

	TRACE("Out %s\n", __FUNCTION__);
}

BEGIN_MESSAGE_MAP(CICChemPropsPage2, CPropsPropertyPage)
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

	// DDX failure
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
END_MESSAGE_MAP()


// CICChemPropsPage2 message handlers

void CICChemPropsPage2::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemOld.hItem;
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->SolutionProperty.treeitem)
		{
			this->SolutionProperty.DDV_SoftValidate();
		}
		else if (this->ItemDDX == this->EquilibriumPhasesProperty.treeitem)
		{
			this->EquilibriumPhasesProperty.DDV_SoftValidate();
		}
		else if (this->ItemDDX == this->SurfaceProperty.treeitem)
		{
			this->SurfaceProperty.DDV_SoftValidate();
		}
		else if (this->ItemDDX == this->ExchangeProperty.treeitem)
		{
			this->ExchangeProperty.DDV_SoftValidate();
		}
		else if (this->ItemDDX == this->GasPhaseProperty.treeitem)
		{
			this->GasPhaseProperty.DDV_SoftValidate();
		}
		else if (this->ItemDDX == this->SolidSolutionsProperty.treeitem)
		{
			this->SolidSolutionsProperty.DDV_SoftValidate();
		}
		else if (this->ItemDDX == this->KineticsProperty.treeitem)
		{
			this->KineticsProperty.DDV_SoftValidate();
		}
		else
		{
			ASSERT(FALSE);
		}


		// force CInPlaceXXX to lose focus
		this->TreeCtrl.SetFocus();

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

void CICChemPropsPage2::OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
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
			COMPARE_SET(PSZ_SOLUTION,           this->m_sSolutionRTF);
			COMPARE_SET(PSZ_EQUILIBRIUM_PHASES, this->m_sEquilibriumPhasesRTF);
			COMPARE_SET(PSZ_EXCHANGE,           this->m_sExchangeRTF);
			COMPARE_SET(PSZ_SURFACE,            this->m_sSurfaceRTF);
			COMPARE_SET(PSZ_GAS_PHASE,          this->m_sGasPhaseRTF);
			COMPARE_SET(PSZ_SOLID_SOLUTIONS,    this->m_sSolidSolutionsRTF);
			COMPARE_SET(PSZ_KINETICS,           this->m_sKineticsRTF);
		}
	}
	if (this->TreeCtrl.GetSafeHwnd())
	{
		this->TreeCtrl.SetFocus();
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CICChemPropsPage2::OnEnSetfocusDescEdit()
{
	TRACE("In %s\n", __FUNCTION__);
	this->RichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
	TRACE("Out %s\n", __FUNCTION__);
}

LRESULT CICChemPropsPage2::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	CWnd* pFocus = (CWnd*)wParam;
	if (pFocus && pFocus->GetSafeHwnd())
	{
		ASSERT_KINDOF(CWnd, pFocus);
		pFocus->SetFocus();
	}
	return 0;
}

void CICChemPropsPage2::OnBnClickedButtonXYZ()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->SolutionProperty.treeitem)
		{
			this->SolutionProperty.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->EquilibriumPhasesProperty.treeitem)
		{
			this->EquilibriumPhasesProperty.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->SurfaceProperty.treeitem)
		{
			this->SurfaceProperty.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->ExchangeProperty.treeitem)
		{
			this->ExchangeProperty.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->GasPhaseProperty.treeitem)
		{
			this->GasPhaseProperty.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->SolidSolutionsProperty.treeitem)
		{
			this->SolidSolutionsProperty.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->KineticsProperty.treeitem)
		{
			this->KineticsProperty.OnBnClickedButtonXYZ();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

void CICChemPropsPage2::SetUnits(const CUnits &u)
{
	this->SolutionProperty.SetUnits(u);
	this->EquilibriumPhasesProperty.SetUnits(u);
	this->SurfaceProperty.SetUnits(u);
	this->ExchangeProperty.SetUnits(u);
	this->GasPhaseProperty.SetUnits(u);
	this->SolidSolutionsProperty.SetUnits(u);
	this->KineticsProperty.SetUnits(u);
}

void CICChemPropsPage2::OnCbnSelchangeComboProptype()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->SolutionProperty.treeitem)
		{
			this->SolutionProperty.OnCbnSelchangeComboProptype();
		}
		else if (this->ItemDDX == this->EquilibriumPhasesProperty.treeitem)
		{
			this->EquilibriumPhasesProperty.OnCbnSelchangeComboProptype();
		}
		else if (this->ItemDDX == this->SurfaceProperty.treeitem)
		{
			this->SurfaceProperty.OnCbnSelchangeComboProptype();
		}
		else if (this->ItemDDX == this->ExchangeProperty.treeitem)
		{
			this->ExchangeProperty.OnCbnSelchangeComboProptype();
		}
		else if (this->ItemDDX == this->GasPhaseProperty.treeitem)
		{
			this->GasPhaseProperty.OnCbnSelchangeComboProptype();
		}
		else if (this->ItemDDX == this->SolidSolutionsProperty.treeitem)
		{
			this->SolidSolutionsProperty.OnCbnSelchangeComboProptype();
		}
		else if (this->ItemDDX == this->KineticsProperty.treeitem)
		{
			this->KineticsProperty.OnCbnSelchangeComboProptype();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

void CICChemPropsPage2::OnBnClickedCheckMixture()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->SolutionProperty.treeitem)
		{
			this->SolutionProperty.OnBnClickedCheckMixture();
		}
		else if (this->ItemDDX == this->EquilibriumPhasesProperty.treeitem)
		{
			this->EquilibriumPhasesProperty.OnBnClickedCheckMixture();
		}
		else if (this->ItemDDX == this->SurfaceProperty.treeitem)
		{
			this->SurfaceProperty.OnBnClickedCheckMixture();
		}
		else if (this->ItemDDX == this->ExchangeProperty.treeitem)
		{
			this->ExchangeProperty.OnBnClickedCheckMixture();
		}
		else if (this->ItemDDX == this->GasPhaseProperty.treeitem)
		{
			this->GasPhaseProperty.OnBnClickedCheckMixture();
		}
		else if (this->ItemDDX == this->SolidSolutionsProperty.treeitem)
		{
			this->SolidSolutionsProperty.OnBnClickedCheckMixture();
		}
		else if (this->ItemDDX == this->KineticsProperty.treeitem)
		{
			this->KineticsProperty.OnBnClickedCheckMixture();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}
