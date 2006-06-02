#pragma once


// CBCTypeDialog dialog

class CBCTypeDialog : public CDialog
{
	DECLARE_DYNAMIC(CBCTypeDialog)

public:
	CBCTypeDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBCTypeDialog();

// Dialog Data
	enum { IDD = IDD_BC_TYPE_DIALOG };

	int bc_type; // SPECIFIED, FLUX, LEAKY

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
