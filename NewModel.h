#pragma once

#include "FlowOnly.h"
#include "Units.h"
#include "Grid.h"
#include "GridElt.h"
#include "HeadIC.h"
#include "TimeControl.h"
#include "SiteMap.h"
#include "SteadyFlow.h"
#include "FreeSurface.h"
#include "PrintFreq.h"
#include "ChemIC.h"

class CNewModel
{
public:
	CNewModel(void);
	~CNewModel(void);

	bool HasSiteMap(void)const;
	void SetSiteMap(const CSiteMap &siteMap);
	CSiteMap GetSiteMap()const;
	static CNewModel Default();

public:
	CFlowOnly    m_flowOnly;
	CSteadyFlow  m_steadyFlow;
	CFreeSurface m_freeSurface;
	CUnits       m_units;
	CGrid        m_grid[3];
	CGridElt     m_media;
	CHeadIC      m_headIC;
	CChemIC      m_chemIC;
	CPrintFreq	 m_printFreq;
	CTimeControl m_timeControl;

protected:
	CSiteMap     m_siteMap;
	bool         m_bHaveSiteMap;
};
