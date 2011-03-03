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

class CUnits : public cunits
{
public:
	// ctor
	CUnits(void);

	// copy ctor
	CUnits(const struct cunits& src);

	// copy assignment
	CUnits& operator=(const CUnits& rhs);
// COMMENT: {12/19/2008 9:41:32 PM}	CUnits& CUnits::operator=(const struct cunits& rhs);

	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiUnits);
	void Serialize(bool bStoring, hid_t loc_id);
	void Edit(CTreeCtrl* pTreeCtrl);
	HTREEITEM GetTreeItem(void)const { return m_htiUnits; }

	SOLID_UNITS exchange_units;
	SOLID_UNITS surface_units;
	SOLID_UNITS ssassemblage_units;
	SOLID_UNITS ppassemblage_units;
	SOLID_UNITS gasphase_units;
	SOLID_UNITS kinetics_units;

protected:
	friend std::ostream& operator<< (std::ostream &os, const CUnits &a);
	HTREEITEM m_htiUnits;
};
