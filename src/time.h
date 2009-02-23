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

class Ctime : public time
{
public:
	Ctime(void);
	~Ctime(void);

	// copy ctors
	Ctime(const struct time& t);
	Ctime(const Ctime& t);
	static void Copy(struct time& dest, const struct time& src);

	// copy assignment
	Ctime& operator=(const Ctime& rhs);
	Ctime& operator=(const struct time& rhs);

	bool operator<(const Ctime& rhs)const;

	int SetInput(const char* input);
	void SetValue(double dValue);

	void Serialize(bool bStoring, hid_t loc_id);
	void Serialize(CArchive& ar);
};
