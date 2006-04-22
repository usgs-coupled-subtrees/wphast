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

	CRichEditCtrl m_wndRichEditCtrl;

	std::string m_sSM_IterativeTrue;    // IDR_SM_ITERATIVE_TRUE_RTF
	std::string m_sSM_IterativeFalse;   // IDR_SM_ITERATIVE_FALSE_RTF
	std::string m_sSM_Tolerance;        // IDR_SM_TOLERANCE_RTF
	std::string m_sSM_SaveDir;          // IDR_SM_SAVE_DIR_RTF
	std::string m_sSM_MaxIter;          // IDR_SM_MAX_ITER_RTF
	std::string m_sSM_Space;            // IDR_SM_SPACE_RTF
	std::string m_sSM_TimeDiff;         // IDR_SM_TIME_DIFF_RTF
	std::string m_sSM_CrossDisp;        // IDR_SM_CROSS_DISP_RTF
	std::string m_sSM_Rebalance;        // IDR_SM_REBALANCE_RTF

public:
	CSolutionMethod solutionMethod;
	afx_msg void OnBnClickedRadioIter();
	afx_msg void OnBnClickedRadioDirect();
	virtual BOOL OnInitDialog();
	CSpinButtonCtrl spinSave;
	CSpinButtonCtrl spinMax;
	afx_msg void OnEnSetfocusEditTol();
	afx_msg void OnEnSetfocusEditSave();
	afx_msg void OnEnSetfocusEditMax();
	afx_msg void OnEnSetfocusEditSpace();
	afx_msg void OnEnSetfocusEditTime();
	afx_msg void OnBnSetfocusCheckCrossDisp();
	afx_msg void OnEnSetfocusEditRebal();
	afx_msg void OnBnSetfocusRadioDirect();
	afx_msg void OnBnSetfocusRadioIter();
};
