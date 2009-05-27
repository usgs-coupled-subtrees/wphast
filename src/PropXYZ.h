#pragma once

// COMMENT: {5/26/2009 9:18:50 PM}#include "TreePropSheetEx/ResizablePage.h"
// COMMENT: {5/26/2009 9:18:50 PM}#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "PropPage.h"

#include "afxwin.h"
#include "resource.h"
#include "property.h"


// CPropXYZ dialog

class CPropXYZ : public CPropPage
{
	DECLARE_DYNAMIC(CPropXYZ)

public:
	CPropXYZ();
	virtual ~CPropXYZ();

	Cproperty GetProperty();
	void SetProperty(Cproperty p);

// Dialog Data
	enum { IDD = IDD_PROP_XYZ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
// COMMENT: {5/26/2009 9:19:37 PM}	bool      bSkipUpdateData;

	Cproperty prop;

public:
	virtual BOOL OnInitDialog();
// COMMENT: {5/26/2009 9:19:44 PM}	virtual BOOL OnKillActive();
// COMMENT: {5/26/2009 9:19:44 PM}	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
// COMMENT: {5/26/2009 9:19:44 PM}	CComboBox ComboType;
// COMMENT: {5/26/2009 9:19:44 PM}	afx_msg void OnCbnSelchangeTypeCombo();
	afx_msg void OnBnClickedButtonXyz();
};
