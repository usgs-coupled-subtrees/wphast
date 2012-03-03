#include "StdAfx.h"
#include "NewModel.h"

CNewModel::CNewModel(void)
: m_bHaveSiteMap3(false)
// COMMENT: {7/18/2005 6:39:21 PM}, m_print_input_xy(true)
{
// COMMENT: {7/18/2005 6:39:19 PM}	for(int i = 0; i < 3; ++i)
// COMMENT: {7/18/2005 6:39:19 PM}	{	
// COMMENT: {7/18/2005 6:39:19 PM}		this->m_axes[i] = TRUE;
// COMMENT: {7/18/2005 6:39:19 PM}	}
}

CNewModel::~CNewModel(void)
{
}

CNewModel CNewModel::Default(void)
{
	CNewModel model;
	model.m_media     = CGridElt::NewDefaults();
	model.m_headIC    = CHeadIC::NewDefaults();
	model.m_chemIC    = CChemIC::NewDefaults();
	model.m_printFreq = CPrintFreq::NewDefaults();

// COMMENT: {7/18/2005 6:39:30 PM}	for(int i = 0; i < 3; ++i)
// COMMENT: {7/18/2005 6:39:30 PM}	{	
// COMMENT: {7/18/2005 6:39:30 PM}		model.m_grid[i].count_coord = 2;
// COMMENT: {7/18/2005 6:39:30 PM}		model.m_grid[i].uniform     = TRUE;
// COMMENT: {7/18/2005 6:39:30 PM}		model.m_grid[i].coord[0]    = 0;
// COMMENT: {7/18/2005 6:39:30 PM}		model.m_grid[i].coord[1]    = 1;
// COMMENT: {7/18/2005 6:39:30 PM}	}
// COMMENT: {7/18/2005 6:39:30 PM}	model.m_grid[0].c = 'x';
// COMMENT: {7/18/2005 6:39:30 PM}	model.m_grid[1].c = 'y';
// COMMENT: {7/18/2005 6:39:30 PM}	model.m_grid[2].c = 'z';

	model.m_timeControl2 = CTimeControl2::NewDefaults();

	return model;
}

bool CNewModel::HasSiteMap3(void)const
{
	return this->m_bHaveSiteMap3;
}

void CNewModel::SetSiteMap3(const CSiteMap3 &siteMap3)
{
	this->m_siteMap3 = siteMap3;
	this->m_bHaveSiteMap3 = true;
}

CSiteMap3 CNewModel::GetSiteMap3(void)const
{
	return this->m_siteMap3;
}
