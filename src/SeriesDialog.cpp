// SeriesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "SeriesDialog.h"


// CSeriesDialog dialog

IMPLEMENT_DYNAMIC(CSeriesDialog, CDialog)
CSeriesDialog::CSeriesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSeriesDialog::IDD, pParent)
	, m_nType(SD_LINEAR)
	, m_dStartValue(1.0)
	, m_dStepValue(1.0)
	, m_dStopValue(0)
	, m_bStopValueDefined(false)
{
}

CSeriesDialog::~CSeriesDialog()
{
}

void CSeriesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (pDX->m_bSaveAndValidate)
	{
		int id = this->GetCheckedRadioButton(IDC_RADIO_LINEAR, IDC_RADIO_GROWTH);
		if (id == IDC_RADIO_LINEAR)
		{
			this->m_nType = SD_LINEAR;
		}
		else if (id == IDC_RADIO_GROWTH)
		{
			this->m_nType = SD_GROWTH;
		}
		else
		{
			ASSERT(FALSE);
			this->m_nType = SD_LINEAR;
		}
	}
	else
	{
		switch (this->m_nType)
		{
		case SD_LINEAR:
			this->CheckRadioButton(IDC_RADIO_LINEAR, IDC_RADIO_GROWTH, IDC_RADIO_LINEAR);
			break;
		case SD_GROWTH:
			this->CheckRadioButton(IDC_RADIO_LINEAR, IDC_RADIO_GROWTH, IDC_RADIO_GROWTH);
			break;
		default:
			this->CheckRadioButton(IDC_RADIO_LINEAR, IDC_RADIO_GROWTH, IDC_RADIO_LINEAR);
			ASSERT(FALSE);
		}
	}

	// Start value
	DDX_Text(pDX, IDC_EDIT_START, this->m_dStartValue);

	// Step value
	DDX_Text(pDX, IDC_EDIT_STEP, this->m_dStepValue);
	if (pDX->m_bSaveAndValidate && this->m_dStepValue < 0)
	{
		::AfxMessageBox("Step value must be greater than zero.");
		pDX->Fail();
	}
	if (pDX->m_bSaveAndValidate && this->m_dStepValue > DBL_MAX)
	{
		CString str;
		str.Format("Step value must be less than %g.", DBL_MAX);
		::AfxMessageBox(str);
		pDX->Fail();
	}

	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		// Stop value
		this->m_bStopValueDefined = false;
		DDX_Text(pDX, IDC_EDIT_STOP, str);
		if (!str.IsEmpty())
		{
			DDX_Text(pDX, IDC_EDIT_STOP, this->m_dStopValue);
			this->m_bStopValueDefined = true;
		}
	}
	else
	{
		if (this->m_bStopValueDefined)
		{
			DDX_Text(pDX, IDC_EDIT_STOP, this->m_dStopValue);
		}
		else
		{
			CString str(_T(""));
			DDX_Text(pDX, IDC_EDIT_STOP, str);
		}
	}
}


BEGIN_MESSAGE_MAP(CSeriesDialog, CDialog)
END_MESSAGE_MAP()


// CSeriesDialog message handlers
