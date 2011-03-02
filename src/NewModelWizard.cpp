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
	, m_units3Page(IDS_UNITS3_CAPTION_164,  IDS_UNITS3_TITLE_165,  IDS_UNITS3_SUBTITLE_166)
	, m_units4Page(IDS_UNITS4_CAPTION_167,  IDS_UNITS4_TITLE_168,  IDS_UNITS4_SUBTITLE_169)
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
	AddPage(&this->m_units3Page);
	AddPage(&this->m_units4Page);
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
	CNewModel model = CNewModel::Default();
	model.m_flowOnly = this->m_introPage.GetFlowOnly();
	model.m_freeSurface = this->m_introPage.GetFreeSurface();
	model.m_steadyFlow = this->m_introPage.GetSteadyFlow();
	this->m_units1Page.GetProperties(model.m_units);
	this->m_units2Page.GetProperties(model.m_units);
	this->m_units3Page.GetProperties(model.m_units);
	this->m_units4Page.GetProperties(model.m_units);
	if (this->m_importPage.m_bImport)
	{
		CSiteMap2 siteMap2 = this->m_importPage.GetSiteMap2();
		siteMap2.FileName = (LPCTSTR)this->m_importPage.m_strMapFileName;
		double pt[3] = {0.0, 0.0, 0.0};
		pt[2] = this->m_gridPage.GridKeyword.m_grid[2].coord[0] - .01 *(this->m_gridPage.GridKeyword.m_grid[2].coord[1] - this->m_gridPage.GridKeyword.m_grid[2].coord[0]);
		vtkTransform *trans = vtkTransform::New();
		trans->Scale(
			model.m_units.horizontal.input_to_si,
			model.m_units.horizontal.input_to_si,
			model.m_units.vertical.input_to_si);
		trans->TransformPoint(pt, pt);
		siteMap2.Origin[2] = pt[2];
		trans->Delete();
		model.SetSiteMap2(siteMap2);
	}
	model.m_gridKeyword = this->m_gridPage.GridKeyword;
	this->m_mediaPage.GetProperties(model.m_media);
	this->m_icHeadPage.GetProperties(model.m_headIC);
	this->m_chemICPage.GetProperties(model.m_chemIC);
	this->m_tcPage.GetProperties(model.m_timeControl2);
	return model;
}

CFlowOnly CNewModelWizard::GetFlowOnly(void)const
{
	return this->m_introPage.GetFlowOnly();
}

CUnits CNewModelWizard::GetUnits(void)const
{
	CUnits units;
	this->m_units1Page.GetProperties(units);
	this->m_units2Page.GetProperties(units);
	this->m_units3Page.GetProperties(units);
	this->m_units4Page.GetProperties(units);
	return units;
}

CGrid CNewModelWizard::GetGridX(void)const
{
	CGrid grid(this->m_gridPage.GridKeyword.m_grid[0]);
	return grid;
}

CGrid CNewModelWizard::GetGridY(void)const
{
	CGrid grid(this->m_gridPage.GridKeyword.m_grid[1]);
	return grid;
}

CGrid CNewModelWizard::GetGridZ(void)const
{
	CGrid grid(this->m_gridPage.GridKeyword.m_grid[2]);
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
