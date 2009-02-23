#pragma once
#include "afxwin.h"

#include "Units.h"

#define WM_SETPAGEFOCUS WM_APP+2

// CUnits3PropertyPage dialog

class CUnits3PropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CUnits3PropertyPage)

public:
	CUnits3PropertyPage();
	CUnits3PropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

	virtual ~CUnits3PropertyPage();

	void SetProperties(const CUnits& r_units);
	void GetProperties(CUnits& r_units)const;

	BOOL Contains(int nID)const;
	void SetControlFocus(int nID);

// Dialog Data
	enum { IDD = IDD_UNITS3_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnSetPageFocus(WPARAM, LPARAM);
	int m_idSetFocus;

public:
	CComboBox m_wndFluxNumCombo;           // flux  [L/T]
	CComboBox m_wndFluxDenomCombo;         // flux  [L/T]
	CComboBox m_wndLeakyKNumCombo;         // leaky_hydraulic_conductivity [L/T]
	CComboBox m_wndLeakyKDenomCombo;       // leaky_hydraulic_conductivity [L/T]
	CComboBox m_wndLeakyThickCombo;        // leaky_thickness [L]
	CComboBox m_wndWellDiamCombo;          // well_diameter [L]
	CComboBox m_wndWellPumpageNumCombo;    // well_flow_rate [L^3/T]
	CComboBox m_wndWellPumpageDenomCombo;  // well_flow_rate [L^3/T]
	CComboBox m_wndWellDepthCombo;         // well_depth [L]

	CUnits m_units;
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
};
