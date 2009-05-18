#pragma once

#include "TreePropSheetEx/ResizablePage.h"
#include "TreePropSheetEx/TreePropSheetUtil.hpp"

// CPropDesc dialog

class CPropDesc : public CResizablePage, public TreePropSheet::CWhiteBackgroundProvider
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
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
