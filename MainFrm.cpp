// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "WPhast.h"

#include "MainFrm.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_RESETDEFAULTLAYOUT, OnViewResetDefaultLayout)
	// ON_UPDATE_COMMAND_UI(ID_INDICATOR_XYZ, OnUpdateXYZ)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_XYZ,
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

HBITMAP IconToBitmap(UINT uIcon, COLORREF transparentColor)
{
	HICON hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(uIcon), IMAGE_ICON, 10, 10, LR_DEFAULTCOLOR);
	if (!hIcon)
		return NULL;

	RECT     rect;

	rect.right = ::GetSystemMetrics(SM_CXMENUCHECK);
	rect.bottom = ::GetSystemMetrics(SM_CYMENUCHECK);

	rect.left =
		rect.top  = 0;

	HWND desktop    = ::GetDesktopWindow();
	if (desktop == NULL)
		return NULL;

	HDC  screen_dev = ::GetDC(desktop);
	if (screen_dev == NULL)
		return NULL;

	// Create a compatible DC
	HDC dst_hdc = ::CreateCompatibleDC(screen_dev);
	if (dst_hdc == NULL)
	{
		::ReleaseDC(desktop, screen_dev); 
		return NULL;
	}

	// Create a new bitmap of icon size
	HBITMAP bmp = ::CreateCompatibleBitmap(screen_dev, rect.right, rect.bottom);
	if (bmp == NULL)
	{
		::DeleteDC(dst_hdc);
		::ReleaseDC(desktop, screen_dev); 
		return NULL;
	}

	// Select it into the compatible DC
	HBITMAP old_dst_bmp = (HBITMAP)::SelectObject(dst_hdc, bmp);
	if (old_dst_bmp == NULL)
		return NULL;

	// Fill the background of the compatible DC with the given colour
	HBRUSH brush = ::CreateSolidBrush(transparentColor);
	if (brush == NULL)
	{
		::DeleteDC(dst_hdc);
		::ReleaseDC(desktop, screen_dev); 
		return NULL;
	}
	::FillRect(dst_hdc, &rect, brush);
	::DeleteObject(brush);

	// Draw the icon into the compatible DC
	::DrawIconEx(dst_hdc, 0, 0, hIcon, rect.right, rect.bottom, 0, NULL, DI_NORMAL);

	// Restore settings
	::SelectObject(dst_hdc, old_dst_bmp);
	::DeleteDC(dst_hdc);
	::ReleaseDC(desktop, screen_dev); 
	::DestroyIcon(hIcon);
	return bmp;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

#ifdef _SCB_REPLACE_MINIFRAME
    m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
#endif //_SCB_REPLACE_MINIFRAME

	CCreateContext *pContext = static_cast<CCreateContext*>(lpCreateStruct->lpCreateParams);
	ASSERT(pContext);
	CWPhastDoc *pDoc = static_cast<CWPhastDoc*>(pContext->m_pCurrentDoc);
	ASSERT_VALID(pDoc);

	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TreeCtrl Bar
	//
	if (!m_wndTreeControlBar.Create(_T("Properties"), this, IDW_CONTROLBAR_TREE))
	{
		TRACE0("Failed to create instant bar\n");
		return -1;		// fail to create
	}
	m_wndTreeControlBar.SetSCBStyle(m_wndTreeControlBar.GetSCBStyle() |
		SCBS_SIZECHILD);
	m_wndTreeControlBar.SetBarStyle(m_wndTreeControlBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	pDoc->Attach(&this->m_wndTreeControlBar);

	// Box properties Bar
	//
	if (!m_wndDialogBarBoxProperties.Create(IDD_PROPS_CUBE2, _T("Zone Properties"), this, IDW_CONTROLBAR_BOXPROPS))
	{
		TRACE0("Failed to create m_wndDialogBar\n");
		return -1;
	}
	m_wndDialogBarBoxProperties.SetBarStyle(m_wndDialogBarBoxProperties.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndDialogBarBoxProperties.EnableDocking(CBRS_ALIGN_ANY);
	pDoc->Attach(&this->m_wndDialogBarBoxProperties);


	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

#ifndef USE_ORIGINAL
// COMMENT: {4/20/2005 6:25:54 PM}	////{{{{
// COMMENT: {4/20/2005 6:25:54 PM}	if (!m_wndReBar.Create(this) || !m_wndReBar.AddBar(&m_wndToolBar))
// COMMENT: {4/20/2005 6:25:54 PM}	{
// COMMENT: {4/20/2005 6:25:54 PM}		TRACE0("Failed to create rebar\n");
// COMMENT: {4/20/2005 6:25:54 PM}		return -1;      // fail to create
// COMMENT: {4/20/2005 6:25:54 PM}	}
// COMMENT: {4/20/2005 6:25:54 PM}	m_wndReBar.SetBarStyle(m_wndReBar.GetBarStyle() & ~CBRS_HIDE_INPLACE);
// COMMENT: {4/20/2005 6:25:54 PM}
// COMMENT: {4/20/2005 6:25:54 PM}	// position m_wndToolBarRun next to m_wndToolBar
// COMMENT: {4/20/2005 6:25:54 PM}	m_wndReBar.GetReBarCtrl().MaximizeBand(1);
// COMMENT: {4/20/2005 6:25:54 PM}	////}}}}

	// Layout ControlBars
	//
	OnViewResetDefaultLayout();

	// Restore last ControlBar state from registry
	CString sProfile = _T("BarState");
	// if (VerifyBarState(sProfile))
	{
		CSizingControlBar::GlobalLoadState(this, sProfile);
		LoadBarState(sProfile);
	}
	m_wndTreeControlBar.SetFocus();
#endif

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnViewResetDefaultLayout()
{
#ifdef _SCB_REPLACE_MINIFRAME
    m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
#endif //_SCB_REPLACE_MINIFRAME

	// show all ControlBars
	ShowControlBar(&m_wndToolBar, TRUE, FALSE);	
	ShowControlBar(&m_wndStatusBar, TRUE, FALSE);	
	ShowControlBar(&m_wndTreeControlBar, TRUE, FALSE);	
	ShowControlBar(&m_wndDialogBarBoxProperties, TRUE, FALSE);

	// Dock box props bar below TreeCtrl bar
	//
	m_wndTreeControlBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndTreeControlBar, AFX_IDW_DOCKBAR_LEFT);
	RecalcLayout();
	CRect rcTreeControlBar;
	m_wndTreeControlBar.GetWindowRect(rcTreeControlBar);
	rcTreeControlBar.OffsetRect(0, 1);	
	m_wndDialogBarBoxProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndDialogBarBoxProperties, AFX_IDW_DOCKBAR_LEFT, rcTreeControlBar);
}

BOOL CMainFrame::DestroyWindow()
{
	CString sProfile = _T("BarState");
	CSizingControlBar::GlobalSaveState(this, sProfile);
	SaveBarState(sProfile);

	return CFrameWnd::DestroyWindow();
}

void CMainFrame::UpdateXYZ(float x, float y, float z, const char* xy_units, const char* z_units)
{
	TCHAR buffer[80];
	::_sntprintf(buffer, 80, _T("%6.2f %s, %6.2f %s, %6.2f %s"), x, xy_units, y, xy_units, z, z_units);
	m_wndStatusBar.SetPaneText(1, buffer);
}
