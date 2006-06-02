#pragma once

#define UM_CHECKSTATECHANGE (WM_USER + 100)

// CCheckTreeCtrl

class CCheckTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CCheckTreeCtrl)

public:
	CCheckTreeCtrl();
	virtual ~CCheckTreeCtrl();

	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST, int nCheck = BST_UNCHECKED);
	void SetCheck(HTREEITEM hItem, int nCheck = BST_CHECKED);
	int GetCheck(HTREEITEM hItem)const;

	void SetEnabled(HTREEITEM hItem, BOOL bEnabled);
	BOOL GetEnabled(HTREEITEM hItem)const;


protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();

	CImageList *m_pImageList;
	
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
	// void SetCheck(HTREEITEM hItem, BOOL fCheck); // not implemented
public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
};


