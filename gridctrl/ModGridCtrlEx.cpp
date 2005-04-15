#include "stdafx.h"
#include "ModGridCtrlEx.h"

#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED 0x031A
#endif

#include "ListBoxDrop.h"

CTheme CModGridCtrlEx::s_themeButton;
CTheme CModGridCtrlEx::s_themeCombo;

CModGridCtrlEx::CModGridCtrlEx(int nRows, int nCols, int nFixedRows, int nFixedCols)
: CModGridCtrl(nRows, nCols, nFixedRows, nFixedCols)
, m_bButtonDown(FALSE)
, m_bButtonCaptured(FALSE)
{
	s_themeButton.Create(L"BUTTON");
	s_themeCombo.Create(L"COMBOBOX");
}

CModGridCtrlEx::~CModGridCtrlEx(void)
{
	std::map<int, CListBox*>::iterator iter = this->m_mapColToList.begin();
	for (; iter != this->m_mapColToList.end(); ++iter) {
		delete iter->second;
	}
}

BOOL CModGridCtrlEx::DrawCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk)
{
	BOOL bValue = CModGridCtrl::DrawCell(pDC, nRow, nCol, rect, bEraseBk);

	if (bValue)
	{
		GV_ITEM Item;
		Item.mask = GVIF_STATE;
		Item.row = nRow;
		Item.col = nCol;
		if (!GetItem(&Item))
			return FALSE;

		// draw button
		if ((Item.state & GVIS_FOCUSED))
		{
			if (this->IsDropDownCell(nRow, nCol))
			{
				this->DrawButton2(pDC, nRow, nCol, rect, bEraseBk, this->m_bButtonDown);
				// redraw focus rect
				pDC->SelectStockObject(BLACK_PEN);
				pDC->SelectStockObject(NULL_BRUSH);
				pDC->Rectangle(rect);
			}
		}
		if (this->IsCheckMarkCell(nRow, nCol))
		{
			this->DrawButtonCheck(pDC, nRow, nCol, rect, bEraseBk, this->GetCheck(nRow, nCol));
		}
	}
    return bValue;
}

void CModGridCtrlEx::DrawButton(CDC* pDC, int nRow, int nCol, CRect rcCell, BOOL bEraseBk, BOOL bDown)
{
	UNREFERENCED_PARAMETER(bEraseBk);
	const int nButtonWidth = 17;
	const CPoint& ptPos = rcCell.TopLeft();
	if (bDown)
	{
		CRect rc(ptPos, CSize(nButtonWidth, this->GetRowHeight(nRow)));
		rc.OffsetRect(max(this->GetColumnWidth(nCol) - nButtonWidth, 0), 0);
		rc.DeflateRect(0, 1, 1, 1);
		this->m_rcButton = rc;

		CBrush brush;
		brush.CreateSysColorBrush(COLOR_3DFACE);
        pDC->FillRect(rc, &brush);

		pDC->DrawEdge(rc, EDGE_SUNKEN, BF_BOTTOMRIGHT);
		rc.DeflateRect(0, 0, 1, 1);
		pDC->DrawEdge(rc, BDR_SUNKENINNER, BF_TOPLEFT);
		rc.DeflateRect(1, 1, 1, 1);
		pDC->DrawEdge(rc, BDR_SUNKENOUTER, BF_TOPLEFT);
	}
	else
	{
		CRect rc(ptPos, CSize(nButtonWidth, this->GetRowHeight(nRow)));
		rc.OffsetRect(max(this->GetColumnWidth(nCol) - nButtonWidth, 0), 0);
		rc.DeflateRect(0, 1, 1, 1);
		this->m_rcButton = rc;

		CBrush brush;
		brush.CreateSysColorBrush(COLOR_3DFACE);
		pDC->FillRect(rc, &brush);

		pDC->DrawEdge(rc, EDGE_RAISED, BF_BOTTOMRIGHT);
		rc.DeflateRect(0, 0, 1, 1);
		pDC->DrawEdge(rc, BDR_RAISEDINNER, BF_TOPLEFT);
	}

	// draw down arrow
	CRect rc(this->m_rcButton.TopLeft(), CSize(7, 1));
	rc.OffsetRect(4, 7);
	if (bDown)
	{
		rc.OffsetRect(1, 1);
	}
	pDC->FillRect(rc, CBrush::FromHandle((HBRUSH)::GetSysColorBrush(COLOR_BTNTEXT)));

	rc.OffsetRect(0, 1);
	rc.DeflateRect(1, 0, 1, 0);
	pDC->FillRect(rc, CBrush::FromHandle((HBRUSH)::GetSysColorBrush(COLOR_BTNTEXT)));

	rc.OffsetRect(0, 1);
	rc.DeflateRect(1, 0, 1, 0);
	pDC->FillRect(rc, CBrush::FromHandle((HBRUSH)::GetSysColorBrush(COLOR_BTNTEXT)));

	rc.OffsetRect(0, 1);
	rc.DeflateRect(1, 0, 1, 0);
	pDC->FillRect(rc, CBrush::FromHandle((HBRUSH)::GetSysColorBrush(COLOR_BTNTEXT)));
}

CString CModGridCtrlEx::GetItemText(const CCellID& cell)
{
	return CGridCtrl::GetItemText(cell.row, cell.col);
}

CString CModGridCtrlEx::GetItemText(int nRow, int nCol)
{
	return CGridCtrl::GetItemText(nRow, nCol);
}

BOOL CModGridCtrlEx::SetColumnOptions(int nCol, const std::vector<LPCTSTR>& vecOptions)
{
	try {
		// NOTE: may want to update map on Insert/Delete Column

		//{{HACK
		// don't allow paste
		for (int i = 0; i < this->GetRowCount(); ++i) {
			this->SetItemState(i, nCol, this->GetItemState(i, nCol) | GVIS_READONLY);
		}
		//}}HACK

		this->m_mapColToOptions.insert(std::map< int, std::vector<LPCTSTR> >::value_type(nCol, vecOptions));
		this->m_mapColToList.insert(std::map< int, CListBox* >::value_type(nCol, new CListBoxDrop()));

		ASSERT(this->m_mapColToList[nCol]);
		CString strWindowName;
#ifdef _DEBUG
		strWindowName.Format(_T("Col %d"), nCol);
#endif

		CRect rect(0, 0, 0, 0);
		VERIFY(this->m_mapColToList[nCol]->CreateEx(
			WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
			_T("LISTBOX"),
			strWindowName,
			WS_CHILDWINDOW|WS_CLIPSIBLINGS|WS_BORDER,
			rect,
			CWnd::GetDesktopWindow(),
			IDC_INPLACE_CONTROL,
			NULL
			));

		this->m_mapColToList[nCol]->SetFont(this->GetParent()->GetFont());

		this->m_mapColToList[nCol]->SetOwner(this);
		ASSERT(this->m_mapColToList[nCol]->GetOwner() == this);	
		ASSERT(this->m_mapColToList[nCol]->GetParent() == CWnd::GetDesktopWindow());	

		std::vector<LPCTSTR>::const_iterator opt = vecOptions.begin();
		for (; opt != vecOptions.end(); ++opt) {
			this->m_mapColToList[nCol]->AddString(*opt);
		}
		ASSERT(this->m_mapColToList[nCol]->GetCount() == (int)vecOptions.size());
	}
	catch(...) {
		ASSERT(FALSE);
		return FALSE;
	}
	return TRUE;
}

bool CModGridCtrlEx::IsDropDownCell(const CCellID& cell)const
{
	return this->IsDropDownCell(cell.row, cell.col);
}

bool CModGridCtrlEx::IsDropDownCell(int nRow, int nCol)const
{
	if (nRow < this->GetFixedRowCount()) return false;
	if (nCol < this->GetFixedColumnCount()) return false;

	std::map<int, std::vector<LPCTSTR> >::const_iterator iter = this->m_mapColToOptions.find(nCol);
	ASSERT(iter != this->m_mapColToOptions.end() ||
		this->m_mapColToList.find(nCol) == this->m_mapColToList.end());
	return (iter != this->m_mapColToOptions.end());
}

bool CModGridCtrlEx::IsCheckMarkCell(const CCellID& cell)const
{
	return this->IsCheckMarkCell(cell.row, cell.col);
}

bool CModGridCtrlEx::IsCheckMarkCell(int nRow, int nCol)const
{
	if (CGridCell *pCell = CGridCtrl::GetCell(nRow, nCol))
	{
		return ((pCell->state & GVIM_CHECKMARK) != 0);
	}
	return false;
}

int CModGridCtrlEx::GetCheck(const CCellID& cell)const
{
	return this->GetCheck(cell.row, cell.col);
}

int CModGridCtrlEx::GetCheck(int nRow, int nCol)const
{
	if (CGridCell *pCell = CGridCtrl::GetCell(nRow, nCol))
	{
		if (pCell->state & GVIM_CHECKMARK)
		{
			switch (pCell->state & GVIM_CHECKMARK)
			{
			case GVIS_UNCHECKED:
				return BST_UNCHECKED;
			case GVIS_CHECKED:
				return BST_CHECKED;
			case GVIS_INDETERMINATE:
				return BST_INDETERMINATE;
			default:
				return BST_UNCHECKED;
			}
		}
	}
	return BST_UNCHECKED;
}

BOOL CModGridCtrlEx::SetCheck(const CCellID& cell, int nState)
{
	return this->SetCheck(cell.row, cell.col, nState);
}

BOOL CModGridCtrlEx::SetCheck(int nRow, int nCol, int nState)
{
    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell)
	{
        return FALSE;
	}

	// clear current
    pCell->state &= ~GVIM_CHECKMARK;

	switch (nState)
	{
	case BST_UNCHECKED:
		pCell->state |= GVIS_UNCHECKED;
		pCell->state |= GVIS_READONLY;
		break;
	case BST_CHECKED:
		pCell->state |= GVIS_CHECKED;
		pCell->state |= GVIS_READONLY;
		break;
	case BST_INDETERMINATE:
		pCell->state |= GVIS_INDETERMINATE;
		pCell->state |= GVIS_READONLY;
		break;
	default:
		ASSERT(FALSE);
		return FALSE;
	}
    return TRUE;
}

const std::vector<LPCTSTR>* CModGridCtrlEx::GetOptionsVector(const CCellID& cell)const
{
	return this->GetOptionsVector(cell.row, cell.col);
}

const std::vector<LPCTSTR>* CModGridCtrlEx::GetOptionsVector(int nRow, int nCol)const
{
	if (nRow < this->GetFixedRowCount()) return 0;
	if (nCol < this->GetFixedColumnCount()) return 0;

	std::map<int, std::vector<LPCTSTR> >::const_iterator iter =
		this->m_mapColToOptions.find(nCol);

	if (iter != this->m_mapColToOptions.end()) {
		return &(iter->second);
	}
	return 0;
}

CListBox* CModGridCtrlEx::GetListBox(const CCellID& cell)
{
	return this->GetListBox(cell.row, cell.col);
}

CListBox* CModGridCtrlEx::GetListBox(int nRow, int nCol)
{
	if (nRow < this->GetFixedRowCount()) return 0;
	if (nCol < this->GetFixedColumnCount()) return 0;

	std::map<int, CListBox*>::iterator iter = this->m_mapColToList.find(nCol);
	if (iter != this->m_mapColToList.end()) {
		return (iter->second);
	}
	return 0;
}


BEGIN_MESSAGE_MAP(CModGridCtrlEx, CModGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//ON_WM_THEMECHANGED()
	{ WM_THEMECHANGED, 0, 0, 0, AfxSig_l, \
		(AFX_PMSG)(AFX_PMSGW) \
		(static_cast< LRESULT (AFX_MSG_CALL CWnd::*)(void) > (OnThemeChanged)) },
END_MESSAGE_MAP()

void CModGridCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CModGridCtrl::OnLButtonDown(nFlags, point);
	TraceMouseMode();

	if (this->m_MouseMode == MOUSE_PREPARE_EDIT)
	{
		// This state occurs when user clicks within currently selected cell
		if (this->IsDropDownCell(this->m_idCurrentCell))
		{
			ASSERT(this->m_LeftClickDownCell == this->m_idCurrentCell);
			this->m_MouseMode = MOUSE_NOTHING;
			if (this->m_rcButton.PtInRect(point))
			{
				this->SetCapture();
				this->m_bButtonCaptured = TRUE;
				this->m_bButtonDown = TRUE;
				this->RedrawCell(this->m_idCurrentCell);

				CRect rc;
				if (!this->GetCellRect(this->m_idCurrentCell, &rc)) return;
				this->MapWindowPoints(CWnd::GetDesktopWindow(), &rc);

				GV_DISPINFO dispinfo;
			    
				dispinfo.hdr.hwndFrom = GetSafeHwnd();
				dispinfo.hdr.idFrom   = GetDlgCtrlID();
				dispinfo.hdr.code     = LBDM_EDITLABEL;
				dispinfo.item.mask    = LVIF_TEXT|LVIF_PARAM;
				dispinfo.item.row     = this->m_idCurrentCell.row;
				dispinfo.item.col     = this->m_idCurrentCell.col;
				if (!this->GetItem(&dispinfo.item)) return;
				dispinfo.item.lParam  = (LPARAM)&rc; // screen coordinates

				if (CListBox* pListBox = this->GetListBox(this->m_idCurrentCell))
				{
					pListBox->SendMessage(LBDM_EDITLABEL, 0, (LPARAM)&dispinfo);
					pListBox->GetWindowRect(&this->m_rcListBox);
					this->ScreenToClient(&this->m_rcListBox);
				}
			}			
		}
		else if (this->IsCheckMarkCell(this->m_LeftClickDownCell.row, this->m_LeftClickDownCell.col))
		{
			ASSERT(CWnd::GetCapture() == 0);
			CRect rc = this->GetCheckRect(this->m_LeftClickDownCell.row, this->m_LeftClickDownCell.col);
			if (rc.PtInRect(point))
			{
				TRACE("LButtonDown in Check 1\n");
				this->SetCapture();
				this->m_bButtonCaptured = TRUE;
				this->m_bButtonDown = TRUE;
				this->RedrawCell(this->m_LeftClickDownCell);
			}
		}
	}
	else if (this->m_MouseMode == MOUSE_SELECT_CELLS)
	{
		ASSERT(CWnd::GetCapture() == this); // captured in CGridCtrl::OnLButtonDown
		if (this->IsCheckMarkCell(this->m_LeftClickDownCell.row, this->m_LeftClickDownCell.col))
		{
			CRect rc = this->GetCheckRect(this->m_LeftClickDownCell.row, this->m_LeftClickDownCell.col);
			if (rc.PtInRect(point))
			{
				// kill timer if active
				// (disable dragging/autoscroll)
				if (this->m_nTimerID != 0)
				{
					this->KillTimer(this->m_nTimerID);
					this->m_nTimerID = 0;
				}
				ASSERT(CWnd::GetCapture() == this); // captured in CGridCtrl::OnLButtonDown
				TRACE("LButtonDown in Check 2\n");
				this->m_bButtonCaptured = TRUE;
				this->m_bButtonDown = TRUE;
				this->RedrawCell(this->m_LeftClickDownCell);
			}
		}
	}
}

void CModGridCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (this->m_MouseMode == MOUSE_NOTHING
		&&
		this->IsDropDownCell(this->m_idCurrentCell)
		&&
		this->m_LeftClickDownCell == this->m_idCurrentCell
		)
	{	
		if (!this->IsCellEnabled(this->m_idCurrentCell)) return;

		const std::vector<LPCTSTR>* pVec = 
			this->GetOptionsVector(this->m_idCurrentCell);
		ASSERT(pVec);

		int nItemCount = (int)pVec->size();
		CString item = this->GetItemText(this->m_idCurrentCell);
		int nCurrent = -1;
		for (int i = 0; i < nItemCount; ++i) {
			if (item.Compare(pVec->at(i)) == 0) {
				nCurrent = i;
				break;
			}
		}
		int nNext = (nCurrent + 1) % nItemCount;
		// Note: OnEndEditCell doesn't check if cell is editable
		this->OnEndEditCell(this->m_idCurrentCell.row, this->m_idCurrentCell.col, pVec->at(nNext));
		this->RedrawCell(this->m_idCurrentCell);
	}
	else
	{
		// Note: this->m_LeftClickDownCell could probably work here
		// since by the time this message is received an LButtonDown
		// and an LButtonUp have already been processed
		//
		CCellID dblClkCell = this->GetCellFromPt(point);
		if (this->IsCheckMarkCell(dblClkCell))
		{
			//{{ {4/14/2005 8:02:17 PM}
			if (!this->m_idCurrentCell.IsValid()) return;
			//}} {4/14/2005 8:02:17 PM}
			if (!this->IsCellEnabled(this->m_idCurrentCell)) return;

			CRect rc = this->GetCheckRect(dblClkCell.row, dblClkCell.col);
			if (rc.PtInRect(point))
			{
				TRACE("LButtonDblClk in Check\n");
				switch (this->GetCheck(dblClkCell))
				{
				case BST_CHECKED:
					this->SetCheck(dblClkCell, BST_UNCHECKED);
					this->SetModified(TRUE, dblClkCell.row, dblClkCell.col);
					this->SendMessageToParent(dblClkCell.row, dblClkCell.col, GVN_ITEMCHANGED);
					this->RedrawCell(dblClkCell);
					break;
				case BST_UNCHECKED:
					this->SetCheck(dblClkCell, BST_CHECKED);
					this->SetModified(TRUE, dblClkCell.row, dblClkCell.col);
					this->SendMessageToParent(dblClkCell.row, dblClkCell.col, GVN_ITEMCHANGED);
					this->RedrawCell(dblClkCell);
					break;
				case BST_INDETERMINATE:
					break;
				default:
					ASSERT(FALSE);
				}
				return;
			}
		}
		CModGridCtrl::OnLButtonDblClk(nFlags, point);
	}
}

void CModGridCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	// Add your message handler code here and/or call default
	if (this->m_bButtonCaptured)
	{
		ASSERT(CWnd::GetCapture() == this);
		if (this->IsDropDownCell(this->m_idCurrentCell))
		{
			BOOL bLastButtonDown = this->m_bButtonDown;
			this->m_bButtonDown = this->m_rcButton.PtInRect(point);
			if (nFlags & MK_LBUTTON && this->m_rcListBox.PtInRect(point))
			{
				// transfer capture to listctrl
				this->m_bButtonDown = FALSE;
				this->m_bButtonCaptured = FALSE;
				if (CWnd::GetCapture() == this)
				{
					::ReleaseCapture();
				}
			}
			if (bLastButtonDown != this->m_bButtonDown)
			{
				this->RedrawCell(this->m_idCurrentCell);
			}
		}
		else if (this->IsCheckMarkCell(this->m_LeftClickDownCell))
		{
			this->RedrawCell(this->m_LeftClickDownCell);
		}
	}
	else
	{
		ASSERT(this->m_bButtonDown == FALSE);
		this->m_bButtonDown = FALSE;
		if (this->s_themeButton || this->s_themeCombo)
		{
			if (this->m_idLastHotCell.IsValid())
			{
				this->RedrawCell(this->m_idLastHotCell);
				this->m_idLastHotCell.col = this->m_idLastHotCell.row = -1;
			}
			CCellID idHotCell = this->GetCellFromPt(point);
			if (this->IsCheckMarkCell(idHotCell))
			{
				this->RedrawCell(idHotCell);
				this->m_idLastHotCell = idHotCell;
			}
			if (idHotCell == this->m_idCurrentCell)
			{
				if (this->IsDropDownCell(idHotCell))
				{
					this->RedrawCell(idHotCell);
					this->m_idLastHotCell = idHotCell;
				}
			}
		}
		CModGridCtrl::OnMouseMove(nFlags, point);
	}
}

void CModGridCtrlEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	// Add your message handler code here and/or call default
	if (this->m_bButtonCaptured)
	{
		//// ASSERT(this->IsCheckMarkCell(this->m_idCurrentCell) ^ this->IsDropDownCell(this->m_idCurrentCell));

		ASSERT(CWnd::GetCapture() == this);
		::ReleaseCapture();
		this->m_bButtonCaptured = FALSE;
		this->m_bButtonDown = FALSE;
		this->RedrawCell(this->m_idCurrentCell);

		if (this->IsDropDownCell(this->m_idCurrentCell))
		{
			CListBox* pListBox = this->GetListBox(this->m_idCurrentCell);
			if (pListBox && pListBox->IsWindowVisible())
			{
				pListBox->SendMessage(LBDM_SETCAPTURE, 0, 0);
			}
		}
		else if (this->IsCheckMarkCell(this->m_LeftClickDownCell))
		{
			CRect rc = this->GetCheckRect(this->m_LeftClickDownCell.row, this->m_LeftClickDownCell.col);
			if (rc.PtInRect(point))
			{
				TRACE("LButtonUp in Check\n");
				switch (this->GetCheck(this->m_LeftClickDownCell.row, this->m_LeftClickDownCell.col))
				{
				case BST_CHECKED:
					this->SetCheck(this->m_LeftClickDownCell, BST_UNCHECKED);
					this->SetModified(TRUE, this->m_LeftClickDownCell.row, this->m_LeftClickDownCell.col);
					this->SendMessageToParent(this->m_LeftClickDownCell.row, this->m_LeftClickDownCell.col, GVN_ITEMCHANGED);
					this->RedrawCell(this->m_LeftClickDownCell);
					break;
				case BST_UNCHECKED:
					this->SetCheck(this->m_LeftClickDownCell, BST_CHECKED);
					this->SetModified(TRUE, this->m_LeftClickDownCell.row, this->m_LeftClickDownCell.col);
					this->SendMessageToParent(this->m_LeftClickDownCell.row, this->m_LeftClickDownCell.col, GVN_ITEMCHANGED);
					this->RedrawCell(this->m_LeftClickDownCell);
					break;
				case BST_INDETERMINATE:
					break;
				default:
					ASSERT(FALSE);
				}
			}
		}
	}
	else
	{
		CModGridCtrl::OnLButtonUp(nFlags, point);
	}

	this->m_bButtonDown = FALSE;
}

void CModGridCtrlEx::ShowDropDown(BOOL bShowIt)
{
	if (bShowIt)
	{
		/* TODO
		if (CWnd* pParent = this->GetParent()) {
			pParent->SendMessage(WM_COMMAND, CBN_DROPDOWN, (LPARAM)this->GetSafeHwnd());
		}
		*/

		// determine size of dropdown list
		CRect rc;
		if (!this->GetCellRect(this->m_idCurrentCell, &rc)) return;
		this->MapWindowPoints(CWnd::GetDesktopWindow(), &rc);
		rc.OffsetRect(0, rc.Height());

		int x, y, cx, cy;
		cx = rc.Width();
		x = rc.left;
		y = rc.top;
		this->m_mapColToList[this->m_idCurrentCell.col]->GetCurSel();
		this->m_mapColToList[this->m_idCurrentCell.col]->SetTopIndex(0);
		cy = this->m_mapColToList[this->m_idCurrentCell.col]->GetItemHeight(0) * this->m_mapColToList[this->m_idCurrentCell.col]->GetCount() + 2;

		// this->m_rcListBox.SetRect(x, y, cx, cy);
		this->m_rcListBox.SetRect(x, y, x+cx, y+cy);
		this->ScreenToClient(&this->m_rcListBox);

		// place in front of all windows
		this->m_mapColToList[this->m_idCurrentCell.col]->SetWindowPos(&wndTopMost, x, y, cx, cy, SWP_NOACTIVATE|SWP_SHOWWINDOW);

	}
	else
	{
		// hide DropDown
		this->m_mapColToList[this->m_idCurrentCell.col]->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_HIDEWINDOW);

		if (CWnd* pParent = this->GetParent()) {
			/** TODO
			pParent->SendMessage(
				WM_COMMAND,
				MAKEWORD(this->GetDlgCtrlID(), CBN_CLOSEUP),
				(LPARAM)this->GetSafeHwnd());
			**/
		}
	}
}

void CModGridCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CListBox* pListBox = this->GetListBox(this->m_idCurrentCell);
	if (pListBox && pListBox->IsWindowVisible()) {
		pListBox->SendMessage(WM_KEYDOWN, nChar, MAKEWORD(nRepCnt, nFlags));
		return;
		//switch (nChar) {
		//	case VK_PRIOR: // Page Down
		//	case VK_NEXT:  // Page Up
		//	case VK_END:
		//	case VK_HOME:
		//	case VK_LEFT:
		//	case VK_UP:
		//	case VK_RIGHT:
		//	case VK_DOWN:
		//		pListBox->SendMessage(WM_KEYDOWN, nChar, MAKEWORD(nRepCnt, nFlags));
		//		return;
		//		break;
		//}
	}
	CModGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CModGridCtrlEx::PreTranslateMessage(MSG* pMsg)
{
	CListBox* pListBox = this->GetListBox(this->m_idCurrentCell);
	if (pListBox)
	{
		if (pListBox->IsWindowVisible())
		{
			// Esc
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
			{
				return pListBox->SendMessage(LBDM_ENDEDITLABELNOW, TRUE, 0);
			}

			// Enter
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
			{
				return pListBox->SendMessage(LBDM_ENDEDITLABELNOW, FALSE, 0);
			}

			// F4
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F4)
			{
				return pListBox->SendMessage(LBDM_ENDEDITLABELNOW, FALSE, 0);
			}

			// Alt+Down
			if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_DOWN)
			{
				return pListBox->SendMessage(LBDM_ENDEDITLABELNOW, FALSE, 0);
			}
		}
		else
		{
			// F4
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F4)
			{
				this->OnLButtonDown(0, this->m_rcButton.CenterPoint());
				this->OnLButtonUp(0, this->m_rcButton.CenterPoint());
				return TRUE;
			}

			// Alt+Down
			if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_DOWN)
			{
				this->OnLButtonDown(0, this->m_rcButton.CenterPoint());
				this->OnLButtonUp(0, this->m_rcButton.CenterPoint());
				return TRUE;
			}
		}
	}
	return CModGridCtrl::PreTranslateMessage(pMsg);
}

void CModGridCtrlEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//
	// Option list handling
	//
	CListBox* pListBox = this->GetListBox(this->m_idCurrentCell);
	if (pListBox)
	{
		if (pListBox->IsWindowVisible())
		{
			LRESULT lresult = pListBox->SendMessage(WM_CHAR, nChar, MAKEWORD(nRepCnt, nFlags));
			UNREFERENCED_PARAMETER(lresult);
			return;
		}
		else
		{
			if (!IsCTRLpressed() && m_MouseMode == MOUSE_NOTHING)
			{
				if (!this->m_bHandleTabKey || (this->m_bHandleTabKey && nChar != VK_TAB))
				{
					int nItemCount = pListBox->GetCount();
					int nItem = -1;
					CString str;
					for (int i = 0; i < nItemCount; ++i)
					{
						pListBox->GetText(i, str);
						UINT nFirstChar = str[0];
						if (::toupper(nFirstChar) == ::toupper(nChar))
						{
							nItem = i;
							break;
						}
					}
					if (nItem > -1)
					{
						this->OnEndEditCell(this->m_idCurrentCell.row, this->m_idCurrentCell.col, str);
						this->RedrawCell(this->m_idCurrentCell);
					}
					CWnd::OnChar(nChar, nRepCnt, nFlags);
					return;
				}
			}
		}
	}
	//
	// Checkmark handling
	//
	if (this->IsCheckMarkCell(this->m_idCurrentCell) && nChar == ' ')
	{
		TRACE("CModGridCtrlEx::OnChar in Check\n");
		switch (this->GetCheck(this->m_idCurrentCell))
		{
		case BST_CHECKED:
			this->SetCheck(this->m_idCurrentCell, BST_UNCHECKED);
			this->SetModified(TRUE, this->m_idCurrentCell.row, this->m_idCurrentCell.col);
			this->SendMessageToParent(this->m_idCurrentCell.row, this->m_idCurrentCell.col, GVN_ITEMCHANGED);
			this->RedrawCell(this->m_idCurrentCell);
			break;
		case BST_UNCHECKED:
			this->SetCheck(this->m_idCurrentCell, BST_CHECKED);
			this->SetModified(TRUE, this->m_idCurrentCell.row, this->m_idCurrentCell.col);
			this->SendMessageToParent(this->m_idCurrentCell.row, this->m_idCurrentCell.col, GVN_ITEMCHANGED);
			this->RedrawCell(this->m_idCurrentCell);
			break;
		case BST_INDETERMINATE:
			break;
		default:
			ASSERT(FALSE);
		}
	}

	CModGridCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void CModGridCtrlEx::DrawButton2(CDC* pDC, int nRow, int nCol, CRect rcCell, BOOL bEraseBk, BOOL bDown)
{
	if (!this->IsCellEnabled(nRow, nCol)) return;

	UNREFERENCED_PARAMETER(bEraseBk);
	const int nButtonWidth = ::GetSystemMetrics(SM_CXVSCROLL);

	const CPoint& ptPos = rcCell.TopLeft();
	if (bDown)
	{
		CRect rc(ptPos, CSize(nButtonWidth, this->GetRowHeight(nRow)));
		rc.OffsetRect(max(this->GetColumnWidth(nCol) - nButtonWidth, 0), 0);
		rc.DeflateRect(-1, 1, 1, 1);
		this->m_rcButton = rc;
		if (this->s_themeCombo)
		{
			rc.DeflateRect(0, 0, 0, 1);
			g_xpStyle.DrawThemeBackground(this->s_themeCombo, *pDC, CP_DROPDOWNBUTTON, CBXS_PRESSED, &rc, NULL);			
		}
		else
		{
			pDC->DrawFrameControl(&rc, DFC_SCROLL, DFCS_SCROLLCOMBOBOX | DFCS_PUSHED);
		}
		return;
	}
	else
	{
		CRect rc(ptPos, CSize(nButtonWidth, this->GetRowHeight(nRow)));
		rc.OffsetRect(max(this->GetColumnWidth(nCol) - nButtonWidth, 0), 0);
		rc.DeflateRect(-1, 1, 1, 1);
		this->m_rcButton = rc;
		if (this->s_themeCombo)
		{
			rc.DeflateRect(0, 0, 0, 1);
			CPoint point;
			::GetCursorPos(&point);
			this->ScreenToClient(&point);
			if (rc.PtInRect(point))
			{
				g_xpStyle.DrawThemeBackground(this->s_themeCombo, *pDC, CP_DROPDOWNBUTTON, CBXS_HOT, &rc, NULL);
				return;
			}
			else
			{
				g_xpStyle.DrawThemeBackground(this->s_themeCombo, *pDC, CP_DROPDOWNBUTTON, CBXS_NORMAL, &rc, NULL);			
			}
		}
		else
		{
			pDC->DrawFrameControl(&rc, DFC_SCROLL, DFCS_SCROLLCOMBOBOX);
		}
	}
}

#include <uxtheme.h>
void CModGridCtrlEx::DrawButtonCheck(CDC* pDC, int nRow, int nCol, CRect rcCell, BOOL bEraseBk, int nCheck)
{
	UNREFERENCED_PARAMETER(bEraseBk);

	// State of the check box: 0 for clear, 1 for checked, and 2 for indeterminate. 
	UINT uState;
	switch (nCheck)
	{
	case BST_UNCHECKED:
		uState = DFCS_BUTTONCHECK | DFCS_FLAT;
		break;
	case BST_CHECKED:
		uState = DFCS_BUTTONCHECK | DFCS_FLAT | DFCS_CHECKED;
		break;
	case BST_INDETERMINATE:
		uState = DFCS_BUTTONCHECK | DFCS_FLAT | DFCS_INACTIVE;
		break;
	default:
		uState = DFCS_BUTTONCHECK | DFCS_FLAT;
		break;
	}

	CRect rc = this->GetCheckRect(nRow, nCol);
	CPoint point;
	::GetCursorPos(&point);
	this->ScreenToClient(&point);

	if (this->s_themeButton)
	{
		int iStateId;
		bool bHot = (rc.PtInRect(point) == TRUE);
		if (bHot && this->m_bButtonDown)
		{
			switch (nCheck)
			{
			case BST_UNCHECKED:
				iStateId = CBS_UNCHECKEDPRESSED;
				break;
			case BST_CHECKED:
				iStateId = CBS_CHECKEDPRESSED;
				break;
			case BST_INDETERMINATE:
				iStateId = CBS_UNCHECKEDDISABLED;
				break;
			default:
				ASSERT(FALSE);
				iStateId = CBS_UNCHECKEDPRESSED;
				break;
			}
			g_xpStyle.DrawThemeBackground(this->s_themeButton, *pDC, BP_CHECKBOX, iStateId, &rc, NULL);
			return;
		}
		if (!bHot && this->m_bButtonDown)
		{
			if (this->m_LeftClickDownCell.row == nRow && this->m_LeftClickDownCell.col == nCol)
			{
				//
				bHot = true;
			}
		}
		switch (nCheck)
		{
		case BST_UNCHECKED:
			iStateId = bHot ? CBS_UNCHECKEDHOT : CBS_UNCHECKEDNORMAL;
			break;
		case BST_CHECKED:
			iStateId = bHot ? CBS_CHECKEDHOT : CBS_CHECKEDNORMAL;
			break;
		case BST_INDETERMINATE:
			iStateId = CBS_UNCHECKEDDISABLED;
			break;
		default:
			iStateId = bHot ? CBS_UNCHECKEDHOT : CBS_UNCHECKEDNORMAL;
			break;
		}
		g_xpStyle.DrawThemeBackground(this->s_themeButton, *pDC, BP_CHECKBOX, iStateId, &rc, NULL);
	}
	else
	{
		if (this->m_bButtonDown && this->m_LeftClickDownCell.row == nRow && this->m_LeftClickDownCell.col == nCol)
		{
			if (rc.PtInRect(point))
			{
				uState |= DFCS_INACTIVE;
			}
		}
		pDC->DrawFrameControl(&rc, DFC_BUTTON, uState);		
	}
	return;
}

CRect CModGridCtrlEx::GetCheckRect(int nRow, int nCol)const
{
	CRect rcCell;
	if (this->GetCellRect(nRow, nCol, &rcCell))
	{
		const int nCheckWidth = ::GetSystemMetrics(SM_CXMENUCHECK);

		CRect rc(rcCell.TopLeft(), CSize(nCheckWidth, this->GetRowHeight(nRow)));
		rc.OffsetRect(max(this->GetColumnWidth(nCol) - nCheckWidth, 0) / 2, 0);
		rc.DeflateRect(0, 1, 0, 1);
		return rc;
	}
	return CRect(0, 0, 0, 0);
}

BOOL CModGridCtrlEx::SetColumnCheck(int nCol, int nState)
{
	try
	{
		for (int i = 0; i < this->GetRowCount(); ++i)
		{
			this->SetCheck(i, nCol, nState);
		}
	}
	catch(...)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	return TRUE;
}

void CModGridCtrlEx::TraceMouseMode(void)const
{
	switch (this->m_MouseMode)
	{
	case MOUSE_NOTHING:
		TRACE("MOUSE_NOTHING\n");
		break;
	case MOUSE_SELECT_ALL:
		TRACE("MOUSE_SELECT_ALL\n");
		break;
	case MOUSE_SELECT_COL:
		TRACE("MOUSE_SELECT_COL\n");
		break;
	case MOUSE_SELECT_ROW:
		TRACE("MOUSE_SELECT_ROW\n");
		break;
	case MOUSE_SELECT_CELLS:
		TRACE("MOUSE_SELECT_CELLS\n");
		break;
	case MOUSE_SCROLLING_CELLS:
		TRACE("MOUSE_SCROLLING_CELLS\n");
		break;
	case MOUSE_OVER_ROW_DIVIDE:
		TRACE("MOUSE_OVER_ROW_DIVIDE\n");
		break;
	case MOUSE_SIZING_ROW:
		TRACE("MOUSE_SIZING_ROW\n");
		break;
	case MOUSE_OVER_COL_DIVIDE:
		TRACE("MOUSE_OVER_COL_DIVIDE\n");
		break;
	case MOUSE_SIZING_COL:
		TRACE("MOUSE_SIZING_COL\n");
		break;
	case MOUSE_PREPARE_EDIT:
		TRACE("MOUSE_PREPARE_EDIT\n");
		break;
	case MOUSE_PREPARE_DRAG:
		TRACE("MOUSE_PREPARE_DRAG\n");
		break;
	case MOUSE_DRAGGING:
		TRACE("MOUSE_DRAGGING\n");
		break;
	default:
		TRACE("Unknown mouse state\n");
		break;
	}
}

LRESULT CModGridCtrlEx::OnThemeChanged()
{
	// This feature requires Windows XP or greater.
	// The symbol _WIN32_WINNT must be >= 0x0501.
	// TODO: Add your message handler code here and/or call default
	this->s_themeButton.OnThemeChanged(L"BUTTON");
	this->s_themeCombo.OnThemeChanged(L"COMBOBOX");

	return 1;
}
