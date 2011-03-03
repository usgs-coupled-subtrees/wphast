#pragma once

#include "Units.h"

// CUnits5PropertyPage dialog
#define WM_SETPAGEFOCUS WM_APP+2

class CUnits5PropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CUnits5PropertyPage)

public:
	CUnits5PropertyPage();
	CUnits5PropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

	virtual ~CUnits5PropertyPage();

	void SetProperties(const CUnits& r_units);
	void GetProperties(CUnits& r_units)const;

	BOOL Contains(int nID)const;
	void SetControlFocus(int nID);

// Dialog Data
	enum { IDD = IDD_UNITS5_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnSetPageFocus(WPARAM, LPARAM);
	int m_idSetFocus;

public:
	CComboBox ppassemblageCombo;
	CComboBox exchangeCombo;
	CComboBox surfaceCombo;
	CComboBox ssassemblageCombo;
	CComboBox kineticsCombo;
	CComboBox gasphaseCombo;

	CUnits m_units;
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
};
