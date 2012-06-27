#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#define WM_RUN_FINISHED WM_APP+3

// CRunDialog dialog

class CRunDialog : public CDialog
{
	DECLARE_DYNAMIC(CRunDialog)

public:
	CRunDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRunDialog();

// Dialog Data
	enum { IDD = IDD_RUN_OUTPUT_DIALOG };

public:
	BOOL    bParallel;
	CString strCommand;
	CString strCommandArgs;

protected:
	TCHAR m_szOriginalDir[_MAX_PATH];

	HANDLE m_hChildProcess;
	HANDLE m_hStdIn; // Handle to parents std input.
	BOOL m_bRunThread;
	HANDLE eventTerminateProcessGroup;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void PrepAndLaunchRedirectedChild(HANDLE hChildStdOut, HANDLE hChildStdIn, HANDLE hChildStdErr);
	void ReadAndHandleOutput(HANDLE hPipeRead);
	static UINT RunThreadProc(LPVOID pParam);

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnRunFinished(WPARAM, LPARAM);

public:
	void SetWorkingDirectory(LPCTSTR pstrDirName);
	virtual INT_PTR DoModal();
	CRichEditCtrl m_wndOutputRichEditCtrl;
	virtual BOOL OnInitDialog();
	CButton m_wndOKButton;
	afx_msg void OnBnClickedAbort();
	CButton m_wndAbortButton;
};
