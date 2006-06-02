// ImportErrorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ImportErrorDialog.h"


// CImportErrorDialog dialog

IMPLEMENT_DYNAMIC(CImportErrorDialog, CDialog)
CImportErrorDialog::CImportErrorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CImportErrorDialog::IDD, pParent)
	, m_lpszErrorMessages(0)
{
}

CImportErrorDialog::~CImportErrorDialog()
{
}

void CImportErrorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ERRORS_RICHEDIT, m_wndErrorMessages);
}


BEGIN_MESSAGE_MAP(CImportErrorDialog, CDialog)
END_MESSAGE_MAP()


// CImportErrorDialog message handlers

BOOL CImportErrorDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndStaticIcon.Create(NULL, WS_CHILD|WS_VISIBLE|SS_ICON|SS_CENTERIMAGE, CRect(10, 10, 42, 42), this);
	m_wndStaticIcon.SetIcon(::LoadIcon(NULL, IDI_EXCLAMATION));

	// m_wndErrorMessages.SetWindowText(this->m_lpszErrorMessages);
	m_wndErrorMessages.ReplaceSel(this->m_lpszErrorMessages);
	


	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

INT_PTR CImportErrorDialog::DoModal()
{
	// TODO: Add your specialized code here and/or call the base class
	::MessageBeep(MB_ICONEXCLAMATION);

	return CDialog::DoModal();
}
