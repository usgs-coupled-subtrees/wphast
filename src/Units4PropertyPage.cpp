// Units3PropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "Units4PropertyPage.h"

#include "Global.h"
#include <algorithm>
#include <locale>

// CUnits4PropertyPage dialog

IMPLEMENT_DYNAMIC(CUnits4PropertyPage, CPropertyPage)
CUnits4PropertyPage::CUnits4PropertyPage()
	: CPropertyPage(CUnits4PropertyPage::IDD)
	, m_idSetFocus(0)
{
}

CUnits4PropertyPage::CUnits4PropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CUnits4PropertyPage::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
	, m_idSetFocus(0)
{
}

CUnits4PropertyPage::~CUnits4PropertyPage()
{
}

void CUnits4PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RIVER_COND_NUM_COMBO, m_wndRiverBedKNumCombo);
	DDX_Control(pDX, IDC_RIVER_COND_DENOM_COMBO, m_wndRiverBedKDenomCombo);
	DDX_Control(pDX, IDC_RIVER_THICK_COMBO, m_wndRiverBedThickCombo);
	DDX_Control(pDX, IDC_RIVER_WIDTH_COMBO, m_wndRiverWidthCombo);
	DDX_Control(pDX, IDC_RIVER_DEPTH_COMBO, m_wndRiverDepthCombo);
	DDX_Control(pDX, IDC_DRAIN_COND_NUM_COMBO, m_wndDrainBedKNumCombo);
	DDX_Control(pDX, IDC_DRAIN_COND_DENOM_COMBO, m_wndDrainBedKDenomCombo);
	DDX_Control(pDX, IDC_DRAIN_THICK_COMBO, m_wndDrainBedThickCombo);
	DDX_Control(pDX, IDC_DRAIN_WIDTH_COMBO, m_wndDrainWidthCombo);

	// river_bed_k numerator
	if (m_wndRiverBedKNumCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndRiverBedKNumCombo);
	}
	// river_bed_k denominator
	if (m_wndRiverBedKDenomCombo.GetCount() == 0)
	{
		CGlobal::AddTimeUnitsDenom(&m_wndRiverBedKDenomCombo);
	}
	// river_bed_thickness
	if (m_wndRiverBedThickCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndRiverBedThickCombo);
	}
	// river_width
	if (m_wndRiverWidthCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndRiverWidthCombo);
	}
	// river_depth
	if (m_wndRiverDepthCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndRiverDepthCombo);
	}
	// drain_bed_k numerator
	if (m_wndDrainBedKNumCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndDrainBedKNumCombo);
	}
	// drain_bed_k denominator
	if (m_wndDrainBedKDenomCombo.GetCount() == 0)
	{
		CGlobal::AddTimeUnitsDenom(&m_wndDrainBedKDenomCombo);
	}
	// drain_bed_thickness
	if (m_wndDrainBedThickCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndDrainBedThickCombo);
	}
	// drain_width
	if (m_wndDrainWidthCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndDrainWidthCombo);
	}


	if (pDX->m_bSaveAndValidate)
	{
		CString numer;
		CString denom;
		CString value;

		// river_bed_k
		this->m_wndRiverBedKNumCombo.GetLBText(this->m_wndRiverBedKNumCombo.GetCurSel(), numer);
		this->m_wndRiverBedKDenomCombo.GetLBText(this->m_wndRiverBedKDenomCombo.GetCurSel(), denom);
		value = numer;
		value += "/";
		value += denom;
		VERIFY(this->m_units.river_bed_k.set_input(value) == OK);

		// river_bed_thickness
		this->m_wndRiverBedThickCombo.GetLBText(this->m_wndRiverBedThickCombo.GetCurSel(), value);
		VERIFY(this->m_units.river_bed_thickness.set_input(value) == OK);

		// river_width
		this->m_wndRiverWidthCombo.GetLBText(this->m_wndRiverWidthCombo.GetCurSel(), value);
		VERIFY(this->m_units.river_width.set_input(value) == OK);

		// river_depth
		this->m_wndRiverDepthCombo.GetLBText(this->m_wndRiverDepthCombo.GetCurSel(), value);
		VERIFY(this->m_units.river_depth.set_input(value) == OK);

		// drain_bed_k
		this->m_wndDrainBedKNumCombo.GetLBText(this->m_wndDrainBedKNumCombo.GetCurSel(), numer);
		this->m_wndDrainBedKDenomCombo.GetLBText(this->m_wndDrainBedKDenomCombo.GetCurSel(), denom);
		value = numer;
		value += "/";
		value += denom;
		VERIFY(this->m_units.drain_bed_k.set_input(value) == OK);

		// drain_bed_thickness
		this->m_wndDrainBedThickCombo.GetLBText(this->m_wndDrainBedThickCombo.GetCurSel(), value);
		VERIFY(this->m_units.drain_bed_thickness.set_input(value) == OK);

		// drain_width
		this->m_wndDrainWidthCombo.GetLBText(this->m_wndDrainWidthCombo.GetCurSel(), value);
		VERIFY(this->m_units.drain_width.set_input(value) == OK);
	}
	else
	{
		// river_bed_k [L/T]
		if (this->m_units.river_bed_k.defined)
		{
			std::string str(this->m_units.river_bed_k.input);
			std::string::size_type n = str.find('/');
			ASSERT(n != std::string::npos);
			std::string strNum = str.substr(0, n);
			std::string strDenom = str.substr(n+1, std::string::npos);
			// river_bed_k numerator/denominator
			VERIFY(this->m_wndRiverBedKNumCombo.SelectString(0, CGlobal::GetStdLengthUnits(strNum.c_str()).c_str()) != CB_ERR);
			VERIFY(this->m_wndRiverBedKDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strDenom.c_str()).c_str()) != CB_ERR);
		}
		else
		{
			std::string str(this->m_units.river_bed_k.si);
			std::string::size_type n = str.find('/');
			ASSERT(n != std::string::npos);
			std::string strNum = str.substr(0, n);
			std::string strDenom = str.substr(n+1, std::string::npos);
			// river_bed_k numerator/denominator
			VERIFY(this->m_wndRiverBedKNumCombo.SelectString(0, CGlobal::GetStdLengthUnits(strNum.c_str()).c_str()) != CB_ERR);
			VERIFY(this->m_wndRiverBedKDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strDenom.c_str()).c_str()) != CB_ERR);
		}

		// river_bed_thickness [L]
		if (this->m_units.river_bed_thickness.defined)
		{
			VERIFY(this->m_wndRiverBedThickCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.river_bed_thickness.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndRiverBedThickCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.river_bed_thickness.si).c_str()) != CB_ERR);
		}

		// river_width [L]
		if (this->m_units.river_width.defined)
		{
			VERIFY(this->m_wndRiverWidthCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.river_width.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndRiverWidthCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.river_width.si).c_str()) != CB_ERR);
		}

		// river_depth [L]
		if (this->m_units.river_depth.defined)
		{
			VERIFY(this->m_wndRiverDepthCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.river_depth.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndRiverDepthCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.river_depth.si).c_str()) != CB_ERR);
		}

		// drain_bed_k [L/T]
		if (this->m_units.drain_bed_k.defined)
		{
			std::string str(this->m_units.drain_bed_k.input);
			std::string::size_type n = str.find('/');
			ASSERT(n != std::string::npos);
			std::string strNum = str.substr(0, n);
			std::string strDenom = str.substr(n+1, std::string::npos);
			// drain_bed_k numerator/denominator
			VERIFY(this->m_wndDrainBedKNumCombo.SelectString(0, CGlobal::GetStdLengthUnits(strNum.c_str()).c_str()) != CB_ERR);
			VERIFY(this->m_wndDrainBedKDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strDenom.c_str()).c_str()) != CB_ERR);
		}
		else
		{
			std::string str(this->m_units.drain_bed_k.si);
			std::string::size_type n = str.find('/');
			ASSERT(n != std::string::npos);
			std::string strNum = str.substr(0, n);
			std::string strDenom = str.substr(n+1, std::string::npos);
			// drain_bed_k numerator/denominator
			VERIFY(this->m_wndDrainBedKNumCombo.SelectString(0, CGlobal::GetStdLengthUnits(strNum.c_str()).c_str()) != CB_ERR);
			VERIFY(this->m_wndDrainBedKDenomCombo.SelectString(0, CGlobal::GetStdTimeUnitsDenom(strDenom.c_str()).c_str()) != CB_ERR);
		}


		// drain_bed_thickness [L]
		if (this->m_units.drain_bed_thickness.defined)
		{
			VERIFY(this->m_wndDrainBedThickCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.drain_bed_thickness.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndDrainBedThickCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.drain_bed_thickness.si).c_str()) != CB_ERR);
		}

		// drain_width [L]
		if (this->m_units.drain_width.defined)
		{
			VERIFY(this->m_wndDrainWidthCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.drain_width.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndDrainWidthCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.drain_width.si).c_str()) != CB_ERR);
		}
	}
}

void CUnits4PropertyPage::SetProperties(const CUnits& r_units)
{
	this->m_units = r_units;
}

void CUnits4PropertyPage::GetProperties(CUnits& r_units)const
{
	// river_bed_k
	r_units.river_bed_k         = this->m_units.river_bed_k;
	// river_bed_thickness
	r_units.river_bed_thickness = this->m_units.river_bed_thickness;
	// river_width
	r_units.river_width         = this->m_units.river_width;
	// drain_bed_k
	r_units.drain_bed_k         = this->m_units.drain_bed_k;
	// drain_bed_thickness
	r_units.drain_bed_thickness = this->m_units.drain_bed_thickness;
	// drain_width
	r_units.drain_width         = this->m_units.drain_width;
	// river_depth
	r_units.river_depth         = this->m_units.river_depth;
}


BEGIN_MESSAGE_MAP(CUnits4PropertyPage, CPropertyPage)
    ON_MESSAGE(WM_SETPAGEFOCUS, OnSetPageFocus)
END_MESSAGE_MAP()


// CUnits4PropertyPage message handlers

BOOL CUnits4PropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CUnits4PropertyPage::Contains(int nID)const
{
	switch (nID)
	{
		case IDC_RIVER_COND_NUM_COMBO:
		case IDC_RIVER_COND_DENOM_COMBO:
		case IDC_RIVER_THICK_COMBO:
		case IDC_RIVER_WIDTH_COMBO:
		case IDC_DRAIN_COND_NUM_COMBO:
		case IDC_DRAIN_COND_DENOM_COMBO:
		case IDC_DRAIN_THICK_COMBO:
		case IDC_DRAIN_WIDTH_COMBO:
		case IDC_RIVER_DEPTH_COMBO:
			return TRUE;
	}
	return FALSE;
}

void CUnits4PropertyPage::SetControlFocus(int nID)
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


BOOL CUnits4PropertyPage::OnSetActive()
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

LRESULT CUnits4PropertyPage::OnSetPageFocus(WPARAM, LPARAM)
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
