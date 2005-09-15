#pragma once

#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"

#include "BC.h"

#ifndef baseCBCLeakyPropertyPage2
#define baseCBCLeakyPropertyPage2 ETSLayoutPropertyPageXP
#endif

// CBCLeakyPropertyPage2 dialog

class CBCLeakyPropertyPage2 : public baseCBCLeakyPropertyPage2
{
	DECLARE_DYNAMIC(CBCLeakyPropertyPage2)

public:
	CBCLeakyPropertyPage2();
	virtual ~CBCLeakyPropertyPage2();

	void SetProperties(const CBC& r_bc);
	void GetProperties(CBC& r_bc);

// Dialog Data
	enum { IDD = IDD_BC_LEAKY_PROPPAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL SetupGrids(void);

	CBC m_bc;
	CModGridCtrlEx   m_gridHead;     // IDC_HEAD_GRID
	CModGridCtrlEx   m_gridSolution; // IDC_SOLUTION_GRID
	CModGridCtrlEx   m_gridSingle;   // IDC_SINGLE_GRID

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCheckChangedHead(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnCheckChangedSolution(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnCheckChangedSingle(NMHDR *pNotifyStruct, LRESULT *result);
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
};
