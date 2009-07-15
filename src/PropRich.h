#pragma once

#include "PropPage.h"

// CPropRich dialog

class CPropRich : public CPropPage
{
	DECLARE_DYNAMIC(CPropRich)

public:
	CPropRich();
	virtual ~CPropRich();

// COMMENT: {5/29/2009 4:05:05 PM}	void SetDesc(LPCTSTR desc)           { this->sDesc = desc; }
// COMMENT: {5/29/2009 4:05:05 PM}	LPCTSTR GetDesc()const               { return sDesc.c_str(); }

// Dialog Data
	enum { IDD = IDD_PROP_RICH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CRichEditCtrl RichEditCtrl;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
// COMMENT: {5/26/2009 9:23:10 PM}	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
