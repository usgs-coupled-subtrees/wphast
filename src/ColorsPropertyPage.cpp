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
// COMMENT: {6/8/2006 6:14:25 PM}	this->m_colors.crMedia = RGB(0, 128, 0);

	// Head ic (turquoise)
	this->m_headICColorPicker.SetDefaultText(_T(""));
// COMMENT: {6/8/2006 6:14:27 PM}	this->m_colors.crHeadIC = RGB(0, 255, 255);

	// Chem ic (yellow)
	this->m_chemICColorPicker.SetDefaultText(_T(""));
// COMMENT: {6/8/2006 6:14:30 PM}	this->m_colors.crChemIC = RGB(255, 255, 0);

	// Flux (orange)
	this->m_fluxColorPicker.SetDefaultText(_T(""));
// COMMENT: {6/8/2006 6:14:33 PM}	this->m_colors.crFlux = RGB(255, 102, 0);

	// Leaky (red)
	this->m_leakyColorPicker.SetDefaultText(_T(""));
// COMMENT: {6/8/2006 6:14:35 PM}	this->m_colors.crLeaky = RGB(255, 0, 0);

	// Spec head (brown)
	this->m_specHeadColorPicker.SetDefaultText(_T(""));
// COMMENT: {6/8/2006 6:14:37 PM}	this->m_colors.crSpecHead = RGB(153, 51, 0);

	// River (blue)
	this->m_riverColorPicker.SetDefaultText(_T(""));
// COMMENT: {6/8/2006 6:14:40 PM}	this->m_colors.crRiver = RGB(0, 0, 255);

	// Wells (light purple)
	this->m_wellColorPicker.SetDefaultText(_T(""));
// COMMENT: {6/8/2006 6:14:43 PM}	this->m_colors.crWell = RGB(204, 153, 255);
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

	// Wells
    DDX_Control(pDX, IDC_WELL_BTN, m_wellColorPicker);
    DDX_ColourPicker(pDX, IDC_WELL_BTN, m_colors.crWell);
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
