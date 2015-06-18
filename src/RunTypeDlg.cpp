// RunTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "RunTypeDlg.h"

#include "srcimpl.h"
#include "SaveCurrentDirectory.h"
#include "Global.h"

// CRunTypeDlg dialog

#undef IDC_SERIAL_RADIO    // -> IDC_MT_RADIO
#undef IDC_PARALLEL_RADIO  // -> IDC_MPI_RADIO

IMPLEMENT_DYNAMIC(CRunTypeDlg, baseCRunTypeDlg)

CRunTypeDlg::CRunTypeDlg(CWnd* pParent /*=NULL*/)
: baseCRunTypeDlg(CRunTypeDlg::IDD, pParent)
, pRecentDBFileList(0)
, nThreads(1)
{

}

CRunTypeDlg::~CRunTypeDlg()
{
}

void CRunTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	baseCRunTypeDlg::DoDataExchange(pDX);

	::DDX_Control(pDX, IDC_DB_COMBO, this->cboDB);

	// database
	::DDX_Text(pDX, IDC_DB_COMBO, this->strDatabase);
	CGlobal::IsValidDatabaseFile(this->strDatabase, pDX);

	// serial or parallel
	if (pDX->m_bSaveAndValidate)
	{
		switch (this->GetCheckedRadioButton(IDC_MPI_RADIO, IDC_MT_RADIO))
		{
		case IDC_MT_RADIO:
			this->bRunMPI = FALSE;
			break;
		case IDC_MPI_RADIO:
			this->bRunMPI = TRUE;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	else
	{
		if (this->bRunMPI)
		{
			this->CheckRadioButton(IDC_MPI_RADIO, IDC_MT_RADIO, IDC_MPI_RADIO);
		}
		else
		{
			this->CheckRadioButton(IDC_MPI_RADIO, IDC_MT_RADIO, IDC_MT_RADIO);
		}
		this->OnBnClickedRadio();
	}

	if (this->bRunMPI)
	{
		// run command
		::DDX_Text(pDX, IDC_COMMAND_EDIT, this->strMPICommand);

		// run command
		::DDX_Text(pDX, IDC_CMD_ARGS_EDIT, this->strCommandArgs);

		if (!pDX->m_bSaveAndValidate)
		{
			// threads
			::DDX_Text(pDX, IDC_THREADS_EDIT, this->nThreads);
		}
	}
	else
	{
		// run command
		::DDX_Text(pDX, IDC_COMMAND_EDIT, this->strMTCommand);

		// threads
		::DDX_Text(pDX, IDC_THREADS_EDIT, this->nThreads);
		::DDV_MinMaxInt(pDX, this->nThreads, 1, INT_MAX);
	}

}


BEGIN_MESSAGE_MAP(CRunTypeDlg, baseCRunTypeDlg)
	ON_BN_CLICKED(IDC_MT_RADIO, &CRunTypeDlg::OnBnClickedRadio)	
	ON_BN_CLICKED(IDC_MPI_RADIO, &CRunTypeDlg::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_DB_BROWSE, &CRunTypeDlg::OnBnClickedDbBrowse)
END_MESSAGE_MAP()


// CRunTypeDlg message handlers

void CRunTypeDlg::OnBnClickedRadio()
{
	switch (this->GetCheckedRadioButton(IDC_MPI_RADIO, IDC_MT_RADIO))
	{
	case IDC_MT_RADIO:
		if (CWnd *pWnd = this->GetDlgItem(IDC_COMMAND_STATIC))
		{
			pWnd->EnableWindow(TRUE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_COMMAND_EDIT))
		{
			pWnd->EnableWindow(TRUE);
			pWnd->SetWindowTextA(this->strMTCommand);
		}

		if (CWnd *pWnd = this->GetDlgItem(IDC_CMD_ARGS_STATIC))
		{
			pWnd->EnableWindow(FALSE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_CMD_ARGS_EDIT))
		{
			pWnd->SetWindowTextA(_T(""));
			pWnd->EnableWindow(FALSE);
		}

		if (CWnd *pWnd = this->GetDlgItem(IDC_THREADS_STATIC))
		{
			pWnd->EnableWindow(TRUE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_THREADS_EDIT))
		{
			CString str;
			pWnd->GetWindowTextA(str);
			str.Trim();
			if (str.IsEmpty())
			{
				str.Format(_T("%d"), this->nThreads);
				pWnd->SetWindowTextA(str);
			}
			pWnd->EnableWindow(TRUE);
		}
		break;
	case IDC_MPI_RADIO:
		if (CWnd *pWnd = this->GetDlgItem(IDC_COMMAND_STATIC))
		{
			pWnd->EnableWindow(TRUE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_COMMAND_EDIT))
		{
			pWnd->EnableWindow(TRUE);
			pWnd->SetWindowTextA(this->strMPICommand);
		}

		if (CWnd *pWnd = this->GetDlgItem(IDC_CMD_ARGS_STATIC))
		{
			pWnd->EnableWindow(TRUE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_CMD_ARGS_EDIT))
		{
			pWnd->EnableWindow(TRUE);
			pWnd->SetWindowTextA(this->strCommandArgs);
		}

		if (CWnd *pWnd = this->GetDlgItem(IDC_THREADS_STATIC))
		{
			pWnd->EnableWindow(FALSE);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_THREADS_EDIT))
		{
			pWnd->EnableWindow(FALSE);
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}


BOOL CRunTypeDlg::OnInitDialog()
{
	baseCRunTypeDlg::OnInitDialog();
	CreateRoot(VERTICAL, 5, 6)
		<< ( pane(HORIZONTAL)
			<< item(IDC_DB_STATIC, NORESIZE | ALIGN_VCENTER )
			<< item(IDC_DB_COMBO, ABSOLUTE_VERT | ALIGN_VCENTER )
			<< item(IDC_DB_BROWSE, NORESIZE | ALIGN_VCENTER )
		)
		<< itemFixed(VERTICAL, 2)
		<< 	( paneCtrl(IDC_STATIC_RT_GB, VERTICAL, GREEDY, nDefaultBorder, 10, 10 )
			<< itemFixed(VERTICAL, 1)
			<< (pane(HORIZONTAL)
				<< item(IDC_MT_RADIO, NORESIZE | ALIGN_VCENTER )
				<< item(IDC_THREADS_STATIC, NORESIZE | ALIGN_VCENTER )
				<< item(IDC_THREADS_EDIT, NORESIZE | ALIGN_VCENTER )
				<< itemGrowing(HORIZONTAL)
				<< item(IDC_MPI_RADIO, NORESIZE | ALIGN_VCENTER )
				<< itemGrowing(HORIZONTAL)
			)
			<< itemFixed(VERTICAL, 6)
			<< (pane(HORIZONTAL)
				<< item(IDC_COMMAND_STATIC, NORESIZE | ALIGN_VCENTER )
				<< item(IDC_COMMAND_EDIT, ABSOLUTE_VERT | ALIGN_VCENTER )
			)
			<< itemFixed(VERTICAL, 6)
			<< (pane(HORIZONTAL)
				<< item(IDC_CMD_ARGS_STATIC, NORESIZE | ALIGN_VCENTER )
				<< item(IDC_CMD_ARGS_EDIT, ABSOLUTE_VERT | ALIGN_VCENTER )
			)
		)
		<< itemFixed(VERTICAL, 6)
		<< (pane(HORIZONTAL)
			<< itemGrowing(HORIZONTAL)
			<< item(IDOK, NORESIZE | ALIGN_VCENTER )
			<< itemFixed(HORIZONTAL, 6)
			<< item(IDCANCEL, NORESIZE | ALIGN_VCENTER )
			<< itemGrowing(HORIZONTAL)
		)
		<< itemFixed(VERTICAL, 6)
		;
	UpdateLayout();

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
