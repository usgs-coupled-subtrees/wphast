#pragma once


// CFlowOnlyPropertyPage dialog
#include "FlowOnly.h"

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
public:
	afx_msg void OnBnClickedSoluteTransportRadio();
	afx_msg void OnBnClickedFlowOnlyRadio();
//	virtual BOOL OnSetActive();
};
