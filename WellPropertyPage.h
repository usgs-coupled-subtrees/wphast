#pragma once
#include <set>

#include "gridctrl/ModGridCtrlEx.h"
#include "Units.h"
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

	BOOL SetGrid(const struct grid &g);

	void SetFlowOnly(bool bFlowOnly)     { m_bFlowOnly = bFlowOnly; }
	bool GetFlowOnly(void)const          { return m_bFlowOnly; }

// Dialog Data
	enum { IDD = IDD_WELL_PROPPAGE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void FillUnits(void);
	void SetScreenHeadings(BOOL bByDepth);
	void SetPumpSchedHeadings(void);
	void EnableByDepth(BOOL bByDepth);
	void UpdateScreens(BOOL bByDepth);

	CUnits           m_units;
	CWellSchedule    m_well;
	CString          m_strHorizontalUnits;
	CString          m_strVerticalUnits;
	CString          m_strWellDiameterUnits;
	CString          m_strTimeUnits;
	CString          m_strWellPumpageUnits;
	CGrid            m_grid;
	CModGridCtrlEx   m_wndScreensGrid;      // IDC_GRID_SCREENS
	CModGridCtrlEx   m_wndPumpSchedGrid;    // IDC_GRID_SCHEDULES
	std::set<int>    m_usedWellNumbers;

	std::string      m_sWellTimeRTF;        // IDR_WELL_TIME_RTF
	std::string      m_sWellUnitsRTF;       // IDR_WELL_UNITS_RTF
	std::string      m_sWellRateRTF;        // IDR_WELL_RATE_RTF
	std::string      m_sWellSolutionRTF;    // IDR_WELL_SOLUTION_RTF

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
};

