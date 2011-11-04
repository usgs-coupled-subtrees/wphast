// WPhastView.cpp : implementation of the CWPhastView class
//

#include "stdafx.h"
#include <map>
#include "WPhast.h"

#include "WPhastDoc.h"
#include "WPhastView.h"
#include "PropertyTreeControlBar.h"

#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
#include <vtkMFCWindow.h>
#else
#include <vtkWin32OpenGLRenderWindow.h>
#endif

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
#include <vtkProperty.h>
#include <vtkCylinderSource.h>

#include <vtkPropAssembly.h>
#include <vtkPropCollection.h>

#include "PrismWidget.h"
#include "ZoneActor.h"
#include "WellActor.h"
#include "RiverActor.h"
#include "DrainActor.h"
#include "GridLODActor.h"
#include "GridActor.h"
#include "PointConnectorCreateAction.h"
#include "RiverPropertyPage2.h"
#include "Grid.h"
#include "FlowOnly.h"
#include "ZonePrismResetAction.h"
#include "PointSelectionObject.h"
#include "Global.h"
#include "srcinput/Domain.h"
#include "NullPolyhedron.h"

#include <vtkImplicitPlaneWidget.h>


// #include "CreateZoneAction.h"
#include "ZoneCreateAction.h"

#define CANCEL_WHEN_GRID_UNCHECKED

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
	ON_COMMAND(ID_VIEW_FROM_MAP_NZ, OnViewFromMapNz)
	ON_COMMAND(ID_VIEW_FROM_MAP_PZ, OnViewFromMapPz)
	ON_COMMAND(ID_VIEW_FROM_NEXT_DIRECTION, OnViewFromNextDirection)
	ON_COMMAND(ID_VIEW_FROM_PREV_DIRECTION, OnViewFromPrevDirection)
	// Wells
	ON_COMMAND(ID_TOOLS_NEWWELL, OnToolsNewWell)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWWELL, OnUpdateToolsNewWell)
	// River
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWRIVER, OnUpdateToolsNewRiver)
	ON_COMMAND(ID_TOOLS_NEWRIVER, OnToolsNewRiver)
// COMMENT: {9/8/2009 8:43:12 PM}	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	// Move Grid Line
	ON_COMMAND(ID_TOOLS_MOVE_VER_LINE, OnToolsMoveVerLine)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MOVE_VER_LINE, OnUpdateToolsMoveVerLine)
	// Rotate Grid
	ON_COMMAND(ID_TOOLS_ROTATE_GRID, &CWPhastView::OnToolsRotateGrid)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ROTATE_GRID, &CWPhastView::OnUpdateToolsRotateGrid)
// COMMENT: {8/29/2005 6:46:54 PM}	ON_UPDATE_COMMAND_UI(ID_TOOLS_MODIFYGRID, OnUpdateToolsModifyGrid)
// COMMENT: {8/29/2005 6:46:54 PM}	ON_COMMAND(ID_TOOLS_MODIFYGRID, OnToolsModifyGrid)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTOBJECT, OnUpdateToolsSelectObject)
	ON_COMMAND(ID_TOOLS_SELECTOBJECT, OnToolsSelectObject)
	ON_COMMAND(ID_CANCEL_MODE, &CWPhastView::OnCancelMode)
	ON_COMMAND(ID_VIEW_RESETVIEWPOINT, &CWPhastView::OnViewResetviewpoint)
END_MESSAGE_MAP()

// CWPhastView construction/destruction

CWPhastView::CWPhastView()
: MFCWindow(0)
, Renderer(0)
, BoxWidget(0)
, PointWidget(0)
, PrismWidget(0)
, ViewVTKCommand(0)
, InteractorStyle(0)
, bMovingGridLine(false)
, bRotatingGrid(false)
, Cursor3D(0)
, Cursor3DMapper(0)
, Cursor3DActor(0)
, ViewFromDirection(ID_VIEW_FROM_PZ)
, WellActor(0)
, RiverActor(0)
, CurrentProp(0)
, RiverCallbackCommand(0)
, PrismWidgetCallbackCommand(0)
{
	// Create the renderer
	//
	// This is a hack.  It forces the renderer to delay deletion.
	// Something is causing the ReferenceCount to be decreased and
	// is deleted early (see dtor)
	//
	this->Renderer = vtkRenderer::New();
	this->Renderer->SetReferenceCount(10);    // delay deletion

	// Command/Observer
	//
	this->ViewVTKCommand = CViewVTKCommand::New(this);

	// Create the BoxWidget
	//
	this->BoxWidget = vtkBoxWidgetEx::New();
	this->BoxWidget->SetPlaceFactor(1.0);
	this->BoxWidget->AddObserver(vtkCommand::InteractionEvent, this->ViewVTKCommand);
	this->BoxWidget->AddObserver(vtkCommand::EndInteractionEvent, this->ViewVTKCommand);
	this->BoxWidget->AddObserver(vtkCommand::StartInteractionEvent, this->ViewVTKCommand);

	// Create the PointWidget2
	//
	this->PointWidget = vtkPointWidget2::New();
	this->PointWidget->AddObserver(vtkCommand::InteractionEvent, this->ViewVTKCommand);
	this->PointWidget->AddObserver(vtkCommand::EndInteractionEvent, this->ViewVTKCommand);
	this->PointWidget->AddObserver(vtkCommand::StartInteractionEvent, this->ViewVTKCommand);

	// Create the PrismWidget listener
	//
	this->PrismWidgetCallbackCommand = vtkCallbackCommand::New();
	this->PrismWidgetCallbackCommand->SetClientData(this);
	this->PrismWidgetCallbackCommand->SetCallback(CWPhastView::PrismWidgetListener);

	// Create the PrismWidget
	//
	this->PrismWidget = CPrismWidget::New();
	this->PrismWidget->AddObserver(vtkCommand::EndInteractionEvent, this->PrismWidgetCallbackCommand);
	this->PrismWidget->AddObserver(CPrismWidget::InsertPointEvent, this->PrismWidgetCallbackCommand);
	this->PrismWidget->AddObserver(CPrismWidget::DeletePointEvent, this->PrismWidgetCallbackCommand);

	// 3D Cursor
	//
	this->Cursor3D = vtkCursor3D::New();
	this->Cursor3D->XShadowsOff();
	this->Cursor3D->YShadowsOff();
	this->Cursor3D->ZShadowsOff();
	this->Cursor3D->OutlineOff();

	this->Cursor3DMapper = vtkPolyDataMapper::New();
	this->Cursor3DMapper->SetInput(this->Cursor3D->GetOutput());

	this->Cursor3DActor = vtkActor::New();
	this->Cursor3DActor->SetMapper(this->Cursor3DMapper);
	this->Cursor3DActor->SetPosition(0, 0, 0);
	this->Cursor3DActor->VisibilityOff();
}

CWPhastView::~CWPhastView()
{
// COMMENT: {9/18/2009 6:41:38 PM}	this->CancelMode();

	if (this->GetInteractor())
	{
		this->GetInteractor()->Disable();
	}

	if (this->BoxWidget)
	{
		this->BoxWidget->SetEnabled(0);
		this->BoxWidget->SetInteractor(0);
		this->BoxWidget->Delete();
	}

	if (this->PointWidget)
	{
		this->PointWidget->SetEnabled(0);
		this->PointWidget->SetInteractor(0);
		this->PointWidget->Delete();
	}

	if (this->PrismWidget)
	{
		this->PrismWidget->SetEnabled(0);
		this->PrismWidget->SetInteractor(0);
		this->PrismWidget->Delete();
	}

	// 3D Cursor
	if (this->Cursor3D)
	{
		this->Cursor3D->Delete();
	}
	if (this->Cursor3DMapper)
	{
		this->Cursor3DMapper->Delete();
	}
	if (this->Cursor3DActor)
	{
		this->Cursor3DActor->Delete();
	}


	// Well actor
	if (this->WellActor)
	{
		this->WellActor->Delete();
		this->WellActor = 0;
	}

	// River actor
	if (this->RiverActor)
	{
		this->Renderer->RemoveViewProp(this->RiverActor);
		this->RiverActor->Delete();
		this->RiverActor = 0;
	}

	// River listener
	if (this->RiverCallbackCommand)
	{
		this->RiverCallbackCommand->Delete();
		this->RiverCallbackCommand = 0;
	}

	// PrismWidget listener
	if (this->PrismWidgetCallbackCommand)
	{
		this->PrismWidgetCallbackCommand->Delete();
		this->PrismWidgetCallbackCommand = 0;
	}
	if (this->bMovingGridLine)
	{
		if (CGridLODActor* pGridLODActor = CGridLODActor::SafeDownCast(this->GetDocument()->GetGridActor()))
		{
			pGridLODActor->SetEnabled(0);
		}
	}

// COMMENT: {9/18/2009 9:57:24 PM}#if !((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
	// this allows RemoveObserver of the Interactor of CPointConnectorActor to be called for the EventCallbackCommand
	// fixing a crash that sometimes occurred during program exit
	//
	if (CWPhastDoc *pDoc = this->GetDocument())
	{
		if (vtkPropCollection *pPropCollection = this->GetDocument()->GetPropAssemblyRivers()->GetParts())
		{
			vtkProp *pProp = 0;
			vtkCollectionSimpleIterator csi;
			pPropCollection->InitTraversal(csi);
			for (; (pProp = pPropCollection->GetNextProp(csi)); )
			{
				if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(pProp))
				{
					pRiverActor->SetInteractor(0);
				}
			}
		}
		if (vtkPropCollection *pPropCollection = this->GetDocument()->GetPropAssemblyDrains()->GetParts())
		{
			vtkProp *pProp = 0;
			vtkCollectionSimpleIterator csi;
			pPropCollection->InitTraversal(csi);
			for (; (pProp = pPropCollection->GetNextProp(csi)); )
			{
				if (CDrainActor *pDrainActor = CDrainActor::SafeDownCast(pProp))
				{
					pDrainActor->SetInteractor(0);
				}
			}
		}
	}
// COMMENT: {9/18/2009 9:57:27 PM}#endif

#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
// COMMENT: {9/18/2009 8:02:20 PM}	if (this->MFCWindow)
// COMMENT: {9/18/2009 8:02:20 PM}	{
// COMMENT: {9/18/2009 8:02:20 PM}		delete this->MFCWindow;
// COMMENT: {9/18/2009 8:02:20 PM}	}
	if (this->InteractorStyle)
	{
		this->InteractorStyle->Delete();
	}
// {{ {9/18/2009 8:02:26 PM}
	if (this->MFCWindow)
	{
		delete this->MFCWindow;
	}
// }} {9/18/2009 8:02:26 PM}
	if (this->ViewVTKCommand)
	{
		this->ViewVTKCommand->Delete();
	}

	if (this->Renderer)
	{
		TRACE("Deleting this->Renderer = %p\n", this->Renderer);
		TRACE("this->Renderer->GetReferenceCount() = %d\n",  this->Renderer->GetReferenceCount());
		this->Renderer->SetReferenceCount(1);
		this->Renderer->Delete();
	}

#else
	// Delete the the renderer, window and interactor objects.
	this->Renderer->Delete();
	this->m_RenderWindow->Delete();
	this->m_RenderWindowInteractor->Delete();
#endif
}

// COMMENT: {9/8/2009 8:50:25 PM}BOOL CWPhastView::PreCreateWindow(CREATESTRUCT& cs)
// COMMENT: {9/8/2009 8:50:25 PM}{
// COMMENT: {9/8/2009 8:50:25 PM}	// Modify the Window class or styles here by modifying
// COMMENT: {9/8/2009 8:50:25 PM}	//  the CREATESTRUCT cs
// COMMENT: {9/8/2009 8:50:25 PM}
// COMMENT: {9/8/2009 8:50:25 PM}	return CView::PreCreateWindow(cs);
// COMMENT: {9/8/2009 8:50:25 PM}}

// CWPhastView drawing
void CWPhastView::OnDraw(CDC* pDC)
{
	TRACE("CWPhastView::OnDraw\n");
	CWPhastDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
	if (this->MFCWindow)
	{
		if (pDC->IsPrinting())
		{
			this->MFCWindow->DrawDC(pDC);
		}
	}
#else
	if ( !this->m_RenderWindowInteractor->GetInitialized() )
	{
		CRect rect;

		this->GetClientRect(&rect);
		this->m_RenderWindowInteractor->Initialize();
		this->m_RenderWindow->SetSize(rect.right-rect.left,rect.bottom-rect.top);

		this->Renderer->ResetCamera();
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
#endif
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

#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
vtkRenderWindowInteractor* CWPhastView::GetInteractor(void)
{
	if (this->MFCWindow)
	{
		return this->MFCWindow->GetInteractor();
	};
	return 0;
}
#endif

// CWPhastView message handlers

int CWPhastView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	TRACE("CWPhastView::OnCreate\n");

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

#if !((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))

	if (!this->m_RenderWindow)
		return -1;

	if (!this->Renderer)
		return -1;

	if (!this->m_RenderWindowInteractor)
		return -1;

	this->m_RenderWindow->AddRenderer(this->Renderer);
#if ((VTK_MAJOR_VERSION <= 4) && (VTK_MINOR_VERSION <= 2))
	// Note: No double-click events will be received
	//
	this->m_RenderWindow->SetParentId(this->GetSafeHwnd());
#else
	this->m_RenderWindow->SetWindowId(this->GetSafeHwnd());
#endif
	this->m_RenderWindowInteractor->SetRenderWindow(this->m_RenderWindow);

#endif

	// add listener
	//
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

#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
	if (this->MFCWindow)
	{
		this->MFCWindow->MoveWindow(0, 0, cx, cy);
	}
	return;
#endif

#if ((VTK_MAJOR_VERSION <= 4) && (VTK_MINOR_VERSION <= 2))
	CRect rect;
	this->GetClientRect(&rect);
	this->m_RenderWindow->SetSize(rect.right-rect.left,rect.bottom-rect.top);
#endif
}

BOOL CWPhastView::OnEraseBkgnd(CDC* pDC)
{
	static bool bFirst = true;
	if (bFirst)
	{
		// this is reqd in order to reduce flicker under windows 7
		//

		// Set brush to desired background color
		CBrush backBrush(RGB(0, 0, 0));

		// Save old brush
		CBrush* pOldBrush = pDC->SelectObject(&backBrush);

		CRect rect;
		pDC->GetClipBox(&rect);     // Erase the area needed

		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);
		bFirst = false;
	}
	TRACE("CWPhastView::OnEraseBkgnd\n");
	return TRUE;
}

#if !((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
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
				case 'R' :
				case 'r' :
					TRACE("Reset camera\n");
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
	ASSERT(this->Renderer);
	this->Renderer->SetBackground(this->BackgroundColor);

	/* TODO:
	if (this->m_pNewCubeActor && this->m_pNewCubeActor->GetVisibility()) {
		this->Renderer->AddViewProp(this->m_pNewCubeActor);
	}
	*/

	//{{
	// this->Renderer->RemoveAllProps(); // this removes the selection actor and the BoxWidget
	//}}

	this->Renderer->AddViewProp(this->Cursor3DActor);

	// add props to renderer
	if ( vtkPropCollection *props = this->GetDocument()->GetPropCollection() ) {
		vtkCollectionSimpleIterator csi;
		props->InitTraversal(csi);
		for (int i = 0; i < props->GetNumberOfItems(); ++i) {
			vtkProp* prop = props->GetNextProp(csi);
			//{{ {4/25/2006 10:15:10 PM}
			if (prop)
			{
				this->Renderer->AddViewProp(prop);
			}
			//}} {4/25/2006 10:15:10 PM}

			if (this->m_bResetCamera) {
				this->Renderer->ResetCamera();
				this->m_bResetCamera = false;
			}

			/* TODO:
			if (vtkCubeAxesActor2D *pCubeAxisActor2D = vtkCubeAxesActor2D::SafeDownCast(prop)) {
				pCubeAxisActor2D->SetCamera(this->Renderer->GetActiveCamera());
			}
			*/
		}
	}
	//{{
	this->Renderer->ResetCameraClippingRange();
	//}}
}
#endif //!((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))


// COMMENT: {9/8/2009 8:50:53 PM}void CWPhastView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
// COMMENT: {9/8/2009 8:50:53 PM}{
// COMMENT: {9/8/2009 8:50:53 PM}	//{{FIXME
// COMMENT: {9/8/2009 8:50:53 PM}	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
// COMMENT: {9/8/2009 8:50:53 PM}	//{{ {5/4/2006 5:38:44 PM}
// COMMENT: {9/8/2009 8:50:53 PM}	///this->Invalidate();
// COMMENT: {9/8/2009 8:50:53 PM}	///this->Pipeline();
// COMMENT: {9/8/2009 8:50:53 PM}	//}} {5/4/2006 5:38:44 PM}
// COMMENT: {9/8/2009 8:50:53 PM}	return;
// COMMENT: {9/8/2009 8:50:53 PM}
// COMMENT: {9/8/2009 8:50:53 PM}	// Note: The code below causes the Zone dimensions to be disabled
// COMMENT: {9/8/2009 8:50:53 PM}	// when the hardware pick fails
// COMMENT: {9/8/2009 8:50:53 PM}
// COMMENT: {9/8/2009 8:50:53 PM}	//}}FIXME
// COMMENT: {9/8/2009 8:50:53 PM}
// COMMENT: {9/8/2009 8:50:53 PM}#if !((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
// COMMENT: {9/8/2009 8:50:53 PM}	// Add your specialized code here and/or call the base class
// COMMENT: {9/8/2009 8:50:53 PM}	TRACE("CWPhastView::OnActivateView\n");
// COMMENT: {9/8/2009 8:50:53 PM}	//{{
// COMMENT: {9/8/2009 8:50:53 PM}	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( this->GetRenderWindowInteractor()->GetPicker() )) {
// COMMENT: {9/8/2009 8:50:53 PM}		if (vtkProp3D* prop = picker->GetProp3D()) {
// COMMENT: {9/8/2009 8:50:53 PM}			// Update StatusBar
// COMMENT: {9/8/2009 8:50:53 PM}			//
// COMMENT: {9/8/2009 8:50:53 PM}			if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
// COMMENT: {9/8/2009 8:50:53 PM}				double* bounds = prop->GetBounds();
// COMMENT: {9/8/2009 8:50:53 PM}				TCHAR buffer[80];
// COMMENT: {9/8/2009 8:50:53 PM}				// ::_sntprintf(buffer, 80, "%6.4f x %6.4f x %6.4f", fabs(bounds[1] - bounds[0]), fabs(bounds[3] - bounds[2]), fabs(bounds[5] - bounds[4]));
// COMMENT: {9/8/2009 8:50:53 PM}				::_sntprintf(buffer, 80, "%6.4g x %6.4g x %6.4g", fabs(bounds[1] - bounds[0]), fabs(bounds[3] - bounds[2]), fabs(bounds[5] - bounds[4]));
// COMMENT: {9/8/2009 8:50:53 PM}
// COMMENT: {9/8/2009 8:50:53 PM}				// FIXME: This will cause a buffer overrun if the transform is invalid
// COMMENT: {9/8/2009 8:50:53 PM}				// FIXME: WHY ARE THE BOUNDS OUT OF RANGE???
// COMMENT: {9/8/2009 8:50:53 PM}				// ::_stprintf(buffer, "%6.4f x %6.4f x %6.4f", fabs(bounds[1] - bounds[0]), fabs(bounds[3] - bounds[2]), fabs(bounds[5] - bounds[4]));
// COMMENT: {9/8/2009 8:50:53 PM}				//
// COMMENT: {9/8/2009 8:50:53 PM}				pWnd->SetWindowText(buffer);
// COMMENT: {9/8/2009 8:50:53 PM}
// COMMENT: {9/8/2009 8:50:53 PM}			}
// COMMENT: {9/8/2009 8:50:53 PM}
// COMMENT: {9/8/2009 8:50:53 PM}			////// Update BoxPropertiesDialogBar
// COMMENT: {9/8/2009 8:50:53 PM}			//////
// COMMENT: {9/8/2009 8:50:53 PM}			////if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) ) {
// COMMENT: {9/8/2009 8:50:53 PM}			////	pBar->Set(this, prop);
// COMMENT: {9/8/2009 8:50:53 PM}			////}
// COMMENT: {9/8/2009 8:50:53 PM}		}
// COMMENT: {9/8/2009 8:50:53 PM}		else {
// COMMENT: {9/8/2009 8:50:53 PM}			// Update StatusBar
// COMMENT: {9/8/2009 8:50:53 PM}			//
// COMMENT: {9/8/2009 8:50:53 PM}			if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
// COMMENT: {9/8/2009 8:50:53 PM}				CString status;
// COMMENT: {9/8/2009 8:50:53 PM}				status.LoadString(AFX_IDS_IDLEMESSAGE);
// COMMENT: {9/8/2009 8:50:53 PM}				pWnd->SetWindowText(status);
// COMMENT: {9/8/2009 8:50:53 PM}			}
// COMMENT: {9/8/2009 8:50:53 PM}
// COMMENT: {9/8/2009 8:50:53 PM}			// Update BoxPropertiesDialogBar
// COMMENT: {9/8/2009 8:50:53 PM}			//
// COMMENT: {9/8/2009 8:50:53 PM}			if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS))) {
// COMMENT: {9/8/2009 8:50:53 PM}				pBar->Set(NULL, NULL, this->GetDocument()->GetUnits());
// COMMENT: {9/8/2009 8:50:53 PM}			}
// COMMENT: {9/8/2009 8:50:53 PM}		}
// COMMENT: {9/8/2009 8:50:53 PM}	}
// COMMENT: {9/8/2009 8:50:53 PM}	//}}
// COMMENT: {9/8/2009 8:50:53 PM}
// COMMENT: {9/8/2009 8:50:53 PM}	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
// COMMENT: {9/8/2009 8:50:53 PM}#endif // !((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
// COMMENT: {9/8/2009 8:50:53 PM}}

void CWPhastView::ResetCamera(void)
{
#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
	if (this->GetRenderer())
	{
		this->GetRenderer()->ResetCamera();
		this->GetRenderer()->ResetCameraClippingRange();
	}	
#else
	this->m_bResetCamera = true;
#endif
}

void CWPhastView::ResetCamera(double bounds[6])
{
	if (this->GetRenderer())
	{
		this->GetRenderer()->ResetCamera(bounds);
		this->GetRenderer()->ResetCameraClippingRange();
	}
}

void CWPhastView::ResetCamera(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
	double bounds[6];

	bounds[0] = xmin;
	bounds[1] = xmax;
	bounds[2] = ymin;
	bounds[3] = ymax;
	bounds[4] = zmin;
	bounds[5] = zmax;

	this->ResetCamera(bounds);
}

void CWPhastView::OnInitialUpdate()
{
	TRACE("CWPhastView::OnInitialUpdate\n");
	CView::OnInitialUpdate();

#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
// COMMENT: {9/16/2009 10:45:17 PM}	if (this->MFCWindow)
// COMMENT: {9/16/2009 10:45:17 PM}	{
// COMMENT: {9/16/2009 10:45:17 PM}		this->ClearSelection();
// COMMENT: {9/16/2009 10:45:17 PM}		delete this->MFCWindow;
// COMMENT: {9/16/2009 10:45:17 PM}	}
// COMMENT: {9/16/2009 10:45:17 PM}	this->MFCWindow = new vtkMFCWindow(this);
// COMMENT: {9/16/2009 10:45:17 PM}	this->MFCWindow->GetRenderWindow()->AddRenderer(this->Renderer);
	if (!this->MFCWindow)
	{
		this->MFCWindow = new vtkMFCWindow(this);
		this->MFCWindow->GetRenderWindow()->AddRenderer(this->Renderer);

		ASSERT(this->GetInteractor());
		if (this->GetInteractor())
		{
			// set picker
			//
			vtkPropPickerFixed* picker = vtkPropPickerFixed::New();
			this->GetInteractor()->SetPicker(picker);
			picker->Delete();

			// set interactor
			//
			ASSERT(this->InteractorStyle == 0);
			this->InteractorStyle = vtkInteractorStyleTrackballCameraEx::New();
			this->GetInteractor()->SetInteractorStyle(this->InteractorStyle);

			// box widget (cubes)
			//
			ASSERT(this->BoxWidget);
			if (this->BoxWidget)
			{
				this->BoxWidget->SetInteractor(this->GetInteractor());
				if (vtkBoxWidgetEx *widget = vtkBoxWidgetEx::SafeDownCast(this->BoxWidget))
				{
					widget->HexPickerEnabledOff();
				}
			}

			// point widget (wells)
			//
			ASSERT(this->PointWidget);
			if (this->PointWidget)
			{
				this->PointWidget->SetInteractor(this->GetInteractor());
			}

			// prism widget
			//
			ASSERT(this->PrismWidget);
			if (this->PrismWidget)
			{
				this->PrismWidget->SetInteractor(this->GetInteractor());
			}

			// Add Observers
			//
			ASSERT(this->ViewVTKCommand);

			// mouse events (priority 1.0 > 0.0 (Interactor) so that well mouse down can veto)
			//
			this->GetInteractor()->AddObserver(vtkCommand::MouseMoveEvent, this->ViewVTKCommand, 1.0);
			this->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, this->ViewVTKCommand, 1.0);
			this->GetInteractor()->AddObserver(vtkCommand::LeftButtonReleaseEvent, this->ViewVTKCommand, 1.0);

			// keyboard events
			//
			this->GetInteractor()->AddObserver(vtkCommand::KeyPressEvent, this->ViewVTKCommand);

			// pick events
			//
			this->GetInteractor()->AddObserver(vtkCommand::EndPickEvent, this->ViewVTKCommand);

			// camera events
			//
			vtkCamera* pCamera = this->Renderer->GetActiveCamera();
			ASSERT(pCamera);
			pCamera->AddObserver(vtkCommand::ModifiedEvent, this->ViewVTKCommand);

			// add cursor
			//
			this->Renderer->AddViewProp(this->Cursor3DActor);
		}
	}
#endif
}

void CWPhastView::DeleteContents(void)
{
	//{{ {9/23/2009 9:29:09 PM}
	this->GetRenderer()->RemoveAllViewProps();
	this->Cursor3DActor->SetVisibility(0);
	this->GetRenderer()->AddViewProp(this->Cursor3DActor);
	//}} {9/23/2009 9:29:09 PM}

	// clear the selection
	//
	this->ClearSelection();

	// reset the camera
	//
	vtkCamera* pCamera = this->Renderer->GetActiveCamera();
	pCamera->SetFocalPoint(0, 0, 0);
	pCamera->SetPosition(0, 0, 1);
	pCamera->SetViewUp(0, 1, 0);

	// cancel all modes
	//
	this->CancelMode();
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

	// set real-time zoom cursor
	// (ctrl + shift + left button is for zooming)
	if (::GetAsyncKeyState(VK_SHIFT) < 0 && ::GetAsyncKeyState(VK_CONTROL) && ::GetAsyncKeyState(VK_LBUTTON))
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_ZOOM_REAL));
		return TRUE;
	}

	// set real-time rotate cursor
	// (ctrl + left button for spinning)
	if (::GetAsyncKeyState(VK_CONTROL) && ::GetAsyncKeyState(VK_LBUTTON))
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_ROTATE));
		return TRUE;
	}

	// set real-time zoom cursor
	// (right button)
	if (::GetAsyncKeyState(VK_RBUTTON) < 0)
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_ZOOM_REAL));
		return TRUE;
	}

	// set real-time pan cursor
	// (middle button)
	if (::GetAsyncKeyState(VK_MBUTTON) < 0)
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_PAN_REAL));
		return TRUE;
	}

	// set real-time pan cursor
	// (shift + left button)
	if (::GetAsyncKeyState(VK_SHIFT) < 0 && ::GetAsyncKeyState(VK_LBUTTON))
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_PAN_REAL));
		return TRUE;
	}

	// these are set after (IDC_PAN_REAL) so that the pan cursor overides the null
	// cursor
	if (this->CreatingNewRiver())
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_NULL));
		return TRUE;
	}
	if (this->CreatingNewWell())
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_NULL));
		return TRUE;
	}

	// set 3D cursor
	//
	if (this->Cursor3DActor->GetVisibility() && nHitTest == HTCLIENT)
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_NULL));
		return TRUE;
	}

	if (CWPhastDoc* pDoc = this->GetDocument())
	{
		if (pDoc->OnSetCursor(pWnd, nHitTest, message))
		{
			return TRUE;
		}
		if (this->PrismWidget && this->PrismWidget->GetEnabled())
		{
			return this->PrismWidget->OnSetCursor(pWnd, nHitTest, message);
		}
	}

	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CWPhastView::OnUpdateViewFromNx(CCmdUI *pCmdUI)
{
	// Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromNx(void)
{
	// Add your command handler code here
	vtkCamera *camera = this->Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) +
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0] - dist, fp[1], fp[2]);
	camera->SetViewUp(0, 0, 1);
	camera->ComputeViewPlaneNormal();
	this->Renderer->ResetCameraClippingRange();
	this->ViewFromDirection = ID_VIEW_FROM_NX;
	this->Invalidate();
}

void CWPhastView::OnUpdateViewFromPx(CCmdUI *pCmdUI)
{
	// Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromPx(void)
{
	// Add your command handler code here
	vtkCamera *camera = this->Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) +
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0] + dist, fp[1], fp[2]);
	camera->SetViewUp(0, 0, 1);
	camera->ComputeViewPlaneNormal();
	this->Renderer->ResetCameraClippingRange();
	this->ViewFromDirection = ID_VIEW_FROM_PX;
	this->Invalidate();
}

void CWPhastView::OnUpdateViewFromNy(CCmdUI *pCmdUI)
{
	// Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromNy(void)
{
	// Add your command handler code here
	vtkCamera *camera = this->Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) +
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0], fp[1] - dist, fp[2]);
	camera->SetViewUp(0, 0, 1);
	camera->ComputeViewPlaneNormal();
	this->Renderer->ResetCameraClippingRange();
	this->ViewFromDirection = ID_VIEW_FROM_NY;
	this->Invalidate();
}

void CWPhastView::OnUpdateViewFromPy(CCmdUI *pCmdUI)
{
	// Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromPy(void)
{
	// Add your command handler code here
	vtkCamera *camera = this->Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) +
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0], fp[1] + dist, fp[2]);
	camera->SetViewUp(0, 0, 1);
	camera->ComputeViewPlaneNormal();
	this->Renderer->ResetCameraClippingRange();
	this->ViewFromDirection = ID_VIEW_FROM_PY;
	this->Invalidate();
}

void CWPhastView::OnUpdateViewFromNz(CCmdUI *pCmdUI)
{
	// Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromNz(void)
{
	// Add your command handler code here
	vtkCamera *camera = this->Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) +
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0], fp[1], fp[2] - dist);
	camera->SetViewUp(0, -1, 0);
	camera->ComputeViewPlaneNormal();
	this->Renderer->ResetCameraClippingRange();
	this->ViewFromDirection = ID_VIEW_FROM_NZ;
	this->Invalidate();
}

void CWPhastView::OnViewFromMapNz(void)
{
	// Add your command handler code here
	vtkCamera *camera = this->Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) +
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0], fp[1], fp[2] - dist);

	CGridKeyword gridKeyword = this->GetDocument()->GetGridKeyword();
	double angle = ((gridKeyword.m_grid_angle) / 180.) * acos(-1.0);
	camera->SetViewUp(-sin(angle), -cos(angle), 0);

	camera->ComputeViewPlaneNormal();
	this->Renderer->ResetCameraClippingRange();
	this->ViewFromDirection = ID_VIEW_FROM_NZ;
	this->Invalidate();
}

void CWPhastView::OnUpdateViewFromPz(CCmdUI *pCmdUI)
{
	// Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CWPhastView::OnViewFromPz(void)
{
	// Add your command handler code here
	vtkCamera *camera = this->Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) +
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0], fp[1], fp[2] + dist);
	camera->SetViewUp(0, 1, 0);
	camera->ComputeViewPlaneNormal();
	this->Renderer->ResetCameraClippingRange();
	this->ViewFromDirection = ID_VIEW_FROM_PZ;
	this->Invalidate();
}

void CWPhastView::OnViewFromMapPz(void)
{
	// Add your command handler code here
	vtkCamera *camera = this->Renderer->GetActiveCamera();
	double *pos = camera->GetPosition();
	double *fp = camera->GetFocalPoint();
	double dist = sqrt((pos[0]-fp[0])*(pos[0]-fp[0]) +
		(pos[1]-fp[1])*(pos[1]-fp[1]) + (pos[2]-fp[2])*(pos[2]-fp[2]));
	camera->SetPosition(fp[0], fp[1], fp[2] + dist);

	CGridKeyword gridKeyword = this->GetDocument()->GetGridKeyword();
	double angle = ((gridKeyword.m_grid_angle) / 180.) * acos(-1.0);
	camera->SetViewUp(sin(angle), cos(angle), 0);

	camera->ComputeViewPlaneNormal();
	this->Renderer->ResetCameraClippingRange();
	this->ViewFromDirection = ID_VIEW_FROM_PZ;
	this->Invalidate();
}

void CWPhastView::OnViewFromNextDirection(void)
{
	switch (this->ViewFromDirection)
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
	default:
		ASSERT(FALSE);
	}
}

void CWPhastView::OnViewFromPrevDirection()
{
	switch (this->ViewFromDirection)
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
	default:
		ASSERT(FALSE);
	}
}

void CWPhastView::Select(vtkProp *pProp)
{
	if (!this->MFCWindow) return;

	// hide zone widget
	//
	if (this->BoxWidget)
	{
		this->BoxWidget->Off();
		this->BoxWidget->SetProp3D(0);
	}

	// hide well widget
	//
	if (this->PointWidget)
	{
		this->PointWidget->Off();
		this->PointWidget->SetProp3D(0);
	}

	// hide prism widget
	//
	if (this->PrismWidget)
	{
		this->PrismWidget->Off();
		this->PrismWidget->SetProp3D(0);
	}


	// disable rivers
	//
	if (vtkPropCollection *pPropCollection = this->GetDocument()->GetPropAssemblyRivers()->GetParts())
	{
		vtkProp *pProp = 0;
		vtkCollectionSimpleIterator csi;
		pPropCollection->InitTraversal(csi);
		for (; (pProp = pPropCollection->GetNextProp(csi)); )
		{
			if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(pProp))
			{
				pRiverActor->ClearSelection();
				pRiverActor->Off();
			}
		}
	}

	// disable drains
	//
	if (vtkPropCollection *pPropCollection = this->GetDocument()->GetPropAssemblyDrains()->GetParts())
	{
		vtkProp *pProp = 0;
		vtkCollectionSimpleIterator csi;
		pPropCollection->InitTraversal(csi);
		for (; (pProp = pPropCollection->GetNextProp(csi)); )
		{
			if (CDrainActor *pDrainActor = CDrainActor::SafeDownCast(pProp))
			{
				pDrainActor->ClearSelection();
				pDrainActor->Off();
			}
		}
	}

	// Clear selection
	//
	if (!pProp) this->HighlightProp(0);

	// Set Picker
	//
	if (vtkAbstractPropPicker *pPicker = vtkAbstractPropPicker::SafeDownCast( this->GetInteractor()->GetPicker() ))
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
		// highlight even if not visible
		// (this may be inconsistent with other selectable objects)
		if (pZoneActor->GetVisibility() == 0)
		{
			// Highlight Prop
			//
			this->HighlightProp(pProp);

			// redraw windows
			//
			this->GetDocument()->UpdateAllViews(this);
			return;
		}

		Polyhedron::POLYHEDRON_TYPE n = pZoneActor->GetPolyhedron()->get_type();
		if (n == Polyhedron::PRISM)
		{
			if (Prism *p = dynamic_cast<Prism*>(pZoneActor->GetPolyhedron()))
			{
				Data_source::DATA_SOURCE_TYPE s = p->perimeter.Get_user_source_type();
				if (s == Data_source::POINTS || s == Data_source::NONE)
				{
					this->HighlightProp(this->CurrentProp = NULL);

					// Set PrismWidget
					//
					if (this->PrismWidget)
					{
						this->PrismWidget->SetProp3D(pZoneActor);
						this->PrismWidget->On();
					}
				}
				else
				{
					// Highlight Prop
					//
					this->HighlightProp(pProp);
					ASSERT(!pZoneActor->GetDefault());
				}
			}
		}
		else if (Cube *c = dynamic_cast<Cube*>(pZoneActor->GetPolyhedron()))
		{
			if (pZoneActor->GetDefault() || dynamic_cast<Domain*>(pZoneActor->GetPolyhedron()))
			{
				// Highlight Prop
				//
				this->HighlightProp(pProp);
			}
			else
			{
				this->HighlightProp(this->CurrentProp = NULL);

				ASSERT(this->BoxWidget);
				if (this->BoxWidget)
				{
					const CUnits &units = this->GetDocument()->GetUnits();
					double* scale = this->GetDocument()->GetScale();

					ostringstream oss;
					oss << *pZoneActor;
					TRACE("%s\n", oss.str().c_str());

					// Reset BoxWidget
					//
					this->BoxWidget->SetProp3D(pZoneActor);
					const struct zone *z = pZoneActor->GetPolyhedron()->Get_bounding_box();

					if (c->Get_user_coordinate_system() == PHAST_Transform::MAP)
					{
						if (Cube *c = dynamic_cast<Cube*>(pZoneActor->GetPolyhedron()))
						{
							zone *z = c->Get_bounding_box();
							this->BoxWidget->PlaceWidget(
								z->x1,
								z->x2,
								z->y1,
								z->y2,
								z->z1,
								z->z2
								);
						}

						if (vtkTransform *t = vtkTransform::SafeDownCast(pZoneActor->GetUserTransform()))
						{
							this->BoxWidget->SetTransform(t);
						}
						else
						{
							ASSERT(FALSE);
						}
					}
					else
					{
						this->BoxWidget->PlaceWidget(
							z->x1,
							z->x2,
							z->y1,
							z->y2,
							z->z1,
							z->z2
							);
						if (vtkTransform *t = vtkTransform::SafeDownCast(pZoneActor->GetUserTransform()))
						{
							this->BoxWidget->SetTransform(t);
						}
						else
						{
							ASSERT(FALSE);
						}
					}
					this->GetInteractor()->Render();
					this->BoxWidget->SetInteractor(this->GetInteractor());
					this->BoxWidget->On();
				}
			}
		}
		else if (dynamic_cast<NullPolyhedron*>(pZoneActor->GetPolyhedron()))
		{
			this->HighlightProp(0);
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	else if (CWellActor *pWellActor = CWellActor::SafeDownCast(pProp))
	{
		if (this->PointWidget)
		{
			// clear red selection box
			this->HighlightProp(0);

			// Reset PointWidget
			//
			this->PointWidget->TranslationModeOff();
			this->PointWidget->SetProp3D(pWellActor);
			this->PointWidget->PlaceWidget();
			ASSERT(pWellActor == this->PointWidget->GetProp3D());
			this->PointWidget->TranslationModeOn();

			if (pWellActor->GetWell().xy_coordinate_system_user == PHAST_Transform::MAP)
			{
				CGridKeyword gridKeyword;
				this->GetDocument()->GetGridKeyword(gridKeyword);
				this->Cursor3DActor->SetOrientation(0, 0, -gridKeyword.m_grid_angle);
				this->PointWidget->SetOrientation(0, 0, -gridKeyword.m_grid_angle);
			}
			else
			{
				this->Cursor3DActor->SetOrientation(0, 0, 0);
				this->PointWidget->SetOrientation(0, 0, 0);
			}

			this->PointWidget->SetInteractor(this->GetInteractor());
			this->PointWidget->On();
		}
	}
	else if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(pProp))
	{
		// Highlight river
		//
		this->HighlightProp(pProp);

		pRiverActor->SetInteractor(this->GetInteractor());
		pRiverActor->On();
	}
	else if (CDrainActor *pDrainActor = CDrainActor::SafeDownCast(pProp))
	{
		// Highlight drain
		//
		this->HighlightProp(pProp);

		pDrainActor->SetInteractor(this->GetInteractor());
		pDrainActor->On();
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
	return (this->WellActor != 0);
}

void CWPhastView::StartNewWell(void)
{
	// set size of 3D cursor
	//
	double dim = 0.016 * CGlobal::ComputeRadius(this->GetRenderer());
	this->Cursor3D->SetModelBounds(-dim, dim, -dim, dim, -dim, dim);
	this->Cursor3DActor->VisibilityOn();

	// set yellow cursor
	//
	this->Cursor3DActor->GetProperty()->SetColor(1, 1, 0);

#if 9991 // well w/ grid rotation
	if (this->GetDocument()->GetCoordinateMode() == CWPhastDoc::GridMode)
	{
		this->Cursor3DActor->SetOrientation(0, 0, 0);
	}
	else if (this->GetDocument()->GetCoordinateMode() == CWPhastDoc::MapMode)
	{
		CGridKeyword gridKeyword = this->GetDocument()->GetGridKeyword();
		this->Cursor3DActor->SetOrientation(0, 0, -gridKeyword.m_grid_angle);
	}
	else
	{
		ASSERT(FALSE);
	}
#endif // 9991 well w/ grid rotation

	// create well actor
	//
	ASSERT(this->WellActor == 0);
	this->WellActor = CWellActor::New();
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
	this->Cursor3DActor->VisibilityOff();

	// stop rendering the well actor
	//
	this->Renderer->RemoveViewProp(this->WellActor);

	// clean-up
	//
	ASSERT(this->WellActor != 0);
	if (this->WellActor)
	{
		this->WellActor->Delete();
		this->WellActor = 0;
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
	if (!this->GetInteractor()) return;
	//}}
	//{{
	if (vtkInteractorStyle *pStyle = vtkInteractorStyle::SafeDownCast(this->GetInteractor()->GetInteractorStyle()))
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
			pStyle->SetInteractor(this->GetInteractor());
			ASSERT(pStyle->GetInteractor() == this->GetInteractor());
			pStyle->HighlightProp(pProp);
			pStyle->SetInteractor(0);
			// HACK }}
		}
	}
	//}}
}

void CWPhastView::HighlightProp3D(vtkProp3D *pProp3D)
{
	if (vtkInteractorStyle *pStyle = vtkInteractorStyle::SafeDownCast(this->GetInteractor()->GetInteractorStyle()))
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
			pStyle->SetInteractor(this->GetInteractor());
			ASSERT(pStyle->GetInteractor() == this->GetInteractor());
			pStyle->HighlightProp3D(pProp3D);
			pStyle->SetInteractor(0);
			// HACK }}
		}
	}
}

// COMMENT: {9/8/2009 8:37:50 PM}void CWPhastView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
// COMMENT: {9/8/2009 8:37:50 PM}{
// COMMENT: {9/8/2009 8:37:50 PM}	// Added GetSaveHwnd in order to avoid assertion that occurs when
// COMMENT: {9/8/2009 8:37:50 PM}	// the user is in the middle of creating a new river and selects
// COMMENT: {9/8/2009 8:37:50 PM}	// file->exit
// COMMENT: {9/8/2009 8:37:50 PM}	if (this->GetSafeHwnd())
// COMMENT: {9/8/2009 8:37:50 PM}	{
// COMMENT: {9/8/2009 8:37:50 PM}		CView::OnUpdate(pSender, lHint, pHint);
// COMMENT: {9/8/2009 8:37:50 PM}
// COMMENT: {9/8/2009 8:37:50 PM}		// Add your specialized code here and/or call the base class
// COMMENT: {9/8/2009 8:37:50 PM}		this->Invalidate();
// COMMENT: {9/8/2009 8:37:50 PM}		this->RedrawWindow();
// COMMENT: {9/8/2009 8:37:50 PM}	}
// COMMENT: {9/8/2009 8:37:50 PM}}

void CWPhastView::Update(IObserver* pSender, LPARAM lHint, CObject* pHint, vtkObject* pObject)
{
	ASSERT(pSender != this);
	ASSERT(this);

	switch (lHint)
	{
	case WPN_NONE:
		::OutputDebugString("View->Update WPN_NONE\n");
		break;
	case WPN_SELCHANGING:
		::OutputDebugString("View->Update WPN_SELCHANGING\n");
		break;
	case WPN_SELCHANGED:
		::OutputDebugString("View->Update WPN_SELCHANGED\n");
		if (this->CurrentProp = vtkProp::SafeDownCast(pObject))
		{
			this->Select(this->CurrentProp);
			if (pHint)
			{
				if (pHint->IsKindOf(RUNTIME_CLASS(CPointSelectionObject)))
				{
					CPointSelectionObject *pt = (CPointSelectionObject*)pHint;
					if (CPointConnectorActor *pActor = CPointConnectorActor::SafeDownCast(pObject))
					{
						pActor->SelectPoint(pt->GetPoint());
					}
				}
			}
		}
		else if (pObject == 0)
		{
			this->CurrentProp = NULL;
			this->ClearSelection();
		}
		break;
	case WPN_VISCHANGED:
		::OutputDebugString("View->Update WPN_VISCHANGED\n");
		if (vtkProp* pProp = vtkProp::SafeDownCast(pObject))
		{
			if (!pProp->GetVisibility())
			{
				if (vtkAbstractPropPicker *pPicker = vtkAbstractPropPicker::SafeDownCast( this->GetInteractor()->GetPicker() ))
				{
					if (vtkPropAssembly* pPropAssembly = vtkPropAssembly::SafeDownCast(pObject))
					{
						if (vtkPropCollection *pParts = pPropAssembly->GetParts())
						{
							if (pParts->IsItemPresent(pPicker->GetViewProp()))
							{
								this->ClearSelection();
							}
						}
					}
					else
					{
						if (pPicker->GetViewProp() == pProp)
						{
							this->ClearSelection();
						}
					}
				}
			}
		}
		break;
	case WPN_SCALE_CHANGED: case WPN_DOMAIN_CHANGED:
		::OutputDebugString("View->Update WPN_SCALE_CHANGED\n");
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
			this->RiverActor->SetScale(this->GetDocument()->GetScale());
		}
		if (this->BoxWidget->GetEnabled())
		{
			if (vtkProp3D* pProp3D = this->BoxWidget->GetProp3D())
			{
				if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(pProp3D))
				{
					const CUnits &units = this->GetDocument()->GetUnits();
					double* scale = this->GetDocument()->GetScale();
					const struct zone *z = pZoneActor->GetPolyhedron()->Get_bounding_box();

					if (Cube *c = dynamic_cast<Cube*>(pZoneActor->GetPolyhedron()))
					{
						this->BoxWidget->PlaceWidget(
							z->x1,
							z->x2,
							z->y1,
							z->y2,
							z->z1,
							z->z2
							);

						vtkTransform *user = vtkTransform::New();
						if (c->Get_user_coordinate_system() == PHAST_Transform::MAP)
						{
							double *o = pZoneActor->GetGridOrigin();
							double angle = pZoneActor->GetGridAngle();

							user->Scale(
								units.map_horizontal.input_to_si * scale[0],
								units.map_horizontal.input_to_si * scale[1],
								units.map_vertical.input_to_si   * scale[2]);
							user->RotateZ(-angle);
							user->Translate(-o[0], -o[1], -o[2]);
						}
						else
						{
							user->Scale(
								units.horizontal.input_to_si * scale[0],
								units.horizontal.input_to_si * scale[1],
								units.vertical.input_to_si   * scale[2]);
						}
						this->BoxWidget->SetTransform(user);
						user->Delete();
					}
				}
			}
		}
		if (this->PrismWidget->GetEnabled())
		{
			if (vtkProp3D* pProp3D = this->PrismWidget->GetProp3D())
			{
				this->PrismWidget->SetProp3D(pProp3D);
			}
		}

		if (this->CurrentProp) this->Select(this->CurrentProp);
		//}} {5/8/2006 4:00:51 PM}

// COMMENT: {1/25/2011 7:12:24 PM}// COMMENT: {5/8/2006 4:35:15 PM}		//{{ {4/25/2006 4:41:58 PM}
// COMMENT: {1/25/2011 7:12:24 PM}// COMMENT: {5/8/2006 4:35:15 PM}		this->ResetCamera();
// COMMENT: {1/25/2011 7:12:24 PM}		this->RedrawWindow();
// COMMENT: {1/25/2011 7:12:24 PM}// COMMENT: {5/8/2006 4:35:15 PM}		//}} {4/25/2006 4:41:58 PM}
		break;
	default:
		::OutputDebugString("View->Update default\n");
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

		if (this->RiverActor != 0)
		{
			this->RiverActor->Delete();
		}
		this->RiverActor = CRiverActor::StartNewRiver(this->GetInteractor());

		CRiver river;
		river.coordinate_system = PHAST_Transform::GRID;
		river.z_coordinate_system_user = PHAST_Transform::GRID;
		river.n_user = this->GetDocument()->GetNextRiverNumber();
		this->RiverActor->SetRiver(river, this->GetDocument()->GetUnits(), this->GetDocument()->GetGridKeyword());
		this->RiverActor->SetCoordinateMode(this->GetDocument()->GetCoordinateMode());

		this->RiverActor->AddObserver(CRiverActor::EndNewEvent, RiverCallbackCommand);
		this->RiverActor->AddObserver(CRiverActor::CancelNewEvent, RiverCallbackCommand);

		double* scale = this->GetDocument()->GetScale();
		this->RiverActor->SetScale(scale[0], scale[1], scale[2]);

		this->RiverActor->ScaleFromBounds(this->GetDocument()->GetGridBounds(), this->GetRenderer());

		CGrid x, y, z;
		this->GetDocument()->GetGrid(x, y, z);
		z.Setup();
		this->RiverActor->SetZ(z.coord[z.count_coord - 1]);
		this->Renderer->AddViewProp(this->RiverActor);
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
		style->SetInteractor(this->GetInteractor());
	}

	// stop rendering the river actor
	//
	this->Renderer->RemoveViewProp(this->RiverActor);

	// clean-up
	//
	ASSERT(this->RiverActor != 0);
	if (this->RiverActor)
	{
		this->RiverActor->SetEnabled(0);
		this->RiverActor->SetInteractor(0);
		this->RiverActor->Delete();
		this->RiverActor = 0;
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
		ASSERT(pRiverActor == self->RiverActor);
		switch (eid)
		{
		case CRiverActor::CancelNewEvent:
			self->OnEndNewRiver(true);
			break;
		case CRiverActor::EndNewEvent:
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
		style->SetInteractor(this->GetInteractor());
	}

	//
	// NOTE: this->RiverActor->Delete() can't be called within
	// here because the list of listeners will also be deleted
	// *** this may be fixed in 4.4 ***
	//
	ASSERT(this->RiverActor != 0);
	if (this->RiverActor)
	{
		if (!bCancel)
		{
			if (this->RiverActor->GetPointCount() > 1)
			{
				CRiver river = this->RiverActor->GetRiver();

				ASSERT(river.coordinate_system == PHAST_Transform::GRID); // new river default
				if (this->GetDocument()->GetCoordinateMode() == CWPhastDoc::MapMode)
				{
					const CUnits& units = this->GetDocument()->GetUnits();
					const CGridKeyword& gridKeyword = this->GetDocument()->GetGridKeyword();
					PHAST_Transform map2grid(
						gridKeyword.m_grid_origin[0],
						gridKeyword.m_grid_origin[1],
						gridKeyword.m_grid_origin[2],
						gridKeyword.m_grid_angle,
						units.map_horizontal.input_to_si/units.horizontal.input_to_si,
						units.map_horizontal.input_to_si/units.horizontal.input_to_si,
						units.map_vertical.input_to_si/units.vertical.input_to_si
						);

					river.coordinate_system = PHAST_Transform::MAP;

					std::list<CRiverPoint>::iterator it = river.m_listPoints.begin();
					for (; it != river.m_listPoints.end(); ++it)
					{
						Point p(
							it->x_user,
							it->y_user,
							0.0
							);

						map2grid.Inverse_transform(p);
						it->x_user = p.x();
						it->y_user = p.y();
					}
				}

				CString title;
				title.Format("River %d Properties", river.n_user);

				CPropertySheet sheet(title);
				CRiverPropertyPage2 page;
				page.SetProperties(river);
				page.SetUnits(this->GetDocument()->GetUnits());
				page.SetGridKeyword(this->GetDocument()->GetGridKeyword());
				page.SetFlowOnly(bool(this->GetDocument()->GetFlowOnly()));
				std::set<int> riverNums;
				this->GetDocument()->GetUsedRiverNumbers(riverNums);
				page.SetUsedRiverNumbers(riverNums);
				sheet.AddPage(&page);
				if (sheet.DoModal() == IDOK)
				{
					CRiver river;
					page.GetProperties(river);
					this->GetDocument()->Execute(new CPointConnectorCreateAction<CRiverActor, CRiver>(this->GetDocument(), river));
				}
			}
			else
			{
				::AfxMessageBox("Rivers must contain at least two points");
			}
		}
		this->Renderer->RemoveViewProp(this->RiverActor);
	}

	if (this->RiverCallbackCommand)
	{
		this->RiverCallbackCommand->Delete();
		this->RiverCallbackCommand = 0;
	}
	this->GetDocument()->UpdateAllViews(0);
}

// COMMENT: {9/17/2009 4:19:04 PM}void CWPhastView::OnLButtonDblClk(UINT nFlags, CPoint point)
// COMMENT: {9/17/2009 4:19:04 PM}{
// COMMENT: {9/17/2009 4:19:04 PM}	// Add your message handler code here and/or call default
// COMMENT: {9/17/2009 4:19:04 PM}#if defined(_DEBUG)
// COMMENT: {9/17/2009 4:19:04 PM}	::AfxMessageBox("OnLButtonDblClk");
// COMMENT: {9/17/2009 4:19:04 PM}#endif
// COMMENT: {9/17/2009 4:19:04 PM}
// COMMENT: {9/17/2009 4:19:04 PM}	__super::OnLButtonDblClk(nFlags, point);
// COMMENT: {9/17/2009 4:19:04 PM}}

void CWPhastView::OnDestroy()
{
#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
	//{{ {9/18/2009 6:42:01 PM}
	this->CancelMode();
	//}} {9/18/2009 6:42:01 PM}

// COMMENT: {9/18/2009 8:36:29 PM}	if (this->Renderer)
// COMMENT: {9/18/2009 8:36:29 PM}	{
// COMMENT: {9/18/2009 8:36:29 PM}		TRACE("Deleting this->Renderer = %p\n", this->Renderer);
// COMMENT: {9/18/2009 8:36:29 PM}		TRACE("this->Renderer->GetReferenceCount() = %d\n",  this->Renderer->GetReferenceCount());
// COMMENT: {9/18/2009 8:36:29 PM}		//this->Renderer->FastDelete();
// COMMENT: {9/18/2009 8:36:29 PM}		if (this->Renderer->GetReferenceCount() <= 2)
// COMMENT: {9/18/2009 8:36:29 PM}		{
// COMMENT: {9/18/2009 8:36:29 PM}			this->Renderer->SetReferenceCount(3);
// COMMENT: {9/18/2009 8:36:29 PM}		}
// COMMENT: {9/18/2009 8:36:29 PM}
// COMMENT: {9/18/2009 8:36:29 PM}		this->Renderer->Delete();
// COMMENT: {9/18/2009 8:36:29 PM}	}
#endif
	__super::OnDestroy();

	// Add your message handler code here
}

bool CWPhastView::MovingGridLine()const
{
	return this->bMovingGridLine;
}

void CWPhastView::EndMoveGridLine()
{
	if (CGridActor* pGridActor = CGridActor::SafeDownCast(this->GetDocument()->GetGridActor()))
	{
		pGridActor->SetInteractor(this->GetInteractor());
		pGridActor->SetEnabled(0);
	}
	this->bMovingGridLine = false;
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
		pGridActor->SetInteractorMode(CGridActor::IModeMoveGridLine);
		pGridActor->SetInteractor(this->GetInteractor());
		ASSERT(pGridActor->GetEnabled());
		this->bMovingGridLine = true;
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
#if defined(CANCEL_WHEN_GRID_UNCHECKED)
			this->CancelMoveGridLine();
#endif
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

bool CWPhastView::RotatingGrid()const
{
	return this->bRotatingGrid;
}

void CWPhastView::EndRotateGrid()
{
	if (CGridActor* pGridActor = CGridActor::SafeDownCast(this->GetDocument()->GetGridActor()))
	{
		pGridActor->SetInteractor(this->GetInteractor());
		pGridActor->SetEnabled(0);
	}
	this->bRotatingGrid = false;
}

void CWPhastView::CancelRotateGrid()
{
	if (this->RotatingGrid())
	{
		this->EndRotateGrid();
	}
}

void CWPhastView::StartRotateGrid()
{
	if (CGridActor* pGridActor = CGridActor::SafeDownCast(this->GetDocument()->GetGridActor()))
	{
		pGridActor->SetInteractorMode(CGridActor::IModeRotateGrid);
		pGridActor->SetInteractor(this->GetInteractor());
		ASSERT(pGridActor->GetEnabled());
		this->bRotatingGrid = true;
	}
}

void CWPhastView::OnToolsRotateGrid()
{
	if (this->RotatingGrid())
	{
		this->CancelRotateGrid();
	}
	else
	{
		this->CancelMode();
		this->StartRotateGrid();
	}
}

void CWPhastView::OnUpdateToolsRotateGrid(CCmdUI *pCmdUI)
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
#if defined(CANCEL_WHEN_GRID_UNCHECKED)
			this->CancelRotateGrid();
#endif
		}

		if (this->RotatingGrid())
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

	// Rotate Grid
	//
	this->CancelRotateGrid();

	// TODO: this should be a single function
	// ie this->GetDocument()->CancelMode()
	if (this->GetDocument())
	{
		// Modify Grid
		//
		this->GetDocument()->EndModifyGrid();

		// New Zone
		//
		this->GetDocument()->EndNewZone();

		// New Wedge
		//
		this->GetDocument()->EndNewWedge();

		// New Prism
		//
		this->GetDocument()->EndNewPrism();

		// New Drain
		//
		this->GetDocument()->EndNewDrain();
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
	// Add your specialized code here and/or call the base class
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

			// maybe use GetForegroundWindow
			if (::GetFocus() == pPropertyTreeControlBar->GetTreeCtrl()->GetSafeHwnd())
			{
				if (nCode == CN_COMMAND || nCode == CN_UPDATE_COMMAND_UI)
				{
					if (pPropertyTreeControlBar->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
					{
						return TRUE;
					}
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
			this->PointWidget->PlaceWidget();
		}

		// update river actor
		//
		if (this->RiverActor)
		{
			this->RiverActor->SetRadius(0.008 * size);
		}
	}
}

void CWPhastView::SetBackground(COLORREF cr)
{
	this->BackgroundColor[0] = (double)GetRValue(cr)/255.;
	this->BackgroundColor[1] = (double)GetGValue(cr)/255.;
	this->BackgroundColor[2] = (double)GetBValue(cr)/255.;
	if (this->Renderer)
	{
		this->Renderer->SetBackground(this->BackgroundColor);
	}
}

void CWPhastView::PrismWidgetListener(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
	ASSERT(caller->IsA("CPrismWidget"));
	ASSERT(clientdata);

	if (clientdata)
	{
		CWPhastView* self = reinterpret_cast<CWPhastView*>(clientdata);
		if (eid == vtkCommand::EndInteractionEvent)
		{
			TRACE("CPrismWidget::EndInteractionEvent recd\n");
			if (CPrismWidget *widget = CPrismWidget::SafeDownCast(caller))
			{
				if (CZoneActor *zoneActor = CZoneActor::SafeDownCast(widget->GetProp3D()))
				{
					if (Prism *prism = dynamic_cast<Prism*>(zoneActor->GetPolyhedron()))
					{
						Prism copy(*prism);
						ASSERT(copy.perimeter.Get_source_type() == Data_source::POINTS);
						ASSERT(copy.perimeter.Get_user_source_type() == Data_source::POINTS || copy.perimeter.Get_user_source_type() == Data_source::NONE);

						std::vector<Point>& vect = copy.perimeter.Get_user_points();
						vect.clear();

						// rewrite points
						vtkPoints *points = widget->GetPoints();
						if (points)
						{
							vtkIdType i = points->GetNumberOfPoints() - 1;
							Point pt;
							double *coor = pt.get_coord();
							for (; i >= 0; i-=2)
							{
								points->GetPoint(i, coor);
								vect.push_back(Point(pt.get_coord()[0], pt.get_coord()[1], pt.get_coord()[2]));
							}

							// load new prism
							Prism new_prism;
							copy.perimeter.Set_user_source_type(Data_source::POINTS);
							CGlobal::DumpAndLoadPrism(copy, new_prism);
							ASSERT(new_prism.perimeter.Get_user_source_type() == Data_source::NONE || new_prism.perimeter.Get_user_source_type() == Data_source::POINTS);

							// setup domain in order to tidy
							self->GetDocument()->GetDefaultZone(::domain);
							new_prism.Tidy();

							CZonePrismResetAction *pAction = new CZonePrismResetAction(
								self,
								zoneActor,
								&new_prism
								);

							if (Prism::Polygon_intersects_self(vect))
							{
								::AfxMessageBox(_T("Perimeter cannot cross itself. Resetting original coordinates."));
								pAction->UnExecute();
								delete pAction;
								return;
							}
							self->GetDocument()->Execute(pAction);
						}
					}
				}
			}
		}
		else if (eid == CPrismWidget::InsertPointEvent)
		{
			TRACE("CPrismWidget::InsertPointEvent recd\n");
			if (CPrismWidget *widget = CPrismWidget::SafeDownCast(caller))
			{
				if (CZoneActor *zoneActor = CZoneActor::SafeDownCast(widget->GetProp3D()))
				{
					if (Prism *prism = dynamic_cast<Prism*>(zoneActor->GetPolyhedron()))
					{
						Prism copy(*prism);
						ASSERT(copy.perimeter.Get_source_type() == Data_source::POINTS);
						ASSERT(copy.perimeter.Get_user_source_type() == Data_source::POINTS || copy.perimeter.Get_user_source_type() == Data_source::NONE);

						std::vector<Point>& vect = copy.perimeter.Get_user_points();
						vect.clear();

						// rewrite points
						vtkPoints *points = widget->GetInsertPoints();
						if (points)
						{
							vtkIdType i = 0;
							Point pt;
							double *coor = pt.get_coord();
							for (; i < points->GetNumberOfPoints(); i+=2)
							{
								points->GetPoint(i, coor);
								vect.push_back(Point(pt.get_coord()[0], pt.get_coord()[1], pt.get_coord()[2]));
							}
							points->Delete();

							// load new prism
							Prism new_prism;
							CGlobal::DumpAndLoadPrism(copy, new_prism);
							ASSERT(new_prism.perimeter.Get_user_source_type() == Data_source::POINTS);

							// setup domain in order to tidy
							self->GetDocument()->GetDefaultZone(::domain);
							new_prism.Tidy();

							CZonePrismResetAction *pAction = new CZonePrismResetAction(
								self,
								zoneActor,
								&new_prism
								);
							self->GetDocument()->Execute(pAction);
						}
					}
				}
			}
		}
		else if (eid == CPrismWidget::DeletePointEvent)
		{
			TRACE("CPrismWidget::DeletePointEvent recd\n");
			if (CPrismWidget *widget = CPrismWidget::SafeDownCast(caller))
			{
				if (CZoneActor *zoneActor = CZoneActor::SafeDownCast(widget->GetProp3D()))
				{
					if (Prism *prism = dynamic_cast<Prism*>(zoneActor->GetPolyhedron()))
					{
						Prism copy(*prism);
						ASSERT(copy.perimeter.Get_source_type() == Data_source::POINTS);
						ASSERT(copy.perimeter.Get_user_source_type() == Data_source::POINTS || copy.perimeter.Get_user_source_type() == Data_source::NONE);

						std::vector<Point>& vect = copy.perimeter.Get_user_points();
						vect.clear();

						// rewrite points
						vtkPoints *points = widget->GetDeletePoints();
						if (points)
						{
							if (points->GetNumberOfPoints() < 6)
							{
								::AfxMessageBox(_T("Perimeters must contain at least three points."));
								points->Delete();
								return;
							}

							vtkIdType i = 0;
							Point pt;
							double *coor = pt.get_coord();
							for (; i < points->GetNumberOfPoints(); i+=2)
							{
								points->GetPoint(i, coor);
								vect.push_back(Point(pt.get_coord()[0], pt.get_coord()[1], pt.get_coord()[2]));
							}
							points->Delete();

							// load new prism
							Prism new_prism;
							CGlobal::DumpAndLoadPrism(copy, new_prism);
							ASSERT(new_prism.perimeter.Get_user_source_type() == Data_source::POINTS);

							// setup domain in order to tidy
							self->GetDocument()->GetDefaultZone(::domain);
							new_prism.Tidy();

							if (Prism::Polygon_intersects_self(vect))
							{
								::AfxMessageBox(_T("Deleting this point would cause the perimeter to cross itself. Resetting original coordinates."));
								return;
							}

							CZonePrismResetAction *pAction = new CZonePrismResetAction(
								self,
								zoneActor,
								&new_prism
								);
							self->GetDocument()->Execute(pAction);
						}
					}
				}
			}
		}
	}
}

void CWPhastView::SetCoordinateMode(CWPhastDoc::CoordinateState mode)
{
	if (CWPhastView::CreatingNewWell() || this->PointWidget->GetEnabled())
	{
		if (mode == CWPhastDoc::GridMode)
		{
			this->Cursor3DActor->SetOrientation(0, 0, 0);
		}
		else if (mode == CWPhastDoc::MapMode)
		{
			CGridKeyword gridKeyword = this->GetDocument()->GetGridKeyword();
			this->Cursor3DActor->SetOrientation(0, 0, -gridKeyword.m_grid_angle);
		}
		else
		{
			ASSERT(FALSE);
		}
	}

	if (this->RiverActor && this->RiverActor->GetEnabled())
	{
		this->RiverActor->SetCoordinateMode(mode);
	}
}

#if defined(_DEBUG) && !defined(__CPPUNIT__)
void vtkMFCWindow::AssertValid() const
{
  CWnd::AssertValid();
}

void vtkMFCWindow::Dump(CDumpContext& dc) const
{
  CWnd::Dump(dc);
}
#endif //defined(_DEBUG) && !defined(__CPPUNIT__)

void CWPhastView::OnCancelMode()
{
	this->CancelMode();
}

void CWPhastView::ResetSelection()
{
	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( this->GetInteractor()->GetPicker() ))
	{
		if (vtkProp3D* prop = picker->GetProp3D())
		{
			this->Select(prop);
		}
	}
}

void CWPhastView::OnViewResetviewpoint()
{
	if (vtkRenderer *r = this->GetRenderer())
	{
		r->ResetCamera();
		if (vtkRenderWindowInteractor *rwi = this->GetInteractor())
		{
			rwi->Render();
		}
	}
}
