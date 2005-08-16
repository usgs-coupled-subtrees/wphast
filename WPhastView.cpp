// WPhastView.cpp : implementation of the CWPhastView class
//

#include "stdafx.h"
#include "WPhast.h"

#include "WPhastDoc.h"
#include "WPhastView.h"
#include "PropertyTreeControlBar.h"

#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkWin32RenderWindowInteractor.h>

#include <vtkBoxWidget.h>
#include <vtkPointWidget.h>
#include "vtkPointWidget2.h"

#include <vtkAbstractPropPicker.h>
#include "BoxPropertiesDialogBar.h"


#include "ViewVTKCommand.h"

#include <vtkInteractorStyleSwitch.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include "vtkInteractorStyleTrackballCameraEx.h"

#include <vtkPropPicker.h>
#include "vtkPropPickerFixed.h"


#include <vtkCamera.h>

#include <vtkCursor3D.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

#include <vtkCubeSource.h>
#include <vtkProperty.h>
#include <vtkCylinderSource.h>

#include <vtkPropAssembly.h>
#include <vtkPropCollection.h>

#include "ZoneActor.h"
#include "WellActor.h"
#include "RiverActor.h"
#include "GridLODActor.h"
#include "GridActor.h"
#include "RiverCreateAction.h"
#include "RiverPropertyPage2.h"
#include "Grid.h"
#include "FlowOnly.h"


// #include "CreateZoneAction.h"
#include "ZoneCreateAction.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWPhastView

IMPLEMENT_DYNCREATE(CWPhastView, CView)

BEGIN_MESSAGE_MAP(CWPhastView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWZONE, OnUpdateToolsNewZone)
	ON_COMMAND(ID_TOOLS_NEWZONE, OnToolsNewZone)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FROM_NX, OnUpdateViewFromNx)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FROM_NY, OnUpdateViewFromNy)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FROM_NZ, OnUpdateViewFromNz)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FROM_PX, OnUpdateViewFromPx)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FROM_PY, OnUpdateViewFromPy)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FROM_PZ, OnUpdateViewFromPz)
	ON_COMMAND(ID_VIEW_FROM_NX, OnViewFromNx)
	ON_COMMAND(ID_VIEW_FROM_NY, OnViewFromNy)
	ON_COMMAND(ID_VIEW_FROM_NZ, OnViewFromNz)
	ON_COMMAND(ID_VIEW_FROM_PX, OnViewFromPx)
	ON_COMMAND(ID_VIEW_FROM_PY, OnViewFromPy)
	ON_COMMAND(ID_VIEW_FROM_PZ, OnViewFromPz)
	ON_COMMAND(ID_VIEW_FROM_NEXT_DIRECTION, OnViewFromNextDirection)
	ON_COMMAND(ID_TOOLS_NEWWELL, OnToolsNewWell)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWWELL, OnUpdateToolsNewWell)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWRIVER, OnUpdateToolsNewRiver)
	ON_COMMAND(ID_TOOLS_NEWRIVER, OnToolsNewRiver)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_COMMAND(ID_TOOLS_MOVE_VER_LINE, OnToolsMoveVerLine)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MOVE_VER_LINE, OnUpdateToolsMoveVerLine)
END_MESSAGE_MAP()

// CWPhastView construction/destruction

CWPhastView::CWPhastView()
: m_bResetCamera(false)
, m_pNewCube(0)
, m_pNewCubeMapper(0)
, m_pNewCubeActor(0)
, m_ViewFromDirection(ID_VIEW_FROM_PZ)
, m_pWellActor(0)
, m_pPointWidget2(0)
, m_pRiverActor(0)
, RiverCallbackCommand(0)
, m_bMovingGridLine(false)
{
	// Create the renderer, window and interactor objects.
	//
	this->m_RenderWindow           = vtkWin32OpenGLRenderWindow::New();
	this->m_Renderer               = vtkRenderer::New();
	this->m_RenderWindowInteractor = vtkWin32RenderWindowInteractor::New();

	// Create the the BoxWidget
	//
	this->m_pBoxWidget = vtkBoxWidget::New();
	this->m_pBoxWidget->SetInteractor(this->m_RenderWindowInteractor);
	this->m_pBoxWidget->SetPlaceFactor(1.0);
	this->m_pBoxWidget->RotationEnabledOff();

	// Create the the PointWidget2
	//
	this->m_pPointWidget2 = vtkPointWidget2::New();
	this->m_pPointWidget2->SetInteractor(this->m_RenderWindowInteractor);
	///this->m_pPointWidget2->TranslationModeOn();

	//
	this->m_pInteractorStyle = vtkInteractorStyleTrackballCameraEx::New();
	this->m_RenderWindowInteractor->SetInteractorStyle(this->m_pInteractorStyle);

	// picker
	vtkPropPickerFixed* picker = vtkPropPickerFixed::New();
	this->m_RenderWindowInteractor->SetPicker(picker);
	picker->Delete();

	// 3D Cursor
	//
	this->m_pCursor3D = vtkCursor3D::New();
	this->m_pCursor3D->XShadowsOff();
	this->m_pCursor3D->YShadowsOff();
	this->m_pCursor3D->ZShadowsOff();
	this->m_pCursor3D->OutlineOff();

	this->m_pCursor3DMapper = vtkPolyDataMapper::New();
	this->m_pCursor3DMapper->SetInput(this->m_pCursor3D->GetOutput());
	
	this->m_pCursor3DActor = vtkActor::New();
	this->m_pCursor3DActor->SetMapper(this->m_pCursor3DMapper);
	this->m_pCursor3DActor->SetPosition(0, 0, 0);
	this->m_pCursor3DActor->VisibilityOff();

	// Add Observers
	//
	this->m_pViewVTKCommand = CViewVTKCommand::New(this);
	this->m_RenderWindowInteractor->AddObserver(vtkCommand::EndPickEvent, this->m_pViewVTKCommand);

	this->m_pBoxWidget->AddObserver(vtkCommand::InteractionEvent, this->m_pViewVTKCommand);
	this->m_pBoxWidget->AddObserver(vtkCommand::EndInteractionEvent, this->m_pViewVTKCommand);
	this->m_pBoxWidget->AddObserver(vtkCommand::StartInteractionEvent, this->m_pViewVTKCommand);

	this->m_pPointWidget2->AddObserver(vtkCommand::InteractionEvent, this->m_pViewVTKCommand);
	this->m_pPointWidget2->AddObserver(vtkCommand::EndInteractionEvent, this->m_pViewVTKCommand);
	this->m_pPointWidget2->AddObserver(vtkCommand::StartInteractionEvent, this->m_pViewVTKCommand);


	// camera events
	//
	vtkCamera* pCamera = this->m_Renderer->GetActiveCamera();
	ASSERT(pCamera);
	pCamera->AddObserver(vtkCommand::ModifiedEvent, this->m_pViewVTKCommand);

	// mouse events
	//
	this->m_RenderWindowInteractor->AddObserver(vtkCommand::MouseMoveEvent, this->m_pViewVTKCommand);
	this->m_RenderWindowInteractor->AddObserver(vtkCommand::LeftButtonPressEvent, this->m_pViewVTKCommand);
	this->m_RenderWindowInteractor->AddObserver(vtkCommand::LeftButtonReleaseEvent, this->m_pViewVTKCommand);

	// 	keyboard events
	//
	this->m_RenderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, this->m_pViewVTKCommand);

}

CWPhastView::~CWPhastView()
{
	this->m_pInteractorStyle->Delete();
	this->m_pBoxWidget->Delete();
	this->m_pPointWidget2->Delete();

	this->m_pViewVTKCommand->Delete();

	// 3D Cursor
	this->m_pCursor3D->Delete();
	this->m_pCursor3DMapper->Delete();
	this->m_pCursor3DActor->Delete();


	// Well actor
	if (this->m_pWellActor)
	{
		this->m_pWellActor->Delete();
		this->m_pWellActor = 0;
	}

	// River actor
	if (this->m_pRiverActor)
	{
		this->m_Renderer->RemoveProp(this->m_pRiverActor);
		this->m_pRiverActor->Delete();
		this->m_pRiverActor = 0;
	}
	// River listener
	if (this->RiverCallbackCommand)
	{
		this->RiverCallbackCommand->Delete();
		this->RiverCallbackCommand = 0;
	}

	if (this->m_bMovingGridLine)
	{
		if (CGridLODActor* pGridLODActor = CGridLODActor::SafeDownCast(this->GetDocument()->GetGridActor()))
		{
			pGridLODActor->SetEnabled(0);
		}
	}

	if (this->CreatingNewZone())
	{
		this->EndNewZone();
	}


	// Delete the the renderer, window and interactor objects.
	//
	this->m_RenderWindowInteractor->SetRenderWindow(NULL);
	this->m_Renderer->Delete();
	this->m_RenderWindowInteractor->Delete();
	this->m_RenderWindow->Delete();
}

BOOL CWPhastView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CWPhastView drawing

void CWPhastView::OnDraw(CDC* pDC)
{
	TRACE("CWPhastView::OnDraw\n");
	CWPhastDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if ( !this->m_RenderWindowInteractor->GetInitialized() )
	{
		CRect rect;

		this->GetClientRect(&rect);
		this->m_RenderWindowInteractor->Initialize();
		this->m_RenderWindow->SetSize(rect.right-rect.left,rect.bottom-rect.top);

		this->m_Renderer->ResetCamera();
	}

	// Invoke the pipeline
	Pipeline();

	if ( pDC->IsPrinting() )
	{
		this->BeginWaitCursor();

		// Obtain the size of the printer page in pixels.
		int cxPage = pDC->GetDeviceCaps(HORZRES);
		int cyPage = pDC->GetDeviceCaps(VERTRES);

		// Get the size of the window in pixels.
		int *size = this->m_RenderWindow->GetSize();
		int cxWindow = size[0];
		int cyWindow = size[1];
		float fx = float(cxPage) / float(cxWindow);
		float fy = float(cyPage) / float(cyWindow);
		float scale = min(fx,fy);
		int x = int(scale * float(cxWindow));
		int y = int(scale * float(cyWindow));
		this->m_RenderWindow->SetupMemoryRendering(cxWindow, cyWindow, pDC->GetSafeHdc());
		this->m_RenderWindow->Render();
		HDC memDC = this->m_RenderWindow->GetMemoryDC();
		StretchBlt(pDC->GetSafeHdc(),0,0,x,y,memDC,0,0,cxWindow,cyWindow,SRCCOPY);
		this->m_RenderWindow->ResumeScreenRendering();

		this->EndWaitCursor();
	}
	else
	{
		this->m_RenderWindow->Render();
	}
}


// CWPhastView printing

BOOL CWPhastView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWPhastView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWPhastView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CWPhastView diagnostics

#ifdef _DEBUG
void CWPhastView::AssertValid() const
{
	CView::AssertValid();
}

void CWPhastView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWPhastDoc* CWPhastView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWPhastDoc)));
	return (CWPhastDoc*)m_pDocument;
}
#endif //_DEBUG


// CWPhastView message handlers

int CWPhastView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!this->m_RenderWindow)
		return -1;

	if (!this->m_Renderer)
		return -1;

	if (!this->m_RenderWindowInteractor)
		return -1;

	this->m_RenderWindow->AddRenderer(this->m_Renderer);
#if ((VTK_MAJOR_VERSION <= 4) && (VTK_MINOR_VERSION <= 2))
	// Note: No double-click events will be received
	//
	this->m_RenderWindow->SetParentId(this->GetSafeHwnd());
#else
	this->m_RenderWindow->SetWindowId(this->GetSafeHwnd());
#endif
	this->m_RenderWindowInteractor->SetRenderWindow(this->m_RenderWindow);


	CCreateContext *pContext = static_cast<CCreateContext*>(lpCreateStruct->lpCreateParams);
	ASSERT(pContext);
	CWPhastDoc *pDoc = static_cast<CWPhastDoc*>(pContext->m_pCurrentDoc);
	ASSERT_VALID(pDoc);
	pDoc->Attach(this);

	return 0;
}

void CWPhastView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

#if ((VTK_MAJOR_VERSION <= 4) && (VTK_MINOR_VERSION <= 2))
	CRect rect;
	this->GetClientRect(&rect);
	this->m_RenderWindow->SetSize(rect.right-rect.left,rect.bottom-rect.top);
#endif
}

BOOL CWPhastView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

LRESULT CWPhastView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	//case WM_PAINT: 
	case WM_LBUTTONDOWN: 
	case WM_LBUTTONUP: 
	case WM_MBUTTONDOWN: 
	case WM_MBUTTONUP: 
	case WM_RBUTTONDOWN: 
	case WM_RBUTTONUP: 
	case WM_MOUSEMOVE:
	case WM_CHAR:
	case WM_TIMER:
    case WM_KEYDOWN:  // vtkCommand::KeyPressEvent:
    case WM_KEYUP:    // vtkCommand::KeyReleaseEvent

		if (this->m_RenderWindowInteractor->GetInitialized())
		{
			if (message == WM_CHAR)
			{
				switch (wParam)
				{
				case 'Q' :
				case 'q' :
				case 'e' :
				case 'E' :
					// don't allow vtkHandleMessage2 call ExitCallback
					return CView::WindowProc(message, wParam, lParam);
					break;
				default  :
					break;
				}
			}
			return vtkHandleMessage2(this->m_hWnd, message, wParam, lParam, this->m_RenderWindowInteractor);
		}
		break;
	}

	return CView::WindowProc(message, wParam, lParam);
}

void CWPhastView::Pipeline()
{
	// set background color
	ASSERT(this->m_Renderer);
	this->m_Renderer->SetBackground(0.0, 0.0, 0.0);

	/* TODO:
	if (this->m_pNewCubeActor && this->m_pNewCubeActor->GetVisibility()) {
		this->m_Renderer->AddProp(this->m_pNewCubeActor);
	}
	*/

	//{{
	// this->m_Renderer->RemoveAllProps(); // this removes the selection actor and the BoxWidget
	//}}

	this->m_Renderer->AddProp(this->m_pCursor3DActor);

	if (this->m_pNewCubeActor) {
		this->m_Renderer->AddProp(this->m_pNewCubeActor);
	}


	// add props to renderer
	if ( vtkPropCollection *props = this->GetDocument()->GetPropCollection() ) {
		props->InitTraversal();
		for (int i = 0; i < props->GetNumberOfItems(); ++i) {
			vtkProp* prop = props->GetNextProp();
			this->m_Renderer->AddProp(prop);

			if (this->m_bResetCamera) {
				this->m_Renderer->ResetCamera();
				this->m_bResetCamera = false;
			}

			/* TODO:
			if (vtkCubeAxesActor2D *pCubeAxisActor2D = vtkCubeAxesActor2D::SafeDownCast(prop)) {
				pCubeAxisActor2D->SetCamera(this->m_Renderer->GetActiveCamera());
			}
			*/
		}
	}
	//{{
	this->m_Renderer->ResetCameraClippingRange();
	//}}
}


void CWPhastView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	//{{FIXME
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	return;

	// Note: The code below causes the Zone dimensions to be disabled
	// when the hardware pick fails

	//}}FIXME

	// TODO: Add your specialized code here and/or call the base class
	TRACE("CWPhastView::OnActivateView\n");
	//{{
	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( this->GetRenderWindowInteractor()->GetPicker() )) {
		if (vtkProp3D* prop = picker->GetProp3D()) {
			// Update StatusBar
			//
			if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
				vtkFloatingPointType* bounds = prop->GetBounds();
				TCHAR buffer[80];
				// ::_sntprintf(buffer, 80, "%6.4f x %6.4f x %6.4f", fabs(bounds[1] - bounds[0]), fabs(bounds[3] - bounds[2]), fabs(bounds[5] - bounds[4]));
				::_sntprintf(buffer, 80, "%6.4g x %6.4g x %6.4g", fabs(bounds[1] - bounds[0]), fabs(bounds[3] - bounds[2]), fabs(bounds[5] - bounds[4]));

				// FIXME: This will cause a buffer overrun if the transform is invalid
				// FIXME: WHY ARE THE BOUNDS OUT OF RANGE???
				// ::_stprintf(buffer, "%6.4f x %6.4f x %6.4f", fabs(bounds[1] - bounds[0]), fabs(bounds[3] - bounds[2]), fabs(bounds[5] - bounds[4]));
				//
				pWnd->SetWindowText(buffer);

			}

			////// Update BoxPropertiesDialogBar
			//////
			////if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) ) {
			////	pBar->Set(this, prop);
			////}
		}
		else {
			// Update StatusBar
			//
			if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
				CString status;
				status.LoadString(AFX_IDS_IDLEMESSAGE);
				pWnd->SetWindowText(status);
			}

			// Update BoxPropertiesDialogBar
			//
			if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS))) {
				pBar->Set(NULL, NULL, this->GetDocument()->GetUnits());
			}
		}
	}
	//}}

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CWPhastView::ResetCamera(void)
{
	///this->m_Renderer->ResetCamera();
	///this->m_RenderWindowInteractor->Render();
	this->m_bResetCamera = true;
}

void CWPhastView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

// COMMENT: {8/17/2004 8:34:13 PM}	CWPhastDoc *pDoc = this->GetDocument();
// COMMENT: {8/17/2004 8:34:13 PM}	ASSERT(pDoc);
// COMMENT: {8/17/2004 8:34:13 PM}	ASSERT(pDoc->GetPropertyTreeControlBar());
// COMMENT: {8/17/2004 8:34:13 PM}	ASSERT(pDoc->GetBoxPropertiesDialogBar());
// COMMENT: {8/17/2004 8:34:13 PM}
// COMMENT: {8/17/2004 8:34:13 PM}	if (pDoc)
// COMMENT: {8/17/2004 8:34:13 PM}	{
// COMMENT: {8/17/2004 8:34:13 PM}		pDoc->Attach(this);
// COMMENT: {8/17/2004 8:34:13 PM}		pDoc->Attach(pDoc->GetPropertyTreeControlBar());
// COMMENT: {8/17/2004 8:34:13 PM}		pDoc->Attach(pDoc->GetBoxPropertiesDialogBar());
// COMMENT: {8/17/2004 8:34:13 PM}	}
}

void CWPhastView::DeleteContents(void)
{
	// clear the selection
	//
	this->ClearSelection();
// COMMENT: {7/12/2004 3:00:18 PM}	if (vtkInteractorStyle *pStyle = vtkInteractorStyle::SafeDownCast(this->GetRenderWindowInteractor()->GetInteractorStyle())) {
// COMMENT: {7/12/2004 3:00:18 PM}		pStyle->HighlightProp(0);
// COMMENT: {7/12/2004 3:00:18 PM}	}
// COMMENT: {7/12/2004 3:00:18 PM}	if (vtkAbstractPropPicker *pPicker = vtkAbstractPropPicker::SafeDownCast( this->GetRenderWindowInteractor()->GetPicker() )) {
// COMMENT: {7/12/2004 3:00:18 PM}		pPicker->SetPath(0);
// COMMENT: {7/12/2004 3:00:18 PM}	}
// COMMENT: {7/12/2004 3:00:18 PM}	if (vtkBoxWidget *pBoxWidget = this->GetBoxWidget()) {
// COMMENT: {7/12/2004 3:00:18 PM}		pBoxWidget->Off();
// COMMENT: {7/12/2004 3:00:18 PM}	}

	// reset the camera
	//
	vtkCamera* pCamera = this->m_Renderer->GetActiveCamera();
	pCamera->SetFocalPoint(0, 0, 0);
	pCamera->SetPosition(0, 0, 1);
	pCamera->SetViewUp(0, 1, 0);

	//{{
	this->CancelNewZone();
	this->CancelNewWell();
	this->CancelNewRiver();  // TODO check this out
	//}}

	this->m_bMovingGridLine = false;
}

void CWPhastView::OnUpdateToolsNewZone(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	if (!this->CreatingNewZone())
	{
		pCmdUI->SetCheck(0);
	}
	else
	{
		pCmdUI->SetCheck(1);
	}
}

void CWPhastView::OnToolsNewZone()
{
	if (this->CreatingNewZone())
	{
		this->CancelNewZone();
	}
	else
	{
		if (this->CreatingNewRiver())
		{
			this->CancelNewRiver();
		}
		if (this->CreatingNewWell())
		{
			this->CancelNewWell();
		}
		this->StartNewZone();
	}
}

bool CWPhastView::CreatingNewZone(void)const
{
	return (this->m_pNewCubeActor != 0);
}

void CWPhastView::StartNewZone(void)
{
	// set size of 3D cursor
	//
	vtkFloatingPointType* bounds = this->GetDocument()->GetGridBounds();
	vtkFloatingPointType dim = (bounds[1] - bounds[0]) / 20.0;
	this->m_pCursor3D->SetModelBounds(-dim, dim, -dim, dim, -dim, dim);
	this->m_pCursor3DActor->VisibilityOn();

	// set white cursor
	this->m_pCursor3DActor->GetProperty()->SetColor(1, 1, 1);

	ASSERT(this->m_pNewCube == 0);
	ASSERT(this->m_pNewCubeMapper == 0);
	ASSERT(this->m_pNewCubeActor == 0);

	// create cube source
	//
	this->m_pNewCube = vtkCubeSource::New();
	this->m_pNewCube->SetXLength( 0.0 );
	this->m_pNewCube->SetYLength( 0.0 );
	this->m_pNewCube->SetZLength( bounds[5] - bounds[4] );

	// create cube mapper
	//
	this->m_pNewCubeMapper = vtkPolyDataMapper::New();
	this->m_pNewCubeMapper->SetInput( this->m_pNewCube->GetOutput() );

	// create cube actor
	//
	this->m_pNewCubeActor = vtkActor::New();
	this->m_pNewCubeActor->SetMapper( this->m_pNewCubeMapper );
	this->m_pNewCubeActor->GetProperty()->SetColor(0.71, 0.71, 0.71);
	this->m_pNewCubeActor->GetProperty()->SetOpacity(0.5);
	this->m_pNewCubeActor->VisibilityOff();

	// Disable Interactor
	//
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->m_pInteractorStyle))
	{
		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style))
		{
			style = switcher->GetCurrentStyle();
		}
		style->SetInteractor(0);
	}

	// hide BoxWidget
	//
	this->m_pBoxWidget->Off();

	// render
	//
	this->m_Renderer->AddProp(this->m_pNewCubeActor);
    this->m_Renderer->Render();
}

void CWPhastView::CancelNewZone(void)
{
	if (this->CreatingNewZone())
	{
		this->EndNewZone();
		this->GetDocument()->UpdateAllViews(0);
	}
}

void CWPhastView::EndNewZone(void)
{
	ASSERT(this->m_pNewCube != 0);
	ASSERT(this->m_pNewCubeMapper != 0);
	ASSERT(this->m_pNewCubeActor != 0);

	// reset cursor
	//
	this->m_pCursor3DActor->VisibilityOff();

	// reattach interactor
	//
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->m_pInteractorStyle))
	{
		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style))
		{
			style = switcher->GetCurrentStyle();
		}
		style->SetInteractor(this->m_RenderWindowInteractor);
	}

	// stop rendering the cube actor
	//
	this->m_Renderer->RemoveProp(this->m_pNewCubeActor);

	// clean-up
	//
	this->m_pNewCubeActor->Delete();
	this->m_pNewCubeMapper->Delete();
	this->m_pNewCube->Delete();
	this->m_pNewCubeActor  = 0;
	this->m_pNewCubeMapper = 0;
	this->m_pNewCube       = 0;
}

BOOL CWPhastView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// set real-time pan cursor
	//
	if (::GetAsyncKeyState(VK_MBUTTON) < 0)
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_PAN_REAL));
		return TRUE;
	}
	if (::GetAsyncKeyState(VK_SHIFT) < 0 && ::GetAsyncKeyState(VK_LBUTTON))
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_PAN_REAL));
		return TRUE;
	}

	// set 3D cursor
	//
	if (this->m_pCursor3DActor->GetVisibility() && nHitTest == HTCLIENT)
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_NULL));
		return TRUE;
	}

	if (this->CreatingNewRiver() && nHitTest == HTCLIENT)
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_NULL));
		return TRUE;
	}

	if (this->m_bMovingGridLine && nHitTest == HTCLIENT)
	{
		if (CGridLODActor* pGridLODActor = CGridLODActor::SafeDownCast(this->GetDocument()->GetGridActor()))
		{
// COMMENT: {8/4/2005 8:57:01 PM}			extern HCURSOR Test();
// COMMENT: {8/4/2005 8:57:01 PM}			//{{
// COMMENT: {8/4/2005 8:57:01 PM}			static HCURSOR s_hcur = 0;
// COMMENT: {8/4/2005 8:57:01 PM}			if (s_hcur == 0)
// COMMENT: {8/4/2005 8:57:01 PM}			{
// COMMENT: {8/4/2005 8:57:01 PM}				s_hcur = Test();
// COMMENT: {8/4/2005 8:57:01 PM}			}
// COMMENT: {8/4/2005 8:57:01 PM}			if (s_hcur && (::GetKeyState(VK_CONTROL) < 0))
// COMMENT: {8/4/2005 8:57:01 PM}			{
// COMMENT: {8/4/2005 8:57:01 PM}				::SetCursor(s_hcur);
// COMMENT: {8/4/2005 8:57:01 PM}				return TRUE;
// COMMENT: {8/4/2005 8:57:01 PM}			}
// COMMENT: {8/4/2005 8:57:01 PM}			//}}
			if (pGridLODActor->OnSetCursor(pWnd, nHitTest, message))
			{
				return TRUE;
			}
		}
	}

	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CWPhastView::OnUpdateViewFromNx(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromNx(void)
{
	// TODO: Add your command handler code here
	vtkCamera *camera = this->m_Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) + 
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0] - dist, fp[1], fp[2]);
	camera->SetViewUp(0, 0, 1);
	camera->ComputeViewPlaneNormal();
	///PlaceHeadlightWithCamera();
	this->m_Renderer->ResetCameraClippingRange();
	this->m_ViewFromDirection = ID_VIEW_FROM_NX;
	this->Invalidate();
}

void CWPhastView::OnUpdateViewFromPx(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromPx(void)
{
	// TODO: Add your command handler code here
	vtkCamera *camera = this->m_Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) + 
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0] + dist, fp[1], fp[2]);
	camera->SetViewUp(0, 0, 1);
	camera->ComputeViewPlaneNormal();
	///PlaceHeadlightWithCamera();
	this->m_Renderer->ResetCameraClippingRange();
	this->m_ViewFromDirection = ID_VIEW_FROM_PX;
	this->Invalidate();
}

void CWPhastView::OnUpdateViewFromNy(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromNy(void)
{
	// TODO: Add your command handler code here
	vtkCamera *camera = this->m_Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) + 
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0], fp[1] - dist, fp[2]);
	camera->SetViewUp(0, 0, 1);
	camera->ComputeViewPlaneNormal();
	///PlaceHeadlightWithCamera();
	this->m_Renderer->ResetCameraClippingRange();
	this->m_ViewFromDirection = ID_VIEW_FROM_NY;
	this->Invalidate();
}

void CWPhastView::OnUpdateViewFromPy(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromPy(void)
{
	// TODO: Add your command handler code here
	vtkCamera *camera = this->m_Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) + 
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0], fp[1] + dist, fp[2]);
	camera->SetViewUp(0, 0, 1);
	camera->ComputeViewPlaneNormal();
	///PlaceHeadlightWithCamera();
	this->m_Renderer->ResetCameraClippingRange();
	this->m_ViewFromDirection = ID_VIEW_FROM_PY;
	this->Invalidate();
}

void CWPhastView::OnUpdateViewFromNz(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromNz(void)
{
	// TODO: Add your command handler code here
	vtkCamera *camera = this->m_Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) + 
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0], fp[1], fp[2] - dist);
	camera->SetViewUp(0, -1, 0);
	camera->ComputeViewPlaneNormal();
	///PlaceHeadlightWithCamera();
	this->m_Renderer->ResetCameraClippingRange();
	this->m_ViewFromDirection = ID_VIEW_FROM_NZ;
	this->Invalidate();
}

void CWPhastView::OnUpdateViewFromPz(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromPz(void)
{
	// TODO: Add your command handler code here
	vtkCamera *camera = this->m_Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) + 
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0], fp[1], fp[2] + dist);
	camera->SetViewUp(0, 1, 0);
	camera->ComputeViewPlaneNormal();
	///PlaceHeadlightWithCamera();
	this->m_Renderer->ResetCameraClippingRange();
	this->m_ViewFromDirection = ID_VIEW_FROM_PZ;
	this->Invalidate();
}

void CWPhastView::OnViewFromNextDirection(void)
{
	switch (this->m_ViewFromDirection)
	{
	case ID_VIEW_FROM_NX:
		this->OnViewFromNy();
		return;
	case ID_VIEW_FROM_NY:
		this->OnViewFromPx();
		return;
	case ID_VIEW_FROM_PX:
		this->OnViewFromPy();
		return;
	case ID_VIEW_FROM_PY:
		this->OnViewFromNz();
		return;
	case ID_VIEW_FROM_NZ:
		this->OnViewFromPz();
		return;
	case ID_VIEW_FROM_PZ:
		this->OnViewFromNx();
		return;
	}
}

void CWPhastView::Select(vtkProp *pProp)
{
	// Highlight Prop
	//
	this->HighlightProp(pProp);

	// Set Picker
	//
	if (vtkAbstractPropPicker *pPicker = vtkAbstractPropPicker::SafeDownCast( this->GetRenderWindowInteractor()->GetPicker() ))
	{
		if (pProp)
		{
			vtkAssemblyPath *path = vtkAssemblyPath::New();
			path->AddNode(pProp, pProp->GetMatrix());
			pPicker->SetPath(path);
			path->Delete();
		}
		else
		{
			pPicker->SetPath(0);
		}
	}

	// Set BoxWidget
	//
	if (vtkBoxWidget *pBoxWidget = this->GetBoxWidget())
	{
		if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(pProp))
		{
			pBoxWidget->SetProp3D(pZoneActor);
			pBoxWidget->PlaceWidget();
			ASSERT(pZoneActor == pBoxWidget->GetProp3D());
			pBoxWidget->On();
		}
		else if (CWellActor *pWellActor = CWellActor::SafeDownCast(pProp))
		{
			// Reset BoxWidget
			//
			this->m_pPointWidget2->SetProp3D(pWellActor);
			this->m_pPointWidget2->PlaceWidget();
			ASSERT(pWellActor == this->m_pPointWidget2->GetProp3D());
			this->m_pPointWidget2->On();

			// Reset BoxWidget
			pBoxWidget->Off();
// COMMENT: {8/30/2004 3:28:11 PM}			pBoxWidget->SetProp3D(pWellActor);
// COMMENT: {8/30/2004 3:28:11 PM}			pBoxWidget->PlaceWidget();
// COMMENT: {8/30/2004 3:28:11 PM}			ASSERT(pWellActor == pBoxWidget->GetProp3D());
// COMMENT: {8/30/2004 3:28:11 PM}			pBoxWidget->On();
// COMMENT: {8/30/2004 3:28:11 PM}
// COMMENT: {8/30/2004 3:28:11 PM}			this->m_pPointWidget2->Off();

		}
		else
		{
			pBoxWidget->Off();
			if (this->m_pPointWidget2) this->m_pPointWidget2->Off();
		}
	}
}

void CWPhastView::ClearSelection(void)
{
	this->Select(0);

// COMMENT: {8/17/2004 7:30:58 PM}	if (vtkInteractorStyle *pStyle = vtkInteractorStyle::SafeDownCast(this->GetRenderWindowInteractor()->GetInteractorStyle()))
// COMMENT: {8/17/2004 7:30:58 PM}	{
// COMMENT: {8/17/2004 7:30:58 PM}		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(pStyle))
// COMMENT: {8/17/2004 7:30:58 PM}		{
// COMMENT: {8/17/2004 7:30:58 PM}			pStyle = switcher->GetCurrentStyle();
// COMMENT: {8/17/2004 7:30:58 PM}		}
// COMMENT: {8/17/2004 7:30:58 PM}		if (pStyle->GetInteractor())
// COMMENT: {8/17/2004 7:30:58 PM}		{
// COMMENT: {8/17/2004 7:30:58 PM}			pStyle->HighlightProp(0);
// COMMENT: {8/17/2004 7:30:58 PM}		}
// COMMENT: {8/17/2004 7:30:58 PM}		else
// COMMENT: {8/17/2004 7:30:58 PM}		{
// COMMENT: {8/17/2004 7:30:58 PM}			// HACK {{
// COMMENT: {8/17/2004 7:30:58 PM}			// temporarily reattach interactor
// COMMENT: {8/17/2004 7:30:58 PM}			// pStyle->HighlightProp will attempt to use its Interactor
// COMMENT: {8/17/2004 7:30:58 PM}			// causing a fault
// COMMENT: {8/17/2004 7:30:58 PM}			pStyle->SetInteractor(this->m_RenderWindowInteractor);
// COMMENT: {8/17/2004 7:30:58 PM}			ASSERT(pStyle->GetInteractor() == this->m_RenderWindowInteractor);
// COMMENT: {8/17/2004 7:30:58 PM}			pStyle->HighlightProp(0);
// COMMENT: {8/17/2004 7:30:58 PM}			pStyle->SetInteractor(0);
// COMMENT: {8/17/2004 7:30:58 PM}			// HACK }}
// COMMENT: {8/17/2004 7:30:58 PM}		}
// COMMENT: {8/17/2004 7:30:58 PM}	}
// COMMENT: {8/17/2004 7:30:58 PM}	if (vtkAbstractPropPicker *pPicker = vtkAbstractPropPicker::SafeDownCast( this->GetRenderWindowInteractor()->GetPicker() ))
// COMMENT: {8/17/2004 7:30:58 PM}	{
// COMMENT: {8/17/2004 7:30:58 PM}		pPicker->SetPath(0);
// COMMENT: {8/17/2004 7:30:58 PM}	}
// COMMENT: {8/17/2004 7:30:58 PM}	if (vtkBoxWidget *pBoxWidget = this->GetBoxWidget())
// COMMENT: {8/17/2004 7:30:58 PM}	{
// COMMENT: {8/17/2004 7:30:58 PM}		pBoxWidget->Off();
// COMMENT: {8/17/2004 7:30:58 PM}	}
}

void CWPhastView::ParallelProjectionOff(void)
{
	if (vtkRenderer *renderer = this->GetRenderer()) {
		if (vtkCamera *camera = renderer->GetActiveCamera()) {
			camera->ParallelProjectionOff();
		}
	}
}

void CWPhastView::ParallelProjectionOn(void)
{
	if (vtkRenderer *renderer = this->GetRenderer()) {
		if (vtkCamera *camera = renderer->GetActiveCamera()) {
			camera->ParallelProjectionOn();
		}
	}
}

bool CWPhastView::CreatingNewWell(void)const
{
	return (this->m_pWellActor != 0);
}

void CWPhastView::StartNewWell(void)
{
	// set size of 3D cursor
	//
	vtkFloatingPointType* bounds = this->GetDocument()->GetGridBounds();
	vtkFloatingPointType dim = (bounds[1] - bounds[0]) / 20.0;
	this->m_pCursor3D->SetModelBounds(-dim, dim, -dim, dim, -dim, dim);
	this->m_pCursor3DActor->VisibilityOn();

	// set yellow cursor
	//
	this->m_pCursor3DActor->GetProperty()->SetColor(1, 1, 0);

	// create well actor
	//
	ASSERT(this->m_pWellActor == 0);
	this->m_pWellActor = CWellActor::New();

	// Disable Interactor
	//
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->m_pInteractorStyle)) {
		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style)) {
			style = switcher->GetCurrentStyle();
		}
		style->SetInteractor(0);
	}

	// hide BoxWidget
	//
	// Note: This is reqd because the widget will 
	// recieve all the mouse input
	this->m_pBoxWidget->Off();
	this->m_pPointWidget2->Off();
}

void CWPhastView::CancelNewWell(void)
{
	if (this->CreatingNewWell()) {
		this->EndNewWell();
		this->GetDocument()->UpdateAllViews(0);
	}
}

void CWPhastView::EndNewWell(void)
{
	// reset cursor
	//
	this->m_pCursor3DActor->VisibilityOff();

	// reattach interactor
	//
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->m_pInteractorStyle)) {
		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style)) {
			style = switcher->GetCurrentStyle();
		}
		style->SetInteractor(this->m_RenderWindowInteractor);
	}

	// stop rendering the well actor
	//
	this->m_Renderer->RemoveProp(this->m_pWellActor);

	// clean-up
	//
	ASSERT(this->m_pWellActor != 0);
	if (this->m_pWellActor) {
		this->m_pWellActor->Delete();
		this->m_pWellActor = 0;
	}
	this->GetDocument()->UpdateAllViews(0);
}

void CWPhastView::OnToolsNewWell()
{
	if (this->CreatingNewWell())
	{
		this->CancelNewWell();
	}
	else
	{
		if (this->CreatingNewRiver())
		{
			this->CancelNewRiver();
		}
		if (this->CreatingNewZone())
		{
			this->CancelNewZone();
		}
		this->StartNewWell();
	}
}

void CWPhastView::OnUpdateToolsNewWell(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	if (this->CreatingNewWell()) {
		pCmdUI->SetCheck(1);
	}
	else {
		pCmdUI->SetCheck(0);
	}
}

void CWPhastView::HighlightProp(vtkProp *pProp)
{
// COMMENT: {8/17/2004 7:28:04 PM}	if (vtkInteractorStyle *pStyle = vtkInteractorStyle::SafeDownCast(this->GetRenderWindowInteractor()->GetInteractorStyle()))
// COMMENT: {8/17/2004 7:28:04 PM}	{
// COMMENT: {8/17/2004 7:28:04 PM}		pStyle->HighlightProp(pProp);
// COMMENT: {8/17/2004 7:28:04 PM}	}
	//{{
	if (vtkInteractorStyle *pStyle = vtkInteractorStyle::SafeDownCast(this->GetRenderWindowInteractor()->GetInteractorStyle()))
	{
		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(pStyle))
		{
			pStyle = switcher->GetCurrentStyle();
		}

		if (pStyle->GetInteractor())
		{
			pStyle->HighlightProp(pProp);
		}
		else
		{
			// HACK {{
			// temporarily reattach interactor
			// pStyle->HighlightProp will attempt to use its Interactor
			// causing a GPF
			pStyle->SetInteractor(this->m_RenderWindowInteractor);
			ASSERT(pStyle->GetInteractor() == this->m_RenderWindowInteractor);
			pStyle->HighlightProp(pProp);
			pStyle->SetInteractor(0);
			// HACK }}
		}
	}
	//}}
}

void CWPhastView::HighlightProp3D(vtkProp3D *pProp3D)
{
	if (vtkInteractorStyle *pStyle = vtkInteractorStyle::SafeDownCast(this->GetRenderWindowInteractor()->GetInteractorStyle()))
	{
		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(pStyle))
		{
			pStyle = switcher->GetCurrentStyle();
		}

		if (pStyle->GetInteractor())
		{
			pStyle->HighlightProp3D(pProp3D);
		}
		else
		{
			// HACK {{
			// temporarily reattach interactor
			// pStyle->HighlightProp will attempt to use its Interactor
			// causing a GPF
			pStyle->SetInteractor(this->m_RenderWindowInteractor);
			ASSERT(pStyle->GetInteractor() == this->m_RenderWindowInteractor);
			pStyle->HighlightProp3D(pProp3D);
			pStyle->SetInteractor(0);
			// HACK }}
		}
	}
}

void CWPhastView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CView::OnUpdate(pSender, lHint, pHint);
	// TODO: Add your specialized code here and/or call the base class
}

void CWPhastView::Update(IObserver* pSender, LPARAM lHint, CObject* /*pHint*/, vtkObject* pObject)
{
	ASSERT(pSender != this);

	switch (lHint)
	{
	case WPN_NONE:
		break;
	case WPN_SELCHANGED:
		if (vtkProp* pProp = vtkProp::SafeDownCast(pObject))
		{
			this->HighlightProp(pProp);

			if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(pProp))
			{
				ASSERT(FALSE); // untested
				pZoneActor->Select(this);
			}
			else if (CWellActor *pWellActor = CWellActor::SafeDownCast(pProp))
			{
				//{{ TESTING
				if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( this->GetRenderWindowInteractor()->GetPicker() )) {
					vtkAssemblyPath *path = vtkAssemblyPath::New();
					path->AddNode(pWellActor, pWellActor->GetMatrix());
					picker->SetPath(path);
					path->Delete();
				}
				//}} TESTING

				//{{
				this->m_pPointWidget2->TranslationModeOff();
				this->m_pPointWidget2->SetProp3D(pWellActor);
				this->m_pPointWidget2->PlaceWidget();
				ASSERT(pWellActor == this->m_pPointWidget2->GetProp3D());
				this->m_pPointWidget2->TranslationModeOn();
				this->m_pPointWidget2->On();

				// Hide Zone BoxWidget
				this->GetBoxWidget()->Off();
// COMMENT: {8/30/2004 3:27:03 PM}				this->m_pBoxWidget->SetProp3D(pWellActor);
// COMMENT: {8/30/2004 3:27:03 PM}				this->m_pBoxWidget->PlaceWidget();
// COMMENT: {8/30/2004 3:27:03 PM}				this->m_pBoxWidget->On();

				this->Invalidate(TRUE);
				//}}
			}
			else if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(pProp))
			{
				//{{ TESTING
				if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( this->GetRenderWindowInteractor()->GetPicker() )) {
					vtkAssemblyPath *path = vtkAssemblyPath::New();
					path->AddNode(pRiverActor, pRiverActor->GetMatrix());
					picker->SetPath(path);
					path->Delete();
				}
				//}} TESTING

				// Hide Zone BoxWidget
				this->GetBoxWidget()->Off();
				this->m_pPointWidget2->Off();
				pRiverActor->SetEnabled(1);

				this->Invalidate(TRUE);
			}			
			else
			{
				ASSERT(FALSE); // untested
			}
		}
		break;
	case WPN_VISCHANGED:
		if (vtkProp* pProp = vtkProp::SafeDownCast(pObject))
		{
			if (!pProp->GetVisibility())
			{
				if (vtkAbstractPropPicker *pPicker = vtkAbstractPropPicker::SafeDownCast( this->GetRenderWindowInteractor()->GetPicker() ))
				{
					if (vtkPropAssembly* pPropAssembly = vtkPropAssembly::SafeDownCast(pObject))
					{
						if (vtkPropCollection *pParts = pPropAssembly->GetParts())
						{
							if (pParts->IsItemPresent(pPicker->GetProp()))
							{
								this->ClearSelection();
							}
						}
					}
					else
					{
						if (pPicker->GetProp() == pProp)
						{
							this->ClearSelection();
						}
					}
				}
			}
		}		
		break;
	case WPN_SCALE_CHANGED:
		if (this->m_pPointWidget2->GetEnabled())
		{
			if (vtkProp3D* pProp3D = this->m_pPointWidget2->GetProp3D())
			{
				this->m_pPointWidget2->TranslationModeOff();
				this->m_pPointWidget2->SetProp3D(0);
				this->m_pPointWidget2->SetProp3D(pProp3D);
				this->m_pPointWidget2->PlaceWidget();
				ASSERT(pProp3D == this->m_pPointWidget2->GetProp3D());
				this->m_pPointWidget2->TranslationModeOn();
				this->m_pPointWidget2->On();
			}
		}
		break;
	default:
		ASSERT(FALSE);
	}
}

void CWPhastView::OnUpdateToolsNewRiver(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	if (this->CreatingNewRiver())
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CWPhastView::OnToolsNewRiver()
{
	if (this->CreatingNewRiver())
	{
		this->CancelNewRiver();
	}
	else
	{
		if (this->CreatingNewZone())
		{
			this->CancelNewZone();
		}
		if (this->CreatingNewWell())
		{
			this->CancelNewWell();
		}
		this->StartNewRiver();
	}
}

bool CWPhastView::CreatingNewRiver(void)const
{
	return (this->RiverCallbackCommand != 0);
}

void CWPhastView::StartNewRiver(void)
{
	{
		ASSERT(this->RiverCallbackCommand == 0);
		this->RiverCallbackCommand = vtkCallbackCommand::New();
		this->RiverCallbackCommand->SetClientData(this);
		this->RiverCallbackCommand->SetCallback(CWPhastView::RiverListener);

		if (this->m_pRiverActor != 0)
		{
			this->m_pRiverActor->Delete();
		}
		this->m_pRiverActor = CRiverActor::StartNewRiver(this->GetRenderWindowInteractor());
		
		CRiver river;
		river.n_user = this->GetDocument()->GetNextRiverNumber();
		this->m_pRiverActor->SetRiver(river, this->GetDocument()->GetUnits());

		this->m_pRiverActor->AddObserver(CRiverActor::EndNewRiverEvent, RiverCallbackCommand);

		vtkFloatingPointType* scale = this->GetDocument()->GetScale();
		this->m_pRiverActor->SetScale(scale[0], scale[1], scale[2]);

		this->m_pRiverActor->ScaleFromBounds(this->GetDocument()->GetGridBounds());

		CGrid x, y, z;
		this->GetDocument()->GetGrid(x, y, z);
		z.Setup();
		this->m_pRiverActor->SetZ(z.coord[z.count_coord - 1]);
		this->m_Renderer->AddProp(this->m_pRiverActor);
	}

	// Disable Interactor - NOTE (This may be unnec)
	//
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->m_pInteractorStyle))
	{
		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style))
		{
			style = switcher->GetCurrentStyle();
		}
		style->SetInteractor(0);
	}

	// hide Widgets
	//
	// Note: This is reqd because the widget will 
	// recieve all the mouse input
	this->GetDocument()->ClearSelection();
	this->m_pBoxWidget->Off();
	this->m_pPointWidget2->Off();
}

void CWPhastView::EndNewRiver(void)
{
	// reattach interactor
	//
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->m_pInteractorStyle))
	{
		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style))
		{
			style = switcher->GetCurrentStyle();
		}
		style->SetInteractor(this->m_RenderWindowInteractor);
	}

	// stop rendering the river actor
	//
	this->m_Renderer->RemoveProp(this->m_pRiverActor);

	// clean-up
	//
	ASSERT(this->m_pRiverActor != 0);
	if (this->m_pRiverActor)
	{
		this->m_pRiverActor->Delete();
		this->m_pRiverActor = 0;
	}
	ASSERT(this->RiverCallbackCommand != 0);
	if (this->RiverCallbackCommand)
	{
		this->RiverCallbackCommand->Delete();
		this->RiverCallbackCommand = 0;
	}

	this->GetDocument()->UpdateAllViews(0);
}

void CWPhastView::CancelNewRiver(void)
{
	if (this->CreatingNewRiver())
	{
		this->EndNewRiver();
		this->GetDocument()->UpdateAllViews(0);
	}
}

void CWPhastView::RiverListener(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
	ASSERT(caller->IsA("CRiverActor"));
	ASSERT(clientdata);

	if (CRiverActor* pRiverActor = CRiverActor::SafeDownCast(caller))
	{
		CWPhastView* self = reinterpret_cast<CWPhastView*>(clientdata);
		ASSERT(pRiverActor == self->m_pRiverActor);
		switch (eid)
		{
		case CRiverActor::StartNewRiverEvent:
			break;
		case CRiverActor::CancelNewRiverEvent:
			break;
		case CRiverActor::EndNewRiverEvent:
			self->OnEndNewRiver();
			break;
		}
	}
}

void CWPhastView::OnEndNewRiver(void)
{
	// reattach interactor
	//
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->m_pInteractorStyle))
	{
		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style))
		{
			style = switcher->GetCurrentStyle();
		}
		style->SetInteractor(this->m_RenderWindowInteractor);
	}

	//
	// NOTE: this->m_pRiverActor->Delete() can't be called within
	// here because the list of listeners will also be deleted
	// *** this may be fixed in 4.4 ***
	//
	ASSERT(this->m_pRiverActor != 0);
	if (this->m_pRiverActor)
	{
		if (this->m_pRiverActor->GetPointCount() > 1)
		{
			CPropertySheet sheet;
			CRiverPropertyPage2 page;
			page.SetProperties(this->m_pRiverActor->GetRiver());
			page.SetUnits(this->GetDocument()->GetUnits());
			page.SetFlowOnly(bool(this->GetDocument()->GetFlowOnly()));
			std::set<int> riverNums;
			this->GetDocument()->GetUsedRiverNumbers(riverNums);
			page.SetUsedRiverNumbers(riverNums);
			sheet.AddPage(&page);
			if (sheet.DoModal() == IDOK)
			{
				CRiver river;
				page.GetProperties(river);
				this->GetDocument()->Execute(new CRiverCreateAction(this->GetDocument(), river));
			}
		}
		else
		{
			::AfxMessageBox("Rivers must contain at least two points");
		}
		this->m_Renderer->RemoveProp(this->m_pRiverActor);
	}

	if (this->RiverCallbackCommand)
	{
		this->RiverCallbackCommand->Delete();
		this->RiverCallbackCommand = 0;
	}
	this->GetDocument()->UpdateAllViews(0);
}

void CWPhastView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	::AfxMessageBox("OnLButtonDblClk");

	__super::OnLButtonDblClk(nFlags, point);
}

void CWPhastView::OnDestroy()
{
	__super::OnDestroy();

	// TODO: Add your message handler code here
}

void CWPhastView::OnToolsMoveVerLine()
{
	// TODO: Add your command handler code here
	if (this->m_bMovingGridLine)
	{
		////this->GetDocument()->GetGridActor()->CancelMoveGridLine();
		///if (CGridLODActor* pGridLODActor = CGridLODActor::SafeDownCast(this->GetDocument()->GetGridActor()))
		if (CGridActor* pGridActor = CGridActor::SafeDownCast(this->GetDocument()->GetGridActor()))
		{
			// pGridLODActor->SetInteractor(0);
			pGridActor->SetEnabled(0);
		}
		this->m_bMovingGridLine = false;
	}
	else
	{
		this->m_bMovingGridLine = true;
		////this->GetDocument()->GetGridActor()->StartMoveGridLine(this->m_RenderWindowInteractor);
		if (CGridActor* pGridActor = CGridActor::SafeDownCast(this->GetDocument()->GetGridActor()))
		{
			pGridActor->SetInteractor(this->m_RenderWindowInteractor);
			// pGridLODActor->SetEnabled(1);
		}
	}
}

void CWPhastView::OnUpdateToolsMoveVerLine(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	if (this->m_bMovingGridLine)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}


#define CreateMonoBitmap( cx,  cy) CreateBitmap(cx, cy, 1, 1, NULL)
#define _BitSizeOf(x) (sizeof(x)*8)
#define SIZEOF(x) (sizeof(x))
typedef WORD CURMASK;
#define	DROPEFFECT_NONE	( 0 )
#define	DROPEFFECT_COPY	( 1 )
#define	DROPEFFECT_MOVE	( 2 )
#define	DROPEFFECT_LINK	( 4 )

extern HBITMAP FAR PASCAL CreateColorBitmap(int cx, int cy);

// COMMENT: {8/4/2005 9:28:44 PM}HBITMAP FAR PASCAL CreateColorBitmap(int cx, int cy)
// COMMENT: {8/4/2005 9:28:44 PM}{
// COMMENT: {8/4/2005 9:28:44 PM}    HBITMAP hbm;
// COMMENT: {8/4/2005 9:28:44 PM}    HDC hdc;
// COMMENT: {8/4/2005 9:28:44 PM}
// COMMENT: {8/4/2005 9:28:44 PM}    hdc = GetDC(NULL);
// COMMENT: {8/4/2005 9:28:44 PM}
// COMMENT: {8/4/2005 9:28:44 PM}    //
// COMMENT: {8/4/2005 9:28:44 PM}    // on a multimonitor system with mixed bitdepths
// COMMENT: {8/4/2005 9:28:44 PM}    // always use a 32bit bitmap for our work buffer
// COMMENT: {8/4/2005 9:28:44 PM}    // this will prevent us from losing colors when
// COMMENT: {8/4/2005 9:28:44 PM}    // blting to and from the screen.  this is mainly
// COMMENT: {8/4/2005 9:28:44 PM}    // important for the drag & drop offscreen buffers.
// COMMENT: {8/4/2005 9:28:44 PM}    //
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}    if (!(GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE) &&
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}		TRUE &&
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}		TRUE)
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}// COMMENT: {9/8/2004 8:42:09 PM}        GetSystemMetrics(SM_CMONITORS) > 1 &&
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}// COMMENT: {9/8/2004 8:42:09 PM}        GetSystemMetrics(SM_SAMEDISPLAYFORMAT) == 0)
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}    {
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}        LPVOID p;
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}#ifndef UNIX
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}        BITMAPINFO bi = {sizeof(BITMAPINFOHEADER), cx, cy, 1, 32};
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}#else
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}        BITMAPINFO bi;
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}        bi.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER); 
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}        bi.bmiHeader.biWidth    = cx;
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}        bi.bmiHeader.biHeight   = cy;
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}        bi.bmiHeader.biPlanes   = 1 ;
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}        bi.bmiHeader.biBitCount = 32;
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}#endif
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}        hbm = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &p, NULL, 0);
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}    }
// COMMENT: {8/4/2005 9:28:44 PM}// COMMENT: {9/8/2004 10:48:49 PM}    else
// COMMENT: {8/4/2005 9:28:44 PM}    {
// COMMENT: {8/4/2005 9:28:44 PM}        hbm = CreateCompatibleBitmap(hdc, cx, cy);
// COMMENT: {8/4/2005 9:28:44 PM}    }
// COMMENT: {8/4/2005 9:28:44 PM}
// COMMENT: {8/4/2005 9:28:44 PM}    ReleaseDC(NULL, hdc);
// COMMENT: {8/4/2005 9:28:44 PM}    return hbm;
// COMMENT: {8/4/2005 9:28:44 PM}}

void GetCursorLowerRight(HCURSOR hcursor, int * px, int * py, POINT *pptHotSpot)
{
    ICONINFO iconinfo;
    CURMASK CurMask[16*8];
    BITMAP bm;
    int i;
    int xFine = 16;

    GetIconInfo(hcursor, &iconinfo);
    GetObject(iconinfo.hbmMask, SIZEOF(bm), (LPTSTR)&bm);
    GetBitmapBits(iconinfo.hbmMask, SIZEOF(CurMask), CurMask);
    pptHotSpot->x = iconinfo.xHotspot;
    pptHotSpot->y = iconinfo.yHotspot;
    if (iconinfo.hbmColor) 
    {
        i = (int)(bm.bmWidth * bm.bmHeight / _BitSizeOf(CURMASK) - 1);
    } 
    else 
    {
        i = (int)(bm.bmWidth * (bm.bmHeight/2) / _BitSizeOf(CURMASK) - 1);
    }

    if ( i >= SIZEOF(CurMask)) 
    {
        i = SIZEOF(CurMask) -1;
    }

    // BUGBUG: this assumes that the first pixel encountered on this bottom
    // up/right to left search will be reasonably close to the rightmost pixel
    // which for all of our cursors is correct, but it not necessarly correct.

    // also, it assumes the cursor has a good mask... not like the IBeam XOR only
    // cursor
    for (; i >= 0; i--)   
    {
        if (CurMask[i] != 0xFFFF) 
        {
            // this is only accurate to 16 pixels... which is a big gap..
            // so let's try to be a bit more accurate.
            int j;
            DWORD dwMask;

            for (j = 0; j < 16; j++, xFine--) 
            {
                if (j < 8) 
                {
                    dwMask = (1 << (8 + j));
                } 
                else 
                {
                    dwMask = (1 << (j - 8));
                }

                if (!(CurMask[i] & dwMask))
                    break;
            }
            ASSERT(j < 16);
            break;
        }
    }

    if (iconinfo.hbmColor) 
    {
        DeleteObject(iconinfo.hbmColor);
    }

    if (iconinfo.hbmMask) 
    {
        DeleteObject(iconinfo.hbmMask);
    }

    // Compute the pointer height
    // use width in both directions because the cursor is square, but the
    // height might be doubleheight if it's mono
    *py = ((i + 1) * _BitSizeOf(CURMASK)) / (int)bm.bmWidth;
    *px = ((i * _BitSizeOf(CURMASK)) % (int)bm.bmWidth) + xFine + 2; // hang it off a little
}



// this will draw iiMerge's image over iiMain on main's lower right.
BOOL MergeIcons(HCURSOR hcursor, LPCTSTR idMerge, HBITMAP *phbmImage, HBITMAP *phbmMask, POINT* pptHotSpot)
{
    BITMAP bm;
    int xBitmap;
    int yBitmap;
    int xDraw;
    int yDraw;
    HDC hdcCursor, hdcBitmap;
    HBITMAP hbmTemp;
    HBITMAP hbmImage;
    HBITMAP hbmMask;
    int xCursor = GetSystemMetrics(SM_CXCURSOR);
    int yCursor = GetSystemMetrics(SM_CYCURSOR);
    HBITMAP hbmp;

    // find the lower corner of the cursor and put it there.
    // do this whether or not we have an idMerge because it will set the hotspot
    GetCursorLowerRight(hcursor, &xDraw, &yDraw, pptHotSpot);
    if (idMerge != (LPCTSTR)-1) 
    {
// COMMENT: {8/4/2005 7:50:45 PM}        hbmp = (HBITMAP)LoadImage(HINST_THISDLL, idMerge, IMAGE_BITMAP, 0, 0, 0);
        hbmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), idMerge, IMAGE_BITMAP, 0, 0, 0);		
        if (hbmp) 
        {
            // GetObject(hbmp, SIZEOF(bm), &bm);
            GetObject(hbmp, sizeof(bm), &bm);
            xBitmap = bm.bmWidth;
            yBitmap = bm.bmHeight/2;

            if (xDraw + xBitmap > xCursor)
                xDraw = xCursor - xBitmap;
            if (yDraw + yBitmap > yCursor)
                yDraw = yCursor - yBitmap;
        }
    } 
    else
        hbmp = NULL;


    hdcCursor = CreateCompatibleDC(NULL);

    hbmMask = CreateMonoBitmap(xCursor, yCursor);
    hbmImage = CreateColorBitmap(xCursor, yCursor);

    if (hdcCursor && hbmMask && hbmImage) 
    {

        hbmTemp = (HBITMAP)SelectObject(hdcCursor, hbmImage);
        DrawIconEx(hdcCursor, 0, 0, hcursor, 0, 0, 0, NULL, DI_NORMAL);

        if (hbmp) 
        {
            hdcBitmap = CreateCompatibleDC(NULL);
            SelectObject(hdcBitmap, hbmp);

            //blt the two bitmaps onto the color and mask bitmaps for the cursor
            BitBlt(hdcCursor, xDraw, yDraw, xBitmap, yBitmap, hdcBitmap, 0, 0, SRCCOPY);
        }

        SelectObject(hdcCursor, hbmMask);

        DrawIconEx(hdcCursor, 0, 0, hcursor, 0, 0, 0, NULL, DI_MASK);

        if (hbmp) 
        {
            BitBlt(hdcCursor, xDraw, yDraw, xBitmap, yBitmap, hdcBitmap, 0, yBitmap, SRCCOPY);

            // select back in the old bitmaps
            SelectObject(hdcBitmap, hbmTemp);
            DeleteDC(hdcBitmap);
            DeleteObject(hbmp);
        }

        // select back in the old bitmaps
        SelectObject(hdcCursor, hbmTemp);
    }

    if (hdcCursor)
        DeleteDC(hdcCursor);

    *phbmImage = hbmImage;
    *phbmMask = hbmMask;
    return (hbmImage && hbmMask);
}

HCURSOR SetCursorHotspot(HCURSOR hcur, POINT *ptHot)
{
    ICONINFO iconinfo;
    HCURSOR hcurHotspot;

    GetIconInfo(hcur, &iconinfo);
    iconinfo.xHotspot = ptHot->x;
    iconinfo.yHotspot = ptHot->y;
    iconinfo.fIcon = FALSE;
    hcurHotspot = (HCURSOR)CreateIconIndirect(&iconinfo);
    if (iconinfo.hbmColor) 
    {
        DeleteObject(iconinfo.hbmColor);
    }

    if (iconinfo.hbmMask) 
    {
        DeleteObject(iconinfo.hbmMask);
    }
    return hcurHotspot;
}


HCURSOR Test()
{
	//{{
    HIMAGELIST  _himlCursors;

    UINT uFlags = ILC_MASK;

    //
    // if this is not a palette device, use a DDB for the imagelist
    // this is important when displaying high-color cursors
    //
	HDC hdc = ::GetDC(NULL);
	if (!(::GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE))
    {
        uFlags |= ILC_COLORDDB;
    }
	::ReleaseDC(NULL, hdc);

	_himlCursors = ImageList_Create(::GetSystemMetrics(SM_CXCURSOR),
		::GetSystemMetrics(SM_CYCURSOR),
		uFlags, 1, 0);
	//}}

	//{{
	// _MapCursorIDToImageListIndex(DCID_INVALID);
	POINT ptHotSpot;
	{
		HCURSOR hcur = ::LoadCursor(NULL, IDC_ARROW);
		//int n = AddCursorToImageList(hcur, MAKEINTRESOURCE(IDB_PLUS_MERGE), &ptHotSpot);
		{
			/// int CDragImages::_AddCursorToImageList(HCURSOR hcur, LPCTSTR idMerge, POINT *pptHotSpot)
			LPCTSTR idMerge = MAKEINTRESOURCE(IDB_PLUS_MERGE);
			POINT *pptHotSpot = &ptHotSpot;
			{
				int iIndex;
				HBITMAP hbmImage, hbmMask;

				// merge in the plus or link arrow if it's specified
				if (MergeIcons(hcur, idMerge, &hbmImage, &hbmMask, pptHotSpot)) 
				{
					iIndex = ImageList_Add(_himlCursors, hbmImage, hbmMask);
				} 
				else 
				{
					iIndex = -1;
				}

				if (hbmImage)
					DeleteObject(hbmImage);

				if (hbmMask)
					DeleteObject(hbmMask);

				//return iIndex;
			}
		}
	}
	//}}

// COMMENT: {8/4/2005 8:03:58 PM}	HCURSOR hcurColor = ImageList_GetIcon(_himlCursors, iIndex, 0);
	HCURSOR hcurColor = ImageList_GetIcon(_himlCursors, 0, 0);
	//
	// On non C1_COLORCURSOR displays, CopyImage() will enforce
	// monochrome.  So on color cursor displays, we'll get colored
	// dragdrop pix.
	//
	HCURSOR hcurScreen = (HCURSOR)::CopyImage(hcurColor, IMAGE_CURSOR,
		0, 0, LR_COPYRETURNORG | LR_DEFAULTSIZE);

	HCURSOR hcurFinal = SetCursorHotspot(hcurScreen, &ptHotSpot);

	if (hcurScreen != hcurColor) 
	{
		::DestroyCursor(hcurColor);
	}

	if (hcurFinal)
		::DestroyCursor(hcurScreen);
	else
		hcurFinal = hcurScreen;

	return hcurFinal;
}
