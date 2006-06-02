// BCFluxPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "BCFluxPropertyPage.h"

#include "Global.h"
#include "property.h"


// CBCFluxPropertyPage dialog

IMPLEMENT_DYNAMIC(CBCFluxPropertyPage, CPropertyPage)
CBCFluxPropertyPage::CBCFluxPropertyPage()
	: CPropertyPage(CBCFluxPropertyPage::IDD)
	, m_nStressPeriod(-1)
{
	// init properties
	//
	this->_SetProperties(this->m_bc);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sAssocSolutionRTF, IDR_BC_FLUX_ASSOC_SOL_RTF);
	CGlobal::LoadRTFString(this->m_sFluxRTF,          IDR_BC_FLUX_FLUX_RTF);
}

CBCFluxPropertyPage::~CBCFluxPropertyPage()
{
}

void CBCFluxPropertyPage::SetProperties(const CBC& r_bc)
{
	// BUGBUG is _SetProperties necessary?
	// also should this SetProperties be combined with SetStressPeriod
#ifdef _DEBUG
	ASSERT(this->m_nStressPeriod >= 1);
	r_bc.AssertValid(this->m_nStressPeriod);
#endif
	this->_SetProperties(r_bc);
}

void CBCFluxPropertyPage::_SetProperties(const CBC& r_bc)
{
	this->m_bc = r_bc;

	// fill empty properties
	//
// COMMENT: {2/23/2005 1:19:42 PM}	if (!this->m_bc.bc_flux) {
// COMMENT: {2/23/2005 1:19:42 PM}		this->m_bc.bc_flux = new Cproperty();
// COMMENT: {2/23/2005 1:19:42 PM}	}
// COMMENT: {2/23/2005 1:19:47 PM}	if (!this->m_bc.bc_solution) {
// COMMENT: {2/23/2005 1:19:47 PM}		this->m_bc.bc_solution = new Cproperty();
// COMMENT: {2/23/2005 1:19:47 PM}	}

	// copy to fixed and linear
	this->m_bcFixed = this->m_bcLinear = this->m_bc;
}

void CBCFluxPropertyPage::GetProperties(CBC& r_bc)
{
	r_bc = this->m_bc;

	// empty undefined properties
	//
// COMMENT: {2/23/2005 1:18:59 PM}	if (r_bc.bc_flux->type == UNDEFINED) {
// COMMENT: {2/23/2005 1:18:59 PM}		Cproperty::CopyProperty(&r_bc.bc_flux, 0);
// COMMENT: {2/23/2005 1:18:59 PM}	}
// COMMENT: {2/23/2005 1:18:59 PM}	if (r_bc.bc_solution->type == UNDEFINED) {
// COMMENT: {2/23/2005 1:18:59 PM}		Cproperty::CopyProperty(&r_bc.bc_solution, 0);
// COMMENT: {2/23/2005 1:18:59 PM}	}
#ifdef _DEBUG
	ASSERT(this->m_nStressPeriod >= 1);
	r_bc.AssertValid(this->m_nStressPeriod);
#endif
}

void CBCFluxPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_TREE, m_wndTreeCtrl);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	ASSERT(this->m_nStressPeriod >= 1);

	CGlobal::DDX_Face(pDX, this->m_bc);

	if (this->m_wndTreeCtrl.GetCount() == 0) {
		HTREEITEM hti;

		// Associated Solution
		hti = this->m_wndTreeCtrl.InsertItem(_T("Associated Solution"), TVI_ROOT, TVI_SORT);
// COMMENT: {2/23/2005 1:19:11 PM}		if (this->m_bc.bc_solution->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:19:11 PM}			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
// COMMENT: {2/23/2005 1:19:11 PM}		}

		// Flux
		hti = this->m_wndTreeCtrl.InsertItem(_T("Flux"), TVI_ROOT, TVI_SORT);
// COMMENT: {2/23/2005 1:19:17 PM}		if (this->m_bc.bc_flux->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:19:17 PM}			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
// COMMENT: {2/23/2005 1:19:17 PM}		}

		if (this->m_nStressPeriod > 1) {
			// disable face
			CGlobal::EnableFace(pDX, FALSE);
		}

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sAssocSolutionRTF.c_str());

		// init
		this->m_htiDDX = this->m_wndTreeCtrl.GetFirstVisibleItem();
	}

	if (this->m_htiDDX) {
		CString strItem = this->m_wndTreeCtrl.GetItemText(this->m_htiDDX);

		// Associated Solution
		if ( strItem.Compare(_T("Associated Solution")) == 0) {
// COMMENT: {2/23/2005 1:19:53 PM}			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
// COMMENT: {2/23/2005 1:19:53 PM}				static_cast<Cproperty*>(this->m_bc.bc_solution),
// COMMENT: {2/23/2005 1:19:53 PM}				static_cast<Cproperty*>(this->m_bcFixed.bc_solution),
// COMMENT: {2/23/2005 1:19:53 PM}				static_cast<Cproperty*>(this->m_bcLinear.bc_solution)
// COMMENT: {2/23/2005 1:19:53 PM}				);
			if (pDX->m_bSaveAndValidate) {
// COMMENT: {2/23/2005 1:19:21 PM}				if (this->m_bc.bc_solution->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:19:21 PM}					this->m_bc.bc_solution_type = ASSOCIATED;
// COMMENT: {2/23/2005 1:19:21 PM}				}
			}
		}
		// Flux
		else if (strItem.Compare(_T("Flux")) == 0) {
// COMMENT: {2/23/2005 1:20:02 PM}			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
// COMMENT: {2/23/2005 1:20:02 PM}				static_cast<Cproperty*>(this->m_bc.bc_flux),
// COMMENT: {2/23/2005 1:20:02 PM}				static_cast<Cproperty*>(this->m_bcFixed.bc_flux),
// COMMENT: {2/23/2005 1:20:02 PM}				static_cast<Cproperty*>(this->m_bcLinear.bc_flux)
// COMMENT: {2/23/2005 1:20:02 PM}				);
		}
		// ERROR
		else {
			ASSERT(FALSE);
		}
	}
	// bc_type
	if (pDX->m_bSaveAndValidate) {
		this->m_bc.bc_type = FLUX;
	}
}

BEGIN_MESSAGE_MAP(CBCFluxPropertyPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROP_TREE, OnTvnSelchangedPropTree)
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTvnSelchangingPropTree)
	//{{USER
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	ON_MESSAGE(UM_CHECKSTATECHANGE, OnUM_CheckStateChange)
	//}}USER
	ON_BN_CLICKED(IDC_SINGLE_VALUE_RADIO, OnBnClickedSingleValueRadio)
	ON_BN_CLICKED(IDC_LINEAR_INTERPOLATION_RADIO, OnBnClickedLinearInterpolationRadio)
END_MESSAGE_MAP()


// CBCFluxPropertyPage message handlers

void CBCFluxPropertyPage::OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult)
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

		// Associated Solution
		if ( strItem.Compare(_T("Associated Solution")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sAssocSolutionRTF.c_str());
		}
		// Flux
		else if (strItem.Compare(_T("Flux")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sFluxRTF.c_str());
		}
		// ERROR
		else {
			ASSERT(FALSE);
			this->m_wndRichEditCtrl.SetWindowText("");
		}
	}

	*pResult = 0;
}

void CBCFluxPropertyPage::OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult)
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

LRESULT CBCFluxPropertyPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	return 0;
}

LRESULT CBCFluxPropertyPage::OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam)
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

void CBCFluxPropertyPage::OnBnClickedSingleValueRadio()
{
	CGlobal::EnableLinearInterpolation(this, FALSE);
	CGlobal::EnableFixed(this, TRUE);
}

void CBCFluxPropertyPage::OnBnClickedLinearInterpolationRadio()
{
	CGlobal::EnableLinearInterpolation(this, TRUE);
	CGlobal::EnableFixed(this, FALSE);
}

BOOL CBCFluxPropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard()) {
		pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	}
	return bRet;
}
