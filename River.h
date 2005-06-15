#pragma once

#include <list>

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

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
public:
	void Serialize(bool bStoring, hid_t loc_id);
};

class CRiver
{
public:
	CRiver(void);
	CRiver(const River &r);
	~CRiver(void);

	void Serialize(bool bStoring, hid_t loc_id);

public:
	int n_user;
	std::string description;
	std::list<CRiverPoint> m_listPoints;
};
