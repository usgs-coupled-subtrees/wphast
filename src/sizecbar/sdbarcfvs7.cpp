#include "stdafx.h"
#include "sdbarcfvs7.h"

/////////////////////////////////////////////////////////////////////////
// CSizingDialogBarCFVS7

IMPLEMENT_DYNAMIC(CSizingDialogBarCFVS7, baseCSizingDialogBarCFVS7);

BEGIN_MESSAGE_MAP(CSizingDialogBarCFVS7, baseCSizingDialogBarCFVS7)
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

CSizingDialogBarCFVS7::CSizingDialogBarCFVS7(void)
{
}

CSizingDialogBarCFVS7::~CSizingDialogBarCFVS7(void)
{
}

// preffered creation method
BOOL CSizingDialogBarCFVS7::Create(UINT nIDTemplate,
							   LPCTSTR lpszWindowName,
                               CWnd* pParentWnd, UINT nID,
                               DWORD dwStyle)
{
    // must have a parent
    ASSERT_VALID(pParentWnd);
    // cannot be both fixed and dynamic
    // (CBRS_SIZE_DYNAMIC is used for resizng when floating)
    ASSERT (!((dwStyle & CBRS_SIZE_FIXED) &&
              (dwStyle & CBRS_SIZE_DYNAMIC)));

	// create a modeless dialog
	BOOL bSuccess = CreateDlg(MAKEINTRESOURCE(nIDTemplate), pParentWnd);

	if (!bSuccess)
		return FALSE;

	// dialog template MUST specify that the dialog
	//  is an invisible child window
	SetDlgCtrlID(nID);
	CRect rect;
	GetWindowRect(&rect);

	m_szHorz = rect.Size();
	m_szHorz.cx += 9;
	m_szHorz.cy += 9;

	m_szVert = rect.Size();
	m_szVert.cx += 9;
	m_szVert.cy += 9;

	m_szFloat = rect.Size();
	m_szFloat.cx += 9;
	m_szFloat.cy += 18;

	m_szMinHorz = rect.Size();
	m_szMinHorz.cx += 9;
	m_szMinHorz.cy += 9;

	m_szMinVert = rect.Size();
	m_szMinVert.cx += 9;
	m_szMinVert.cy += 9;

	m_szMinFloat = rect.Size();
	m_szMinFloat.cx += 9;
	m_szMinFloat.cy += 18;

	// force WS_CLIPSIBLINGS
	ModifyStyle(0, WS_CLIPSIBLINGS);

	if (!ExecuteDlgInit(MAKEINTRESOURCE(nIDTemplate)))
		return FALSE;


	// force the size to zero - resizing bar will occur later
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);

    m_dwStyle = dwStyle & CBRS_ALL; // save the control bar styles

	SetWindowText(lpszWindowName); // implicit paint

#pragma warning( disable : 4311 ) // 'type cast' : pointer truncation from 'HBRUSH' to 'LONG'
    // set the background color
#ifdef _WIN64
	::SetClassLongPtr(this->m_hWnd, GCLP_HBRBACKGROUND,
#else
	::SetClassLong(this->m_hWnd, GCL_HBRBACKGROUND,
#endif
		(LONG)::GetSysColorBrush(COLOR_BTNFACE));
#pragma warning( default : 4311 )
	return TRUE;
}

void CSizingDialogBarCFVS7::OnNcPaint()
{
    // get window DC that is clipped to the non-client area
    CWindowDC dc(this); // the HDC will be released by the destructor

    CRect rcClient, rcBar;
    GetClientRect(rcClient);
    ClientToScreen(rcClient);
    GetWindowRect(rcBar);
    rcClient.OffsetRect(-rcBar.TopLeft());
    rcBar.OffsetRect(-rcBar.TopLeft());

    CDC mdc;
    mdc.CreateCompatibleDC(&dc);
    
    CBitmap bm;
    bm.CreateCompatibleBitmap(&dc, rcBar.Width(), rcBar.Height());
    CBitmap* pOldBm = mdc.SelectObject(&bm);

#ifdef USE_ORIGINAL
    // draw borders in non-client area
    CRect rcDraw = rcBar;
    DrawBorders(&mdc, rcDraw);
#else
    // draw borders in non-client area
    CRect rcDraw = rcBar;
    CRect rcDelayDraw = rcBar;
#endif

    // erase the NC background
#pragma warning( disable : 4312 ) // 'type cast' : conversion from 'DWORD' to 'HBRUSH' of greater size
    mdc.FillRect(rcDraw, CBrush::FromHandle(
#ifdef _WIN64
        (HBRUSH) GetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND)));
#else
        (HBRUSH) GetClassLong(m_hWnd, GCL_HBRBACKGROUND)));
#endif
#pragma warning( default : 4312 )

    if (m_dwSCBStyle & SCBS_SHOWEDGES)
    {
        CRect rcEdge; // paint the sizing edges
        for (int i = 0; i < 4; i++)
            if (GetEdgeRect(rcBar, GetEdgeHTCode(i), rcEdge))
                mdc.Draw3dRect(rcEdge, ::GetSysColor(COLOR_BTNHIGHLIGHT),
                    ::GetSysColor(COLOR_BTNSHADOW));
    }

    NcPaintGripper(&mdc, rcClient);

    // client area is not our bussiness :)
    dc.IntersectClipRect(rcBar);
    dc.ExcludeClipRect(rcClient);

#ifndef USE_ORIGINAL
    DrawBorders(&mdc, rcDelayDraw);
#endif

    dc.BitBlt(0, 0, rcBar.Width(), rcBar.Height(), &mdc, 0, 0, SRCCOPY);

    mdc.SelectObject(pOldBm);
    bm.DeleteObject();
    mdc.DeleteDC();
}

#ifndef _AFX_NO_OCC_SUPPORT
BOOL CSizingDialogBarCFVS7::SetOccDialogInfo(_AFX_OCC_DIALOG_INFO* pOccDialogInfo)
{
	// This is necessary if AfxEnableControlContainer() is used
	//
	m_pOccDialogInfo = pOccDialogInfo;
	return TRUE;
}
#endif

//void CSizingDialogBarCFVS7::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
//{
//	UpdateDialogControls(pTarget, bDisableIfNoHndler);
//}
//
//BOOL CSizingDialogBarCFVS7::PreTranslateMessage(MSG* pMsg)
//{
//	// Add your specialized code here and/or call the base class
//	if (pMsg->message == WM_KEYDOWN) {
//		TRACE("WM_KEYDOWN\n");
//	}
//
//	return CSizingControlBarCFVS7::PreTranslateMessage(pMsg);
//}
