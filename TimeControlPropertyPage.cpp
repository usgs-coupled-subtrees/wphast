// TimeControl.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "TimeControlPropertyPage.h"

#include "Global.h"
#include "NewModelWizard.h"


// CTimeControlPropertyPage dialog

IMPLEMENT_DYNAMIC(CTimeControlPropertyPage, CPropertyPage)
CTimeControlPropertyPage::CTimeControlPropertyPage()
	: CPropertyPage(CTimeControlPropertyPage::IDD)
	, m_pLastTimeControl(0)
{
	this->CommonConstruct();
}

CTimeControlPropertyPage::CTimeControlPropertyPage(UINT nIDCaption)
	: CPropertyPage(CTimeControlPropertyPage::IDD, nIDCaption)
	, m_pLastTimeControl(0)
{
	this->CommonConstruct();
}

CTimeControlPropertyPage::CTimeControlPropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CTimeControlPropertyPage::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
	, m_pLastTimeControl(0)
{
	this->CommonConstruct();
}

void CTimeControlPropertyPage::CommonConstruct(void)
{
	this->m_timeControl.SetTimeStepValue(1.0);
	this->m_timeControl.SetTimeEndValue(1.0);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sTimeStepRTF, IDR_TC_TIME_STEP_RTF);
	CGlobal::LoadRTFString(this->m_sTimeChangeRTF, IDR_TC_TIME_CHANGE_RTF);
}

CTimeControlPropertyPage::~CTimeControlPropertyPage()
{
	delete m_pLastTimeControl;
}

void CTimeControlPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	// TimeStep units
	DDX_Control(pDX, IDC_TIME_STEP_COMBO, m_wndTimeStepCombo);
	if (this->m_wndTimeStepCombo.GetCount() == 0) {
		CGlobal::AddTimeUnits(&this->m_wndTimeStepCombo);

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sTimeStepRTF.c_str());
	}

	// TimeChange units
	DDX_Control(pDX, IDC_TIME_CHANGE_COMBO, m_wndTimeChangeCombo);
	if (this->m_wndTimeChangeCombo.GetCount() == 0) {
		CGlobal::AddTimeUnits(&this->m_wndTimeChangeCombo);
	}


	if (pDX->m_bSaveAndValidate)
	{
		double dVal;
		CString sVal;
		Ctime timeStep;
		Ctime timeEnd;

		// TimeStep
		DDX_Text(pDX, IDC_TIME_STEP_EDIT, dVal);
		if (dVal <= 0.0) {
			::AfxMessageBox("Please enter a number greater than 0.0.", MB_ICONEXCLAMATION);
			pDX->Fail();
		}
		timeStep.SetValue(dVal);

		// TimeStep Units
		this->m_wndTimeStepCombo.GetLBText(this->m_wndTimeStepCombo.GetCurSel(), sVal);
		VERIFY(timeStep.SetInput(sVal) == OK);

		// TimeEnd
		DDX_Text(pDX, IDC_TIME_CHANGE_EDIT, dVal);
		if (dVal <= 0.0) {
			::AfxMessageBox("Please enter a number greater than 0.0.", MB_ICONEXCLAMATION);
			pDX->Fail();
		}
		timeEnd.SetValue(dVal);

		// TimeEnd Units
		this->m_wndTimeChangeCombo.GetLBText(this->m_wndTimeChangeCombo.GetCurSel(), sVal);
		VERIFY(timeEnd.SetInput(sVal) == OK);

		if (this->m_pLastTimeControl) {
			Ctime prevTimeEnd = this->m_pLastTimeControl->GetTimeEnd();
			if ((timeEnd.value * timeEnd.input_to_si) <= (prevTimeEnd.value * prevTimeEnd.input_to_si)) {
				CString str;
				str.Format(
					"Time_end value is %g %s, previous time_end was %g %s.\n"
					"Time_end must be greater than zero and greater than the time_end\n"
					"of the previous simulation period.",
					timeEnd.value, timeEnd.input, prevTimeEnd.value, prevTimeEnd.input);
				::AfxMessageBox(str);
				pDX->Fail();
			}
		}

		// OK if here
		this->m_timeControl.SetTimeStep(timeStep);
		this->m_timeControl.SetTimeEnd(timeEnd);

		//double dVal;

		//// TimeStep
		//DDX_Text(pDX, IDC_TIME_STEP_EDIT,   dVal);
		//if (dVal <= 0.0) {
		//	::AfxMessageBox("Please enter a number greater than 0.0.", MB_ICONEXCLAMATION);
		//	pDX->Fail();
		//}
		//this->m_timeControl.SetTimeStepValue(dVal);

		//// TimeChange
		//DDX_Text(pDX, IDC_TIME_CHANGE_EDIT, dVal);
		//if (dVal <= 0.0) {
		//	::AfxMessageBox("Please enter a number greater than 0.0.", MB_ICONEXCLAMATION);
		//	pDX->Fail();
		//}
		//if (this->m_pLastTimeControl && dVal <= this->m_pLastTimeControl->GetTimeEndValue()) {
		//	CString str;
		//	str.Format(
		//		"Time_end value is %g, previous time_end was %g.\n"
		//		"Time_end must be greater than zero and greater than the time_end\n"
		//		"of the previous simulation period.",
		//		dVal, this->m_pLastTimeControl->GetTimeEndValue());
		//	::AfxMessageBox(str);
		//	pDX->Fail();
		//}
		//this->m_timeControl.SetTimeEndValue(dVal);

		//CString strVal;

		//// TimeStep units
		//this->m_wndTimeStepCombo.GetLBText(this->m_wndTimeStepCombo.GetCurSel(), strVal);
		//this->m_timeControl.SetTimeStepInput(strVal);

		//// TimeChange units
		//this->m_wndTimeChangeCombo.GetLBText(this->m_wndTimeChangeCombo.GetCurSel(), strVal);
		//this->m_timeControl.SetTimeEndInput(strVal);

	}
	else
	{
		CString strEmpty;

		// TimeStep
		if (this->m_timeControl.GetTimeStep().value_defined) {
			double dVal = this->m_timeControl.GetTimeStepValue();
			DDX_Text(pDX, IDC_TIME_STEP_EDIT, dVal);
		}
		else {
			DDX_Text(pDX, IDC_TIME_STEP_EDIT, strEmpty);
		}

		// TimeChange
		if (this->m_timeControl.GetTimeEnd().value_defined) {
			double dVal = this->m_timeControl.GetTimeEndValue();
			DDX_Text(pDX, IDC_TIME_CHANGE_EDIT, dVal);
		}
		else {
			DDX_Text(pDX, IDC_TIME_CHANGE_EDIT, strEmpty);
		}


		// TimeStep units
		if (this->m_timeControl.GetTimeStep().type == UNDEFINED) {
			// time
			if (this->m_units.time.defined) {
				VERIFY(this->m_wndTimeStepCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str()) != CB_ERR);
			}
			else {
				VERIFY(this->m_wndTimeStepCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.si).c_str()) != CB_ERR);
			}
		}
		else {
			ASSERT(this->m_timeControl.GetTimeStepInput());
			VERIFY(this->m_wndTimeStepCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_timeControl.GetTimeStepInput()).c_str()) != CB_ERR);
		}

		// TimeChange units
		if (this->m_timeControl.GetTimeEnd().type == UNDEFINED) {
			// time
			if (this->m_units.time.defined) {
				VERIFY(this->m_wndTimeChangeCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str()) != CB_ERR);
			}
			else {
				VERIFY(this->m_wndTimeChangeCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.si).c_str()) != CB_ERR);
			}
		}
		else {
			ASSERT(this->m_timeControl.GetTimeEndInput());
			VERIFY(this->m_wndTimeChangeCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_timeControl.GetTimeEndInput()).c_str()) != CB_ERR);
		}

	}
}

void CTimeControlPropertyPage::SetUnits(const CUnits& units)
{
	this->m_units = units;
}

void CTimeControlPropertyPage::GetProperties(CTimeControl& timeControl)const
{
	timeControl = this->m_timeControl;
}

void CTimeControlPropertyPage::SetProperties(const CTimeControl& timeControl)
{
	this->m_timeControl = timeControl;
}

void CTimeControlPropertyPage::SetPreviousTimeStep(const CTimeControl& timeControl)
{
	delete this->m_pLastTimeControl;
	this->m_pLastTimeControl = new CTimeControl(timeControl);
	ASSERT(this->m_pLastTimeControl);
}

void CTimeControlPropertyPage::GetPreviousTimeStep(CTimeControl& timeControl)const
{
	timeControl = *(this->m_pLastTimeControl);
}

BEGIN_MESSAGE_MAP(CTimeControlPropertyPage, CPropertyPage)
	ON_EN_SETFOCUS(IDC_TIME_STEP_EDIT, OnEnSetfocusTimeStepEdit)
	ON_EN_SETFOCUS(IDC_TIME_CHANGE_EDIT, OnEnSetfocusTimeChangeEdit)
	//ON_CBN_SETFOCUS(IDC_TIME_STEP_COMBO, OnCbnSetfocusTimeStepCombo)
	//ON_CBN_SETFOCUS(IDC_TIME_CHANGE_COMBO, OnCbnSetfocusTimeChangeCombo)
	ON_CBN_SETFOCUS(IDC_TIME_STEP_COMBO, OnEnSetfocusTimeStepEdit)
	ON_CBN_SETFOCUS(IDC_TIME_CHANGE_COMBO, OnEnSetfocusTimeChangeEdit)
END_MESSAGE_MAP()


// CTimeControlPropertyPage message handlers

BOOL CTimeControlPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

// COMMENT: {6/2/2004 9:42:00 PM}	// Disable cancel if no apply button
// COMMENT: {6/2/2004 9:42:00 PM}	//
// COMMENT: {6/2/2004 9:42:00 PM}	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
// COMMENT: {6/2/2004 9:42:00 PM}	ASSERT_KINDOF(CPropertySheet, pSheet);
// COMMENT: {6/2/2004 9:42:00 PM}	if (pSheet->m_psh.dwFlags & PSH_NOAPPLYNOW) {
// COMMENT: {6/2/2004 9:42:00 PM}		pSheet->GetDlgItem(2)->EnableWindow(FALSE);
// COMMENT: {6/2/2004 9:42:00 PM}		pSheet->ModifyStyle(WS_SYSMENU, 0);
// COMMENT: {6/2/2004 9:42:00 PM}	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTimeControlPropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard()) {
		if (pSheet->IsKindOf(RUNTIME_CLASS(CNewModelWizard))) {
			// get units
			for (int i = 0; i < pSheet->GetPageCount(); ++i) {
				if (pSheet->GetPage(i)->IsKindOf(RUNTIME_CLASS(CUnits1PropertyPage))) {
					CUnits units;
					static_cast<CUnits1PropertyPage*>(pSheet->GetPage(i))->GetProperties(units);
					this->SetUnits(units);
					this->UpdateData(FALSE);
					break;
				}
			}
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
		}
		else {
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
		}
	}
	return bRet;
}

void CTimeControlPropertyPage::OnEnSetfocusTimeStepEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sTimeStepRTF.c_str());
}

void CTimeControlPropertyPage::OnEnSetfocusTimeChangeEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sTimeChangeRTF.c_str());
}

void CTimeControlPropertyPage::OnCbnSetfocusTimeStepCombo()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sTimeStepRTF.c_str());
}

void CTimeControlPropertyPage::OnCbnSetfocusTimeChangeCombo()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sTimeChangeRTF.c_str());
}
