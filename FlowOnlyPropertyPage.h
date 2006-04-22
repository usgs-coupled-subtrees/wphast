#pragma once


// CFlowOnlyPropertyPage dialog
#include "FlowOnly.h"
#include "afxcmn.h"

class CFlowOnlyPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFlowOnlyPropertyPage)

public:
	CFlowOnlyPropertyPage();
	virtual ~CFlowOnlyPropertyPage();

	void SetFlowOnly(const CFlowOnly& flowOnly);
	CFlowOnly GetFlowOnly(void)const;

// Dialog Data
	enum { IDD = IDD_FLOW_ONLY_INT_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CFlowOnly    m_flowOnly;
	bool         m_bDidFlowOnlyComeInAsTrue;

	std::string m_sSoluteTrueRTF;  // IDR_SOLUTE_TRUE_RTF
	std::string m_sSoluteFalseRTF; // IDR_SOLUTE_FALSE_RTF
	std::string m_sDiffusivityRTF; // IDR_DIFFUSIVITY_RTF

public:
	afx_msg void OnBnClickedSoluteTransportRadio();
	afx_msg void OnBnClickedFlowOnlyRadio();
//	virtual BOOL OnSetActive();
	afx_msg void OnBnSetfocusSoluteTransportRadio();
	afx_msg void OnBnSetfocusFlowOnlyRadio();
	afx_msg void OnEnSetfocusMolDiffEdit();
	CRichEditCtrl m_wndRichEditCtrl;
};
