// RunTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "RunTypeDlg.h"


// CRunTypeDlg dialog

IMPLEMENT_DYNAMIC(CRunTypeDlg, CDialog)

CRunTypeDlg::CRunTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRunTypeDlg::IDD, pParent)
{

}

CRunTypeDlg::~CRunTypeDlg()
{
}

void CRunTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// database
	::DDX_Text(pDX, IDC_DB_EDIT, this->strDatabase);


	// serial or parallel
	if (pDX->m_bSaveAndValidate)
	{
		switch (this->GetCheckedRadioButton(IDC_PARALLEL_RADIO, IDC_SERIAL_RADIO))
		{
		case IDC_SERIAL_RADIO:
			this->bParallel = FALSE;
			break;
		case IDC_PARALLEL_RADIO:
			this->bParallel = TRUE;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	else
	{
		if (this->bParallel)
		{
			this->CheckRadioButton(IDC_PARALLEL_RADIO, IDC_SERIAL_RADIO, IDC_PARALLEL_RADIO);
		}
		else
		{
			this->CheckRadioButton(IDC_PARALLEL_RADIO, IDC_SERIAL_RADIO, IDC_SERIAL_RADIO);
		}
		this->OnBnClickedRadio();
	}

	// run command
	::DDX_Text(pDX, IDC_COMMAND_EDIT, this->strCommand);

	// run command
	::DDX_Text(pDX, IDC_CMD_ARGS_EDIT, this->strCommandArgs);
}


BEGIN_MESSAGE_MAP(CRunTypeDlg, CDialog)
	ON_BN_CLICKED(IDC_SERIAL_RADIO, &CRunTypeDlg::OnBnClickedRadio)	
	ON_BN_CLICKED(IDC_PARALLEL_RADIO, &CRunTypeDlg::OnBnClickedRadio)
END_MESSAGE_MAP()


// CRunTypeDlg message handlers

void CRunTypeDlg::OnBnClickedRadio()
{
	switch (this->GetCheckedRadioButton(IDC_PARALLEL_RADIO, IDC_SERIAL_RADIO))
	{
	case IDC_SERIAL_RADIO:
		if (CWnd *pWnd = this->GetDlgItem(IDC_COMMAND_STATIC))
		{
			pWnd->EnableWindow(FALSE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_COMMAND_EDIT))
		{
			pWnd->EnableWindow(FALSE);
		}

		if (CWnd *pWnd = this->GetDlgItem(IDC_CMD_ARGS_STATIC))
		{
			pWnd->EnableWindow(FALSE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_CMD_ARGS_EDIT))
		{
			pWnd->EnableWindow(FALSE);
		}
		break;
	case IDC_PARALLEL_RADIO:
		if (CWnd *pWnd = this->GetDlgItem(IDC_COMMAND_STATIC))
		{
			pWnd->EnableWindow(TRUE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_COMMAND_EDIT))
		{
			pWnd->EnableWindow(TRUE);
		}

		if (CWnd *pWnd = this->GetDlgItem(IDC_CMD_ARGS_STATIC))
		{
			pWnd->EnableWindow(TRUE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_CMD_ARGS_EDIT))
		{
			pWnd->EnableWindow(TRUE);
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

