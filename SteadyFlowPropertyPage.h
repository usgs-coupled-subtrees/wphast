#pragma once


// CSteadyFlowPropertyPage dialog
#include <string>
#include "SteadyFlow.h"

class CUnits;

class CSteadyFlowPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSteadyFlowPropertyPage)

public:
	CSteadyFlowPropertyPage();
	virtual ~CSteadyFlowPropertyPage();

	void SetSteadyFlow(const CSteadyFlow& steadyFlow);
	CSteadyFlow GetSteadyFlow(void)const              { return m_steadyFlow; }

	void SetUnits(const CUnits& units);

// Dialog Data
	enum { IDD = IDD_STEADY_FLOW_INT_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void SetSteadyFlowEnabled(BOOL bEnabled);

	DECLARE_MESSAGE_MAP()

	CSteadyFlow      m_steadyFlow;
	CString          m_strHeadUnits;
	CString          m_strTimeUnits;
	CSpinButtonCtrl  m_spinIterations;
	CComboBox        m_cboMinUnits;
	CComboBox        m_cboMaxUnits;

	BOOL m_bDefaultHCT;
	BOOL m_bDefaultMin;
	BOOL m_bDefaultMax;

	CRichEditCtrl m_wndRichEditCtrl;

	std::string   m_sSteadyFlowRTF;           // IDR_STEADY_FLOW_RTF
	std::string   m_sTransientRTF;            // IDR_TRANSIENT_RTF
	std::string   m_sHeadToleranceRTF;        // IDR_HEAD_TOLERANCE_RTF
	std::string   m_sFlowBalanceToleranceRTF; // IDR_FLOW_BALANCE_TOLERANCE_RTF
	std::string   m_sMinimumTimeStepRTF;      // IDR_MINIMUM_TIME_STEP_RTF
	std::string   m_sMaximumTimeStepRTF;      // IDR_MAXIMUM_TIME_STEP_RTF
	std::string   m_sHeadChangeTargetRTF;     // IDR_HEAD_CHANGE_TARGET_RTF
	std::string   m_sIterationsRTF;           // IDR_ITERATIONS_RTF

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedTransientFlowRadio();
	afx_msg void OnBnClickedSteadyFlowRadio();
	afx_msg void OnBnClickedHctCheck();
	afx_msg void OnBnClickedMinCheck();
	afx_msg void OnBnClickedMaxCheck();
	afx_msg void OnBnSetfocusTransientFlowRadio();
	afx_msg void OnBnSetfocusSteadyFlowRadio();
	afx_msg void OnEnSetfocusHeadTolEdit();
	afx_msg void OnEnSetfocusFbTolEdit();
	afx_msg void OnEnSetfocusMinTsEdit();
	afx_msg void OnEnSetfocusMaxTsEdit();
	afx_msg void OnEnSetfocusHctEdit();
	afx_msg void OnEnSetfocusIterEdit();
	afx_msg void OnBnSetfocusMinTsCheck();
	afx_msg void OnBnSetfocusMaxTsCheck();
public:
	afx_msg void OnBnSetfocusHctCheck();
	afx_msg void OnCbnSetfocusMinTsCombo();
	afx_msg void OnCbnSetfocusMaxTsCombo();
};

