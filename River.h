#pragma once

#include <list>

extern "C" {
#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
}

#include <iosfwd> // std::ostream
#include "TimeSeries.h"
#include "property.h"


class CRiverPoint
{
public:
	double x;
	double y;
	double z;
	CTimeSeries<Cproperty> m_head;
	CTimeSeries<Cproperty> m_solution;
};

class CRiver
{
public:
	CRiver(void);
	CRiver(const River &r);
	~CRiver(void);

public:
	int n_user;
	std::string description;
	std::list<CRiverPoint> m_listPoints;
};
