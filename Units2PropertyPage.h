#pragma once
#include "afxwin.h"

#include "Units.h"

#define WM_SETPAGEFOCUS WM_APP+2

// CUnits2PropertyPage dialog

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
	CComboBox m_wndFluxNumCombo;           // flux  [L/T]
	CComboBox m_wndFluxDenomCombo;         // flux  [L/T]
	CComboBox m_wndLeakyKNumCombo;         // leaky_hydraulic_conductivity [L/T]
	CComboBox m_wndLeakyKDenomCombo;       // leaky_hydraulic_conductivity [L/T]
	CComboBox m_wndLeakyThickCombo;        // leaky_thickness [L]
	CComboBox m_wndWellDiamCombo;          // well_diameter [L]
	CComboBox m_wndWellPumpageNumCombo;    // well_flow_rate [L^3/T]
	CComboBox m_wndWellPumpageDenomCombo;  // well_flow_rate [L^3/T]
	CComboBox m_wndRiverBedKNumCombo;      // river_bed_hydraulic_conductivity [L/T]
	CComboBox m_wndRiverBedKDenomCombo;    // river_bed_hydraulic_conductivity [L/T]
	CComboBox m_wndRiverBedThickCombo;     // river_bed_thickness [L]
	CUnits m_units;
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
};
