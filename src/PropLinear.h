#pragma once

// COMMENT: {5/26/2009 9:33:09 PM}#include "TreePropSheetEx/ResizablePage.h"
// COMMENT: {5/26/2009 9:33:09 PM}#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "PropPage.h"

#include "afxwin.h"
#include "resource.h"

#include "property.h"

// CPropLinear dialog

class CPropLinear : public CPropPage
{
	DECLARE_DYNAMIC(CPropLinear)

public:
	CPropLinear();
	virtual ~CPropLinear();

	Cproperty GetProperty();
	void SetProperty(Cproperty p);

// Dialog Data
	enum { IDD = IDD_PROP_LINEAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	Cproperty prop;
// COMMENT: {5/26/2009 9:33:41 PM}	bool      bSkipUpdateData;

public:
	virtual BOOL OnInitDialog();
// COMMENT: {5/26/2009 9:33:55 PM}	virtual BOOL OnKillActive();
// COMMENT: {5/26/2009 9:33:55 PM}	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
// COMMENT: {5/26/2009 9:33:55 PM}	CComboBox ComboType;
// COMMENT: {5/26/2009 9:33:55 PM}	afx_msg void OnCbnSelchangeTypeCombo();
};
