// Proto3PropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "Proto3PropertyPage.h"
#include <windowsx.h>
#include "global.h"

// CProto3PropertyPage dialog

IMPLEMENT_DYNAMIC(CProto3PropertyPage, CPropertyPage)

CProto3PropertyPage::CProto3PropertyPage()
	: CPropertyPage(CProto3PropertyPage::IDD)
	, m_htiDDX(0)
{
	// init props
	//
	this->SetProperties(this->m_propActive, this->m_propKx, this->m_propKy, this->m_propPorosity);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sActiveRTF, IDR_MEDIA_ACTIVE_RTF);
	CGlobal::LoadRTFString(this->m_sKxRTF, IDR_MEDIA_KX_RTF);
	CGlobal::LoadRTFString(this->m_sKyRTF, IDR_MEDIA_KY_RTF);
	CGlobal::LoadRTFString(this->m_sPorosityRTF, IDR_MEDIA_POROSITY_RTF);
}

CProto3PropertyPage::~CProto3PropertyPage()
{
}

void CProto3PropertyPage::SetProperties(const Cproperty& active, const Cproperty& kx, const Cproperty& ky, const Cproperty& porosity)
{
	this->m_propActive   = this->m_propActiveFixed   = this->m_propActiveLinear   = active;
	this->m_propKx       = this->m_propKxFixed       = this->m_propKxLinear       = kx;
	this->m_propKy       = this->m_propKyFixed       = this->m_propKyLinear       = ky;
	this->m_propPorosity = this->m_propPorosityFixed = this->m_propPorosityLinear = porosity;
}

void CProto3PropertyPage::GetProperties(Cproperty& active, Cproperty& kx, Cproperty& ky, Cproperty& porosity)const
{
	active   = this->m_propActive;
	kx       = this->m_propKx;
	ky       = this->m_propKy;
	porosity = this->m_propPorosity;
}

void CProto3PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_TREE, m_wndTreeCtrl);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	if (this->m_wndTreeCtrl.GetCount() == 0) {
		HTREEITEM hti;

		// Kx
		hti = this->m_wndTreeCtrl.InsertItem(_T("Kx"), TVI_ROOT, TVI_SORT);
		if (this->m_propKx.type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// active
		hti = this->m_wndTreeCtrl.InsertItem(_T("Active"), TVI_ROOT, TVI_SORT);
		if (this->m_propActive.type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// Ky
		hti = this->m_wndTreeCtrl.InsertItem(_T("Ky"), TVI_ROOT, TVI_SORT);
		if (this->m_propKy.type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// Porosity
		hti = this->m_wndTreeCtrl.InsertItem(_T("Porosity"), TVI_ROOT, TVI_SORT);
		if (this->m_propPorosity.type != UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sActiveRTF.c_str());
	}

	if (this->m_htiDDX) {
		CString strItem = this->m_wndTreeCtrl.GetItemText(this->m_htiDDX);

		// active
		if (strItem.Compare(_T("Active")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(&this->m_propActive),
				static_cast<Cproperty*>(&this->m_propActiveFixed),
				static_cast<Cproperty*>(&this->m_propActiveLinear)
				);
		}
		// Kx
		else if (strItem.Compare(_T("Kx")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(&this->m_propKx),
				static_cast<Cproperty*>(&this->m_propKxFixed),
				static_cast<Cproperty*>(&this->m_propKxLinear)
				);
		}
		// Ky
		else if (strItem.Compare(_T("Ky")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(&this->m_propKy),
				static_cast<Cproperty*>(&this->m_propKyFixed),
				static_cast<Cproperty*>(&this->m_propKyLinear)
				);
		}
		// Porosity
		else if (strItem.Compare(_T("Porosity")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(&this->m_propPorosity),
				static_cast<Cproperty*>(&this->m_propPorosityFixed),
				static_cast<Cproperty*>(&this->m_propPorosityLinear)
				);
		}
		// ERROR
		else {
			ASSERT(FALSE);
		}
	}
}


BEGIN_MESSAGE_MAP(CProto3PropertyPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROP_TREE, OnTvnSelchangedPropTree)
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTvnSelchangingPropTree)
	//{{USER
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	ON_MESSAGE(UM_CHECKSTATECHANGE, OnUM_CheckStateChange)
	//}}USER
	ON_BN_CLICKED(IDC_SINGLE_VALUE_RADIO, OnBnClickedSingleValueRadio)
	ON_BN_CLICKED(IDC_LINEAR_INTERPOLATION_RADIO, OnBnClickedLinearInterpolationRadio)
END_MESSAGE_MAP()


// CProto3PropertyPage message handlers

void CProto3PropertyPage::OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRACE("OnTvnSelchangedPropTree\n");
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

		// Active
		if (strItem.Compare(_T("Active")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sActiveRTF.c_str());
		}
		// Kx
		else if (strItem.Compare(_T("Kx")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sKxRTF.c_str());
		}
		// Ky
		else if (strItem.Compare(_T("Ky")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sKyRTF.c_str());
		}
		// Porosity
		else if (strItem.Compare(_T("Porosity")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sPorosityRTF.c_str());
		}
		// ERROR
		else {
			this->m_wndRichEditCtrl.SetWindowText("");
		}
	}

	*pResult = 0;
}

void CProto3PropertyPage::OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRACE("OnTvnSelchangingPropTree\n");

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

LRESULT CProto3PropertyPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	return 0;
}

LRESULT CProto3PropertyPage::OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam)
{
	TRACE("OnUM_CheckStateChange\n");

	HTREEITEM hSelectedItem = this->m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hItemChanged = (HTREEITEM)lParam;

#ifdef _DEBUG
	CString strItem = this->m_wndTreeCtrl.GetItemText(hSelectedItem);
	TRACE("Current Selection = %s\n", strItem);
	strItem = this->m_wndTreeCtrl.GetItemText(hItemChanged);
	TRACE("Item changed = %s\n", strItem);
#endif

	// handle special case when hSelectedItem is the same
	// as hItemChanged
	if (hSelectedItem == hItemChanged) {
		// update current item
		//
		this->m_htiDDX = hSelectedItem;
		this->UpdateData(FALSE);
		return 0;
	}

	//{{ NEW
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
	//}} NEW

	////////if (this->m_wndTreeCtrl.GetCheck(hSelectedItem) == BST_CHECKED) {
	////////	// validate old item
	////////	//
	////////	this->m_htiDDX = hSelectedItem;
	////////	if (this->UpdateData(TRUE)) {
	////////		// set selection on item that was checked
	////////		// causes TVN_SELCHANGING and TVN_SELCHANGED
	////////		//
	////////		this->m_wndTreeCtrl.SelectItem(hItemChanged);

	////////		//{{ MAYBE UNNEC
	////////		// update new item
	////////		//
	////////		////this->m_htiDDX = hItemChanged;
	////////		////this->UpdateData(FALSE);
	////////		//}} MAYBE UNNEC
	////////		return 0;
	////////	}
	////////	else {
	////////		// UpdateData failed
	////////		// notify which control caused the failure
	////////		//
	////////		return (LRESULT)CWnd::GetFocus();
	////////	}
	////////}
	return 0;
}

void CProto3PropertyPage::OnBnClickedSingleValueRadio()
{
	switch (this->GetCheckedRadioButton(IDC_SINGLE_VALUE_RADIO, IDC_LINEAR_INTERPOLATION_RADIO))
	{
	case IDC_SINGLE_VALUE_RADIO:
		CGlobal::EnableFixed(this, TRUE);
		CGlobal::EnableLinearInterpolation(this, FALSE);
		break;

	case IDC_LINEAR_INTERPOLATION_RADIO:
		CGlobal::EnableFixed(this, FALSE);
		CGlobal::EnableLinearInterpolation(this, TRUE);
		break;
	}
}

void CProto3PropertyPage::OnBnClickedLinearInterpolationRadio()
{
	switch (this->GetCheckedRadioButton(IDC_SINGLE_VALUE_RADIO, IDC_LINEAR_INTERPOLATION_RADIO))
	{
	case IDC_SINGLE_VALUE_RADIO:
		CGlobal::EnableFixed(this, TRUE);
		CGlobal::EnableLinearInterpolation(this, FALSE);
		break;

	case IDC_LINEAR_INTERPOLATION_RADIO:
		CGlobal::EnableFixed(this, FALSE);
		CGlobal::EnableLinearInterpolation(this, TRUE);
		break;
	}
}
