// ImportWarningDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ImportWarningDialog.h"


// CImportWarningDialog dialog

IMPLEMENT_DYNAMIC(CImportWarningDialog, CDialog)

CImportWarningDialog::CImportWarningDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CImportWarningDialog::IDD, pParent)
{

}

CImportWarningDialog::~CImportWarningDialog()
{
}

void CImportWarningDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ERRORS_RICHEDIT, this->RichEditCtrl);
}


BEGIN_MESSAGE_MAP(CImportWarningDialog, CDialog)
END_MESSAGE_MAP()


// CImportWarningDialog message handlers

BOOL CImportWarningDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add extra initialization here
	this->StaticIcon.Create(NULL, WS_CHILD|WS_VISIBLE|SS_ICON|SS_CENTERIMAGE, CRect(10, 10, 42, 42), this);
	this->StaticIcon.SetIcon(::LoadIcon(NULL, IDI_EXCLAMATION));

	this->RichEditCtrl.ReplaceSel(this->WarningMessages);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

INT_PTR CImportWarningDialog::DoModal()
{
	// Add your specialized code here and/or call the base class
	::MessageBeep(MB_ICONEXCLAMATION);

	return CDialog::DoModal();
}
