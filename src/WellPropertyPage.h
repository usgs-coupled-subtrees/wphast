#pragma once
#include <set>

#include "gridctrl/ModGridCtrlEx.h"
#include "Units.h"
#include "GridKeyword.h"
#include "Well.h"
#include "WellSchedule.h"
#include "wellctrl.h"


// CWellPropertyPage dialog

class CWellPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CWellPropertyPage)

public:
	CWellPropertyPage();
	virtual ~CWellPropertyPage();


	void SetUnits(const CUnits& units);

	void SetProperties(const CWellSchedule& well);
	void GetProperties(CWellSchedule& well)const;

	void SetUsedWellNumbers(const std::set<int>& used);

	void SetGridKeyword(const CGridKeyword& gridKeyword);

	void SetFlowOnly(bool bFlowOnly)     { m_bFlowOnly = bFlowOnly; }
	bool GetFlowOnly(void)const          { return m_bFlowOnly; }

// Dialog Data
	enum { IDD = IDD_WELL_PROPPAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void FillUnits(void);
	void SetScreenHeadings(BOOL bByDepth);
	void SetPumpSchedHeadings(void);
	void EnableByDepth(BOOL bByDepth);
	void UpdateScreens(BOOL bByDepth);

	BOOL             m_bUpdatePositionOnly;
	BOOL             m_bUpdateZOnly;
	CUnits           m_units;
	CWellSchedule    m_well;
	CString          m_strHorizontalUnits;
	CString          m_strVerticalUnits;
	CString          m_strMapHorizontalUnits;
	CString          m_strMapVerticalUnits;
	CString          m_strWellDiameterUnits;
	CString          m_strTimeUnits;
	CString          m_strWellPumpageUnits;
	CGridKeyword     m_gridKeyword;
	CModGridCtrlEx   m_wndScreensGrid;      // IDC_GRID_SCREENS
	CModGridCtrlEx   m_wndPumpSchedGrid;    // IDC_GRID_SCHEDULES
	std::set<int>    m_usedWellNumbers;

	std::string      m_sWellTimeRTF;        // IDR_WELL_TIME_RTF
	std::string      m_sWellUnitsRTF;       // IDR_WELL_UNITS_RTF
	std::string      m_sWellRateRTF;        // IDR_WELL_RATE_RTF
	std::string      m_sWellSolutionRTF;    // IDR_WELL_SOLUTION_RTF
	std::string      m_sWellAllocateRTF;    // IDR_WELL_ALLOCATE_RTF
	std::string      m_sWellDepthRTF;       // IDR_WELL_DEPTH_RTF
	std::string      m_sWellDescRTF;        // IDR_WELL_DESC_RTF
	std::string      m_sWellDiamRTF;        // IDR_WELL_DIAM_RTF
	std::string      m_sWellElevationRTF;   // IDR_WELL_ELEVATION_RTF
	std::string      m_sWellLocationRTF;    // IDR_WELL_LOCATION_RTF
	std::string      m_sWellLSDRTF;         // IDR_WELL_LSD_RTF
	std::string      m_sWellNumberRTF;      // IDR_WELL_NUMBER_RTF
	std::string      m_sWellRadiusRTF;      // IDR_WELL_RADIUS_RTF
	std::string      m_sWellUseMapXYRTF;    // IDR_WELL_USE_MAP_XY_RTF
	std::string      m_sWellUseMapZRTF;     // IDR_WELL_USE_MAP_Z_RTF

	bool             m_bFlowOnly;

	CRichEditCtrl m_wndRichEditCtrl;

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bByDepth;
	BOOL m_bConvertBetweenDepthsAndElevation;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedElevationRadio();
	afx_msg void OnBnClickedDepthRadio();

	afx_msg void OnEndLabelEditScreen(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedScreen(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedSchedules(NMHDR *pNotifyStruct, LRESULT *result);	

	CWellCtrl m_wndWellCtrl;
	afx_msg void OnEnChangeLsdEdit();
	BOOL m_allocate;
	afx_msg void OnEnSetfocusNuserEdit();
	afx_msg void OnEnSetfocusDescEdit();
	afx_msg void OnEnSetfocusXEdit();
	afx_msg void OnEnSetfocusYEdit();
	afx_msg void OnBnClickedDiamRadio();
	afx_msg void OnBnClickedRadiusRadio();
	afx_msg void OnEnSetfocusDiamEdit();
	afx_msg void OnBnSetfocusAllocateByHead();
	afx_msg void OnEnSetfocusLsdEdit();
	afx_msg void OnBnSetfocusElevationRadio();
	afx_msg void OnBnSetfocusDepthRadio();
	afx_msg void OnBnSetfocusDiamRadio();
	afx_msg void OnBnSetfocusRadiusRadio();
	afx_msg void OnBnClickedUseMap();
	afx_msg void OnBnClickedUseMapZ();
	afx_msg void OnBnSetfocusCheckUseMap();
	afx_msg void OnBnSetfocusCheckUseMapZ();
};

