// StartupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "StartupDialog.h"


// CStartupDialog dialog

IMPLEMENT_DYNAMIC(CStartupDialog, CDialog)
CStartupDialog::CStartupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CStartupDialog::IDD, pParent)
	, m_nAction(CStartupDialog::SDA_OPEN_FILE)
	, m_strFileName("C:\\Documents and Settings\\charlton\\My Documents\\Visual Studio Projects\\WPhast-dev\\Test\\ok_flow.wphast")
{
}

CStartupDialog::~CStartupDialog()
{
}

void CStartupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (pDX->m_bSaveAndValidate)
	{

		// m_nAction
		switch (this->GetCheckedRadioButton(IDC_OPEN_FILE, IDC_NEW_WIZARD))
		{
		case IDC_OPEN_FILE:
			this->m_nAction = CStartupDialog::SDA_OPEN_FILE;
			{
				char szFilters[] = "WPhast Files (*.wphast)|*.wphast||";

				CFileDialog fileDlg(TRUE, "wphast", "*.wphast",
					OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this, 0);

				if (fileDlg.DoModal() != IDOK) {
					// TODO check if file is valid
					pDX->Fail();
				}

				// m_strFileName
				//
				this->m_strFileName = fileDlg.GetFileName();
			}
			break;
		case IDC_NEW_WIZARD:
		default:
			this->m_nAction = CStartupDialog::SDA_NEW_WIZARD;
			break;
		}
	}
	else
	{
		switch (this->m_nAction)
		{
		case CStartupDialog::SDA_OPEN_FILE:
			this->CheckRadioButton(IDC_OPEN_FILE, IDC_NEW_WIZARD, IDC_OPEN_FILE);
			break;

		case CStartupDialog::SDA_NEW_WIZARD:
			this->CheckRadioButton(IDC_OPEN_FILE, IDC_NEW_WIZARD, IDC_NEW_WIZARD);
			break;
		}
	}
}


BEGIN_MESSAGE_MAP(CStartupDialog, CDialog)
END_MESSAGE_MAP()


// CStartupDialog message handlers

BOOL CStartupDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add extra initialization here

	// Disable red cancel X 
	//
	this->ModifyStyle(WS_SYSMENU, 0);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
