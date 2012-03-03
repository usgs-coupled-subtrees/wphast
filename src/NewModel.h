#pragma once

#include "FlowOnly.h"
#include "Units.h"
//#include "Grid.h"
#include "GridKeyword.h"
#include "GridElt.h"
#include "HeadIC.h"
#include "TimeControl2.h"
#include "SiteMap3.h"
#include "SteadyFlow.h"
#include "FreeSurface.h"
#include "PrintInput.h"
#include "PrintFreq.h"
#include "ChemIC.h"
#include "Snap.h"
#include "SolutionMethod.h"
#include "Title.h"

class CNewModel
{
public:
	CNewModel(void);
	~CNewModel(void);

	bool HasSiteMap3(void)const;
	void SetSiteMap3(const CSiteMap3 &siteMap3);
	CSiteMap3 GetSiteMap3()const;
	static CNewModel Default();

public:
	CTitle           m_title;
	CFlowOnly        m_flowOnly;
	CSteadyFlow      m_steadyFlow;
	CFreeSurface     m_freeSurface;
	CUnits           m_units;
	CGridKeyword     m_gridKeyword;
	CGridElt         m_media;
	CHeadIC          m_headIC;
	CChemIC          m_chemIC;
	CPrintInput      m_printInput;
	CPrintFreq	     m_printFreq;
	CTimeControl2    m_timeControl2;
	CSolutionMethod  m_solutionMethod;

protected:
	CSiteMap3        m_siteMap3;
	bool             m_bHaveSiteMap3;
};
