// Units2PropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "Units2PropertyPage.h"

#include "Global.h"


// CUnits2PropertyPage dialog

IMPLEMENT_DYNAMIC(CUnits2PropertyPage, CPropertyPage)

CUnits2PropertyPage::CUnits2PropertyPage()
	: CPropertyPage(CUnits2PropertyPage::IDD)
	, m_idSetFocus(0)
{
}

CUnits2PropertyPage::CUnits2PropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CUnits2PropertyPage::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
	, m_idSetFocus(0)
{
}

CUnits2PropertyPage::~CUnits2PropertyPage()
{
}

void CUnits2PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HYD_COND_NUM_COMBO, m_wndKNumCombo);
	DDX_Control(pDX, IDC_HYD_COND_DENOM_COMBO, m_wndKDenomCombo);
	DDX_Control(pDX, IDC_STORAGE_DENOM_COMBO, m_wndSDenomCombo);
	DDX_Control(pDX, IDC_DISP_COMBO, m_wndAlphaCombo);

	// k numerator
	if (m_wndKNumCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndKNumCombo);
	}
	// k denominator
	if (m_wndKDenomCombo.GetCount() == 0)
	{
		CGlobal::AddTimeUnitsDenom(&m_wndKDenomCombo);
	}
	// s denominator
	if (m_wndSDenomCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnitsDenom(&m_wndSDenomCombo);
	}
	// alpha
	if (m_wndAlphaCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndAlphaCombo);
	}

	if (pDX->m_bSaveAndValidate)
	{
		CString numer;
		CString denom;
		CString value;

		// k
		this->m_wndKNumCombo.GetLBText(this->m_wndKNumCombo.GetCurSel(), numer);
		this->m_wndKDenomCombo.GetLBText(this->m_wndKDenomCombo.GetCurSel(), denom);
		value = numer;
		value += "/";
		value += denom;
		VERIFY(this->m_units.k.set_input(value) == OK);

		// s
		this->m_wndSDenomCombo.GetLBText(this->m_wndSDenomCombo.GetCurSel(), denom);
		value = "1/";
		value += denom;
		VERIFY(this->m_units.s.set_input(value) == OK);

		// alpha
		this->m_wndAlphaCombo.GetLBText(this->m_wndAlphaCombo.GetCurSel(), numer);
		VERIFY(this->m_units.alpha.set_input(numer) == OK);

	}
	else
	{
		// k
		if (this->m_units.k.defined)
		{
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
		else
		{
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
		if (this->m_units.s.defined)
		{
			std::string strS(this->m_units.s.input);
			std::string::size_type n = strS.find('/');
			ASSERT(n != std::string::npos);
			std::string strSNum = strS.substr(0, n);
			std::string strSDenom = strS.substr(n+1, std::string::npos);
			// s denominator
			VERIFY(this->m_wndSDenomCombo.SelectString(0, CGlobal::GetStdLengthUnitsDenom(strSDenom.c_str()).c_str()) != CB_ERR);
		}
		else
		{
			std::string strS(this->m_units.s.si);
			std::string::size_type n = strS.find('/');
			ASSERT(n != std::string::npos);
			std::string strSNum = strS.substr(0, n);
			std::string strSDenom = strS.substr(n+1, std::string::npos);
			// s denominator
			VERIFY(this->m_wndSDenomCombo.SelectString(0, CGlobal::GetStdLengthUnitsDenom(strSDenom.c_str()).c_str()) != CB_ERR);
		}

		// alpha
		if (this->m_units.alpha.defined)
		{
			VERIFY(this->m_wndAlphaCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.alpha.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndAlphaCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.alpha.si).c_str()) != CB_ERR);
		}
	}
}

void CUnits2PropertyPage::SetProperties(const CUnits& r_units)
{
	this->m_units = r_units;
}

void CUnits2PropertyPage::GetProperties(CUnits& r_units)const
{
	// k
	r_units.k          = this->m_units.k;
	// s
	r_units.s          = this->m_units.s;
	// alpha
	r_units.alpha      = this->m_units.alpha;
}

BEGIN_MESSAGE_MAP(CUnits2PropertyPage, CPropertyPage)
    ON_MESSAGE(WM_SETPAGEFOCUS, OnSetPageFocus)
END_MESSAGE_MAP()


// CUnits2PropertyPage message handlers

BOOL CUnits2PropertyPage::Contains(int nID)const
{
	switch (nID)
	{
		case IDC_HYD_COND_NUM_COMBO:
		case IDC_HYD_COND_DENOM_COMBO:
		case IDC_STORAGE_DENOM_COMBO:
		case IDC_DISP_COMBO:
			return TRUE;
	}
	return FALSE;
}

void CUnits2PropertyPage::SetControlFocus(int nID)
{
	if (this->Contains(nID))
	{
		this->m_idSetFocus = nID;
	}
	else
	{
		this->m_idSetFocus = 0;
	}
}

BOOL CUnits2PropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();

	CPropertySheet* pSheet = (CPropertySheet*) this->GetParent();
	if (pSheet->IsWizard())
	{
		pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	}
	else
	{
		this->PostMessage(WM_SETPAGEFOCUS, 0, 0L);
	}
	return bRet;
}

LRESULT CUnits2PropertyPage::OnSetPageFocus(WPARAM, LPARAM)
{
	if (this->m_idSetFocus != 0)
	{
		if (CWnd* pWnd = this->GetDlgItem(this->m_idSetFocus))
		{
			pWnd->SetFocus();
		}
	}
	return 0;
}

BOOL CUnits2PropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
