#pragma once
#include "afxwin.h"


// CRunTypeDlg dialog

#include "ETSLayout/ETSLayout.h"

#ifndef baseCRunTypeDlg
#define baseCRunTypeDlg ETSLayoutDialog
#endif

class CRunTypeDlg : public baseCRunTypeDlg
{
	DECLARE_DYNAMIC(CRunTypeDlg)

public:
	CRunTypeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRunTypeDlg();

// Dialog Data
	enum { IDD = IDD_RUNTYPEDLG };

	CString strDatabase;
	BOOL bParallel;
	CString strCommand;
	CString strCommandArgs;
	CRecentFileList* pRecentDBFileList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedRadio();
	virtual BOOL OnInitDialog();
	CComboBox cboDB;
	afx_msg void OnBnClickedDbBrowse();
};
