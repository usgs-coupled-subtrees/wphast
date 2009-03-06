// TestVTKDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "TestVTKDialog.h"

#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkWin32RenderWindowInteractor.h>

//{{
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

vtkConeSource *cone;
vtkPolyDataMapper *coneMapper;
vtkActor *coneActor;

#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkDataSetMapper.h>
#include <vtkImageShiftScale.h>
#include <vtkImageData.h>
#include <vtkImageActor.h> 
#include <vtkInteractorStyleImage.h> 
#include <vtkCubeSource.h>
#include <vtkProperty.h>

// CTestVTKDialog dialog

IMPLEMENT_DYNAMIC(CTestVTKDialog, CDialog)
CTestVTKDialog::CTestVTKDialog(CWnd* pParent /*=NULL*/)
	: ETSLayoutDialog(CTestVTKDialog::IDD, pParent)
	, m_hWndStatusBar(0)
{
	setRootBorders(0, 0);


	// Create the the renderer, window and interactor objects.
	//
	this->m_RenderWindow           = vtkWin32OpenGLRenderWindow::New();
	this->m_Renderer               = vtkRenderer::New();
	this->m_RenderWindowInteractor = vtkWin32RenderWindowInteractor::New();



	///////////////{{{{{{{{{{{{{
	const char filename[] = "ok1.bmp";

	vtkImageReader2Factory *factory = vtkImageReader2Factory::New();
	vtkImageReader2 *imageReader = factory->CreateImageReader2(filename);

	double xleft = 577741.906767;
	double yupper = 3986366.032909;

	double xscale = 183.586248;
	double yscale = 183.586248;

	imageReader->SetFileName(filename);
	imageReader->SetDataSpacing(183.586248, 183.586248, 1);
	imageReader->Update();
	int *dims = imageReader->GetDataExtent();
	ASSERT(dims[0] == 0 && dims[2] == 0);

	double ylower = yupper - dims[3]*yscale;
	imageReader->SetDataOrigin(xleft, ylower, 0.0);

	//{{
#ifdef _DEBUG
	afxDump << "vtkImageReader2\n";
	ostrstream oss;

	imageReader->PrintSelf(oss, 4);

	oss << ends;
	afxDump << oss.str() << "\n";
	oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
#endif
	//}}

// COMMENT: {2/4/2004 10:27:46 PM}	vtkDataSetMapper *mapper = vtkDataSetMapper::New();
// COMMENT: {2/4/2004 10:27:46 PM}	mapper->SetInput(imageReader->GetOutput());

	vtkImageShiftScale *shiftScale = vtkImageShiftScale::New();
	shiftScale->SetInput( imageReader->GetOutput() );
	shiftScale->SetShift(0);
	shiftScale->SetScale(1.0);
	shiftScale->SetOutputScalarTypeToUnsignedChar();

	vtkImageActor *imageActor = vtkImageActor::New();
	imageActor->SetInput(shiftScale->GetOutput());
	///////////////}}}}}}}}}}}}}


	///////////////{{{{{{{{{{{{{
	//
	// Next we create an instance of vtkConeSource and set some of its
	// properties. The instance of vtkConeSource "cone" is part of a
	// visualization pipeline (it is a source process object); it produces data
	// (output type is vtkPolyData) which other filters may process.
	//
	cone = vtkConeSource::New();
	cone->SetHeight( 3.0 );
	cone->SetRadius( 1.0 );
	cone->SetResolution( 10 );

	//
	// In this example we terminate the pipeline with a mapper process object.
	// (Intermediate filters such as vtkShrinkPolyData could be inserted in
	// between the source and the mapper.)  We create an instance of
	// vtkPolyDataMapper to map the polygonal data into graphics primitives. We
	// connect the output of the cone souece to the input of this mapper.
	//
	coneMapper = vtkPolyDataMapper::New();
	coneMapper->SetInput( cone->GetOutput() );

	//
	// Create an actor to represent the cone. The actor orchestrates rendering
	// of the mapper's graphics primitives. An actor also refers to properties
	// via a vtkProperty instance, and includes an internal transformation
	// matrix. We set this actor's mapper to be coneMapper which we created
	// above.
	//
	coneActor = vtkActor::New();
	coneActor->SetMapper( coneMapper );

	//
	// Create the Renderer and assign actors to it. A renderer is like a
	// viewport. It is part or all of a window on the screen and it is
	// responsible for drawing the actors it has.  We also set the background
	// color here.
	//
	///this->m_Renderer->AddActor( coneActor );
	this->m_Renderer->AddActor( imageActor );
	this->m_Renderer->SetBackground( 0.1, 0.2, 0.4 );
	///////////////}}}}}}}}}}}}}
}

CTestVTKDialog::~CTestVTKDialog()
{
	// Delete the the renderer, window and interactor objects.
	//
	this->m_RenderWindow->Delete();
	this->m_Renderer->Delete();
	this->m_RenderWindowInteractor->Delete();

	///////////////{{{{{{{{{{{{{
	cone->Delete();
	coneMapper->Delete();
	coneActor->Delete();
	///////////////}}}}}}}}}}}}}
}

void CTestVTKDialog::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestVTKDialog, ETSLayoutDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


CRect CTestVTKDialog::GetRect() 
{ 
	CRect r; 
	GetClientRect(r);

	if( m_bGripper ) 
	{
		if( ::IsWindow(m_StatusBar.GetSafeHwnd()) ) 
		{
			CRect rcSizeIcon;
			m_StatusBar.GetWindowRect( rcSizeIcon);
			/// r.bottom -= (rcSizeIcon.Height() - m_sizeRootBorders.cy - 5);
			r.bottom -= rcSizeIcon.Height();
		}
	}
	return r; 
}


// CTestVTKDialog message handlers

int CTestVTKDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (ETSLayoutDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Add your specialized creation code here
	if (!this->m_RenderWindow)
		return -1;

	if (!this->m_Renderer)
		return -1;

	if (!this->m_RenderWindowInteractor)
		return -1;

	this->m_RenderWindow->AddRenderer(this->m_Renderer);
	// setup the parent window
	this->m_RenderWindow->SetParentId(this->m_hWnd);
	this->m_RenderWindowInteractor->SetRenderWindow(this->m_RenderWindow);


	return 0;
}

void CTestVTKDialog::OnSize(UINT nType, int cx, int cy)
{
	ETSLayoutDialog::OnSize(nType, cx, cy);
// COMMENT: {2/3/2004 8:36:21 PM}	::SendMessage(this->m_hWndStatusBar, WM_SIZE, 0, 0);
// COMMENT: {2/3/2004 8:36:21 PM}	//this->RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
// COMMENT: {2/3/2004 8:36:21 PM}
// COMMENT: {2/3/2004 8:36:21 PM}
// COMMENT: {2/3/2004 8:36:21 PM}	HDWP hDWP = ::BeginDeferWindowPos(8);
// COMMENT: {2/3/2004 8:36:21 PM}
// COMMENT: {2/3/2004 8:36:21 PM}	// Add your message handler code here
// COMMENT: {2/3/2004 8:36:21 PM}	CRect rcClient, rcVTK; // rect
// COMMENT: {2/3/2004 8:36:21 PM}	this->GetClientRect(&rcClient);
// COMMENT: {2/3/2004 8:36:21 PM}	ASSERT(rcClient.Width() == cx);
// COMMENT: {2/3/2004 8:36:21 PM}	ASSERT(rcClient.Height() == cy);
// COMMENT: {2/3/2004 8:36:21 PM}
// COMMENT: {2/3/2004 8:36:21 PM}	rcVTK = rcClient;
// COMMENT: {2/3/2004 8:36:21 PM}	const int BORDER = 10;
// COMMENT: {2/3/2004 8:36:21 PM}
// COMMENT: {2/3/2004 8:36:21 PM}	if (this->m_hWnd) {
// COMMENT: {2/3/2004 8:36:21 PM}		if (HWND hOK = ::GetDlgItem(this->m_hWnd, IDOK)) {
// COMMENT: {2/3/2004 8:36:21 PM}			CRect rc;
// COMMENT: {2/3/2004 8:36:21 PM}			::GetClientRect(hOK, &rc);
// COMMENT: {2/3/2004 8:36:21 PM}			::MapWindowPoints(hOK, this->m_hWnd, (LPPOINT)&rc, 2);
// COMMENT: {2/3/2004 8:36:21 PM}			if (hDWP) {
// COMMENT: {2/3/2004 8:36:21 PM}				hDWP = ::DeferWindowPos(hDWP, hOK, NULL, rcClient.Width() - rc.Width() - BORDER, rc.top, 0, 0, SWP_NOSIZE|SWP_NOOWNERZORDER);
// COMMENT: {2/3/2004 8:36:21 PM}			}
// COMMENT: {2/3/2004 8:36:21 PM}			else {
// COMMENT: {2/3/2004 8:36:21 PM}				::SetWindowPos(hOK, NULL, rcClient.Width() - rc.Width() - BORDER, rc.top, 0, 0, SWP_NOSIZE|SWP_NOOWNERZORDER);
// COMMENT: {2/3/2004 8:36:21 PM}			}
// COMMENT: {2/3/2004 8:36:21 PM}			rcVTK.DeflateRect(0, 0, rc.Width() + 2 * BORDER , 0);
// COMMENT: {2/3/2004 8:36:21 PM}		}
// COMMENT: {2/3/2004 8:36:21 PM}		if (HWND hCancel = ::GetDlgItem(this->m_hWnd, IDCANCEL)) {
// COMMENT: {2/3/2004 8:36:21 PM}			CRect rc;
// COMMENT: {2/3/2004 8:36:21 PM}			::GetClientRect(hCancel, &rc);
// COMMENT: {2/3/2004 8:36:21 PM}			::MapWindowPoints(hCancel, this->m_hWnd, (LPPOINT)&rc, 2);
// COMMENT: {2/3/2004 8:36:21 PM}			if (hDWP) {
// COMMENT: {2/3/2004 8:36:21 PM}				hDWP = ::DeferWindowPos(hDWP, hCancel, NULL, rcClient.Width() - rc.Width() - BORDER, rc.top, 0, 0, SWP_NOSIZE|SWP_NOOWNERZORDER);
// COMMENT: {2/3/2004 8:36:21 PM}			}
// COMMENT: {2/3/2004 8:36:21 PM}			else {
// COMMENT: {2/3/2004 8:36:21 PM}				::SetWindowPos(hCancel, NULL, rcClient.Width() - rc.Width() - BORDER, rc.top, 0, 0, SWP_NOSIZE|SWP_NOOWNERZORDER);
// COMMENT: {2/3/2004 8:36:21 PM}			}
// COMMENT: {2/3/2004 8:36:21 PM}		}
// COMMENT: {2/3/2004 8:36:21 PM}	}
// COMMENT: {2/3/2004 8:36:21 PM}// COMMENT: {2/2/2004 8:27:00 PM}	if ( hwndStatusBar ) {
// COMMENT: {2/3/2004 8:36:21 PM}// COMMENT: {2/2/2004 8:27:00 PM}		CRect rcSB;
// COMMENT: {2/3/2004 8:36:21 PM}// COMMENT: {2/2/2004 8:27:00 PM}		::GetWindowRect(hwndStatusBar, &rcSB);
// COMMENT: {2/3/2004 8:36:21 PM}// COMMENT: {2/2/2004 8:27:00 PM}		rcVTK.DeflateRect(0, 0, 0, rcSB.Height());
// COMMENT: {2/3/2004 8:36:21 PM}// COMMENT: {2/2/2004 8:27:00 PM}	}
// COMMENT: {2/3/2004 8:36:21 PM}	if (HWND hDIV = ::GetDlgItem(this->m_hWnd, IDC_BOTTOM)) {
// COMMENT: {2/3/2004 8:36:21 PM}		CRect rc;
// COMMENT: {2/3/2004 8:36:21 PM}		::GetClientRect(hDIV, &rc);
// COMMENT: {2/3/2004 8:36:21 PM}		afxDump << "hDiv client = " << rc << "\n";
// COMMENT: {2/3/2004 8:36:21 PM}		::MapWindowPoints(hDIV, this->m_hWnd, (LPPOINT)&rc, 2);
// COMMENT: {2/3/2004 8:36:21 PM}
// COMMENT: {2/3/2004 8:36:21 PM}		CRect rcSB;
// COMMENT: {2/3/2004 8:36:21 PM}		::GetWindowRect(this->m_hWndStatusBar, &rcSB);
// COMMENT: {2/3/2004 8:36:21 PM}		rcVTK.DeflateRect(0, 0, 0, rcSB.Height());
// COMMENT: {2/3/2004 8:36:21 PM}		rcVTK.DeflateRect(0, 0, 0, max(rcClient.Height() - rc.top, rcSB.Height()));
// COMMENT: {2/3/2004 8:36:21 PM}		afxDump << "hDiv map = " << rc << "\n";
// COMMENT: {2/3/2004 8:36:21 PM}
// COMMENT: {2/3/2004 8:36:21 PM}		::SetWindowPos(hDIV, NULL, 0, rc.top, cx - 3, 0, SWP_NOZORDER);
// COMMENT: {2/3/2004 8:36:21 PM}// COMMENT: {2/3/2004 4:06:30 PM}		if (hDWP) {
// COMMENT: {2/3/2004 8:36:21 PM}// COMMENT: {2/3/2004 4:06:30 PM}			hDWP = ::DeferWindowPos(hDWP, hDIV, NULL, 0, rc.top, cx - 3, 0, SWP_NOOWNERZORDER);
// COMMENT: {2/3/2004 8:36:21 PM}// COMMENT: {2/3/2004 4:06:30 PM}		}
// COMMENT: {2/3/2004 8:36:21 PM}// COMMENT: {2/3/2004 4:06:30 PM}		else {
// COMMENT: {2/3/2004 8:36:21 PM}// COMMENT: {2/3/2004 4:06:30 PM}			::SetWindowPos(hDIV, NULL, 0, rc.top, cx - 3, 0, SWP_NOOWNERZORDER);
// COMMENT: {2/3/2004 8:36:21 PM}// COMMENT: {2/3/2004 4:06:30 PM}		}
// COMMENT: {2/3/2004 8:36:21 PM}	}
// COMMENT: {2/3/2004 8:36:21 PM}
// COMMENT: {2/3/2004 8:36:21 PM}	HWND hVTK = this->m_RenderWindow->GetWindowId();
// COMMENT: {2/3/2004 8:36:21 PM}	if (hDWP) {
// COMMENT: {2/3/2004 8:36:21 PM}		hDWP = ::DeferWindowPos(hDWP, hVTK, NULL, rcVTK.left, rcVTK.top, rcVTK.Width(), rcVTK.Height(), SWP_NOOWNERZORDER);
// COMMENT: {2/3/2004 8:36:21 PM}	}
// COMMENT: {2/3/2004 8:36:21 PM}	else {
// COMMENT: {2/3/2004 8:36:21 PM}		::SetWindowPos(hVTK, NULL, rcVTK.left, rcVTK.top, rcVTK.Width(), rcVTK.Height(), SWP_NOOWNERZORDER);	
// COMMENT: {2/3/2004 8:36:21 PM}	}
// COMMENT: {2/3/2004 8:36:21 PM}
// COMMENT: {2/3/2004 8:36:21 PM}	if (hDWP) {
// COMMENT: {2/3/2004 8:36:21 PM}		::EndDeferWindowPos(hDWP);
// COMMENT: {2/3/2004 8:36:21 PM}	}
}

void CTestVTKDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

// COMMENT: {2/3/2004 8:44:56 PM}	if ( !this->m_RenderWindowInteractor->GetInitialized() )
// COMMENT: {2/3/2004 8:44:56 PM}	{
// COMMENT: {2/3/2004 8:44:56 PM}		CRect rect;
// COMMENT: {2/3/2004 8:44:56 PM}		this->GetClientRect(&rect);
// COMMENT: {2/3/2004 8:44:56 PM}		rect.DeflateRect(0, 0, 90, 0);
// COMMENT: {2/3/2004 8:44:56 PM}		this->m_RenderWindowInteractor->Initialize();
// COMMENT: {2/3/2004 8:44:56 PM}		this->m_RenderWindow->SetSize(rect.right-rect.left,rect.bottom-rect.top);
// COMMENT: {2/3/2004 8:44:56 PM}		this->m_Renderer->SetBackground(1, 0.0, 0.0);
// COMMENT: {2/3/2004 8:44:56 PM}		this->m_Renderer->ResetCamera();
// COMMENT: {2/3/2004 8:44:56 PM}
// COMMENT: {2/3/2004 8:44:56 PM}// COMMENT: {2/3/2004 2:54:19 PM}		hwndStatusBar = CreateStatusWindow(WS_CHILD|WS_VISIBLE, "", this->m_hWnd, 105);
// COMMENT: {2/3/2004 8:44:56 PM}// COMMENT: {2/3/2004 2:54:19 PM}		ASSERT(hwndStatusBar);
// COMMENT: {2/3/2004 8:44:56 PM}	}

}

BOOL CTestVTKDialog::OnEraseBkgnd(CDC* pDC)
{
// COMMENT: {2/4/2004 9:20:08 PM}	// Add your message handler code here and/or call default
// COMMENT: {2/4/2004 9:20:08 PM}	//
// COMMENT: {2/4/2004 9:20:08 PM}	if (HWND hVTK = this->m_RenderWindow->GetWindowId()) {
// COMMENT: {2/4/2004 9:20:08 PM}		CRect rc;
// COMMENT: {2/4/2004 9:20:08 PM}		::GetClientRect(hVTK, &rc);
// COMMENT: {2/4/2004 9:20:08 PM}		::MapWindowPoints(hVTK, this->m_hWnd, (LPPOINT)&rc, 2);
// COMMENT: {2/4/2004 9:20:08 PM}		pDC->ExcludeClipRect(&rc);
// COMMENT: {2/4/2004 9:20:08 PM}	}
	return ETSLayoutDialog::OnEraseBkgnd(pDC);
}

LRESULT CTestVTKDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
// COMMENT: {2/2/2004 4:27:23 PM}	switch (message)
// COMMENT: {2/2/2004 4:27:23 PM}	{
// COMMENT: {2/2/2004 4:27:23 PM}	//case WM_PAINT: 
// COMMENT: {2/2/2004 4:27:23 PM}	case WM_LBUTTONDOWN: 
// COMMENT: {2/2/2004 4:27:23 PM}	case WM_LBUTTONUP: 
// COMMENT: {2/2/2004 4:27:23 PM}	case WM_MBUTTONDOWN: 
// COMMENT: {2/2/2004 4:27:23 PM}	case WM_MBUTTONUP: 
// COMMENT: {2/2/2004 4:27:23 PM}	case WM_RBUTTONDOWN: 
// COMMENT: {2/2/2004 4:27:23 PM}	case WM_RBUTTONUP: 
// COMMENT: {2/2/2004 4:27:23 PM}	case WM_MOUSEMOVE:
// COMMENT: {2/2/2004 4:27:23 PM}	case WM_CHAR:
// COMMENT: {2/2/2004 4:27:23 PM}	case WM_TIMER:
// COMMENT: {2/2/2004 4:27:23 PM}    case WM_KEYDOWN:  // vtkCommand::KeyPressEvent:
// COMMENT: {2/2/2004 4:27:23 PM}    case WM_KEYUP:    // vtkCommand::KeyReleaseEvent
// COMMENT: {2/2/2004 4:27:23 PM}
// COMMENT: {2/2/2004 4:27:23 PM}		if (this->m_RenderWindowInteractor->GetInitialized())
// COMMENT: {2/2/2004 4:27:23 PM}		{
// COMMENT: {2/2/2004 4:27:23 PM}			return vtkHandleMessage2(this->m_hWnd, message, wParam, lParam, this->m_RenderWindowInteractor);
// COMMENT: {2/2/2004 4:27:23 PM}		}
// COMMENT: {2/2/2004 4:27:23 PM}		break;
// COMMENT: {2/2/2004 4:27:23 PM}	}

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CTestVTKDialog::PreTranslateMessage(MSG* pMsg)
{
	// Add your specialized code here and/or call the base class
// COMMENT: {2/2/2004 4:44:31 PM}	switch (pMsg->message)
// COMMENT: {2/2/2004 4:44:31 PM}	{
// COMMENT: {2/2/2004 4:44:31 PM}	//case WM_PAINT: 
// COMMENT: {2/2/2004 4:44:31 PM}	case WM_LBUTTONDOWN: 
// COMMENT: {2/2/2004 4:44:31 PM}	case WM_LBUTTONUP: 
// COMMENT: {2/2/2004 4:44:31 PM}	case WM_MBUTTONDOWN: 
// COMMENT: {2/2/2004 4:44:31 PM}	case WM_MBUTTONUP: 
// COMMENT: {2/2/2004 4:44:31 PM}	case WM_RBUTTONDOWN: 
// COMMENT: {2/2/2004 4:44:31 PM}	case WM_RBUTTONUP: 
// COMMENT: {2/2/2004 4:44:31 PM}	case WM_MOUSEMOVE:
// COMMENT: {2/2/2004 4:44:31 PM}	case WM_CHAR:
// COMMENT: {2/2/2004 4:44:31 PM}	case WM_TIMER:
// COMMENT: {2/2/2004 4:44:31 PM}    case WM_KEYDOWN:  // vtkCommand::KeyPressEvent:
// COMMENT: {2/2/2004 4:44:31 PM}    case WM_KEYUP:    // vtkCommand::KeyReleaseEvent
// COMMENT: {2/2/2004 4:44:31 PM}
// COMMENT: {2/2/2004 4:44:31 PM}		if (this->m_RenderWindowInteractor->GetInitialized())
// COMMENT: {2/2/2004 4:44:31 PM}		{
// COMMENT: {2/2/2004 4:44:31 PM}			return vtkHandleMessage2(this->m_hWnd, pMsg->message, pMsg->wParam, pMsg->lParam, this->m_RenderWindowInteractor);
// COMMENT: {2/2/2004 4:44:31 PM}		}
// COMMENT: {2/2/2004 4:44:31 PM}		break;
// COMMENT: {2/2/2004 4:44:31 PM}	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CTestVTKDialog::OnInitDialog()
{
	ETSLayoutDialog::OnInitDialog();

// COMMENT: {2/4/2004 9:21:25 PM}	///{{{
// COMMENT: {2/4/2004 9:21:25 PM}	//////if ( !this->m_RenderWindowInteractor->GetInitialized() )
// COMMENT: {2/4/2004 9:21:25 PM}	{
// COMMENT: {2/4/2004 9:21:25 PM}		CRect rect;
// COMMENT: {2/4/2004 9:21:25 PM}		this->GetClientRect(&rect);
// COMMENT: {2/4/2004 9:21:25 PM}		rect.DeflateRect(0, 0, 90, 0);
		this->m_RenderWindowInteractor->Initialize();
// COMMENT: {2/4/2004 9:21:25 PM}		this->m_RenderWindow->SetSize(rect.right-rect.left,rect.bottom-rect.top);
// COMMENT: {2/4/2004 9:21:25 PM}		/////this->m_Renderer->SetBackground(1, 0.0, 0.0);
		this->m_Renderer->ResetCamera();
// COMMENT: {2/4/2004 9:21:25 PM}	}
// COMMENT: {2/4/2004 9:21:25 PM}
// COMMENT: {2/4/2004 9:21:25 PM}	///}}}


// COMMENT: {2/3/2004 9:27:48 PM}	// Possibility 1
// COMMENT: {2/3/2004 9:27:48 PM}	CreateRoot(VERTICAL)
// COMMENT: {2/3/2004 9:27:48 PM}		<< item ( IDC_NEW_ITEM_STATIC, NORESIZE )
// COMMENT: {2/3/2004 9:27:48 PM}
// COMMENT: {2/3/2004 9:27:48 PM}		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT )
// COMMENT: {2/3/2004 9:27:48 PM}			<< item( IDC_NEW_ITEM, GREEDY )
// COMMENT: {2/3/2004 9:27:48 PM}			<< item( IDC_ADD_ITEM, NORESIZE )
// COMMENT: {2/3/2004 9:27:48 PM}			)
// COMMENT: {2/3/2004 9:27:48 PM}
// COMMENT: {2/3/2004 9:27:48 PM}		<< item ( IDC_ITEM_LIST_STATIC, NORESIZE )
// COMMENT: {2/3/2004 9:27:48 PM}		<< item ( IDC_ITEM_LIST, GREEDY )
// COMMENT: {2/3/2004 9:27:48 PM}	
// COMMENT: {2/3/2004 9:27:48 PM}		<<	( pane(HORIZONTAL, ABSOLUTE_VERT )
// COMMENT: {2/3/2004 9:27:48 PM}			<< item( IDOK, RELATIVE_HORZ, 70 )
// COMMENT: {2/3/2004 9:27:48 PM}			<< item( IDCANCEL, RELATIVE_HORZ, 30 )
// COMMENT: {2/3/2004 9:27:48 PM}			);


	CWnd* pWnd = CWnd::FromHandle(this->m_RenderWindow->GetWindowId());
	ASSERT(pWnd);

	CreateRoot(VERTICAL, 0, 0) 
		
		<< 	( pane(HORIZONTAL, GREEDY, 7, 7 )

			<< item ( pWnd, GREEDY )

			<< 	( pane(VERTICAL, ABSOLUTE_HORZ, 3, 3 )
				<< item( IDC_STATIC_MODEL_ORIGIN, NORESIZE )

				<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
					<< item( IDC_STATIC_MO_X, NORESIZE | ALIGN_VCENTER )
					<< item( IDC_EDIT_MO_X, NORESIZE )
					)

				<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
					<< item( IDC_STATIC_MO_Y, NORESIZE | ALIGN_VCENTER )
					<< item( IDC_EDIT_MO_Y, NORESIZE )
					)

				<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
					<< item( IDC_STATIC_MO_ANGLE, NORESIZE | ALIGN_VCENTER )
					<< item( IDC_EDIT_MO_ANGLE, NORESIZE )
					)

				)
			)

		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 7, 7, 0 )
			<< itemGrowing(HORIZONTAL)
			<< item( IDOK, NORESIZE )
			<< item( IDCANCEL, NORESIZE )
			)

		;


// COMMENT: {2/3/2004 9:28:03 PM}	CreateRoot(HORIZONTAL, 0, 0) 
// COMMENT: {2/3/2004 9:28:03 PM}		
// COMMENT: {2/3/2004 9:28:03 PM}		<< item ( pWnd, GREEDY )
// COMMENT: {2/3/2004 9:28:03 PM}
// COMMENT: {2/3/2004 9:28:03 PM}		<< 	( pane(VERTICAL, ABSOLUTE_HORZ, 7, 7 )
// COMMENT: {2/3/2004 9:28:03 PM}			<< item( IDOK, NORESIZE )
// COMMENT: {2/3/2004 9:28:03 PM}			<< item( IDCANCEL, NORESIZE )
// COMMENT: {2/3/2004 9:28:03 PM}			)
// COMMENT: {2/3/2004 9:28:03 PM}
// COMMENT: {2/3/2004 9:28:03 PM}		;

	UpdateLayout();



// COMMENT: {2/3/2004 8:41:02 PM}	// Add extra initialization here
// COMMENT: {2/3/2004 8:41:02 PM}	ASSERT(this->m_hWnd);
// COMMENT: {2/3/2004 8:41:02 PM}
// COMMENT: {2/3/2004 8:41:02 PM}// COMMENT: {2/3/2004 7:26:20 PM}	this->m_hWndStatusBar = ::CreateStatusWindow(WS_CHILD|WS_VISIBLE, "", this->m_hWnd, 105);
// COMMENT: {2/3/2004 8:41:02 PM}// COMMENT: {2/3/2004 7:26:20 PM}	ASSERT(this->m_hWndStatusBar);
// COMMENT: {2/3/2004 8:41:02 PM}
// COMMENT: {2/3/2004 8:41:02 PM}	if (statusBar.Create(this)) {                           
// COMMENT: {2/3/2004 8:41:02 PM}		this->RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
// COMMENT: {2/3/2004 8:41:02 PM}		this->m_hWndStatusBar = statusBar.m_hWnd;
// COMMENT: {2/3/2004 8:41:02 PM}	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
