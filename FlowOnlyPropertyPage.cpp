// FlowOnlyPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "FlowOnlyPropertyPage.h"


// CFlowOnlyPropertyPage dialog

IMPLEMENT_DYNAMIC(CFlowOnlyPropertyPage, CPropertyPage)
CFlowOnlyPropertyPage::CFlowOnlyPropertyPage()
	: CPropertyPage(CFlowOnlyPropertyPage::IDD)
	, m_bDidFlowOnlyComeInAsTrue(false)
{
}

CFlowOnlyPropertyPage::~CFlowOnlyPropertyPage()
{
}

void CFlowOnlyPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	if (pDX->m_bSaveAndValidate)
	{
		// FLOW_ONLY
		if (IDC_FLOW_ONLY_RADIO == this->GetCheckedRadioButton(IDC_FLOW_ONLY_RADIO, IDC_SOLUTE_TRANSPORT_RADIO)) {
			this->m_flowOnly.SetFlowOnly(true);
		}
		else {
			this->m_flowOnly.SetFlowOnly(false);

			double dDiffusivity;
			DDX_Text(pDX, IDC_MOL_DIFF_EDIT, dDiffusivity);
			DDV_MinMaxDouble(pDX, dDiffusivity, 0, DBL_MAX);
			this->m_flowOnly.SetDiffusivity(dDiffusivity);
		}
	}
	else
	{
		// FLOW_ONLY
		if (this->m_flowOnly) {
			this->CheckRadioButton(IDC_FLOW_ONLY_RADIO, IDC_SOLUTE_TRANSPORT_RADIO, IDC_FLOW_ONLY_RADIO);
			this->OnBnClickedFlowOnlyRadio();
		}
		else {
			this->CheckRadioButton(IDC_FLOW_ONLY_RADIO, IDC_SOLUTE_TRANSPORT_RADIO, IDC_SOLUTE_TRANSPORT_RADIO);
			this->OnBnClickedSoluteTransportRadio();
		}
		double dDiffusivity = this->m_flowOnly.GetDiffusivity();
		DDX_Text(pDX, IDC_MOL_DIFF_EDIT, dDiffusivity);
	}
}

void CFlowOnlyPropertyPage::SetFlowOnly(const CFlowOnly& flowOnly)
{
	this->m_flowOnly = flowOnly;
	this->m_bDidFlowOnlyComeInAsTrue = (bool)flowOnly;
}

CFlowOnly CFlowOnlyPropertyPage::GetFlowOnly(void)const
{
	return this->m_flowOnly;
}


BEGIN_MESSAGE_MAP(CFlowOnlyPropertyPage, CPropertyPage)
	ON_BN_CLICKED(IDC_SOLUTE_TRANSPORT_RADIO, OnBnClickedSoluteTransportRadio)
	ON_BN_CLICKED(IDC_FLOW_ONLY_RADIO, OnBnClickedFlowOnlyRadio)
END_MESSAGE_MAP()


// CFlowOnlyPropertyPage message handlers
void CFlowOnlyPropertyPage::OnBnClickedSoluteTransportRadio()
{
	this->GetDlgItem(IDC_MOL_DIFF_STATIC)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_MOL_DIFF_EDIT)->EnableWindow(TRUE);
	//{{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard()) {
		if (this->m_bDidFlowOnlyComeInAsTrue) {
			pSheet->SetWizardButtons(PSWIZB_NEXT);
		}
		else {
			pSheet->SetWizardButtons(PSWIZB_FINISH);
		}

	}
	//}}
}

void CFlowOnlyPropertyPage::OnBnClickedFlowOnlyRadio()
{
	this->GetDlgItem(IDC_MOL_DIFF_STATIC)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_MOL_DIFF_EDIT)->EnableWindow(FALSE);
	//{{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard()) {
		pSheet->SetWizardButtons(PSWIZB_FINISH);
	}
	//}}
}
