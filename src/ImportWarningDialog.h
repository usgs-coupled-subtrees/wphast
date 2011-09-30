#pragma once


// CImportWarningDialog dialog

class CImportWarningDialog : public CDialog
{
	DECLARE_DYNAMIC(CImportWarningDialog)

public:
	CImportWarningDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportWarningDialog();

// Dialog Data
	enum { IDD = IDD_IMPORT_WARNING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoModal();
	CString WarningMessages;

protected:
	CStatic StaticIcon;
	CRichEditCtrl RichEditCtrl;
};
