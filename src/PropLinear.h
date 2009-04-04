#pragma once

#include "TreePropSheetEx/ResizablePage.h"
#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "afxwin.h"
#include "resource.h"

#include "property.h"

// CPropLinear dialog

class CPropLinear : public CResizablePage, public TreePropSheet::CWhiteBackgroundProvider
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
	bool      bSkipUpdateData;

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CComboBox ComboType;
	afx_msg void OnCbnSelchangeTypeCombo();
};
