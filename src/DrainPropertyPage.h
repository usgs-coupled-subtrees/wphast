#pragma once

#include <set>
#include "gridctrl/ModGridCtrlEx.h"
#include "River.h"
#include "Drain.h"
#include "Units.h"
#include "Tree.h"
#include "GridKeyword.h"

// CDrainPropertyPage dialog

class CDrainPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CDrainPropertyPage)

public:
	CDrainPropertyPage();
	virtual ~CDrainPropertyPage();

	void SetProperties(const CDrain& drain);
	void GetProperties(CDrain& drain)const;

	void SetUnits(const CUnits& units);
	void SetGridKeyword(const CGridKeyword& gridKeyword);
	void SetPoint(int index);

	void SetFlowOnly(bool bFlowOnly)     { m_bFlowOnly = bFlowOnly; }
	bool GetFlowOnly(void)const          { return m_bFlowOnly; }

	void SetUsedDrainNumbers(const std::set<int>& used); // was SetUsedRiverNumbers

// Dialog Data
	enum { IDD = IDD_DRAIN_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CDrain           Drain;
	BOOL             m_bUpdatePositionOnly;
	CUnits           m_units;
	CGridKeyword     m_gridKeyword;

	CTreeCtrlEx      m_wndTreeCtrl;
	CRichEditCtrl    m_wndRichEditCtrl;

	CString          m_strHorizontalUnits;
	CString          m_strVerticalUnits;
	CString          m_strMapHorizontalUnits;
	CString          m_strMapVerticalUnits;
	CString          m_strHeadUnits;
	CString          m_strDrainKUnits;
	CString          m_strDrainThicknessUnits;

	bool             m_bFullVerify;
	UINT             m_pointIndex;
	bool             m_bFlowOnly;
	std::set<int>    m_usedDrainNumbers; // was m_usedRiverNumbers

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

// COMMENT: {10/15/2008 6:19:02 PM}	afx_msg void OnSelChangedSchedule(NMHDR *pNotifyStruct, LRESULT *result);

// COMMENT: {10/20/2008 5:32:35 PM}	afx_msg void OnBnClickedRadioDepth();
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
	afx_msg void OnBnClickedUseMap();
// COMMENT: {2/9/2009 8:32:16 PM}	afx_msg void OnBnClickedUseMapZ();
};
