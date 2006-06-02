// NewModelWizard.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "NewModelWizard2.h"


// CNewModelWizard2

IMPLEMENT_DYNAMIC(CNewModelWizard2, CPropertySheet)


CNewModelWizard2::CNewModelWizard2(LPCTSTR pszCaption, CWnd* pParentWnd,
	UINT iSelectPage, HBITMAP hbmWatermark, HPALETTE hpalWatermark,
	HBITMAP hbmHeader)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage, hbmWatermark, hpalWatermark, hbmHeader)
	, m_introPage()
	, m_units1Page(IDS_UNITS1_CAPTION_137,  IDS_UNITS1_TITLE_138,  IDS_UNITS1_SUBTITLE_139)
	, m_units2Page(IDS_UNITS2_CAPTION_140,  IDS_UNITS2_TITLE_141,  IDS_UNITS2_SUBTITLE_142)
	, m_gridPage  (IDS_GRID_CAPTION_143,    IDS_GRID_TITLE_144,    IDS_GRID_SUBTITLE_145)
	, m_mediaPage (IDS_MEDIA_CAPTION_146,   IDS_MEDIA_TITLE_147,   IDS_MEDIA_SUBTITLE_148)
	, m_icHeadPage(IDS_IC_HEAD_CAPTION_149, IDS_IC_HEAD_TITLE_150, IDS_IC_HEAD_SUBTITLE_151)
	, m_tcPage    (IDS_TC_CAPTION_152,      IDS_TC_TITLE_153,      IDS_TC_SUBTITLE_154)
{
	// add all the pages of the wizard
	AddPage(&this->m_introPage);
	AddPage(&this->m_units1Page);
	AddPage(&this->m_units2Page);
	AddPage(&this->m_gridPage);
	AddPage(&this->m_mediaPage);
	AddPage(&this->m_icHeadPage);
	AddPage(&this->m_tcPage);	

	// set the WIZARD97 flag
	m_psh.dwFlags |= PSH_WIZARD97|PSH_NOAPPLYNOW;
}

CNewModelWizard2::~CNewModelWizard2()
{
}


BEGIN_MESSAGE_MAP(CNewModelWizard2, CPropertySheet)
END_MESSAGE_MAP()

CFlowOnly CNewModelWizard2::GetFlowOnly(void)const
{
	return this->m_introPage.GetFlowOnly();
}

CUnits CNewModelWizard2::GetUnits(void)const
{
	CUnits units;
	this->m_units1Page.GetProperties(units);
	this->m_units2Page.GetProperties(units);
	return units;
}

CGrid CNewModelWizard2::GetGridX(void)const
{
	CGrid grid(this->m_gridPage.m_grid[0]);
	return grid;
}

CGrid CNewModelWizard2::GetGridY(void)const
{
	CGrid grid(this->m_gridPage.m_grid[1]);
	return grid;
}

CGrid CNewModelWizard2::GetGridZ(void)const
{
	CGrid grid(this->m_gridPage.m_grid[2]);
	return grid;
}

CGridElt CNewModelWizard2::GetGridElt(void)const
{
	CGridElt gridElt;
	this->m_mediaPage.GetProperties(gridElt);
	return gridElt;
}

CHeadIC CNewModelWizard2::GetHeadIC(void)const
{
	CHeadIC headIC;
	this->m_icHeadPage.GetProperties(headIC);
	return headIC;
}

void CNewModelWizard2::SetGridElt(const CGridElt& rGridElt)
{
	this->m_mediaPage.SetProperties(rGridElt);
}

void CNewModelWizard2::SetHeadIC(const CHeadIC& rHeadIC)
{
	this->m_icHeadPage.SetProperties(rHeadIC);
}

CTimeControl2 CNewModelWizard2::GetTimeControl2(void)const
{
	CTimeControl2 timeControl2;
	this->m_tcPage.GetProperties(timeControl2);
	return timeControl2;
}

// CNewModelWizard2 message handlers

//INT_PTR CNewModelWizard2::DoModal()
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	return CPropertySheet::DoModal();
//}
