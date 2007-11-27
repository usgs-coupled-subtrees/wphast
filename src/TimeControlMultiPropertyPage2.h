#pragma once

#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"

#include "TimeControl2.h"

#ifndef baseCTimeControlMultiPropertyPage2
#define baseCTimeControlMultiPropertyPage2 ETSLayoutPropertyPageXP
#endif

class CUnits;

// CTimeControlMultiPropertyPage2 dialog

class CTimeControlMultiPropertyPage2 : public baseCTimeControlMultiPropertyPage2
{
	DECLARE_DYNAMIC(CTimeControlMultiPropertyPage2)

public:
	CTimeControlMultiPropertyPage2();
	virtual ~CTimeControlMultiPropertyPage2();

	void SetProperties(const CTimeControl2& r_tc2);
	void GetProperties(CTimeControl2& r_tc2);

	void SetUnits(const CUnits& units);

// Dialog Data
	enum { IDD = IDD_TC_INT_M_PROPPAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL SetupGrids(void);

	CModGridCtrlEx   m_gridTimeEnd;     // IDC_TIMEEND_GRID
	CModGridCtrlEx   m_gridTimeStep;    // IDC_TIMESTEP_GRID
	CComboBox        m_cboStartTime;    // IDC_STARTTIME_COMBO

	CRichEditCtrl m_wndRichEditCtrl;
	CTimeControl2 m_tc2;
	CString       m_strTimeUnits;		// Not currently used

	std::string m_sTimeEndRTF;         // IDR_TC_TE_TIME_END_RTF
	std::string m_sTimeEndUnitsRTF;    // IDR_TC_TE_TIME_END_UNITS_RTF

	std::string m_sTSTimeRTF;          // IDR_TC_TS_TIME_RTF
	std::string m_sTSUnitsRTF;         // IDR_TC_TS_UNITS_RTF
	std::string m_sTSTimeStepRTF;      // IDR_TC_TS_TIME_STEP_RTF
	std::string m_sTSTimeStepUnitsRTF; // IDR_TC_TS_TIME_STEP_UNITS_RTF

	afx_msg void OnSelChangedTimeEnd(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedTimeStep(NMHDR *pNotifyStruct, LRESULT *result);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
