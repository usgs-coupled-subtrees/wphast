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


class CHeadIC : public Head_ic
{
public:
	// ctor
	CHeadIC(void);
	// dtor
	virtual ~CHeadIC(void);
	// copy ctor
	CHeadIC(const struct Head_ic& src);
	CHeadIC(const CHeadIC& src);
	// copy assignment
	CHeadIC& operator=(const CHeadIC& rhs); 

	bool operator==(const Head_ic& rhs)const;

	static CHeadIC NewDefaults(void);
	static CHeadIC Full(void);

	bool RemovePropZones(void);

	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);
	friend std::ostream& operator<< (std::ostream &os, const CHeadIC &a);

	static CLIPFORMAT clipFormat;

private:
	void InternalCopy(const struct Head_ic& src);
	void InternalDelete(void);
	void InternalInit(void);
};
