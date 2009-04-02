#pragma once

#include "TreePropSheetEx/ResizablePage.h"
#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "afxwin.h"
#include "resource.h"

// CPropConstant dialog

class CPropConstant : public CResizablePage , public TreePropSheet::CWhiteBackgroundProvider
{
	DECLARE_DYNAMIC(CPropConstant)

public:
	CPropConstant();
	virtual ~CPropConstant();

// Dialog Data
	enum { IDD = IDD_PROP_CONSTANT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CComboBox ComboType;
	double value;
	afx_msg void OnCbnSelchangeTypeCombo();
};
