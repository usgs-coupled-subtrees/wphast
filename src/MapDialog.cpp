// MapDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "MapDialog.h"

#include <fstream>

#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkWin32RenderWindowInteractor.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkCallbackCommand.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>

#include "CoorDialog.h"
#include "Global.h"
#include "MapImageActor.h"
#include "vtkInteractorStyleImage2.h"
#include "vtkPlaneWidget2.h"
#include "Marker.h"

#if !defined(IDC_HEADER_BORDER)
#define IDC_HEADER_BORDER 1133
#endif


static UINT s_PointsIDs[] =
{ 
	IDC_STATIC_P1_GB,
		IDC_STATIC_XP1,
		IDC_EDIT_XP1,
		IDC_SPIN_XP1,
		IDC_STATIC_YP1,
		IDC_EDIT_YP1,
		IDC_SPIN_YP1,
		IDC_STATIC_XC1,
		IDC_EDIT_XC1,
		IDC_STATIC_YC1,
		IDC_EDIT_YC1,
	IDC_STATIC_P2_GB,
		IDC_STATIC_XP2,
		IDC_EDIT_XP2,
		IDC_SPIN_XP2,
		IDC_STATIC_YP2,
		IDC_EDIT_YP2,
		IDC_SPIN_YP2,
		IDC_STATIC_XC2,
		IDC_EDIT_XC2,
		IDC_STATIC_YC2,
		IDC_EDIT_YC2,
};

static UINT s_Point1IDs[] =
{ 
	IDC_STATIC_P1_GB,
	IDC_STATIC_XP1,
	IDC_EDIT_XP1,
	IDC_SPIN_XP1,
	IDC_STATIC_YP1,
	IDC_EDIT_YP1,
	IDC_SPIN_YP1,
	IDC_STATIC_XC1,
	IDC_EDIT_XC1,
	IDC_STATIC_YC1,
	IDC_EDIT_YC1,
};

static UINT s_Point2IDs[] =
{ 
	IDC_STATIC_P2_GB,
	IDC_STATIC_XP2,
	IDC_EDIT_XP2,
	IDC_SPIN_XP2,
	IDC_STATIC_YP2,
	IDC_EDIT_YP2,
	IDC_SPIN_YP2,
	IDC_STATIC_XC2,
	IDC_EDIT_XC2,
	IDC_STATIC_YC2,
	IDC_EDIT_YC2,
};

static UINT s_GridIDs[] =
{
	IDC_STATIC_MO_GB,
	IDC_STATIC_MO_X,
	IDC_EDIT_MO_X,
	IDC_STATIC_MO_Y,
	IDC_EDIT_MO_Y,
	IDC_STATIC_MO_ANGLE,
	IDC_EDIT_MO_ANGLE,
	IDC_STATIC_XY,
	IDC_STATIC_X,
	IDC_EDIT_X,
	IDC_STATIC_Y,
	IDC_EDIT_Y,
	IDC_STATIC_Z,
	IDC_EDIT_Z,
	IDC_STATIC_GD,
	IDC_STATIC_LENGTH,
	IDC_EDIT_LENGTH,
	IDC_STATIC_WIDTH,
	IDC_EDIT_WIDTH,
	IDC_STATIC_DEPTH,
	IDC_EDIT_DEPTH,
	IDC_X_NODES_STATIC,
	IDC_X_NODES_EDIT,
	IDC_X_SPIN,
	IDC_Y_NODES_STATIC,
	IDC_Y_NODES_EDIT,
	IDC_Y_SPIN,
	IDC_Z_NODES_STATIC,
	IDC_Z_NODES_EDIT,
	IDC_Z_SPIN
};

// CMapDialog dialog

#define UPDATE_NODES(ID) \
	OnEnUpdateNodes(ID - FIRST_NODE_ID)

#define KILLFOCUS_NODES(ID) \
	OnEnKillfocusNodesEdit(ID - FIRST_NODE_ID)

#define SELECT_ALL(ID) \
	this->GetDlgItem(ID)->SendMessage(EM_SETSEL, 0, -1)


#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

#define TITLEX                 22
#define TITLEY                 10
#define SUBTITLEX              44
#define SUBTITLEY              25
#define DEFAULTHEADERHEIGHT    58   // in pixels
#define DRAWTEXT_WIZARD97FLAGS (DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_EDITCONTROL)


const UINT FIRST_NODE_ID = IDC_X_NODES_EDIT;

CGridPoint::CGridPoint(void)
: x_defined(false)
, y_defined(false)
, x_val_defined(false)
, y_val_defined(false)
{
}

CMapDialog::CMapDialog(CWnd* pParent /*=NULL*/)
	: CMapDialogBase(CMapDialog::IDD, pParent)
	, m_bFirstPaint(true)
	, m_bHaveWorld(false)
	, m_xMin(0)
	, m_yMin(0)
	, m_zMin(0)
	, m_xLength(0)
	, m_xNodes(2)
	, m_CurrentState(MDS_Point1)
	, m_strHeaderTitle(_T("MDS_Point1_TITLE"))
	, m_strHeaderSubTitle(_T("MDS_Point1_SUBTITLE"))
	, m_Point1Actor(0)
	, m_Point2Actor(0)
{
	// IDC_X_NODES_EDIT - IDC_Z_NODES_EDIT must be contiguous
	//
	ASSERT(IDC_X_NODES_EDIT     == FIRST_NODE_ID);
	ASSERT(IDC_X_NODES_EDIT + 1 == IDC_Y_NODES_EDIT);
	ASSERT(IDC_X_NODES_EDIT + 2 == IDC_Z_NODES_EDIT);

	// initialize grid
	for (int i = 0; i < 3; ++i)
	{
		this->m_grid[i].coord[0]    = 0.0;
		this->m_grid[i].coord[1]    = 0.0;
		this->m_grid[i].count_coord = 2;
		this->m_grid[i].c           = 'x' + i;
		this->m_grid[i].uniform     = TRUE;
	}

	// adjust layout
	this->setRootBorders(0, 0);

	// Create the the renderer, window and interactor objects.
	//
	this->m_RenderWindow           = vtkWin32OpenGLRenderWindow::New();
	this->m_Renderer               = vtkRenderer::New();
	this->m_RenderWindowInteractor = vtkWin32RenderWindowInteractor::New();

	this->m_Style                  = vtkInteractorStyleImage2::New();

	this->m_Widget = vtkPlaneWidget2::New();
	this->m_Widget->NormalToZAxisOn();
	this->m_Widget->SetInteractor(this->m_RenderWindowInteractor);
	this->m_Widget->SetEnabled(0);
	this->m_Widget->GetHandleProperty()->SetOpacity(0.75);
	this->m_Widget->GetSelectedHandleProperty()->SetOpacity(0.75);
	for (int i = 0; i < 3; ++i)
	{
		this->m_Widget->SetResolution(i, this->m_grid[i].count_coord - 1);
	}
	this->m_Widget->SizeHandles();

	this->m_CallbackCommand = vtkCallbackCommand::New();
	this->m_CallbackCommand->SetClientData(this); 
	this->m_CallbackCommand->SetCallback(CMapDialog::ProcessEvents);

	this->m_Widget->AddObserver(vtkCommand::InteractionEvent, this->m_CallbackCommand);

	// mouse events
	//
	this->m_RenderWindowInteractor->AddObserver(vtkCommand::MouseMoveEvent, this->m_CallbackCommand);
	this->m_RenderWindowInteractor->AddObserver(vtkCommand::LeftButtonPressEvent, this->m_CallbackCommand);
	this->m_RenderWindowInteractor->AddObserver(vtkCommand::LeftButtonReleaseEvent, this->m_CallbackCommand);

	//{{ {4/18/2006 10:05:59 PM}
	this->m_RenderWindowInteractor->AddObserver(vtkCommand::ModifiedEvent, this->m_CallbackCommand);
	//}} {4/18/2006 10:05:59 PM}

	if (this->m_Style) this->m_RenderWindowInteractor->SetInteractorStyle(this->m_Style);

	this->m_MapImageActor = CMapImageActor::New();

	this->m_Point1Actor = CMarker::New();
	this->m_Point2Actor = CMarker::New();
}

CMapDialog::~CMapDialog()
{
	// Delete the the renderer, window and interactor objects.
	//
	this->m_RenderWindow->Delete();
	this->m_Renderer->Delete();
	this->m_RenderWindowInteractor->Delete();

	if (this->m_Style) this->m_Style->Delete();
	if (this->m_Widget) this->m_Widget->Delete();
	if (this->m_CallbackCommand) this->m_CallbackCommand->Delete();

	this->m_MapImageActor->Delete();
	this->m_Point1Actor->Delete();
	this->m_Point2Actor->Delete();
}

void CMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CMapDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_X_SPIN, m_udXNodes);
	DDX_Control(pDX, IDC_Y_SPIN, m_udYNodes);
	DDX_Control(pDX, IDC_Z_SPIN, m_udZNodes);

	DDX_Control(pDX, IDC_SPIN_XP1, m_udXP1);
	DDX_Control(pDX, IDC_SPIN_YP1, m_udYP1);
	DDX_Control(pDX, IDC_SPIN_XP2, m_udXP2);
	DDX_Control(pDX, IDC_SPIN_YP2, m_udYP2);

// COMMENT: {4/13/2006 5:24:24 PM}	//{{
// COMMENT: {4/13/2006 5:24:24 PM}	DDX_Control(pDX, IDC_ZOOM_BUTTON, m_btnZoom);	
// COMMENT: {4/13/2006 5:24:24 PM}	//}}

	switch (this->m_CurrentState)
	{
	case MDS_Point1:
		this->DDX_Point1(pDX);
		break;
	case MDS_Point2:
		this->DDX_Point2(pDX);
		break;
	case MDS_Grid:
		this->DDX_Grid(pDX);
		break;
	default:
		ASSERT(FALSE);
	}
}

void CMapDialog::DDX_Point1(CDataExchange* pDX)
{
	if (pDX->m_bSaveAndValidate)
	{
		// these aren't used just make sure they're valid integers
		// see OnEnUpdateEditXp1 & OnEnUpdateEditYp1
		int pixelx;
		int pixely;
		DDX_Text(pDX, IDC_EDIT_XP1, pixelx);
		DDX_Text(pDX, IDC_EDIT_YP1, pixely);

		double x;
		double y;
		DDX_Text(pDX, IDC_EDIT_XC1, x);
		DDX_Text(pDX, IDC_EDIT_YC1, y);

		this->m_point1.x_val = x;
		this->m_point1.y_val = y;
		this->m_point1.x_val_defined = true;
		this->m_point1.y_val_defined = true;
	}
}

void CMapDialog::DDX_Point2(CDataExchange* pDX)
{
	if (pDX->m_bSaveAndValidate)
	{
		// these aren't used just make sure they're valid integers
		// see OnEnUpdateEditXp2 & OnEnUpdateEditYp2
		int pixelx;
		int pixely;
		DDX_Text(pDX, IDC_EDIT_XP2, pixelx);
		DDX_Text(pDX, IDC_EDIT_YP2, pixely);

		double x;
		double y;
		DDX_Text(pDX, IDC_EDIT_XC2, x);
		DDX_Text(pDX, IDC_EDIT_YC2, y);

		this->m_point2.x_val = x;
		this->m_point2.y_val = y;
		this->m_point2.x_val_defined = true;
		this->m_point2.y_val_defined = true;

		{
			vtkFloatingPointType *dataSpacing = this->m_MapImageActor->GetImageReader2()->GetDataSpacing();
			vtkFloatingPointType *dataOrigin  = this->m_MapImageActor->GetImageReader2()->GetDataOrigin();
			int                  *dataExtent  = this->m_MapImageActor->GetDataExtent();
			ASSERT(dataSpacing[0] != 0);
			ASSERT(dataSpacing[1] != 0);
			ASSERT(dataSpacing[2] != 0);

			double cy = (double) dataExtent[3];

			double xpix[2];
			double ypix[2];

			// x_world and y_world are set in ProcessEvents
			// or in OnEnUpdateEdit[12]p[12]
			xpix[0] = (this->m_point1.x_world - dataOrigin[0]) / (dataSpacing[0]);
			xpix[1] = (this->m_point2.x_world - dataOrigin[0]) / (dataSpacing[0]);

			ypix[0] = (this->m_point1.y_world - dataOrigin[1]) / (dataSpacing[1]);
			ypix[1] = (this->m_point2.y_world - dataOrigin[1]) / (dataSpacing[1]);

			double spacing[3];
			ASSERT( (xpix[1] - xpix[0]) != 0);
			ASSERT( (ypix[1] - ypix[0]) != 0);
			spacing[0] = (this->m_point2.x_val - this->m_point1.x_val) / (xpix[1] - xpix[0]);
			spacing[1] = (this->m_point2.y_val - this->m_point1.y_val) / (ypix[1] - ypix[0]);;
			spacing[2] = 1.0;
			this->m_worldTransform.SetDataSpacing(spacing);

			double upperleft[3];
			upperleft[0] = this->m_point1.x_val - spacing[0] * xpix[0];
			upperleft[1] = this->m_point1.y_val + spacing[1] * (cy - ypix[0]);
			upperleft[2] = 0;
			this->m_worldTransform.SetUpperLeft(upperleft);

			// TODO validate site map
			this->m_siteMap.SetWorldTransform(this->m_worldTransform);
		}
	}
}

void CMapDialog::DDX_Grid(CDataExchange* pDX)
{
	// Angle
	::DDX_Text(pDX, IDC_EDIT_MO_ANGLE, this->m_siteMap.m_angle);

	if (pDX->m_bSaveAndValidate)
	{
		// Grid origin in map coordinates
		double map_coor[2];
		::DDX_Text(pDX, IDC_EDIT_MO_X, map_coor[0]);
		::DDX_Text(pDX, IDC_EDIT_MO_Y, map_coor[1]);

		int nodes[3];
		double minimum[3];
		double length[3];
		for (int i = 0; i < 3; ++i)
		{
			// nodes
			DDX_Text(pDX, FIRST_NODE_ID + i, nodes[i]);
			if (nodes[i] < 2)
			{
				::AfxMessageBox("Each direction must have at least two nodes.");
				pDX->Fail();
			}
			// minimums
			DDX_Text(pDX, IDC_EDIT_X + i, minimum[i]);
		}

		// width
		DDX_Text(pDX, IDC_EDIT_WIDTH, length[0]);
		if (length[0] == 0)
		{
			::AfxMessageBox("Width must be non-zero.");
			pDX->Fail();
		}

		// length
		DDX_Text(pDX, IDC_EDIT_LENGTH, length[1]);
		if (length[1] == 0)
		{
			::AfxMessageBox("Length must be non-zero.");
			pDX->Fail();
		}

		// Depth
		DDX_Text(pDX, IDC_EDIT_DEPTH, length[2]);
		if (length[2] == 0)
		{
			::AfxMessageBox("Depth must be non-zero.");
			pDX->Fail();
		}

		for (int i = 0; i < 3; ++i)
		{
			this->m_grid[i].count_coord = nodes[i];
			if (length[i] < 0)
			{
				this->m_grid[i].coord[0] = minimum[i] + length[i];
				this->m_grid[i].coord[1] = minimum[i];
			}
			else
			{
				this->m_grid[i].coord[0] = minimum[i];
				this->m_grid[i].coord[1] = minimum[i] + length[i];
			}
			ASSERT( this->m_grid[i].coord[0] < this->m_grid[i].coord[1] );
		}

		// set placement
		this->m_siteMap.m_placement[0] = map_coor[0] - minimum[0];
		this->m_siteMap.m_placement[1] = map_coor[1] - minimum[1];
		this->m_siteMap.m_placement[2] = 0.0;
	}
	else
	{
		for (int i = 0; i < 3; ++i)
		{
			// nodes
			this->m_Widget->SetResolution(i, this->m_grid[i].count_coord - 1);
			this->UpdateNodes(i);

			// minimums
			DDX_Text(pDX, IDC_EDIT_X + i, this->m_grid[i].coord[0]);
		}

		double defaultZ = 1.0;
		DDX_Text(pDX, IDC_EDIT_DEPTH, defaultZ);

		//{{
		//BUGBUG check to see if setting a world file breaks this
		this->UpdateLength();
		this->UpdateWidth();
		this->UpdateModelOriginX();
		this->UpdateModelOriginY();
		this->UpdateModelOriginAngle();
		//}}
	}
}

CRect CMapDialog::GetRect() 
{ 
	return CMapDialogBase::GetRect();
}

BEGIN_MESSAGE_MAP(CMapDialog, CMapDialogBase)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_HELPINFO()
	ON_EN_KILLFOCUS(IDC_EDIT_MO_ANGLE, OnEnKillfocusEditMoAngle)
	ON_EN_KILLFOCUS(IDC_EDIT_MO_Y, OnEnKillfocusEditMoY)
	ON_EN_KILLFOCUS(IDC_EDIT_MO_X, OnEnKillfocusEditMoX)
	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH, OnEnKillfocusEditLength)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH, OnEnKillfocusEditWidth)
	ON_EN_KILLFOCUS(IDC_X_NODES_EDIT, OnEnKillfocusXNodesEdit)
	ON_EN_KILLFOCUS(IDC_Y_NODES_EDIT, OnEnKillfocusYNodesEdit)
	ON_EN_KILLFOCUS(IDC_Z_NODES_EDIT, OnEnKillfocusZNodesEdit)
	ON_EN_UPDATE(IDC_X_NODES_EDIT, OnEnUpdateXNodesEdit)
	ON_EN_UPDATE(IDC_Y_NODES_EDIT, OnEnUpdateYNodesEdit)
	ON_EN_UPDATE(IDC_EDIT_XP1, OnEnUpdateEditXp1)
	ON_EN_UPDATE(IDC_EDIT_YP1, OnEnUpdateEditYp1)
	ON_EN_UPDATE(IDC_EDIT_XP2, OnEnUpdateEditXp2)
	ON_EN_UPDATE(IDC_EDIT_YP2, OnEnUpdateEditYp2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BACK_BUTTON, OnWizardBack)
	ON_MESSAGE(WM_SHOWCOORDLG, OnShowCoorDlg)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_XP1, IDC_EDIT_YC1, OnEnChangeRange)
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CMapDialog::OnStaticMOGB()
{
	::AfxMessageBox("CLICK");
}

// CMapDialog message handlers

BOOL CMapDialog::OnInitDialog()
{
	CMapDialogBase::OnInitDialog();

	// Add extra initialization here

	UDACCEL uda[7];
	uda[0].nSec = 0;
	uda[0].nInc = 1;
	uda[1].nSec = 2;
	uda[1].nInc = 2;
	uda[2].nSec = 4;
	uda[2].nInc = 10;
	uda[3].nSec = 8;
	uda[3].nInc = 100;
	uda[4].nSec = 16;
	uda[4].nInc = 1000;
	uda[5].nSec = 32;
	uda[5].nInc = 10000;
	uda[6].nSec = 64;
	uda[6].nInc = 100000;

	// init spinners
	this->m_udXNodes.SetRange32(2, INT_MAX - 1);
	this->m_udXNodes.SetAccel(7, uda);
	this->m_udYNodes.SetRange32(2, INT_MAX - 1);
	this->m_udYNodes.SetAccel(7, uda);
	this->m_udZNodes.SetRange32(2, INT_MAX - 1);
	this->m_udZNodes.SetAccel(7, uda);

	this->m_udXP1.SetRange32(INT_MIN, INT_MAX);
	this->m_udXP1.SetAccel(7, uda);
	this->m_udYP1.SetRange32(INT_MIN, INT_MAX);
	this->m_udYP1.SetAccel(7, uda);
	this->m_udXP2.SetRange32(INT_MIN, INT_MAX);
	this->m_udXP2.SetAccel(7, uda);
	this->m_udYP2.SetRange32(INT_MIN, INT_MAX);
	this->m_udYP2.SetAccel(7, uda);

	this->m_RenderWindowInteractor->Initialize();
	this->m_Renderer->ResetCamera();

	CWnd* pWnd = CWnd::FromHandle(this->m_RenderWindow->GetWindowId());
	ASSERT(pWnd && pWnd->GetSafeHwnd());
	CRect rect;
	pWnd->GetWindowRect(&rect);
	this->m_szMinimumRender.cx = rect.Width();
	this->m_szMinimumRender.cy = rect.Height();

	this->SetState(this->m_CurrentState);

// COMMENT: {4/13/2006 5:24:08 PM}	//{{
// COMMENT: {4/13/2006 5:24:08 PM}	HICON h = ::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ZOOM_REAL));
// COMMENT: {4/13/2006 5:24:08 PM}	m_btnZoom.SetIcon(h);
// COMMENT: {4/13/2006 5:24:08 PM}	//}}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


int CMapDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMapDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Add your specialized creation code here

	CRect rect(0, 0, 0, 0);
	if (!this->m_headerBorder.Create("", WS_CHILDWINDOW | WS_VISIBLE |
		SS_ETCHEDHORZ | SS_SUNKEN, rect, this, IDC_HEADER_BORDER))
		return -1;

	if (!this->m_RenderWindow)
		return -1;

	if (!this->m_Renderer)
		return -1;

	if (!this->m_RenderWindowInteractor)
		return -1;

	if (!this->m_MapImageActor)
		return -1;

	// setup the parent window
	this->m_RenderWindow->AddRenderer(this->m_Renderer);
	this->m_RenderWindow->SetParentId(this->m_hWnd);
	this->m_RenderWindowInteractor->SetRenderWindow(this->m_RenderWindow);

	return 0;
}

int CMapDialog::SetFileName(const char *filename)
{
	if (this->m_MapImageActor->SetFileName(filename) == 1) {
		this->m_Renderer->AddActor( this->m_MapImageActor );
		// BUGBUG: consolidate SetFileName and SetWorldFileName into SetSiteMap
		this->m_siteMap.m_fileName = filename;
		return 1; // success
	}
	return 0; // failure
}

int CMapDialog::SetWorldFileName(const char *filename)
{
	int nreturn = 0;
	CWorldTransform wtrans;

	if (CGlobal::LoadWorldFile(filename, wtrans) == 0)
	{
		nreturn = this->m_MapImageActor->SetWorldTransform(wtrans);
		if (nreturn == 1)
		{
			// TODO get rid of this->m_worldTransform
			this->m_worldTransform = wtrans;
			this->m_siteMap.SetWorldTransform(wtrans);
			this->m_Widget->SetInput( this->m_MapImageActor->GetInput() );
			this->m_Widget->PlaceWidget();
			this->m_bHaveWorld = true;
			this->m_CurrentState = MDS_Grid;
		}
	}
	return nreturn;
}

void CMapDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (CWnd *pWnd = this->GetDlgItem(IDC_HEADER_BORDER))
	{
		CRect rect;
		this->GetClientRect(&rect);

		CRect rcHeader;
		pWnd->GetWindowRect(&rcHeader);
		this->ScreenToClient(&rcHeader);
		rect.bottom = rcHeader.top;

		CBrush fill;
		fill.CreateSysColorBrush(COLOR_WINDOW);
		dc.FillRect(&rect, &fill);
        dc.SetBkColor(::GetSysColor(COLOR_WINDOW));
		dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

		LOGFONT lf;
		this->GetFont()->GetLogFont(&lf);

		CString strTitle;
		CString strSubTitle;

		switch (this->m_CurrentState)
		{
		case MDS_Point1:
			strTitle.LoadString(IDS_MDS_POINT1_177);
			strSubTitle.LoadString(IDS_MDS_POINT1_SUB_180);
			break;
		case MDS_Point2:
			strTitle.LoadString(IDS_MDS_POINT2_178);
			strSubTitle.LoadString(IDS_MDS_POINT2_SUB_181);
			break;
		case MDS_Grid:
			strTitle.LoadString(IDS_MDS_GRID_179);
			strSubTitle.LoadString(IDS_MDS_GRID_SUB_182);
			break;
		default:
			ASSERT(FALSE); // unhandled state
		}

		CFont fntBold;
		lf.lfWeight = FW_BOLD;
		if (fntBold.CreateFontIndirect(&lf))
		{
			RECT rectHeader;
			this->GetClientRect(&rectHeader);
			rectHeader.bottom = DEFAULTHEADERHEIGHT;
			CFont *pOrigFont = dc.SelectObject(&fntBold);
			int cx = TITLEX;
			int cy = TITLEY;
			dc.ExtTextOut(cx, cy, 0, &rectHeader, strTitle, NULL);
			if (pOrigFont) dc.SelectObject(pOrigFont);
		}

		CFont fntNormal;
		lf.lfWeight = FW_NORMAL;
		if (fntNormal.CreateFontIndirect(&lf))
		{
			RECT rcWrap;
			this->GetClientRect(&rcWrap);
			rcWrap.left = SUBTITLEX;
			rcWrap.top  = SUBTITLEY;
			CFont *pOrigFont = dc.SelectObject(&fntNormal);
			dc.DrawText(strSubTitle, &rcWrap, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX);
			if (pOrigFont) dc.SelectObject(pOrigFont);			
		}
	}
	
	// Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	if (this->m_bFirstPaint)
	{
		this->m_bFirstPaint = false;
		switch(this->m_CurrentState)
		{
		case MDS_Point1:
			this->m_Widget->Off();
			break;
		case MDS_Point2:
			this->m_Widget->Off();
			break;
		case MDS_Grid:
			this->m_Widget->SizeHandles();
			this->m_Widget->On();
			this->m_Widget->SizeHandles();
			this->ProcessEvents(this->m_Widget, vtkCommand::InteractionEvent, this, NULL);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
}

#include <vtkCamera.h>
#include <vtkImageReader2.h>

void CMapDialog::ProcessEvents(vtkObject* caller,
							   unsigned long event,
							   void* clientdata, 
							   void *calldata)
{
	CMapDialog* self 
		= reinterpret_cast<CMapDialog *>( clientdata );

	if (caller && caller == self->m_Widget)
	{
		// look for char and delete events
		switch(event)
		{
		case vtkCommand::InteractionEvent:
			if (vtkPlaneWidget2 *widget = vtkPlaneWidget2::SafeDownCast(caller))
			{
				int state = self->m_Widget->GetState();
				switch(state)
				{
				case vtkPlaneWidget2::Start:
					TRACE("Start\n");
					break;
				case vtkPlaneWidget2::Moving:
					TRACE("Moving\n");
					switch (self->m_Widget->GetSubstate())
					{
					case vtkPlaneWidget2::MovingMoveOrigin:
						TRACE("MovingMoveOrigin\n");
						self->UpdateModelOriginX();
						self->UpdateModelOriginY();
						self->UpdateLength();
						self->UpdateWidth();
						break;
					case vtkPlaneWidget2::MovingMovePoint1:
						TRACE("MovingMovePoint1\n");
						self->UpdateModelOriginY();
						self->UpdateLength();
						self->UpdateWidth();
						break;
					case vtkPlaneWidget2::MovingMovePoint2:
						TRACE("MovingMovePoint2\n");
						self->UpdateModelOriginX();
						self->UpdateLength();
						self->UpdateWidth();
						break;
					case vtkPlaneWidget2::MovingMovePoint3:
						TRACE("MovingMovePoint3\n");
						self->UpdateLength();
						self->UpdateWidth();
						break;
					case vtkPlaneWidget2::MovingTranslate:
						TRACE("MovingTranslate\n");
						self->UpdateModelOriginX();
						self->UpdateModelOriginY();
						break;
					}
					break;
				case vtkPlaneWidget2::Scaling:
					TRACE("Scaling\n");
					self->UpdateModelOriginX();
					self->UpdateModelOriginY();
					self->UpdateLength();
					self->UpdateWidth();
					break;
				case vtkPlaneWidget2::Pushing:
					TRACE("Pushing\n");
					break;
				case vtkPlaneWidget2::Rotating:
					TRACE("Rotating\n");
					break;
				case vtkPlaneWidget2::Outside:
					TRACE("Outside\n");
					break;
				case vtkPlaneWidget2::Spinning:
					TRACE("Spinning\n");
					self->UpdateModelOriginX();
					self->UpdateModelOriginY();
					self->UpdateModelOriginAngle();
					break;
				default:
					TRACE("UNKNOWN\n");
				}
			}
			break;
		}

	}

	if (caller && caller == self->m_RenderWindowInteractor)
	{
		int* pos;
		static int x;
		static int y;
		switch ( event ) 
		{
		case vtkCommand::LeftButtonReleaseEvent:
			switch (self->m_CurrentState)
			{
			case MDS_Point1:
				if (self->m_mouseDownPoint.x_defined && self->m_mouseDownPoint.y_defined)
				{
					self->m_point1 = self->m_mouseDownPoint;
					self->UpdatePoint1();
					self->PostMessage(WM_SHOWCOORDLG);
				}
				break;
			case MDS_Point2:
				if (self->m_mouseDownPoint.x_defined && self->m_mouseDownPoint.y_defined)
				{
					self->m_point2 = self->m_mouseDownPoint;
					self->UpdatePoint2();
					self->PostMessage(WM_SHOWCOORDLG);
				}
				break;
			case MDS_Grid:
				// do nothing
				break;
			default:
				ASSERT(false);
				break;
			}				
			break;

		case vtkCommand::MouseMoveEvent:
			break;

		case vtkCommand::LeftButtonPressEvent:
			pos = self->m_RenderWindowInteractor->GetEventPosition();
			if (self->m_CurrentState == MDS_Point1 || self->m_CurrentState == MDS_Point2)
			{
				// Modified from 
				// int vtkPicker::Pick(float selectionX, float selectionY, float selectionZ,
				//                     vtkRenderer *renderer)
				int i;
				double worldPointXYPlane[4];

				vtkRenderer *renderer = self->m_Renderer;

				vtkCamera *camera = renderer->GetActiveCamera();	
				vtkFloatingPointType cameraFP[4];
				camera->GetFocalPoint((vtkFloatingPointType*)cameraFP); cameraFP[3] = 1.0;

				renderer->SetWorldPoint(cameraFP);
				renderer->WorldToDisplay();
				vtkFloatingPointType *displayCoords = renderer->GetDisplayPoint();

				// Convert the selection point into world coordinates.
				//
				renderer->SetDisplayPoint(pos[0], pos[1], displayCoords[2]);
				renderer->DisplayToWorld();
				vtkFloatingPointType *worldCoords = renderer->GetWorldPoint();
				if ( worldCoords[3] == 0.0 )
				{
					ASSERT(FALSE);
					return;
				}
				vtkFloatingPointType PickPosition[3];
				for (i = 0; i < 3; ++i)
				{
					PickPosition[i] = worldCoords[i] / worldCoords[3];
				}

				if ( camera->GetParallelProjection() )
				{
					double* cameraDOP = camera->GetDirectionOfProjection();
					double t = -PickPosition[2] / cameraDOP[2];
					for (i = 0; i < 2; ++i)
					{
						worldPointXYPlane[i] = PickPosition[i] + t * cameraDOP[i];
					}
				}
				else
				{
					double *cameraPos = camera->GetPosition();
					double t = -cameraPos[2] / ( PickPosition[2] - cameraPos[2] );
					for (i = 0; i < 2; ++i)
					{
						worldPointXYPlane[i] = cameraPos[i] + t * ( PickPosition[i] - cameraPos[i] );
					}
				}

				vtkFloatingPointType* dataSpacing = self->m_MapImageActor->GetImageReader2()->GetDataSpacing();
				vtkFloatingPointType* dataOrigin = self->m_MapImageActor->GetImageReader2()->GetDataOrigin();

				ASSERT(dataSpacing[0] == 1);
				ASSERT(dataSpacing[1] == 1);
				ASSERT(dataOrigin[0] == 0);
				ASSERT(dataOrigin[1] == 0);

				x = (worldPointXYPlane[0] - dataOrigin[0]) / (dataSpacing[0]) + 0.5;
				y = (worldPointXYPlane[1] - dataOrigin[1]) / (dataSpacing[1]) + 0.5;
				TRACE("LeftButtonPressEvent display(%d, %d)\n", pos[0], pos[1]);
				TRACE("LeftButtonPressEvent world(%g, %g)\n", worldPointXYPlane[0], worldPointXYPlane[1]);
				TRACE("LeftButtonPressEvent pixels(%d, %d)\n", x, y);

				self->m_mouseDownPoint.x = x;
				self->m_mouseDownPoint.y = y;
				self->m_mouseDownPoint.x_defined = true;
				self->m_mouseDownPoint.y_defined = true;
				self->m_mouseDownPoint.x_world = worldPointXYPlane[0];
				self->m_mouseDownPoint.y_world = worldPointXYPlane[1];
			}
			break;

		default:
			TRACE("m_RenderWindowInteractor Unknown Event \n");
			self->m_Widget->SizeHandles();
			break;
		}
	}
}

void CMapDialog::OnEnKillfocusEditMoAngle()
{
	double value;
	if (CGlobal::GetEditValue(this, IDC_EDIT_MO_ANGLE, value))
	{
		this->m_Widget->SetAngle(value);
		this->m_RenderWindowInteractor->Render();
	}
	else
	{
		if (::IsWindowVisible(this->GetDlgItem(IDC_EDIT_MO_ANGLE)->GetSafeHwnd()))
		{
			this->UpdateModelOriginAngle();
			::MessageBeep(-1);
		}
	}
}

void CMapDialog::OnEnKillfocusEditMoY()
{
	double value;
	vtkFloatingPointType* origin = this->m_Widget->GetModelOrigin();
	if (CGlobal::GetEditValue(this, IDC_EDIT_MO_Y, value))
	{
		this->m_Widget->SetModelOrigin(origin[0], value);
		this->m_RenderWindowInteractor->Render();
	}
	else
	{
		if (::IsWindowVisible(this->GetDlgItem(IDC_EDIT_MO_Y)->GetSafeHwnd()))
		{
			this->UpdateModelOriginY();
			::MessageBeep(-1);
		}
	}
}

void CMapDialog::OnEnKillfocusEditMoX()
{
	double value;
	vtkFloatingPointType* origin = this->m_Widget->GetModelOrigin();
	if (CGlobal::GetEditValue(this, IDC_EDIT_MO_X, value))
	{
		this->m_Widget->SetModelOrigin(value, origin[1]);
		this->m_RenderWindowInteractor->Render();
	}
	else
	{
		if (::IsWindowVisible(this->GetDlgItem(IDC_EDIT_MO_X)->GetSafeHwnd()))
		{
			this->UpdateModelOriginX();
			::MessageBeep(-1);
		}
	}
}

void CMapDialog::OnEnKillfocusEditLength()
{
	double dx;
	if (CGlobal::GetEditValue(this, IDC_EDIT_LENGTH, dx))
	{
		this->m_Widget->SetDeltaX(dx);
		this->m_RenderWindowInteractor->Render();
	}
	else
	{
		if (::IsWindowVisible(this->GetDlgItem(IDC_EDIT_LENGTH)->GetSafeHwnd()))
		{
			this->UpdateLength();
			::MessageBeep(-1);
		}
	}
}

void CMapDialog::OnEnKillfocusEditWidth()
{
	double dy;
	if (CGlobal::GetEditValue(this, IDC_EDIT_WIDTH, dy))
	{
		this->m_Widget->SetDeltaY(dy);
		this->m_RenderWindowInteractor->Render();
	}
	else 
	{
		if (::IsWindowVisible(this->GetDlgItem(IDC_EDIT_WIDTH)->GetSafeHwnd()))
		{
			this->UpdateWidth();
			::MessageBeep(-1);
		}
	}
}

void CMapDialog::UpdateModelOriginX(void)const
{
	const TCHAR format[] = "%.2f";
	static TCHAR buffer[40];
	vtkFloatingPointType* origin = this->m_Widget->GetModelOrigin();
	
	::sprintf(buffer, format, origin[0]);
	this->GetDlgItem(IDC_EDIT_MO_X)->SetWindowText(buffer);
}

void CMapDialog::UpdateModelOriginY(void)const
{
	const TCHAR format[] = "%.2f";
	static TCHAR buffer[40];
	vtkFloatingPointType* origin = this->m_Widget->GetModelOrigin();
	
	::sprintf(buffer, format, origin[1]);
	this->GetDlgItem(IDC_EDIT_MO_Y)->SetWindowText(buffer);
}

void CMapDialog::UpdateModelOriginAngle(void)const
{
	const TCHAR format[] = "%.2f";
	static TCHAR buffer[40];
	sprintf(buffer, format, this->m_Widget->GetAngle());
	this->GetDlgItem(IDC_EDIT_MO_ANGLE)->SetWindowText(buffer);
}

void CMapDialog::UpdateLength(void)const
{
	static TCHAR buffer[40];
	double value = this->m_Widget->GetDeltaX();
	if (value == 0.0)
	{
		_stprintf(buffer, _T("%.*g"), 2, value);
	}
	else
	{
		_stprintf(buffer, _T("%.*g"), DBL_DIG, value);
	}
	this->GetDlgItem(IDC_EDIT_LENGTH)->SetWindowText(buffer);
}

void CMapDialog::UpdateWidth(void)const
{
	static TCHAR buffer[40];
	double value = this->m_Widget->GetDeltaY();
	if (value == 0.0)
	{
		_stprintf(buffer, _T("%.*g"), 2, value);
	}
	else
	{
		_stprintf(buffer, _T("%.*g"), DBL_DIG, value);
	}
	this->GetDlgItem(IDC_EDIT_WIDTH)->SetWindowText(buffer);
}

void CMapDialog::UpdatePoint1(void)
{
	const TCHAR format[] = "%d";
	const TCHAR dformat[] = "%g";

	static CString strBuffer;
	static TCHAR buffer[40];

	this->m_udXP1.SetPos32(this->m_point1.x);
	this->m_udYP1.SetPos32(this->m_point1.y);

	if (this->m_point1.x_val_defined)
	{
		TCHAR *buffer = strBuffer.GetBuffer(40);
		_stprintf(buffer, _T("%.*g"), DBL_DIG, this->m_point1.x_val);
		strBuffer.ReleaseBuffer();
		if (strBuffer.Find(_T(".")) == -1) strBuffer += _T(".0");
		this->GetDlgItem(IDC_EDIT_XC1)->SetWindowText(strBuffer);
	}
	if (this->m_point1.y_val_defined)
	{
		TCHAR *buffer = strBuffer.GetBuffer(40);
		_stprintf(buffer, _T("%.*g"), DBL_DIG, this->m_point1.y_val);
		strBuffer.ReleaseBuffer();
		if (strBuffer.Find(_T(".")) == -1) strBuffer += _T(".0");
		this->GetDlgItem(IDC_EDIT_YC1)->SetWindowText(strBuffer);
	}

	double point[3];
	point[0] = this->m_point1.x_world;
	point[1] = this->m_point1.y_world;
	point[2] = 0;

	this->m_Point1Actor->SetPoint(point);
	this->m_Point1Actor->GetProperty()->SetColor(1, 0, 1);
	this->m_Point1Actor->VisibilityOn();

	this->m_Renderer->AddActor(this->m_Point1Actor);
	this->m_RenderWindow->Render();
}

void CMapDialog::UpdatePoint2(void)
{
	const TCHAR format[] = "%d";
	static CString strBuffer;
	static TCHAR buffer[40];

	this->m_udXP2.SetPos32(this->m_point2.x);
	this->m_udYP2.SetPos32(this->m_point2.y);

	if (this->m_point2.x_val_defined)
	{
		TCHAR *buffer = strBuffer.GetBuffer(40);
		_stprintf(buffer, _T("%.*g"), DBL_DIG, this->m_point2.x_val);
		strBuffer.ReleaseBuffer();
		if (strBuffer.Find(_T(".")) == -1) strBuffer += _T(".0");
		this->GetDlgItem(IDC_EDIT_XC2)->SetWindowText(strBuffer);
	}
	if (this->m_point2.y_val_defined)
	{
		TCHAR *buffer = strBuffer.GetBuffer(40);
		_stprintf(buffer, _T("%.*g"), DBL_DIG, this->m_point2.y_val);
		strBuffer.ReleaseBuffer();
		if (strBuffer.Find(_T(".")) == -1) strBuffer += _T(".0");
		this->GetDlgItem(IDC_EDIT_YC2)->SetWindowText(strBuffer);
	}

	double point[3];
	point[0] = this->m_point2.x_world;
	point[1] = this->m_point2.y_world;
	point[2] = 0;

	this->m_Point2Actor->SetPoint(point);
	this->m_Point2Actor->GetProperty()->SetColor(1, 1, 0);
	this->m_Point2Actor->VisibilityOn();

	this->m_Renderer->AddActor(this->m_Point2Actor);
	this->m_RenderWindow->Render();
}

void CMapDialog::UpdateNodes(int idx)const
{
	const TCHAR format[] = "%d";
	static TCHAR buffer[40];

	ASSERT(this->m_grid[idx].count_coord >= 2);

	_stprintf(buffer, format, this->m_grid[idx].count_coord);
	this->GetDlgItem(FIRST_NODE_ID + idx)->SetWindowText(buffer);
}

BOOL CMapDialog::PreTranslateMessage(MSG* pMsg)
{
	if (this->m_CurrentState == CMapDialog::MDS_Point1 || this->m_CurrentState == CMapDialog::MDS_Point2)
	{
		if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_XP1)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				SELECT_ALL(IDC_EDIT_XP1);
				return TRUE;
			}
		}
		if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_YP1)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				SELECT_ALL(IDC_EDIT_YP1);
				return TRUE;
			}
		}
		if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_XP2)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				SELECT_ALL(IDC_EDIT_XP2);
				return TRUE;
			}
		}
		if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_YP2)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				SELECT_ALL(IDC_EDIT_YP2);
				return TRUE;
			}
		}
	}

	if (this->m_CurrentState == CMapDialog::MDS_Grid)
	{
		if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_MO_X)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				this->OnEnKillfocusEditMoX();
				SELECT_ALL(IDC_EDIT_MO_X);
				return TRUE;
			}
		}
		if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_MO_Y)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				this->OnEnKillfocusEditMoY();
				SELECT_ALL(IDC_EDIT_MO_Y);
				return TRUE;
			}
		}
		if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_MO_ANGLE)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				this->OnEnKillfocusEditMoAngle();
				this->UpdateModelOriginAngle();
				SELECT_ALL(IDC_EDIT_MO_ANGLE);
				return TRUE;
			}
		}
		if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_LENGTH)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				this->OnEnKillfocusEditLength();
				this->UpdateLength();
				SELECT_ALL(IDC_EDIT_LENGTH);
				return TRUE;
			}
		}
		if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_WIDTH)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				this->OnEnKillfocusEditWidth();
				this->UpdateWidth();
				SELECT_ALL(IDC_EDIT_WIDTH);
				return TRUE;
			}
		}
		if (pMsg->hwnd == this->GetDlgItem(IDC_X_NODES_EDIT)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				KILLFOCUS_NODES(IDC_X_NODES_EDIT);
				SELECT_ALL(IDC_X_NODES_EDIT);
				return TRUE;
			}
		}
		if (pMsg->hwnd == this->GetDlgItem(IDC_Y_NODES_EDIT)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				KILLFOCUS_NODES(IDC_Y_NODES_EDIT);
				SELECT_ALL(IDC_Y_NODES_EDIT);
				return TRUE;
			}
		}
		if (pMsg->hwnd == this->GetDlgItem(IDC_Z_NODES_EDIT)->GetSafeHwnd()) {
			if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
				KILLFOCUS_NODES(IDC_Z_NODES_EDIT);
				SELECT_ALL(IDC_Z_NODES_EDIT);
				return TRUE;
			}
		}
	}

	return CMapDialogBase::PreTranslateMessage(pMsg);
}

BOOL CMapDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	if (pWnd->GetSafeHwnd() == this->m_RenderWindow->GetWindowId())
	{		
		// set real-time pan cursor
		//
		if (::GetAsyncKeyState(VK_MBUTTON) < 0) {
			::SetCursor(AfxGetApp()->LoadCursor(IDC_PAN_REAL));
			return TRUE;
		}
		if (::GetAsyncKeyState(VK_SHIFT) < 0 && ::GetAsyncKeyState(VK_LBUTTON)) {
			::SetCursor(AfxGetApp()->LoadCursor(IDC_PAN_REAL));
			return TRUE;
		}

		if (::GetAsyncKeyState(VK_RBUTTON) < 0 && !(::GetAsyncKeyState(VK_SHIFT) < 0)) {
#if defined(_DEBUG)
			int state = this->m_Widget->GetState();
			switch(state) {
				case vtkPlaneWidget2::Start:
					TRACE("Start\n");
					break;
				case vtkPlaneWidget2::Moving:
					TRACE("Moving\n");
					break;
				case vtkPlaneWidget2::Scaling:
					TRACE("Scaling\n");
					break;
				case vtkPlaneWidget2::Pushing:
					TRACE("Pushing\n");
					break;
				case vtkPlaneWidget2::Rotating:
					TRACE("Rotating\n");
					break;
				case vtkPlaneWidget2::Outside:
					TRACE("Outside\n");
					break;
				case vtkPlaneWidget2::Spinning:
					TRACE("Spinning\n");
					break;
				default:
					TRACE("UNKNOWN\n");
			}
#endif
			::SetCursor(AfxGetApp()->LoadCursor(IDC_ZOOM_REAL));
			return TRUE;
		}
		if (this->m_CurrentState == CMapDialog::MDS_Point1 || this->m_CurrentState == CMapDialog::MDS_Point2)
		{
			::SetCursor(AfxGetApp()->LoadCursor(IDC_PIXEL_PICKER));
			return TRUE;
		}
	}


	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CMapDialog::OnBnClickedButton1()
{
	// Toggle states
	switch (this->m_CurrentState)
	{
	case MDS_Point1:
		this->m_CurrentState = MDS_Point2;
		break;
	case MDS_Point2:
		this->m_CurrentState = MDS_Grid;
		break;
	case MDS_Grid:
		this->m_CurrentState = MDS_Point1;
		break;
	default:
		ASSERT(FALSE); // unhandled state
	}

	this->m_Widget->SetInput( this->m_MapImageActor->GetInput() );
	this->m_Widget->PlaceWidget();

	switch (this->m_CurrentState)
	{
	case MDS_Point1:
		this->m_Widget->SetEnabled(0);
		this->LayoutPointsPage();
		break;
	case MDS_Point2:
		this->m_Widget->SetEnabled(0);
		this->LayoutPointsPage();
		break;
	case MDS_Grid:
		this->m_Widget->SetEnabled(1);
		this->LayoutGridPage();
		break;
	default:
		ASSERT(FALSE); // unhandled state
	}

	// update controls
	this->UpdateData(FALSE);
}

void CMapDialog::OnEnUpdateNodes(int idx)
{
	BOOL bTranslated;
	UINT nodes = this->GetDlgItemInt(FIRST_NODE_ID + idx, &bTranslated, FALSE);
	if (bTranslated && nodes > 1)
	{
		--nodes;
		this->m_Widget->SetResolution(idx, nodes);
		this->m_RenderWindowInteractor->Render();
	}
}

void CMapDialog::OnEnUpdateXNodesEdit()
{
	UPDATE_NODES(IDC_X_NODES_EDIT);
}

void CMapDialog::OnEnUpdateYNodesEdit()
{
	UPDATE_NODES(IDC_Y_NODES_EDIT);
}

void CMapDialog::OnEnKillfocusNodesEdit(int idx)
{
	BOOL bTranslated;
	UINT nNodes = this->GetDlgItemInt(FIRST_NODE_ID + idx, &bTranslated, FALSE);
	if (bTranslated && nNodes > 1)
	{
		this->m_grid[idx].count_coord = nNodes;
		this->m_Widget->SetResolution(idx, nNodes - 1);
		this->m_RenderWindowInteractor->Render();
	}
	else
	{
		if (::IsWindowVisible(this->GetDlgItem(FIRST_NODE_ID + idx)->GetSafeHwnd()))
		{
			this->UpdateNodes(idx);
			::MessageBeep(-1);
		}
	}
}

void CMapDialog::OnEnKillfocusXNodesEdit()
{
	KILLFOCUS_NODES(IDC_X_NODES_EDIT);
}

void CMapDialog::OnEnKillfocusYNodesEdit()
{
	KILLFOCUS_NODES(IDC_Y_NODES_EDIT);
}

void CMapDialog::OnEnKillfocusZNodesEdit()
{
	KILLFOCUS_NODES(IDC_Z_NODES_EDIT);
}

void CMapDialog::EnablePoint1(BOOL bEnable)
{
	size_t n = sizeof(s_Point1IDs) / sizeof(s_Point1IDs[0]);

	CWnd *pWnd;
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = this->GetDlgItem(s_PointsIDs[i]);
		ASSERT(pWnd && pWnd->GetSafeHwnd());
		if (pWnd)
		{
			pWnd->EnableWindow(bEnable);
		}
	}
}

void CMapDialog::EnablePoint2(BOOL bEnable)
{
	size_t n = sizeof(s_Point2IDs) / sizeof(s_Point2IDs[0]);

	CWnd *pWnd;
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = this->GetDlgItem(s_Point2IDs[i]);
		ASSERT(pWnd && pWnd->GetSafeHwnd());
		if (pWnd)
		{
			pWnd->EnableWindow(bEnable);
		}
	}
}

void CMapDialog::VisiblePointsPage(BOOL bVisible)
{
	size_t n = sizeof(s_PointsIDs) / sizeof(s_PointsIDs[0]);

	int nCmdShow = (bVisible) ? SW_SHOW : SW_HIDE;

	CWnd *pWnd;
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = this->GetDlgItem(s_PointsIDs[i]);
		ASSERT(pWnd && pWnd->GetSafeHwnd());
		if (pWnd)
		{
			pWnd->ShowWindow(nCmdShow);
		}

	}
}

void CMapDialog::VisibleGridPage(BOOL bVisible)
{
	size_t n = sizeof(s_GridIDs) / sizeof(s_GridIDs[0]);

	int nCmdShow = (bVisible) ? SW_SHOW : SW_HIDE;

	CWnd *pWnd;
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = this->GetDlgItem(s_GridIDs[i]);
		ASSERT(pWnd && pWnd->GetSafeHwnd());
		if (pWnd)
		{
			pWnd->ShowWindow(nCmdShow);
		}

	}
}

void CMapDialog::LayoutGridPage(void)
{
	this->VisiblePointsPage(FALSE);
	this->VisibleGridPage(TRUE);

	CWnd* pWnd = CWnd::FromHandle(this->m_RenderWindow->GetWindowId());
	ASSERT(pWnd && pWnd->GetSafeHwnd());

	this->CreateRoot(VERTICAL, 0, 0) 

		<< 	( pane(VERTICAL, ABSOLUTE_VERT, 0, 0, 0 )
			<< itemFixed(VERTICAL, 60)
			<< item ( &this->m_headerBorder, ABSOLUTE_VERT, 0, 0, 0, 0)
			)
		
		<< 	( pane(HORIZONTAL, GREEDY, 7, 7 )

			<< item ( pWnd, GREEDY, 0, 0, this->m_szMinimumRender.cx, this->m_szMinimumRender.cy )

			<< 	( pane(VERTICAL, ABSOLUTE_HORZ, 3, 3 )

				<< 	( paneCtrl(IDC_STATIC_MO_GB, VERTICAL, ABSOLUTE_VERT, nDefaultBorder, 10, 10 )

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
				<< 	( paneCtrl(IDC_STATIC_XY, VERTICAL, ABSOLUTE_VERT, nDefaultBorder, 10, 10 )

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_X, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_X, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_Y, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_Y, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_Z, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_Z, NORESIZE )
						)
				)

				<< 	( paneCtrl(IDC_STATIC_GD, VERTICAL, ABSOLUTE_VERT, nDefaultBorder, 10, 10 )

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_LENGTH, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_LENGTH, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_WIDTH, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_WIDTH, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_DEPTH, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_DEPTH, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_X_NODES_STATIC, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_X_NODES_EDIT, NORESIZE )
						<< item( IDC_X_SPIN, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_Y_NODES_STATIC, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_Y_NODES_EDIT, NORESIZE )
						<< item( IDC_Y_SPIN, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_Z_NODES_STATIC, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_Z_NODES_EDIT, NORESIZE )
						<< item( IDC_Z_SPIN, NORESIZE )
						)
				)
			)
		)
		
		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 7, 7, 0 )
			<< itemGrowing(HORIZONTAL)
			<< item( IDC_BACK_BUTTON, NORESIZE )			
			<< item( IDOK, NORESIZE )
			<< item( IDCANCEL, NORESIZE )
			)
		;

	this->UpdateLayout();
}

void CMapDialog::LayoutPointsPage(void)
{
	this->VisiblePointsPage(TRUE);
	this->VisibleGridPage(FALSE);

	CWnd* pWnd = CWnd::FromHandle(this->m_RenderWindow->GetWindowId());
	ASSERT(pWnd && pWnd->GetSafeHwnd());

	this->CreateRoot(VERTICAL, 0, 0) 

		// Header
		<< 	( pane(VERTICAL, ABSOLUTE_VERT, 0, 0, 0 )
			<< itemFixed(VERTICAL, 60)
			<< item ( &this->m_headerBorder, ABSOLUTE_VERT, 0, 0, 0, 0 )
			)
		
		// 
		<< 	( pane(HORIZONTAL, GREEDY, 7, 7 )
			
			// vtkWindow
			<< item ( pWnd, GREEDY, 0, 0, this->m_szMinimumRender.cx, this->m_szMinimumRender.cy )

			// right-hand side
			<< 	( pane(VERTICAL, ABSOLUTE_HORZ, 3, 3 )

				// Point 1 group-box
				<< 	( paneCtrl(IDC_STATIC_P1_GB, VERTICAL, ABSOLUTE_VERT, nDefaultBorder, 10, 10 )

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_XP1, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_XP1, NORESIZE )
						<< item( IDC_SPIN_XP1, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_YP1, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_YP1, NORESIZE )
						<< item( IDC_SPIN_YP1, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_XC1, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_XC1, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_YC1, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_YC1, NORESIZE )
						)
				)

				// Point 2 group-box
				<< 	( paneCtrl(IDC_STATIC_P2_GB, VERTICAL, ABSOLUTE_VERT, nDefaultBorder, 10, 10 )

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_XP2, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_XP2, NORESIZE )
						<< item( IDC_SPIN_XP2, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_YP2, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_YP2, NORESIZE )
						<< item( IDC_SPIN_YP2, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_XC2, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_XC2, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_YC2, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_YC2, NORESIZE )
						)
				)

				// World Parameters group-box
				<< 	( paneCtrl(IDC_STATIC_WORLD_GB, VERTICAL, ABSOLUTE_VERT, nDefaultBorder, 10, 10 )

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_MODIFY_CHECK )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_WXP, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_WXP, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_WYP, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_WYP, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_WXC, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_WXC, NORESIZE )
						)

					<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
						<< item( IDC_STATIC_WYC, NORESIZE | ALIGN_VCENTER )
						<< item( IDC_EDIT_WYC, NORESIZE )
						)
				)

			)
		)
		
		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 7, 7, 0 )
			<< itemGrowing(HORIZONTAL)
			<< item( IDC_BACK_BUTTON, NORESIZE )			
			<< item( IDOK, NORESIZE )
			<< item( IDCANCEL, NORESIZE )
			)
		;
	this->UpdateLayout();
}

BOOL CMapDialog::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: Add your message handler code here and/or call default

	return CDialog::OnHelpInfo(pHelpInfo);
}

LRESULT CMapDialog::OnShowCoorDlg(WPARAM wParam, LPARAM lParam)
{
	CCoorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		if (this->m_CurrentState == MDS_Point1)
		{
			this->m_point1.x_val         = dlg.m_x;
			this->m_point1.y_val         = dlg.m_y;
			this->m_point1.x_val_defined = true;
			this->m_point1.y_val_defined = true;
			this->UpdatePoint1();
			// UpdatePoint1 causes OnEnUpdateEdit[XY]p1 overwriting
			// the world coordinates
			//
			this->m_point1.x_world = this->m_mouseDownPoint.x_world;
			this->m_point1.y_world = this->m_mouseDownPoint.y_world;

			this->OnOK();
			ASSERT( this->m_CurrentState == MDS_Point2 );
		}
		else if (this->m_CurrentState == MDS_Point2)
		{
			this->m_point2.x_val         = dlg.m_x;
			this->m_point2.y_val         = dlg.m_y;
			this->m_point2.x_val_defined = true;
			this->m_point2.y_val_defined = true;
			this->UpdatePoint2();
			// UpdatePoint2 causes OnEnUpdateEdit[XY]p2 overwriting
			// the world coordinates
			//
			this->m_point2.x_world = this->m_mouseDownPoint.x_world;
			this->m_point2.y_world = this->m_mouseDownPoint.y_world;

			this->OnOK();
			ASSERT( this->m_CurrentState == MDS_Grid );
		}
	}
	return 0;
}

void CMapDialog::OnWizardBack()
{
	// set prev state
	switch (this->m_CurrentState)
	{
	case MDS_Point1:
		ASSERT(FALSE);
		this->SetState(MDS_Point1);
		break;
	case MDS_Point2:
		this->SetState(MDS_Point1);
		break;
	case MDS_Grid:
		this->SetState(MDS_Point2);
		break;
	default:
		ASSERT(FALSE); // unhandled state
	}
}

void CMapDialog::OnOK()
{
	if (!this->UpdateData(TRUE))
	{
		TRACE(traceAppMsg, 0, "UpdateData failed during dialog termination.\n");
		// the UpdateData routine will set focus to correct item
		return;
	}

	// set next state
	switch (this->m_CurrentState)
	{
	case MDS_Point1:
		this->SetState(MDS_Point2);
		break;
	case MDS_Point2:
		this->SetState(MDS_Grid);
		break;
	case MDS_Grid:
		this->EndDialog(IDOK);
		return;
		break;
	default:
		ASSERT(FALSE); // unhandled state
	}
}

void CMapDialog::OnEnUpdateEditXp1()
{
	BOOL bTranslated;
	int pixel =(int)this->GetDlgItemInt(IDC_EDIT_XP1, &bTranslated, TRUE);
	if (bTranslated)
	{
		vtkFloatingPointType* dataSpacing = this->m_MapImageActor->GetImageReader2()->GetDataSpacing();
		vtkFloatingPointType* dataOrigin  = this->m_MapImageActor->GetImageReader2()->GetDataOrigin();

		this->m_point1.x       = pixel;
		this->m_point1.x_world = pixel * dataSpacing[0] + dataOrigin[0];

		this->m_Point1Actor->SetPoint(this->m_point1.x_world, this->m_point1.y_world, 0);
		this->m_Point1Actor->GetProperty()->SetColor(1, 0, 1);
		this->m_Point1Actor->VisibilityOn();
		this->m_RenderWindow->Render();
	}
}

void CMapDialog::OnEnUpdateEditYp1()
{
	BOOL bTranslated;
	int pixel =(int)this->GetDlgItemInt(IDC_EDIT_YP1, &bTranslated, TRUE);
	if (bTranslated)
	{
		vtkFloatingPointType* dataSpacing = this->m_MapImageActor->GetImageReader2()->GetDataSpacing();
		vtkFloatingPointType* dataOrigin = this->m_MapImageActor->GetImageReader2()->GetDataOrigin();

		this->m_point1.y       = pixel;
		this->m_point1.y_world = pixel * dataSpacing[1] + dataOrigin[1];
		this->m_Point1Actor->SetPoint(this->m_point1.x_world, this->m_point1.y_world, 0);
		this->m_Point1Actor->GetProperty()->SetColor(1, 0, 1);
		this->m_Point1Actor->VisibilityOn();
		this->m_RenderWindow->Render();
	}
}

void CMapDialog::OnEnUpdateEditXp2()
{
	BOOL bTranslated;
	int pixel =(int)this->GetDlgItemInt(IDC_EDIT_XP2, &bTranslated, TRUE);
	if (bTranslated)
	{
		vtkFloatingPointType* dataSpacing = this->m_MapImageActor->GetImageReader2()->GetDataSpacing();
		vtkFloatingPointType* dataOrigin = this->m_MapImageActor->GetImageReader2()->GetDataOrigin();

		this->m_point2.x       = pixel;
		this->m_point2.x_world = pixel * dataSpacing[0] + dataOrigin[0];
		this->m_Point2Actor->SetPoint(this->m_point2.x_world, this->m_point2.y_world, 0);
		this->m_Point2Actor->GetProperty()->SetColor(1, 1, 0);
		this->m_Point2Actor->VisibilityOn();
		this->m_RenderWindow->Render();
	}
}

void CMapDialog::OnEnUpdateEditYp2()
{
	BOOL bTranslated;
	int pixel =(int)this->GetDlgItemInt(IDC_EDIT_YP2, &bTranslated, TRUE);
	if (bTranslated)
	{
		vtkFloatingPointType* dataSpacing = this->m_MapImageActor->GetImageReader2()->GetDataSpacing();
		vtkFloatingPointType* dataOrigin = this->m_MapImageActor->GetImageReader2()->GetDataOrigin();

		this->m_point2.y       = pixel;
		this->m_point2.y_world = pixel * dataSpacing[1] + dataOrigin[1];
		this->m_Point2Actor->SetPoint(this->m_point2.x_world, this->m_point2.y_world, 0);
		this->m_Point2Actor->GetProperty()->SetColor(1, 1, 0);
		this->m_Point2Actor->VisibilityOn();
		this->m_RenderWindow->Render();
	}
}

void CMapDialog::OnDeltaposSpinXp1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	this->m_Renderer->AddActor(this->m_Point1Actor);

	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CMapDialog::OnEnChangeEditXp1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	TRACE("OnEnChangeEditXp1\n");

	// TODO:  Add your control notification handler code here
}

void CMapDialog::SetState(State state)
{
	if (this->m_bHaveWorld)
	{
		this->m_CurrentState = MDS_Grid;
	}
	else
	{
		this->m_CurrentState = state;
	}

	switch (state)
	{
	case MDS_Point1:
		this->m_Widget->SetEnabled(0);
		this->GetDlgItem(IDC_BACK_BUTTON)->EnableWindow(FALSE);
		this->GetDlgItem(IDOK)->SetWindowText(_T("&Next >"));
		this->EnablePoint1(TRUE);
		this->EnablePoint2(FALSE);
		this->LayoutPointsPage();
		this->m_MapImageActor->GetImageReader2()->SetDataOrigin(0, 0, 0);
		this->m_MapImageActor->GetImageReader2()->SetDataSpacing(1, 1, 1);
		this->m_Point1Actor->VisibilityOff();
		this->m_Point2Actor->VisibilityOff();
		this->m_Renderer->ResetCamera();
		this->m_Point1Actor->VisibilityOn();
		this->m_Point2Actor->VisibilityOff();
		this->m_Renderer->Render();
		break;
	case MDS_Point2:
		this->m_Point1Actor->VisibilityOn();
		this->m_Point2Actor->VisibilityOn();
		this->m_Widget->SetEnabled(0);
		this->GetDlgItem(IDC_BACK_BUTTON)->EnableWindow(TRUE);
		this->GetDlgItem(IDOK)->SetWindowText(_T("&Next >"));
		this->EnablePoint1(FALSE);
		this->EnablePoint2(TRUE);
		this->LayoutPointsPage();
		this->m_MapImageActor->GetImageReader2()->SetDataOrigin(0, 0, 0);
		this->m_MapImageActor->GetImageReader2()->SetDataSpacing(1, 1, 1);
		this->m_Point1Actor->VisibilityOff();
		this->m_Point2Actor->VisibilityOff();
		///this->m_Renderer->ResetCamera();
		this->m_Point1Actor->VisibilityOn();
		this->m_Point2Actor->VisibilityOn();
		this->m_Renderer->Render();
		break;
	case MDS_Grid:
		this->m_Point1Actor->VisibilityOff();
		this->m_Point2Actor->VisibilityOff();
		this->m_MapImageActor->SetWorldTransform(this->m_worldTransform);
		this->m_Renderer->ResetCamera();
		this->m_Widget->SetInput( this->m_MapImageActor->GetInput() );
		this->m_Widget->PlaceWidget();
		this->m_Widget->SetEnabled(1);
		if (this->m_bHaveWorld)
		{
			this->GetDlgItem(IDC_BACK_BUTTON)->EnableWindow(FALSE);
		}
		else
		{
			this->GetDlgItem(IDC_BACK_BUTTON)->EnableWindow(TRUE);
		}
		this->GetDlgItem(IDOK)->SetWindowText(_T("&Next >"));
		this->UpdateData(FALSE);
		this->LayoutGridPage();
		break;
	default:
		ASSERT(FALSE); // unknown state
	}
}

BOOL CMapDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CMapDialogBase::OnEraseBkgnd(pDC);
}

void CMapDialog::OnEnChangeRange(UINT nID)
{
	// this->m_needsExchange[nID] = true;
}

void CMapDialog::OnSize(UINT nType, int cx, int cy)
{
	CMapDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	this->m_Widget->SizeHandles();
}
