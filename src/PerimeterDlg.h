#pragma once
#include "afxwin.h"


// CPerimeterDlg dialog

class CPerimeterDlg : public CDialog
{
	DECLARE_DYNAMIC(CPerimeterDlg)

public:
	CPerimeterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPerimeterDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PERIMETER };

	typedef enum tagPERIMETER_TYPE {
		PT_DRAW,
		PT_SHAPEFILE,
	} PERIMETER_TYPE;
	
	PERIMETER_TYPE type;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioPerimDraw();
	afx_msg void OnBnClickedRadioPerimShapefile();
	CEdit m_wndEditShapefile;
	afx_msg void OnBnClickedButtonShape();
	CButton m_wndButtonBrowse;
	CString m_strShapefile;
};
