#pragma once

#include <string>
#include "WorldTransform.h"

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */


class CSiteMap2
{
public:
	CSiteMap2(void);
	~CSiteMap2(void);

	CWorldTransform GetWorldTransform(void)const;
	void SetWorldTransform(const CWorldTransform &wtrans);
	void Serialize(bool bStoring, hid_t loc_id);

public:
	std::string       FileName;
	double            Origin[3];    // Grid origin in map coordinates
	double            Angle;

protected:
	CWorldTransform   WorldTransform;
};
