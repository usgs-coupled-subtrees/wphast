#include "stdafx.h"
#include "ModGridCtrl.h"
#include <float.h>  // DBL_DIG


void AFXAPI DDX_TextGridControl(CDataExchange* pDX, int nIDC, int nRow, int nCol, double& value)
{
	pDX->PrepareCtrl(nIDC);
	CModGridCtrl* pGrid = static_cast<CModGridCtrl*>(pDX->m_pDlgWnd->GetDlgItem(nIDC));
	ASSERT_KINDOF(CModGridCtrl, pGrid);
	CString str;
	if (pDX->m_bSaveAndValidate)
	{
		str = pGrid->GetItemText(nRow, nCol);
		double d;
		if (_stscanf(str, _T("%lf"), &d) != 1)
		{
			pGrid->SetCurrentFocusCell(nRow, nCol);
			AfxMessageBox(AFX_IDP_PARSE_REAL);
			pDX->Fail();            // throws exception
		}
		value = d;
	}
	else
	{
		TCHAR szBuffer[400];
		_stprintf(szBuffer, _T("%.*g"), DBL_DIG, value);
		VERIFY(pGrid->SetItemText(nRow, nCol, szBuffer));
	}
}

void AFXAPI DDX_TextGridControl(CDataExchange* pDX, int nIDC, int nRow, int nCol, CString& value)
{
	pDX->PrepareCtrl(nIDC);
	CModGridCtrl* pGrid = static_cast<CModGridCtrl*>(pDX->m_pDlgWnd->GetDlgItem(nIDC));
	ASSERT_KINDOF(CModGridCtrl, pGrid);
	CString str;
	if (pDX->m_bSaveAndValidate)
	{
		value = pGrid->GetItemText(nRow, nCol);
	}
	else
	{
		VERIFY(pGrid->SetItemText(nRow, nCol, value));
	}
}

void AFXAPI DDX_TextGridControl(CDataExchange* pDX, int nIDC, int nRow, int nCol, int& value)
{
	pDX->PrepareCtrl(nIDC);
	CModGridCtrl* pGrid = static_cast<CModGridCtrl*>(pDX->m_pDlgWnd->GetDlgItem(nIDC));
	ASSERT_KINDOF(CModGridCtrl, pGrid);
	CString str;
	if (pDX->m_bSaveAndValidate)
	{
		str = pGrid->GetItemText(nRow, nCol);
		int n;
		if (_stscanf(str, _T("%d"), &n) != 1)
		{
			pGrid->SetCurrentFocusCell(nRow, nCol);
			AfxMessageBox(AFX_IDP_PARSE_INT);
			pDX->Fail();            // throws exception
		}
		value = n;
	}
	else
	{
		TCHAR szBuffer[400];
		_stprintf(szBuffer, _T("%d"), value);
		VERIFY(pGrid->SetItemText(nRow, nCol, szBuffer));
	}
}



CModGridCtrl::CModGridCtrl(int nRows, int nCols, int nFixedRows, int nFixedCols)
: CGridCtrl(nRows, nCols, nFixedRows, nFixedCols)
{
}

CModGridCtrl::~CModGridCtrl(void)
{
}

BEGIN_MESSAGE_MAP(CModGridCtrl, CGridCtrl)
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
    ON_NOTIFY(GVN_ENDLABELEDIT, IDC_INPLACE_CONTROL, OnEndInPlaceEdit)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

// MODIFICATIONS:
// Changed col resize cursor
// Changed row resize cursor
BOOL CModGridCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
    if (nHitTest == HTCLIENT)
    {
        switch (m_MouseMode) 
        {
			case MOUSE_OVER_COL_DIVIDE:
				::SetCursor(AfxGetApp()->LoadCursor(IDC_RESIZE_COLUMN));
				return TRUE;
				break;
            case MOUSE_OVER_ROW_DIVIDE:
				::SetCursor(AfxGetApp()->LoadCursor(IDC_RESIZE_ROW));
				return TRUE;
				break;
        }
    }
	return CGridCtrl::OnSetCursor(pWnd, nHitTest, message);
}

// MODIFICATIONS:
// Changed col resize cursor
// Changed row resize cursor
void CModGridCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
    CRect rect;
    GetClientRect(rect);

#ifndef GRIDCONTROL_NO_DRAGDROP
    // If outside client area, return (unless we are drag n dropping)
    if (m_MouseMode != MOUSE_DRAGGING && !rect.PtInRect(point))
        return;
#endif

    // If the left mouse button is up, then test to see if row/column sizing is imminent
    if (!(nFlags & MK_LBUTTON) 
        || (m_MouseMode == MOUSE_NOTHING && (nFlags & MK_LBUTTON)))
    {
        if (m_bAllowColumnResize && MouseOverColumnResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
            {
				::SetCursor(AfxGetApp()->LoadCursor(IDC_RESIZE_COLUMN));
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
        }
        else if (m_bAllowRowResize && MouseOverRowResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
            {
				::SetCursor(AfxGetApp()->LoadCursor(IDC_RESIZE_ROW));
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
        }
        else if (m_MouseMode != MOUSE_NOTHING)
        {
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
            m_MouseMode = MOUSE_NOTHING;
        }

#ifndef GRIDCONTROL_NO_TITLETIPS
        if (m_MouseMode == MOUSE_NOTHING && m_bTitleTips)
        {
            CCellID idCurrentCell = GetCellFromPt(point);
            CRect TextRect, CellRect;
            if (GetTextRect(idCurrentCell.row, idCurrentCell.col, TextRect) &&
                GetCellRect(idCurrentCell.row, idCurrentCell.col, CellRect))
                m_TitleTip.Show(TextRect, 
                                GetItemText(idCurrentCell.row, idCurrentCell.col), 
                                0,
                                CellRect,
                                GetItemFont(idCurrentCell.row, idCurrentCell.col));
        }
#endif

        m_LastMousePoint = point;
        return;
    }
	CGridCtrl::OnMouseMove(nFlags, point);
}

// MODIFICATIONS:
// no text shift when cell is selected
BOOL CModGridCtrl::DrawCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk)
{
    if (!m_bAllowDraw) 
        return FALSE;

    GV_ITEM Item;
    Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_STATE | GVIF_IMAGE | GVIF_BKCLR | GVIF_FGCLR;
    Item.row = nRow;
    Item.col = nCol;
    if (!GetItem(&Item))
        return FALSE;

    COLORREF TextBkClr = (Item.crBkClr == CLR_DEFAULT)? GetTextBkColor() : Item.crBkClr;
    COLORREF TextClr   = (Item.crFgClr == CLR_DEFAULT)? GetTextColor()   : Item.crFgClr;

	//{{
	if (Item.state & GVIS_DISABLED)
	{
		TextBkClr = this->m_crFixedBkColour;
		TextClr   = ::GetSysColor(COLOR_GRAYTEXT);
	}
	//}}

    // Force redraw of background if custom colour
    //if (Item.crBkClr != CLR_DEFAULT)
        bEraseBk = TRUE;

    int nSavedDC = pDC->SaveDC();

    pDC->SetBkMode(TRANSPARENT);

    if (Item.state & GVIS_FOCUSED) 
    {
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        if (bEraseBk) 
        {
            CBrush brush(TextBkClr);
            pDC->FillRect(rect, &brush);
        }
        rect.right--; rect.bottom--;    
        pDC->SelectStockObject(BLACK_PEN);
        pDC->SelectStockObject(NULL_BRUSH);
        pDC->Rectangle(rect);
        pDC->SetTextColor(TextClr);
		/// TRACE("GVIS_FOCUSED\n");

		//{{MODIFIED
        // rect.DeflateRect(1,1);
		//}}MODIFIED

    }
    else if (Item.state & GVIS_SELECTED) 
    {
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_HIGHLIGHT));
        rect.right--; rect.bottom--;
        pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    }
	else
	{
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        if (bEraseBk) 
        {
            CBrush brush(TextBkClr);
            pDC->FillRect(rect, &brush);
        }
        rect.right--; rect.bottom--;
        pDC->SetTextColor(TextClr);
    }

    if (Item.state & GVIS_DROPHILITED 
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
        && !pDC->IsPrinting()
#endif
        )
    {
        pDC->SelectStockObject(BLACK_PEN);
        pDC->SelectStockObject(NULL_BRUSH);
        pDC->Rectangle(rect);
    }

    // Create the appropriate font and select into DC
    LOGFONT *pLF = GetItemFont(nRow, nCol);

    //CCellID FocusCell = GetFocusCell();
    //if (IsCellFixed(nRow, nCol) && (FocusCell.row == nRow || FocusCell.col == nCol))
    //{
    //    static LOGFONT lf;
    //    memcpy(&lf, pLF, sizeof(LOGFONT));
    //    lf.lfWeight = SELECTED_CELL_FONT_WEIGHT;
    //    pLF = &lf;
    //}
   
    CFont Font;
    Font.CreateFontIndirect(pLF);
    pDC->SelectObject(&Font);

    rect.DeflateRect(m_nMargin, 0);

    if (m_pImageList && Item.iImage >= 0)
    {
        IMAGEINFO Info;
        if (m_pImageList->GetImageInfo(Item.iImage, &Info))
        {
            //CRgn rgn;
            //rgn.CreateRectRgnIndirect(rect);
            //pDC->SelectClipRgn(&rgn);
            //rgn.DeleteObject();

            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            m_pImageList->Draw(pDC, Item.iImage, rect.TopLeft(), ILD_NORMAL);
            rect.left += nImageWidth+m_nMargin;
        }
    }

    DrawText(pDC->m_hDC, Item.szText, -1, rect, Item.nFormat);

    pDC->RestoreDC(nSavedDC);
    Font.DeleteObject();

    return TRUE;
}

// MODIFICATIONS:
// all fixed cells are drawn the same regardless of selection
BOOL CModGridCtrl::DrawFixedCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk)
{
	// save current cell
    CCellID FocusCell(m_idCurrentCell);

	m_idCurrentCell.col = -1;
	m_idCurrentCell.row = -1;

	// do default
	BOOL bVal = CGridCtrl::DrawFixedCell(pDC, nRow, nCol, rect, bEraseBk);

	// restore current cell
	m_idCurrentCell = FocusCell;
	return bVal;
}

// MODIFICATIONS:
// Added Ctrl+Insert  => Ctrl+C [=] ID_EDIT_COPY
// Added Shift+Insert => Ctrl+V [=] ID_EDIT_PASTE
// Added Shift+Delete => Ctrl+X [=] ID_EDIT_CUT
void CModGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!IsValid(m_idCurrentCell)) 
	{
		CGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	// Add your message handler code here and/or call default
    if (IsCTRLpressed())
    {
        switch (nChar)
        {
#ifndef GRIDCONTROL_NO_CLIPBOARD
           case VK_INSERT: OnEditCopy();       break;
#endif
        }
    }
    if (IsSHIFTpressed())
    {
        switch (nChar)
        {
#ifndef GRIDCONTROL_NO_CLIPBOARD
           case VK_DELETE: OnEditCut();        break;
           case VK_INSERT: OnEditPaste();       break;
#endif
        }
    }
	else
	{
		switch (nChar)
		{
		case VK_DELETE:
			CutSelectedText();
			CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
			return;
			break;
		}
	}

	if (nChar == VK_RETURN)
	{
		CCellID next = m_idCurrentCell;
		BOOL bChangeLine = FALSE;
		if (IsSHIFTpressed())
		{
			if (next.row > m_nFixedRows) --next.row; 
		}
		else
		{
			if (next.row < (GetRowCount() - 1)) ++next.row; 
		}  
		if (next != m_idCurrentCell) 
		{
			if (m_MouseMode == MOUSE_NOTHING)
			{
				m_PrevSelectedCellMap.RemoveAll();
				m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
				m_SelectionStartCell = next;
				OnSelecting(next);
				m_MouseMode = MOUSE_NOTHING;
			}
			SetFocusCell(next);
			if (!IsCellVisible(next))
			{   
				EnsureVisible(next); // Make sure cell is visible
				if (IsSHIFTpressed())
				{
					SendMessage(WM_VSCROLL, SB_LINEUP, 0);    
				}
				else
				{
					SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);  
				}
				Invalidate();
			}
		}
	}

	CGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

// MODIFICATIONS:
void CModGridCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: Add your message handler code here
	if (point.x == -1 || point.y == -1) {
		// keyboard invoked => Shift+F10 or ContextMenuKey
#ifdef _DEBUG
		CString str;
		str.Format(_T("OnContextMenu point.x = %d, point.y = %d"), point.x, point.y);
		::AfxMessageBox(str);
#endif
	}
}

void CModGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	/**
	// Note: this works but ::mouse_event is more backwards
	// compatible
	INPUT click[2];
	click[0].type           = INPUT_MOUSE;
	click[0].mi.dx          = point.x;
	click[0].mi.dy          = point.y; 
	click[0].mi.mouseData   = 0;
	click[0].mi.dwFlags     = MOUSEEVENTF_LEFTDOWN;
	click[0].mi.time        = 0;
	click[0].mi.dwExtraInfo = 0;

	click[1].type           = INPUT_MOUSE;
	click[1].mi.dx          = point.x;
	click[1].mi.dy          = point.y; 
	click[1].mi.mouseData   = 0;
	click[1].mi.dwFlags     = MOUSEEVENTF_LEFTUP;
	click[1].mi.time        = 0;
	click[1].mi.dwExtraInfo = 0;

	::SendInput(2, click, sizeof(INPUT));
	**/

	// Need to avoid setting 
	// m_MouseMode = MOUSE_PREPARE_EDIT
	////CPoint ptClient(point);
	////this->ScreenToClient(&ptClient);
	CCellID cell = this->GetCellFromPt(point);

	if (cell != m_idCurrentCell) {
		// simulate LeftMouseClick
		::mouse_event(MOUSEEVENTF_LEFTDOWN, point.x, point.y, 0, 0);
		::mouse_event(MOUSEEVENTF_LEFTUP, point.x, point.y, 0, 0);
		// this doesn't work
		// this->OnLButtonDown(0, ptClient);
		// this->OnLButtonUp(0, ptClient);
	}

	CGridCtrl::OnRButtonDown(nFlags, point);
}

void CModGridCtrl::SetCurrentFocusCell(int nRow, int nCol)
{
	CGridCtrl::ResetSelectedRange();
	CGridCtrl::SetFocusCell(nRow, nCol);
	CGridCtrl::EnsureVisible(nRow, nCol);
}

// Modifications: OnEditCell no longer called if lastkey
// is an arrow key
void CModGridCtrl::OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{    
    GV_DISPINFO *pgvDispInfo = (GV_DISPINFO *)pNMHDR;
    GV_ITEM     *pgvItem = &pgvDispInfo->item;

    // In case OnEndInPlaceEdit called as window is being destroyed
    if (!IsWindow(GetSafeHwnd()))
        return;

    OnEndEditCell(pgvItem->row, pgvItem->col, pgvItem->szText);
    InvalidateCellRect(CCellID(pgvItem->row, pgvItem->col));

    SendMessageToParent(pgvItem->row, pgvItem->col, GVN_ENDLABELEDIT);

    switch (pgvItem->lParam) 
    {
		//{{
		case VK_RETURN:
		//}}
        case VK_TAB:
        case VK_DOWN: 
        case VK_UP:   
        case VK_RIGHT:
        case VK_LEFT:  
        case VK_NEXT:  
        case VK_PRIOR: 
        case VK_HOME:  
        case VK_END:    
            OnKeyDown(pgvItem->lParam, 0, 0);
			TRACE("Skipping OnEditCell call\n");
    }
    *pResult = 0;
}

void CModGridCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN)
	{
		TRACE("CModGridCtrl::OnChar - Enter Key Pressed\n");
// COMMENT: {3/21/2005 7:32:25 PM}		if (IsSHIFTpressed())
// COMMENT: {3/21/2005 7:32:25 PM}		{
// COMMENT: {3/21/2005 7:32:25 PM}			CGridCtrl::OnChar(VK_UP, nRepCnt, nFlags);
// COMMENT: {3/21/2005 7:32:25 PM}		}
// COMMENT: {3/21/2005 7:32:25 PM}		else
// COMMENT: {3/21/2005 7:32:25 PM}		{
// COMMENT: {3/21/2005 7:32:25 PM}			CGridCtrl::OnChar(VK_DOWN, nRepCnt, nFlags);
// COMMENT: {3/21/2005 7:32:25 PM}		}
		return;
	}
    CGridCtrl::OnChar(nChar, nRepCnt, nFlags);
}

UINT CModGridCtrl::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default
	TRACE("CModGridCtrl::OnGetDlgCode()\n");
	return DLGC_WANTALLKEYS;
	// return CGridCtrl::OnGetDlgCode();
}

void CModGridCtrl::CutSelectedText()
{
    if (!IsEditable())
        return;

    // Clear contents of selected cells.
    for (POSITION pos = m_SelectedCellMap.GetStartPosition(); pos != NULL; )
    {
        DWORD key;
        CCellID cell;
        m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);

        if (!IsCellEditable(cell))
            continue;

        CGridCell* pCell = GetCell(cell.row, cell.col);
        if (pCell)
        {
		    SendMessageToParent(cell.row, cell.col, GVN_BEGINLABELEDIT);
// COMMENT: {3/21/2005 8:34:57 PM}            EmptyCell(pCell, cell.row, cell.col);
			SetItemText(cell.row, cell.col, _T(""));
            SetModified(TRUE, cell.row, cell.col);
		    SendMessageToParent(cell.row, cell.col, GVN_ENDLABELEDIT);
			RedrawCell(cell);
        }
    }
}

BOOL CModGridCtrl::DisableCell(const CCellID& cell)
{
	return this->DisableCell(cell.row, cell.col);
}

BOOL CModGridCtrl::DisableCell(int nRow, int nCol)
{
    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
	{
        return FALSE;
	}

	pCell->state |= GVIS_READONLY;
	pCell->state |= GVIS_DISABLED;	

    return TRUE;
}

BOOL CModGridCtrl::EnableCell(const CCellID& cell)
{
	return this->EnableCell(cell.row, cell.col);
}

BOOL CModGridCtrl::EnableCell(int nRow, int nCol)
{
    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
	{
        return FALSE;
	}

	pCell->state &= ~GVIS_READONLY;
	pCell->state &= ~GVIS_DISABLED;	

    return TRUE;
}

bool CModGridCtrl::IsCellEnabled(const CCellID& cell)const
{
	return this->IsCellEnabled(cell.row, cell.col);
}

bool CModGridCtrl::IsCellEnabled(int nRow, int nCol)const
{
    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
	{
        return false;
	}
	return ((pCell->state & GVIS_DISABLED) == 0);
}
