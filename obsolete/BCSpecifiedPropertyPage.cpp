// BCSpecifiedPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "BCSpecifiedPropertyPage.h"

#include "Global.h"
#include "property.h"

// CBCSpecifiedPropertyPage dialog

IMPLEMENT_DYNAMIC(CBCSpecifiedPropertyPage, CPropertyPage)
CBCSpecifiedPropertyPage::CBCSpecifiedPropertyPage()
	: CPropertyPage(CBCSpecifiedPropertyPage::IDD)
	, m_nStressPeriod(-1)
{
	// init properties
	//
	this->_SetProperties(this->m_bc);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sHeadRTF,     IDR_BC_SPECIFIED_HEAD_RTF);
	CGlobal::LoadRTFString(this->m_sSolutionRTF, IDR_BC_SPECIFIED_SOL_RTF);
}

CBCSpecifiedPropertyPage::~CBCSpecifiedPropertyPage()
{
}

void CBCSpecifiedPropertyPage::SetProperties(const CBC& r_bc)
{
#ifdef _DEBUG
	ASSERT(this->m_nStressPeriod >= 1);
	r_bc.AssertValid(this->m_nStressPeriod);
#endif
	this->_SetProperties(r_bc);
}

void CBCSpecifiedPropertyPage::_SetProperties(const CBC& r_bc)
{
	this->m_bc = r_bc;

// COMMENT: {4/11/2005 4:41:39 PM}	// fill empty properties
// COMMENT: {4/11/2005 4:41:39 PM}	//
// COMMENT: {4/11/2005 4:41:39 PM}
// COMMENT: {4/11/2005 4:41:39 PM}	// Head
// COMMENT: {4/11/2005 4:41:39 PM}	if (!this->m_bc.bc_head) {
// COMMENT: {4/11/2005 4:41:39 PM}		this->m_bc.bc_head = new CTimeSeries<Cproperty>;
// COMMENT: {4/11/2005 4:41:39 PM}	}
// COMMENT: {4/11/2005 4:41:39 PM}	// Solution (Fixed or Associated)
// COMMENT: {4/11/2005 4:41:39 PM}	if (!this->m_bc.bc_solution) {
// COMMENT: {4/11/2005 4:41:39 PM}		this->m_bc.bc_solution = new CTimeSeries<Cproperty>;
// COMMENT: {4/11/2005 4:41:39 PM}	}

	// copy to fixed and linear
	this->m_bcFixed = this->m_bcLinear = this->m_bc;
}

void CBCSpecifiedPropertyPage::GetProperties(CBC& r_bc)
{
	r_bc = this->m_bc;

	// empty undefined properties
	//

// COMMENT: {2/23/2005 1:14:47 PM}	// Head
// COMMENT: {2/23/2005 1:14:47 PM}	if (r_bc.bc_head->type == UNDEFINED) {
// COMMENT: {2/23/2005 1:14:47 PM}		Cproperty::CopyProperty(&r_bc.bc_head, 0);
// COMMENT: {2/23/2005 1:14:47 PM}	}
// COMMENT: {2/23/2005 1:14:47 PM}	// Solution (Fixed or Associated)
// COMMENT: {2/23/2005 1:14:47 PM}	if (r_bc.bc_solution->type == UNDEFINED) {
// COMMENT: {2/23/2005 1:14:47 PM}		Cproperty::CopyProperty(&r_bc.bc_solution, 0);
// COMMENT: {2/23/2005 1:14:47 PM}	}
#ifdef _DEBUG
	ASSERT(this->m_nStressPeriod >= 1);
	r_bc.AssertValid(this->m_nStressPeriod);
#endif
}


void CBCSpecifiedPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_TREE, m_wndTreeCtrl);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	ASSERT(this->m_nStressPeriod >= 1);

	if (this->m_wndTreeCtrl.GetCount() == 0) {
		HTREEITEM hti;

		// Head
		hti = this->m_wndTreeCtrl.InsertItem(_T("Head"), TVI_ROOT, TVI_SORT);
// COMMENT: {2/23/2005 1:14:57 PM}		if (this->m_bc.bc_head->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:14:57 PM}			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
// COMMENT: {2/23/2005 1:14:57 PM}		}

		// Solution
		hti = this->m_wndTreeCtrl.InsertItem(_T("Solution"), TVI_ROOT, TVI_SORT);
// COMMENT: {2/23/2005 1:14:59 PM}		if (this->m_bc.bc_solution->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:14:59 PM}			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
// COMMENT: {2/23/2005 1:14:59 PM}		}

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sSolutionRTF.c_str());

		// init
		this->m_htiDDX = this->m_wndTreeCtrl.GetFirstVisibleItem();
	}

	if (this->m_htiDDX) {
		CString strItem = this->m_wndTreeCtrl.GetItemText(this->m_htiDDX);

		// Head
		if (strItem.Compare(_T("Head")) == 0) {
// COMMENT: {2/23/2005 1:15:59 PM}			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
// COMMENT: {2/23/2005 1:15:59 PM}				static_cast<Cproperty*>(this->m_bc.bc_head),
// COMMENT: {2/23/2005 1:15:59 PM}				static_cast<Cproperty*>(this->m_bcFixed.bc_head),
// COMMENT: {2/23/2005 1:15:59 PM}				static_cast<Cproperty*>(this->m_bcLinear.bc_head)
// COMMENT: {2/23/2005 1:15:59 PM}				);
			if (!pDX->m_bSaveAndValidate) {
				// disable solution radio buttons
				//
				pDX->m_pDlgWnd->GetDlgItem(IDC_SOL_TYPE_STATIC)->EnableWindow(FALSE);
				pDX->m_pDlgWnd->GetDlgItem(IDC_ASSOC_RADIO)->EnableWindow(FALSE);
				pDX->m_pDlgWnd->GetDlgItem(IDC_FIXED_RADIO)->EnableWindow(FALSE);
			}
		}
		// Solution
		else if (strItem.Compare(_T("Solution")) == 0) {
// COMMENT: {2/23/2005 1:16:13 PM}			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
// COMMENT: {2/23/2005 1:16:13 PM}				static_cast<Cproperty*>(this->m_bc.bc_solution),
// COMMENT: {2/23/2005 1:16:13 PM}				static_cast<Cproperty*>(this->m_bcFixed.bc_solution),
// COMMENT: {2/23/2005 1:16:13 PM}				static_cast<Cproperty*>(this->m_bcLinear.bc_solution)
// COMMENT: {2/23/2005 1:16:13 PM}				);
// COMMENT: {2/25/2005 7:27:19 PM}			CGlobal::DDX_AssocFixed(pDX, &this->m_wndTreeCtrl, this->m_htiDDX, this->m_bc);
			//{{
			if (this->m_nStressPeriod > 1) {
				// cannot change solution type
				this->GetDlgItem(IDC_SOL_TYPE_STATIC)->EnableWindow(FALSE);
				this->GetDlgItem(IDC_ASSOC_RADIO)->EnableWindow(FALSE);
				this->GetDlgItem(IDC_FIXED_RADIO)->EnableWindow(FALSE);
			}
			//}}
		}
		// ERROR
		else {
			ASSERT(FALSE);
		}
	}
	// bc_type
	if (pDX->m_bSaveAndValidate) {
		this->m_bc.bc_type = SPECIFIED;
	}
}


BEGIN_MESSAGE_MAP(CBCSpecifiedPropertyPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROP_TREE, OnTvnSelchangedPropTree)
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTvnSelchangingPropTree)
	//{{USER
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	ON_MESSAGE(UM_CHECKSTATECHANGE, OnUM_CheckStateChange)
	//}}USER
	ON_BN_CLICKED(IDC_SINGLE_VALUE_RADIO, OnBnClickedSingleValueRadio)
	ON_BN_CLICKED(IDC_LINEAR_INTERPOLATION_RADIO, OnBnClickedLinearInterpolationRadio)
END_MESSAGE_MAP()


// CBCSpecifiedPropertyPage message handlers

void CBCSpecifiedPropertyPage::OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult)
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

		// Head
		if ( strItem.Compare(_T("Head")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sHeadRTF.c_str());
		}
		// Solution
		else if ( strItem.Compare(_T("Solution")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sSolutionRTF.c_str());
		}
		// ERROR
		else {
			ASSERT(FALSE);
			this->m_wndRichEditCtrl.SetWindowText("");
		}
	}

	*pResult = 0;
}

void CBCSpecifiedPropertyPage::OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult)
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

LRESULT CBCSpecifiedPropertyPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	return 0;
}

LRESULT CBCSpecifiedPropertyPage::OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam)
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

void CBCSpecifiedPropertyPage::OnBnClickedSingleValueRadio()
{
	CGlobal::EnableLinearInterpolation(this, FALSE);
	CGlobal::EnableFixed(this, TRUE);
}

void CBCSpecifiedPropertyPage::OnBnClickedLinearInterpolationRadio()
{
	CGlobal::EnableLinearInterpolation(this, TRUE);
	CGlobal::EnableFixed(this, FALSE);
}

BOOL CBCSpecifiedPropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard()) {
		pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	}
	return bRet;
}
