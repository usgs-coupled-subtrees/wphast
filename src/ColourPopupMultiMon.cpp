#include "StdAfx.h"
#include "ColourPopupMultiMon.h"
#include "colorpicker/ColourPicker.h"


CColourPopupMultiMon::CColourPopupMultiMon(void)
: CColourPopup()
{
}

CColourPopupMultiMon::CColourPopupMultiMon(CPoint p, COLORREF crColour, CWnd* pParentWnd,
										   LPCTSTR szDefaultText /* = NULL */,
										   LPCTSTR szCustomText  /* = NULL */)
{
    m_crColour       = m_crInitialColour = crColour;
    m_pParent        = pParentWnd;
    m_strDefaultText = (szDefaultText)? szDefaultText : _T("");
    m_strCustomText  = (szCustomText)?  szCustomText  : _T("");

    CColourPopupMultiMon::Create(p, crColour, pParentWnd, szDefaultText, szCustomText);
}


CColourPopupMultiMon::~CColourPopupMultiMon(void)
{
}

BOOL CColourPopupMultiMon::Create(CPoint p, COLORREF crColour, CWnd* pParentWnd,
								  LPCTSTR szDefaultText /* = NULL */,
								  LPCTSTR szCustomText  /* = NULL */)
{
    ASSERT(pParentWnd && ::IsWindow(pParentWnd->GetSafeHwnd()));
    ASSERT(pParentWnd->IsKindOf(RUNTIME_CLASS(CColourPicker)));

    m_pParent  = pParentWnd;
    m_crColour = m_crInitialColour = crColour;

    // Get the class name and create the window
    CString szClassName = AfxRegisterWndClass(CS_CLASSDC|CS_SAVEBITS|CS_HREDRAW|CS_VREDRAW,
                                              0,
                                              (HBRUSH) (COLOR_BTNFACE+1), 
                                              0);

    if (!CWnd::CreateEx(0, szClassName, _T(""), WS_VISIBLE|WS_POPUP, 
                        p.x, p.y, 100, 100, // size updated soon
                        pParentWnd->GetSafeHwnd(), 0, NULL))
        return FALSE;

    // Store the Custom text
    if (szCustomText != NULL) 
        m_strCustomText = szCustomText;

    // Store the Default Area text
    if (szDefaultText != NULL) 
        m_strDefaultText = szDefaultText;
        
    // Set the window size
    SetWindowSize();

    // Create the tooltips
    CreateToolTips();

    // Find which cell (if any) corresponds to the initial colour
    FindCellFromColour(crColour);

    // Capture all mouse events for the life of this window
    SetCapture();

    return TRUE;
}

// Works out an appropriate size and position of this window
void CColourPopupMultiMon::SetWindowSize()
{
    CSize TextSize;

    // If we are showing a custom or default text area, get the font and text size.
    if (m_strCustomText.GetLength() || m_strDefaultText.GetLength())
    {
        CClientDC dc(this);
        CFont* pOldFont = (CFont*) dc.SelectObject(&m_Font);

        // Get the size of the custom text (if there IS custom text)
        TextSize = CSize(0,0);
        if (m_strCustomText.GetLength())
            TextSize = dc.GetTextExtent(m_strCustomText);

        // Get the size of the default text (if there IS default text)
        if (m_strDefaultText.GetLength())
        {
            CSize DefaultSize = dc.GetTextExtent(m_strDefaultText);
            if (DefaultSize.cx > TextSize.cx) TextSize.cx = DefaultSize.cx;
            if (DefaultSize.cy > TextSize.cy) TextSize.cy = DefaultSize.cy;
        }

        dc.SelectObject(pOldFont);
        TextSize += CSize(2*m_nMargin,2*m_nMargin);

        // Add even more space to draw the horizontal line
        TextSize.cy += 2*m_nMargin + 2;
    }

    // Get the number of columns and rows
    //m_nNumColumns = (int) sqrt((double)m_nNumColours);    // for a square window (yuk)
    m_nNumColumns = 8;
    m_nNumRows = m_nNumColours / m_nNumColumns;
    if (m_nNumColours % m_nNumColumns) m_nNumRows++;

    // Get the current window position, and set the new size
    CRect rect;
    GetWindowRect(rect);

    m_WindowRect.SetRect(rect.left, rect.top, 
                         rect.left + m_nNumColumns*m_nBoxSize + 2*m_nMargin,
                         rect.top  + m_nNumRows*m_nBoxSize + 2*m_nMargin);

    // if custom text, then expand window if necessary, and set text width as
    // window width
    if (m_strDefaultText.GetLength()) 
    {
        if (TextSize.cx > m_WindowRect.Width())
            m_WindowRect.right = m_WindowRect.left + TextSize.cx;
        TextSize.cx = m_WindowRect.Width()-2*m_nMargin;

        // Work out the text area
        m_DefaultTextRect.SetRect(m_nMargin, m_nMargin, 
                                  m_nMargin+TextSize.cx, 2*m_nMargin+TextSize.cy);
        m_WindowRect.bottom += m_DefaultTextRect.Height() + 2*m_nMargin;
    }

    // if custom text, then expand window if necessary, and set text width as
    // window width
    if (m_strCustomText.GetLength()) 
    {
        if (TextSize.cx > m_WindowRect.Width())
            m_WindowRect.right = m_WindowRect.left + TextSize.cx;
        TextSize.cx = m_WindowRect.Width()-2*m_nMargin;

        // Work out the text area
        m_CustomTextRect.SetRect(m_nMargin, m_WindowRect.Height(), 
                                 m_nMargin+TextSize.cx, 
                                 m_WindowRect.Height()+m_nMargin+TextSize.cy);
        m_WindowRect.bottom += m_CustomTextRect.Height() + 2*m_nMargin;
   }


#ifdef SKIP_FOR_MULTI_MONITOR
    // Need to check it'll fit on screen: Too far right?
    CSize ScreenSize(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
    if (m_WindowRect.right > ScreenSize.cx)
        m_WindowRect.OffsetRect(-(m_WindowRect.right - ScreenSize.cx), 0);

    // Too far left?
    if (m_WindowRect.left < 0)
        m_WindowRect.OffsetRect( -m_WindowRect.left, 0);

    // Bottom falling out of screen?
    if (m_WindowRect.bottom > ScreenSize.cy)
    {
        CRect ParentRect;
        m_pParent->GetWindowRect(ParentRect);
        m_WindowRect.OffsetRect(0, -(ParentRect.Height() + m_WindowRect.Height()));
    }
#endif

    // Set the window size and position
    MoveWindow(m_WindowRect, TRUE);
}
