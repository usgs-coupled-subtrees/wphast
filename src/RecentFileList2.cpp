// RecentFileList2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "RecentFileList2.h"

#include <atlpath.h> // ATL::ATLPath::FileExists
#include "srcimpl.h"

#ifndef __AFXWIN_H__
	#include <afxwin.h>
#endif


// CRecentFileList2

// COMMENT: {10/22/2012 10:26:03 PM}IMPLEMENT_DYNAMIC(CRecentFileList2, CWnd)

CRecentFileList2::CRecentFileList2(UINT nStart, LPCTSTR lpszSection,
	LPCTSTR lpszEntryFormat, int nSize, int nMaxDispLen)
	: CRecentFileList(nStart, lpszSection, lpszEntryFormat, nSize, nMaxDispLen)
{

}

CRecentFileList2::~CRecentFileList2()
{
}


// COMMENT: {10/22/2012 10:26:37 PM}BEGIN_MESSAGE_MAP(CRecentFileList2, CWnd)
// COMMENT: {10/22/2012 10:26:37 PM}END_MESSAGE_MAP()



// CRecentFileList2 message handlers


// Operations
void CRecentFileList2::Add(LPCTSTR lpszPathName)
{
	ASSERT(m_arrNames != NULL);
	ASSERT(AfxIsValidString(lpszPathName));

	// fully qualify the path name
	TCHAR szTemp[_MAX_PATH];
	TCHAR szExpanded[2*_MAX_PATH];
	if ( lpszPathName == NULL || lstrlen(lpszPathName) >= _MAX_PATH )
	{
		ASSERT(FALSE);
		// MFC requires paths with length < _MAX_PATH
		// No other way to handle the error from a void function
		AfxThrowFileException(CFileException::badPath);
	}

	VERIFY(::ExpandEnvironmentStrings(lpszPathName, szExpanded, 2*_MAX_PATH));
// COMMENT: {10/23/2012 9:09:04 PM}	LPTSTR lpszFilePart;
// COMMENT: {10/23/2012 9:09:04 PM}	if (::GetFullPathName(szTemp, _MAX_PATH, szExpanded, &lpszFilePart) == 0)
// COMMENT: {10/23/2012 9:09:04 PM}	{
// COMMENT: {10/23/2012 9:09:04 PM}		ASSERT(FALSE);
// COMMENT: {10/23/2012 9:09:04 PM}		// MFC requires paths with length < _MAX_PATH
// COMMENT: {10/23/2012 9:09:04 PM}		// No other way to handle the error from a void function
// COMMENT: {10/23/2012 9:09:04 PM}		AfxThrowFileException(CFileException::badPath);
// COMMENT: {10/23/2012 9:09:04 PM}	}
	if( SrcFullPath(szTemp, szExpanded) == FALSE )
	{
		ASSERT(FALSE);
		// MFC requires paths with length < _MAX_PATH
		// No other way to handle the error from a void function
		AfxThrowFileException(CFileException::badPath);
	}


// COMMENT: {10/22/2012 10:49:15 PM}// COMMENT: {10/22/2012 10:36:30 PM}	if( AfxFullPath(szTemp, lpszPathName) == FALSE )
// COMMENT: {10/22/2012 10:49:15 PM}	if( AfxFullPath(szTemp, szExpanded) == FALSE )
// COMMENT: {10/22/2012 10:49:15 PM}	{
// COMMENT: {10/22/2012 10:49:15 PM}		ASSERT(FALSE);
// COMMENT: {10/22/2012 10:49:15 PM}		// MFC requires paths with length < _MAX_PATH
// COMMENT: {10/22/2012 10:49:15 PM}		// No other way to handle the error from a void function
// COMMENT: {10/22/2012 10:49:15 PM}		AfxThrowFileException(CFileException::badPath);
// COMMENT: {10/22/2012 10:49:15 PM}	}
	
	// update the MRU list, if an existing MRU string matches file name
	int iMRU;
	for (iMRU = 0; iMRU < m_nSize-1; iMRU++)
	{
		if (SrcComparePath(m_arrNames[iMRU], lpszPathName))
			break;      // iMRU will point to matching entry
	}
	// move MRU strings before this one down
	for (; iMRU > 0; iMRU--)
	{
		ASSERT(iMRU > 0);
		ASSERT(iMRU < m_nSize);
		m_arrNames[iMRU] = m_arrNames[iMRU-1];
	}
	// place this one at the beginning
	m_arrNames[0] = lpszPathName;
}
