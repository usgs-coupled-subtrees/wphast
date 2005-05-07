#include "StdAfx.h"
#include "NewModel.h"

CNewModel::CNewModel(void)
: m_bHaveSiteMap(false)
{
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

	for(int i = 0; i < 3; ++i)
	{	
		model.m_grid[i].count_coord = 2;
		model.m_grid[i].uniform     = TRUE;
		model.m_grid[i].coord[0]    = 0;
		model.m_grid[i].coord[1]    = 1;
	}
	model.m_grid[0].c = 'x';
	model.m_grid[1].c = 'y';
	model.m_grid[2].c = 'z';

// COMMENT: {4/22/2005 3:43:39 PM}	model.m_timeControl.SetTimeStepValue(1.0);
// COMMENT: {4/22/2005 3:43:39 PM}	model.m_timeControl.SetTimeEndValue(1.0);

	return model;
}

bool CNewModel::HasSiteMap(void)const
{
	return this->m_bHaveSiteMap;
}

void CNewModel::SetSiteMap(const CSiteMap &siteMap)
{
	this->m_siteMap = siteMap;
	this->m_bHaveSiteMap = true;
}

CSiteMap CNewModel::GetSiteMap(void)const
{
	return this->m_siteMap;
}
