// WPhast.h : main header file for the WPhast application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CWPhastApp:
// See WPhast.cpp for the implementation of this class
//

class CWPhastApp : public CWinApp
{
public:
	CWPhastApp();


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
};

extern CWPhastApp theApp;