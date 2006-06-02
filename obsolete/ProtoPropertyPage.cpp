// ProtoPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ProtoPropertyPage.h"

#include "Global.h"

#include <windowsx.h>


// CProtoPropertyPage dialog

IMPLEMENT_DYNAMIC(CProtoPropertyPage, CPropertyPage)
CProtoPropertyPage::CProtoPropertyPage()
	: CPropertyPage(CProtoPropertyPage::IDD)
	, m_htiDDX(0)
{
	this->SetProperties(this->m_propActive, this->m_propKx, this->m_propKy, this->m_propPorosity);
}

CProtoPropertyPage::~CProtoPropertyPage()
{
}

void CProtoPropertyPage::SetProperties(const Cproperty& active, const Cproperty& kx, const Cproperty& ky, const Cproperty& porosity)
{
	this->m_propActive = this->m_propActiveFixed = this->m_propActiveLinear = active;
	this->m_propKx = this->m_propKxFixed = this->m_propKxLinear = kx;
	this->m_propKy = this->m_propKyFixed = this->m_propKyLinear = ky;
	this->m_propPorosity = this->m_propPorosityFixed = this->m_propPorosityLinear = porosity;
}

void CProtoPropertyPage::GetProperties(Cproperty& active, Cproperty& kx, Cproperty& ky, Cproperty& porosity)
{
}

void CProtoPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_TREE, m_wndTreeCtrl);

	if (this->m_wndTreeCtrl.GetCount() == 0) {
		HTREEITEM hti;

		// for some reason this is necessary
		// for the checkboxes
		this->m_wndTreeCtrl.ModifyStyle(TVS_CHECKBOXES, 0);
		this->m_wndTreeCtrl.ModifyStyle(0, TVS_CHECKBOXES);

		// active
		hti = this->m_wndTreeCtrl.InsertItem("Active");
		if (this->m_propActive.type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, TRUE);
		}

		// Kx
		hti = this->m_wndTreeCtrl.InsertItem(_T("Kx"));
		if (this->m_propKx.type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, TRUE);
		}

		// Ky
		hti = this->m_wndTreeCtrl.InsertItem(_T("Ky"));
		if (this->m_propKy.type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, TRUE);
		}

		// Porosity
		hti = this->m_wndTreeCtrl.InsertItem("Porosity");
		if (this->m_propPorosity.type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, TRUE);
		}
	}

	//if (this->m_htiDDX) {
	//	CString strItem = this->m_wndTreeCtrl.GetItemText(this->m_htiDDX);
	//	if (strItem.Compare(_T("Active")) == 0) {
	//		DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX, this->m_propActive, this->m_propActiveFixed, this->m_propActiveLinear);
	//	}
	//	else if (strItem.Compare(_T("Kx")) == 0) {
	//		DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX, this->m_propKx, this->m_propKxFixed, this->m_propKxLinear);
	//	}
	//	else if (strItem.Compare(_T("Ky")) == 0) {
	//		DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX, this->m_propKy, this->m_propKyFixed, this->m_propKyLinear);
	//	}
	//	else if (strItem.Compare(_T("Porosity")) == 0) {
	//		DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX, this->m_propPorosity, this->m_propPorosityFixed, this->m_propPorosityLinear);
	//	}
	//	else {
	//		ASSERT(FALSE);
	//	}
	//}

}


BEGIN_MESSAGE_MAP(CProtoPropertyPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTvnSelchangingPropTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROP_TREE, OnTvnSelchangedPropTree)
	ON_NOTIFY(NM_CLICK, IDC_PROP_TREE, OnNMClickPropTree)
	//{{USER
	ON_MESSAGE(UM_CHECKSTATECHANGE, OnUM_CheckStateChange)
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	//}}USER
	ON_NOTIFY(TVN_KEYDOWN, IDC_PROP_TREE, OnTvnKeydownPropTree)
	ON_BN_CLICKED(IDC_SINGLE_VALUE_RADIO, OnBnClickedSingleValueRadio)
	ON_BN_CLICKED(IDC_LINEAR_INTERPOLATION_RADIO, OnBnClickedLinearInterpolationRadio)
END_MESSAGE_MAP()


// CProtoPropertyPage message handlers

void CProtoPropertyPage::OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRACE("OnTvnSelchangingPropTree\n");

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

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
	*pResult = 0; // allow change
}

void CProtoPropertyPage::OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRACE("OnTvnSelchangedPropTree\n");

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// update itemNew
	//
	this->m_htiDDX = pNMTreeView->itemNew.hItem;
	this->UpdateData(FALSE);
	*pResult = 0;
}

void CProtoPropertyPage::OnNMClickPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRACE("OnNMClickPropTree\n");

	TVHITTESTINFO ht = {0};

	// verify notification
	//
	ASSERT((pNMHDR->code == NM_CLICK) && (pNMHDR->idFrom == this->m_wndTreeCtrl.GetDlgCtrlID()));

	// get mouse position when NM_CLICK occured
	//
	DWORD dwpos = ::GetMessagePos();

	// convert to treectrl points
	// include <windowsx.h> // GET_X_LPARAM GET_Y_LPARAM
	// 
	ht.pt.x = GET_X_LPARAM(dwpos);
	ht.pt.y = GET_Y_LPARAM(dwpos);
	::MapWindowPoints(HWND_DESKTOP, pNMHDR->hwndFrom, &ht.pt, 1);

	// check if click occured within the checkbox
	//
	this->m_wndTreeCtrl.HitTest(&ht);
	if(TVHT_ONITEMSTATEICON & ht.flags)
	{
		// see OnUM_CheckStateChange
		//
		if (CWnd* pFocus = (CWnd*)::SendMessage(this->GetSafeHwnd(), UM_CHECKSTATECHANGE, 0, (LPARAM)ht.hItem)) {
			// notify which control caused failure
			//
			::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);
			*pResult = 1; // disallow check change
			return;
		}
	}

    *pResult = 0;
}

LRESULT CProtoPropertyPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	return 0;
}

LRESULT CProtoPropertyPage::OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam)
{
	// Note: hItemChanged checkstate doesn't change until
	// after this is called. That's why the checkstate is
	// toggled before and after UpdateData(FALSE)
	//
	// If hSelectedItem is not the same as hItemChanged
	// than hSelectedItem is validated.
	// If validation fails the control that caused the problem
	// is returned and the checkstate changed is disallowed
	//
	HTREEITEM hItemChanged = (HTREEITEM)lParam;
	HTREEITEM hSelectedItem = this->m_wndTreeCtrl.GetSelectedItem();

#ifdef _DEBUG
	TRACE("OnUM_CheckStateChange\n");
	CString strItem = this->m_wndTreeCtrl.GetItemText(hSelectedItem);
	TRACE("Current Selection = %s\n", strItem);
#endif

	// handle special case when hSelectedItem is the same
	// as hItemChanged
	if (hSelectedItem == hItemChanged) {
		// update current item
		//
		this->m_htiDDX = hItemChanged;
		this->m_wndTreeCtrl.SetCheck(hItemChanged, !this->m_wndTreeCtrl.GetCheck(hItemChanged));
		this->UpdateData(FALSE);
		this->m_wndTreeCtrl.SetCheck(hItemChanged, !this->m_wndTreeCtrl.GetCheck(hItemChanged));
		return 0;
	}	

	// this may be unnec
	if (this->m_wndTreeCtrl.GetCheck(hSelectedItem)) {
		// validate old item
		//
		this->m_htiDDX = hSelectedItem;
		if (this->UpdateData(TRUE)) {
			// set selection on item that was checked
			//
			this->m_wndTreeCtrl.SelectItem(hItemChanged);

			// update new item
			//
			this->m_htiDDX = hItemChanged;
			this->m_wndTreeCtrl.SetCheck(hItemChanged, !this->m_wndTreeCtrl.GetCheck(hItemChanged));
			this->UpdateData(FALSE);
			this->m_wndTreeCtrl.SetCheck(hItemChanged, !this->m_wndTreeCtrl.GetCheck(hItemChanged));
			return 0;
		}
		else {
			// UpdateData failed
			// notify which control caused failure
			//
			return (LRESULT)CWnd::GetFocus();
		}
	}

	// set selection on item that was unchecked
	//
	this->m_wndTreeCtrl.SelectItem(hItemChanged);

	// update new item
	// 
	this->m_htiDDX = hItemChanged;
	this->m_wndTreeCtrl.SetCheck(hItemChanged, !this->m_wndTreeCtrl.GetCheck(hItemChanged));
	this->UpdateData(FALSE);
	this->m_wndTreeCtrl.SetCheck(hItemChanged, !this->m_wndTreeCtrl.GetCheck(hItemChanged));
	return 0;
}

void CProtoPropertyPage::OnTvnKeydownPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);

	// space toggles the checkmark
	//
	if (pTVKeyDown->wVKey == VK_SPACE) {
		// update selected item
		// 
		HTREEITEM hSelectedItem = this->m_wndTreeCtrl.GetSelectedItem();
		this->m_htiDDX = hSelectedItem;
		this->m_wndTreeCtrl.SetCheck(hSelectedItem, !this->m_wndTreeCtrl.GetCheck(hSelectedItem));
		this->UpdateData(FALSE);
		this->m_wndTreeCtrl.SetCheck(hSelectedItem, !this->m_wndTreeCtrl.GetCheck(hSelectedItem));
	}
	*pResult = 0;
}

void CProtoPropertyPage::OnBnClickedSingleValueRadio()
{
	CGlobal::EnableLinearInterpolation(this, FALSE);
	CGlobal::EnableFixed(this, TRUE);
}

void CProtoPropertyPage::OnBnClickedLinearInterpolationRadio()
{
	CGlobal::EnableLinearInterpolation(this, TRUE);
	CGlobal::EnableFixed(this, FALSE);
}
