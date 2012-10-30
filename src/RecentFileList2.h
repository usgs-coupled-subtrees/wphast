#pragma once

#include <afxadv.h>

// CRecentFileList2

class CRecentFileList2 : public CRecentFileList
{
// COMMENT: {10/22/2012 10:24:29 PM}	DECLARE_DYNAMIC(CRecentFileList2)
// COMMENT: {10/22/2012 10:24:29 PM}
public:
	CRecentFileList2(UINT nStart, LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat, int nSize,
		int nMaxDispLen = AFX_ABBREV_FILENAME_LEN);
	virtual ~CRecentFileList2();

	virtual void Add(LPCTSTR lpszPathName);

// COMMENT: {10/23/2012 7:58:38 PM}protected:
// COMMENT: {10/23/2012 7:58:38 PM}	DECLARE_MESSAGE_MAP()
};


