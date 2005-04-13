// BCLeakyPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "BCLeakyPropertyPage.h"

#include "Global.h"
#include "property.h"

// CBCLeakyPropertyPage dialog

IMPLEMENT_DYNAMIC(CBCLeakyPropertyPage, CPropertyPage)
CBCLeakyPropertyPage::CBCLeakyPropertyPage()
	: CPropertyPage(CBCLeakyPropertyPage::IDD)
	, m_nStressPeriod(-1)
{
	// init properties
	//
	this->_SetProperties(this->m_bc);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sHeadRTF,          IDR_BC_LEAKY_HEAD_RTF);
	CGlobal::LoadRTFString(this->m_sThicknessRTF,     IDR_BC_LEAKY_THICKNESS_RTF);
	CGlobal::LoadRTFString(this->m_sHydCondRTF,       IDR_BC_LEAKY_HYD_COND_RTF);
	CGlobal::LoadRTFString(this->m_sAssocSolutionRTF, IDR_BC_LEAKY_ASSOC_SOL_RTF);
}

CBCLeakyPropertyPage::~CBCLeakyPropertyPage()
{
}

void CBCLeakyPropertyPage::SetProperties(const CBC& r_bc)
{
#ifdef _DEBUG
	ASSERT(this->m_nStressPeriod >= 1);
	r_bc.AssertValid(this->m_nStressPeriod);
#endif
	this->_SetProperties(r_bc);
}

void CBCLeakyPropertyPage::_SetProperties(const CBC& r_bc)
{
	this->m_bc = r_bc;

	// fill empty properties
	//

// COMMENT: {2/23/2005 1:17:20 PM}	// Head
// COMMENT: {2/23/2005 1:17:20 PM}	if (!this->m_bc.bc_head) {
// COMMENT: {2/23/2005 1:17:20 PM}		this->m_bc.bc_head = new Cproperty();
// COMMENT: {2/23/2005 1:17:20 PM}	}
	// Thickness
	if (!this->m_bc.bc_thick) {
		this->m_bc.bc_thick = new Cproperty();
	}
	// Hydraulic Conductivity
	if (!this->m_bc.bc_k) {
		this->m_bc.bc_k = new Cproperty();
	}
// COMMENT: {2/23/2005 1:17:20 PM}	// Associated Solution
// COMMENT: {2/23/2005 1:17:20 PM}	if (!this->m_bc.bc_solution) {
// COMMENT: {2/23/2005 1:17:20 PM}		this->m_bc.bc_solution = new Cproperty();
// COMMENT: {2/23/2005 1:17:20 PM}	}

	// copy to fixed and linear
	this->m_bcFixed = this->m_bcLinear = this->m_bc;
}

void CBCLeakyPropertyPage::GetProperties(CBC& r_bc)
{
	r_bc = this->m_bc;

	// empty undefined properties
	//

// COMMENT: {2/23/2005 1:16:47 PM}	// Head
// COMMENT: {2/23/2005 1:16:47 PM}	if (r_bc.bc_head->type == UNDEFINED) {
// COMMENT: {2/23/2005 1:16:47 PM}		Cproperty::CopyProperty(&r_bc.bc_head, 0);
// COMMENT: {2/23/2005 1:16:47 PM}	}
// COMMENT: {2/23/2005 1:16:47 PM}	// Thickness
// COMMENT: {2/23/2005 1:16:47 PM}	if (r_bc.bc_thick->type == UNDEFINED) {
// COMMENT: {2/23/2005 1:16:47 PM}		Cproperty::CopyProperty(&r_bc.bc_thick, 0);
// COMMENT: {2/23/2005 1:16:47 PM}	}
// COMMENT: {2/23/2005 1:16:47 PM}	// Hydraulic Conductivity
// COMMENT: {2/23/2005 1:16:47 PM}	if (r_bc.bc_k->type == UNDEFINED) {
// COMMENT: {2/23/2005 1:16:47 PM}		Cproperty::CopyProperty(&r_bc.bc_k, 0);
// COMMENT: {2/23/2005 1:16:47 PM}	}
// COMMENT: {2/23/2005 1:16:47 PM}	// Associated Solution
// COMMENT: {2/23/2005 1:16:47 PM}	if (r_bc.bc_solution->type == UNDEFINED) {
// COMMENT: {2/23/2005 1:16:47 PM}		Cproperty::CopyProperty(&r_bc.bc_solution, 0);
// COMMENT: {2/23/2005 1:16:47 PM}	}
// COMMENT: {2/23/2005 1:16:47 PM}	else {
// COMMENT: {2/23/2005 1:16:47 PM}		r_bc.bc_solution_type = ASSOCIATED;
// COMMENT: {2/23/2005 1:16:47 PM}	}

#ifdef _DEBUG
	ASSERT(this->m_nStressPeriod >= 1);
	r_bc.AssertValid(this->m_nStressPeriod);
#endif
}


void CBCLeakyPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_TREE, m_wndTreeCtrl);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	ASSERT(this->m_nStressPeriod >= 1);

	CGlobal::DDX_Face(pDX, this->m_bc);

	if (this->m_wndTreeCtrl.GetCount() == 0) {
		HTREEITEM hti;

		// Head
		hti = this->m_wndTreeCtrl.InsertItem(_T("Head"), TVI_ROOT, TVI_SORT);
// COMMENT: {2/23/2005 1:16:57 PM}		if (this->m_bc.bc_head->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:16:57 PM}			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
// COMMENT: {2/23/2005 1:16:57 PM}		}

		// Thickness
		hti = this->m_wndTreeCtrl.InsertItem(_T("Thickness"), TVI_ROOT, TVI_SORT);
		if (this->m_nStressPeriod > 1) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_INDETERMINATE);
		}
		else if (this->m_bc.bc_thick->type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// Hydraulic Conductivity
		hti = this->m_wndTreeCtrl.InsertItem(_T("Hydraulic Conductivity"), TVI_ROOT, TVI_SORT);
		if (this->m_nStressPeriod > 1) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_INDETERMINATE);
		}
		else if (this->m_bc.bc_k->type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// Associated Solution
		hti = this->m_wndTreeCtrl.InsertItem(_T("Associated Solution"), TVI_ROOT, TVI_SORT);
// COMMENT: {2/23/2005 1:17:02 PM}		if (this->m_bc.bc_solution->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:17:02 PM}			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
// COMMENT: {2/23/2005 1:17:02 PM}		}

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

		// Head
		if ( strItem.Compare(_T("Head")) == 0) {
// COMMENT: {2/23/2005 1:17:27 PM}			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
// COMMENT: {2/23/2005 1:17:27 PM}				static_cast<Cproperty*>(this->m_bc.bc_head),
// COMMENT: {2/23/2005 1:17:27 PM}				static_cast<Cproperty*>(this->m_bcFixed.bc_head),
// COMMENT: {2/23/2005 1:17:27 PM}				static_cast<Cproperty*>(this->m_bcLinear.bc_head)
// COMMENT: {2/23/2005 1:17:27 PM}				);
		}
		// Thickness
		else if ( strItem.Compare(_T("Thickness")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_bc.bc_thick),
				static_cast<Cproperty*>(this->m_bcFixed.bc_thick),
				static_cast<Cproperty*>(this->m_bcLinear.bc_thick)
				);
		}
		// Hydraulic Conductivity
		else if ( strItem.Compare(_T("Hydraulic Conductivity")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_bc.bc_k),
				static_cast<Cproperty*>(this->m_bcFixed.bc_k),
				static_cast<Cproperty*>(this->m_bcLinear.bc_k)
				);
		}
		// Associated Solution
		else if ( strItem.Compare(_T("Associated Solution")) == 0) {
// COMMENT: {2/23/2005 1:17:46 PM}			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
// COMMENT: {2/23/2005 1:17:46 PM}				static_cast<Cproperty*>(this->m_bc.bc_solution),
// COMMENT: {2/23/2005 1:17:46 PM}				static_cast<Cproperty*>(this->m_bcFixed.bc_solution),
// COMMENT: {2/23/2005 1:17:46 PM}				static_cast<Cproperty*>(this->m_bcLinear.bc_solution)
// COMMENT: {2/23/2005 1:17:46 PM}				);
			if (pDX->m_bSaveAndValidate) {
// COMMENT: {2/23/2005 1:17:09 PM}				if (this->m_bc.bc_solution->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:17:09 PM}					this->m_bc.bc_solution_type = ASSOCIATED;
// COMMENT: {2/23/2005 1:17:09 PM}				}
			}
		}
		// ERROR
		else {
			ASSERT(FALSE);
		}
	}
	// bc_type
	if (pDX->m_bSaveAndValidate) {
		this->m_bc.bc_type = LEAKY;
	}
}


BEGIN_MESSAGE_MAP(CBCLeakyPropertyPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROP_TREE, OnTvnSelchangedPropTree)
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTvnSelchangingPropTree)
	//{{USER
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	ON_MESSAGE(UM_CHECKSTATECHANGE, OnUM_CheckStateChange)
	//}}USER
	ON_BN_CLICKED(IDC_SINGLE_VALUE_RADIO, OnBnClickedSingleValueRadio)
	ON_BN_CLICKED(IDC_LINEAR_INTERPOLATION_RADIO, OnBnClickedLinearInterpolationRadio)
END_MESSAGE_MAP()


// CBCLeakyPropertyPage message handlers

void CBCLeakyPropertyPage::OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult)
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
		// Thickness
		else if ( strItem.Compare(_T("Thickness")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sThicknessRTF.c_str());
		}
		// Hydraulic Conductivity
		else if ( strItem.Compare(_T("Hydraulic Conductivity")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sHydCondRTF.c_str());
		}
		// Associated Solution
		else if ( strItem.Compare(_T("Associated Solution")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sAssocSolutionRTF.c_str());
		}
		// ERROR
		else {
			ASSERT(FALSE);
			this->m_wndRichEditCtrl.SetWindowText("");
		}
	}

	*pResult = 0;
}

void CBCLeakyPropertyPage::OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult)
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

LRESULT CBCLeakyPropertyPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	return 0;
}

LRESULT CBCLeakyPropertyPage::OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam)
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

void CBCLeakyPropertyPage::OnBnClickedSingleValueRadio()
{
	CGlobal::EnableLinearInterpolation(this, FALSE);
	CGlobal::EnableFixed(this, TRUE);
}

void CBCLeakyPropertyPage::OnBnClickedLinearInterpolationRadio()
{
	CGlobal::EnableLinearInterpolation(this, TRUE);
	CGlobal::EnableFixed(this, FALSE);
}

BOOL CBCLeakyPropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard()) {
		pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	}
	return bRet;
}
