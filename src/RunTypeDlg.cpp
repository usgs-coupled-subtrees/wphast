// RunTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "RunTypeDlg.h"

#include "srcimpl.h"
#include "SaveCurrentDirectory.h"

// CRunTypeDlg dialog

IMPLEMENT_DYNAMIC(CRunTypeDlg, CDialog)

CRunTypeDlg::CRunTypeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRunTypeDlg::IDD, pParent)
, pRecentDBFileList(0)
{

}

CRunTypeDlg::~CRunTypeDlg()
{
}

void CRunTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	::DDX_Control(pDX, IDC_DB_COMBO, this->cboDB);

	// database
	::DDX_Text(pDX, IDC_DB_COMBO, this->strDatabase);

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
	ON_BN_CLICKED(IDC_DB_BROWSE, &CRunTypeDlg::OnBnClickedDbBrowse)
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


BOOL CRunTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add extra initialization here
	if (this->pRecentDBFileList)
	{
		int n = this->pRecentDBFileList->GetSize();
		for (int i = 0; i < n; ++i)
		{
			if ((*this->pRecentDBFileList)[i].GetLength() == 0) break;
			this->cboDB.AddString((*this->pRecentDBFileList)[i]);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRunTypeDlg::OnBnClickedDbBrowse()
{
	// Add your control notification handler code here

	CSaveCurrentDirectory scd;

	static char szFilters[] =
		"Database Files (*.dat)|*.dat|"
		"All Files (*.*)|*.*|"
		"|";

	CFileDialog fileDlg(TRUE, "dat", NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY
#if (_WIN32_WINNT >= 0x0500)
		| OFN_DONTADDTORECENT | OFN_NOCHANGEDIR
#endif
		, szFilters, this);


	if (CWnd *pWnd = this->GetDlgItem(IDC_DB_COMBO))
	{
		TCHAR buffer[_MAX_PATH];
		pWnd->GetWindowTextA(buffer, _MAX_PATH);

		// fully qualify the path name
		TCHAR szExpanded[2*_MAX_PATH];
		VERIFY(::ExpandEnvironmentStrings(buffer, szExpanded, 2*_MAX_PATH));

		if (::SrcFullPath(buffer, szExpanded))
		{
			if (::PathRemoveFileSpec(buffer))
			{
				fileDlg.m_ofn.lpstrInitialDir = buffer;
			}
		}
	}

	if (fileDlg.DoModal() == IDOK)
	{
		if (CWnd *pWnd = this->GetDlgItem(IDC_DB_COMBO))
		{
			TCHAR szPath[_MAX_PATH];
			::ExpandEnvironmentStrings(_T("%INSTALLDIR%"), szPath, _MAX_PATH);
			CString str(szPath);
			CString strFile(fileDlg.GetPathName());
			CString strFileLower(fileDlg.GetPathName());
			strFileLower.MakeLower();
			str.MakeLower();
			int n = strFileLower.Find(str);
			if (strFileLower.Find(str) == 0)
			{
				str = strFile.Mid(str.GetLength());
				strFile = _T("%INSTALLDIR%");
				strFile += str;
			}
			pWnd->SetWindowTextA(strFile);
		}
	}
}
