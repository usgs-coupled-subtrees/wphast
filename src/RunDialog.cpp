// RunDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "RunDialog.h"


#include <atlpath.h> // ATL::ATLPath::FileExists
// CRunDialog dialog

IMPLEMENT_DYNAMIC(CRunDialog, CDialog)
CRunDialog::CRunDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRunDialog::IDD, pParent)
	, m_hChildProcess(NULL)
	, m_hStdIn(NULL) // Handle to parents std input.
	, m_bRunThread(TRUE)
	, eventTerminateProcessGroup(NULL)
{
	VERIFY(::GetCurrentDirectory(_MAX_PATH, this->m_szOriginalDir));
}

CRunDialog::~CRunDialog()
{
	VERIFY(::SetCurrentDirectory(this->m_szOriginalDir));
}

void CRunDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ERRORS_RICHEDIT, m_wndOutputRichEditCtrl);
	DDX_Control(pDX, IDOK, m_wndOKButton);
	DDX_Control(pDX, IDC_ABORT, m_wndAbortButton);
}


BEGIN_MESSAGE_MAP(CRunDialog, CDialog)
    ON_MESSAGE(WM_RUN_FINISHED, OnRunFinished)
	ON_BN_CLICKED(IDC_ABORT, OnBnClickedAbort)
END_MESSAGE_MAP()


// CRunDialog message handlers

void CRunDialog::SetWorkingDirectory(LPCTSTR pstrDirName)
{
	VERIFY(::SetCurrentDirectory(pstrDirName));
}

INT_PTR CRunDialog::DoModal()
{
	return CDialog::DoModal();
}

/////////////////////////////////////////////////////////////////////// 
// PrepAndLaunchRedirectedChild
// Sets up STARTUPINFO structure, and launches redirected child.
/////////////////////////////////////////////////////////////////////// 
void CRunDialog::PrepAndLaunchRedirectedChild(HANDLE hChildStdOut,
                                              HANDLE hChildStdIn,
                                              HANDLE hChildStdErr)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	// Set up the start up info struct.
	ZeroMemory(&si,sizeof(STARTUPINFO));
	si.cb         = sizeof(STARTUPINFO);
	si.dwFlags    = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	si.hStdOutput = hChildStdOut;
	si.hStdInput  = hChildStdIn;
	si.hStdError  = hChildStdErr;

	// Use this if you want to hide the child:
	si.wShowWindow = SW_HIDE;
	// Note that dwFlags must include STARTF_USESHOWWINDOW if you want to
	// use the wShowWindow flags.

	TCHAR szPhastExe[] = _T("phast-ser.exe");

	TCHAR szCmdLine[2*MAX_PATH+100];
	TCHAR szExpandedCmdLine[2*MAX_PATH+100];


	// Create an inheritable event handle to use as our IPC mechanism
	// to terminate the processes in the process group
	// NOTE: Keep this handle for the entire lifetime of this application
	this->eventTerminateProcessGroup = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	::SetHandleInformation(this->eventTerminateProcessGroup, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

	::ResetEvent(this->eventTerminateProcessGroup);   // Do this before each spawn

	::GetModuleFileName(NULL, szCmdLine, MAX_PATH);
	*(_tcsrchr(szCmdLine, _TEXT('\\')) + 1) = 0; // truncate EXE filename

	if (this->bParallel)
	{
		::_stprintf(::_tcschr(szCmdLine, 0), _TEXT("spawnc.exe %d %s %s"), (int)this->eventTerminateProcessGroup, this->strCommand, this->strCommandArgs);
	}
	else
	{
		::_stprintf(::_tcschr(szCmdLine, 0), _TEXT("spawnc.exe %d %s"), (int)this->eventTerminateProcessGroup, szPhastExe);
	}
	::ExpandEnvironmentStrings(szCmdLine, szExpandedCmdLine, 2*MAX_PATH+100);

	if (!::CreateProcess(
		NULL,
		szExpandedCmdLine,
		NULL,
		NULL,
		TRUE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi
		))
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);

		static TCHAR message[] = "CreateProcess failed:\n";
		DWORD dwWritten;
		::WriteFile(si.hStdError, message, (DWORD)::strlen(message), &dwWritten, NULL);
		::WriteFile(si.hStdError, lpMsgBuf, (DWORD)::strlen((LPCTSTR)lpMsgBuf), &dwWritten, NULL);

		// Free the buffer.
		LocalFree( lpMsgBuf );

		return;
	}

	// Set child process handle to cause threads to exit.
	this->m_hChildProcess = pi.hProcess;

	// Close any unnecessary handles.
	if (!CloseHandle(pi.hThread))
	{
		::AfxMessageBox("CloseHandle pi.hThread");
	}
}

/////////////////////////////////////////////////////////////////////// 
// ReadAndHandleOutput
// Monitors handle for input. Exits when child exits or pipe breaks.
/////////////////////////////////////////////////////////////////////// 
void CRunDialog::ReadAndHandleOutput(HANDLE hPipeRead)
{
	CHAR lpBuffer[256];
	DWORD nBytesRead;

	while(TRUE)
	{
		if (!ReadFile(hPipeRead, lpBuffer, sizeof(lpBuffer) - 1, &nBytesRead, NULL) || !nBytesRead)
		{
			if (::GetLastError() == ERROR_BROKEN_PIPE) {
				break; // pipe done - normal exit path.
			}
			else {
                ::AfxMessageBox("ReadFile"); // Something bad happened.
			}
		}

		if (nBytesRead > 0) {
			lpBuffer[nBytesRead] = 0;
			::OutputDebugString(lpBuffer);
			this->m_wndOutputRichEditCtrl.SetSel(-1, -1);
			this->m_wndOutputRichEditCtrl.ReplaceSel(lpBuffer);
			this->m_wndOutputRichEditCtrl.LineScroll(INT_MAX);
			//this->m_wndOutputRichEditCtrl.SetSel(-1, -1);
		}
	}
}

BOOL CRunDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWinThread* pWinThread = ::AfxBeginThread(RunThreadProc, this);
	if (pWinThread){
		this->ModifyStyle(WS_SYSMENU, 0);
		this->m_wndOKButton.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

UINT CRunDialog::RunThreadProc(LPVOID pParam)
{
	CRunDialog* pThis = static_cast<CRunDialog*>(pParam);

	HANDLE hOutputReadTmp;
	HANDLE hOutputRead;
	HANDLE hOutputWrite;

	HANDLE hInputWriteTmp;
	HANDLE hInputRead;
	HANDLE hInputWrite;

	HANDLE hErrorWrite;
	///////////////HANDLE hThread;
	///////////////DWORD ThreadId;
	SECURITY_ATTRIBUTES sa;

	// Set up the security attributes struct.
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;


	// Create the child output pipe.
	if (!::CreatePipe(
		&hOutputReadTmp,
		&hOutputWrite,
		&sa,
		0
		))
	{
		::AfxMessageBox("CreatePipe hOutputReadTmp");
	}

	// Create the child input pipe.
	if (!::CreatePipe(
		&hInputRead,
		&hInputWriteTmp,
		&sa,
		0
		))
	{
		::AfxMessageBox("CreatePipe hInputRead");
	}

	// Create a duplicate of the output write handle for the std error
	// write handle. This is necessary in case the child application
	// closes one of its std output handles.
	if (!::DuplicateHandle(
		GetCurrentProcess(),
		hOutputWrite,
		GetCurrentProcess(),
		&hErrorWrite,
		0,
		TRUE,
		DUPLICATE_SAME_ACCESS
		))
	{
		::AfxMessageBox("DuplicateHandle hOutputWrite");
	}

	// Create new output read handle and the input write handles. Set
	// the Properties to FALSE. Otherwise, the child inherits the
	// properties and, as a result, non-closeable handles to the pipes
	// are created.
	if (!::DuplicateHandle(
		GetCurrentProcess(),
		hOutputReadTmp,
		GetCurrentProcess(),
		&hOutputRead, // Address of new handle.
		0,
		FALSE, // Make it uninheritable.
		DUPLICATE_SAME_ACCESS
		))
	{
		::AfxMessageBox("DuplicateHandle hOutputReadTmp");
	}

	if (!::DuplicateHandle(
		GetCurrentProcess(),
		hInputWriteTmp,
		GetCurrentProcess(),
		&hInputWrite, // Address of new handle.
		0,
		FALSE, // Make it uninheritable.
		DUPLICATE_SAME_ACCESS
		))
	{
		::AfxMessageBox("DuplicateHandle hInputWriteTmp");
	}


	// Close inheritable copies of the handles you do not want to be
	// inherited.
	if (!CloseHandle(hOutputReadTmp))
	{
		::AfxMessageBox("CloseHandle hOutputReadTmp");
	}
	if (!CloseHandle(hInputWriteTmp))
	{
		::AfxMessageBox("CloseHandle hInputWriteTmp");
	}


	/*************************************************************************************
	// Get std input handle so you can close it and force the ReadFile to
	// fail when you want the input thread to exit.
	if ( (this->m_hStdIn = ::GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE )
	{
		::AfxMessageBox("GetStdHandle STD_INPUT_HANDLE");
	}
	*************************************************************************************/

	/**********************************************
	check out to cancel
	BOOL TerminateProcess(
	HANDLE hProcess, // handle to the process
	UINT uExitCode   // exit code for the process
	);
	**********************************************/


	pThis->PrepAndLaunchRedirectedChild(hOutputWrite, hInputRead, hErrorWrite);

	// Close pipe handles (do not continue to modify the parent).
	// You need to make sure that no handles to the write end of the
	// output pipe are maintained in this process or else the pipe will
	// not close when the child process exits and the ReadFile will hang.
	if (!::CloseHandle(hOutputWrite)) {
		::AfxMessageBox("CloseHandle hOutputWrite");
	}
	if (!::CloseHandle(hInputRead )) {
		::AfxMessageBox("CloseHandle hInputRead");
	}
	if (!::CloseHandle(hErrorWrite)) {
		::AfxMessageBox("CloseHandle hErrorWrite");
	}

	/*********************************************
	May want to use to send a Ctrl+C
	// Launch the thread that gets the input and sends it to the child.
	hThread = ::CreateThread(
		NULL,
		0,
		GetAndSendInputThread,
		(LPVOID)hInputWrite,
		0,
		&ThreadId
		);
	if (hThread == NULL) {
		::AfxMessageBox("CreateThread");
	}
	*********************************************/


	// Read the child's output.
	pThis->ReadAndHandleOutput(hOutputRead);
	// Redirection is complete


	/*************************************************************************************
	// Force the read on the input to return by closing the stdin handle.
	if (!::CloseHandle(this->m_hStdIn)) {
		::AfxMessageBox("CloseHandle this->m_hStdIn");
	}
	*************************************************************************************/


	// Tell the thread to exit and wait for thread to die.
	pThis->m_bRunThread = FALSE;

	/**
	if (::WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED) {
		::AfxMessageBox("WaitForSingleObject");
	}
	**/

	if (!::CloseHandle(hOutputRead)) {
		::AfxMessageBox("CloseHandle hOutputRead");
	}
	if (!::CloseHandle(hInputWrite)) {
		::AfxMessageBox("CloseHandle hInputWrite");
	}

	pThis->PostMessage(WM_RUN_FINISHED, 0, 0L);

	return 0;
}

LRESULT CRunDialog::OnRunFinished(WPARAM, LPARAM)
{
	::MessageBeep(-1);
	this->m_wndAbortButton.EnableWindow(FALSE);
	this->m_wndOKButton.EnableWindow(TRUE);
	this->ModifyStyle(0, WS_SYSMENU);
	this->SetWindowText("Finished");
	this->RedrawWindow();
	return 0;
}

void CRunDialog::OnBnClickedAbort()
{
	if (this->m_hChildProcess)
	{
		::SetEvent(this->eventTerminateProcessGroup);
		this->SetWindowText("Aborting");
		this->RedrawWindow();
	}
}
