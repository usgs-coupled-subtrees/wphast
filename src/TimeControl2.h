#pragma once

#include "TimeSeries.h"
#include <set>

class CTimeControl2
{
public:
	CTimeControl2(void);
	CTimeControl2(const struct time_series& timeStep, const struct time *timeEnd, int nTimeEnd);
	CTimeControl2(const struct time_series& timeStep, const struct time *timeEnd, int nTimeEnd, const struct time& timeStart);

	~CTimeControl2(void);

	void Edit(CTreeCtrl* pTreeCtrl);
	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiTimeControl);
	void Serialize(bool bStoring, hid_t loc_id);
	friend std::ostream& operator<< (std::ostream &os, const CTimeControl2& tc2);

	static CTimeControl2 CTimeControl2::NewDefaults(void);

public:
	CTimeSeries<Ctime> m_timeStep;
	std::set<Ctime> m_timeEnd;
	Ctime m_timeStart;
protected:
	HTREEITEM m_htiTimeControl;
};
