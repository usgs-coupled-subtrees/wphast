#pragma once

#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"


#ifndef baseCBCSpecifiedHeadPropertyPage
#define baseCBCSpecifiedHeadPropertyPage ETSLayoutPropertyPageXP
#endif

// CBCSpecifiedHeadPropertyPage dialog
class CBCSpecifiedHeadPropertyPage : public baseCBCSpecifiedHeadPropertyPage
{
	DECLARE_DYNAMIC(CBCSpecifiedHeadPropertyPage)

public:
	CBCSpecifiedHeadPropertyPage();
	virtual ~CBCSpecifiedHeadPropertyPage();

// Dialog Data
	enum { IDD = IDD_BC_SPECIFIED_HEAD_PROPPAGE };
	DECLARE_LAYOUT();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CModGridCtrlEx   m_gridHead;     // IDC_HEAD_GRID
	CModGridCtrlEx   m_gridSolution; // IDC_SOLUTION_GRID

	DECLARE_MESSAGE_MAP()
	afx_msg void OnItemChangedHead(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnItemChangedSolution(NMHDR *pNotifyStruct, LRESULT *result);

public:
	virtual BOOL OnInitDialog();
	//CString m_editEnabled;
	//CString m_editDisabled;
};
