#pragma once

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
#include "enum_fix.h"

#include <iosfwd> // std::ostream

class CSnap
{
public:
	CSnap(void);
	~CSnap(void);
	CSnap& operator=(const double rhs[3]);
	double& operator[](size_t index);
public:
	double m_snap[3];
};
