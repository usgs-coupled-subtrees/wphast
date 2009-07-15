#pragma once

#include "PropPage.h"

#include "afxwin.h"
#include "resource.h"

#include "property.h"

#define __MULTIPLE__

#if defined(__MULTIPLE__)
#include "gridctrl/ModGridCtrlEx.h"
#endif

// CPropLinearM dialog

class CPropLinearM : public CPropPage
{
	DECLARE_DYNAMIC(CPropLinearM)

public:
	CPropLinearM();
	virtual ~CPropLinearM();

	Cproperty GetProperty();
	void SetProperty(Cproperty p);

// Dialog Data
	enum { IDD = IDD_PROP_LINEAR_M };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	Cproperty prop;

#if defined(__MULTIPLE__)
	CModGridCtrlEx   m_wndTimesGrid;      // IDC_TIMES_GRID
#endif
	afx_msg void OnEndLabelEditTimes(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedTimes(NMHDR *pNotifyStruct, LRESULT *result);

public:
	virtual BOOL OnInitDialog();
};
