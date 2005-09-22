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
{
	this->CommonConstruct();
}

CTimeControlPropertyPage::CTimeControlPropertyPage(UINT nIDCaption)
	: CPropertyPage(CTimeControlPropertyPage::IDD, nIDCaption)
{
	this->CommonConstruct();
}

CTimeControlPropertyPage::CTimeControlPropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CTimeControlPropertyPage::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
{
	this->CommonConstruct();
}

void CTimeControlPropertyPage::CommonConstruct(void)
{
	this->m_timeControl2 = CTimeControl2::NewDefaults();

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sTimeStepRTF, IDR_TC_TIME_STEP_RTF);
	CGlobal::LoadRTFString(this->m_sTimeChangeRTF, IDR_TC_TIME_CHANGE_RTF);
}

CTimeControlPropertyPage::~CTimeControlPropertyPage()
{
}

void CTimeControlPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	// Fill TimeStep units combo
	DDX_Control(pDX, IDC_TIME_STEP_COMBO, m_wndTimeStepCombo);
	if (this->m_wndTimeStepCombo.GetCount() == 0)
	{
		CGlobal::AddTimeUnits(&this->m_wndTimeStepCombo);

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sTimeStepRTF.c_str());
	}

	// Fill TimeChange units combo
	DDX_Control(pDX, IDC_TIME_CHANGE_COMBO, m_wndTimeChangeCombo);
	if (this->m_wndTimeChangeCombo.GetCount() == 0)
	{
		CGlobal::AddTimeUnits(&this->m_wndTimeChangeCombo);
	}

	if (pDX->m_bSaveAndValidate)
	{
		CTimeControl2 tc2;
		double dVal;
		CString sVal;

		// timeStep value
		//
		DDX_Text(pDX, IDC_TIME_STEP_EDIT, dVal);
		if (dVal <= 0.0) 
		{
			::AfxMessageBox("Please enter a number greater than 0.0.", MB_ICONEXCLAMATION);
			pDX->Fail();
		}

		Ctime time;
		time.type = UNITS;
		time.SetValue(0.0);

		Ctime timeStep;
		timeStep.type = UNITS;
		timeStep.SetValue(dVal);

		// timeStep units
		//
		this->m_wndTimeStepCombo.GetLBText(this->m_wndTimeStepCombo.GetCurSel(), sVal);
		if (this->m_units.time.defined)
		{
			std::string u = CGlobal::GetStdTimeUnits(this->m_units.time.input);
			if (sVal.Compare(u.c_str()) != 0)
			{
				VERIFY(timeStep.SetInput(sVal) == OK);
			}
		}
		else
		{
			std::string u = CGlobal::GetStdTimeUnits(this->m_units.time.si);
			if (sVal.Compare(u.c_str()) != 0)
			{
				VERIFY(timeStep.SetInput(sVal) == OK);
			}
		}
		tc2.m_timeStep.insert(CTimeSeries<Ctime>::value_type(time, timeStep));

		// timeEnd value
		//
		DDX_Text(pDX, IDC_TIME_CHANGE_EDIT, dVal);
		if (dVal <= 0.0) 
		{
			::AfxMessageBox("Please enter a number greater than 0.0.", MB_ICONEXCLAMATION);
			pDX->Fail();
		}

		Ctime timeEnd;
		timeEnd.type = UNITS;
		timeEnd.SetValue(dVal);

		// timeEnd units
		//
		this->m_wndTimeChangeCombo.GetLBText(this->m_wndTimeChangeCombo.GetCurSel(), sVal);
		if (this->m_units.time.defined)
		{
			std::string u = CGlobal::GetStdTimeUnits(this->m_units.time.input);
			if (sVal.Compare(u.c_str()) != 0)
			{
				VERIFY(timeEnd.SetInput(sVal) == OK);
			}
		}
		else
		{
			std::string u = CGlobal::GetStdTimeUnits(this->m_units.time.si);
			if (sVal.Compare(u.c_str()) != 0)
			{
				VERIFY(timeEnd.SetInput(sVal) == OK);
			}
		}
		tc2.m_timeEnd.insert(timeEnd);

		this->m_timeControl2 = tc2;
	}
	else
	{
		// timeStep units
		//
		ASSERT(this->m_timeControl2.m_timeStep.size() > 0);
		Ctime t(this->m_timeControl2.m_timeStep.begin()->second);
		if (t.input)
		{
			VERIFY(this->m_wndTimeStepCombo.SelectString(0, CGlobal::GetStdTimeUnits(t.input).c_str()) != CB_ERR);
		}
		else
		{
			if (this->m_units.time.defined)
			{
				VERIFY(this->m_wndTimeStepCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str()) != CB_ERR);
			}
			else
			{
				VERIFY(this->m_wndTimeStepCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.si).c_str()) != CB_ERR);
			}
		}

		// timeStep value
		//
		ASSERT(t.value_defined);
		DDX_Text(pDX, IDC_TIME_STEP_EDIT, t.value);


		// timeEnd units
		//
		ASSERT(this->m_timeControl2.m_timeEnd.size() > 0);
		Ctime tEnd(*this->m_timeControl2.m_timeEnd.begin());
		if (tEnd.input)
		{
			VERIFY(this->m_wndTimeChangeCombo.SelectString(0, CGlobal::GetStdTimeUnits(tEnd.input).c_str()) != CB_ERR);
		}
		else
		{
			if (this->m_units.time.defined)
			{
				VERIFY(this->m_wndTimeChangeCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str()) != CB_ERR);
			}
			else
			{
				VERIFY(this->m_wndTimeChangeCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.si).c_str()) != CB_ERR);
			}
		}

		// timeEnd value
		//
		ASSERT(tEnd.value_defined);
		DDX_Text(pDX, IDC_TIME_CHANGE_EDIT, tEnd.value);
	}
}

void CTimeControlPropertyPage::SetUnits(const CUnits& units)
{
	this->m_units = units;
}

void CTimeControlPropertyPage::GetProperties(CTimeControl2& timeControl2)const
{
	timeControl2 = this->m_timeControl2;
}

void CTimeControlPropertyPage::SetProperties(const CTimeControl2& timeControl2)
{
	this->m_timeControl2 = timeControl2;
}

BEGIN_MESSAGE_MAP(CTimeControlPropertyPage, CPropertyPage)
	ON_EN_SETFOCUS(IDC_TIME_STEP_EDIT, OnEnSetfocusTimeStepEdit)
	ON_EN_SETFOCUS(IDC_TIME_CHANGE_EDIT, OnEnSetfocusTimeChangeEdit)
	ON_CBN_SETFOCUS(IDC_TIME_STEP_COMBO, OnEnSetfocusTimeStepEdit)
	ON_CBN_SETFOCUS(IDC_TIME_CHANGE_COMBO, OnEnSetfocusTimeChangeEdit)
END_MESSAGE_MAP()


// CTimeControlPropertyPage message handlers

BOOL CTimeControlPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTimeControlPropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (pSheet->IsKindOf(RUNTIME_CLASS(CNewModelWizard)))
		{
			// get units
			for (int i = 0; i < pSheet->GetPageCount(); ++i)
			{
				if (pSheet->GetPage(i)->IsKindOf(RUNTIME_CLASS(CUnits1PropertyPage)))
				{
					CUnits units;
					static_cast<CUnits1PropertyPage*>(pSheet->GetPage(i))->GetProperties(units);
					this->SetUnits(units);
					this->UpdateData(FALSE);
					break;
				}
			}
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
		}
		else
		{
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

BOOL CTimeControlPropertyPage::OnKillActive()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
}

LRESULT CTimeControlPropertyPage::OnWizardNext()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (!this->UpdateData(TRUE))
		{
			return -1; // return –1 to prevent the page from changing 
		}
	}
	return CPropertyPage::OnWizardNext();
}
