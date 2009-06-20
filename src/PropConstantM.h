#pragma once

#include "PropPage.h"

#include "afxwin.h"
#include "resource.h"
#include "property.h"

#define __MULTIPLE__

#if defined(__MULTIPLE__)
#include "gridctrl/ModGridCtrlEx.h"
#endif



// CPropConstantM dialog

class CPropConstantM : public CPropPage
{
	DECLARE_DYNAMIC(CPropConstantM)

public:
	CPropConstantM();
	virtual ~CPropConstantM();

	Cproperty GetProperty();
	void SetProperty(Cproperty p);

// Dialog Data
#if defined(__MULTIPLE__)
	enum { IDD = IDD_PROP_CONSTANT_M };
#else
	enum { IDD = IDD_PROP_CONSTANT };
#endif

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
