#pragma once

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
#include "enum_fix.h"

#include "PropStruct.h"

#include <iosfwd> // std::ostream

class CGridElt : public grid_elt
{
public:
	// ctor
	CGridElt(void);
	// dtor
	~CGridElt(void);
	// copy ctor
	CGridElt(const grid_elt& src);
	CGridElt(const CGridElt& src);
	// copy assignment
	CGridElt& operator=(const CGridElt& rhs); 

	bool operator==(const grid_elt& rhs)const;

	static CGridElt NewDefaults(bool bFlowOnly = false);
	static CGridElt Full(void);

	static CLIPFORMAT clipFormat;

#ifdef _DEBUG
	void Dump(CDumpContext& dc)const;
#endif
	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);
	void Fill(std::vector<CPropStruct>& v)const;

	bool RemovePropZones(void);

	friend std::ostream& operator<< (std::ostream &os, const CGridElt &a);

private:
	void InternalCopy(const grid_elt& src);
	void InternalDelete(void);
	void InternalInit(void);
};
