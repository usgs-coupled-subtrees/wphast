#pragma once
#include "afxcmn.h"

#include <map>
#include "afxwin.h"

// CFileRecreateDialog dialog

class CFileRecreateDialog : public CDialog
{
	DECLARE_DYNAMIC(CFileRecreateDialog)

public:
	CFileRecreateDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFileRecreateDialog();

	void SetOriginal2New(const std::map< CString, CString >& map);

// Dialog Data
	enum { IDD = IDD_FILE_RECREATE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CListCtrl                    ListCtrl;
	CRect                        MinRect;
	std::map< CString, CString > Map;
	LONG                         HGap;
	LONG                         VGap;

public:
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL OnInitDialog();
	CButton OkButton;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
