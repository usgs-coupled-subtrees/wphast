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
#include "property.h"
#include "TimeSeries.h"

class CBC :	public BC
{
public:
	// ctor
	CBC(void);
	// dtor
	~CBC(void);
	// copy ctor
	CBC(const struct BC& src);
	CBC(const CBC& src);
	// copy assignment
	CBC& operator=(const CBC& rhs); 
	// helper functions
#ifdef _DEBUG
	void AssertValid(int nSim/* = 1*/) const;
	void Dump(CDumpContext& dc)const;
#endif
	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);
	void Merge(const CBC& src);
	void ClearProperties(void);
	bool ContainsProperties(void)const;
	void RemoveMutableProperties(void);
	bool RemovePropZones(void);

	friend std::ostream& operator<< (std::ostream &os, const CBC &a);

private:
	void InternalCopy(const struct BC& src);
	void InternalDelete(void);
	void InternalInit(void);

public:
	CTimeSeries<Cproperty> m_bc_head;
	CTimeSeries<Cproperty> m_bc_flux;	
	CTimeSeries<Cproperty> m_bc_solution;

	static CLIPFORMAT clipFormat;	
};
