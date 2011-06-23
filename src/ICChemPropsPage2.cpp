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

	if (this->TreeCtrl.GetCount() == 0)
	{
		// setup tree
		this->SolutionProperty.treeitem          = this->TreeCtrl.InsertItem(PSZ_SOLUTION,           TVI_ROOT, TVI_LAST);
		this->EquilibriumPhasesProperty.treeitem = this->TreeCtrl.InsertItem(PSZ_EQUILIBRIUM_PHASES, TVI_ROOT, TVI_LAST);
		this->ExchangeProperty.treeitem          = this->TreeCtrl.InsertItem(PSZ_EXCHANGE,           TVI_ROOT, TVI_LAST);
		this->SurfaceProperty.treeitem           = this->TreeCtrl.InsertItem(PSZ_SURFACE,            TVI_ROOT, TVI_LAST);
		this->GasPhaseProperty.treeitem          = this->TreeCtrl.InsertItem(PSZ_GAS_PHASE,          TVI_ROOT, TVI_LAST);
		this->SolidSolutionsProperty.treeitem    = this->TreeCtrl.InsertItem(PSZ_SOLID_SOLUTIONS,    TVI_ROOT, TVI_LAST);
		this->KineticsProperty.treeitem          = this->TreeCtrl.InsertItem(PSZ_KINETICS,           TVI_ROOT, TVI_LAST);

		// setup tree selection
		this->ItemDDX = this->SolutionProperty.treeitem;
		this->TreeCtrl.SelectItem(this->ItemDDX);
		this->TreeCtrl.ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);

		// set points grid for each propety
		this->SolutionProperty.SetPointsGrid(&this->PointsGrid);
		this->EquilibriumPhasesProperty.SetPointsGrid(&this->PointsGrid);
		this->SurfaceProperty.SetPointsGrid(&this->PointsGrid);
		this->ExchangeProperty.SetPointsGrid(&this->PointsGrid);
		this->GasPhaseProperty.SetPointsGrid(&this->PointsGrid);
		this->SolidSolutionsProperty.SetPointsGrid(&this->PointsGrid);
		this->KineticsProperty.SetPointsGrid(&this->PointsGrid);
	}

	// description
	::DDX_Text(pDX, IDC_DESC_EDIT, this->Description);

	// properties
	if (pDX->m_bSaveAndValidate)
	{
		this->DDV_SoftValidate();
	}
	this->DDX_Single(pDX);

	TRACE("Out %s\n", __FUNCTION__);
}

void CICChemPropsPage2::DDV_SoftValidate()
{
	// description
	CDataExchange dx(this, TRUE);
	::DDX_Text(&dx, IDC_DESC_EDIT, this->Description);

	if (this->ItemDDX)
	{
		// properties
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
	}
}

void CICChemPropsPage2::DDX_Single(CDataExchange* pDX)
{
	if (this->ItemDDX)
	{
		if (!pDX->m_bSaveAndValidate)
		{
			// display single
			CGridTimeSeries::ShowSingleProperty(this, SW_SHOW);
		}

		// DDX_Single selected property
		if (this->ItemDDX == this->SolutionProperty.treeitem)
		{
			this->SolutionProperty.DDX_Single(pDX);
		}
		else if (this->ItemDDX == this->EquilibriumPhasesProperty.treeitem)
		{
			this->EquilibriumPhasesProperty.DDX_Single(pDX, false);
		}
		else if (this->ItemDDX == this->SurfaceProperty.treeitem)
		{
			this->SurfaceProperty.DDX_Single(pDX, false);
		}
		else if (this->ItemDDX == this->ExchangeProperty.treeitem)
		{
			this->ExchangeProperty.DDX_Single(pDX, false);
		}
		else if (this->ItemDDX == this->GasPhaseProperty.treeitem)
		{
			this->GasPhaseProperty.DDX_Single(pDX, false);
		}
		else if (this->ItemDDX == this->SolidSolutionsProperty.treeitem)
		{
			this->SolidSolutionsProperty.DDX_Single(pDX, false);
		}
		else if (this->ItemDDX == this->KineticsProperty.treeitem)
		{
			this->KineticsProperty.DDX_Single(pDX, false);
		}
		else
		{
			ASSERT(FALSE);
		}
	}
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
// COMMENT: {7/13/2009 7:27:18 PM}	// IDC_PROP_TREE
// COMMENT: {7/13/2009 7:27:18 PM}	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTreeSelChanging)
// COMMENT: {7/13/2009 7:27:18 PM}	ON_NOTIFY(TVN_SELCHANGED,  IDC_PROP_TREE, OnTreeSelChanged)

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

// COMMENT: {7/13/2009 7:47:03 PM}void CICChemPropsPage2::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
// COMMENT: {7/13/2009 7:47:03 PM}{
// COMMENT: {7/13/2009 7:47:03 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:47:03 PM}	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
// COMMENT: {7/13/2009 7:47:03 PM}	this->ItemDDX = pTvn->itemOld.hItem;
// COMMENT: {7/13/2009 7:47:03 PM}	if (this->ItemDDX)
// COMMENT: {7/13/2009 7:47:03 PM}	{
// COMMENT: {7/13/2009 7:47:03 PM}		if (this->ItemDDX == this->SolutionProperty.treeitem)
// COMMENT: {7/13/2009 7:47:03 PM}		{
// COMMENT: {7/13/2009 7:47:03 PM}			this->SolutionProperty.DDV_SoftValidate();
// COMMENT: {7/13/2009 7:47:03 PM}		}
// COMMENT: {7/13/2009 7:47:03 PM}		else if (this->ItemDDX == this->EquilibriumPhasesProperty.treeitem)
// COMMENT: {7/13/2009 7:47:03 PM}		{
// COMMENT: {7/13/2009 7:47:03 PM}			this->EquilibriumPhasesProperty.DDV_SoftValidate();
// COMMENT: {7/13/2009 7:47:03 PM}		}
// COMMENT: {7/13/2009 7:47:03 PM}		else if (this->ItemDDX == this->SurfaceProperty.treeitem)
// COMMENT: {7/13/2009 7:47:03 PM}		{
// COMMENT: {7/13/2009 7:47:03 PM}			this->SurfaceProperty.DDV_SoftValidate();
// COMMENT: {7/13/2009 7:47:03 PM}		}
// COMMENT: {7/13/2009 7:47:03 PM}		else if (this->ItemDDX == this->ExchangeProperty.treeitem)
// COMMENT: {7/13/2009 7:47:03 PM}		{
// COMMENT: {7/13/2009 7:47:03 PM}			this->ExchangeProperty.DDV_SoftValidate();
// COMMENT: {7/13/2009 7:47:03 PM}		}
// COMMENT: {7/13/2009 7:47:03 PM}		else if (this->ItemDDX == this->GasPhaseProperty.treeitem)
// COMMENT: {7/13/2009 7:47:03 PM}		{
// COMMENT: {7/13/2009 7:47:03 PM}			this->GasPhaseProperty.DDV_SoftValidate();
// COMMENT: {7/13/2009 7:47:03 PM}		}
// COMMENT: {7/13/2009 7:47:03 PM}		else if (this->ItemDDX == this->SolidSolutionsProperty.treeitem)
// COMMENT: {7/13/2009 7:47:03 PM}		{
// COMMENT: {7/13/2009 7:47:03 PM}			this->SolidSolutionsProperty.DDV_SoftValidate();
// COMMENT: {7/13/2009 7:47:03 PM}		}
// COMMENT: {7/13/2009 7:47:03 PM}		else if (this->ItemDDX == this->KineticsProperty.treeitem)
// COMMENT: {7/13/2009 7:47:03 PM}		{
// COMMENT: {7/13/2009 7:47:03 PM}			this->KineticsProperty.DDV_SoftValidate();
// COMMENT: {7/13/2009 7:47:03 PM}		}
// COMMENT: {7/13/2009 7:47:03 PM}		else
// COMMENT: {7/13/2009 7:47:03 PM}		{
// COMMENT: {7/13/2009 7:47:03 PM}			ASSERT(FALSE);
// COMMENT: {7/13/2009 7:47:03 PM}		}
// COMMENT: {7/13/2009 7:47:03 PM}
// COMMENT: {7/13/2009 7:47:03 PM}
// COMMENT: {7/13/2009 7:47:03 PM}		// force CInPlaceXXX to lose focus
// COMMENT: {7/13/2009 7:47:03 PM}		this->TreeCtrl.SetFocus();
// COMMENT: {7/13/2009 7:47:03 PM}
// COMMENT: {7/13/2009 7:47:03 PM}		if (!this->UpdateData(TRUE))
// COMMENT: {7/13/2009 7:47:03 PM}		{
// COMMENT: {7/13/2009 7:47:03 PM}			// notify which control caused failure
// COMMENT: {7/13/2009 7:47:03 PM}			//
// COMMENT: {7/13/2009 7:47:03 PM}			CWnd* pFocus = CWnd::GetFocus();
// COMMENT: {7/13/2009 7:47:03 PM}			this->PostMessage(UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);
// COMMENT: {7/13/2009 7:47:03 PM}
// COMMENT: {7/13/2009 7:47:03 PM}			// disallow change
// COMMENT: {7/13/2009 7:47:03 PM}			//
// COMMENT: {7/13/2009 7:47:03 PM}			*pResult = TRUE;
// COMMENT: {7/13/2009 7:47:03 PM}			TRACE("Out %s Disallowed\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:47:03 PM}			return;
// COMMENT: {7/13/2009 7:47:03 PM}		}
// COMMENT: {7/13/2009 7:47:03 PM}	}
// COMMENT: {7/13/2009 7:47:03 PM}	*pResult = 0;
// COMMENT: {7/13/2009 7:47:03 PM}	TRACE("Out Allowed %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:47:03 PM}}

#define COMPARE_SET(S, R) \
do { \
	if (strItem.Compare(S) == 0) { \
		this->RichEditCtrl.SetWindowText(R.c_str()); \
	} \
} while (0)

// COMMENT: {7/13/2009 7:43:20 PM}void CICChemPropsPage2::OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
// COMMENT: {7/13/2009 7:43:20 PM}{
// COMMENT: {7/13/2009 7:43:20 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:43:20 PM}	UNREFERENCED_PARAMETER(pResult);
// COMMENT: {7/13/2009 7:43:20 PM}	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
// COMMENT: {7/13/2009 7:43:20 PM}	this->ItemDDX = pTvn->itemNew.hItem;
// COMMENT: {7/13/2009 7:43:20 PM}	if (this->ItemDDX)
// COMMENT: {7/13/2009 7:43:20 PM}	{
// COMMENT: {7/13/2009 7:43:20 PM}		this->UpdateData(FALSE);
// COMMENT: {7/13/2009 7:43:20 PM}
// COMMENT: {7/13/2009 7:43:20 PM}		// update property description
// COMMENT: {7/13/2009 7:43:20 PM}		//
// COMMENT: {7/13/2009 7:43:20 PM}		if (this->TreeCtrl.GetSafeHwnd())
// COMMENT: {7/13/2009 7:43:20 PM}		{
// COMMENT: {7/13/2009 7:43:20 PM}			CString strItem = this->TreeCtrl.GetItemText(this->ItemDDX);
// COMMENT: {7/13/2009 7:43:20 PM}			COMPARE_SET(PSZ_SOLUTION,           this->m_sSolutionRTF);
// COMMENT: {7/13/2009 7:43:20 PM}			COMPARE_SET(PSZ_EQUILIBRIUM_PHASES, this->m_sEquilibriumPhasesRTF);
// COMMENT: {7/13/2009 7:43:20 PM}			COMPARE_SET(PSZ_EXCHANGE,           this->m_sExchangeRTF);
// COMMENT: {7/13/2009 7:43:20 PM}			COMPARE_SET(PSZ_SURFACE,            this->m_sSurfaceRTF);
// COMMENT: {7/13/2009 7:43:20 PM}			COMPARE_SET(PSZ_GAS_PHASE,          this->m_sGasPhaseRTF);
// COMMENT: {7/13/2009 7:43:20 PM}			COMPARE_SET(PSZ_SOLID_SOLUTIONS,    this->m_sSolidSolutionsRTF);
// COMMENT: {7/13/2009 7:43:20 PM}			COMPARE_SET(PSZ_KINETICS,           this->m_sKineticsRTF);
// COMMENT: {7/13/2009 7:43:20 PM}		}
// COMMENT: {7/13/2009 7:43:20 PM}	}
// COMMENT: {7/13/2009 7:43:20 PM}	if (this->TreeCtrl.GetSafeHwnd())
// COMMENT: {7/13/2009 7:43:20 PM}	{
// COMMENT: {7/13/2009 7:43:20 PM}		this->TreeCtrl.SetFocus();
// COMMENT: {7/13/2009 7:43:20 PM}	}
// COMMENT: {7/13/2009 7:43:20 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:43:20 PM}}

void CICChemPropsPage2::SetPropertyDescription()
{
	if (this->ItemDDX)
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
