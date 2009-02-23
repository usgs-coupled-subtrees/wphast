#pragma once

#include "Units.h"

// CUnits1PropertyPage dialog
#define WM_SETPAGEFOCUS WM_APP+2

class CUnits1PropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CUnits1PropertyPage)

public:
	CUnits1PropertyPage();
	CUnits1PropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

	virtual ~CUnits1PropertyPage();

	void SetProperties(const CUnits& r_units);
	void GetProperties(CUnits& r_units)const;

	BOOL Contains(int nID)const;
	void SetControlFocus(int nID);

// Dialog Data
	enum { IDD = IDD_UNITS1_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnSetPageFocus(WPARAM, LPARAM);
	int m_idSetFocus;

public:
	CComboBox m_wndTimeCombo;
	CComboBox m_wndHorizontalCombo;
	CComboBox m_wndVerticalCombo;
	//{{
	CComboBox m_wndHorizontalMapCombo;
	CComboBox m_wndVerticalMapCombo;
	//}}
	CComboBox m_wndHeadCombo;
// COMMENT: {11/25/2008 2:27:35 PM}	CComboBox m_wndKNumCombo;
// COMMENT: {11/25/2008 2:27:35 PM}	CComboBox m_wndKDenomCombo;
// COMMENT: {11/25/2008 2:27:35 PM}	CComboBox m_wndSDenomCombo;
// COMMENT: {11/25/2008 2:27:35 PM}	CComboBox m_wndAlphaCombo;
	CUnits m_units;
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
};
