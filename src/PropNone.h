#pragma once

#include "TreePropSheetEx/ResizablePage.h"
#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "afxwin.h"
#include "resource.h"

// CPropNone dialog

class CPropNone : public CResizablePage, public TreePropSheet::CWhiteBackgroundProvider
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

public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CComboBox ComboType;
	afx_msg void OnCbnSelchangeTypeCombo();
};
