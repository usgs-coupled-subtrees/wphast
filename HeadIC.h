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


class CHeadIC :
	public head_ic
{
public:
	// ctor
	CHeadIC(void);
	// dtor
	virtual ~CHeadIC(void);
	// copy ctor
	CHeadIC(const struct head_ic& src);
	CHeadIC(const CHeadIC& src);
	// copy assignment
	CHeadIC& operator=(const CHeadIC& rhs); 

	static CHeadIC NewDefaults(void);
	static CHeadIC Full(void);

	void Serialize(bool bStoring, hid_t loc_id);
	friend std::ostream& operator<< (std::ostream &os, const CHeadIC &a);


private:
	void InternalCopy(const struct head_ic& src);
	void InternalDelete(void);
	void InternalInit(void);
};
