#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define UM_CHECKSTATECHANGE (WM_USER + 100)
#define UM_DDX_FAILURE (WM_USER + 101)


// CTestPropertyPage dialog

class CTestPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTestPropertyPage)

public:
	CTestPropertyPage();
	virtual ~CTestPropertyPage();

// Dialog Data
	enum { IDD = IDD_TEST_PROPPAGE_LARGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// CListBox m_wndListBox;
	CCheckListBox m_wndListBox;
	CTreeCtrl m_wndTreeCtrl;
	CListCtrl m_wndListCtrl;
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnKeydownTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);

	double m_dValue1Active;
	double m_dValue2Active;

	double m_dValue1Kx;
	double m_dValue2Kx;

	double m_dValue1Porosity;
	double m_dValue2Porosity;

	HTREEITEM m_htiValidate;
};
