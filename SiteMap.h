#pragma once

#include <string>
#include "WorldTransform.h"

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */


class CSiteMap
{
public:
	CSiteMap(void);
	~CSiteMap(void);

	// bool SetWorldFileName(const char *filename);
	CWorldTransform GetWorldTransform(void)const;
	void SetWorldTransform(const CWorldTransform &wtrans);

	void Serialize(bool bStoring, hid_t loc_id);

public:
	std::string       m_fileName;
	double            m_placement[3];    // Grid origin in map coordinates
	double            m_angle;
	//const std::string &WorldFileName;

protected:
	//std::string       m_worldFileName;
	CWorldTransform   m_worldTransform;
};
