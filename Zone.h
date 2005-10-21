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

class CZone : public zone {
public:
	// ctor
	CZone(void);
	// dtor
	~CZone(void);
	// copy ctor
	CZone(const struct zone& src);
	CZone(const CZone& src);

	bool operator==(const struct zone& rhs)const throw();

	// helper functions
#ifdef _DEBUG
	void AssertValid(void) const;
	void Dump(CDumpContext& dc)const;
#endif
	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);
	friend std::ostream& operator<< (std::ostream &os, const CZone &a);
protected:
	void InternalCopy(const zone& src);
};

