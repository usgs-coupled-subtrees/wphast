// WPhast.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WPhast.h"
#include "MainFrm.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

#include "NewModelWizard.h"
#include "StartupDialog.h"

#include "NewModel.h"
#include "DelayRedraw.h"

#if defined(_DEBUG)
// see CWPhastApp::InitInstance()
#include <../atlmfc/src/atl/atls/Common.h>
#include <../atlmfc/src/atl/atls/Allocate.h>
#include <../atlmfc/src/atl/atls/AtlTraceModuleManager.h>
#endif

#if defined(__CPPUNIT__)
#include "TCPPUnit.h"
#endif

#include "afxcmn.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWPhastApp

BEGIN_MESSAGE_MAP(CWPhastApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
END_MESSAGE_MAP()

// CWPhastApp construction

CWPhastApp::CWPhastApp()
: m_bShellFileNew(FALSE)
{
	// add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CWPhastApp object

CWPhastApp theApp;

#if defined(WPHAST_AUTOMATION)
// This identifier was generated to be statistically unique for your app
// You may change it if you prefer to choose a specific identifier

// {36353903-2137-43FD-9AD6-40B65A96A839}
static const CLSID clsid =
{ 0x36353903, 0x2137, 0x43FD, { 0x9A, 0xD6, 0x40, 0xB6, 0x5A, 0x96, 0xA8, 0x39 } };
const GUID CDECL BASED_CODE _tlid =
		{ 0xD2FD46F3, 0xCD0, 0x4432, { 0xB8, 0x80, 0x72, 0xEC, 0xBE, 0x57, 0xD7, 0xD4 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;
#endif


// CWPhastApp initialization

BOOL CWPhastApp::InitInstance()
{
#if defined(__CPPUNIT__)
	{
		TCPPUnit theTests;  // TCPPUnit must be constructed after/within theApp.InitInstance()
		// CWPhastApp anotherApp; // this will ASSERT
		/***
		CMapDialog dlg;
		dlg.SetFileName("ok1.bmp");
		dlg.SetWorldFileName("ok1.bpw");
		dlg.DoModal();
		return FALSE;
		***/
// COMMENT: {11/26/2008 4:44:28 PM}		int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
// COMMENT: {11/26/2008 4:44:28 PM}		tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
// COMMENT: {11/26/2008 4:44:28 PM}		_CrtSetDbgFlag(tmpDbgFlag);

// COMMENT: {8/21/2009 4:29:28 PM}		// set this to avoid warning display window
// COMMENT: {8/21/2009 4:29:28 PM}		// 
// COMMENT: {8/21/2009 4:29:28 PM}		vtkObject::SetGlobalWarningDisplay(0);
	}
#endif
#if defined(_DEBUG)
	// this treats
	// TRACE("CWPhastDoc::DeleteContents()\n");
	// as
	// TRACE("%s(%d) : CWPhastDoc::DeleteContents()\n", THIS_FILE, __LINE__);
	//
	// CHECKOUT => TRACE(traceAppMsg, 0, _T("Warning: could not parse the path '%s'.\n"), "TESTING");
	///g_Allocator.GetProcess()->m_bFileNameAndLineNo = true;
#endif

	// Check requirements
	// ie  Version 4.71 and later of Shlwapi.dll

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	//{{
	// For richedit in dialogs
	::AfxInitRichEdit2();
	//}}

	// Initialize OLE libraries
// COMMENT: {4/27/2005 4:19:12 PM}	if (!AfxOleInit())
// COMMENT: {4/27/2005 4:19:12 PM}	{
// COMMENT: {4/27/2005 4:19:12 PM}		AfxMessageBox(IDP_OLE_INIT_FAILED);
// COMMENT: {4/27/2005 4:19:12 PM}		return FALSE;
// COMMENT: {4/27/2005 4:19:12 PM}	}
	//{{
	// HACK
    _AFX_THREAD_STATE* pState = AfxGetThreadState();
    if (!pState->m_bNeedTerm && !AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	//}}


	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME_SDI,
		RUNTIME_CLASS(CWPhastDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CWPhastView));
	AddDocTemplate(pDocTemplate);
#if defined(WPHAST_AUTOMATION)
	// Connect the COleTemplateServer to the document template
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template
	m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);
	// Register all OLE server factories as running.  This enables the
	//  OLE libraries to create objects from other applications
	COleTemplateServer::RegisterAll();
		// Note: MDI applications register all server objects without regard
		//  to the /Embedding or /Automation on the command line
#endif
#ifdef RUN_AS_MDI
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
#endif
	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
#if defined(WPHAST_AUTOMATION)
	// App was launched with /Embedding or /Automation switch.
	// Run app as automation server.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Register all OLE server factories as running.  This enables the
		//  OLE libraries to create objects from other applications
		COleTemplateServer::RegisterAll();

		// Don't show the main window
		return TRUE;
	}
	// App was launched with /Unregserver or /Unregister switch.  Unregister
	// typelibrary.  Other unregistration occurs in ProcessShellCommand().
	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		m_server.UpdateRegistry(OAT_DISPATCH_OBJECT, NULL, NULL, FALSE);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
	}
	// App was launched standalone or with other switches (e.g. /Register
	// or /Regserver).  Update registry entries, including typelibrary.
	else
	{
		m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
	}
#endif
	////{{SRC
	//DebugBreak();
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	{
		this->m_bShellFileNew = TRUE;
        if (!AfxGetApp()->OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL))
			OnFileNew();
		if (m_pMainWnd == NULL)
			return FALSE;
	}
	else
	{
	//}}SRC
		// Dispatch commands specified on the command line.  Will return FALSE if
		// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	//{{SRC
	}

#if !defined(_USE_DEFAULT_MENUS_)
	CMenu* pMenu = m_pMainWnd->GetMenu();
	if (pMenu) pMenu->DestroyMenu();
	HMENU hMenu = ((CMainFrame*)m_pMainWnd)->NewMenu();
	pMenu = CMenu::FromHandle(hMenu);
	m_pMainWnd->SetMenu(pMenu);
	((CMainFrame*)m_pMainWnd)->m_hMenuDefault = hMenu;
#endif

	//}}SRC
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// force the view to redraw
	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	pFrame->GetActiveView()->Invalidate();

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CWPhastApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CWPhastApp message handlers


void CWPhastApp::OnFileNew()
{
	if (this->m_pMainWnd && ::IsWindow(this->m_pMainWnd->m_hWnd))
	{
		this->m_pMainWnd->Invalidate(TRUE);
	}
	if (m_pDocManager != NULL)
	{
		POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
		if (pos == NULL)
		{
			TRACE(traceAppMsg, 0, "Error: no document templates registered with CWinApp.\n");
			AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
			return;
		}
		CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT_KINDOF(CDocTemplate, pTemplate);

		CWPhastDoc* pDoc = static_cast<CWPhastDoc*> (pTemplate->OpenDocumentFile(NULL));

		// The main window has been initialized, so show and update it
		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();

		if (pDoc)
		{
			CWnd *pWndTreeCtrl = 0;
			if (CPropertyTreeControlBar *pPropertyTreeControlBar = pDoc->GetPropertyTreeControlBar())
			{
				pWndTreeCtrl = pPropertyTreeControlBar->GetTreeCtrl();
			}
			CDelayRedraw tree(pWndTreeCtrl);

			if (this->m_bShellFileNew)
			{
				this->m_bShellFileNew = FALSE;
				CStartupDialog dlg;
				dlg.DoModal();
				if (dlg.GetAction() == CStartupDialog::SDA_OPEN_FILE)
				{
					this->m_pMainWnd->UpdateWindow();
					pDoc->SetModifiedFlag(FALSE);
					pDoc = static_cast<CWPhastDoc*> (pTemplate->OpenDocumentFile(dlg.GetFileName()));
					return;
				}
				else if (dlg.GetAction() == CStartupDialog::SDA_CREATE_DEFAULT)
				{
					CDelayRedraw delay(this->m_pMainWnd, pDoc);
					pDoc->New(CNewModel::Default());
					pDoc->SetModifiedFlag(FALSE);
					return;
				}
				else if (dlg.GetAction() == CStartupDialog::SDA_IMPORT_FILE)
				{
					CDelayRedraw delay(this->m_pMainWnd, pDoc);
					pDoc->DoImport(dlg.GetFileName());
					pDoc->SetModifiedFlag(FALSE);
					return;
				}
			}

			CBitmap bmpWatermark;
			CBitmap bmpHeader;
			VERIFY(bmpWatermark.LoadBitmap(IDB_WATERMARK));
			VERIFY(bmpHeader.LoadBitmap(IDB_BANNER));

			CNewModelWizard wizard("New Model Wizard", this->m_pMainWnd, 0, bmpWatermark, 0, bmpHeader);

			wizard.SetGridElt(CGridElt::NewDefaults(false));
			wizard.SetHeadIC(CHeadIC::NewDefaults());
			wizard.SetChemIC(CChemIC::NewDefaults());

			if (wizard.DoModal() == ID_WIZFINISH)
			{
				CDelayRedraw delay(this->m_pMainWnd, pDoc);
				CNewModel model = wizard.GetModel();
				pDoc->New(model);
			}
			else
			{
				CDelayRedraw delay(this->m_pMainWnd, pDoc);
				pDoc->New(CNewModel::Default());
				pDoc->SetModifiedFlag(FALSE);
			}
		}
	}
}
