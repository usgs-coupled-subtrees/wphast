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

class CPrintZone : public print_zones
{
public:
	// ctor
	CPrintZone(void);
	// dtor
	~CPrintZone(void);
	// copy ctor
	CPrintZone(const struct print_zones& src);
	CPrintZone(const CPrintZone& src);
	// copy assignment
	CPrintZone& operator=(const CPrintZone& rhs); 

	static CPrintZone NewDefaults(void);
	static CPrintZone Full(void);

	bool operator==(const struct print_zones& rhs)const;

	bool RemovePropZones(void);

	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);
	friend std::ostream& operator<< (std::ostream &os, const CPrintZone &a);

	static CLIPFORMAT clipFormat;

private:
	void InternalCopy(const struct print_zones& src);
	void InternalDelete(void);
	void InternalInit(void);
};
