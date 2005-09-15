// NewModelWizard.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "NewModelWizard.h"


// CNewModelWizard

IMPLEMENT_DYNAMIC(CNewModelWizard, CPropertySheet)


CNewModelWizard::CNewModelWizard(LPCTSTR pszCaption, CWnd* pParentWnd,
	UINT iSelectPage, HBITMAP hbmWatermark, HPALETTE hpalWatermark,
	HBITMAP hbmHeader)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage, hbmWatermark, hpalWatermark, hbmHeader)
	, m_introPage()
	, m_units1Page(IDS_UNITS1_CAPTION_137,  IDS_UNITS1_TITLE_138,  IDS_UNITS1_SUBTITLE_139)
	, m_units2Page(IDS_UNITS2_CAPTION_140,  IDS_UNITS2_TITLE_141,  IDS_UNITS2_SUBTITLE_142)
	, m_importPage(IDS_IMPMAP_CAPTION_155,  IDS_IMPMAP_TITLE_156,  IDS_IMPMAP_SUBTITLE_157)
	, m_gridPage  (IDS_GRID_CAPTION_143,    IDS_GRID_TITLE_144,    IDS_GRID_SUBTITLE_145)
	, m_mediaPage (IDS_MEDIA_CAPTION_146,   IDS_MEDIA_TITLE_147,   IDS_MEDIA_SUBTITLE_148)
	, m_icHeadPage(IDS_IC_HEAD_CAPTION_149, IDS_IC_HEAD_TITLE_150, IDS_IC_HEAD_SUBTITLE_151)
	, m_chemICPage(CChemICPropertyPage::IDD_INTERIOR, IDS_CHEM_IC_CAPTION_158, IDS_CHEM_IC_TITLE_159, IDS_CHEM_IC_SUBTITLE_160)
	, m_tcPage    (IDS_TC_CAPTION_152,      IDS_TC_TITLE_153,      IDS_TC_SUBTITLE_154)
{
	this->m_mediaPage.SetNewModelWizard(true);
	this->m_chemICPage.SetNewModelWizard(true);

	// add all the pages of the wizard
	AddPage(&this->m_introPage);
	AddPage(&this->m_units1Page);
	AddPage(&this->m_units2Page);
	AddPage(&this->m_importPage);
	AddPage(&this->m_gridPage);
	AddPage(&this->m_mediaPage);
	AddPage(&this->m_icHeadPage);
	AddPage(&this->m_chemICPage);
	AddPage(&this->m_tcPage);	

	// set the WIZARD97 flag
	m_psh.dwFlags |= PSH_WIZARD97|PSH_NOAPPLYNOW;
}

CNewModelWizard::~CNewModelWizard()
{
}


BEGIN_MESSAGE_MAP(CNewModelWizard, CPropertySheet)
END_MESSAGE_MAP()

CNewModel CNewModelWizard::GetModel(void)const
{
	CNewModel model;
	model.m_flowOnly = this->m_introPage.GetFlowOnly();
	model.m_freeSurface = this->m_introPage.GetFreeSurface();
	model.m_steadyFlow = this->m_introPage.GetSteadyFlow();
	this->m_units1Page.GetProperties(model.m_units);
	this->m_units2Page.GetProperties(model.m_units);
	if (this->m_importPage.m_bImport)
	{
		CSiteMap site = this->m_importPage.GetSiteMap();
		site.m_fileName = (LPCTSTR)this->m_importPage.m_strMapFileName;
		site.m_placement[2] = this->m_gridPage.m_grid[2].coord[0] - .01 *(this->m_gridPage.m_grid[2].coord[1] - this->m_gridPage.m_grid[2].coord[0]);
		model.SetSiteMap(site);
	}
	for (int i = 0; i < 3; ++i)
	{
		model.m_gridKeyword.m_grid[i] = this->m_gridPage.m_grid[i];
	}
	this->m_mediaPage.GetProperties(model.m_media);
	this->m_icHeadPage.GetProperties(model.m_headIC);
	this->m_chemICPage.GetProperties(model.m_chemIC);
// COMMENT: {4/22/2005 3:43:04 PM}	this->m_tcPage.GetProperties(model.m_timeControl);
	this->m_tcPage.GetProperties(model.m_timeControl2);
	return model;
}

CFlowOnly CNewModelWizard::GetFlowOnly(void)const
{
	return this->m_introPage.GetFlowOnly();
}

// COMMENT: {6/1/2004 1:48:56 PM}void CNewModelWizard::SetFlowOnly(const CFlowOnly &flowOnly)
// COMMENT: {6/1/2004 1:48:56 PM}{
// COMMENT: {6/1/2004 1:48:56 PM}	this->m_introPage.SetFlowOnly(flowOnly);
// COMMENT: {6/1/2004 1:48:56 PM}// COMMENT: {6/1/2004 1:40:18 PM}	if (!flowOnly) {
// COMMENT: {6/1/2004 1:48:56 PM}// COMMENT: {6/1/2004 1:40:18 PM}		CGridElt gridElt;
// COMMENT: {6/1/2004 1:48:56 PM}// COMMENT: {6/1/2004 1:40:18 PM}		this->m_mediaPage.GetProperties(gridElt);
// COMMENT: {6/1/2004 1:48:56 PM}// COMMENT: {6/1/2004 1:40:18 PM}		if (!gridElt.alpha_horizontal) {
// COMMENT: {6/1/2004 1:48:56 PM}// COMMENT: {6/1/2004 1:40:18 PM}			gridElt.alpha_horizontal = new Property();
// COMMENT: {6/1/2004 1:48:56 PM}// COMMENT: {6/1/2004 1:40:18 PM}		}
// COMMENT: {6/1/2004 1:48:56 PM}// COMMENT: {6/1/2004 1:40:18 PM}	}
// COMMENT: {6/1/2004 1:48:56 PM}}

CUnits CNewModelWizard::GetUnits(void)const
{
	CUnits units;
	this->m_units1Page.GetProperties(units);
	this->m_units2Page.GetProperties(units);
	return units;
}

CGrid CNewModelWizard::GetGridX(void)const
{
	CGrid grid(this->m_gridPage.m_grid[0]);
	return grid;
}

CGrid CNewModelWizard::GetGridY(void)const
{
	CGrid grid(this->m_gridPage.m_grid[1]);
	return grid;
}

CGrid CNewModelWizard::GetGridZ(void)const
{
	CGrid grid(this->m_gridPage.m_grid[2]);
	return grid;
}

CGridElt CNewModelWizard::GetGridElt(void)const
{
	CGridElt gridElt;
	this->m_mediaPage.GetProperties(gridElt);
	return gridElt;
}

CHeadIC CNewModelWizard::GetHeadIC(void)const
{
	CHeadIC headIC;
	this->m_icHeadPage.GetProperties(headIC);
	return headIC;
}

void CNewModelWizard::SetGridElt(const CGridElt& rGridElt)
{
	this->m_mediaPage.SetProperties(rGridElt);
}

void CNewModelWizard::SetHeadIC(const CHeadIC& rHeadIC)
{
	this->m_icHeadPage.SetProperties(rHeadIC);
}

void CNewModelWizard::SetChemIC(const CChemIC& chemIC)
{
	this->m_chemICPage.SetProperties(chemIC);
}

CTimeControl2 CNewModelWizard::GetTimeControl2(void)const
{
	CTimeControl2 timeControl2;
	this->m_tcPage.GetProperties(timeControl2);
	return timeControl2;
}

// CNewModelWizard message handlers

//INT_PTR CNewModelWizard::DoModal()
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	return CPropertySheet::DoModal();
//}
