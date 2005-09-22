// ChemICPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ChemICPropertyPage.h"

#include "Global.h"
#include "property.h"
#include "NewModelWizard.h"

// CChemICPropertyPage dialog

const TCHAR PSZ_SOLUTION[]           = _T("Solution");
const TCHAR PSZ_EQUILIBRIUM_PHASES[] = _T("Equilibrium phases");
const TCHAR PSZ_EXCHANGE[]           = _T("Exchange");
const TCHAR PSZ_SURFACE[]            = _T("Surface");
const TCHAR PSZ_GAS_PHASE[]          = _T("Gas phase");
const TCHAR PSZ_SOLID_SOLUTIONS[]    = _T("Solid solutions");
const TCHAR PSZ_KINETICS[]           = _T("Kinetics");


IMPLEMENT_DYNAMIC(CChemICPropertyPage, CPropertyPage)

CChemICPropertyPage::CChemICPropertyPage()
	: CPropertyPage(CChemICPropertyPage::IDD, IDS_CHEM_IC_CAPTION161)
{
	this->CommonConstruct();
}

	CChemICPropertyPage(UINT nIDTemplate, UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

CChemICPropertyPage::CChemICPropertyPage(UINT nIDTemplate, UINT nIDCaption)
	: CPropertyPage(nIDTemplate, nIDCaption)
{
	this->CommonConstruct();
}

CChemICPropertyPage::CChemICPropertyPage(UINT nIDTemplate, UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(nIDTemplate, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
{
	this->CommonConstruct();
}

void CChemICPropertyPage::CommonConstruct(void)
{
	this->m_htiDDX              = 0;

	// init properties
	//
	this->SetProperties(this->m_chem_ic);

	this->m_bFirstPaint = true;
	this->SetNewModelWizard(false);
	this->SetFlowOnlyWizard(false);
	this->SetShowFlowOnlyMessage(false);


	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sSolutionRTF,          IDR_CHEM_IC_SOLUTION_RTF);
	CGlobal::LoadRTFString(this->m_sEquilibriumPhasesRTF, IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF);
	CGlobal::LoadRTFString(this->m_sSurfaceRTF,           IDR_CHEM_IC_SURFACE_RTF);
	CGlobal::LoadRTFString(this->m_sExchangeRTF,          IDR_CHEM_IC_EXCHANGE_RTF);
	CGlobal::LoadRTFString(this->m_sGasPhaseRTF,          IDR_CHEM_IC_GAS_PHASE_RTF);
	CGlobal::LoadRTFString(this->m_sSolidSolutionsRTF,    IDR_CHEM_IC_SOLID_SOLUTIONS_RTF);
	CGlobal::LoadRTFString(this->m_sKineticsRTF,          IDR_CHEM_IC_KINETICS_RTF);
}

CChemICPropertyPage::~CChemICPropertyPage()
{
}

void CChemICPropertyPage::GetProperties(CChemIC& chemIC)const
{
	chemIC = this->m_chem_ic;

	// delete UNDEFINED properties
	//
	if (chemIC.solution->type == UNDEFINED) {
		Cproperty::CopyProperty(&chemIC.solution, 0);
	}
	if (chemIC.equilibrium_phases->type == UNDEFINED) {
		Cproperty::CopyProperty(&chemIC.equilibrium_phases, 0);
	}
	if (chemIC.surface->type == UNDEFINED) {
		Cproperty::CopyProperty(&chemIC.surface, 0);
	}
	if (chemIC.gas_phase->type == UNDEFINED) {
		Cproperty::CopyProperty(&chemIC.gas_phase, 0);
	}
	if (chemIC.solid_solutions->type == UNDEFINED) {
		Cproperty::CopyProperty(&chemIC.solid_solutions, 0);
	}
	if (chemIC.kinetics->type == UNDEFINED) {
		Cproperty::CopyProperty(&chemIC.kinetics, 0);
	}
}

void CChemICPropertyPage::SetProperties(const CChemIC& chemIC)
{
	// copy grid_elt
	this->m_chem_ic = chemIC;
	
	// fill empty properties
	//
	if (this->m_chem_ic.solution == NULL) {
		this->m_chem_ic.solution = new Cproperty();
	}
	if (this->m_chem_ic.equilibrium_phases == NULL) {
		this->m_chem_ic.equilibrium_phases = new Cproperty();
	}
	if (this->m_chem_ic.exchange == NULL) {
		this->m_chem_ic.exchange = new Cproperty();
	}
	if (this->m_chem_ic.surface == NULL) {
		this->m_chem_ic.surface = new Cproperty();
	}
	if (this->m_chem_ic.gas_phase == NULL) {
		this->m_chem_ic.gas_phase = new Cproperty();
	}
	if (this->m_chem_ic.solid_solutions == NULL) {
		this->m_chem_ic.solid_solutions = new Cproperty();
	}
	if (this->m_chem_ic.kinetics == NULL) {
		this->m_chem_ic.kinetics = new Cproperty();
	}

	// copy to fixed and linear
	//
	this->m_chem_icFixed = this->m_chem_icLinear = this->m_chem_ic;
}

void CChemICPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_TREE, m_wndTreeCtrl);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	if (this->m_wndTreeCtrl.GetCount() == 0) {
		HTREEITEM hti;

		// solution
		hti = this->m_wndTreeCtrl.InsertItem(PSZ_SOLUTION, TVI_ROOT, TVI_LAST);
		if (this->m_chem_ic.solution->type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// equilibrium_phases
		hti = this->m_wndTreeCtrl.InsertItem(PSZ_EQUILIBRIUM_PHASES, TVI_ROOT, TVI_LAST);
		if (this->m_chem_ic.equilibrium_phases->type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// exchange
		hti = this->m_wndTreeCtrl.InsertItem(PSZ_EXCHANGE, TVI_ROOT, TVI_LAST);
		if (this->m_chem_ic.exchange->type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// surface
		hti = this->m_wndTreeCtrl.InsertItem(PSZ_SURFACE, TVI_ROOT, TVI_LAST);
		if (this->m_chem_ic.surface->type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// gas_phase
		hti = this->m_wndTreeCtrl.InsertItem(PSZ_GAS_PHASE, TVI_ROOT, TVI_LAST);
		if (this->m_chem_ic.gas_phase->type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// solid_solutions
		hti = this->m_wndTreeCtrl.InsertItem(PSZ_SOLID_SOLUTIONS, TVI_ROOT, TVI_LAST);
		if (this->m_chem_ic.solid_solutions->type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// kinetics
		hti = this->m_wndTreeCtrl.InsertItem(PSZ_KINETICS, TVI_ROOT, TVI_LAST);
		if (this->m_chem_ic.kinetics->type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sSolutionRTF.c_str());

		// init
		this->m_htiDDX = this->m_wndTreeCtrl.GetFirstVisibleItem();
	}

	if (this->m_htiDDX)
	{
		CString strItem = this->m_wndTreeCtrl.GetItemText(this->m_htiDDX);

		// PSZ_SOLUTION
		if (strItem.Compare(PSZ_SOLUTION) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_chem_ic.solution),
				static_cast<Cproperty*>(this->m_chem_icFixed.solution),
				static_cast<Cproperty*>(this->m_chem_icLinear.solution)
				);
			// if ((this->m_bNewModelWizard || this->m_bFlowOnlyWizard) && !this->m_bFlowOnly) {
			if (!this->m_bFlowOnly) {
				if (pDX->m_bSaveAndValidate && this->m_chem_ic.solution->type == UNDEFINED) {
					CString str;
					str.Format("A default \"%s\" must be defined.", PSZ_SOLUTION);
					::AfxMessageBox(str);
					pDX->Fail();
				}
			}
		}
		// PSZ_EQUILIBRIUM_PHASES
		else if (strItem.Compare(PSZ_EQUILIBRIUM_PHASES) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_chem_ic.equilibrium_phases),
				static_cast<Cproperty*>(this->m_chem_icFixed.equilibrium_phases),
				static_cast<Cproperty*>(this->m_chem_icLinear.equilibrium_phases)
				);
		}
		// PSZ_EXCHANGE
		else if (strItem.Compare(PSZ_EXCHANGE) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_chem_ic.exchange),
				static_cast<Cproperty*>(this->m_chem_icFixed.exchange),
				static_cast<Cproperty*>(this->m_chem_icLinear.exchange)
				);
		}
		// PSZ_SURFACE
		else if (strItem.Compare(PSZ_SURFACE) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_chem_ic.surface),
				static_cast<Cproperty*>(this->m_chem_icFixed.surface),
				static_cast<Cproperty*>(this->m_chem_icLinear.surface)
				);
		}
		// PSZ_GAS_PHASE
		else if (strItem.Compare(PSZ_GAS_PHASE) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_chem_ic.gas_phase),
				static_cast<Cproperty*>(this->m_chem_icFixed.gas_phase),
				static_cast<Cproperty*>(this->m_chem_icLinear.gas_phase)
				);
		}
		// PSZ_SOLID_SOLUTIONS
		else if (strItem.Compare(PSZ_SOLID_SOLUTIONS) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_chem_ic.solid_solutions),
				static_cast<Cproperty*>(this->m_chem_icFixed.solid_solutions),
				static_cast<Cproperty*>(this->m_chem_icLinear.solid_solutions)
				);
		}
		// PSZ_KINETICS
		else if (strItem.Compare(PSZ_KINETICS) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_chem_ic.kinetics),
				static_cast<Cproperty*>(this->m_chem_icFixed.kinetics),
				static_cast<Cproperty*>(this->m_chem_icLinear.kinetics)
				);
		}
		// ERROR
		else {
			ASSERT(FALSE);
		}
	}
}

BEGIN_MESSAGE_MAP(CChemICPropertyPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROP_TREE, OnTvnSelchangedPropTree)
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTvnSelchangingPropTree)
	//{{USER
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	ON_MESSAGE(UM_CHECKSTATECHANGE, OnUM_CheckStateChange)
	//}}USER
	ON_BN_CLICKED(IDC_SINGLE_VALUE_RADIO, OnBnClickedSingleValueRadio)
	ON_BN_CLICKED(IDC_LINEAR_INTERPOLATION_RADIO, OnBnClickedLinearInterpolationRadio)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CChemICPropertyPage message handlers

void CChemICPropertyPage::OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// Add your control notification handler code here

	// exchange property
	//
	this->m_htiDDX = pNMTreeView->itemNew.hItem;
	this->UpdateData(FALSE);

	// update property description
	//
	if (this->m_htiDDX) {
		CString strItem = this->m_wndTreeCtrl.GetItemText(this->m_htiDDX);

		// PSZ_SOLUTION
		if (strItem.Compare(PSZ_SOLUTION) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sSolutionRTF.c_str());
		}
		// PSZ_EQUILIBRIUM_PHASES
		else if (strItem.Compare(PSZ_EQUILIBRIUM_PHASES) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sEquilibriumPhasesRTF.c_str());
		}
		// PSZ_EXCHANGE
		else if (strItem.Compare(PSZ_EXCHANGE) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sExchangeRTF.c_str());
		}
		// PSZ_SURFACE
		else if (strItem.Compare(PSZ_SURFACE) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sSurfaceRTF.c_str());
		}
		// PSZ_GAS_PHASE
		else if (strItem.Compare(PSZ_GAS_PHASE) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sGasPhaseRTF.c_str());
		}
		// PSZ_SOLID_SOLUTIONS
		else if (strItem.Compare(PSZ_SOLID_SOLUTIONS) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sSolidSolutionsRTF.c_str());
		}
		// PSZ_KINETICS
		else if (strItem.Compare(PSZ_KINETICS) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sKineticsRTF.c_str());
		}
		// ERROR
		else {
			ASSERT(FALSE);
			this->m_wndRichEditCtrl.SetWindowText("");
		}
	}

	*pResult = 0;
}

void CChemICPropertyPage::OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// Add your control notification handler code here

	// validate itemOld
	//
	this->m_htiDDX = pNMTreeView->itemOld.hItem;
	if (!this->UpdateData(TRUE)) {
		// notify which control caused failure
		//
		CWnd* pFocus = CWnd::GetFocus();
		::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

		// disallow change
		//
		*pResult = TRUE;
		return;
	}

	*pResult = 0;
}

LRESULT CChemICPropertyPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	return 0;
}

LRESULT CChemICPropertyPage::OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hSelectedItem = this->m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hItemChanged = (HTREEITEM)lParam;

	// handle special case when hSelectedItem is the same
	// as hItemChanged
	if (hSelectedItem == hItemChanged) {
		// update current item
		//
		this->m_htiDDX = hSelectedItem;
		this->UpdateData(FALSE);
		return 0;
	}

	this->m_htiDDX = hSelectedItem;
	if (this->UpdateData(TRUE)) {
		// set selection on item that was checked
		// causes TVN_SELCHANGING and TVN_SELCHANGED
		//
		this->m_wndTreeCtrl.SelectItem(hItemChanged);
	}
	else {
		// UpdateData failed
		// notify which control caused the failure
		//
		return (LRESULT)CWnd::GetFocus();
	}

	return 0;
}

void CChemICPropertyPage::OnBnClickedSingleValueRadio()
{
	CGlobal::EnableLinearInterpolation(this, FALSE);
	CGlobal::EnableFixed(this, TRUE);
}

void CChemICPropertyPage::OnBnClickedLinearInterpolationRadio()
{
	CGlobal::EnableLinearInterpolation(this, TRUE);
	CGlobal::EnableFixed(this, FALSE);
}

BOOL CChemICPropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (this->m_bNewModelWizard)
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);

			// force a default solution if not flow only
			CNewModelWizard* pWizard = static_cast<CNewModelWizard*>(pSheet);
			this->SetFlowOnly((bool)pWizard->GetFlowOnly());
		}
		else if (this->m_bFlowOnlyWizard)
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
			this->SetFlowOnly(false);
		}
		else
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
		}
	}
	return bRet;
}

BOOL CChemICPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

// COMMENT: {6/2/2004 9:42:24 PM}	// Disable cancel if no apply button
// COMMENT: {6/2/2004 9:42:24 PM}	//
// COMMENT: {6/2/2004 9:42:24 PM}	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
// COMMENT: {6/2/2004 9:42:24 PM}	ASSERT_KINDOF(CPropertySheet, pSheet);
// COMMENT: {6/2/2004 9:42:24 PM}	if (pSheet->m_psh.dwFlags & PSH_NOAPPLYNOW) {
// COMMENT: {6/2/2004 9:42:24 PM}		pSheet->GetDlgItem(2)->EnableWindow(FALSE);
// COMMENT: {6/2/2004 9:42:24 PM}		pSheet->ModifyStyle(WS_SYSMENU, 0);
// COMMENT: {6/2/2004 9:42:24 PM}	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CChemICPropertyPage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CPropertyPage::OnPaint() for painting messages
	if (this->m_bFirstPaint)
	{
		this->m_bFirstPaint = false;

		if (this->m_bShowFlowOnlyMessage) {
			::AfxMessageBox("Since you have turned on solute transport you may want to review the default solution.");
			this->m_bShowFlowOnlyMessage = false;
		}
	}

}

BOOL CChemICPropertyPage::OnKillActive()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
}

LRESULT CChemICPropertyPage::OnWizardNext()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (!this->UpdateData(TRUE))
		{
			return -1; // return –1 to prevent the page from changing 
		}
	}
	return CPropertyPage::OnWizardNext();
}
