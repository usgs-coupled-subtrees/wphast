// TestPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "TestPropertyPage.h"

#include <windowsx.h> // GET_X_LPARAM GET_Y_LPARAM


// CTestPropertyPage dialog

IMPLEMENT_DYNAMIC(CTestPropertyPage, CPropertyPage)
CTestPropertyPage::CTestPropertyPage()
	: CPropertyPage(CTestPropertyPage::IDD)
	, m_dValue1Active(1)
	, m_dValue2Active(2)
	, m_dValue1Kx(3)
	, m_dValue2Kx(4)
	, m_dValue1Porosity(5)
	, m_dValue2Porosity(6)
	, m_htiValidate(0)
{
}

CTestPropertyPage::~CTestPropertyPage()
{
}

void CTestPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_wndListBox);
	DDX_Control(pDX, IDC_TREE1, m_wndTreeCtrl);
	DDX_Control(pDX, IDC_LIST2, m_wndListCtrl);

	if (this->m_wndListBox.GetCount() == 0) {
		// go through properties and set checkmarks
		int nIndex;

		nIndex = this->m_wndListBox.AddString(_T(" Active"));
		nIndex = this->m_wndListBox.AddString(_T(" Kx"));
		nIndex = this->m_wndListBox.AddString(_T(" Porosity"));
	}

	if (this->m_wndTreeCtrl.GetCount() == 0) {
		HTREEITEM hti;

		// for some reason this is necessary
		//
		this->m_wndTreeCtrl.ModifyStyle(TVS_CHECKBOXES, 0);
		this->m_wndTreeCtrl.ModifyStyle(0, TVS_CHECKBOXES);

		hti = this->m_wndTreeCtrl.InsertItem("Active");
		hti = this->m_wndTreeCtrl.InsertItem(_T("Kx"));
		hti = this->m_wndTreeCtrl.InsertItem("Porosity");
	}

	if (this->m_wndListCtrl.GetItemCount() == 0) {
		this->m_wndListCtrl.SetExtendedStyle(LVS_EX_CHECKBOXES);

		this->m_wndListCtrl.InsertItem(0, "Active");
		this->m_wndListCtrl.InsertItem(1, "Kx");
		this->m_wndListCtrl.InsertItem(2, "Porosity");
	}

	if (pDX->m_bSaveAndValidate) {
		if (this->m_htiValidate && this->m_wndTreeCtrl.GetCheck(this->m_htiValidate)) {
			CString strItem = this->m_wndTreeCtrl.GetItemText(this->m_htiValidate);
			if (strItem.Compare(_T("Active")) == 0) {
				DDX_Text(pDX, IDC_VALUE1_EDIT, m_dValue1Active);
				DDX_Text(pDX, IDC_VALUE2_EDIT, m_dValue2Active);
			}
			else if (strItem.Compare(_T("Kx")) == 0) {
				DDX_Text(pDX, IDC_VALUE1_EDIT, m_dValue1Kx);
				DDX_Text(pDX, IDC_VALUE2_EDIT, m_dValue2Kx);
			}
			else if (strItem.Compare(_T("Porosity")) == 0) {
				DDX_Text(pDX, IDC_VALUE1_EDIT, m_dValue1Porosity);
				DDX_Text(pDX, IDC_VALUE2_EDIT, m_dValue2Porosity);
			}
			else {
				ASSERT(FALSE);
			}
		}
	}
	else {
		if (this->m_htiValidate) {
			CString strItem = this->m_wndTreeCtrl.GetItemText(this->m_htiValidate);
			if (strItem.Compare(_T("Active")) == 0) {
				DDX_Text(pDX, IDC_VALUE1_EDIT, m_dValue1Active);
				DDX_Text(pDX, IDC_VALUE2_EDIT, m_dValue2Active);
			}
			else if (strItem.Compare(_T("Kx")) == 0) {
				DDX_Text(pDX, IDC_VALUE1_EDIT, m_dValue1Kx);
				DDX_Text(pDX, IDC_VALUE2_EDIT, m_dValue2Kx);
			}
			else if (strItem.Compare(_T("Porosity")) == 0) {
				DDX_Text(pDX, IDC_VALUE1_EDIT, m_dValue1Porosity);
				DDX_Text(pDX, IDC_VALUE2_EDIT, m_dValue2Porosity);
			}
			else {
				ASSERT(FALSE);
			}
		}
		else {
			CString strEmpty;
			DDX_Text(pDX, IDC_VALUE1_EDIT, strEmpty);
			DDX_Text(pDX, IDC_VALUE2_EDIT, strEmpty);
		}
	}
}


BEGIN_MESSAGE_MAP(CTestPropertyPage, CPropertyPage)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, OnNMClickTree1)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE1, OnTvnKeydownTree1)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE1, OnTvnSelchangingTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	//{{USER
	ON_MESSAGE(UM_CHECKSTATECHANGE, OnUM_CheckStateChange)
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	//}}USER
END_MESSAGE_MAP()


// CTestPropertyPage message handlers

void CTestPropertyPage::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//{{
	TVHITTESTINFO ht = {0};

	if((pNMHDR->code == NM_CLICK) && (pNMHDR->idFrom == IDC_TREE1))
	{
		DWORD dwpos = ::GetMessagePos();

		// include <windowsx.h> and <windows.h> header files
		ht.pt.x = GET_X_LPARAM(dwpos);
		ht.pt.y = GET_Y_LPARAM(dwpos);
		::MapWindowPoints(HWND_DESKTOP, pNMHDR->hwndFrom, &ht.pt, 1);

		TreeView_HitTest(pNMHDR->hwndFrom, &ht);

		if(TVHT_ONITEMSTATEICON & ht.flags)
		{
			// ::PostMessage(this->GetSafeHwnd()/*hWnd*/, UM_CHECKSTATECHANGE, 0, (LPARAM)ht.hItem);
			if (CWnd* pFocus = (CWnd*)::SendMessage(this->GetSafeHwnd()/*hWnd*/, UM_CHECKSTATECHANGE, 0, (LPARAM)ht.hItem)) {
				///ASSERT(this->GetDlgItem(IDC_EDIT1) == pFocus);
				///pFocus->SetFocus();
				*pResult = 1; // disallow check change
				::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)ht.hItem);
				return;
			}
		}
	}

	//}}
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

LRESULT CTestPropertyPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// ::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)ht.hItem);
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	return 0;
}

LRESULT CTestPropertyPage::OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam)
{
	// handle special case when hSelectedItem is the same
	// as hItemChanged
	TRACE("OnUM_CheckStateChange\n");
	HTREEITEM hSelectedItem = this->m_wndTreeCtrl.GetSelectedItem();
	CString strItem = this->m_wndTreeCtrl.GetItemText(hSelectedItem);
	TRACE("Current Selection = %s\n", strItem);
	HTREEITEM hItemChanged = (HTREEITEM)lParam;

	// handle special case when hSelectedItem is the same
	// as hItemChanged
	if (hSelectedItem == hItemChanged) {
		return 0;
	}	

	if (this->m_wndTreeCtrl.GetCheck(hSelectedItem)) {
		m_htiValidate = hSelectedItem;
		if (this->UpdateData(TRUE)) {
			this->m_wndTreeCtrl.SelectItem(hItemChanged);
			return 0;
		}
		else {
			///ASSERT(this->GetDlgItem(IDC_EDIT1) == CWnd::GetFocus());
			return (LRESULT)CWnd::GetFocus();
		}
	}
	this->m_wndTreeCtrl.SelectItem(hItemChanged);
	return 0;
}

void CTestPropertyPage::OnTvnKeydownTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CTestPropertyPage::OnTvnSelchangingTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// Add your control notification handler code here
	//{{
	TRACE("Selchanging\n");
	this->m_htiValidate = pNMTreeView->itemOld.hItem;
	if (!this->UpdateData(TRUE)) {
		*pResult = TRUE;
		return;
	}

	//this->m_htiValidate = pNMTreeView->itemNew.hItem;
	//this->UpdateData(FALSE);
	//}}
	*pResult = 0;
}

void CTestPropertyPage::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	//{{
	TRACE("Selchanged\n");
	this->m_htiValidate = pNMTreeView->itemNew.hItem;
	this->UpdateData(FALSE);
	//}}
	// Add your control notification handler code here
	*pResult = 0;
}
