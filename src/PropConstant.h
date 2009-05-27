#pragma once

// COMMENT: {5/26/2009 8:47:22 PM}#include "TreePropSheetEx/ResizablePage.h"
// COMMENT: {5/26/2009 8:47:22 PM}#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "PropPage.h"

#include "afxwin.h"
#include "resource.h"
#include "property.h"

// COMMENT: {5/13/2009 6:43:33 PM}#define __MULTIPLE__

#if defined(__MULTIPLE__)
#include "gridctrl/ModGridCtrlEx.h"
#endif



// CPropConstant dialog

class CPropConstant : public CPropPage
{
	DECLARE_DYNAMIC(CPropConstant)

public:
	CPropConstant();
	virtual ~CPropConstant();

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
// COMMENT: {5/26/2009 8:48:06 PM}	bool      bSkipUpdateData;
	Cproperty prop;

#if defined(__MULTIPLE__)
	CModGridCtrlEx   m_wndTimesGrid;      // IDC_TIMES_GRID
#endif

public:
	virtual BOOL OnInitDialog();
// COMMENT: {5/26/2009 8:54:48 PM}	virtual BOOL OnSetActive();
// COMMENT: {5/26/2009 8:48:27 PM}	virtual BOOL OnKillActive();
// COMMENT: {5/26/2009 8:48:27 PM}	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
// COMMENT: {5/26/2009 8:48:17 PM}	CComboBox ComboType;
// COMMENT: {5/26/2009 8:48:17 PM}	afx_msg void OnCbnSelchangeTypeCombo();
};
