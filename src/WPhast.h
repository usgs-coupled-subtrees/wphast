// WPhast.h : main header file for the WPhast application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "RecentFileList2.h"

// CWPhastApp:
// See WPhast.cpp for the implementation of this class
//

class CWPhastApp : public CWinApp
{
public:
	class CSettings
	{
	public:
		CSettings() : bRunParallel(FALSE) {};
		CString strDatabase;
		CString strCommand;
		CString strCommandArgs;
		BOOL bRunParallel;
		virtual ~CSettings(){};
	};
	CSettings settings;


public:
	CWPhastApp();
	virtual ~CWPhastApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
#if defined(WPHAST_AUTOMATION)
	COleTemplateServer m_server;
		// Server object for document creation
#endif
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileNew();
protected:
	BOOL m_bShellFileNew;
	CRecentFileList *pRecentDBFileList;
public:
	afx_msg void OnToolsRunOptions();
	BOOL LoadMoreProfileSettings(void);
	INT_PTR RunTypeDlgDoModal();
	virtual int ExitInstance();
};

extern CWPhastApp theApp;