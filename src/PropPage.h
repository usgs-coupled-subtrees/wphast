#pragma once

#include "TreePropSheetEx/ResizablePage.h"
#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "afxwin.h"
#include "resource.h"

class CPropPage : public CResizablePage, public TreePropSheet::CWhiteBackgroundProvider
{
	DECLARE_DYNCREATE(CPropPage)

// Construction
public:
	CPropPage();
	CPropPage(UINT nIDTemplate, UINT nIDCaption = 0);
	CPropPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropPage)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropPage();

// Generated message map functions
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnKillActive();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCbnSelchangeTypeCombo();

	DECLARE_MESSAGE_MAP()

protected:
	bool SkipUpdateData;
	CComboBox ComboType;
};
