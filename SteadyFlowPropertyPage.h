#pragma once


// CSteadyFlowPropertyPage dialog
#include "SteadyFlow.h"

class CUnits;

class CSteadyFlowPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSteadyFlowPropertyPage)

public:
	CSteadyFlowPropertyPage();
	virtual ~CSteadyFlowPropertyPage();

	void SetSteadyFlow(const CSteadyFlow& steadyFlow) { m_steadyFlow = steadyFlow; }
	CSteadyFlow GetSteadyFlow(void)const              { return m_steadyFlow; }

	void SetUnits(const CUnits& units);

// Dialog Data
	enum { IDD = IDD_STEADY_FLOW_INT_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void SetSteadyFlowEnabled(BOOL bEnabled);

	DECLARE_MESSAGE_MAP()

	CSteadyFlow    m_steadyFlow;
	CString        m_strHeadUnits;

public:
	afx_msg void OnBnClickedTransientFlowRadio();
	afx_msg void OnBnClickedSteadyFlowRadio();
};

