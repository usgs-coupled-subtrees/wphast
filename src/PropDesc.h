#pragma once

// COMMENT: {5/26/2009 9:22:47 PM}#include "TreePropSheetEx/ResizablePage.h"
// COMMENT: {5/26/2009 9:22:47 PM}#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "PropPage.h"

// CPropDesc dialog

class CPropDesc : public CPropPage
{
	DECLARE_DYNAMIC(CPropDesc)

public:
	CPropDesc();
	virtual ~CPropDesc();

	void SetDesc(LPCTSTR desc)           { this->sDesc = desc; }
	LPCTSTR GetDesc()const               { return sDesc.c_str(); }

// Dialog Data
	enum { IDD = IDD_PROP_DESC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	std::string sDesc;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
// COMMENT: {5/26/2009 9:23:10 PM}	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
