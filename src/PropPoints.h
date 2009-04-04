#pragma once

#include "TreePropSheetEx/ResizablePage.h"
#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "gridctrl/ModGridCtrlEx.h"
#include "property.h"

#include "afxwin.h"
#include "resource.h"

// CPropPoints dialog

class CPropPoints : public CResizablePage, public TreePropSheet::CWhiteBackgroundProvider
{
	DECLARE_DYNAMIC(CPropPoints)

public:
	CPropPoints();
	virtual ~CPropPoints();

	Cproperty GetProperty();
	void SetProperty(Cproperty p);


// Dialog Data
	enum { IDD = IDD_PROP_POINTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CModGridCtrlEx   PointsGrid;
	bool      bSkipUpdateData;

	BOOL SetupGrids(void);

	Cproperty prop;

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CComboBox ComboType;
	afx_msg void OnCbnSelchangeTypeCombo();
};
