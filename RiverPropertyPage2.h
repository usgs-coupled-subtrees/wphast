#pragma once

#include <set>
#include "gridctrl/ModGridCtrlEx.h"
#include "River.h"
#include "Units.h"
#include "Tree.h"

// CRiverPropertyPage2 dialog

class CRiverPropertyPage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CRiverPropertyPage2)

public:
	CRiverPropertyPage2();
	virtual ~CRiverPropertyPage2();

	void SetProperties(const CRiver& river);
	void GetProperties(CRiver& river)const;

	void SetUnits(const CUnits& units);
	void SetPoint(int index);

	void SetFlowOnly(bool bFlowOnly)     { m_bFlowOnly = bFlowOnly; }
	bool GetFlowOnly(void)const          { return m_bFlowOnly; }

	void SetUsedRiverNumbers(const std::set<int>& used);

// Dialog Data
	enum { IDD = IDD_RIVER_PROPPAGE4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CRiver           m_river;
	CUnits           m_units;

	CTreeCtrlEx      m_wndTreeCtrl;
	CModGridCtrlEx   m_wndScheduleGrid;      // IDC_GRID_SCHEDULES
	CRichEditCtrl    m_wndRichEditCtrl;

	CString          m_strHorizontalUnits;
	CString          m_strVerticalUnits;
	CString          m_strHeadUnits;
	CString          m_strRiverBedKUnits;
	CString          m_strRiverBedThicknessUnits;

	bool             m_bFullVerify;
	UINT             m_pointIndex;
	bool             m_bFlowOnly;
	std::set<int>    m_usedRiverNumbers;

	std::map<UINT, CCellID > m_mapCurrentFocusCell;

	std::string m_sNumRTF;      // IDR_RIVER_NUM_RTF
	std::string m_sDescRTF;     // IDR_RIVER_DESC_RTF
	std::string m_sLocRTF;      // IDR_RIVER_LOC_RTF
	std::string m_sWidthRTF;    // IDR_RIVER_WIDTH_RTF
	std::string m_sHydCondRTF;  // IDR_RIVER_HYD_COND_RTF
	std::string m_sThickRTF;    // IDR_RIVER_THICK_RTF
	std::string m_sDepthRTF;    // IDR_RIVER_DEPTH_RTF
	std::string m_sBottomRTF;   // IDR_RIVER_BOTTOM_RTF
	std::string m_sStartRTF;    // IDR_RIVER_START_RTF
	std::string m_sUnitsRTF;    // IDR_RIVER_UNITS_RTF
	std::string m_sHeadRTF;     // IDR_RIVER_HEAD_RTF
	std::string m_sSolutionRTF; // IDR_RIVER_SOLUTION_RTF

public:
	afx_msg void OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);

	afx_msg void OnSelChangedSchedule(NMHDR *pNotifyStruct, LRESULT *result);

	afx_msg void OnBnClickedRadioDepth();
	afx_msg void OnBnClickedRadioBottom();
	afx_msg void OnEnSetfocusNuserEdit();
	afx_msg void OnEnSetfocusDescEdit();
	afx_msg void OnEnSetfocusXEdit();
	afx_msg void OnEnSetfocusYEdit();
	afx_msg void OnEnSetfocusWidthEdit();
	afx_msg void OnEnSetfocusRiverKEdit();
	afx_msg void OnEnSetfocusRiverThickEdit();
	afx_msg void OnBnSetfocusRadioDepth();
	afx_msg void OnBnSetfocusRadioBottom();
	afx_msg void OnEnSetfocusDepthEdit();
	afx_msg void OnEnSetfocusBottomEdit();
};
