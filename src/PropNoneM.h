#pragma once

#include "PropPage.h"

#include "afxwin.h"
#include "resource.h"

#define __MULTIPLE__

#if defined(__MULTIPLE__)
#include "gridctrl/ModGridCtrlEx.h"
#endif


// CPropNoneM dialog

class CPropNoneM : public CPropPage
{
	DECLARE_DYNAMIC(CPropNoneM)

public:
	CPropNoneM();
	virtual ~CPropNoneM();

// Dialog Data
	enum { IDD = IDD_PROP_NONE_M };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

#if defined(__MULTIPLE__)
	CModGridCtrlEx   m_wndTimesGrid;      // IDC_TIMES_GRID
#endif
	afx_msg void OnEndLabelEditTimes(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedTimes(NMHDR *pNotifyStruct, LRESULT *result);

public:
	virtual BOOL OnInitDialog();
};
