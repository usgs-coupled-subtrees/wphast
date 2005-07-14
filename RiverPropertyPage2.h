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

public:
	afx_msg void OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedRadioDepth();
	afx_msg void OnBnClickedRadioBottom();
};
