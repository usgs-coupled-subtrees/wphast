// Units5PropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "Units5PropertyPage.h"

#include "Global.h"


// CUnits5PropertyPage dialog

IMPLEMENT_DYNAMIC(CUnits5PropertyPage, CPropertyPage)

CUnits5PropertyPage::CUnits5PropertyPage()
	: CPropertyPage(CUnits5PropertyPage::IDD)
	, m_idSetFocus(0)
{
}

CUnits5PropertyPage::CUnits5PropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CUnits5PropertyPage::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
	, m_idSetFocus(0)
{
}

CUnits5PropertyPage::~CUnits5PropertyPage()
{
}

void CUnits5PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SU_PPASSEMBLAGE_COMBO, this->ppassemblageCombo);
	DDX_Control(pDX, IDC_SU_EXCHANGE_COMBO,     this->exchangeCombo);
	DDX_Control(pDX, IDC_SU_SURFACE_COMBO,      this->surfaceCombo);
	DDX_Control(pDX, IDC_SU_SSASSEMBLAGE_COMBO, this->ssassemblageCombo);
	DDX_Control(pDX, IDC_SU_KINETICS_COMBO,     this->kineticsCombo);
	DDX_Control(pDX, IDC_SU_GASPHASE_COMBO,     this->gasphaseCombo);

	// equilibrium_phases
	CGlobal::AddSolidUnits(&this->ppassemblageCombo);
	// exchange
	CGlobal::AddSolidUnits(&this->exchangeCombo);
	// surface
	CGlobal::AddSolidUnits(&this->surfaceCombo);
	// solid_solutions
	CGlobal::AddSolidUnits(&this->ssassemblageCombo);
	// kinetics
	CGlobal::AddSolidUnits(&this->kineticsCombo);
	// gas_phase
	CGlobal::AddSolidUnits(&this->gasphaseCombo);

	if (pDX->m_bSaveAndValidate)
	{
		// equilibrium_phases
		this->m_units.ppassemblage_units = CGlobal::GetSolidUnits(&this->ppassemblageCombo);
		// exchange
		this->m_units.exchange_units     = CGlobal::GetSolidUnits(&this->exchangeCombo);
		// surface
		this->m_units.surface_units      = CGlobal::GetSolidUnits(&this->surfaceCombo);
		// solid_solutions
		this->m_units.ssassemblage_units = CGlobal::GetSolidUnits(&this->ssassemblageCombo);
		// kinetics
		this->m_units.kinetics_units     = CGlobal::GetSolidUnits(&this->kineticsCombo);
		// gas_phase
		this->m_units.gasphase_units     = CGlobal::GetSolidUnits(&this->gasphaseCombo);
	}
	else
	{
		// equilibrium_phases
		CGlobal::SetSolidUnits(&this->ppassemblageCombo, this->m_units.ppassemblage_units);
		// exchange
		CGlobal::SetSolidUnits(&this->exchangeCombo,     this->m_units.exchange_units);
		// surface
		CGlobal::SetSolidUnits(&this->surfaceCombo,      this->m_units.surface_units);
		// solid_solutions
		CGlobal::SetSolidUnits(&this->ssassemblageCombo, this->m_units.ssassemblage_units);
		// kinetics
		CGlobal::SetSolidUnits(&this->kineticsCombo,     this->m_units.kinetics_units);
		// gas_phase
		CGlobal::SetSolidUnits(&this->gasphaseCombo,     this->m_units.gasphase_units);
	}
}

void CUnits5PropertyPage::SetProperties(const CUnits& r_units)
{
	this->m_units = r_units;
}

void CUnits5PropertyPage::GetProperties(CUnits& r_units)const
{
	// equilibrium_phases
	r_units.ppassemblage_units = this->m_units.ppassemblage_units;
	// exchange
	r_units.exchange_units     = this->m_units.exchange_units;
	// surface
	r_units.surface_units      = this->m_units.surface_units;
	// solid_solutions
	r_units.ssassemblage_units = this->m_units.ssassemblage_units;
	// kinetics
	r_units.kinetics_units     = this->m_units.kinetics_units;
	// gas_phase
	r_units.gasphase_units     = this->m_units.gasphase_units;
}

BEGIN_MESSAGE_MAP(CUnits5PropertyPage, CPropertyPage)
    ON_MESSAGE(WM_SETPAGEFOCUS, OnSetPageFocus)
END_MESSAGE_MAP()


// CUnits5PropertyPage message handlers

BOOL CUnits5PropertyPage::Contains(int nID)const
{
	switch (nID)
	{
		case IDC_SU_PPASSEMBLAGE_COMBO:
		case IDC_SU_EXCHANGE_COMBO:
		case IDC_SU_SURFACE_COMBO:
		case IDC_SU_SSASSEMBLAGE_COMBO:
		case IDC_SU_KINETICS_COMBO:
		case IDC_SU_GASPHASE_COMBO:
			return TRUE;
	}
	return FALSE;
}

void CUnits5PropertyPage::SetControlFocus(int nID)
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

BOOL CUnits5PropertyPage::OnSetActive()
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

LRESULT CUnits5PropertyPage::OnSetPageFocus(WPARAM, LPARAM)
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

BOOL CUnits5PropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
