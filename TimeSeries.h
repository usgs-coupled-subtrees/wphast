#ifndef _INC_TIMESERIES_
#define _INC_TIMESERIES_

#include <map>
#include "time.h"

template<typename T>
class CTimeSeries : public std::map<Ctime, T>
{
public:
	CTimeSeries& operator=(const struct time_series& rhs);
	void AssertValid(void)const;
};

#endif