// ColorsPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ColorsPropertyPage.h"


// CColorsPropertyPage dialog

IMPLEMENT_DYNAMIC(CColorsPropertyPage, CPropertyPage)

CColorsPropertyPage::CColorsPropertyPage()
	: CPropertyPage(CColorsPropertyPage::IDD)
{
	// Media (green)
	this->m_mediaColorPicker.SetDefaultText(_T(""));

	// Head ic (turquoise)
	this->m_headICColorPicker.SetDefaultText(_T(""));

	// Chem ic (yellow)
	this->m_chemICColorPicker.SetDefaultText(_T(""));

	// Flux (orange)
	this->m_fluxColorPicker.SetDefaultText(_T(""));

	// Leaky (red)
	this->m_leakyColorPicker.SetDefaultText(_T(""));

	// Spec head (brown)
	this->m_specHeadColorPicker.SetDefaultText(_T(""));

	// River (blue)
	this->m_riverColorPicker.SetDefaultText(_T(""));

	// Drain (green)
	this->m_drainColorPicker.SetDefaultText(_T(""));

	// Wells (light purple)
	this->m_wellColorPicker.SetDefaultText(_T(""));

	// Zone flow (magenta)
	this->m_zoneFlowColorPicker.SetDefaultText(_T(""));

	// Background (black)
	this->m_bgColorPicker.SetDefaultText(_T(""));	
}

CColorsPropertyPage::~CColorsPropertyPage()
{
}

void CColorsPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	// Media
    DDX_Control(pDX, IDC_MEDIA_BTN, m_mediaColorPicker);
    DDX_ColourPicker(pDX, IDC_MEDIA_BTN, m_colors.crMedia);

	// Head IC
    DDX_Control(pDX, IDC_HEAD_IC_BTN, m_headICColorPicker);
    DDX_ColourPicker(pDX, IDC_HEAD_IC_BTN, m_colors.crICHead);

	// Chem IC
    DDX_Control(pDX, IDC_CHEM_IC_BTN, m_chemICColorPicker);
    DDX_ColourPicker(pDX, IDC_CHEM_IC_BTN, m_colors.crICChem);

	// Flux
    DDX_Control(pDX, IDC_FLUX_BTN, m_fluxColorPicker);
    DDX_ColourPicker(pDX, IDC_FLUX_BTN, m_colors.crFlux);

	// Leaky
    DDX_Control(pDX, IDC_LEAKY_BTN, m_leakyColorPicker);
    DDX_ColourPicker(pDX, IDC_LEAKY_BTN, m_colors.crLeaky);

	// Spec Head
    DDX_Control(pDX, IDC_SPEC_HEAD_BTN, m_specHeadColorPicker);
    DDX_ColourPicker(pDX, IDC_SPEC_HEAD_BTN, m_colors.crSpecHead);

	// River
    DDX_Control(pDX, IDC_RIVER_BTN, m_riverColorPicker);
    DDX_ColourPicker(pDX, IDC_RIVER_BTN, m_colors.crRiver);

	// Drain
    DDX_Control(pDX, IDC_DRAIN_BTN, m_drainColorPicker);
    DDX_ColourPicker(pDX, IDC_DRAIN_BTN, m_colors.crDrain);

	// Wells
    DDX_Control(pDX, IDC_WELL_BTN, m_wellColorPicker);
    DDX_ColourPicker(pDX, IDC_WELL_BTN, m_colors.crWell);

	// Zone flow (magenta)
    DDX_Control(pDX, IDC_ZONE_FLOW_BTN, m_zoneFlowColorPicker);
    DDX_ColourPicker(pDX, IDC_ZONE_FLOW_BTN, m_colors.crZoneFlowRate);

	// Background
    DDX_Control(pDX, IDC_BG_BTN, m_bgColorPicker);
    DDX_ColourPicker(pDX, IDC_BG_BTN, m_colors.crBackground);
}


BEGIN_MESSAGE_MAP(CColorsPropertyPage, CPropertyPage)
    ///ON_MESSAGE(CPN_SELCHANGE, OnSelChange)
    ON_MESSAGE(CPN_SELENDOK, OnSelChange)	
END_MESSAGE_MAP()


// CColorsPropertyPage message handlers

LRESULT CColorsPropertyPage::OnSelChange(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    TRACE0("OnSelEndOK\n");
    return TRUE;
}
