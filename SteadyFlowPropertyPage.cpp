// SteadyFlowPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "SteadyFlowPropertyPage.h"

#include "Units.h"
#include "Global.h"


// CSteadyFlowPropertyPage dialog

IMPLEMENT_DYNAMIC(CSteadyFlowPropertyPage, CPropertyPage)
CSteadyFlowPropertyPage::CSteadyFlowPropertyPage()
	: CPropertyPage(CSteadyFlowPropertyPage::IDD)
{
	CUnits units;
	this->m_strHeadUnits.Format("(%s)", units.head.defined ? units.head.input : units.head.si);
}

CSteadyFlowPropertyPage::~CSteadyFlowPropertyPage()
{
}

void CSteadyFlowPropertyPage::SetUnits(const CUnits& units)
{
	this->m_strHeadUnits.Format("(%s)", units.head.defined ? units.head.input : units.head.si);
	CGlobal::MinimizeLengthUnits(this->m_strHeadUnits);
}

void CSteadyFlowPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_HEAD_TOL_UNITS_STATIC, this->m_strHeadUnits);

	if (pDX->m_bSaveAndValidate)
	{
		// STEADY_FLOW
		//
		this->m_steadyFlow.steady_flow = (IDC_STEADY_FLOW_RADIO == 
			this->GetCheckedRadioButton(IDC_STEADY_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO));

		// -head_tolerance
		//
		if (this->m_steadyFlow.steady_flow)
		{
			// -head_tolerance
			//
			::DDX_Text(pDX, IDC_HEAD_TOL_EDIT, this->m_steadyFlow.head_tolerance);

			// -flow_balance_tolerance
			//
			::DDX_Text(pDX, IDC_FB_TOL_EDIT, this->m_steadyFlow.flow_balance_tolerance);
		}
	}
	else
	{
		// STEADY_FLOW
		//
		if (this->m_steadyFlow.steady_flow)
		{
			this->CheckRadioButton(IDC_STEADY_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO, IDC_STEADY_FLOW_RADIO);
			this->SetSteadyFlowEnabled(TRUE);
		}
		else
		{
			this->CheckRadioButton(IDC_STEADY_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO);
			this->SetSteadyFlowEnabled(FALSE);
		}

		// -head_tolerance
		//
		::DDX_Text(pDX, IDC_HEAD_TOL_EDIT, this->m_steadyFlow.head_tolerance);

		// -flow_balance_tolerance
		//
		::DDX_Text(pDX, IDC_FB_TOL_EDIT, this->m_steadyFlow.flow_balance_tolerance);

	}

}


BEGIN_MESSAGE_MAP(CSteadyFlowPropertyPage, CPropertyPage)
	ON_BN_CLICKED(IDC_TRANSIENT_FLOW_RADIO, OnBnClickedTransientFlowRadio)
	ON_BN_CLICKED(IDC_STEADY_FLOW_RADIO, OnBnClickedSteadyFlowRadio)
END_MESSAGE_MAP()


// CSteadyFlowPropertyPage message handlers
void CSteadyFlowPropertyPage::SetSteadyFlowEnabled(BOOL bEnabled)
{
	int Ids[] = 
	{
		IDC_HEAD_TOL_STATIC,
		IDC_HEAD_TOL_EDIT,
		IDC_HEAD_TOL_UNITS_STATIC,

		IDC_FB_TOL_STATIC,
		IDC_FB_TOL_EDIT,
	};

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(bEnabled);
		}
	}

}

void CSteadyFlowPropertyPage::OnBnClickedTransientFlowRadio()
{
	if (IDC_STEADY_FLOW_RADIO == this->GetCheckedRadioButton(IDC_STEADY_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO))
	{
		this->SetSteadyFlowEnabled(TRUE);
	}
	else
	{
		this->SetSteadyFlowEnabled(FALSE);
	}
}

void CSteadyFlowPropertyPage::OnBnClickedSteadyFlowRadio()
{
	if (IDC_STEADY_FLOW_RADIO == this->GetCheckedRadioButton(IDC_STEADY_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO))
	{
		this->SetSteadyFlowEnabled(TRUE);
	}
	else
	{
		this->SetSteadyFlowEnabled(FALSE);
	}
}
