// Proto2PropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "Proto2PropertyPage.h"


// CProto2PropertyPage dialog

IMPLEMENT_DYNAMIC(CProto2PropertyPage, CPropertyPage)
CProto2PropertyPage::CProto2PropertyPage()
	: CPropertyPage(CProto2PropertyPage::IDD)
	, m_itemIndex(-1)
{
	this->SetProperties(this->m_propActive, this->m_propKx, this->m_propKy, this->m_propPorosity);
}

CProto2PropertyPage::~CProto2PropertyPage()
{
}

void CProto2PropertyPage::SetProperties(const Cproperty& active, const Cproperty& kx, const Cproperty& ky, const Cproperty& porosity)
{
	this->m_propActive = this->m_propActiveFixed = this->m_propActiveLinear = active;
	this->m_propKx = this->m_propKxFixed = this->m_propKxLinear = kx;
	this->m_propKy = this->m_propKyFixed = this->m_propKyLinear = ky;
	this->m_propPorosity = this->m_propPorosityFixed = this->m_propPorosityLinear = porosity;
}

void CProto2PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_LIST, m_wndCheckListCtrl);
	if (this->m_wndCheckListCtrl.GetItemCount() == 0) {
		int index;

		// Active
		index = this->m_wndCheckListCtrl.InsertItem(0, _T("Active"));
		if (this->m_propActive.type != UNDEFINED) {
			this->m_wndCheckListCtrl.SetCheck(index, TRUE);
		}

		// Kx
		index = this->m_wndCheckListCtrl.InsertItem(1, _T("Kx"));
		if (this->m_propKx.type != UNDEFINED) {
			this->m_wndCheckListCtrl.SetCheck(index, TRUE);
		}

		// Ky
		index = this->m_wndCheckListCtrl.InsertItem(2, _T("Ky"));
		if (this->m_propKy.type != UNDEFINED) {
			this->m_wndCheckListCtrl.SetCheck(index, TRUE);
		}

		// Porosity
		index = this->m_wndCheckListCtrl.InsertItem(3, _T("Porosity"));
		if (this->m_propPorosity.type != UNDEFINED) {
			this->m_wndCheckListCtrl.SetCheck(index, TRUE);
		}

		// select first item
		this->m_wndCheckListCtrl.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		this->m_itemIndex = 0;
	}

	//if (this->m_itemIndex >= 0) {
	//	CString strItem = this->m_wndCheckListCtrl.GetItemText(this->m_itemIndex, 0);
	//	if (strItem.Compare(_T("Active")) == 0) {
	//		::DDX_Property(pDX, &this->m_wndCheckListCtrl, this->m_itemIndex, this->m_propActive, this->m_propActiveFixed, this->m_propActiveLinear);
	//	}
	//	else if (strItem.Compare(_T("Kx")) == 0) {
	//		::DDX_Property(pDX, &this->m_wndCheckListCtrl, this->m_itemIndex, this->m_propKx, this->m_propKxFixed, this->m_propKxLinear);
	//	}
	//	else if (strItem.Compare(_T("Ky")) == 0) {
	//		::DDX_Property(pDX, &this->m_wndCheckListCtrl, this->m_itemIndex, this->m_propKy, this->m_propKyFixed, this->m_propKyLinear);
	//	}
	//	else if (strItem.Compare(_T("Porosity")) == 0) {
	//		::DDX_Property(pDX, &this->m_wndCheckListCtrl, this->m_itemIndex, this->m_propPorosity, this->m_propPorosityFixed, this->m_propPorosityLinear);
	//	}
	//	else {
	//		ASSERT(FALSE);
	//	}
	//}
}


BEGIN_MESSAGE_MAP(CProto2PropertyPage, CPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PROP_LIST, OnLvnItemchangedPropList)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_PROP_LIST, OnLvnItemchangingPropList)
END_MESSAGE_MAP()


// CProto2PropertyPage message handlers

void CProto2PropertyPage::OnLvnItemchangingPropList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	TRACE("<Itemchanging>\n");
	TRACE("iItem %d\n", pNMLV->iItem);
	TRACE("iSubItem %d\n", pNMLV->iSubItem);
	TRACE("uChanged %d\n", pNMLV->uChanged);
	TRACE("uNewState %d\n", pNMLV->uNewState);
	TRACE("uOldState %d\n", pNMLV->uOldState);
	TRACE("</Itemchanging>\n");

	static bool bIgnoreNextFocus;

	////if (pNMLV->uOldState == 0 && pNMLV->uNewState == (LVIS_SELECTED|LVIS_FOCUSED)) {
	////	*pResult = TRUE;
	////	return;
	////}



	if (pNMLV->uNewState == 0 && pNMLV->uOldState == LVIS_FOCUSED) { // maybe LVIS_SELECTED instead
		// current selection(pNMLV->iItem) is about to lose focus
		//
		this->m_wndCheckListCtrl.SetFocus();
		if (!UpdateData(TRUE)) {
			// disallow selection change
			// this->m_wndDDXError = CWnd::GetFocus();
			this->m_itemIndex = pNMLV->iItem;
			*pResult = TRUE;
			return;
		}
	}


	if (pNMLV->uNewState == 0 && pNMLV->uOldState == LVIS_SELECTED) { // maybe LVIS_SELECTED instead
		// disallow selection change
		*pResult = TRUE;
		return;
	}

	//{{
	////POSITION pos = this->m_wndCheckListCtrl.GetFirstSelectedItemPosition();
	////if (pos == NULL) {
	////	TRACE0("No items were selected!\n");
	////}
	////else {
	////	while (pos)
	////	{
	////		int nItem = this->m_wndCheckListCtrl.GetNextSelectedItem(pos);
	////		TRACE1("Item %d was selected!\n", nItem);
	////	}
	////}
	//}}


	//{{
	/*
#define LVIS_FOCUSED            0x0001
#define LVIS_SELECTED           0x0002
#define LVIS_CUT                0x0004
#define LVIS_DROPHILITED        0x0008
#define LVIS_GLOW               0x0010
#define LVIS_ACTIVATING         0x0020

#define LVIS_OVERLAYMASK        0x0F00
#define LVIS_STATEIMAGEMASK     0xF000
	*/
	//}}

	////if ((pNMLV->uNewState ^ pNMLV->uOldState) & LVIS_STATEIMAGEMASK)
	////{
	////	TRACE("LVIS_STATEIMAGEMASK\n");
	////	*pResult = TRUE; // deny checkchange
	////	return;
	////}

	//{{
	//*pResult = TRUE;
	//return;
	//}}
	*pResult = 0;
}

void CProto2PropertyPage::OnLvnItemchangedPropList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// Add your control notification handler code here

	TRACE("<Itemchanged>\n");
	TRACE("iItem %d\n", pNMLV->iItem);
	TRACE("iSubItem %d\n", pNMLV->iSubItem);
	TRACE("uChanged %d\n", pNMLV->uChanged);
	TRACE("uNewState %d\n", pNMLV->uNewState);
	TRACE("uOldState %d\n", pNMLV->uOldState);
	TRACE("</Itemchanged>\n");

	if (pNMLV->uNewState == (LVIS_SELECTED|LVIS_FOCUSED) && pNMLV->uOldState == 0) {
		// new selection
		//
		this->m_itemIndex = pNMLV->iItem;
		this->UpdateData(FALSE);
	}




	// update itemNew
	//
	// this->m_itemIndex = pNMLV->iItem;
	// this->UpdateData(FALSE);

	*pResult = 0;
}
