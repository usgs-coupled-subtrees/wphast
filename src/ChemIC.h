#pragma once

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL

#include <iosfwd> // std::ostream

class CChemIC : public chem_ic
{
public:
	// ctor
	CChemIC(void);
	// dtor
	~CChemIC(void);
	// copy ctor
	CChemIC(const struct chem_ic& src);
	CChemIC(const CChemIC& src);
	// copy assignment
	CChemIC& operator=(const CChemIC& rhs); 

	static CChemIC NewDefaults(void);
	static CChemIC Full(void);

	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);
	friend std::ostream& operator<< (std::ostream &os, const CChemIC &a);

	static CLIPFORMAT clipFormat;

private:
	void InternalCopy(const struct chem_ic& src);
	void InternalDelete(void);
	void InternalInit(void);
};
