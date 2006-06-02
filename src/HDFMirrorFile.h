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
	virtual void Abort(void);
	virtual void Close(void);
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError = NULL);

	virtual ULONGLONG GetLength(void) const {return 2000;}

	hid_t GetHID(void)const {return m_hidFile;}

protected:
	CString m_strMirrorName;
	hid_t m_hidFile;
};
