#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSubdivideDialog dialog

class CSubdivideDialog : public CDialog
{
	DECLARE_DYNAMIC(CSubdivideDialog)

public:
	CSubdivideDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubdivideDialog();

// Dialog Data
	enum { IDD = IDD_SUBDIVIDE_DIALOG };

protected:
	enum FactorType { FACTOR_MULTIPLY = 0, FACTOR_DIVIDE = 1 } factorType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	FactorType CSubdivideDialog::GetFactorType()const;
	void SetFactorType(CSubdivideDialog::FactorType type);

	//CEdit startEdit;
	//CEdit endEdit;

	CSpinButtonCtrl spinNumSubInterval;
	CComboBox factorCombo;
	virtual BOOL OnInitDialog();

	void UpdateIntervals();

	DECLARE_MESSAGE_MAP()
public:
	int numSubInterval;
	double multiplier;
    double start;
    double end;
	CString strStart;
	CString strEnd;
	afx_msg void OnEnChangeFactorEdit();
	afx_msg void OnEnChangeSectionsEdit();
	afx_msg void OnCbnSelchangeMultDivCombo();
	afx_msg void OnEnChangeStartEdit();
	afx_msg void OnEnChangeEndEdit();
};
