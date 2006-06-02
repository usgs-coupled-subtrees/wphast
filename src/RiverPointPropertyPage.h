#pragma once

#include "ChildPropertyPage.h"

#include "gridctrl/ModGridCtrlEx.h"
#include "River.h"
#include "Units.h"

// CRiverPointPropertyPage dialog

#ifndef baseCRiverPointPropertyPage
#define baseCRiverPointPropertyPage CPropertyPage
///#define baseCRiverPointPropertyPage CChildPropertyPage
#endif

class CRiverPointPropertyPage : public baseCRiverPointPropertyPage
{
	DECLARE_DYNAMIC(CRiverPointPropertyPage)

public:
	CRiverPointPropertyPage();
	virtual ~CRiverPointPropertyPage();

	void SetProperties(const CRiverPoint& r_riverPt);
	void GetProperties(CRiverPoint& r_riverPt)const;

	void SetUnits(const CUnits& units);

	void SetFirstOrLastPoint(bool bFirstOrLastPoint)      { m_bFirstOrLastPoint = bFirstOrLastPoint; }
	bool GetFirstOrLastPoint(bool bFirstOrLastPoint)const { return m_bFirstOrLastPoint; };

// Dialog Data
	///enum { IDD = IDD_RIVER_PROPPAGE };
	enum { IDD = IDD_RIVERPT_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CRiverPoint      m_riverPt;
	bool             m_bFirstOrLastPoint;
	CUnits           m_units;
	CModGridCtrlEx   m_wndScheduleGrid;      // IDC_GRID_SCHEDULES

	CString          m_strHorizontalUnits;
	CString          m_strVerticalUnits;
	CString          m_strHeadUnits;
	CString          m_strRiverBedKUnits;
	CString          m_strRiverBedThicknessUnits;
};
