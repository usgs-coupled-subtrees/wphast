// PropsPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "FluxPropsPage.h"

#include "PropStruct.h"
#include "Global.h"


// CFluxPropsPage dialog

const TCHAR PSZ_FLUX[]     = _T("Flux");
const TCHAR PSZ_SOLUTION[] = _T("Solution");

IMPLEMENT_DYNAMIC(CFluxPropsPage, CResizablePage)

CFluxPropsPage::CFluxPropsPage()
: CResizablePage(CFluxPropsPage::IDD)
, Splitter(3, SSP_VERT, 33, 3)
, ItemDDX(0)
{
	TRACE("%s\n", __FUNCTION__);
	this->CommonConstruct();
}

void CFluxPropsPage::CommonConstruct(void)
{
	this->m_psp.dwFlags &= (~PSP_HASHELP); // remove help button

	this->SetFlowOnly(false);
	this->SetDefault(false);

	// init properties
	//
	this->SetProperties(this->BC);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,       IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sSolutionRTF,          IDR_CHEM_IC_SOLUTION_RTF);
// COMMENT: {6/1/2009 5:47:12 PM}	CGlobal::LoadRTFString(this->m_sEquilibriumPhasesRTF, IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF);
// COMMENT: {6/1/2009 5:47:12 PM}	CGlobal::LoadRTFString(this->m_sSurfaceRTF,           IDR_CHEM_IC_SURFACE_RTF);
// COMMENT: {6/1/2009 5:47:12 PM}	CGlobal::LoadRTFString(this->m_sExchangeRTF,          IDR_CHEM_IC_EXCHANGE_RTF);
// COMMENT: {6/1/2009 5:47:12 PM}	CGlobal::LoadRTFString(this->m_sGasPhaseRTF,          IDR_CHEM_IC_GAS_PHASE_RTF);
// COMMENT: {6/1/2009 5:47:12 PM}	CGlobal::LoadRTFString(this->m_sSolidSolutionsRTF,    IDR_CHEM_IC_SOLID_SOLUTIONS_RTF);
// COMMENT: {6/1/2009 5:47:12 PM}	CGlobal::LoadRTFString(this->m_sKineticsRTF,          IDR_CHEM_IC_KINETICS_RTF);
}

CFluxPropsPage::~CFluxPropsPage()
{
	TRACE("%s\n", __FUNCTION__);
}

#define COMPARE_DDX(S, P) \
do { \
	if (strItem.Compare(S) == 0) { \
		std::vector<Cproperty*> props; \
		props.push_back(static_cast<Cproperty*>(this->BC.P)); \
		props.push_back(static_cast<Cproperty*>(this->BCConstant.P)); \
		props.push_back(static_cast<Cproperty*>(this->BCLinear.P)); \
		CGlobal::DDX_PropertyM( \
			pDX, \
			&this->SheetTop, \
			this->ItemDDX, \
			props, \
			pages \
			); \
	} \
} while (0)
		//props.push_back(static_cast<Cproperty*>(this->BCPoints.P)); \
		//props.push_back(static_cast<Cproperty*>(this->BCXYZ.P)); \

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

void CFluxPropsPage::DoDataExchange(CDataExchange* pDX)
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
		this->htiFLUX     = pTree->InsertItem(PSZ_FLUX,     TVI_ROOT, TVI_LAST);
		this->htiSOLUTION = pTree->InsertItem(PSZ_SOLUTION, TVI_ROOT, TVI_LAST);

// COMMENT: {6/1/2009 5:47:34 PM}		this->htiEXCHANGE           = pTree->InsertItem(PSZ_EXCHANGE,           TVI_ROOT, TVI_LAST);
// COMMENT: {6/1/2009 5:47:34 PM}		this->htiSURFACE            = pTree->InsertItem(PSZ_SURFACE,            TVI_ROOT, TVI_LAST);
// COMMENT: {6/1/2009 5:47:34 PM}		this->htiGAS_PHASE          = pTree->InsertItem(PSZ_GAS_PHASE,          TVI_ROOT, TVI_LAST);
// COMMENT: {6/1/2009 5:47:34 PM}		this->htiSOLID_SOLUTIONS    = pTree->InsertItem(PSZ_SOLID_SOLUTIONS,    TVI_ROOT, TVI_LAST);
// COMMENT: {6/1/2009 5:47:34 PM}		this->htiKINETICS           = pTree->InsertItem(PSZ_KINETICS,           TVI_ROOT, TVI_LAST);

		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);
		this->RichEditCtrl.SetWindowText(this->m_sSolutionRTF.c_str());

		this->ItemDDX = this->htiFLUX;
		pTree->SelectItem(this->ItemDDX);
	}

	if (this->ItemDDX)
	{
		CString strItem = pTree->GetItemText(this->ItemDDX);

		std::vector<CPropertyPage*> pages;
		pages.push_back(&this->PropNoneM);
		pages.push_back(&this->PropConstantM);
		pages.push_back(&this->PropLinearM);
// COMMENT: {6/1/2009 5:48:08 PM}		pages.push_back(&this->PropPoints);
// COMMENT: {6/1/2009 5:48:08 PM}		pages.push_back(&this->PropXYZ);

		//
		// exchange each property

// COMMENT: {6/3/2009 3:56:37 PM}		COMPARE_DDX(PSZ_FLUX,     m_bc_flux);
// COMMENT: {6/3/2009 3:56:37 PM}		COMPARE_DDX(PSZ_SOLUTION, m_bc_solution);

		if (strItem.Compare(PSZ_FLUX) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->BC.current_bc_flux));
			props.push_back(static_cast<Cproperty*>(this->BCConstant.current_bc_flux));
			props.push_back(static_cast<Cproperty*>(this->BCLinear.current_bc_flux));
			CGlobal::DDX_PropertyM(
				pDX,
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}

		if (strItem.Compare(PSZ_SOLUTION) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->BC.current_bc_solution));
			props.push_back(static_cast<Cproperty*>(this->BCConstant.current_bc_solution));
			props.push_back(static_cast<Cproperty*>(this->BCLinear.current_bc_solution));
			CGlobal::DDX_PropertyM(
				pDX,
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}

// COMMENT: {6/1/2009 5:49:01 PM}		//
// COMMENT: {6/1/2009 5:49:01 PM}		// check props that must be defined for defaults
// COMMENT: {6/1/2009 5:49:01 PM}
// COMMENT: {6/1/2009 5:49:01 PM}		CHECK_DEFAULT(solution,           htiSOLUTION,           PSZ_SOLUTION);
// COMMENT: {6/1/2009 5:49:01 PM}		//CHECK_DEFAULT(equilibrium_phases, htiEQUILIBRIUM_PHASES, PSZ_EQUILIBRIUM_PHASES);
// COMMENT: {6/1/2009 5:49:01 PM}		//CHECK_DEFAULT(exchange,           htiEXCHANGE,           PSZ_EXCHANGE);
// COMMENT: {6/1/2009 5:49:01 PM}		//CHECK_DEFAULT(surface,            htiSURFACE,            PSZ_SURFACE);
// COMMENT: {6/1/2009 5:49:01 PM}		//CHECK_DEFAULT(gas_phase,          htiGAS_PHASE,          PSZ_GAS_PHASE);
// COMMENT: {6/1/2009 5:49:01 PM}		//CHECK_DEFAULT(solid_solutions,    htiSOLID_SOLUTIONS,    PSZ_SOLID_SOLUTIONS);
// COMMENT: {6/1/2009 5:49:01 PM}		//CHECK_DEFAULT(kinetics,           htiKINETICS,           PSZ_KINETICS);
	}
	TRACE("Out %s\n", __FUNCTION__);
}


BEGIN_MESSAGE_MAP(CFluxPropsPage, CResizablePage)
	ON_NOTIFY(TVN_SELCHANGINGA, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGINGW, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGEDA,  0x7EEE, OnPageTreeSelChanged)
	ON_NOTIFY(TVN_SELCHANGEDW,  0x7EEE, OnPageTreeSelChanged)
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
END_MESSAGE_MAP()

// CFluxPropsPage message handlers

bool CFluxPropsPage::RegisterSheet(UINT nID, CPropertySheet& rSheet)
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

BOOL CFluxPropsPage::OnInitDialog()
{
	TRACE("In %s\n", __FUNCTION__);

	this->SheetTop.AddPage(&this->PropNoneM);
	this->SheetTop.AddPage(&this->PropConstantM);
	this->SheetTop.AddPage(&this->PropLinearM);
// COMMENT: {6/1/2009 5:49:19 PM}	this->SheetTop.AddPage(&this->PropPoints);
// COMMENT: {6/1/2009 5:49:19 PM}	this->SheetTop.AddPage(&this->PropXYZ);

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

void CFluxPropsPage::OnPageTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
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

void CFluxPropsPage::OnPageTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
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

// COMMENT: {6/1/2009 5:50:24 PM}			COMPARE_SET(PSZ_SOLUTION,           this->m_sSolutionRTF);
// COMMENT: {6/1/2009 5:50:24 PM}			COMPARE_SET(PSZ_EQUILIBRIUM_PHASES, this->m_sEquilibriumPhasesRTF);
// COMMENT: {6/1/2009 5:50:24 PM}			COMPARE_SET(PSZ_EXCHANGE,           this->m_sExchangeRTF);
// COMMENT: {6/1/2009 5:50:24 PM}			COMPARE_SET(PSZ_SURFACE,            this->m_sSurfaceRTF);
// COMMENT: {6/1/2009 5:50:24 PM}			COMPARE_SET(PSZ_GAS_PHASE,          this->m_sGasPhaseRTF);
// COMMENT: {6/1/2009 5:50:24 PM}			COMPARE_SET(PSZ_SOLID_SOLUTIONS,    this->m_sSolidSolutionsRTF);
// COMMENT: {6/1/2009 5:50:24 PM}			COMPARE_SET(PSZ_KINETICS,           this->m_sKineticsRTF);
		}
	}
	CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl();
	if (pTree && pTree->GetSafeHwnd())
	{
		pTree->SetFocus();
	}
	TRACE("Out %s\n", __FUNCTION__);
}

LRESULT CFluxPropsPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
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

void CFluxPropsPage::SetProperties(const CBC& rBC)
{
	TRACE("In %s\n", __FUNCTION__);
	// copy grid_elt
	this->BC = rBC;

	NEW_PROP(this->BC.current_bc_flux);
	NEW_PROP(this->BC.current_bc_solution);
// COMMENT: {6/1/2009 5:50:32 PM}	NEW_PROP(this->BC.exchange);
// COMMENT: {6/1/2009 5:50:32 PM}	NEW_PROP(this->BC.surface);
// COMMENT: {6/1/2009 5:50:32 PM}	NEW_PROP(this->BC.gas_phase);
// COMMENT: {6/1/2009 5:50:32 PM}	NEW_PROP(this->BC.solid_solutions);
// COMMENT: {6/1/2009 5:50:32 PM}	NEW_PROP(this->BC.kinetics);

	// copy to all
	//
	this->BCXYZ = this->BCPoints = this->BCLinear = this->BCConstant = this->BC;
	TRACE("Out %s\n", __FUNCTION__);
}

#define DELETE_PROP(P) \
do { \
	if (P->type == PROP_UNDEFINED) { \
		Cproperty::CopyProperty(&P, 0); \
	} \
} while (0)

void CFluxPropsPage::GetProperties(CBC& rBC)const
{
	TRACE("In %s\n", __FUNCTION__);
	rBC = this->BC;

	// delete inactive properties
	//
	DELETE_PROP(rBC.current_bc_flux);
	DELETE_PROP(rBC.current_bc_solution);
// COMMENT: {6/1/2009 6:03:25 PM}	DELETE_PROP(rBC.exchange);
// COMMENT: {6/1/2009 6:03:25 PM}	DELETE_PROP(rBC.surface);
// COMMENT: {6/1/2009 6:03:25 PM}	DELETE_PROP(rBC.gas_phase);
// COMMENT: {6/1/2009 6:03:25 PM}	DELETE_PROP(rBC.solid_solutions);
// COMMENT: {6/1/2009 6:03:25 PM}	DELETE_PROP(rBC.kinetics);
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage::SetDesc(LPCTSTR desc)
{
	this->PageDesc.SetDesc(desc);
}

LPCTSTR CFluxPropsPage::GetDesc()const
{
	return this->PageDesc.GetDesc();
}
