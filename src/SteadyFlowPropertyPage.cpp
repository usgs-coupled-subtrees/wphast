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
	, m_bDefaultHCT(FALSE)
{
	CUnits units;
	this->m_strHeadUnits.Format("(%s)", units.head.defined ? units.head.input : units.head.si);

	this->m_strTimeUnits = CGlobal::GetStdTimeUnits(units.time.defined ? units.time.input : units.time.si).c_str();	

	this->m_bDefaultHCT = (this->m_steadyFlow.head_change_target <= 0.0);
	this->m_bDefaultMin = (this->m_steadyFlow.minimum_time_step.value_defined == FALSE);
	this->m_bDefaultMax = (this->m_steadyFlow.maximum_time_step.value_defined == FALSE);

	// load RTF
	CGlobal::LoadRTFString(this->m_sSteadyFlowRTF,           IDR_STEADY_FLOW_RTF);
	CGlobal::LoadRTFString(this->m_sTransientRTF,            IDR_TRANSIENT_RTF);
	CGlobal::LoadRTFString(this->m_sHeadToleranceRTF,        IDR_HEAD_TOLERANCE_RTF);
	CGlobal::LoadRTFString(this->m_sFlowBalanceToleranceRTF, IDR_FLOW_BALANCE_TOLERANCE_RTF);
	CGlobal::LoadRTFString(this->m_sMinimumTimeStepRTF,      IDR_MINIMUM_TIME_STEP_RTF);
	CGlobal::LoadRTFString(this->m_sMaximumTimeStepRTF,      IDR_MAXIMUM_TIME_STEP_RTF);
	CGlobal::LoadRTFString(this->m_sHeadChangeTargetRTF,     IDR_HEAD_CHANGE_TARGET_RTF);
	CGlobal::LoadRTFString(this->m_sIterationsRTF,           IDR_ITERATIONS_RTF);
}

CSteadyFlowPropertyPage::~CSteadyFlowPropertyPage()
{
}

void CSteadyFlowPropertyPage::SetUnits(const CUnits& units)
{
	this->m_strHeadUnits.Format("(%s)", units.head.defined ? units.head.input : units.head.si);
	CGlobal::MinimizeLengthUnits(this->m_strHeadUnits);

	this->m_strTimeUnits = CGlobal::GetStdTimeUnits(units.time.defined ? units.time.input : units.time.si).c_str();	
}

void CSteadyFlowPropertyPage::SetSteadyFlow(const CSteadyFlow& steadyFlow)
{
	this->m_steadyFlow = steadyFlow;
	this->m_bDefaultHCT = (this->m_steadyFlow.head_change_target <= 0.0);
	this->m_bDefaultMin = (this->m_steadyFlow.minimum_time_step.value_defined == FALSE);
	this->m_bDefaultMax = (this->m_steadyFlow.maximum_time_step.value_defined == FALSE);
}

void CSteadyFlowPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	::DDX_Control(pDX, IDC_DESC_RICHEDIT, this->m_wndRichEditCtrl);
	::DDX_Control(pDX, IDC_MIN_TS_COMBO,  this->m_cboMinUnits);
	::DDX_Control(pDX, IDC_MAX_TS_COMBO,  this->m_cboMaxUnits);
	::DDX_Control(pDX, IDC_ITER_SPIN,     this->m_spinIterations);

	if (this->m_bFirstSetActive)
	{
		// set units
		::DDX_Text(pDX, IDC_HEAD_TOL_UNITS_STATIC, this->m_strHeadUnits);
		::DDX_Text(pDX, IDC_HCT_UNITS_STATIC,      this->m_strHeadUnits);

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);

		// fill units combos
		CGlobal::AddTimeUnits(&this->m_cboMinUnits);
		CGlobal::AddTimeUnits(&this->m_cboMaxUnits);
	}

	// set checkmarks
	::DDX_Check(pDX, IDC_HCT_CHECK,    this->m_bDefaultHCT);
	::DDX_Check(pDX, IDC_MIN_TS_CHECK, this->m_bDefaultMin);
	::DDX_Check(pDX, IDC_MAX_TS_CHECK, this->m_bDefaultMax);

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

			// -minimum_time_step
			//
			if (this->m_bDefaultMin)
			{
				this->m_steadyFlow.minimum_time_step.value_defined = FALSE;
			}
			else
			{
				Ctime minTime;

				// value
				//
				::DDX_Text(pDX, IDC_MIN_TS_EDIT, minTime.value);
				minTime.value_defined = TRUE;
				minTime.type = TT_UNITS;

				// units
				//
				CString sVal;
				this->m_cboMinUnits.GetLBText(this->m_cboMinUnits.GetCurSel(), sVal);
				if (sVal.Compare(this->m_strTimeUnits) != 0)
				{
					VERIFY(minTime.SetInput(sVal) == OK);
				}

				this->m_steadyFlow.minimum_time_step = minTime;
			}

			// -maximum_time_step
			//
			if (this->m_bDefaultMax)
			{
				this->m_steadyFlow.maximum_time_step.value_defined = FALSE;
			}
			else
			{
				Ctime maxTime;

				// value
				//
				::DDX_Text(pDX, IDC_MAX_TS_EDIT, maxTime.value);
				maxTime.value_defined = TRUE;
				maxTime.type = TT_UNITS;

				// units
				//
				CString sVal;
				this->m_cboMinUnits.GetLBText(this->m_cboMaxUnits.GetCurSel(), sVal);
				if (sVal.Compare(this->m_strTimeUnits) != 0)
				{
					VERIFY(maxTime.SetInput(sVal) == OK);
				}

				this->m_steadyFlow.maximum_time_step = maxTime;
			}

			// -head_change_target
			//
			if (this->m_bDefaultHCT)
			{
				if (this->m_steadyFlow.head_change_target > 0)
				{
					this->m_steadyFlow.head_change_target *= -1;
				}
			}
			else
			{
				::DDX_Text(pDX, IDC_HCT_EDIT, this->m_steadyFlow.head_change_target);
				if (this->m_steadyFlow.head_change_target <= 0)
				{
					::AfxMessageBox(IDS_GREATER_THAN_ZERO);
					pDX->Fail();
				}
			}

			// -iterations
			//
			::DDX_Text(pDX, IDC_ITER_EDIT, this->m_steadyFlow.iterations);

			// -growth_factor
			//
			::DDX_Text(pDX, IDC_GROWTH_EDIT, this->m_steadyFlow.growth_factor);
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

		// minimum_time_step value
		//
		if (this->m_steadyFlow.minimum_time_step.value_defined)
		{
			::DDX_Text(pDX, IDC_MIN_TS_EDIT, this->m_steadyFlow.minimum_time_step.value);
		}
		else
		{
			CString empty;
			::DDX_Text(pDX, IDC_MIN_TS_EDIT, empty);
		}

		// minimum_time_step units
		//
		if (this->m_steadyFlow.minimum_time_step.input)
		{
			VERIFY(this->m_cboMinUnits.SelectString(0,
				CGlobal::GetStdTimeUnits(this->m_steadyFlow.minimum_time_step.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_cboMinUnits.SelectString(0,
				CGlobal::GetStdTimeUnits(this->m_strTimeUnits).c_str()) != CB_ERR);
		}

		// maximum_time_step value
		//
		if (this->m_steadyFlow.maximum_time_step.value_defined)
		{
			::DDX_Text(pDX, IDC_MAX_TS_EDIT, this->m_steadyFlow.maximum_time_step.value);
		}
		else
		{
			CString empty;
			::DDX_Text(pDX, IDC_MAX_TS_EDIT, empty);
		}

		// maximum_time_step units
		//
		if (this->m_steadyFlow.maximum_time_step.input)
		{
			VERIFY(this->m_cboMaxUnits.SelectString(0,
				CGlobal::GetStdTimeUnits(this->m_steadyFlow.maximum_time_step.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_cboMaxUnits.SelectString(0,
				CGlobal::GetStdTimeUnits(this->m_strTimeUnits).c_str()) != CB_ERR);
		}

		// -head_change_target
		//
		::DDX_Text(pDX, IDC_HCT_EDIT, this->m_steadyFlow.head_change_target);

		// -head_change_target
		//
		if (this->m_steadyFlow.head_change_target <= 0)
		{
			CString d;
			::DDX_Text(pDX, IDC_HCT_EDIT, d);
		}
		else
		{
			::DDX_Text(pDX, IDC_HCT_EDIT, this->m_steadyFlow.head_change_target);
		}
		this->OnBnClickedHctCheck();

		// -iterations
		//
		::DDX_Text(pDX, IDC_ITER_EDIT, this->m_steadyFlow.iterations);

		// -growth_factor
		//
		::DDX_Text(pDX, IDC_GROWTH_EDIT, this->m_steadyFlow.growth_factor);
	}
}


BEGIN_MESSAGE_MAP(CSteadyFlowPropertyPage, CPropertyPage)
	ON_BN_CLICKED(IDC_TRANSIENT_FLOW_RADIO, OnBnClickedTransientFlowRadio)
	ON_BN_CLICKED(IDC_STEADY_FLOW_RADIO, OnBnClickedSteadyFlowRadio)
	ON_BN_CLICKED(IDC_HCT_CHECK, OnBnClickedHctCheck)
	ON_BN_CLICKED(IDC_MIN_TS_CHECK, OnBnClickedMinCheck)
	ON_BN_CLICKED(IDC_MAX_TS_CHECK, OnBnClickedMaxCheck)
	ON_BN_SETFOCUS(IDC_TRANSIENT_FLOW_RADIO, OnBnSetfocusTransientFlowRadio)
	ON_BN_SETFOCUS(IDC_STEADY_FLOW_RADIO, OnBnSetfocusSteadyFlowRadio)
	ON_EN_SETFOCUS(IDC_HEAD_TOL_EDIT, OnEnSetfocusHeadTolEdit)
	ON_EN_SETFOCUS(IDC_FB_TOL_EDIT, OnEnSetfocusFbTolEdit)
	ON_EN_SETFOCUS(IDC_MIN_TS_EDIT, OnEnSetfocusMinTsEdit)
	ON_EN_SETFOCUS(IDC_MAX_TS_EDIT, OnEnSetfocusMaxTsEdit)
	ON_EN_SETFOCUS(IDC_HCT_EDIT, OnEnSetfocusHctEdit)
	ON_EN_SETFOCUS(IDC_ITER_EDIT, OnEnSetfocusIterEdit)
	ON_BN_SETFOCUS(IDC_MIN_TS_CHECK, OnBnSetfocusMinTsCheck)
	ON_BN_SETFOCUS(IDC_MAX_TS_CHECK, OnBnSetfocusMaxTsCheck)
	ON_BN_SETFOCUS(IDC_HCT_CHECK, OnBnSetfocusHctCheck)
	ON_CBN_SETFOCUS(IDC_MIN_TS_COMBO, OnCbnSetfocusMinTsCombo)
	ON_CBN_SETFOCUS(IDC_MAX_TS_COMBO, OnCbnSetfocusMaxTsCombo)
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

		IDC_MIN_TS_STATIC,
		IDC_MIN_TS_EDIT,
		IDC_MIN_TS_COMBO,
		IDC_MIN_TS_CHECK,

		IDC_MAX_TS_STATIC,
		IDC_MAX_TS_EDIT,
		IDC_MAX_TS_COMBO,
		IDC_MAX_TS_CHECK,

		IDC_HCT_STATIC,
		IDC_HCT_EDIT,
		IDC_HCT_UNITS_STATIC,
		IDC_HCT_CHECK,

		IDC_ITER_STATIC,
		IDC_ITER_EDIT,
		IDC_ITER_SPIN,

		IDC_GROWTH_STATIC,
		IDC_GROWTH_EDIT,
	};

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(bEnabled);
		}
	}
	if (bEnabled)
	{
		this->OnBnClickedHctCheck();
		this->OnBnClickedMinCheck();
		this->OnBnClickedMaxCheck();
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

BOOL CSteadyFlowPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	this->m_spinIterations.SetRange32(1, INT_MAX);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSteadyFlowPropertyPage::OnBnClickedHctCheck()
{
	int Ids[] = 
	{
		IDC_HCT_STATIC,
		IDC_HCT_EDIT,
		IDC_HCT_UNITS_STATIC,
	};

	BOOL bEnabled = (this->IsDlgButtonChecked(IDC_HCT_CHECK) != BST_CHECKED);

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(bEnabled);
		}
	}
}

void CSteadyFlowPropertyPage::OnBnClickedMinCheck()
{
	int Ids[] = 
	{
		IDC_MIN_TS_STATIC,
		IDC_MIN_TS_EDIT,
		IDC_MIN_TS_COMBO,
	};

	BOOL bEnabled = (this->IsDlgButtonChecked(IDC_MIN_TS_CHECK) != BST_CHECKED);

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(bEnabled);
		}
	}
}

void CSteadyFlowPropertyPage::OnBnClickedMaxCheck()
{
	int Ids[] = 
	{
		IDC_MAX_TS_STATIC,
		IDC_MAX_TS_EDIT,
		IDC_MAX_TS_COMBO,
	};

	BOOL bEnabled = (this->IsDlgButtonChecked(IDC_MAX_TS_CHECK) != BST_CHECKED);

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(bEnabled);
		}
	}
}

void CSteadyFlowPropertyPage::OnBnSetfocusTransientFlowRadio()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sTransientRTF.c_str());
}

void CSteadyFlowPropertyPage::OnBnSetfocusSteadyFlowRadio()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sSteadyFlowRTF.c_str());
}

void CSteadyFlowPropertyPage::OnEnSetfocusHeadTolEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sHeadToleranceRTF.c_str());
}

void CSteadyFlowPropertyPage::OnEnSetfocusFbTolEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sFlowBalanceToleranceRTF.c_str());
}

void CSteadyFlowPropertyPage::OnEnSetfocusMinTsEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sMinimumTimeStepRTF.c_str());
}

void CSteadyFlowPropertyPage::OnEnSetfocusMaxTsEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sMaximumTimeStepRTF.c_str());
}

void CSteadyFlowPropertyPage::OnEnSetfocusHctEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sHeadChangeTargetRTF.c_str());
}

void CSteadyFlowPropertyPage::OnEnSetfocusIterEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sIterationsRTF.c_str());
}

void CSteadyFlowPropertyPage::OnBnSetfocusMinTsCheck()
{
	this->OnEnSetfocusMinTsEdit();
}

void CSteadyFlowPropertyPage::OnBnSetfocusMaxTsCheck()
{
	this->OnEnSetfocusMaxTsEdit();
}

void CSteadyFlowPropertyPage::OnBnSetfocusHctCheck()
{
	this->OnEnSetfocusHctEdit();
}

void CSteadyFlowPropertyPage::OnCbnSetfocusMinTsCombo()
{
	this->OnEnSetfocusMinTsEdit();
}

void CSteadyFlowPropertyPage::OnCbnSetfocusMaxTsCombo()
{
	this->OnEnSetfocusMaxTsEdit();
}
