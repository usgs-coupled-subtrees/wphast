// SteadyFlowPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "SteadyFlowPropertyPage.h"


// CSteadyFlowPropertyPage dialog

IMPLEMENT_DYNAMIC(CSteadyFlowPropertyPage, CPropertyPage)
CSteadyFlowPropertyPage::CSteadyFlowPropertyPage()
	: CPropertyPage(CSteadyFlowPropertyPage::IDD)
{
}

CSteadyFlowPropertyPage::~CSteadyFlowPropertyPage()
{
}

void CSteadyFlowPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	if (pDX->m_bSaveAndValidate)
	{
		// STEADY_FLOW
		this->m_steadyFlow = (IDC_STEADY_FLOW_RADIO == 
			this->GetCheckedRadioButton(IDC_STEADY_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO));

	}
	else
	{
		// STEADY_FLOW
		if (this->m_steadyFlow) {
			this->CheckRadioButton(IDC_STEADY_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO, IDC_STEADY_FLOW_RADIO);
		}
		else {
			this->CheckRadioButton(IDC_STEADY_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO);
		}
	}

}


BEGIN_MESSAGE_MAP(CSteadyFlowPropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CSteadyFlowPropertyPage message handlers
