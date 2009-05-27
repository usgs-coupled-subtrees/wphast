#pragma once

// COMMENT: {5/26/2009 9:13:52 PM}#include "TreePropSheetEx/ResizablePage.h"
// COMMENT: {5/26/2009 9:13:52 PM}#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "PropPage.h"

#include "gridctrl/ModGridCtrlEx.h"
#include "property.h"

#include "afxwin.h"
#include "resource.h"

// CPropPoints dialog

class CPropPoints : public CPropPage
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
// COMMENT: {5/26/2009 9:14:18 PM}	bool      bSkipUpdateData;

	BOOL SetupGrids(void);

	Cproperty prop;

public:
	virtual BOOL OnInitDialog();
// COMMENT: {5/26/2009 9:14:24 PM}	virtual BOOL OnKillActive();
// COMMENT: {5/26/2009 9:14:24 PM}	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
// COMMENT: {5/26/2009 9:14:24 PM}	CComboBox ComboType;
// COMMENT: {5/26/2009 9:14:24 PM}	afx_msg void OnCbnSelchangeTypeCombo();
};
