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

class CGrid : public grid
{
public:
	// ctor
	CGrid();
	CGrid(double minimum, double maximum, int nodes); // NOTE: variable names min and max confuse Intellisense
	// dtor
	~CGrid();
	// copy ctor
	CGrid(const CGrid& p);
	CGrid(const struct grid& p);
	// copy assignment
	CGrid& operator=(const CGrid& rhs); 
	// helper functions
	void Setup();
	void SetUniformRange(double minimum, double maximum, int count_coord);
	void Serialize(bool bStoring, hid_t loc_id);
	void Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiGrid);
#ifdef _DEBUG
	void Dump(CDumpContext& dc)const;
#endif

	friend std::ostream& operator<< (std::ostream &os, const CGrid &a);

private:
	void InternalCopy(const struct grid& src);
	void InternalDelete(void);
	void InternalInit(void);
};
