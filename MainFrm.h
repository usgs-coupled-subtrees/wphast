// MainFrm.h : interface of the CMainFrame class
//

#include "PropertyTreeControlBar.h"
#include "BoxPropertiesDialogBar.h"

#pragma once
class CMainFrame : public CFrameWnd
{

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
#ifndef USE_ORIGINAL
	//{{SRC
	CPropertyTreeControlBar m_wndTreeControlBar;
	// CSizingDialogBarCFVS7   m_wndDialogBarBoxProperties;
	CBoxPropertiesDialogBar   m_wndDialogBarBoxProperties;
	//CReBar      m_wndReBar;
	//}}SRC
#endif


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewResetDefaultLayout();
	void UpdateXYZ(float x, float y, float z, const char* xy_units, const char* z_units);
};


