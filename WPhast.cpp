// WPhast.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WPhast.h"
#include "MainFrm.h"

#include "WPhastDoc.h"
#include "WPhastView.h"
#include "GridPropertyPage.h"
#include "Units1PropertyPage.h"
#include "Units2PropertyPage.h"
#include "MediaPropertyPage.h"
#include "NewModelIntro.h"
#include "ICHeadPropertyPage.h"
#include "GridElt.h"
#include "HeadIC.h"

#include "NewModelWizard.h"
#include "StartupDialog.h"

#include "ModelessPropertySheet.h"

#include "NewModel.h"

#ifdef _DEBUG
#include "ProtoPropertyPage.h"
#include "Proto2PropertyPage.h"
#include "Proto3PropertyPage.h"
#include "property.h"

#include "IntroPropertyPage.h"
#endif


#if defined(_DEBUG)
// see CWPhastApp::InitInstance()
#include <../atlmfc/src/atl/atls/Common.h>
#include <../atlmfc/src/atl/atls/Allocate.h>
#include <../atlmfc/src/atl/atls/AtlTraceModuleManager.h>
#endif

#if defined(__CPPUNIT__)
#include "TCPPUnit.h"
#include "MapDialog.h"
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
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CWPhastApp object

CWPhastApp theApp;


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
	}
#endif
#if defined(_DEBUG)
	// this treats
	// TRACE("CWPhastDoc::DeleteContents()\n");
	// as
	// TRACE("%s(%d) : CWPhastDoc::DeleteContents()\n", THIS_FILE, __LINE__);
	//
	// CHECKOUT => TRACE(traceAppMsg, 0, _T("Warning: could not parse the path '%s'.\n"), "TESTING");
	g_Allocator.GetProcess()->m_bFileNameAndLineNo = true;
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
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
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
	////{{SRC
	//DebugBreak();
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew) {
		this->m_bShellFileNew = TRUE;
        if (!AfxGetApp()->OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL))
			OnFileNew();
		if (m_pMainWnd == NULL)
			return FALSE;
	}
	else {
	//}}SRC
		// Dispatch commands specified on the command line.  Will return FALSE if
		// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	//{{SRC
	}
	//}}SRC
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
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
	// CWinApp::OnFileNew();
	if (m_pDocManager != NULL) {
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
			}

			CBitmap bmpWatermark;
			CBitmap bmpHeader;
			VERIFY(bmpWatermark.LoadBitmap(IDB_WATERMARK));
			VERIFY(bmpHeader.LoadBitmap(IDB_BANNER));

			CNewModelWizard wizard("New Model Wizard", this->m_pMainWnd, 0, bmpWatermark, 0, bmpHeader);
			// TODO DELETE NEXT LINE
			wizard.SetGridElt(CGridElt::NewDefaults(false));
			wizard.SetHeadIC(CHeadIC::NewDefaults());
			wizard.SetChemIC(CChemIC::NewDefaults());

			if (wizard.DoModal() == ID_WIZFINISH)
			{
				CNewModel model = wizard.GetModel();
				CNewModel deflt = CNewModel::Default();
// COMMENT: {6/10/2004 5:04:48 PM}#if defined(__CPPUNIT__)
// COMMENT: {6/10/2004 5:04:48 PM}				if (!model.HasSiteMap()) {
// COMMENT: {6/10/2004 5:04:48 PM}					CSiteMap siteMap;
// COMMENT: {6/10/2004 5:04:48 PM}					siteMap.m_fileName      = "ok1.bmp";
// COMMENT: {6/10/2004 5:04:48 PM}					siteMap.m_angle         = 15.0;
// COMMENT: {6/10/2004 5:04:48 PM}					siteMap.m_placement[0]  = 621099.81;
// COMMENT: {6/10/2004 5:04:48 PM}					siteMap.m_placement[1]  = 3896414.25;
// COMMENT: {6/10/2004 5:04:48 PM}					siteMap.m_placement[2]  = model.m_grid[2].coord[0] - .01 * (model.m_grid[2].coord[1] - model.m_grid[2].coord[0]);
// COMMENT: {6/10/2004 5:04:48 PM}
// COMMENT: {6/10/2004 5:04:48 PM}					CWorldTransform wtrans;
// COMMENT: {6/10/2004 5:04:48 PM}					wtrans.LoadWorldFile("ok1.bpw");
// COMMENT: {6/10/2004 5:04:48 PM}					siteMap.SetWorldTransform(wtrans);
// COMMENT: {6/10/2004 5:04:48 PM}
// COMMENT: {6/10/2004 5:04:48 PM}					model.SetSiteMap(siteMap);
// COMMENT: {6/10/2004 5:04:48 PM}				}
// COMMENT: {6/10/2004 5:04:48 PM}#endif
				pDoc->New(model);
			}
			else
			{
				pDoc->New(CNewModel::Default());
				pDoc->SetModifiedFlag(FALSE);
			}
		}
		return;
	}

#ifdef RUN_AS_MDI
	if (::AfxMessageBox("TODO: Enter starting dimensions for the grid. Default: (0, 0, 0) - (20, 10, 2)", MB_OKCANCEL) == IDOK) {
		if (m_pDocManager != NULL) {
			// m_pDocManager->OnFileNew();
			//
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

			float xMin = 0;
			float yMin = 0;
			float zMin = 0;

			float xMax = 20;
			float yMax = 10;
			float zMax = 2;

			pDoc->SetGridDims(xMin, xMax, yMin, yMax, zMin, zMax);
		}
	}
#endif // RUN_AS_MDI
}
