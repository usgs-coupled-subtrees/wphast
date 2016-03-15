// SampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "SampleDlg.h"


// CSampleDlg dialog

IMPLEMENT_DYNAMIC(CSampleDlg, CDialog)

CSampleDlg::CSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSampleDlg::IDD, pParent)
{

}

CSampleDlg::~CSampleDlg()
{
}

void CSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_EDIT_XC1, thin_grid[0]);
		DDX_Text(pDX, IDC_EDIT_YC1, thin_grid[1]);
	}
	else
	{

	}
}


BEGIN_MESSAGE_MAP(CSampleDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_X2, &CSampleDlg::OnBnClickedCheckX2)
	ON_BN_CLICKED(IDC_CHECK_Y2, &CSampleDlg::OnBnClickedCheckY2)
	ON_BN_CLICKED(IDC_CHECK_Z2, &CSampleDlg::OnBnClickedCheckZ2)
END_MESSAGE_MAP()


// CSampleDlg message handlers

void CSampleDlg::OnBnClickedCheckX2()
{
	// TODO: Add your control notification handler code here
}

void CSampleDlg::OnBnClickedCheckY2()
{
	// TODO: Add your control notification handler code here
}

void CSampleDlg::OnBnClickedCheckZ2()
{
	// TODO: Add your control notification handler code here
}

