// CheckTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "CheckTreeCtrl.h"
#include <windowsx.h>


// CCheckTreeCtrl

IMPLEMENT_DYNAMIC(CCheckTreeCtrl, CTreeCtrl)
CCheckTreeCtrl::CCheckTreeCtrl() : m_pImageList(0)
{
}

CCheckTreeCtrl::~CCheckTreeCtrl()
{
	if (this->m_pImageList) {
		delete this->m_pImageList;
	}
}


BEGIN_MESSAGE_MAP(CCheckTreeCtrl, CTreeCtrl)
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CCheckTreeCtrl message handlers


void CCheckTreeCtrl::PreSubclassWindow()
{
	// remove checkboxes state (in case set)
	//
	this->ModifyStyle(TVS_CHECKBOXES, 0);

	if (this->m_pImageList == 0) {
		// setup image list
		//
		this->m_pImageList = new CImageList();
		ASSERT_VALID(this->m_pImageList);
		this->m_pImageList->Create(IDB_CHECKS_TREE, 14, 0, RGB(255, 0, 255));
	}
	this->SetImageList(this->m_pImageList, LVSIL_STATE);

	// Add your specialized code here and/or call the base class
	CTreeCtrl::PreSubclassWindow();
}

HTREEITEM CCheckTreeCtrl::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent/* = TVI_ROOT*/, HTREEITEM hInsertAfter/* = TVI_LAST*/, int nCheck/* = BST_UNCHECKED*/)
{
	HTREEITEM hItem = this->CTreeCtrl::InsertItem(lpszItem, hParent, hInsertAfter);
	switch (nCheck) {
		case BST_UNCHECKED:
			this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1), TVIS_STATEIMAGEMASK);
			break;
		case BST_CHECKED:
			this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
			break;
		case BST_INDETERMINATE:
			this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_INDETERMINATE + 1), TVIS_STATEIMAGEMASK);
			break;
		default:
			ASSERT(FALSE);
			break;
	}
	return hItem;
}

int CCheckTreeCtrl::GetCheck(HTREEITEM hItem)const
{
	switch (this->GetItemState(hItem, TVIS_STATEIMAGEMASK) & TVIS_STATEIMAGEMASK) {
		case (INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1)):
			return BST_UNCHECKED;
			break;
		case (INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1)):
			return BST_CHECKED;
			break;
		case (INDEXTOSTATEIMAGEMASK(BST_INDETERMINATE + 1)):
			return BST_INDETERMINATE;
			break;
		case (INDEXTOSTATEIMAGEMASK(BST_INDETERMINATE + 2)):
			return BST_INDETERMINATE;
			break;
		default:
			ASSERT(FALSE);
			break;
	}
	return BST_UNCHECKED;
}

void CCheckTreeCtrl::SetCheck(HTREEITEM hItem, int nCheck/* = BST_CHECKED*/)
{
	switch (nCheck) {
		case BST_UNCHECKED:
			this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1), TVIS_STATEIMAGEMASK);
			break;
		case BST_CHECKED:
			this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
			break;
		case BST_INDETERMINATE:
			this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_INDETERMINATE + 1), TVIS_STATEIMAGEMASK);
			break;
		case BST_INDETERMINATE|BST_CHECKED:
			this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_INDETERMINATE + 2), TVIS_STATEIMAGEMASK);
			break;
		default:
			ASSERT(FALSE);
			break;
	}
}

BOOL CCheckTreeCtrl::GetEnabled(HTREEITEM hItem)const
{
	switch (this->GetItemState(hItem, TVIS_STATEIMAGEMASK) & TVIS_STATEIMAGEMASK)
	{
	case INDEXTOSTATEIMAGEMASK(1):	// enabled unchecked
		return TRUE;
		break;
	case INDEXTOSTATEIMAGEMASK(2):	// enabled checked
		return TRUE;
		break;
	case INDEXTOSTATEIMAGEMASK(3):	// disabled unchecked
		return FALSE;
		break;
	case INDEXTOSTATEIMAGEMASK(4):	// disabled checked
		return FALSE;
		break;
	default:
		ASSERT(FALSE);
	}
	return FALSE;
}

void CCheckTreeCtrl::SetEnabled(HTREEITEM hItem, BOOL bEnabled)
{
	ASSERT(::IsWindow(this->m_hWnd));

	UINT prevState = this->GetItemState(hItem, TVIS_STATEIMAGEMASK) & TVIS_STATEIMAGEMASK;

	switch (prevState)
	{
	case INDEXTOSTATEIMAGEMASK(1):	// enabled unchecked
		bEnabled ? SetItemState(hItem, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK) :
				   SetItemState(hItem, INDEXTOSTATEIMAGEMASK(3), TVIS_STATEIMAGEMASK);	

		break;
	case INDEXTOSTATEIMAGEMASK(2):	// enabled checked
		bEnabled ? SetItemState(hItem, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK) :
				   SetItemState(hItem, INDEXTOSTATEIMAGEMASK(4), TVIS_STATEIMAGEMASK);	

		break;
	case INDEXTOSTATEIMAGEMASK(3):	// disabled unchecked
		bEnabled ? SetItemState(hItem, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK) :
				   SetItemState(hItem, INDEXTOSTATEIMAGEMASK(3), TVIS_STATEIMAGEMASK);	

		break;
	case INDEXTOSTATEIMAGEMASK(4):	// disabled checked
		bEnabled ? SetItemState(hItem, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK) :
				   SetItemState(hItem, INDEXTOSTATEIMAGEMASK(4), TVIS_STATEIMAGEMASK);	

		break;
	default:
		ASSERT(FALSE);
	}
}


void CCheckTreeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_SPACE:
		{
			HTREEITEM hItem = this->GetSelectedItem();
			UINT state = this->GetItemState(hItem, TVIS_STATEIMAGEMASK) & TVIS_STATEIMAGEMASK;
			switch (state) {
			case (INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1)):
				this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
				break;
			case (INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1)):
				this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1), TVIS_STATEIMAGEMASK);
				break;
			case INDEXTOSTATEIMAGEMASK(BST_INDETERMINATE + 1):
				// do nothing
				return;
				break;
			}
			if (::SendMessage(this->GetParent()->GetSafeHwnd(), UM_CHECKSTATECHANGE, 0, (LPARAM)hItem)) {
				// reset original state
				this->SetItemState(hItem, state, TVIS_STATEIMAGEMASK);
			}
			return;
		}
		break;
	}
	
	CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void CCheckTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	TVHITTESTINFO hitTestInfo;
	hitTestInfo.pt = point;

	HTREEITEM hItem = this->HitTest(&hitTestInfo);
	if (hitTestInfo.flags & TVHT_ONITEMSTATEICON)
	{
		ASSERT(hItem == hitTestInfo.hItem);
		UINT state = this->GetItemState(hItem, TVIS_STATEIMAGEMASK) & TVIS_STATEIMAGEMASK;
		switch (state)
		{
		case INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1):
			this->SetRedraw(FALSE);
			this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
			break;
		case INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1):
			this->SetRedraw(FALSE);
			this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1), TVIS_STATEIMAGEMASK);
			break;
		case INDEXTOSTATEIMAGEMASK(BST_INDETERMINATE + 1):
			// do nothing
			return;
			break;
		case INDEXTOSTATEIMAGEMASK(BST_INDETERMINATE + 2):
			// do nothing
			return;
			break;
		default:
			ASSERT(FALSE);
		}
		if (::SendMessage(this->GetParent()->GetSafeHwnd(), UM_CHECKSTATECHANGE, 0, (LPARAM)hItem)) {
			// reset original state
			this->SetItemState(hItem, state, TVIS_STATEIMAGEMASK);
		}
		else {
			this->SelectItem(hItem);
		}
		this->SetRedraw(TRUE);
		return; // don't allow default processing
	}

	// Add your message handler code here and/or call default
	CTreeCtrl::OnLButtonDown(nFlags, point);
}
