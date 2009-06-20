// PropsPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ICChemPropsPage.h"

#include "PropStruct.h"
#include "Global.h"


// CICChemPropsPage dialog

const TCHAR PSZ_SOLUTION[]           = _T("Solution");
const TCHAR PSZ_EQUILIBRIUM_PHASES[] = _T("Equilibrium phases");
const TCHAR PSZ_EXCHANGE[]           = _T("Exchange");
const TCHAR PSZ_SURFACE[]            = _T("Surface");
const TCHAR PSZ_GAS_PHASE[]          = _T("Gas phase");
const TCHAR PSZ_SOLID_SOLUTIONS[]    = _T("Solid solutions");
const TCHAR PSZ_KINETICS[]           = _T("Kinetics");

IMPLEMENT_DYNAMIC(CICChemPropsPage, CResizablePage)

CICChemPropsPage::CICChemPropsPage()
: CResizablePage(CICChemPropsPage::IDD)
, Splitter(3, SSP_VERT, 33, 3)
, ItemDDX(0)
{
	TRACE("%s\n", __FUNCTION__);
	this->CommonConstruct();
}

void CICChemPropsPage::CommonConstruct(void)
{
	this->m_psp.dwFlags &= (~PSP_HASHELP); // remove help button

	this->SetFlowOnly(false);
	this->SetDefault(false);

	// init properties
	//
	this->SetProperties(this->ChemIC);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,       IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sSolutionRTF,          IDR_CHEM_IC_SOLUTION_RTF);
	CGlobal::LoadRTFString(this->m_sEquilibriumPhasesRTF, IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF);
	CGlobal::LoadRTFString(this->m_sSurfaceRTF,           IDR_CHEM_IC_SURFACE_RTF);
	CGlobal::LoadRTFString(this->m_sExchangeRTF,          IDR_CHEM_IC_EXCHANGE_RTF);
	CGlobal::LoadRTFString(this->m_sGasPhaseRTF,          IDR_CHEM_IC_GAS_PHASE_RTF);
	CGlobal::LoadRTFString(this->m_sSolidSolutionsRTF,    IDR_CHEM_IC_SOLID_SOLUTIONS_RTF);
	CGlobal::LoadRTFString(this->m_sKineticsRTF,          IDR_CHEM_IC_KINETICS_RTF);
}

CICChemPropsPage::~CICChemPropsPage()
{
	TRACE("%s\n", __FUNCTION__);
}

#define COMPARE_DDX(S, P) \
do { \
	if (strItem.Compare(S) == 0) { \
		std::vector<Cproperty*> props; \
		props.push_back(static_cast<Cproperty*>(this->ChemIC.P)); \
		props.push_back(static_cast<Cproperty*>(this->ChemICConstant.P)); \
		props.push_back(static_cast<Cproperty*>(this->ChemICLinear.P)); \
		props.push_back(static_cast<Cproperty*>(this->ChemICPoints.P)); \
		props.push_back(static_cast<Cproperty*>(this->ChemICXYZ.P)); \
		CGlobal::DDX_Property( \
			pDX, \
			&this->SheetTop, \
			this->ItemDDX, \
			props, \
			pages \
			); \
	} \
} while (0)

#define CHECK_DEFAULT(P, H, S) \
do { \
	if (this->bDefault && pDX->m_bSaveAndValidate && this->ChemIC.P->type == PROP_UNDEFINED) { \
		CString str; \
		pTree->SelectItem(this->H); \
		str.Format("A default \"%s\" must be defined.", S); \
		::AfxMessageBox(str); \
		pDX->Fail(); \
	} \
} while (0)

void CICChemPropsPage::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);
	CResizablePage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, RichEditCtrl);


	// description
	if (pDX->m_bSaveAndValidate)
	{
		this->PageDesc.UpdateData(TRUE);
	}

	CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl();

	if (pTree && pTree->GetCount() == 0)
	{
		this->htiSOLUTION           = pTree->InsertItem(PSZ_SOLUTION,           TVI_ROOT, TVI_LAST);
		this->htiEQUILIBRIUM_PHASES = pTree->InsertItem(PSZ_EQUILIBRIUM_PHASES, TVI_ROOT, TVI_LAST);
		this->htiEXCHANGE           = pTree->InsertItem(PSZ_EXCHANGE,           TVI_ROOT, TVI_LAST);
		this->htiSURFACE            = pTree->InsertItem(PSZ_SURFACE,            TVI_ROOT, TVI_LAST);
		this->htiGAS_PHASE          = pTree->InsertItem(PSZ_GAS_PHASE,          TVI_ROOT, TVI_LAST);
		this->htiSOLID_SOLUTIONS    = pTree->InsertItem(PSZ_SOLID_SOLUTIONS,    TVI_ROOT, TVI_LAST);
		this->htiKINETICS           = pTree->InsertItem(PSZ_KINETICS,           TVI_ROOT, TVI_LAST);

		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);
		this->RichEditCtrl.SetWindowText(this->m_sSolutionRTF.c_str());

		this->ItemDDX = this->htiSOLUTION;
		pTree->SelectItem(this->ItemDDX);
	}

	if (this->ItemDDX)
	{
		CString strItem = pTree->GetItemText(this->ItemDDX);

		std::vector<CPropertyPage*> pages;
		pages.push_back(&this->PropNone);
		pages.push_back(&this->PropConstant);
		pages.push_back(&this->PropLinear);
		pages.push_back(&this->PropPoints);
		pages.push_back(&this->PropXYZ);

		//
		// exchange each property

		COMPARE_DDX(PSZ_SOLUTION,           solution);
		COMPARE_DDX(PSZ_EQUILIBRIUM_PHASES, equilibrium_phases);
		COMPARE_DDX(PSZ_EXCHANGE,           exchange);
		COMPARE_DDX(PSZ_SURFACE,            surface);
		COMPARE_DDX(PSZ_GAS_PHASE,          gas_phase);
		COMPARE_DDX(PSZ_SOLID_SOLUTIONS,    solid_solutions);
		COMPARE_DDX(PSZ_KINETICS,           kinetics);

		//
		// check props that must be defined for defaults

		CHECK_DEFAULT(solution,           htiSOLUTION,           PSZ_SOLUTION);
		//CHECK_DEFAULT(equilibrium_phases, htiEQUILIBRIUM_PHASES, PSZ_EQUILIBRIUM_PHASES);
		//CHECK_DEFAULT(exchange,           htiEXCHANGE,           PSZ_EXCHANGE);
		//CHECK_DEFAULT(surface,            htiSURFACE,            PSZ_SURFACE);
		//CHECK_DEFAULT(gas_phase,          htiGAS_PHASE,          PSZ_GAS_PHASE);
		//CHECK_DEFAULT(solid_solutions,    htiSOLID_SOLUTIONS,    PSZ_SOLID_SOLUTIONS);
		//CHECK_DEFAULT(kinetics,           htiKINETICS,           PSZ_KINETICS);
	}
	TRACE("Out %s\n", __FUNCTION__);
}


BEGIN_MESSAGE_MAP(CICChemPropsPage, CResizablePage)
	ON_NOTIFY(TVN_SELCHANGINGA, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGINGW, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGEDA,  0x7EEE, OnPageTreeSelChanged)
	ON_NOTIFY(TVN_SELCHANGEDW,  0x7EEE, OnPageTreeSelChanged)
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
END_MESSAGE_MAP()

// CICChemPropsPage message handlers

bool CICChemPropsPage::RegisterSheet(UINT nID, CPropertySheet& rSheet)
{
	TRACE("In %s\n", __FUNCTION__);

 	CWnd* pWnd = this->GetDlgItem( nID );
	if(!pWnd)
	{
		ASSERT(FALSE);
		TRACE("Out %s\n", __FUNCTION__);
		return false;
	}

	VERIFY(rSheet.Create(this, WS_CHILD|WS_VISIBLE, 0));

	if(0 != rSheet.GetPageCount())
	{
		rSheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
		rSheet.ModifyStyle(0, WS_TABSTOP);

		CRect rcSheet;
		pWnd->GetWindowRect(&rcSheet);
		this->ScreenToClient(&rcSheet);
		rSheet.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
	}

	TRACE("Out %s\n", __FUNCTION__);
	return true;
}

BOOL CICChemPropsPage::OnInitDialog()
{
	TRACE("In %s\n", __FUNCTION__);

	this->SheetTop.AddPage(&this->PropNone);
	this->SheetTop.AddPage(&this->PropConstant);
	this->SheetTop.AddPage(&this->PropLinear);
	this->SheetTop.AddPage(&this->PropPoints);
	this->SheetTop.AddPage(&this->PropXYZ);
	
	this->SheetTop.SetTreeViewMode(TRUE, FALSE, FALSE);
	this->SheetTop.SetIsResizable(true);

	if (CWnd* pWnd = this->GetDlgItem(IDC_DESC_PAGE))
	{
		VERIFY(this->PageDesc.Create(IDD_PROP_DESC, this));
		ASSERT(this->PageDesc.GetStyle() | DS_CONTROL);			
		ASSERT(this->PageDesc.GetExStyle() | WS_EX_CONTROLPARENT);

		CRect rcSheet;
		pWnd->GetWindowRect(&rcSheet);
		this->ScreenToClient(&rcSheet);
		this->PageDesc.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		this->PageDesc.ShowWindow(SW_SHOW);
	}
	else
	{
		ASSERT(FALSE);
	}

	this->RegisterSheet(IDC_TREE_PROPS, this->SheetTop);

	VERIFY(this->Splitter.Create(this));
	this->Splitter.SetPane(0, &this->PageDesc);
	this->Splitter.SetPane(1, &this->SheetTop);
	this->Splitter.SetPane(2, this->GetDlgItem(IDC_DESC_RICHEDIT));

	const int splitter_sizes[3] = {33, 164, 80};
	this->Splitter.SetPaneSizes(splitter_sizes);

	const bool frozen[3] = {true, false, false};
	this->Splitter.SetFrozenPanes(frozen);

	this->AddAnchor(this->Splitter, TOP_LEFT, BOTTOM_RIGHT);

	if (CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl())
	{
		pTree->ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);
		pTree->DeleteAllItems();
	}

	__super::OnInitDialog();

	// Add extra initialization here

	TRACE("Out %s\n", __FUNCTION__);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CICChemPropsPage::OnPageTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemOld.hItem;
	if (this->ItemDDX)
	{
		if (!this->UpdateData(TRUE))
		{
			// notify which control caused failure
			//
			CWnd* pFocus = CWnd::GetFocus();
			::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

			// disallow change
			//
			*pResult = TRUE;
			TRACE("Out Disallowed %s\n", __FUNCTION__);
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

void CICChemPropsPage::OnPageTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
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
		CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl();
		if (pTree && pTree->GetSafeHwnd())
		{
			CString strItem = pTree->GetItemText(this->ItemDDX);

			COMPARE_SET(PSZ_SOLUTION,           this->m_sSolutionRTF);
			COMPARE_SET(PSZ_EQUILIBRIUM_PHASES, this->m_sEquilibriumPhasesRTF);
			COMPARE_SET(PSZ_EXCHANGE,           this->m_sExchangeRTF);
			COMPARE_SET(PSZ_SURFACE,            this->m_sSurfaceRTF);
			COMPARE_SET(PSZ_GAS_PHASE,          this->m_sGasPhaseRTF);
			COMPARE_SET(PSZ_SOLID_SOLUTIONS,    this->m_sSolidSolutionsRTF);
			COMPARE_SET(PSZ_KINETICS,           this->m_sKineticsRTF);
		}
	}
	CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl();
	if (pTree && pTree->GetSafeHwnd())
	{
		pTree->SetFocus();
	}
	TRACE("Out %s\n", __FUNCTION__);
}

LRESULT CICChemPropsPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	TRACE("In %s\n", __FUNCTION__);
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	TRACE("Out %s\n", __FUNCTION__);
	return 0;
}

#define NEW_PROP(P) \
do { \
	if (P == 0) { \
		P = new Cproperty(); \
	} \
} while (0)

void CICChemPropsPage::SetProperties(const CChemIC& rChemIC)
{
	TRACE("In %s\n", __FUNCTION__);
	// copy grid_elt
	this->ChemIC = rChemIC;

	NEW_PROP(this->ChemIC.solution);
	NEW_PROP(this->ChemIC.equilibrium_phases);
	NEW_PROP(this->ChemIC.exchange);
	NEW_PROP(this->ChemIC.surface);
	NEW_PROP(this->ChemIC.gas_phase);
	NEW_PROP(this->ChemIC.solid_solutions);
	NEW_PROP(this->ChemIC.kinetics);	

	// copy to all
	//
	this->ChemICXYZ = this->ChemICPoints = this->ChemICLinear = this->ChemICConstant = this->ChemIC;
	TRACE("Out %s\n", __FUNCTION__);
}

#define DELETE_PROP(P) \
do { \
	if (P->type == PROP_UNDEFINED) { \
		Cproperty::CopyProperty(&P, 0); \
	} \
} while (0)
		
void CICChemPropsPage::GetProperties(CChemIC& rChemIC)const
{
	TRACE("In %s\n", __FUNCTION__);
	rChemIC = this->ChemIC;

	// delete inactive properties
	//
	DELETE_PROP(rChemIC.solution);
	DELETE_PROP(rChemIC.equilibrium_phases);
	DELETE_PROP(rChemIC.exchange);
	DELETE_PROP(rChemIC.surface);
	DELETE_PROP(rChemIC.gas_phase);
	DELETE_PROP(rChemIC.solid_solutions);
	DELETE_PROP(rChemIC.kinetics);
	TRACE("Out %s\n", __FUNCTION__);
}

void CICChemPropsPage::SetDesc(LPCTSTR desc)
{
	this->PageDesc.SetDesc(desc);
}

LPCTSTR CICChemPropsPage::GetDesc()const
{
	return this->PageDesc.GetDesc();
}
