#pragma once
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

class CWell : public Well
{
public:
	// ctor
	CWell(void);
	// dtor
	~CWell(void);
	// copy ctor
	CWell(const struct Well& src);
	CWell(const CWell& src);
	// copy assignment
	CWell& operator=(const Well& rhs);
	CWell& operator=(const CWell& rhs);
	// serialize
	void Serialize(bool bStoring, hid_t loc_id);
	void Reduce(void);
private:
	// helper functions
	void InternalCopy(const Well& src);
	void InternalDelete(void);
	void InternalInit(void);
public:
	CTimeSeries<Cproperty> m_q;
	CTimeSeries<Cproperty> m_solution;
};
