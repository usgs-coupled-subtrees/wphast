#pragma once
#include "gridctrl.h"
#include "../resource.h" // IDC_RESIZE_COLUMN, IDC_RESIZE_ROW

#define GVIS_DISABLED             0x0200

#define GV_HIGHLIGHT_NEVER        0x0000
#define GV_HIGHLIGHT_ALWAYS       0x0001
#define GV_HIGHLIGHT_WITH_FOCUS   0x0002

void AFXAPI DDX_TextGridControl(CDataExchange* pDX, int nIDC, int nRow, int nCol, double& value);
void AFXAPI DDX_TextGridControl(CDataExchange* pDX, int nIDC, int nRow, int nCol, CString& value);
void AFXAPI DDX_TextGridControl(CDataExchange* pDX, int nIDC, int nRow, int nCol, int& value);

void AFXAPI DDX_GridControlFail(CDataExchange* pDX, int nIDC, int nRow, int nCol, LPCTSTR lpszText);
void AFXAPI DDX_GridControlFail(CDataExchange* pDX, int nIDC, int nRow, int nCol, UINT nIDText);


class CModGridCtrl :
	public CGridCtrl
{
public:
	CModGridCtrl(int nRows = 0, int nCols = 0, int nFixedRows = 0, int nFixedCols = 0);
	virtual ~CModGridCtrl(void);

	void SetCurrentFocusCell(int nRow, int nCol);

	BOOL DisableCell(int nRow, int nCol);
	BOOL DisableCell(const CCellID& cell);
	BOOL EnableCell(int nRow, int nCol);
	BOOL EnableCell(const CCellID& cell);
	bool IsCellEnabled(int nRow, int nCol)const;
	bool IsCellEnabled(const CCellID& cell)const;
	void SetHighLight(long nNewValue);
	long GetHighLight(void)const;

protected:
	// Virtual overrides
    virtual BOOL  DrawFixedCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk=FALSE);
    virtual BOOL  DrawCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk=FALSE);

#ifndef GRIDCONTROL_NO_CLIPBOARD
    virtual void CutSelectedText();
#endif

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult);
	long m_nHighLight;
public:
//	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};
