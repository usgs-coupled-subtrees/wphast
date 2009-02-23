// Units3PropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "Units3PropertyPage.h"

#include "Global.h"
#include <algorithm>
#include <locale>

// CUnits3PropertyPage dialog

IMPLEMENT_DYNAMIC(CUnits3PropertyPage, CPropertyPage)
CUnits3PropertyPage::CUnits3PropertyPage()
	: CPropertyPage(CUnits3PropertyPage::IDD)
	, m_idSetFocus(0)
{
}

CUnits3PropertyPage::CUnits3PropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CUnits3PropertyPage::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
	, m_idSetFocus(0)
{
}

CUnits3PropertyPage::~CUnits3PropertyPage()
{
}

void CUnits3PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FLUX_NUM_COMBO, m_wndFluxNumCombo);
	DDX_Control(pDX, IDC_FLUX_DENOM_COMBO, m_wndFluxDenomCombo);
	DDX_Control(pDX, IDC_LEAKY_COND_NUM_COMBO, m_wndLeakyKNumCombo);
	DDX_Control(pDX, IDC_LEAKY_COND_DENOM_COMBO, m_wndLeakyKDenomCombo);
	DDX_Control(pDX, IDC_LEAKY_THICK_COMBO, m_wndLeakyThickCombo);
	DDX_Control(pDX, IDC_WELL_DIAM_COMBO, m_wndWellDiamCombo);
	DDX_Control(pDX, IDC_WELL_FLOW_NUM_COMBO, m_wndWellPumpageNumCombo);
	DDX_Control(pDX, IDC_WELL_FLOW_DENOM_COMBO, m_wndWellPumpageDenomCombo);
	DDX_Control(pDX, IDC_WELL_DEPTH_COMBO, m_wndWellDepthCombo);

	// flux numerator
	if (m_wndFluxNumCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndFluxNumCombo);
	}
	// flux denominator
	if (m_wndFluxDenomCombo.GetCount() == 0)
	{
		CGlobal::AddTimeUnitsDenom(&m_wndFluxDenomCombo);
	}
	// leaky_hydraulic_conductivity numerator
	if (m_wndLeakyKNumCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndLeakyKNumCombo);
	}
	// leaky_hydraulic_conductivity denominator
	if (m_wndLeakyKDenomCombo.GetCount() == 0)
	{
		CGlobal::AddTimeUnitsDenom(&m_wndLeakyKDenomCombo);
	}
	// leaky_thickness
	if (m_wndLeakyThickCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndLeakyThickCombo);
	}
	// well_diameter
	if (m_wndWellDiamCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndWellDiamCombo);
	}
	// well_pumpage numerator
	if (m_wndWellPumpageNumCombo.GetCount() == 0)
	{
		CGlobal::AddVolumeUnits(&m_wndWellPumpageNumCombo);
	}
	// well_pumpage denominator
	if (m_wndWellPumpageDenomCombo.GetCount() == 0)
	{
		CGlobal::AddTimeUnitsDenom(&m_wndWellPumpageDenomCombo);
		m_wndWellPumpageDenomCombo.SetFocus();
	}
	// well_depth
	if (m_wndWellDepthCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndWellDepthCombo);
		m_wndWellDepthCombo.SetFocus();
	}

	if (pDX->m_bSaveAndValidate)
	{
		CString numer;
		CString denom;
		CString value;

		// flux
		this->m_wndFluxNumCombo.GetLBText(this->m_wndFluxNumCombo.GetCurSel(), numer);
		this->m_wndFluxDenomCombo.GetLBText(this->m_wndFluxDenomCombo.GetCurSel(), denom);
		value = numer;
		value += "/";
		value += denom;
		this->m_units.flux.set_input(value);

		// leaky_k
		this->m_wndLeakyKNumCombo.GetLBText(this->m_wndLeakyKNumCombo.GetCurSel(), numer);
		this->m_wndLeakyKDenomCombo.GetLBText(this->m_wndLeakyKDenomCombo.GetCurSel(), denom);
		value = numer;
		value += "/";
		value += denom;
		this->m_units.leaky_k.set_input(value);

		// leaky_thick
		this->m_wndLeakyThickCombo.GetLBText(this->m_wndLeakyThickCombo.GetCurSel(), value);
		this->m_units.leaky_thick.set_input(value);

		// well_diameter
		this->m_wndWellDiamCombo.GetLBText(this->m_wndWellDiamCombo.GetCurSel(), value);
		this->m_units.well_diameter.set_input(value);

		// well_pumpage
		this->m_wndWellPumpageNumCombo.GetLBText(this->m_wndWellPumpageNumCombo.GetCurSel(), numer);
		this->m_wndWellPumpageDenomCombo.GetLBText(this->m_wndWellPumpageDenomCombo.GetCurSel(), denom);
		value = numer;
		value += "/";
		value += denom;
		this->m_units.well_pumpage.set_input(value);

		// well_depth
		this->m_wndWellDepthCombo.GetLBText(this->m_wndWellDepthCombo.GetCurSel(), value);
		this->m_units.well_depth.set_input(value);
	}
	else
	{
		// flux [L/T]
		if (this->m_units.flux.defined)
		{
			std::string str(this->m_units.flux.input);
			std::string::size_type n = str.find('/');
			ASSERT(n != std::string::npos);
			std::string strNum = str.substr(0, n);
			std::string strDenom = str.substr(n+1, std::string::npos);
			// flux numerator
			VERIFY(this->m_wndFluxNumCombo.SelectString(0, CGlobal::GetStdLengthUnits(strNum.c_str()).c_str()) != CB_ERR);
			// flux denominator
			VERIFY(this->m_wndFluxDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strDenom.c_str()).c_str()) != CB_ERR);
		}
		else
		{
			std::string str(this->m_units.flux.si);
			std::string::size_type n = str.find('/');
			ASSERT(n != std::string::npos);
			std::string strNum = str.substr(0, n);
			std::string strDenom = str.substr(n+1, std::string::npos);
			// flux numerator
			VERIFY(this->m_wndFluxNumCombo.SelectString(0, CGlobal::GetStdLengthUnits(strNum.c_str()).c_str()) != CB_ERR);
			// flux denominator
			VERIFY(this->m_wndFluxDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strDenom.c_str()).c_str()) != CB_ERR);
		}

		// leaky_k [L/T]
		if (this->m_units.leaky_k.defined)
		{
			std::string str(this->m_units.leaky_k.input);
			std::string::size_type n = str.find('/');
			ASSERT(n != std::string::npos);
			std::string strNum = str.substr(0, n);
			std::string strDenom = str.substr(n+1, std::string::npos);
			// leaky_k numerator
			VERIFY(this->m_wndLeakyKNumCombo.SelectString(0, CGlobal::GetStdLengthUnits(strNum.c_str()).c_str()) != CB_ERR);
			// leaky_k denominator
			VERIFY(this->m_wndLeakyKDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strDenom.c_str()).c_str()) != CB_ERR);
		}
		else
		{
			std::string str(this->m_units.leaky_k.si);
			std::string::size_type n = str.find('/');
			ASSERT(n != std::string::npos);
			std::string strNum = str.substr(0, n);
			std::string strDenom = str.substr(n+1, std::string::npos);
			// leaky_k numerator
			VERIFY(this->m_wndLeakyKNumCombo.SelectString(0, CGlobal::GetStdLengthUnits(strNum.c_str()).c_str()) != CB_ERR);
			// leaky_k denominator
			VERIFY(this->m_wndLeakyKDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strDenom.c_str()).c_str()) != CB_ERR);
		}

		// leaky_thick [L]
		if (this->m_units.leaky_thick.defined)
		{
			VERIFY(this->m_wndLeakyThickCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.leaky_thick.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndLeakyThickCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.leaky_thick.si).c_str()) != CB_ERR);
		}

		// well_diameter [L]
		if (this->m_units.well_diameter.defined)
		{
			VERIFY(this->m_wndWellDiamCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.well_diameter.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndWellDiamCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.well_diameter.si).c_str()) != CB_ERR);
		}

		// well_pumpage [L^3/T]
		if (this->m_units.well_pumpage.defined)
		{
			// handle gpm
			CString s(this->m_units.well_pumpage.input);
			s.MakeLower();
			s.Replace("gpm", "gal/MIN");

			std::string str(s);
			std::string::size_type n = str.find('/');
			ASSERT(n != std::string::npos);
			std::string strNum = str.substr(0, n);
			std::string strDenom = str.substr(n+1, std::string::npos);
			// well_pumpage numerator
			VERIFY(this->m_wndWellPumpageNumCombo.SelectString(0, CGlobal::GetStdVolumeUnits(strNum.c_str()).c_str()) != CB_ERR);
			// well_pumpage denominator
			VERIFY(this->m_wndWellPumpageDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strDenom.c_str()).c_str()) != CB_ERR);
		}
		else
		{
			std::string str(this->m_units.well_pumpage.si);
			std::string::size_type n = str.find('/');
			ASSERT(n != std::string::npos);
			std::string strNum = str.substr(0, n);
			std::string strDenom = str.substr(n+1, std::string::npos);
			// well_pumpage numerator
			VERIFY(this->m_wndWellPumpageNumCombo.SelectString(0, CGlobal::GetStdVolumeUnits(strNum.c_str()).c_str()) != CB_ERR);
			// well_pumpage denominator
			VERIFY(this->m_wndWellPumpageDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strDenom.c_str()).c_str()) != CB_ERR);
		}

		// well_depth [L]
		if (this->m_units.well_depth.defined)
		{
			VERIFY(this->m_wndWellDepthCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.well_depth.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndWellDepthCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.well_depth.si).c_str()) != CB_ERR);
		}
	}
}

void CUnits3PropertyPage::SetProperties(const CUnits& r_units)
{
	this->m_units = r_units;
}

void CUnits3PropertyPage::GetProperties(CUnits& r_units)const
{
	// flux
	r_units.flux                = this->m_units.flux;
	// leaky_k
	r_units.leaky_k             = this->m_units.leaky_k;
	// leaky_thick
	r_units.leaky_thick         = this->m_units.leaky_thick;
	// well_diameter
	r_units.well_diameter       = this->m_units.well_diameter;
	// well_pumpage
	r_units.well_pumpage        = this->m_units.well_pumpage;
	// well_depth
	r_units.well_depth          = this->m_units.well_depth;
}


BEGIN_MESSAGE_MAP(CUnits3PropertyPage, CPropertyPage)
    ON_MESSAGE(WM_SETPAGEFOCUS, OnSetPageFocus)
END_MESSAGE_MAP()


// CUnits3PropertyPage message handlers

BOOL CUnits3PropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CUnits3PropertyPage::Contains(int nID)const
{
	switch (nID)
	{
		case IDC_FLUX_NUM_COMBO:
		case IDC_FLUX_DENOM_COMBO:
		case IDC_LEAKY_COND_NUM_COMBO:
		case IDC_LEAKY_COND_DENOM_COMBO:
		case IDC_LEAKY_THICK_COMBO:
		case IDC_WELL_DIAM_COMBO:
		case IDC_WELL_FLOW_NUM_COMBO:
		case IDC_WELL_FLOW_DENOM_COMBO:
		case IDC_WELL_DEPTH_COMBO:
			return TRUE;
	}
	return FALSE;
}

void CUnits3PropertyPage::SetControlFocus(int nID)
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


BOOL CUnits3PropertyPage::OnSetActive()
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

LRESULT CUnits3PropertyPage::OnSetPageFocus(WPARAM, LPARAM)
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
