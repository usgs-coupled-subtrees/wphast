#pragma once

// COMMENT: {5/26/2009 8:47:07 PM}#include "TreePropSheetEx/ResizablePage.h"
// COMMENT: {5/26/2009 8:47:07 PM}#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "PropPage.h"

#include "afxwin.h"
#include "resource.h"

// CPropNone dialog

class CPropNone : public CPropPage
{
	DECLARE_DYNAMIC(CPropNone)

public:
	CPropNone();
	virtual ~CPropNone();

// Dialog Data
	enum { IDD = IDD_PROP_NONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
// COMMENT: {5/26/2009 8:33:58 PM}	bool      bSkipUpdateData;

public:
	virtual BOOL OnInitDialog();
// COMMENT: {5/26/2009 8:34:09 PM}	virtual BOOL OnKillActive();
// COMMENT: {5/26/2009 8:34:16 PM}	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
// COMMENT: {5/26/2009 8:34:38 PM}	CComboBox ComboType;
// COMMENT: {5/26/2009 8:34:38 PM}	afx_msg void OnCbnSelchangeTypeCombo();
};
