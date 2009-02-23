#pragma once

#include "Units.h"

// CUnits2PropertyPage dialog
#define WM_SETPAGEFOCUS WM_APP+2

class CUnits2PropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CUnits2PropertyPage)

public:
	CUnits2PropertyPage();
	CUnits2PropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

	virtual ~CUnits2PropertyPage();

	void SetProperties(const CUnits& r_units);
	void GetProperties(CUnits& r_units)const;

	BOOL Contains(int nID)const;
	void SetControlFocus(int nID);

// Dialog Data
	enum { IDD = IDD_UNITS2_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnSetPageFocus(WPARAM, LPARAM);
	int m_idSetFocus;

public:
// COMMENT: {11/25/2008 3:04:58 PM}	CComboBox m_wndTimeCombo;
// COMMENT: {11/25/2008 3:04:58 PM}	CComboBox m_wndHorizontalCombo;
// COMMENT: {11/25/2008 3:04:58 PM}	CComboBox m_wndVerticalCombo;
// COMMENT: {11/25/2008 3:04:58 PM}	CComboBox m_wndHeadCombo;
	CComboBox m_wndKNumCombo;
	CComboBox m_wndKDenomCombo;
	CComboBox m_wndSDenomCombo;
	CComboBox m_wndAlphaCombo;
	CUnits m_units;
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
};
