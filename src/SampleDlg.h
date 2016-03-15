#pragma once


// CSampleDlg dialog

class CSampleDlg : public CDialog
{
	DECLARE_DYNAMIC(CSampleDlg)

public:
	CSampleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSampleDlg();

// Dialog Data
	enum { IDD = IDD_PL_SAMPLE };

	int thin_grid[3];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckZ2();
	afx_msg void OnBnClickedCheckX2();
	afx_msg void OnBnClickedCheckY2();
};
