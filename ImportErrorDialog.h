#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CImportErrorDialog dialog

class CImportErrorDialog : public CDialog
{
	DECLARE_DYNAMIC(CImportErrorDialog)

public:
	CImportErrorDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportErrorDialog();

// Dialog Data
	enum { IDD = IDD_IMPORT_ERROR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_wndStaticIcon;
	CRichEditCtrl m_wndErrorMessages;
	LPCTSTR m_lpszErrorMessages;
	virtual INT_PTR DoModal();
};
