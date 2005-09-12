#pragma once

#include "SolutionMethod.h"
#include "afxcmn.h"

// CSolutionMethodPropertyPage dialog

class CSolutionMethodPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSolutionMethodPropertyPage)

public:
	CSolutionMethodPropertyPage();
	virtual ~CSolutionMethodPropertyPage();

// Dialog Data
	enum { IDD = IDD_SOLUTION_METHOD_PROPPAGE };

	void SetSolutionMethod(CSolutionMethod sm)   { this->solutionMethod = sm; }
	CSolutionMethod GetSolutionMethod(void)const { return this->solutionMethod; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void EnableIterative(BOOL value);

	DECLARE_MESSAGE_MAP()
public:
	CSolutionMethod solutionMethod;
	afx_msg void OnBnClickedRadioIter();
	afx_msg void OnBnClickedRadioDirect();
	virtual BOOL OnInitDialog();
	CSpinButtonCtrl spinSave;
	CSpinButtonCtrl spinMax;
};
