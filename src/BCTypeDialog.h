#pragma once

#include "srcinput/BC_info.h"

// CBCTypeDialog dialog

class CBCTypeDialog : public CDialog
{
	DECLARE_DYNAMIC(CBCTypeDialog)

public:
	CBCTypeDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBCTypeDialog();

// Dialog Data
	enum { IDD = IDD_BC_TYPE_DIALOG };

	BC_info::BC_TYPE bc_type;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
};
