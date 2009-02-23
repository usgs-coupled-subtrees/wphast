// ICHeadPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ICHeadPropertyPage.h"

#include "Global.h"
#include "property.h"
#include "NewModelWizard.h"


// CICHeadPropertyPage dialog
static const char s_szHead[] = "Head";

IMPLEMENT_DYNAMIC(CICHeadPropertyPage, CPropertyPage)
CICHeadPropertyPage::CICHeadPropertyPage()
	: CPropertyPage(CICHeadPropertyPage::IDD)
	, m_htiDDX(0)
{
	this->CommonConstruct();
}

CICHeadPropertyPage::CICHeadPropertyPage(UINT nIDCaption)
	: CPropertyPage(CICHeadPropertyPage::IDD, nIDCaption)
	, m_htiDDX(0)
{
	this->CommonConstruct();
}

CICHeadPropertyPage::CICHeadPropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CICHeadPropertyPage::IDD_INTERIOR, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
	, m_htiDDX(0)
{
	this->CommonConstruct();
}

void CICHeadPropertyPage::CommonConstruct(void)
{
	// fill empty properties
	//
	this->SetProperties(this->m_headIC);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sMaskRTF, IDR_IC_HEAD_MASK_RTF);
	CGlobal::LoadRTFString(this->m_sHeadRTF, IDR_IC_HEAD_HEAD_RTF);
}

CICHeadPropertyPage::~CICHeadPropertyPage()
{
}

void CICHeadPropertyPage::GetProperties(CHeadIC& r_headIC)const
{
	r_headIC = this->m_headIC;

	// delete inactive properties
	//
	if (r_headIC.head->type == PROP_UNDEFINED) {
		Cproperty::CopyProperty(&r_headIC.head, 0);
	}
}

void CICHeadPropertyPage::SetProperties(const CHeadIC& r_headIC)
{
	// copy grid_elt
	this->m_headIC = r_headIC;
	
	// fill empty properties
	//
	if (this->m_headIC.head == 0) {
		this->m_headIC.head = new Cproperty();
	}

	// copy to fixed and linear
	//
	this->m_headICFixed = this->m_headICLinear = this->m_headIC;
}

void CICHeadPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_TREE, m_wndTreeCtrl);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	if (this->m_wndTreeCtrl.GetCount() == 0) {
		HTREEITEM hti;

		// head
		hti = this->m_wndTreeCtrl.InsertItem(s_szHead, TVI_ROOT, TVI_LAST);
		if (this->m_headIC.head->type != PROP_UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sHeadRTF.c_str());

		// init
		this->m_htiDDX = this->m_wndTreeCtrl.GetFirstVisibleItem();
	}

	if (this->m_htiDDX) {
		CString strItem = this->m_wndTreeCtrl.GetItemText(this->m_htiDDX);

		// head
		if (strItem.Compare(s_szHead) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_headIC.head),
				static_cast<Cproperty*>(this->m_headICFixed.head),
				static_cast<Cproperty*>(this->m_headICLinear.head)
				);
		}
	}
}


BEGIN_MESSAGE_MAP(CICHeadPropertyPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROP_TREE, OnTvnSelchangedPropTree)
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTvnSelchangingPropTree)
	//{{USER
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	ON_MESSAGE(UM_CHECKSTATECHANGE, OnUM_CheckStateChange)
	//}}USER
	ON_BN_CLICKED(IDC_SINGLE_VALUE_RADIO, OnBnClickedSingleValueRadio)
	ON_BN_CLICKED(IDC_LINEAR_INTERPOLATION_RADIO, OnBnClickedLinearInterpolationRadio)
END_MESSAGE_MAP()


// CICHeadPropertyPage message handlers

void CICHeadPropertyPage::OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult)
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
		if (strItem.Compare(s_szHead) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sHeadRTF.c_str());
		}
		// ERROR
		else {
			ASSERT(FALSE);
			this->m_wndRichEditCtrl.SetWindowText("");
		}
	}

	*pResult = 0;
}

void CICHeadPropertyPage::OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult)
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

LRESULT CICHeadPropertyPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	return 0;
}

LRESULT CICHeadPropertyPage::OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam)
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

void CICHeadPropertyPage::OnBnClickedSingleValueRadio()
{
	CGlobal::EnableLinearInterpolation(this, FALSE);
	CGlobal::EnableFixed(this, TRUE);
}

void CICHeadPropertyPage::OnBnClickedLinearInterpolationRadio()
{
	CGlobal::EnableLinearInterpolation(this, TRUE);
	CGlobal::EnableFixed(this, FALSE);
}

BOOL CICHeadPropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard()) {
		if (pSheet->IsKindOf(RUNTIME_CLASS(CNewModelWizard))) {
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
		}
		else {
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
		}
	}
	return bRet;
}

BOOL CICHeadPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

// COMMENT: {6/2/2004 9:41:42 PM}	// Disable cancel if no apply button
// COMMENT: {6/2/2004 9:41:42 PM}	//
// COMMENT: {6/2/2004 9:41:42 PM}	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
// COMMENT: {6/2/2004 9:41:42 PM}	ASSERT_KINDOF(CPropertySheet, pSheet);
// COMMENT: {6/2/2004 9:41:42 PM}	if (pSheet->m_psh.dwFlags & PSH_NOAPPLYNOW) {
// COMMENT: {6/2/2004 9:41:42 PM}		pSheet->GetDlgItem(2)->EnableWindow(FALSE);
// COMMENT: {6/2/2004 9:41:42 PM}		pSheet->ModifyStyle(WS_SYSMENU, 0);
// COMMENT: {6/2/2004 9:41:42 PM}	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CICHeadPropertyPage::OnKillActive()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
}

LRESULT CICHeadPropertyPage::OnWizardNext()
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
