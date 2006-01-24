// SeriesDialog2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "SeriesDialog2.h"


// CSeriesDialog2 dialog

IMPLEMENT_DYNAMIC(CSeriesDialog2, CDialog)
CSeriesDialog2::CSeriesDialog2(CWnd* pParent /*=NULL*/)
	: CDialog(CSeriesDialog2::IDD, pParent)
	, m_nType(SD_LINEAR)
	, m_dStartValue(1.0)
	, m_dStartIncrement(1.0)
	, m_dFactor(1.0)
	, m_dEndValue(0)
	, m_bEndValueDefined(false)
{
}

CSeriesDialog2::~CSeriesDialog2()
{
}

void CSeriesDialog2::DoDataExchange(CDataExchange* pDX)
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
		this->OnBnClickedRadioGrowth();
	}

	// Start value:
	DDX_Text(pDX, IDC_EDIT_START, this->m_dStartValue);

	// Start increment:
	DDX_Text(pDX, IDC_EDIT_INCR, this->m_dStartIncrement);
	if (pDX->m_bSaveAndValidate && this->m_dStartIncrement < 0)
	{
		::AfxMessageBox("Start increment must be greater than zero.");
		pDX->Fail();
	}
	if (pDX->m_bSaveAndValidate && this->m_dStartIncrement > DBL_MAX)
	{
		CString str;
		str.Format("Start increment must be less than %g.", DBL_MAX);
		::AfxMessageBox(str);
		pDX->Fail();
	}

	// Factor:
	if (pDX->m_bSaveAndValidate)
	{
		if (this->m_nType == SD_GROWTH)
		{
			DDX_Text(pDX, IDC_EDIT_FACTOR, this->m_dFactor);
			if (this->m_dFactor < 0)
			{
				::AfxMessageBox("Factor must be greater than zero.");
				pDX->Fail();
			}
		}
	}
	else
	{
		DDX_Text(pDX, IDC_EDIT_FACTOR, this->m_dFactor);
	}

	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		// End value
		this->m_bEndValueDefined = false;
		DDX_Text(pDX, IDC_EDIT_END, str);
		if (!str.IsEmpty())
		{
			DDX_Text(pDX, IDC_EDIT_END, this->m_dEndValue);
			this->m_bEndValueDefined = true;
		}
	}
	else
	{
		if (this->m_bEndValueDefined)
		{
			DDX_Text(pDX, IDC_EDIT_END, this->m_dEndValue);
		}
		else
		{
			CString str(_T(""));
			DDX_Text(pDX, IDC_EDIT_END, str);
		}
	}

}


BEGIN_MESSAGE_MAP(CSeriesDialog2, CDialog)
	ON_BN_CLICKED(IDC_RADIO_LINEAR, OnBnClickedRadioLinear)
	ON_BN_CLICKED(IDC_RADIO_GROWTH, OnBnClickedRadioGrowth)
END_MESSAGE_MAP()


// CSeriesDialog2 message handlers

void CSeriesDialog2::OnBnClickedRadioLinear()
{
	int Ids[] = 
	{
		IDC_STATIC_FACTOR,
		IDC_EDIT_FACTOR,
	};

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(this->IsDlgButtonChecked(IDC_RADIO_GROWTH) == BST_CHECKED);
		}
	}
}

void CSeriesDialog2::OnBnClickedRadioGrowth()
{
	int Ids[] = 
	{
		IDC_STATIC_FACTOR,
		IDC_EDIT_FACTOR,
	};

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(this->IsDlgButtonChecked(IDC_RADIO_GROWTH) == BST_CHECKED);
		}
	}
}
