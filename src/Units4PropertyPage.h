#pragma once
#include "afxwin.h"

#include "Units.h"

#define WM_SETPAGEFOCUS WM_APP+2

// CUnits4PropertyPage dialog

class CUnits4PropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CUnits4PropertyPage)

public:
	CUnits4PropertyPage();
	CUnits4PropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

	virtual ~CUnits4PropertyPage();

	void SetProperties(const CUnits& r_units);
	void GetProperties(CUnits& r_units)const;

	BOOL Contains(int nID)const;
	void SetControlFocus(int nID);

// Dialog Data
	enum { IDD = IDD_UNITS4_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnSetPageFocus(WPARAM, LPARAM);
	int m_idSetFocus;

public:
	CComboBox m_wndRiverBedKNumCombo;      // river_bed_hydraulic_conductivity [L/T]
	CComboBox m_wndRiverBedKDenomCombo;    // river_bed_hydraulic_conductivity [L/T]
	CComboBox m_wndRiverBedThickCombo;     // river_bed_thickness [L]
	CComboBox m_wndRiverWidthCombo;        // river_width [L]
	CComboBox m_wndRiverDepthCombo;        // river_depth [L]
	CComboBox m_wndDrainBedKNumCombo;      // drain_bed_k [L/T]
	CComboBox m_wndDrainBedKDenomCombo;    // drain_bed_k [L/T]
	CComboBox m_wndDrainBedThickCombo;     // drain_bed_thickness [L]
	CComboBox m_wndDrainWidthCombo;        // drain_width [L]

	CUnits m_units;
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
};
