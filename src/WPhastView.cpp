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
#include "vtkBoxWidgetEx.h"

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
// COMMENT: {3/5/2008 4:27:16 PM}#ifdef USE_WEDGE
// COMMENT: {3/5/2008 4:27:16 PM}#include "MyCubeSource.h"
// COMMENT: {3/5/2008 4:27:16 PM}#endif
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

#include <vtkImplicitPlaneWidget.h>


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
// COMMENT: {5/8/2006 5:28:40 PM}	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWZONE, OnUpdateToolsNewZone)
// COMMENT: {5/8/2006 5:28:40 PM}	ON_COMMAND(ID_TOOLS_NEWZONE, OnToolsNewZone)
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
// COMMENT: {8/29/2005 6:46:54 PM}	ON_UPDATE_COMMAND_UI(ID_TOOLS_MODIFYGRID, OnUpdateToolsModifyGrid)
// COMMENT: {8/29/2005 6:46:54 PM}	ON_COMMAND(ID_TOOLS_MODIFYGRID, OnToolsModifyGrid)
ON_COMMAND(ID_VIEW_FROM_PREV_DIRECTION, OnViewFromPrevDirection)
ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTOBJECT, OnUpdateToolsSelectObject)
ON_COMMAND(ID_TOOLS_SELECTOBJECT, OnToolsSelectObject)
END_MESSAGE_MAP()

// CWPhastView construction/destruction

CWPhastView::CWPhastView()
: m_bResetCamera(false)
// COMMENT: {3/5/2008 4:33:57 PM}, m_pNewCube(0)
// COMMENT: {3/5/2008 4:33:57 PM}, m_pNewCubeMapper(0)
// COMMENT: {3/5/2008 4:33:57 PM}, m_pNewCubeActor(0)
, m_ViewFromDirection(ID_VIEW_FROM_PZ)
, m_pWellActor(0)
, PointWidget(0)
, m_pRiverActor(0)
, RiverCallbackCommand(0)
, m_bMovingGridLine(false)
, BoxWidget(0)
, m_RenderWindow(0)
, m_Renderer(0)
, m_RenderWindowInteractor(0)
, InteractorStyle(0)
, m_pViewVTKCommand(0)
, m_pCursor3D(0)
, m_pCursor3DMapper(0)
, m_pCursor3DActor(0)
, CurrentProp(0)
{
	// Create the renderer, window and interactor objects.
	//
	this->m_RenderWindow           = vtkWin32OpenGLRenderWindow::New();
	this->m_Renderer               = vtkRenderer::New();
	this->m_RenderWindowInteractor = vtkWin32RenderWindowInteractor::New();

	// Create the the BoxWidget
	//
	this->BoxWidget = vtkBoxWidgetEx::New();
	this->BoxWidget->SetInteractor(this->m_RenderWindowInteractor);
	this->BoxWidget->SetPlaceFactor(1.0);
	this->BoxWidget->RotationEnabledOff();
	if (vtkBoxWidgetEx *widget = vtkBoxWidgetEx::SafeDownCast(this->BoxWidget))
	{
		widget->HexPickerEnabledOff();
	}

	// Create the the PointWidget2
	//
	this->PointWidget = vtkPointWidget2::New();
	this->PointWidget->SetInteractor(this->m_RenderWindowInteractor);
	///this->PointWidget->TranslationModeOn();

	//
	this->InteractorStyle = vtkInteractorStyleTrackballCameraEx::New();
	this->m_RenderWindowInteractor->SetInteractorStyle(this->InteractorStyle);

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

	this->BoxWidget->AddObserver(vtkCommand::InteractionEvent, this->m_pViewVTKCommand);
	this->BoxWidget->AddObserver(vtkCommand::EndInteractionEvent, this->m_pViewVTKCommand);
	this->BoxWidget->AddObserver(vtkCommand::StartInteractionEvent, this->m_pViewVTKCommand);

	this->PointWidget->AddObserver(vtkCommand::InteractionEvent, this->m_pViewVTKCommand);
	this->PointWidget->AddObserver(vtkCommand::EndInteractionEvent, this->m_pViewVTKCommand);
	this->PointWidget->AddObserver(vtkCommand::StartInteractionEvent, this->m_pViewVTKCommand);


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
	//{{
	this->CancelMode();
	//}}

	this->InteractorStyle->Delete();
	this->BoxWidget->SetInteractor(0);
	this->BoxWidget->Delete();
	this->PointWidget->SetInteractor(0);
	this->PointWidget->Delete();

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

// COMMENT: {3/5/2008 4:43:20 PM}	if (this->CreatingNewZone())
// COMMENT: {3/5/2008 4:43:20 PM}	{
// COMMENT: {3/5/2008 4:43:20 PM}		this->EndNewZone();
// COMMENT: {3/5/2008 4:43:20 PM}	}

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

// COMMENT: {3/5/2008 4:34:14 PM}	if (this->m_pNewCubeActor) {
// COMMENT: {3/5/2008 4:34:14 PM}		this->m_Renderer->AddProp(this->m_pNewCubeActor);
// COMMENT: {3/5/2008 4:34:14 PM}	}


	// add props to renderer
	if ( vtkPropCollection *props = this->GetDocument()->GetPropCollection() ) {
		props->InitTraversal();
		for (int i = 0; i < props->GetNumberOfItems(); ++i) {
			vtkProp* prop = props->GetNextProp();
			//{{ {4/25/2006 10:15:10 PM}
			if (prop)
			{
				this->m_Renderer->AddProp(prop);
			}
			//}} {4/25/2006 10:15:10 PM}

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
	//{{ {5/4/2006 5:38:44 PM}
	///this->Invalidate();
	///this->Pipeline();
	//}} {5/4/2006 5:38:44 PM}
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
	this->m_bResetCamera = true;
}

void CWPhastView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

void CWPhastView::DeleteContents(void)
{
	// clear the selection
	//
	this->ClearSelection();

	// reset the camera
	//
	vtkCamera* pCamera = this->m_Renderer->GetActiveCamera();
	pCamera->SetFocalPoint(0, 0, 0);
	pCamera->SetPosition(0, 0, 1);
	pCamera->SetViewUp(0, 1, 0);

	// cancel all modes
	//
	this->CancelMode();
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
		this->CancelMode();
		this->StartNewZone();
	}
}

bool CWPhastView::CreatingNewZone(void)const
{
	ASSERT(FALSE); // DEAD ??
	return false;
// COMMENT: {3/5/2008 4:30:34 PM}	return (this->m_pNewCubeActor != 0);
}

void CWPhastView::StartNewZone(void)
{
	ASSERT(FALSE); // DEAD ??

// COMMENT: {3/5/2008 4:27:44 PM}	// set size of 3D cursor
// COMMENT: {3/5/2008 4:27:44 PM}	//
// COMMENT: {3/5/2008 4:27:44 PM}	vtkFloatingPointType* bounds = this->GetDocument()->GetGridBounds();
// COMMENT: {3/5/2008 4:27:44 PM}	vtkFloatingPointType dim = (bounds[1] - bounds[0]) / 20.0;
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pCursor3D->SetModelBounds(-dim, dim, -dim, dim, -dim, dim);
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pCursor3DActor->VisibilityOn();
// COMMENT: {3/5/2008 4:27:44 PM}
// COMMENT: {3/5/2008 4:27:44 PM}	// set white cursor
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pCursor3DActor->GetProperty()->SetColor(1, 1, 1);
// COMMENT: {3/5/2008 4:27:44 PM}
// COMMENT: {3/5/2008 4:27:44 PM}	ASSERT(this->m_pNewCube == 0);
// COMMENT: {3/5/2008 4:27:44 PM}	ASSERT(this->m_pNewCubeMapper == 0);
// COMMENT: {3/5/2008 4:27:44 PM}	ASSERT(this->m_pNewCubeActor == 0);
// COMMENT: {3/5/2008 4:27:44 PM}
// COMMENT: {3/5/2008 4:27:44 PM}	// create cube source
// COMMENT: {3/5/2008 4:27:44 PM}	//
// COMMENT: {3/5/2008 4:27:44 PM}#ifdef USE_WEDGE
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCube = MyCubeSource::New();
// COMMENT: {3/5/2008 4:27:44 PM}#else
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCube = vtkCubeSource::New();
// COMMENT: {3/5/2008 4:27:44 PM}#endif
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCube->SetXLength( 0.0 );
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCube->SetYLength( 0.0 );
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCube->SetZLength( bounds[5] - bounds[4] );
// COMMENT: {3/5/2008 4:27:44 PM}
// COMMENT: {3/5/2008 4:27:44 PM}	// create cube mapper
// COMMENT: {3/5/2008 4:27:44 PM}	//
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCubeMapper = vtkPolyDataMapper::New();
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCubeMapper->SetInput( this->m_pNewCube->GetOutput() );
// COMMENT: {3/5/2008 4:27:44 PM}
// COMMENT: {3/5/2008 4:27:44 PM}	// create cube actor
// COMMENT: {3/5/2008 4:27:44 PM}	//
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCubeActor = vtkActor::New();
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCubeActor->SetMapper( this->m_pNewCubeMapper );
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCubeActor->GetProperty()->SetColor(0.71, 0.71, 0.71);
// COMMENT: {3/5/2008 4:27:44 PM}// COMMENT: {8/23/2007 10:09:31 PM}	this->m_pNewCubeActor->GetProperty()->SetOpacity(0.5);
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCubeActor->GetProperty()->SetOpacity(1.0);
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCubeActor->GetProperty()->SetAmbient(1.0);
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_pNewCubeActor->VisibilityOff();
// COMMENT: {3/5/2008 4:27:44 PM}
// COMMENT: {3/5/2008 4:27:44 PM}	// Disable Interactor
// COMMENT: {3/5/2008 4:27:44 PM}	//
// COMMENT: {3/5/2008 4:27:44 PM}	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->InteractorStyle))
// COMMENT: {3/5/2008 4:27:44 PM}	{
// COMMENT: {3/5/2008 4:27:44 PM}		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style))
// COMMENT: {3/5/2008 4:27:44 PM}		{
// COMMENT: {3/5/2008 4:27:44 PM}			style = switcher->GetCurrentStyle();
// COMMENT: {3/5/2008 4:27:44 PM}		}
// COMMENT: {3/5/2008 4:27:44 PM}		style->SetInteractor(0);
// COMMENT: {3/5/2008 4:27:44 PM}	}
// COMMENT: {3/5/2008 4:27:44 PM}
// COMMENT: {3/5/2008 4:27:44 PM}	// hide BoxWidget
// COMMENT: {3/5/2008 4:27:44 PM}	//
// COMMENT: {3/5/2008 4:27:44 PM}	this->BoxWidget->Off();
// COMMENT: {3/5/2008 4:27:44 PM}
// COMMENT: {3/5/2008 4:27:44 PM}	// render
// COMMENT: {3/5/2008 4:27:44 PM}	//
// COMMENT: {3/5/2008 4:27:44 PM}	this->m_Renderer->AddProp(this->m_pNewCubeActor);
// COMMENT: {3/5/2008 4:27:44 PM}    this->m_Renderer->Render();
}

void CWPhastView::CancelNewZone(void)
{
	ASSERT(FALSE); // DEAD ??
// COMMENT: {3/5/2008 4:30:47 PM}	if (this->CreatingNewZone())
// COMMENT: {3/5/2008 4:30:47 PM}	{
// COMMENT: {3/5/2008 4:30:47 PM}		this->EndNewZone();
// COMMENT: {3/5/2008 4:30:47 PM}	}
}

void CWPhastView::EndNewZone(void)
{
	ASSERT(FALSE); // DEAD ??

// COMMENT: {3/5/2008 4:30:56 PM}	ASSERT(this->m_pNewCube != 0);
// COMMENT: {3/5/2008 4:30:56 PM}	ASSERT(this->m_pNewCubeMapper != 0);
// COMMENT: {3/5/2008 4:30:56 PM}	ASSERT(this->m_pNewCubeActor != 0);
// COMMENT: {3/5/2008 4:30:56 PM}
// COMMENT: {3/5/2008 4:30:56 PM}	// reset cursor
// COMMENT: {3/5/2008 4:30:56 PM}	//
// COMMENT: {3/5/2008 4:30:56 PM}	this->m_pCursor3DActor->VisibilityOff();
// COMMENT: {3/5/2008 4:30:56 PM}
// COMMENT: {3/5/2008 4:30:56 PM}	// reattach interactor
// COMMENT: {3/5/2008 4:30:56 PM}	//
// COMMENT: {3/5/2008 4:30:56 PM}	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->InteractorStyle))
// COMMENT: {3/5/2008 4:30:56 PM}	{
// COMMENT: {3/5/2008 4:30:56 PM}		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style))
// COMMENT: {3/5/2008 4:30:56 PM}		{
// COMMENT: {3/5/2008 4:30:56 PM}			style = switcher->GetCurrentStyle();
// COMMENT: {3/5/2008 4:30:56 PM}		}
// COMMENT: {3/5/2008 4:30:56 PM}		style->SetInteractor(this->m_RenderWindowInteractor);
// COMMENT: {3/5/2008 4:30:56 PM}	}
// COMMENT: {3/5/2008 4:30:56 PM}
// COMMENT: {3/5/2008 4:30:56 PM}	// stop rendering the cube actor
// COMMENT: {3/5/2008 4:30:56 PM}	//
// COMMENT: {3/5/2008 4:30:56 PM}	this->m_Renderer->RemoveProp(this->m_pNewCubeActor);
// COMMENT: {3/5/2008 4:30:56 PM}
// COMMENT: {3/5/2008 4:30:56 PM}	// clean-up
// COMMENT: {3/5/2008 4:30:56 PM}	//
// COMMENT: {3/5/2008 4:30:56 PM}	this->m_pNewCubeActor->Delete();
// COMMENT: {3/5/2008 4:30:56 PM}	this->m_pNewCubeMapper->Delete();
// COMMENT: {3/5/2008 4:30:56 PM}	this->m_pNewCube->Delete();
// COMMENT: {3/5/2008 4:30:56 PM}	this->m_pNewCubeActor  = 0;
// COMMENT: {3/5/2008 4:30:56 PM}	this->m_pNewCubeMapper = 0;
// COMMENT: {3/5/2008 4:30:56 PM}	this->m_pNewCube       = 0;
// COMMENT: {3/5/2008 4:30:56 PM}
// COMMENT: {3/5/2008 4:30:56 PM}	// render
// COMMENT: {3/5/2008 4:30:56 PM}	this->GetDocument()->UpdateAllViews(0);
}

BOOL CWPhastView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (vtkInteractorStyleTrackballCameraEx *style = vtkInteractorStyleTrackballCameraEx::SafeDownCast(this->InteractorStyle))
	{
		if (style->GetPickWithMouse())
		{
			::SetCursor(AfxGetApp()->LoadCursor(IDC_3DGARROW));
			return TRUE;
		}
	}	

	if (this->CreatingNewWell() || this->CreatingNewRiver())
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_NULL));
		return TRUE;
	}

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

// COMMENT: {9/7/2005 4:29:33 PM}	if (this->CreatingNewRiver() && nHitTest == HTCLIENT)
// COMMENT: {9/7/2005 4:29:33 PM}	{
// COMMENT: {9/7/2005 4:29:33 PM}		::SetCursor(AfxGetApp()->LoadCursor(IDC_NULL));
// COMMENT: {9/7/2005 4:29:33 PM}		return TRUE;
// COMMENT: {9/7/2005 4:29:33 PM}	}

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

	if (CWPhastDoc* pDoc = this->GetDocument())
	{
		if (pDoc->OnSetCursor(pWnd, nHitTest, message))
		{
			return TRUE;
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

void CWPhastView::OnViewFromPrevDirection()
{
	switch (this->m_ViewFromDirection)
	{
	case ID_VIEW_FROM_NX:
		this->OnViewFromPz();
		return;
	case ID_VIEW_FROM_NY:
		this->OnViewFromNx();
		return;
	case ID_VIEW_FROM_PX:
		this->OnViewFromNy();
		return;
	case ID_VIEW_FROM_PY:
		this->OnViewFromPx();
		return;
	case ID_VIEW_FROM_NZ:
		this->OnViewFromPy();
		return;
	case ID_VIEW_FROM_PZ:
		this->OnViewFromNz();
		return;
	}
}

void CWPhastView::Select(vtkProp *pProp)
{
	// hide zone widget
	//
	if (this->BoxWidget) this->BoxWidget->Off();

	// hide well widget
	//
	if (this->PointWidget) this->PointWidget->Off();

	// disable rivers
	//
	if (vtkPropCollection *pPropCollection = this->GetDocument()->GetPropAssemblyRivers()->GetParts())
	{
		vtkProp *pProp = 0;
		pPropCollection->InitTraversal();
		for (; (pProp = pPropCollection->GetNextProp()); )
		{
			if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(pProp))
			{
				pRiverActor->ClearSelection();
				pRiverActor->Off();
			}
		}
	}

	// Highlight Prop (clear selection)
	//
	if (!pProp) this->HighlightProp(pProp);

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

	if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(pProp))
	{
		if (!pZoneActor->GetDefault() && this->BoxWidget)
		{
			//{{ {5/8/2006 4:25:45 PM}
			this->HighlightProp(this->CurrentProp = NULL);
			//}} {5/8/2006 4:25:45 PM}

			// Reset BoxWidget
			//
			this->BoxWidget->SetProp3D(pZoneActor);
			this->BoxWidget->PlaceWidget();
			this->BoxWidget->On();
		}
		//{{ {5/3/2006 5:35:09 PM}
		else if (pZoneActor->GetDefault())
		{
			// Highlight Prop
			//
			this->HighlightProp(pProp);
		}
		//}} {5/3/2006 5:35:09 PM}
	}
	else if (CWellActor *pWellActor = CWellActor::SafeDownCast(pProp))
	{
		if (this->PointWidget)
		{
			// Reset PointWidget
			//
			this->PointWidget->TranslationModeOff();
			this->PointWidget->SetProp3D(pWellActor);
			this->PointWidget->PlaceWidget();
			ASSERT(pWellActor == this->PointWidget->GetProp3D());
			this->PointWidget->TranslationModeOn();
			this->PointWidget->On();
		}
	}
	else if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(pProp))
	{
		// Highlight river
		//
		this->HighlightProp(pProp);

		pRiverActor->On();
	}
	else if (pProp)
	{
		ASSERT(FALSE); // unknown prop type
	}

	// redraw windows
	//
	this->GetDocument()->UpdateAllViews(this);
}

void CWPhastView::ClearSelection(void)
{
	this->Select(0);
}

void CWPhastView::ParallelProjectionOff(void)
{
	if (vtkRenderer *renderer = this->GetRenderer())
	{
		if (vtkCamera *camera = renderer->GetActiveCamera())
		{
			camera->ParallelProjectionOff();
		}
	}
}

void CWPhastView::ParallelProjectionOn(void)
{
	if (vtkRenderer *renderer = this->GetRenderer())
	{
		if (vtkCamera *camera = renderer->GetActiveCamera())
		{
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
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->InteractorStyle))
	{
		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style))
		{
			style = switcher->GetCurrentStyle();
		}
		style->SetInteractor(0);
	}

// COMMENT: {9/9/2005 4:45:27 PM}	// hide BoxWidget
// COMMENT: {9/9/2005 4:45:27 PM}	//
// COMMENT: {9/9/2005 4:45:27 PM}	// Note: This is reqd because the widget will 
// COMMENT: {9/9/2005 4:45:27 PM}	// recieve all the mouse input
// COMMENT: {9/9/2005 4:45:27 PM}	this->BoxWidget->Off();
// COMMENT: {9/9/2005 4:45:27 PM}	this->PointWidget->Off();
}

void CWPhastView::CancelNewWell(void)
{
	if (this->CreatingNewWell())
	{
		this->EndNewWell();
	}
}

void CWPhastView::EndNewWell(void)
{
	// reset cursor
	//
	this->m_pCursor3DActor->VisibilityOff();

	// reattach interactor
	//
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->InteractorStyle))
	{
		if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style))
		{
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
	if (this->m_pWellActor)
	{
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
		this->CancelMode();
		this->StartNewWell();
	}
}

void CWPhastView::OnUpdateToolsNewWell(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	if (this->CreatingNewWell())
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
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
	// Added GetSaveHwnd in order to avoid assertion that occurs when
	// the user is in the middle of creating a new river and selects
	// file->exit
	if (this->GetSafeHwnd())
	{
		CView::OnUpdate(pSender, lHint, pHint);

		// Add your specialized code here and/or call the base class
		this->Invalidate();
		this->RedrawWindow();
	}
}

void CWPhastView::Update(IObserver* pSender, LPARAM lHint, CObject* /*pHint*/, vtkObject* pObject)
{
	ASSERT(pSender != this);

	switch (lHint)
	{
	case WPN_NONE:
		break;
	case WPN_SELCHANGED:
		if (this->CurrentProp = vtkProp::SafeDownCast(pObject))
		{
			this->Select(this->CurrentProp);
		}
		else if (pObject == 0)
		{
			this->CurrentProp = NULL;
			this->ClearSelection();
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
		if (this->PointWidget->GetEnabled())
		{
			if (vtkProp3D* pProp3D = this->PointWidget->GetProp3D())
			{
				this->PointWidget->TranslationModeOff();
				this->PointWidget->SetProp3D(0);
				this->PointWidget->SetProp3D(pProp3D);
				this->PointWidget->PlaceWidget();
				ASSERT(pProp3D == this->PointWidget->GetProp3D());
				this->PointWidget->TranslationModeOn();
				this->PointWidget->On();
			}
		}
		if (this->CreatingNewRiver())
		{
			this->m_pRiverActor->SetScale(this->GetDocument()->GetScale());
		}
		//{{ {5/8/2006 4:00:51 PM}
		if (this->BoxWidget->GetEnabled())
		{
			if (vtkProp3D* pProp3D = this->BoxWidget->GetProp3D())
			{
				this->BoxWidget->PlaceWidget();
			}
		}
		if (this->CurrentProp) this->Select(this->CurrentProp);
		//}} {5/8/2006 4:00:51 PM}

// COMMENT: {5/8/2006 4:35:15 PM}		//{{ {4/25/2006 4:41:58 PM}
// COMMENT: {5/8/2006 4:35:15 PM}		this->ResetCamera();
		this->RedrawWindow();
// COMMENT: {5/8/2006 4:35:15 PM}		//}} {4/25/2006 4:41:58 PM}
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
		this->CancelMode();
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
		this->m_pRiverActor->AddObserver(CRiverActor::CancelNewRiverEvent, RiverCallbackCommand);

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
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->InteractorStyle))
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
	this->BoxWidget->Off();
	this->PointWidget->Off();
}

void CWPhastView::EndNewRiver(void)
{
	// reattach interactor
	//
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->InteractorStyle))
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
		case CRiverActor::CancelNewRiverEvent:
			self->OnEndNewRiver(true);
			break;
		case CRiverActor::EndNewRiverEvent:
			self->OnEndNewRiver(false);
			break;
		}
	}
}

void CWPhastView::OnEndNewRiver(bool bCancel)
{
	// reattach interactor
	//
	if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->InteractorStyle))
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
		if (!bCancel)
		{
			if (this->m_pRiverActor->GetPointCount() > 1)
			{
				CRiver river = this->m_pRiverActor->GetRiver();
				CString title;
				title.Format("River %d Properties", river.n_user);

				CPropertySheet sheet(title);
				CRiverPropertyPage2 page;
				page.SetProperties(river);
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

bool CWPhastView::MovingGridLine()const
{
	return this->m_bMovingGridLine;
}

void CWPhastView::EndMoveGridLine()
{
	if (CGridActor* pGridActor = CGridActor::SafeDownCast(this->GetDocument()->GetGridActor()))
	{
		pGridActor->SetEnabled(0);
	}
	this->m_bMovingGridLine = false;
}

void CWPhastView::CancelMoveGridLine()
{
	if (this->MovingGridLine())
	{
		this->EndMoveGridLine();
	}
}

void CWPhastView::StartMoveGridLine()
{
	if (CGridActor* pGridActor = CGridActor::SafeDownCast(this->GetDocument()->GetGridActor()))
	{
		pGridActor->SetInteractor(this->m_RenderWindowInteractor);
		this->m_bMovingGridLine = true;
	}
}

void CWPhastView::OnToolsMoveVerLine()
{
	if (this->MovingGridLine())
	{
		this->CancelMoveGridLine();
	}
	else
	{
		this->CancelMode();
		this->StartMoveGridLine();
	}
}

void CWPhastView::OnUpdateToolsMoveVerLine(CCmdUI *pCmdUI)
{
	if (CGridActor* pGridActor = CGridActor::SafeDownCast(this->GetDocument()->GetGridActor()))
	{
		if (pGridActor->GetVisibility())
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}

		if (this->MovingGridLine())
		{
			pCmdUI->SetCheck(1);
		}
		else
		{
			pCmdUI->SetCheck(0);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
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

void CWPhastView::CancelMode(void)
{
	// River
	//
	this->CancelNewRiver();

	// Well
	//
	this->CancelNewWell();

#ifdef DEPRECATED
	// New Zone
	//
	this->CancelNewZone();
#endif

	// Move Grid Lines
	//
	this->CancelMoveGridLine();

	// Modify Grid
	//
	if (this->GetDocument())
	{
		this->GetDocument()->EndModifyGrid();
	}

	// New Zone
	//
	if (this->GetDocument())
	{
		this->GetDocument()->EndNewZone();
	}

	// New Wedge
	//
	if (this->GetDocument())
	{
		this->GetDocument()->EndNewWedge();
	}


	// Select Object
	//
	if (vtkInteractorStyleTrackballCameraEx *style = vtkInteractorStyleTrackballCameraEx::SafeDownCast(this->InteractorStyle))
	{
		style->SetPickWithMouse(0);
	}
}

bool CWPhastView::SelectingObject()const
{
	if (vtkInteractorStyleTrackballCameraEx *style = vtkInteractorStyleTrackballCameraEx::SafeDownCast(this->InteractorStyle))
	{
		if (style->GetPickWithMouse()) return true;
	}
	return false;
}

void CWPhastView::OnUpdateToolsSelectObject(CCmdUI *pCmdUI)
{
	if (this->InteractorStyle)
	{
		pCmdUI->Enable(TRUE);
	}
	if (this->SelectingObject())
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CWPhastView::OnToolsSelectObject()
{
	if (vtkInteractorStyleTrackballCameraEx *style = vtkInteractorStyleTrackballCameraEx::SafeDownCast(this->InteractorStyle))
	{
		if (this->SelectingObject())
		{
			this->CancelMode();
		}
		else
		{
			this->CancelMode();
			style->SetPickWithMouse(1);
		}
	}
}

BOOL CWPhastView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	if (CWPhastDoc* pDoc = this->GetDocument())
	{
		if (CPropertyTreeControlBar *pPropertyTreeControlBar = pDoc->GetPropertyTreeControlBar())
		{
// COMMENT: {8/1/2007 1:52:50 PM}			//{{
// COMMENT: {8/1/2007 1:52:50 PM}			if (CBoxPropertiesDialogBar *pBar = pDoc->GetBoxPropertiesDialogBar())
// COMMENT: {8/1/2007 1:52:50 PM}			{
// COMMENT: {8/1/2007 1:52:50 PM}				if (pBar->IsChild(CWnd::GetFocus()))
// COMMENT: {8/1/2007 1:52:50 PM}				{
// COMMENT: {8/1/2007 1:52:50 PM}					if (nCode == CN_COMMAND || nCode == CN_UPDATE_COMMAND_UI)
// COMMENT: {8/1/2007 1:52:50 PM}					{
// COMMENT: {8/1/2007 1:52:50 PM}						if (pBar->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
// COMMENT: {8/1/2007 1:52:50 PM}						{
// COMMENT: {8/1/2007 1:52:50 PM}							return TRUE;
// COMMENT: {8/1/2007 1:52:50 PM}						}
// COMMENT: {8/1/2007 1:52:50 PM}					}
// COMMENT: {8/1/2007 1:52:50 PM}					return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
// COMMENT: {8/1/2007 1:52:50 PM}				}
// COMMENT: {8/1/2007 1:52:50 PM}			}
// COMMENT: {8/1/2007 1:52:50 PM}			//}}
			if (nCode == CN_COMMAND || nCode == CN_UPDATE_COMMAND_UI)
			{
				if (pPropertyTreeControlBar->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				{
					return TRUE;
				}
			}
		}
	}
	return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CWPhastView::SizeHandles(double size)
{
	if (CWPhastDoc* pDoc = this->GetDocument())
	{
		pDoc->SizeHandles(size);

		// update well widget
		//
		if (this->PointWidget && this->PointWidget->GetProp3D())
		{
// COMMENT: {5/3/2006 5:24:47 PM}			this->HighlightProp(this->PointWidget->GetProp3D());
// COMMENT: {5/3/2006 5:30:39 PM}			this->HighlightProp(NULL);
			this->PointWidget->PlaceWidget();
		}
	}
}
