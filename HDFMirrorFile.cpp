#include "StdAfx.h"
#include "HDFMirrorFile.h"
// #include <afximpl.h>
#include <Shlwapi.h> // PathStripToRoot

void SrcGetRoot(LPCTSTR lpszPath, CString& strRoot)
{
	ASSERT(lpszPath != NULL);

	LPTSTR lpszRoot = strRoot.GetBuffer(_MAX_PATH);
	memset(lpszRoot, 0, _MAX_PATH);
	lstrcpyn(lpszRoot, lpszPath, _MAX_PATH);
	::PathStripToRoot(lpszRoot);
	strRoot.ReleaseBuffer();
}

void SrcTimeToFileTime(const CTime& time, LPFILETIME pFileTime)
{
	SYSTEMTIME sysTime;
	sysTime.wYear = (WORD)time.GetYear();
	sysTime.wMonth = (WORD)time.GetMonth();
	sysTime.wDay = (WORD)time.GetDay();
	sysTime.wHour = (WORD)time.GetHour();
	sysTime.wMinute = (WORD)time.GetMinute();
	sysTime.wSecond = (WORD)time.GetSecond();
	sysTime.wMilliseconds = 0;

	// convert system time to local file time
	FILETIME localTime;
	if (!SystemTimeToFileTime((LPSYSTEMTIME)&sysTime, &localTime))
		CFileException::ThrowOsError((LONG)::GetLastError());

	// convert local file time to UTC file time
	if (!LocalFileTimeToFileTime(&localTime, pFileTime))
		CFileException::ThrowOsError((LONG)::GetLastError());
}

// Modified from CMirrorFile::Open 
// BUGBUG: This obliterates hard links on NTFS
BOOL CHDFMirrorFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags,
	CFileException* pError)
{
	ASSERT(lpszFileName != NULL);
	m_strMirrorName.Empty();

	CFileStatus status;
	if (nOpenFlags & CFile::modeCreate) //opened for writing
	{
		if (CFile::GetStatus(lpszFileName, status))
		{
			CString strRoot;
			SrcGetRoot(lpszFileName, strRoot);

			DWORD dwSecPerClus, dwBytesPerSec, dwFreeClus, dwTotalClus;
			int nBytes = 0;
			if (GetDiskFreeSpace(strRoot, &dwSecPerClus, &dwBytesPerSec, &dwFreeClus,
				&dwTotalClus))
			{
				nBytes = dwFreeClus*dwSecPerClus*dwBytesPerSec;
			}
			if (nBytes > 2*status.m_size) // at least 2x free space avail
			{
				// get the directory for the file
				TCHAR szPath[_MAX_PATH];
				LPTSTR lpszName;
				GetFullPathName(lpszFileName, _MAX_PATH, szPath, &lpszName);
				*lpszName = NULL;

				//let's create a temporary file name
				GetTempFileName(szPath, _T("WPH"), 0,
					m_strMirrorName.GetBuffer(_MAX_PATH+1));
				m_strMirrorName.ReleaseBuffer();
			}
		}
	}

	if (!m_strMirrorName.IsEmpty() &&
		(m_hidFile = ::H5Fcreate(m_strMirrorName, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)))
//		CFile::Open(m_strMirrorName, nOpenFlags, pError))
	{
		m_strFileName = lpszFileName;
		FILETIME ftCreate, ftAccess, ftModify;
		if (::GetFileTime((HANDLE)m_hFile, &ftCreate, &ftAccess, &ftModify))
		{
			SrcTimeToFileTime(status.m_ctime, &ftCreate);
			SetFileTime((HANDLE)m_hFile, &ftCreate, &ftAccess, &ftModify);
		}

		DWORD dwLength = 0;
		PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
		if (GetFileSecurity(lpszFileName, DACL_SECURITY_INFORMATION,
			NULL, dwLength, &dwLength))
		{
			pSecurityDescriptor = (PSECURITY_DESCRIPTOR) new BYTE[dwLength];
			if (::GetFileSecurity(lpszFileName, DACL_SECURITY_INFORMATION,
				pSecurityDescriptor, dwLength, &dwLength))
			{
				SetFileSecurity(m_strMirrorName, DACL_SECURITY_INFORMATION, pSecurityDescriptor);
			}
			delete[] (BYTE*)pSecurityDescriptor;
		}
		return TRUE;
	}
	m_strMirrorName.Empty();
	if (nOpenFlags & CFile::modeCreate) //opened for writing
	{
		m_hidFile = ::H5Fcreate(lpszFileName, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if (m_hidFile > 0) return TRUE;
		return FALSE;
	}
	if (nOpenFlags == (CFile::modeRead|CFile::shareDenyWrite))
	{
		//{{NEW 08/06/2003
		BOOL b = CFile::Open(lpszFileName, nOpenFlags, pError);
		if (!b) return b;
		CFile::Close();
		//}}NEW 08/06/2003
		m_hidFile = ::H5Fopen(lpszFileName, H5F_ACC_RDWR, H5P_DEFAULT);
		if (m_hidFile > 0) return TRUE;
		pError->m_cause = CFileException::generic;
		return FALSE;
	}
	ASSERT(FALSE); // TODO
	return FALSE;
	//return CFile::Open(lpszFileName, nOpenFlags, pError);
}

void CHDFMirrorFile::Abort()
{
	herr_t status;
	//CFile::Abort();
	if (this->m_hidFile > 0) {
		status = ::H5Fclose(this->m_hidFile);
		ASSERT(status >= 0);
		this->m_hidFile = -1;
	}
	if (!m_strMirrorName.IsEmpty())
		CFile::Remove(m_strMirrorName);
}

void CHDFMirrorFile::Close()
{
	herr_t status;
	CString strName = m_strFileName; //file close empties string
	// CFile::Close();
	ASSERT(this->m_hidFile > 0);
	if (this->m_hidFile > 0) {
		status = ::H5Fclose(this->m_hidFile);
		ASSERT(status >= 0);
		this->m_hidFile = -1;
	}
	if (!m_strMirrorName.IsEmpty())
	{
		BOOL (__stdcall *pfnReplaceFile)(LPCTSTR, LPCTSTR, LPCTSTR, DWORD, LPVOID, LPVOID);

		HMODULE hModule = GetModuleHandle(_T("KERNEL32"));
		ASSERT(hModule != NULL);

		pfnReplaceFile = (BOOL (__stdcall *)(LPCTSTR, LPCTSTR, LPCTSTR, DWORD, LPVOID, LPVOID))
#ifndef _UNICODE
			GetProcAddress(hModule, "ReplaceFileA");
#else
			GetProcAddress(hModule, "ReplaceFileW");
#endif

		if(!pfnReplaceFile || !pfnReplaceFile(strName, m_strMirrorName, NULL, 0, NULL, NULL))
		{
			CFile::Remove(strName);
			CFile::Rename(m_strMirrorName, strName);
		}
	}
}
