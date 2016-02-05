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

class CPrintLoc : public print_zones_struct
{
public:
	// ctor
	CPrintLoc(void);
	// dtor
	~CPrintLoc(void);
	// copy ctor
	CPrintLoc(const struct print_zones_struct& src);
	CPrintLoc(const CPrintLoc& src);
	// copy assignment
	CPrintLoc& operator=(const CPrintLoc& rhs); 

	static CLIPFORMAT clipFormat;
private:
	void InternalCopy(const struct print_zones_struct& src);
	void InternalDelete(void);
	void InternalInit(void);
};
