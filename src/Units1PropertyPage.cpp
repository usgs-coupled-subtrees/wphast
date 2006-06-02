// Units1PropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "Units1PropertyPage.h"

#include "Global.h"


// CUnits1PropertyPage dialog

IMPLEMENT_DYNAMIC(CUnits1PropertyPage, CPropertyPage)

CUnits1PropertyPage::CUnits1PropertyPage()
	: CPropertyPage(CUnits1PropertyPage::IDD)
	, m_idSetFocus(0)
{
}

CUnits1PropertyPage::CUnits1PropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CUnits1PropertyPage::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
	, m_idSetFocus(0)
{
}

CUnits1PropertyPage::~CUnits1PropertyPage()
{
}

void CUnits1PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIME_COMBO, m_wndTimeCombo);
	DDX_Control(pDX, IDC_HORZ_COMBO, m_wndHorizontalCombo);
	DDX_Control(pDX, IDC_VERT_COMBO, m_wndVerticalCombo);
	DDX_Control(pDX, IDC_HEAD_COMBO, m_wndHeadCombo);
	DDX_Control(pDX, IDC_HYD_COND_NUM_COMBO, m_wndKNumCombo);
	DDX_Control(pDX, IDC_HYD_COND_DENOM_COMBO, m_wndKDenomCombo);
	DDX_Control(pDX, IDC_STORAGE_DENOM_COMBO, m_wndSDenomCombo);
	DDX_Control(pDX, IDC_DISP_COMBO, m_wndAlphaCombo);

	// time
	if (m_wndTimeCombo.GetCount() == 0) {
		CGlobal::AddTimeUnits(&m_wndTimeCombo);
	}
	// horizontal
	if (m_wndHorizontalCombo.GetCount() == 0) {
		CGlobal::AddLengthUnits(&m_wndHorizontalCombo);
	}
	// vertical
	if (m_wndVerticalCombo.GetCount() == 0) {
		CGlobal::AddLengthUnits(&m_wndVerticalCombo);
	}
	// head
	if (m_wndHeadCombo.GetCount() == 0) {
		CGlobal::AddLengthUnits(&m_wndHeadCombo);
	}
	// k numerator
	if (m_wndKNumCombo.GetCount() == 0) {
		CGlobal::AddLengthUnits(&m_wndKNumCombo);
	}
	// k denominator
	if (m_wndKDenomCombo.GetCount() == 0) {
		CGlobal::AddTimeUnitsDenom(&m_wndKDenomCombo);
	}
	// s denominator
	if (m_wndSDenomCombo.GetCount() == 0) {
		CGlobal::AddLengthUnitsDenom(&m_wndSDenomCombo);
	}
	// alpha
	if (m_wndAlphaCombo.GetCount() == 0) {
		CGlobal::AddLengthUnits(&m_wndAlphaCombo);
	}

	if (pDX->m_bSaveAndValidate)
	{
		CString numer;
		CString denom;
		CString value;

		// time
		this->m_wndTimeCombo.GetLBText(this->m_wndTimeCombo.GetCurSel(), numer);
		this->m_units.time.set_input(numer);

		// horizontal
		this->m_wndHorizontalCombo.GetLBText(this->m_wndHorizontalCombo.GetCurSel(), numer);
		this->m_units.horizontal.set_input(numer);

		// vertical
		this->m_wndVerticalCombo.GetLBText(this->m_wndVerticalCombo.GetCurSel(), numer);
		this->m_units.vertical.set_input(numer);

		// head
		this->m_wndHeadCombo.GetLBText(this->m_wndHeadCombo.GetCurSel(), numer);
		this->m_units.head.set_input(numer);

		// k
		this->m_wndKNumCombo.GetLBText(this->m_wndKNumCombo.GetCurSel(), numer);
		this->m_wndKDenomCombo.GetLBText(this->m_wndKDenomCombo.GetCurSel(), denom);
		value = numer;
		value += "/";
		value += denom;
		this->m_units.k.set_input(value);

		// s
		this->m_wndSDenomCombo.GetLBText(this->m_wndSDenomCombo.GetCurSel(), denom);
		value = "1/";
		value += denom;
		this->m_units.s.set_input(value);

		// alpha
		this->m_wndAlphaCombo.GetLBText(this->m_wndAlphaCombo.GetCurSel(), numer);
		this->m_units.alpha.set_input(numer);

	}
	else
	{
		// time
		if (this->m_units.time.defined) {
			VERIFY(this->m_wndTimeCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str()) != CB_ERR);
		}
		else {
			VERIFY(this->m_wndTimeCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.si).c_str()) != CB_ERR);
		}

		// horizontal
		if (this->m_units.horizontal.defined) {
			VERIFY(this->m_wndHorizontalCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.horizontal.input).c_str()) != CB_ERR);
		}
		else {
			VERIFY(this->m_wndHorizontalCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.horizontal.si).c_str()) != CB_ERR);
		}

		// vertical
		if (this->m_units.vertical.defined) {
			VERIFY(this->m_wndVerticalCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.vertical.input).c_str()) != CB_ERR);
		}
		else {
			VERIFY(this->m_wndVerticalCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.vertical.si).c_str()) != CB_ERR);
		}

		// head
		if (this->m_units.head.defined) {
			VERIFY(this->m_wndHeadCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.head.input).c_str()) != CB_ERR);
		}
		else {
			VERIFY(this->m_wndHeadCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.head.si).c_str()) != CB_ERR);
		}

		// k
		if (this->m_units.k.defined) {
			std::string strK(this->m_units.k.input);
			std::string::size_type n = strK.find('/');
			ASSERT(n != std::string::npos);
			std::string strKNum = strK.substr(0, n);
			std::string strKDenom = strK.substr(n+1, std::string::npos);
			// k numerator
			VERIFY(this->m_wndKNumCombo.SelectString(0, CGlobal::GetStdLengthUnits(strKNum.c_str()).c_str()) != CB_ERR);
			// k denominator
			VERIFY(this->m_wndKDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strKDenom.c_str()).c_str()) != CB_ERR);
		}
		else {
			std::string strK(this->m_units.k.si);
			std::string::size_type n = strK.find('/');
			ASSERT(n != std::string::npos);
			std::string strKNum = strK.substr(0, n);
			std::string strKDenom = strK.substr(n+1, std::string::npos);
			// k numerator
			VERIFY(this->m_wndKNumCombo.SelectString(0, CGlobal::GetStdLengthUnits(strKNum.c_str()).c_str()) != CB_ERR);
			// k denominator
			VERIFY(this->m_wndKDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strKDenom.c_str()).c_str()) != CB_ERR);
		}

		// s
		if (this->m_units.s.defined) {
			std::string strS(this->m_units.s.input);
			std::string::size_type n = strS.find('/');
			ASSERT(n != std::string::npos);
			std::string strSNum = strS.substr(0, n);
			std::string strSDenom = strS.substr(n+1, std::string::npos);
			// s denominator
			VERIFY(this->m_wndSDenomCombo.SelectString(0, CGlobal::GetStdLengthUnitsDenom(strSDenom.c_str()).c_str()) != CB_ERR);
		}
		else {
			std::string strS(this->m_units.s.si);
			std::string::size_type n = strS.find('/');
			ASSERT(n != std::string::npos);
			std::string strSNum = strS.substr(0, n);
			std::string strSDenom = strS.substr(n+1, std::string::npos);
			// s denominator
			VERIFY(this->m_wndSDenomCombo.SelectString(0, CGlobal::GetStdLengthUnitsDenom(strSDenom.c_str()).c_str()) != CB_ERR);
		}

		// alpha
		if (this->m_units.alpha.defined) {
			VERIFY(this->m_wndAlphaCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.alpha.input).c_str()) != CB_ERR);
		}
		else {
			VERIFY(this->m_wndAlphaCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.alpha.si).c_str()) != CB_ERR);
		}
	}
}

void CUnits1PropertyPage::SetProperties(const CUnits& r_units)
{
	this->m_units = r_units;
}

void CUnits1PropertyPage::GetProperties(CUnits& r_units)const
{
	// time
	r_units.time       = this->m_units.time;
	// horizontal
	r_units.horizontal = this->m_units.horizontal;
	// vertical
	r_units.vertical   = this->m_units.vertical;
	// head
	r_units.head       = this->m_units.head;
	// k
	r_units.k          = this->m_units.k;
	// s
	r_units.s          = this->m_units.s;
	// alpha
	r_units.alpha      = this->m_units.alpha;
}

BEGIN_MESSAGE_MAP(CUnits1PropertyPage, CPropertyPage)
    ON_MESSAGE(WM_SETPAGEFOCUS, OnSetPageFocus)
END_MESSAGE_MAP()


// CUnits1PropertyPage message handlers

BOOL CUnits1PropertyPage::Contains(int nID)const
{
	switch (nID) {
		case IDC_TIME_COMBO:
		case IDC_HORZ_COMBO:
		case IDC_VERT_COMBO:
		case IDC_HEAD_COMBO:
		case IDC_HYD_COND_NUM_COMBO:
		case IDC_HYD_COND_DENOM_COMBO:
		case IDC_STORAGE_DENOM_COMBO:
		case IDC_DISP_COMBO:
			return TRUE;
	}
	return FALSE;
}

void CUnits1PropertyPage::SetControlFocus(int nID)
{
	if (this->Contains(nID)) {
		this->m_idSetFocus = nID;
	}
	else {
		this->m_idSetFocus = 0;
	}
}

BOOL CUnits1PropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();

	CPropertySheet* pSheet = (CPropertySheet*) this->GetParent();   
	if (pSheet->IsWizard()) {
		pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	}
	else {
		this->PostMessage(WM_SETPAGEFOCUS, 0, 0L);
	}
	return bRet;
}

LRESULT CUnits1PropertyPage::OnSetPageFocus(WPARAM, LPARAM)
{
	if (this->m_idSetFocus != 0) {
		if (CWnd* pWnd = this->GetDlgItem(this->m_idSetFocus)) {
			pWnd->SetFocus();
		}
	}
	return 0;
}

BOOL CUnits1PropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

// COMMENT: {6/2/2004 9:42:11 PM}	// Disable cancel if no apply button
// COMMENT: {6/2/2004 9:42:11 PM}	//
// COMMENT: {6/2/2004 9:42:11 PM}	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
// COMMENT: {6/2/2004 9:42:11 PM}	ASSERT_KINDOF(CPropertySheet, pSheet);
// COMMENT: {6/2/2004 9:42:11 PM}	if (pSheet->m_psh.dwFlags & PSH_NOAPPLYNOW) {
// COMMENT: {6/2/2004 9:42:11 PM}		pSheet->GetDlgItem(2)->EnableWindow(FALSE);
// COMMENT: {6/2/2004 9:42:11 PM}		pSheet->ModifyStyle(WS_SYSMENU, 0);
// COMMENT: {6/2/2004 9:42:11 PM}	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
