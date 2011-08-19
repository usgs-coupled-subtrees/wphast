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

	std::string m_sNumRTF;      // IDR_DRAIN_NUM_RTF         2
	std::string m_sDescRTF;     // IDR_DRAIN_DESC_RTF        3
	std::string m_sLocRTF;      // IDR_DRAIN_LOC_RTF         4
	std::string m_sUseMapXYRTF; // IDR_DRAIN_USE_MAP_XY_RTF  5
	std::string m_sWidthRTF;    // IDR_DRAIN_WIDTH_RTF       6
	std::string m_sHydCondRTF;  // IDR_DRAIN_HYD_COND_RTF    7
	std::string m_sThickRTF;    // IDR_DRAIN_THICK_RTF       8
	std::string m_sZRTF;        // IDR_DRAIN_Z_RTF           9
	std::string m_sUseMapZRTF;  // IDR_DRAIN_USE_MAP_Z_RTF  10

public:
	afx_msg void OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedRadioBottom();
	afx_msg void OnEnSetfocusNuserEdit();
	afx_msg void OnEnSetfocusDescEdit();
	afx_msg void OnEnSetfocusXEdit();
	afx_msg void OnEnSetfocusYEdit();
	afx_msg void OnEnSetfocusWidthEdit();
	afx_msg void OnEnSetfocusRiverKEdit();
	afx_msg void OnEnSetfocusRiverThickEdit();
	afx_msg void OnBnClickedUseMap();
	afx_msg void OnEnSetfocusDrainZEdit();
	afx_msg void OnBnSetfocusCheckUseMap();
};
