#pragma once
#include "afx.h"

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>

class CHDFMirrorFile :
	public CFile
{
// Implementation
public:
	virtual void Abort();
	virtual void Close();
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError = NULL);

	virtual ULONGLONG GetLength() const {return 2000;}

	hid_t GetHID()const {return m_hidFile;}
// COMMENT: {6/17/2005 9:11:03 PM}	void write_axis(hid_t loc_id, double* a, int na, const char* name);

protected:
	CString m_strMirrorName;
	hid_t m_hidFile;
};
