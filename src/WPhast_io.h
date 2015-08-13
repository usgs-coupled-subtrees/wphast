#pragma once

#include <set>
#include "phrq_io.h"

class WPhast_io : public PHRQ_io
{
public:
	WPhast_io(std::set<CString> &files);
	virtual ~WPhast_io(void);

	// overrides
	virtual LINE_TYPE get_line(void);

protected:
	std::set<CString> &ufiles;
};
