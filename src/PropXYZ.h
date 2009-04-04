#pragma once

#include "TreePropSheetEx/ResizablePage.h"
#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "afxwin.h"
#include "resource.h"
#include "property.h"


// CPropXYZ dialog

class CPropXYZ : public CResizablePage, public TreePropSheet::CWhiteBackgroundProvider
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
	bool      bSkipUpdateData;

	Cproperty prop;

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CComboBox ComboType;
	afx_msg void OnCbnSelchangeTypeCombo();
	afx_msg void OnBnClickedButtonXyz();
};
