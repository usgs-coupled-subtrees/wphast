// MainFrm.h : interface of the CMainFrame class
//

#include "PropertyTreeControlBar.h"
#include "BoxPropertiesDialogBar.h"

#if !defined(_USE_DEFAULT_MENUS_)
#include "BCMenu/BCMenu.h"
#endif

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

#if !defined(_USE_DEFAULT_MENUS_)
	HMENU NewMenu();
	BCMenu m_menu;
#endif

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
#if defined(_DEBUG)
	CBitmap m_CheckBitmap;
	CBitmap m_UnCheckBitmap;
#endif

#endif


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewResetDefaultLayout();
// COMMENT: {8/4/2009 5:36:02 PM}	void UpdateGrid(float x, float y, float z, const char* xy_units, const char* z_units);
// COMMENT: {8/4/2009 5:36:02 PM}	void UpdateMap(float x, float y, float z, const char* xy_units, const char* z_units);
	void UpdateGrid(double x, double y, double z, const char* xy_units, const char* z_units);
	void UpdateMap(double x, double y, double z, const char* xy_units, const char* z_units);
#if !defined(_USE_DEFAULT_MENUS_)
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
#endif
	afx_msg void OnUpdateViewProperiesview(CCmdUI *pCmdUI);
	afx_msg void OnViewProperiesview();
};


