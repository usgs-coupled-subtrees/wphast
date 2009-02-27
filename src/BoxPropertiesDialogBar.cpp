#include "StdAfx.h"
#include "BoxPropertiesDialogBar.h"
#include "resource.h"

#include "WPhastDoc.h"
#include "WPhastView.h"
#include <vtkProp3D.h>
// #include <vtkCubeSource.h>

#include <vtkWin32RenderWindowInteractor.h>
#include <vtkAssemblyPath.h>
// #include <vtkMapper.h>
#include <vtkAbstractPropPicker.h>
#include <vtkBoxWidget.h>
#include <vtkDataSet.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkActor.h>

#include <vtkMapper.h>
#include <vtkCubeSource.h>
#include <vtkDataSet.h>
#include <vtkLine.h>

#include "MacroAction.h"
#include "PointConnectorMovePointAction.h"
#include "ZoneResizeAction.h"
#include "ZonePrismResetAction.h"
#include "WedgeChangeChopTypeAction.h"
#include "WellSetPositionAction.h"
#include "ZoneSetPolyAction.h"
#include "SetAction.h"

#include "ZoneActor.h"
#include "WellActor.h"
#include "RiverActor.h"
#include "DrainActor.h"
#include "SaveCurrentDirectory.h"
#include "GridKeyword.h"
#include "srcinput/Polyhedron.h"
#include "srcinput/Prism.h"
#include "srcinput/Data_source.h"
#include "srcinput/Shapefiles/shapefile.h"

#include "Units.h"
#include "Global.h"

static int PrismIDs[] = {
	IDC_RADIO_NONE,
	IDC_RADIO_CONSTANT,
	IDC_RADIO_ARCRASTER,
	IDC_RADIO_SHAPE,
	IDC_RADIO_POINTS,

	IDC_EDIT_CONSTANT,
	IDC_EDIT_ARCRASTER,
	IDC_EDIT_SHAPEFILE,

	IDC_BUTTON_ARCRASTER,
	IDC_BUTTON_SHAPE,

	IDC_COMBO_SHAPE,
	IDC_GRID_POINTS,
};

BOOL IsEditCtrl(HWND hWnd);
BOOL IsEditCtrl(CWnd* pWnd);

BOOL IsValidFloatFormat(CWnd* pWnd, int nIDC, double& d);
BOOL IsValidFloatFormat(CWnd* pWnd, HWND hWndCtrl, double& d);
BOOL IsValidFloatFormat(CWnd* pWnd, int nIDC, int nRow, int nCol, double& value);

IMPLEMENT_DYNAMIC(CBoxPropertiesDialogBar, CSizingDialogBarCFVS7);


BEGIN_MESSAGE_MAP(CBoxPropertiesDialogBar, CSizingDialogBarCFVS7)

	// Cube Notifications (BP_POS_PLUS_LENGTH) -- NOT USED
	ON_EN_CHANGE(IDC_EDIT_X, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_Y, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_Z, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_LENGTH, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnEnChange)

	// Cube Notifications (BP_MIN_MAX and BP_WEDGE)
	ON_EN_CHANGE(IDC_EDIT_XMIN, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_YMIN, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_ZMIN, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_XMAX, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_YMAX, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_ZMAX, OnEnChange)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_XMIN, IDC_EDIT_ZMAX, OnEnKillfocusRange)

	// Wedge Notifications (BP_WEDGE)
	ON_BN_CLICKED(IDC_RADIO_X, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_Y, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_Z, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_1, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_2, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_3, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_4, OnBnClickedUpdateWedge)

	// Apply button -- NOT USED
	ON_BN_CLICKED(IDC_APPLY, OnApply)

	// Well Notifications (BP_WELL)
	ON_EN_CHANGE(IDC_EDIT_WELL_X, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_WELL_Y, OnEnChange)
	ON_BN_CLICKED(IDC_CHECK_USE_MAP, OnBnClickedUseMap)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_WELL_X, IDC_EDIT_WELL_Y, OnEnKillfocusRange)

	// River Notifications (BP_RIVER)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_RIVER, OnEndLabelEditGrid)
	ON_BN_CLICKED(IDC_CHECK_USE_MAP2, OnBnClickedUseMap)

	// General
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()

	// Prism Notifications (BP_PRISM)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CBoxPropertiesDialogBar::OnTcnSelchangeTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB, &CBoxPropertiesDialogBar::OnTcnSelchangingTab)
	ON_BN_CLICKED(IDC_RADIO_CONSTANT, OnBnClickedData_source)
	ON_BN_CLICKED(IDC_RADIO_NONE, OnBnClickedData_source)	
	ON_BN_CLICKED(IDC_RADIO_ARCRASTER, OnBnClickedData_source)	
	ON_BN_CLICKED(IDC_RADIO_SHAPE, OnBnClickedData_source)	
	ON_BN_CLICKED(IDC_RADIO_POINTS, OnBnClickedData_source)	
	ON_BN_CLICKED(IDC_BUTTON_ARCRASTER, OnBnClickedArcraster)
	ON_BN_CLICKED(IDC_BUTTON_SHAPE, OnBnClickedShape)
	ON_CBN_SELCHANGE(IDC_COMBO_SHAPE, OnCbnSelChangeShape)
	ON_EN_CHANGE(IDC_EDIT_CONSTANT, OnEnChangeConstant)
	ON_EN_KILLFOCUS(IDC_EDIT_CONSTANT, OnEnKillfocusConstant)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_POINTS, OnEndLabelEditPointsGrid)
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID_POINTS, OnBeginLabelEditPointsGrid)	

END_MESSAGE_MAP()

CBoxPropertiesDialogBar::CBoxPropertiesDialogBar()
 : m_pView(0)
 , m_pProp3D(0)
 , m_bNeedsUpdate(false)
 , m_strHorizontalUnits("")
 , m_strVerticalUnits("")
 , m_nType(CBoxPropertiesDialogBar::BP_NONE)
 , m_hBrush(0)
 , m_bThemeActive(FALSE)
 , m_nPrismPart(PRISM_TOP)
{
	TRACE("%s\n", __FUNCTION__);
	this->m_pds[PRISM_TOP]       = new Data_source;
	this->m_pds[PRISM_PERIMETER] = new Data_source;
	this->m_pds[PRISM_BOTTOM]    = new Data_source;

	for (size_t i = 0; i < 3; ++i)
	{
		this->m_nShapeAttribute[i] = -1;
	}
}

CBoxPropertiesDialogBar::~CBoxPropertiesDialogBar()
{
	TRACE("%s\n", __FUNCTION__);
	delete this->m_pds[PRISM_TOP];
	delete this->m_pds[PRISM_PERIMETER];
	delete this->m_pds[PRISM_BOTTOM];
}

BOOL CBoxPropertiesDialogBar::Create(UINT nIDTemplate, LPCTSTR lpszWindowName,
	CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	TRACE("%s\n", __FUNCTION__);
	return this->CSizingDialogBarCFVS7::Create(nIDTemplate, lpszWindowName, pParentWnd, nID, dwStyle);
}

vtkProp3D* CBoxPropertiesDialogBar::GetProp3D(void)
{
	TRACE("%s\n", __FUNCTION__);
	return this->m_pProp3D;
}

void CBoxPropertiesDialogBar::Update(IObserver* pSender, LPARAM lHint, CObject* pHint, vtkObject* pObject)
{
	TRACE("%s in\n", __FUNCTION__);
	ASSERT(pSender != this);

	CFrameWnd *pFrame = reinterpret_cast<CFrameWnd*>(AfxGetApp()->m_pMainWnd);
	if (!pFrame) return;
	ASSERT_KINDOF(CFrameWnd, pFrame);
	ASSERT_VALID(pFrame);

	CWPhastView* pView = reinterpret_cast<CWPhastView*>(pFrame->GetActiveView());
	ASSERT_KINDOF(CWPhastView, pView);
	ASSERT_VALID(pView);

	switch (lHint)
	{
	case WPN_NONE:
		TRACE("\t%s, WPN_NONE\n", __FUNCTION__);
		break;
	case WPN_SELCHANGING:
		TRACE("\t%s, WPN_SELCHANGING\n", __FUNCTION__);
		if (this->m_bNeedsUpdate)
		{
			this->OnApply();
		}
		break;
	case WPN_SELCHANGED:
		TRACE("\t%s, WPN_SELCHANGED\n", __FUNCTION__);
		if (vtkProp* pProp = vtkProp::SafeDownCast(pObject))
		{
			if (CZoneActor* pZoneActor = CZoneActor::SafeDownCast(pProp))
			{
				if (true) // in case zone has morphed // if (pObject != this->m_pProp3D)
				{
					this->HideRiverControls();
					this->HideWellControls();
					this->HideWedgeControls();
					switch(pZoneActor->GetPolyhedronType())
					{
					case Polyhedron::WEDGE:
						this->m_nType = CBoxPropertiesDialogBar::BP_WEDGE;
						this->HidePrismControls();
						this->ShowWedgeControls();
						break;
					case Polyhedron::CUBE:
						this->m_nType = CBoxPropertiesDialogBar::BP_MIN_MAX;
						this->HidePrismControls();
						this->ShowZoneControls();
						break;
					case Polyhedron::PRISM:
						this->m_nType = CBoxPropertiesDialogBar::BP_PRISM;
						this->ShowPrismControls();
						break;
					default:
						ASSERT(FALSE);
					}
				}
				this->Set(pView, pZoneActor, pView->GetDocument()->GetUnits());
			}
			else if (CWellActor* pWellActor = CWellActor::SafeDownCast(pProp))
			{
				this->m_nType = CBoxPropertiesDialogBar::BP_WELL;

				if (pObject != this->m_pProp3D)
				{
					this->HideRiverControls();
					this->HideWedgeControls();
					this->HidePrismControls();
					this->ShowWellControls();
					this->ShowApply();
				}

				this->Set(pView, pWellActor, pView->GetDocument()->GetUnits());
			}
			else if (CRiverActor* pRiverActor = CRiverActor::SafeDownCast(pProp))
			{
				this->m_nType = CBoxPropertiesDialogBar::BP_RIVER;

				if (pObject != this->m_pProp3D)
				{
					this->HideWedgeControls();
					this->HideWellControls();
					this->HidePrismControls();
					this->ShowRiverControls();
					this->ShowApply();
				}

				this->Set(pView, pRiverActor, pView->GetDocument()->GetUnits());
			}
			else if (CDrainActor* pDrainActor = CDrainActor::SafeDownCast(pProp))
			{
				this->m_nType = CBoxPropertiesDialogBar::BP_DRAIN;

				if (pObject != this->m_pProp3D)
				{
					this->HideWedgeControls();
					this->HideWellControls();
					this->HidePrismControls();
					this->ShowRiverControls();
					this->ShowApply();
				}

				this->Set(pView, pDrainActor, pView->GetDocument()->GetUnits());
			}
			else
			{
				ASSERT(FALSE); // untested
				CUnits units;
				this->Set(0, 0, units);
			}
		}
		else if (pObject == 0)
		{
			this->m_nType = CBoxPropertiesDialogBar::BP_NONE;
			this->HideRiverControls();
			this->HideWedgeControls();
			this->HideWellControls();
			this->HideApply();
			this->Enable(FALSE);
			this->SetWindowText(_T(""));
			this->Set(pView, 0, pView->GetDocument()->GetUnits());
		}
		break;
	case WPN_VISCHANGED:
		TRACE("\t%s, WPN_VISCHANGED\n", __FUNCTION__);
		break;
	case WPN_SCALE_CHANGED:
		TRACE("\t%s, WPN_SCALE_CHANGED\n", __FUNCTION__);
		break;
	default:
		ASSERT(FALSE);
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::Set(CWPhastView* pView, vtkProp3D* pProp3D, const CUnits& units)
{
	TRACE("%s, in\n", __FUNCTION__);
	this->m_pView = pView;
	this->m_pProp3D = pProp3D;

	// always update units
	this->UpdateUnits(units);

	if (CZoneActor* pZone = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
	{
		float bounds[6];
		pZone->GetUserBounds(bounds);

		this->m_XMin = bounds[0];
		this->m_XMax = bounds[1];
		this->m_YMin = bounds[2];
		this->m_YMax = bounds[3];
		this->m_ZMin = bounds[4];
		this->m_ZMax = bounds[5];

		this->m_XLength = bounds[1] - bounds[0];
		this->m_YLength = bounds[3] - bounds[2];
		this->m_ZLength = bounds[5] - bounds[4];
		this->m_X = bounds[0];
		this->m_Y = bounds[2];
		this->m_Z = bounds[4];

		CString caption;

		switch(pZone->GetPolyhedronType())
		{
		case Polyhedron::WEDGE:
			this->m_nType = CBoxPropertiesDialogBar::BP_WEDGE;
			this->m_nOrientation = srcWedgeSource::ConvertChopType(pZone->GetChopType());
			this->Enable(TRUE);
			this->UpdateData(FALSE);

			caption.Format(_T("Wedge dimensions (%s)"), pZone->GetName());
			this->SetWindowText(caption);
			break;
		case Polyhedron::CUBE:
			this->m_nType = CBoxPropertiesDialogBar::BP_MIN_MAX;
			if (Cube *cube = dynamic_cast<Cube*>(pZone->GetPolyhedron()))
			{
				this->m_xy_coordinate_system_user = cube->Get_user_coordinate_system();
			}
			if (pZone->GetDefault())
			{
				// default zones cannot be changed
				this->Enable(FALSE);
				this->UpdateData(FALSE);
			}
			else
			{
				this->Enable(TRUE);
				this->UpdateData(FALSE);
			}
			caption.Format(_T("Zone dimensions (%s)"), pZone->GetName());
			this->SetWindowText(caption);
			break;
		case Polyhedron::PRISM:
			this->m_nType = CBoxPropertiesDialogBar::BP_PRISM;
			this->UpdatePrism(pZone, true);
			this->UpdateData(FALSE);
			caption.Format(_T("Prism dimensions (%s)"), pZone->GetName());
			this->SetWindowText(caption);
			break;
		default:
			ASSERT(FALSE);
		}
	}
	else if (CWellActor* pWell = this->m_pProp3D ? CWellActor::SafeDownCast(this->m_pProp3D) : NULL)
	{
		this->m_nType = CBoxPropertiesDialogBar::BP_WELL;

		this->m_XWell = pWell->GetWell().x_user;
		this->m_YWell = pWell->GetWell().y_user;

		this->m_xy_coordinate_system_user = pWell->GetWell().xy_coordinate_system_user;

		this->UpdateData(FALSE);

		CString caption;
		caption.Format(_T("Well location (%s)"), pWell->GetName());
		this->SetWindowText(caption);
	}
	else if (CRiverActor* pRiverActor = this->m_pProp3D ? CRiverActor::SafeDownCast(this->m_pProp3D) : NULL)
	{
		this->m_nType = CBoxPropertiesDialogBar::BP_RIVER;

		this->UpdateData(FALSE);

		CString caption;
		caption.Format(_T("River points (%s)"), pRiverActor->GetName());
		this->SetWindowText(caption);
	}
	else if (CDrainActor* pDrainActor = this->m_pProp3D ? CDrainActor::SafeDownCast(this->m_pProp3D) : NULL)
	{
		this->m_nType = CBoxPropertiesDialogBar::BP_DRAIN;

		this->UpdateData(FALSE);

		CString caption;
		caption.Format(_T("Drain points (%s)"), pDrainActor->GetName());
		this->SetWindowText(caption);
	}
	else
	{
		this->m_nType = CBoxPropertiesDialogBar::BP_NONE;		
		this->UpdateData(FALSE);
		this->Enable(FALSE);
		this->SetWindowText(_T(""));
	}

	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::DoDataExchange(CDataExchange* pDX)
{
	TRACE("%s, in\n", __FUNCTION__);
	
	DDX_Control(pDX, IDC_TAB, m_wndTab);
	DDX_Control(pDX, IDC_COMBO_SHAPE, m_wndShapeCombo);
	DDX_GridControl(pDX, IDC_GRID_RIVER, this->m_wndRiverGrid);	
	DDX_GridControl(pDX, IDC_GRID_POINTS, this->m_wndPointsGrid);

	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_SHAPEFILE))
	{
		RECT rc;
		pWnd->GetWindowRect(&rc);
		//TRACE("IDC_EDIT_SHAPEFILE = %d, %d, %d, %d\n", rc.bottom, rc.left, rc.right, rc.top);
	}

	// Prepare river point grid
	//
	if (!pDX->m_bSaveAndValidate && this->m_wndRiverGrid.GetColumnCount() == 0)
	{
		TRY
		{
			this->m_wndRiverGrid.SetRowCount(1);
			this->m_wndRiverGrid.SetColumnCount(2);
			this->m_wndRiverGrid.SetFixedRowCount(1);
			this->m_wndRiverGrid.SetFixedColumnCount(0);
			this->m_wndRiverGrid.EnableTitleTips(FALSE);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

		this->m_wndRiverGrid.RedrawWindow();
		this->m_wndRiverGrid.ExpandColumnsToFit();
	}

	// Add your specialized code here and/or call the base class
	if (this->m_nType == CBoxPropertiesDialogBar::BP_POS_PLUS_LENGTH)
	{
		DDX_Text(pDX, IDC_EDIT_X, m_X);
		DDX_Text(pDX, IDC_EDIT_Y, m_Y);
		DDX_Text(pDX, IDC_EDIT_Z, m_Z);
		DDX_Text(pDX, IDC_EDIT_LENGTH, m_XLength);
		DDX_Text(pDX, IDC_EDIT_WIDTH, m_YLength);
		DDX_Text(pDX, IDC_EDIT_HEIGHT, m_ZLength);

		DDX_Text(pDX, IDC_EDIT_X_UNITS, m_strHorizontalUnits);
		DDX_Text(pDX, IDC_EDIT_Y_UNITS, m_strHorizontalUnits);
		DDX_Text(pDX, IDC_EDIT_Z_UNITS, m_strVerticalUnits);

		DDX_Text(pDX, IDC_EDIT_LENGTH_UNITS, m_strHorizontalUnits);
		DDX_Text(pDX, IDC_EDIT_WIDTH_UNITS, m_strHorizontalUnits);
		DDX_Text(pDX, IDC_EDIT_HEIGHT_UNITS, m_strVerticalUnits);

	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_MIN_MAX)
	{
		DDX_Text(pDX, IDC_EDIT_XMIN, m_XMin);
		DDX_Text(pDX, IDC_EDIT_YMIN, m_YMin);
		DDX_Text(pDX, IDC_EDIT_ZMIN, m_ZMin);

		DDX_Text(pDX, IDC_EDIT_XMAX, m_XMax);
		DDX_Text(pDX, IDC_EDIT_YMAX, m_YMax);
		DDX_Text(pDX, IDC_EDIT_ZMAX, m_ZMax);

		// Use map coordinates
		int state = BST_UNCHECKED;
		switch (this->m_xy_coordinate_system_user)
		{
		case PHAST_Transform::MAP:
			state = BST_CHECKED;
			DDX_Text(pDX, IDC_X_UNITS_STATIC2, m_strMapHorizontalUnits);
			DDX_Text(pDX, IDC_Y_UNITS_STATIC2, m_strMapHorizontalUnits);
			DDX_Text(pDX, IDC_Z_UNITS_STATIC2, m_strMapVerticalUnits);
			break;
		case PHAST_Transform::GRID:
			DDX_Text(pDX, IDC_X_UNITS_STATIC2, m_strHorizontalUnits);
			DDX_Text(pDX, IDC_Y_UNITS_STATIC2, m_strHorizontalUnits);
			DDX_Text(pDX, IDC_Z_UNITS_STATIC2, m_strVerticalUnits);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		DDX_Check(pDX, IDC_CHECK_USE_MAP, state);
		if (pDX->m_bSaveAndValidate)
		{
			switch (state)
			{
			case BST_CHECKED:
				this->m_xy_coordinate_system_user = PHAST_Transform::MAP;
				break;
			default:
				this->m_xy_coordinate_system_user = PHAST_Transform::GRID;
				break;
			}
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_WELL)
	{
		// wells
		int state = BST_UNCHECKED;
		switch (this->m_xy_coordinate_system_user)
		{
		case PHAST_Transform::MAP:
			state = BST_CHECKED;
			DDX_Text(pDX, IDC_X_UNITS_STATIC, m_strMapHorizontalUnits);
			DDX_Text(pDX, IDC_Y_UNITS_STATIC, m_strMapHorizontalUnits);
			break;
		case PHAST_Transform::GRID:
			DDX_Text(pDX, IDC_X_UNITS_STATIC, m_strHorizontalUnits);
			DDX_Text(pDX, IDC_Y_UNITS_STATIC, m_strHorizontalUnits);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		DDX_Check(pDX, IDC_CHECK_USE_MAP, state);
		if (pDX->m_bSaveAndValidate)
		{
			switch (state)
			{
			case BST_CHECKED:
				this->m_xy_coordinate_system_user = PHAST_Transform::MAP;
				break;
			default:
				this->m_xy_coordinate_system_user = PHAST_Transform::GRID;
				break;
			}
		}

		DDX_Text(pDX, IDC_EDIT_WELL_X, m_XWell);
		DDX_Text(pDX, IDC_EDIT_WELL_Y, m_YWell);
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_RIVER)
	{
		CRiver river = reinterpret_cast<CRiverActor*>(this->m_pProp3D)->GetRiver();
		this->DoDataExchangePoints<CRiver>(pDX, river);

		// Use map coordinates
		int state = BST_UNCHECKED;
		switch (river.coordinate_system)
		{
		case PHAST_Transform::MAP:
			state = BST_CHECKED;
			break;
		case PHAST_Transform::GRID:
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		DDX_Check(pDX, IDC_CHECK_USE_MAP2, state);
		if (pDX->m_bSaveAndValidate)
		{
			switch (state)
			{
			case BST_CHECKED:
				river.coordinate_system =  PHAST_Transform::MAP;
				break;
			default:
				river.coordinate_system =  PHAST_Transform::GRID;
				break;
			}
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_DRAIN)
	{
		CDrain drain = reinterpret_cast<CDrainActor*>(this->m_pProp3D)->GetDrain();
		this->DoDataExchangePoints<CDrain>(pDX, drain);

		// Use map coordinates
		int state = BST_UNCHECKED;
		switch (drain.coordinate_system)
		{
		case PHAST_Transform::MAP:
			state = BST_CHECKED;
			break;
		case PHAST_Transform::GRID:
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		DDX_Check(pDX, IDC_CHECK_USE_MAP2, state);
		if (pDX->m_bSaveAndValidate)
		{
			switch (state)
			{
			case BST_CHECKED:
				drain.coordinate_system =  PHAST_Transform::MAP;
				break;
			default:
				drain.coordinate_system =  PHAST_Transform::GRID;
				break;
			}
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_WEDGE)
	{
		DDX_Text(pDX, IDC_EDIT_XMIN, m_XMin);
		DDX_Text(pDX, IDC_EDIT_YMIN, m_YMin);
		DDX_Text(pDX, IDC_EDIT_ZMIN, m_ZMin);

		DDX_Text(pDX, IDC_EDIT_XMAX, m_XMax);
		DDX_Text(pDX, IDC_EDIT_YMAX, m_YMax);
		DDX_Text(pDX, IDC_EDIT_ZMAX, m_ZMax);

		DDX_Text(pDX, IDC_X_UNITS_STATIC2, m_strHorizontalUnits);
		DDX_Text(pDX, IDC_Y_UNITS_STATIC2, m_strHorizontalUnits);
		DDX_Text(pDX, IDC_Z_UNITS_STATIC2, m_strVerticalUnits);

		switch (this->m_nOrientation)
		{
		case Wedge::X1:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_X);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_1);
			break;
		case Wedge::X2:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_X);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_2);
			break;
		case Wedge::X3:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_X);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_3);
			break;
		case Wedge::X4:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_X);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_4);
			break;
		case Wedge::Y1:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_Y);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_1);
			break;
		case Wedge::Y2:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_Y);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_2);
			break;
		case Wedge::Y3:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_Y);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_3);
			break;
		case Wedge::Y4:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_Y);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_4);
			break;
		case Wedge::Z1:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_Z);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_1);
			break;
		case Wedge::Z2:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_Z);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_2);
			break;
		case Wedge::Z3:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_Z);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_3);
			break;
		case Wedge::Z4:
			this->CheckRadioButton(IDC_RADIO_X, IDC_RADIO_Z, IDC_RADIO_Z);
			this->CheckRadioButton(IDC_RADIO_1, IDC_RADIO_4, IDC_RADIO_4);
			break;
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_PRISM)
	{
		switch (this->m_nPrismPart)
		{
		case PRISM_TOP:
			DoDataExchangePrism(pDX, this->m_pds[PRISM_TOP]);
			break;
		case PRISM_PERIMETER:
			DoDataExchangePrism(pDX, this->m_pds[PRISM_PERIMETER]);
			break;
		case PRISM_BOTTOM:
			DoDataExchangePrism(pDX, this->m_pds[PRISM_BOTTOM]);
			break;
		default:
			ASSERT(FALSE);
		}
	}

	CSizingDialogBarCFVS7::DoDataExchange(pDX);
	if (!pDX->m_bSaveAndValidate) 
	{
		this->m_bNeedsUpdate = false;
#ifdef SHOW_APPLY_BUTTON
		this->GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
#endif
	}
	TRACE("%s, out\n", __FUNCTION__);
}

template<class T>
void CBoxPropertiesDialogBar::DoDataExchangePoints(CDataExchange* pDX, T &type)
{
	TRY
	{
		this->m_wndRiverGrid.SetRowCount(1 + (int)type.m_listPoints.size());
	}
	CATCH (CMemoryException, e)
	{
		e->ReportError();
		e->Delete();
	}
	END_CATCH

	if (pDX->m_bSaveAndValidate)
	{
		this->m_vectorPoints.clear();
		double x, y;
		for (int row = 1; row < this->m_wndRiverGrid.GetRowCount(); ++row)
		{
			DDX_TextGridControl(pDX, IDC_GRID_RIVER, row, 0, x);
			DDX_TextGridControl(pDX, IDC_GRID_RIVER, row, 1, y);
			std::pair<double, double> pt(x, y);
			this->m_vectorPoints.push_back(pt);
		}
	}
	else
	{
		CString title_x(_T("X "));
		title_x += this->m_strHorizontalUnits;
		DDX_TextGridControl(pDX, IDC_GRID_RIVER, 0, 0, title_x);

		CString title_y(_T("Y "));
		title_y += this->m_strHorizontalUnits;
		DDX_TextGridControl(pDX, IDC_GRID_RIVER, 0, 1, title_y);

		std::list<CRiverPoint>::iterator iter = type.m_listPoints.begin();
		for (int row = 1; row < this->m_wndRiverGrid.GetRowCount(); ++row, ++iter)
		{
			DDX_TextGridControl(pDX, IDC_GRID_RIVER, row, 0, iter->x_user);
			DDX_TextGridControl(pDX, IDC_GRID_RIVER, row, 1, iter->y_user);
			this->m_wndRiverGrid.RedrawCell(row, 0);
			this->m_wndRiverGrid.RedrawCell(row, 1);
		}
		this->m_wndRiverGrid.SetModified(FALSE);
		for (int r = 1; r < this->m_wndRiverGrid.GetRowCount(); ++r)
		{
			ASSERT(!this->m_wndRiverGrid.GetModified(r, 0));
			ASSERT(!this->m_wndRiverGrid.GetModified(r, 1));
		}
	}
}

void CBoxPropertiesDialogBar::OnApply()
{
	TRACE("%s, in\n", __FUNCTION__);

	if (vtkProp3D* pProp3D = this->m_pProp3D)
	{
		if (this->UpdateData(TRUE))
		{
            ASSERT(this->m_pView);
			CAction* pAction = NULL;

			switch (this->m_nType)
			{
			case CBoxPropertiesDialogBar::BP_POS_PLUS_LENGTH:
				if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
				{
					pAction = new CZoneResizeAction(
						this->m_pView,
						pZoneActor,
						(this->m_XLength >= 0) ? this->m_X                   : this->m_X + this->m_XLength,
						(this->m_XLength >= 0) ? this->m_X + this->m_XLength : this->m_X,
						(this->m_YLength >= 0) ? this->m_Y                   : this->m_Y + this->m_YLength,
						(this->m_YLength >= 0) ? this->m_Y + this->m_YLength : this->m_Y,
						(this->m_ZLength >= 0) ? this->m_Z                   : this->m_Z + this->m_ZLength,
						(this->m_ZLength >= 0) ? this->m_Z + this->m_ZLength : this->m_Z
						);
				}
				break;
			case CBoxPropertiesDialogBar::BP_MIN_MAX:
				if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
				{
					float bounds[6];					
					pZoneActor->GetUserBounds(bounds);
					if (bounds[0] != this->m_XMin || bounds[1] != this->m_XMax || bounds[2] != this->m_YMin || 
						bounds[3] != this->m_YMax || bounds[4] != this->m_ZMin || bounds[5] != this->m_ZMax)
					{
						pAction = new CZoneResizeAction(
							this->m_pView,
							pZoneActor,
							(this->m_XMin <= this->m_XMax) ? this->m_XMin : this->m_XMax,
							(this->m_XMin <= this->m_XMax) ? this->m_XMax : this->m_XMin,
							(this->m_YMin <= this->m_YMax) ? this->m_YMin : this->m_YMax,
							(this->m_YMin <= this->m_YMax) ? this->m_YMax : this->m_YMin,
							(this->m_ZMin <= this->m_ZMax) ? this->m_ZMin : this->m_ZMax,
							(this->m_ZMin <= this->m_ZMax) ? this->m_ZMax : this->m_ZMin
							);
					}
					else
					{
						// temporary hack
						if (this->m_bNeedsUpdate)
						{
							this->m_bNeedsUpdate = false;
#ifdef SHOW_APPLY_BUTTON
							this->GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
#endif
						}
					}
				}
				break;
			case CBoxPropertiesDialogBar::BP_WELL:
				if (CWellActor* pWellActor = this->m_pProp3D ? CWellActor::SafeDownCast(this->m_pProp3D) : NULL)
				{
					pAction = new CWellSetPositionAction(
						pWellActor,
						this->m_pView->GetDocument(),
						this->m_XWell,
						this->m_YWell,
						this->m_xy_coordinate_system_user
						);
				}
				break;
			case CBoxPropertiesDialogBar::BP_RIVER:
				if (CRiverActor* pRiverActor = this->m_pProp3D ? CRiverActor::SafeDownCast(this->m_pProp3D) : NULL)
				{
					CRiver river = pRiverActor->GetRiver();
					CMacroAction* pMacroAction = new CMacroAction();
					for (int r = 1; r < this->m_wndRiverGrid.GetRowCount(); ++r)
					{
						CRiverPoint* riverPt = pRiverActor->GetRiverPoint(r-1);
						std::pair<double, double> pt = this->m_vectorPoints[r-1];
						if (riverPt->x_user != pt.first || riverPt->y_user != pt.second)
						{
							CPointConnectorMovePointAction<CRiverActor>* pRMPA = new CPointConnectorMovePointAction<CRiverActor>(
								pRiverActor,
								this->m_pView->GetDocument(),
								r-1,
								riverPt->x_user,
								riverPt->y_user
							);
							pRMPA->SetPoint(pt.first, pt.second);
							pMacroAction->Add(pRMPA);
						}
					}
					pAction = pMacroAction;
				}
				break;
			case CBoxPropertiesDialogBar::BP_DRAIN:
				if (CDrainActor* pDrainActor = this->m_pProp3D ? CDrainActor::SafeDownCast(this->m_pProp3D) : NULL)
				{
					CDrain drain = pDrainActor->GetDrain();
					CMacroAction* pMacroAction = new CMacroAction();
					for (int r = 1; r < this->m_wndRiverGrid.GetRowCount(); ++r)
					{
						CRiverPoint* riverPt = pDrainActor->GetRiverPoint(r-1);
						std::pair<double, double> pt = this->m_vectorPoints[r-1];
						if (riverPt->x_user != pt.first || riverPt->y_user != pt.second)
						{
							CPointConnectorMovePointAction<CDrainActor>* pRMPA = new CPointConnectorMovePointAction<CDrainActor>(
								pDrainActor,
								this->m_pView->GetDocument(),
								r-1,
								riverPt->x_user,
								riverPt->y_user
							);
							pRMPA->SetPoint(pt.first, pt.second);
							pMacroAction->Add(pRMPA);
						}
					}
					pAction = pMacroAction;
				}
				break;
			case CBoxPropertiesDialogBar::BP_WEDGE:
				if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
				{
					float bounds[6];					
					pZoneActor->GetUserBounds(bounds);
					if (bounds[0] != this->m_XMin || bounds[1] != this->m_XMax || bounds[2] != this->m_YMin || 
						bounds[3] != this->m_YMax || bounds[4] != this->m_ZMin || bounds[5] != this->m_ZMax)
					{
						pAction = new CZoneResizeAction(
							this->m_pView,
							pZoneActor,
							(this->m_XMin <= this->m_XMax) ? this->m_XMin : this->m_XMax,
							(this->m_XMin <= this->m_XMax) ? this->m_XMax : this->m_XMin,
							(this->m_YMin <= this->m_YMax) ? this->m_YMin : this->m_YMax,
							(this->m_YMin <= this->m_YMax) ? this->m_YMax : this->m_YMin,
							(this->m_ZMin <= this->m_ZMax) ? this->m_ZMin : this->m_ZMax,
							(this->m_ZMin <= this->m_ZMax) ? this->m_ZMax : this->m_ZMin
							);

					}
					else
					{
						// temporary hack
						if (this->m_bNeedsUpdate)
						{
							this->m_bNeedsUpdate = false;
						}
					}
				}
				break;
			case CBoxPropertiesDialogBar::BP_PRISM:
				if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
				{
					this->ApplyNewPrism(pZoneActor);
				}
				break;
			default:
				ASSERT(FALSE);
				break;
			}
			if (pAction && this->m_pView)
			{
				this->m_pView->GetDocument()->Execute(pAction);
			}
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}


void CBoxPropertiesDialogBar::Enable(bool bEnable)
{
	if (bEnable)
	{
		TRACE("%s, true in\n", __FUNCTION__);
	}
	else
	{
		TRACE("%s, false in\n", __FUNCTION__);
	}


#ifdef SHOW_APPLY_BUTTON
	this->GetDlgItem(IDC_APPLY)->EnableWindow(bEnable);
#endif

	if (this->m_nType == CBoxPropertiesDialogBar::BP_POS_PLUS_LENGTH)
	{
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_X))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_Y))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_Z))
		{
			pEdit->SetReadOnly(!bEnable);
		}

		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_LENGTH))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_WIDTH))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_HEIGHT))
		{
			pEdit->SetReadOnly(!bEnable);
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_MIN_MAX)
	{
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_XMIN))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_YMIN))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_ZMIN))
		{
			pEdit->SetReadOnly(!bEnable);
		}

		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_XMAX))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_YMAX))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_ZMAX))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_CHECK_USE_MAP))
		{
			pWnd->EnableWindow(bEnable);
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_WELL)
	{
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_WELL_X))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_WELL_Y))
		{
			pEdit->SetReadOnly(!bEnable);
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_RIVER || this->m_nType == CBoxPropertiesDialogBar::BP_DRAIN)
	{
		for (int row = 1; row < this->m_wndRiverGrid.GetRowCount(); ++row)
		{
			if (bEnable)
			{
				this->m_wndRiverGrid.EnableCell(row, 0);
				this->m_wndRiverGrid.EnableCell(row, 1);
			}
			else
			{
				this->m_wndRiverGrid.DisableCell(row, 0);
				this->m_wndRiverGrid.DisableCell(row, 1);
			}
			this->m_wndRiverGrid.RedrawCell(row, 0);
			this->m_wndRiverGrid.RedrawCell(row, 1);
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_WEDGE)
	{
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_XMIN))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_YMIN))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_ZMIN))
		{
			pEdit->SetReadOnly(!bEnable);
		}

		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_XMAX))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_YMAX))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_ZMAX))
		{
			pEdit->SetReadOnly(!bEnable);
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_PRISM)
	{
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_NONE))
		{
			pWnd->EnableWindow(bEnable);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CONSTANT))
		{
			pWnd->EnableWindow(bEnable);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_ARCRASTER))
		{
			pWnd->EnableWindow(bEnable);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_POINTS))
		{
			pWnd->EnableWindow(bEnable);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_SHAPE))
		{
			pWnd->EnableWindow(bEnable);
		}

		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_CONSTANT))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_ARCRASTER))
		{
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_SHAPEFILE))
		{
			pEdit->SetReadOnly(!bEnable);
		}

		if (CWnd *pWnd = this->GetDlgItem(IDC_BUTTON_SHAPE))
		{
			pWnd->EnableWindow(bEnable);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_BUTTON_ARCRASTER))
		{
			pWnd->EnableWindow(bEnable);
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_NONE)
	{
		this->HideZoneControls();
		this->HideRiverControls();
		this->HideWedgeControls();
		this->HideWellControls();
		this->HidePrismControls();
	}
	TRACE("%s, out\n", __FUNCTION__);
}

BOOL CBoxPropertiesDialogBar::PreTranslateMessage(MSG* pMsg)
{
	// Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		TRACE("%s, in\n", __FUNCTION__);
		if (pMsg->wParam == VK_RETURN)
		{
			if (this->m_nType == CBoxPropertiesDialogBar::BP_PRISM)
			{
				if (this->GetCheckedRadioButton(IDC_RADIO_NONE, IDC_RADIO_SHAPE) != IDC_RADIO_POINTS)
				{
					if (::IsEditCtrl(pMsg->hwnd))
					{
						CWnd *pWnd = CWnd::FromHandle(pMsg->hwnd);
						int nID = pWnd->GetDlgCtrlID();
#ifdef SAVE_FOR_LATER
						if (nID == IDC_EDIT_SHAPEFILE)
						{
							CString str;
							this->GetDlgItemTextA(IDC_EDIT_SHAPEFILE, str);
							if (CGlobal::IsValidShapefile(str))
							{
								this->OnApply();
							}
							else
							{
								this->UpdateData(FALSE);
							}
							::SendMessage(pMsg->hwnd, EM_SETSEL, 0, -1);
						}
						else if (nID == IDC_EDIT_ARCRASTER)
						{
							CString str;
							this->GetDlgItemTextA(IDC_EDIT_ARCRASTER, str);
							if (CGlobal::IsValidArcraster(str))
							{
								this->OnApply();
							}
							else
							{
								this->UpdateData(FALSE);
							}
							::SendMessage(pMsg->hwnd, EM_SETSEL, 0, -1);
						}
						else
#endif // 
						{
							ASSERT(nID == IDC_EDIT_CONSTANT);
							double d;
							if (IsValidFloatFormat(this, pMsg->hwnd, d))
							{
								this->OnApply();
							}
							else
							{
								this->UpdateData(FALSE);
							}
							::SendMessage(pMsg->hwnd, EM_SETSEL, 0, -1);
						}

					}
				}
			}
			if (this->m_nType != CBoxPropertiesDialogBar::BP_RIVER && this->m_nType != CBoxPropertiesDialogBar::BP_DRAIN && this->m_nType != CBoxPropertiesDialogBar::BP_PRISM)
			{
				if (::IsEditCtrl(pMsg->hwnd))
				{
					double d;
					if (IsValidFloatFormat(this, pMsg->hwnd, d))
					{
						this->OnApply();
					}
					else
					{
						this->UpdateData(FALSE);
					}
					::SendMessage(pMsg->hwnd, EM_SETSEL, 0, -1);
				}
			}
		}
		else if (pMsg->wParam == VK_HOME)
		{
			// Home
			if (::IsEditCtrl(pMsg->hwnd))
			{
				// use default handling (skip accelerator handling)
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
			}
		}
		else if (pMsg->wParam == 'X')
		{
			if (::GetAsyncKeyState(VK_CONTROL) < 0)
			{
				// Ctrl+X
				if (::IsEditCtrl(pMsg->hwnd))
				{
					// use default handling (skip accelerator handling)
					::TranslateMessage(pMsg);
					::DispatchMessage(pMsg);
					return TRUE;
				}
			}
		}
		else if (pMsg->wParam == 'V')
		{
			if (::GetAsyncKeyState(VK_CONTROL) < 0)
			{
				// Ctrl+V
				if (::IsEditCtrl(pMsg->hwnd))
				{
					// use default handling (skip accelerator handling)
					::TranslateMessage(pMsg);
					::DispatchMessage(pMsg);
					return TRUE;
				}
			}
		}
		else if (pMsg->wParam == 'C')
		{
			if (::GetAsyncKeyState(VK_CONTROL) < 0)
			{
				// Ctrl+C
				if (::IsEditCtrl(pMsg->hwnd))
				{
					// use default handling (skip accelerator handling)
					::TranslateMessage(pMsg);
					::DispatchMessage(pMsg);
					return TRUE;
				}
			}
		}
		else if (pMsg->wParam == 'Z')
		{
			if (::GetAsyncKeyState(VK_CONTROL) < 0)
			{
				// Ctrl+Z
				if (::IsEditCtrl(pMsg->hwnd))
				{
					// use default handling (skip accelerator handling)
					::TranslateMessage(pMsg);
					::DispatchMessage(pMsg);
					return TRUE;
				}
			}
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			// Esc
			if (::IsEditCtrl(pMsg->hwnd))
			{
				// use default handling (skip accelerator handling)
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
			}
		}
		else if (pMsg->wParam == VK_DELETE)
		{
			// Delete
			if (::IsEditCtrl(pMsg->hwnd))
			{
				// use default handling (skip accelerator handling)
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
			}
		}
		TRACE("%s, out\n", __FUNCTION__);
	}
	return CSizingDialogBarCFVS7::PreTranslateMessage(pMsg);
}

void CBoxPropertiesDialogBar::OnEnKillfocusEdit()
{
	TRACE("%s\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnEnChange()
{
	TRACE("%s, in\n", __FUNCTION__);
	this->UpdateApply();
	TRACE("%s, out\n", __FUNCTION__);
}

BOOL IsEditCtrl(CWnd* pWnd)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (pWnd == NULL)
	{
		TRACE("%s, out\n", __FUNCTION__);
		return FALSE;
	}
	return IsEditCtrl(pWnd->GetSafeHwnd());
}

BOOL IsEditCtrl(HWND hWnd)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (hWnd == NULL)
	{
		return FALSE;
	}

	static TCHAR szCompare[6];
	::GetClassName(hWnd, szCompare, 6);
	TRACE("%s, out\n", __FUNCTION__);
	return lstrcmpi(szCompare, _T("edit")) == 0;
}

void CBoxPropertiesDialogBar::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	TRACE("%s, in\n", __FUNCTION__);
	CWnd* pFocus = CWnd::GetFocus();
	ASSERT(this->IsChild(pFocus));
	if (this->IsChild(pFocus))
	{
		if (::IsEditCtrl(pFocus))
		{
			CEdit* pEdit = static_cast<CEdit*>(pFocus);
			int nStart, nEnd;
			pEdit->GetSel(nStart, nEnd);
			if (nStart != nEnd)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnEditCut()
{
	TRACE("%s, in\n", __FUNCTION__);
	CWnd* pFocus = CWnd::GetFocus();
	ASSERT(this->IsChild(pFocus));
	if (this->IsChild(pFocus))
	{
		if (::IsEditCtrl(pFocus))
		{
			CEdit* pEdit = static_cast<CEdit*>(pFocus);
			pEdit->Cut();
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	TRACE("%s, in\n", __FUNCTION__);
	CWnd* pFocus = CWnd::GetFocus();
	ASSERT(this->IsChild(pFocus));
	if (this->IsChild(pFocus))
	{
		if (::IsEditCtrl(pFocus))
		{
			CEdit* pEdit = static_cast<CEdit*>(pFocus);
			int nStart, nEnd;
			pEdit->GetSel(nStart, nEnd);
			if (nStart != nEnd)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnEditCopy()
{
	TRACE("%s, in\n", __FUNCTION__);
	CWnd* pFocus = CWnd::GetFocus();
	ASSERT(this->IsChild(pFocus));
	if (this->IsChild(pFocus))
	{
		if (::IsEditCtrl(pFocus))
		{
			CEdit* pEdit = static_cast<CEdit*>(pFocus);
			pEdit->Copy();
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	TRACE("%s, in\n", __FUNCTION__);
    BOOL bCanPaste = ::IsClipboardFormatAvailable(CF_TEXT);
    pCmdUI->Enable(bCanPaste);
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnEditPaste()
{
	TRACE("%s, in\n", __FUNCTION__);
	CWnd* pFocus = CWnd::GetFocus();
	ASSERT(this->IsChild(pFocus));
	if (this->IsChild(pFocus))
	{
		if (::IsEditCtrl(pFocus))
		{
			CEdit* pEdit = static_cast<CEdit*>(pFocus);
			pEdit->Paste();
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	TRACE("%s, in\n", __FUNCTION__);
	CWnd* pFocus = CWnd::GetFocus();
	ASSERT(this->IsChild(pFocus));
	if (this->IsChild(pFocus))
	{
		if (::IsEditCtrl(pFocus))
		{
			CEdit* pEdit = static_cast<CEdit*>(pFocus);
			int nStart, nEnd;
			pEdit->GetSel(nStart, nEnd);
			if (nStart != nEnd)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnEditClear()
{
	TRACE("%s, in\n", __FUNCTION__);
	CWnd* pFocus = CWnd::GetFocus();
	ASSERT(this->IsChild(pFocus));
	if (this->IsChild(pFocus))
	{
		if (::IsEditCtrl(pFocus))
		{
			CEdit* pEdit = static_cast<CEdit*>(pFocus);
			pEdit->Clear();
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	TRACE("%s, in\n", __FUNCTION__);
	CWnd* pFocus = CWnd::GetFocus();
	ASSERT(this->IsChild(pFocus));
	if (this->IsChild(pFocus))
	{
		if (::IsEditCtrl(pFocus))
		{
			CEdit* pEdit = static_cast<CEdit*>(pFocus);
			if (pEdit->CanUndo())
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnEditUndo()
{
	TRACE("%s, in\n", __FUNCTION__);
	CWnd* pFocus = CWnd::GetFocus();
	ASSERT(this->IsChild(pFocus));
	if (this->IsChild(pFocus))
	{
		if (::IsEditCtrl(pFocus))
		{
			CEdit* pEdit = static_cast<CEdit*>(pFocus);
			pEdit->Undo();
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	TRACE("%s, in\n", __FUNCTION__);
	// no redo for edit boxes
	pCmdUI->Enable(FALSE);
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnEditRedo()
{
	// no redo for edit boxes
	TRACE("%s\n", __FUNCTION__);
}

static int ZoneIDs[] = {
	IDC_STATIC_MIN,
	IDC_STATIC_MAX,
	IDC_STATIC_X,
	IDC_STATIC_Y,
	IDC_STATIC_Z,
	IDC_EDIT_XMIN,
	IDC_EDIT_YMIN,
	IDC_EDIT_ZMIN,
	IDC_EDIT_XMAX,
	IDC_EDIT_YMAX,
	IDC_EDIT_ZMAX,
	IDC_X_UNITS_STATIC2,
	IDC_Y_UNITS_STATIC2,
	IDC_Z_UNITS_STATIC2,
	IDC_CHECK_USE_MAP,
};

void CBoxPropertiesDialogBar::ShowZoneControls()
{
	TRACE("%s, in\n", __FUNCTION__);
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_MIN))
	{
		pWnd->MoveWindow(54, 24, 18, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_MAX))
	{
		pWnd->MoveWindow(134, 24, 21, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_X))
	{
		pWnd->MoveWindow(11, 50, 12, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_Y))
	{
		pWnd->MoveWindow(11, 83, 12, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_Z))
	{
		pWnd->MoveWindow(11, 117, 12, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_XMIN))
	{
		pWnd->MoveWindow(29, 47, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_YMIN))
	{
		pWnd->MoveWindow(29, 80, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_ZMIN))
	{
		pWnd->MoveWindow(29, 114, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_XMAX))
	{
		pWnd->MoveWindow(110, 47, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_YMAX))
	{
		pWnd->MoveWindow(110, 80, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_ZMAX))
	{
		pWnd->MoveWindow(110, 114, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_X_UNITS_STATIC2))
	{
		pWnd->MoveWindow(186, 50, 30, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_Y_UNITS_STATIC2))
	{
		pWnd->MoveWindow(186, 83, 30, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_Z_UNITS_STATIC2))
	{
		pWnd->MoveWindow(186, 117, 30, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_CHECK_USE_MAP))
	{
		pWnd->ShowWindow(SW_SHOW);
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::HideZoneControls()
{
	TRACE("%s, in\n", __FUNCTION__);
	RECT rc;
	for (int i = 0; i < sizeof(ZoneIDs)/sizeof(ZoneIDs[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(ZoneIDs[i]))
		{
			pWnd->ShowWindow(SW_HIDE);
			pWnd->GetWindowRect(&rc);
			this->ScreenToClient(&rc);
			this->InvalidateRect(&rc, TRUE);
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

static int WellIDs[] = {
//	IDC_STATIC_LOC,
	IDC_STATIC_WELL_X,
	IDC_STATIC_WELL_Y,
	IDC_EDIT_WELL_X,
	IDC_EDIT_WELL_Y,
	IDC_X_UNITS_STATIC,
	IDC_Y_UNITS_STATIC,
	IDC_CHECK_USE_MAP,
};

void CBoxPropertiesDialogBar::ShowWellControls()
{
	TRACE("%s, in\n", __FUNCTION__);
	for (int i = 0; i < sizeof(WellIDs)/sizeof(WellIDs[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(WellIDs[i]))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::HideWellControls()
{
	TRACE("%s, in\n", __FUNCTION__);
	RECT rc;
	for (int i = 0; i < sizeof(WellIDs)/sizeof(WellIDs[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(WellIDs[i]))
		{
			pWnd->ShowWindow(SW_HIDE);
			pWnd->GetWindowRect(&rc);
			this->ScreenToClient(&rc);
			this->InvalidateRect(&rc, TRUE);
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

static int RiverIDs[] = {
	IDC_GRID_RIVER,
	IDC_CHECK_USE_MAP2,
};

void CBoxPropertiesDialogBar::ShowRiverControls()
{
	TRACE("%s, in\n", __FUNCTION__);
	for (int i = 0; i < sizeof(RiverIDs)/sizeof(RiverIDs[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(RiverIDs[i]))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::HideRiverControls()
{
	TRACE("%s, in\n", __FUNCTION__);
	RECT rc;
	for (int i = 0; i < sizeof(RiverIDs)/sizeof(RiverIDs[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(RiverIDs[i]))
		{
			pWnd->ShowWindow(SW_HIDE);
			pWnd->GetWindowRect(&rc);
			this->ScreenToClient(&rc);
			this->InvalidateRect(&rc, TRUE);
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

static int WedgeIDs[] = {
	IDC_STATIC_MIN,
	IDC_STATIC_MAX,
	IDC_STATIC_X,
	IDC_STATIC_Y,
	IDC_STATIC_Z,
	IDC_EDIT_XMIN,
	IDC_EDIT_YMIN,
	IDC_EDIT_ZMIN,
	IDC_EDIT_XMAX,
	IDC_EDIT_YMAX,
	IDC_EDIT_ZMAX,
	IDC_X_UNITS_STATIC2,
	IDC_Y_UNITS_STATIC2,
	IDC_Z_UNITS_STATIC2,
	IDC_RADIO_X,
	IDC_RADIO_Y,
	IDC_RADIO_Z,
	IDC_RADIO_1,
	IDC_RADIO_2,
	IDC_RADIO_3,
	IDC_RADIO_4,
	IDC_GB_AXIS,
	IDC_GB_CORNER,
};

void CBoxPropertiesDialogBar::ShowWedgeControls()
{
	TRACE("%s, in\n", __FUNCTION__);
	const int D1 = 14;
	const int D2 = 22;
	const int D3 = 30;
	const int D4 = 39;
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_MIN))
	{
		pWnd->MoveWindow(54, 24-D1, 18, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_MAX))
	{
		pWnd->MoveWindow(134, 24-D1, 21, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_X))
	{
		pWnd->MoveWindow(11, 50-D2, 12, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_Y))
	{
		pWnd->MoveWindow(11, 83-D3, 12, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_Z))
	{
		pWnd->MoveWindow(11, 117-D4, 12, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_XMIN))
	{
		pWnd->MoveWindow(29, 47-D2, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_YMIN))
	{
		pWnd->MoveWindow(29, 80-D3, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_ZMIN))
	{
		pWnd->MoveWindow(29, 114-D4, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_XMAX))
	{
		pWnd->MoveWindow(110, 47-D2, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_YMAX))
	{
		pWnd->MoveWindow(110, 80-D3, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_ZMAX))
	{
		pWnd->MoveWindow(110, 114-D4, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_X_UNITS_STATIC2))
	{
		pWnd->MoveWindow(186, 50-D2, 30, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_Y_UNITS_STATIC2))
	{
		pWnd->MoveWindow(186, 83-D3, 30, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_Z_UNITS_STATIC2))
	{
		pWnd->MoveWindow(186, 117-D4, 30, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}



	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_X))
	{
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_Y))
	{
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_Z))
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_1))
	{
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_2))
	{
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_3))
	{
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_4))
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	if (CWnd *pWnd = this->GetDlgItem(IDC_GB_AXIS))
	{
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_GB_CORNER))
	{
		pWnd->ShowWindow(SW_SHOW);
	}


	/*
	for (int i = 0; i < sizeof(WedgeIDs)/sizeof(WedgeIDs[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(WedgeIDs[i]))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
	}
	*/
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::HideWedgeControls()
{
	TRACE("%s, in\n", __FUNCTION__);
	RECT rc;
	for (int i = 0; i < sizeof(WedgeIDs)/sizeof(WedgeIDs[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(WedgeIDs[i]))
		{
			pWnd->ShowWindow(SW_HIDE);
			pWnd->GetWindowRect(&rc);
			this->ScreenToClient(&rc);
			this->InvalidateRect(&rc, TRUE);
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::ShowApply()
{
#ifdef SHOW_APPLY_BUTTON
	TRACE("%s, in\n", __FUNCTION__);
	if (CWnd *pWnd = this->GetDlgItem(IDC_APPLY))
	{
			pWnd->ShowWindow(SW_SHOW);
	}
	TRACE("%s, out\n", __FUNCTION__);
#endif
}

void CBoxPropertiesDialogBar::HideApply()
{
#ifdef SHOW_APPLY_BUTTON
	TRACE("%s, in\n", __FUNCTION__);
	if (CWnd *pWnd = this->GetDlgItem(IDC_APPLY))
	{
		RECT rc;
		pWnd->ShowWindow(SW_HIDE);
		pWnd->GetWindowRect(&rc);
		this->ScreenToClient(&rc);
		this->InvalidateRect(&rc, TRUE);
	}
	TRACE("%s, out\n", __FUNCTION__);
#endif
}

void CBoxPropertiesDialogBar::OnEndLabelEditGrid(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("%s, in\n", __FUNCTION__);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	double d;
	if (IsValidFloatFormat(this, IDC_GRID_RIVER, pnmgv->iRow, pnmgv->iColumn, d))
	{
		this->OnApply();
	}
	else
	{
		this->UpdateData(FALSE);
	}

	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnEndLabelEditPointsGrid(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("%s, in\n", __FUNCTION__);
	this->TestPointsGrid(pNotifyStruct, result, TRUE);
	TRACE("%s, out\n", __FUNCTION__);
}

BOOL IsValidFloatFormat(CWnd* pWnd, int nIDC, double& d)
{
	TRACE("%s, in\n", __FUNCTION__);
	HWND hWndCtrl;
	pWnd->GetDlgItem(nIDC, &hWndCtrl);
	BOOL b = IsValidFloatFormat(pWnd, hWndCtrl, d);
	TRACE("%s, out\n", __FUNCTION__);
	return b;
}

BOOL IsValidFloatFormat(CWnd* pWnd, HWND hWndCtrl, double& d)
{
	TRACE("%s, in\n", __FUNCTION__);
	const int TEXT_BUFFER_SIZE = 400;
	TCHAR szBuffer[TEXT_BUFFER_SIZE];

	::GetWindowText(hWndCtrl, szBuffer, _countof(szBuffer));
	if (_sntscanf_s(szBuffer, _countof(szBuffer), _T("%lf"), &d) != 1)
	{
		TRACE("%s, out\n", __FUNCTION__);
		return FALSE;
	}
	TRACE("%s, out\n", __FUNCTION__);
	return TRUE;
}

BOOL IsValidFloatFormat(CWnd* pWnd, int nIDC, int nRow, int nCol, double& d)
{
	TRACE("%s, in\n", __FUNCTION__);
	CGridCtrl* pGrid = static_cast<CGridCtrl*>(pWnd->GetDlgItem(nIDC));
	ASSERT_KINDOF(CModGridCtrl, pGrid);
	CString str = pGrid->GetItemText(nRow, nCol);
	if (_stscanf(str, _T("%lf"), &d) != 1)
	{
		TRACE("%s, out\n", __FUNCTION__);
		return FALSE;
	}
		TRACE("%s, out\n", __FUNCTION__);
	return TRUE;
}

void CBoxPropertiesDialogBar::OnEnKillfocusRange(UINT nID)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (this->m_bNeedsUpdate)
	{
		double d;
		if (IsValidFloatFormat(this, nID, d))
		{
			this->OnApply();
		}
		else
		{
			this->UpdateData(FALSE);
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::UpdateApply()
{
	TRACE("%s, in\n", __FUNCTION__);
	if (vtkProp3D* pProp3D = this->m_pProp3D)
	{
		switch (this->m_nType)
		{
		case CBoxPropertiesDialogBar::BP_POS_PLUS_LENGTH:
			ASSERT(FALSE);
			break;
		case CBoxPropertiesDialogBar::BP_MIN_MAX:
			if (CZoneActor* pZone = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
			{
				float bounds[6];
				pZone->GetUserBounds(bounds);

				static int aIDs[] = {
					IDC_EDIT_XMIN,
					IDC_EDIT_XMAX,
					IDC_EDIT_YMIN,
					IDC_EDIT_YMAX,
					IDC_EDIT_ZMIN,
					IDC_EDIT_ZMAX
				};
				bool bNeedsUpdate = false;
				for (int i = 0; i < 6; ++i)
				{
					double d;
					if (!(IsValidFloatFormat(this, aIDs[i], d) && bounds[i] == (float)d))
					{
						bNeedsUpdate = true;
						break;
					}
				}
				if (bNeedsUpdate)
				{
					this->m_bNeedsUpdate = true;
#ifdef SHOW_APPLY_BUTTON
					this->GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
#endif
				}
				else
				{
					this->m_bNeedsUpdate = false;
#ifdef SHOW_APPLY_BUTTON
					this->GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
#endif
				}
			}
			break;
		case CBoxPropertiesDialogBar::BP_WELL:
			if (CWellActor* pWell = this->m_pProp3D ? CWellActor::SafeDownCast(this->m_pProp3D) : NULL)
			{
				double d;
				bool bNeedsUpdate = false;
				do
				{
					if (!(IsValidFloatFormat(this, IDC_EDIT_WELL_X, d) && pWell->GetWell().x_user == (float)d))
					{
						bNeedsUpdate = true;
						break;
					}
					if (!(IsValidFloatFormat(this, IDC_EDIT_WELL_Y, d) && pWell->GetWell().y_user == (float)d))
					{
						bNeedsUpdate = true;
						break;
					}
				} while (0);
				if (bNeedsUpdate)
				{
					this->m_bNeedsUpdate = true;
#ifdef SHOW_APPLY_BUTTON
					this->GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
#endif
				}
				else
				{
					this->m_bNeedsUpdate = false;
#ifdef SHOW_APPLY_BUTTON
					this->GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
#endif
				}
			}
			break;
		case CBoxPropertiesDialogBar::BP_RIVER:
			if (CRiverActor* pRiverActor = this->m_pProp3D ? CRiverActor::SafeDownCast(this->m_pProp3D) : NULL)
			{
				double d;
				bool bNeedsUpdate = false;
				CRiver river = pRiverActor->GetRiver();
				std::list<CRiverPoint>::iterator iter = river.m_listPoints.begin();
				for (int r = 1; r < this->m_wndRiverGrid.GetRowCount() && iter != river.m_listPoints.end(); ++r, ++iter)
				{
					if (!(IsValidFloatFormat(this, IDC_GRID_RIVER, r, 0, d) && iter->x_user == d))
					{
						bNeedsUpdate = true;
						break;
					}
					if (!(IsValidFloatFormat(this, IDC_GRID_RIVER, r, 1, d) && iter->y_user == d))
					{
						bNeedsUpdate = true;
						break;
					}
				}
				if (bNeedsUpdate)
				{
					this->m_bNeedsUpdate = true;
#ifdef SHOW_APPLY_BUTTON
					this->GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
#endif
				}
				else
				{
					this->m_bNeedsUpdate = false;
#ifdef SHOW_APPLY_BUTTON
					this->GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
#endif
				}
			}
			break;
		case CBoxPropertiesDialogBar::BP_DRAIN:
			if (CDrainActor* pDrainActor = this->m_pProp3D ? CDrainActor::SafeDownCast(this->m_pProp3D) : NULL)
			{
				double d;
				bool bNeedsUpdate = false;
				CDrain drain = pDrainActor->GetDrain();
				std::list<CRiverPoint>::iterator iter = drain.m_listPoints.begin();
				for (int r = 1; r < this->m_wndRiverGrid.GetRowCount() && iter != drain.m_listPoints.end(); ++r, ++iter)
				{
					if (!(IsValidFloatFormat(this, IDC_GRID_RIVER, r, 0, d) && iter->x_user == d))
					{
						bNeedsUpdate = true;
						break;
					}
					if (!(IsValidFloatFormat(this, IDC_GRID_RIVER, r, 1, d) && iter->y_user == d))
					{
						bNeedsUpdate = true;
						break;
					}
				}
				if (bNeedsUpdate)
				{
					this->m_bNeedsUpdate = true;
#ifdef SHOW_APPLY_BUTTON
					this->GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
#endif
				}
				else
				{
					this->m_bNeedsUpdate = false;
#ifdef SHOW_APPLY_BUTTON
					this->GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
#endif
				}
			}
			break;
		case CBoxPropertiesDialogBar::BP_WEDGE:
			if (CZoneActor* pZone = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
			{
				float bounds[6];
				pZone->GetUserBounds(bounds);

				static int aIDs[] = {
					IDC_EDIT_XMIN,
					IDC_EDIT_XMAX,
					IDC_EDIT_YMIN,
					IDC_EDIT_YMAX,
					IDC_EDIT_ZMIN,
					IDC_EDIT_ZMAX
				};
				bool bNeedsUpdate = false;
				for (int i = 0; i < 6; ++i)
				{
					double d;
					if (!(IsValidFloatFormat(this, aIDs[i], d) && bounds[i] == (float)d))
					{
						bNeedsUpdate = true;
						break;
					}
				}
				if (bNeedsUpdate)
				{
					this->m_bNeedsUpdate = true;
				}
				else
				{
					this->m_bNeedsUpdate = false;
				}
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
			
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::UpdateUnits(const CUnits& units)
{
	if (units.horizontal.defined)
	{
		this->m_strHorizontalUnits.Format("(%s)", units.horizontal.input);
		CGlobal::MinimizeLengthUnits(this->m_strHorizontalUnits);
	}
	else
	{
		this->m_strHorizontalUnits.Format("(%s)", units.horizontal.si);
	}

	if (units.vertical.defined)
	{
		this->m_strVerticalUnits.Format("(%s)", units.vertical.input);
		CGlobal::MinimizeLengthUnits(this->m_strVerticalUnits);
	}
	else
	{
		this->m_strVerticalUnits.Format("(%s)", units.vertical.si);
	}

	if (units.map_horizontal.defined)
	{
		this->m_strMapHorizontalUnits.Format("(%s)", units.map_horizontal.input);
		CGlobal::MinimizeLengthUnits(this->m_strMapHorizontalUnits);
	}
	else
	{
		this->m_strMapHorizontalUnits.Format("(%s)", units.horizontal.si);
	}

	if (units.map_vertical.defined)
	{
		this->m_strMapVerticalUnits.Format("(%s)", units.map_vertical.input);
		CGlobal::MinimizeLengthUnits(this->m_strMapVerticalUnits);
	}
	else
	{
		this->m_strMapVerticalUnits.Format("(%s)", units.vertical.si);
	}
}

void CBoxPropertiesDialogBar::OnBnClickedUpdateWedge()
{
	TRACE("%s, in\n", __FUNCTION__);
	if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
	{
		int axis;

		switch (this->GetCheckedRadioButton(IDC_RADIO_X, IDC_RADIO_Z))
		{
		case IDC_RADIO_X:
			axis = 0;
			break;
		case IDC_RADIO_Y:
			axis = 1;
			break;
		case IDC_RADIO_Z:
			axis = 2;
			break;
		default:
			ASSERT(FALSE);
			return;
		}

		srcWedgeSource::ChopType ct = srcWedgeSource::CHOP_NONE;
		switch (this->GetCheckedRadioButton(IDC_RADIO_1, IDC_RADIO_4))
		{
		case IDC_RADIO_1:
			if (axis == 0)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::X1);
			}
			else if (axis == 1)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::Y1);
			}
			else if (axis == 2)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::Z1);
			}
			break;
		case IDC_RADIO_2:
			if (axis == 0)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::X2);
			}
			else if (axis == 1)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::Y2);
			}
			else if (axis == 2)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::Z2);
			}
			break;
		case IDC_RADIO_3:
			if (axis == 0)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::X3);
			}
			else if (axis == 1)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::Y3);
			}
			else if (axis == 2)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::Z3);
			}
			break;
		case IDC_RADIO_4:
			if (axis == 0)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::X4);
			}
			else if (axis == 1)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::Y4);
			}
			else if (axis == 2)
			{
				ct = srcWedgeSource::ConvertWedgeOrientation(Wedge::Z4);
			}
			break;
		default:
			ASSERT(FALSE);
			return;
		}
		if (ct != srcWedgeSource::CHOP_NONE && ct != pZoneActor->GetChopType()) 
		{
			CAction *pAction = new WedgeChangeChopTypeAction(
				this->m_pView,
				pZoneActor,
				ct
				);

			if (pAction && this->m_pView)
			{
				this->m_pView->GetDocument()->Execute(pAction);
			}
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

HBRUSH CBoxPropertiesDialogBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
// COMMENT: {7/14/2008 9:39:52 PM}	static HBRUSH hbrRed = ::CreateSolidBrush(RGB(255, 0, 0));
// COMMENT: {7/14/2008 9:39:52 PM}
// COMMENT: {7/14/2008 9:39:52 PM}	static HBRUSH hbrWhite = ::CreateSolidBrush(RGB(0, 255, 0));
// COMMENT: {7/14/2008 9:39:52 PM}
// COMMENT: {7/14/2008 9:39:52 PM}	static HBRUSH hbrBlue = ::CreateSolidBrush(RGB(0, 0, 255));
// COMMENT: {7/14/2008 9:39:52 PM}
// COMMENT: {7/14/2008 9:39:52 PM}
// COMMENT: {7/14/2008 9:39:52 PM}	TCHAR szClass[16];
// COMMENT: {7/14/2008 9:39:52 PM}	::GetClassName(pWnd->GetSafeHwnd(), szClass, 16);
// COMMENT: {7/14/2008 9:39:52 PM}	TRACE("CBoxPropertiesDialogBar::OnCtlColor control class = %s\n", szClass);
// COMMENT: {7/14/2008 9:39:52 PM}	if (_tcsicmp(szClass, WC_COMBOBOX) == 0)
// COMMENT: {7/14/2008 9:39:52 PM}	{
// COMMENT: {7/14/2008 9:39:52 PM}		return hbrWhite;
// COMMENT: {7/14/2008 9:39:52 PM}	}
// COMMENT: {7/14/2008 9:39:52 PM}	if (_tcsicmp(szClass, WC_EDIT) == 0)
// COMMENT: {7/14/2008 9:39:52 PM}	{
// COMMENT: {7/14/2008 9:39:52 PM}		return hbrBlue;
// COMMENT: {7/14/2008 9:39:52 PM}	}
// COMMENT: {7/14/2008 9:39:52 PM}	if (_tcsicmp(szClass, "ComboLBox") == 0)
// COMMENT: {7/14/2008 9:39:52 PM}	{
// COMMENT: {7/14/2008 9:39:52 PM}		return hbrWhite;
// COMMENT: {7/14/2008 9:39:52 PM}	}
// COMMENT: {7/14/2008 9:39:52 PM}	return hbrRed;


	if (this->m_bThemeActive)
	{
		if (nCtlColor == CTLCOLOR_DLG)
		{
			// just return the brush for dialogs
			return this->m_hBrush;
		}

		TCHAR szClass[16];
		::GetClassName(pWnd->GetSafeHwnd(), szClass, 16);
// COMMENT: {7/22/2008 9:24:34 PM}		TRACE("CBoxPropertiesDialogBar::OnCtlColor control class = %s\n", szClass);
		if (_tcsicmp(szClass, WC_EDIT) == 0  && (pWnd->GetStyle() & ES_READONLY))
		{
			// use default for read-only edit controls
			return CSizingDialogBarCFVS7::OnCtlColor(pDC, pWnd, nCtlColor);
		}
		if (_tcsicmp(szClass, "ComboLBox") == 0)
		{
			return CSizingDialogBarCFVS7::OnCtlColor(pDC, pWnd, nCtlColor);
		}

		RECT rc;
		// Set the background mode to transparent
// COMMENT: {7/30/2008 4:22:33 PM}		pDC->SetBkMode(TRANSPARENT);

		// Get the controls window dimensions
		pWnd->GetWindowRect(&rc);

		// Map the coordinates to coordinates with the upper left corner of dialog control as base
		::MapWindowPoints(NULL, m_wndTab, (LPPOINT)(&rc), 2);	

		// Adjust the position of the brush for this control (else we see the top left of the brush as background)
		::SetBrushOrgEx((HDC)(*pDC), -rc.left, -rc.top, NULL);

		// Return the brush
		return this->m_hBrush;
	}
	else
	{
		return CSizingDialogBarCFVS7::OnCtlColor(pDC, pWnd, nCtlColor);
	}
}

void CBoxPropertiesDialogBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingDialogBarCFVS7::OnSize(nType, cx, cy);

	// Can't use m_wndTab since CBoxPropertiesDialogBar::DoDataExchange
	// may not have been called yet
	CWnd* pTab = this->GetDlgItem(IDC_TAB);
	if (pTab && pTab->GetSafeHwnd())
	{
		pTab->MoveWindow(0, 0, cx, cy);
		this->UpdateBackgroundBrush();
	}

	if (this->m_nType == CBoxPropertiesDialogBar::BP_PRISM)
	{
		this->SizePrismControls(cx, cy);
	}
}

void CBoxPropertiesDialogBar::UpdateBackgroundBrush()
{
	HMODULE hinstDll;

	// Check if the application is themed
	hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
	this->m_bThemeActive = FALSE;
	if (hinstDll)
	{
		typedef BOOL (*ISAPPTHEMEDPROC)();
		ISAPPTHEMEDPROC pIsAppThemed;
		pIsAppThemed = (ISAPPTHEMEDPROC) ::GetProcAddress(hinstDll, "IsAppThemed");

		if (pIsAppThemed)
		{
			this->m_bThemeActive = pIsAppThemed();
		}
		::FreeLibrary(hinstDll);
	}

	// Destroy old brush
	if (this->m_hBrush)
	{
		::DeleteObject(this->m_hBrush);
	}
	this->m_hBrush = 0;

	// Can't use m_wndTab since CBoxPropertiesDialogBar::DoDataExchange
	// may not have been called yet
	CWnd* pTab = this->GetDlgItem(IDC_TAB);

	// Only do this if the theme is active
	if (this->m_bThemeActive && pTab && pTab->GetSafeHwnd())
	{
		RECT rc;

		// Get tab control dimensions
		pTab->GetWindowRect(&rc);

		// Get the tab control DC
		CDC *pDC = pTab->GetDC();

		// Create a compatible DC
		HDC hDCMem = ::CreateCompatibleDC(pDC->GetSafeHdc());
		HBITMAP hBmp = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), rc.right - rc.left, rc.bottom - rc.top);
		HBITMAP hBmpOld = (HBITMAP)(::SelectObject(hDCMem, hBmp));

		// Tell the tab control to paint in our DC
		pTab->SendMessage(WM_PRINTCLIENT, (WPARAM)(hDCMem), (LPARAM)(PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT));

		// Create a pattern brush from the bitmap selected in our DC
		m_hBrush = ::CreatePatternBrush(hBmp);

		// Restore the bitmap
		::SelectObject(hDCMem, hBmpOld);

		// Cleanup
		::DeleteObject(hBmp);
		::DeleteDC(hDCMem);
		pTab->ReleaseDC(pDC);
	}
}

void CBoxPropertiesDialogBar::ShowPrismControls(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (this->m_wndTab.GetSafeHwnd() == 0)
	{
		return;
	}

	if (this->m_wndTab.GetItemCount() == 0)
	{
		this->m_wndTab.InsertItem(0, "Top");
		this->m_wndTab.InsertItem(1, "Perimeter");
		this->m_wndTab.InsertItem(2, "Bottom");
		this->m_nPrismPart = CBoxPropertiesDialogBar::PRISM_TOP;
	}

	for (int i = 0; i < sizeof(PrismIDs)/sizeof(PrismIDs[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(PrismIDs[i]))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
	}

	CRect rect;
	this->GetClientRect(&rect);
	this->SizePrismControls(rect.Width(), rect.Height());
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::SizePrismControls(int cx, int cy)
{
	CRect rect;
	this->GetClientRect(&rect);

	CRect rcC;
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_CONSTANT))
	{
		pWnd->GetWindowRect(&rcC);
		this->ScreenToClient(&rcC);
	}
	CRect rcS;
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_SHAPEFILE))
	{
		pWnd->GetWindowRect(&rcS);
		this->ScreenToClient(&rcS);
	}

	const int x = 5;
	//const int y = 165;
	const int y = rcS.top + rcS.top - rcC.top;
	this->m_wndPointsGrid.MoveWindow(x, y, cx-2*x, cy-y-x);
}

void CBoxPropertiesDialogBar::HidePrismControls(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (this->m_wndTab.GetSafeHwnd() == 0) return;
	this->m_wndTab.DeleteAllItems();

	RECT rc;
	for (int i = 0; i < sizeof(PrismIDs)/sizeof(PrismIDs[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(PrismIDs[i]))
		{
			pWnd->ShowWindow(SW_HIDE);
			pWnd->GetWindowRect(&rc);
			this->ScreenToClient(&rc);
			this->InvalidateRect(&rc, TRUE);
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRACE("%s, in\n", __FUNCTION__);
	ASSERT(this->m_nType == CBoxPropertiesDialogBar::BP_PRISM);

	ASSERT(this->m_nPrismPart == this->m_wndTab.GetCurSel());

	if (this->UpdateData(TRUE))
	{
		if (this->m_bNeedsUpdate)
		{
			this->OnApply();
		}
		*pResult = 0;
	}
	else
	{
		*pResult = 1;
	}
	ASSERT(this->m_nType == CBoxPropertiesDialogBar::BP_PRISM);
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRACE("%s, in\n", __FUNCTION__);
	ASSERT(this->m_nType == CBoxPropertiesDialogBar::BP_PRISM);

	this->m_nPrismPart = static_cast<CBoxPropertiesDialogBar::PRISM_PART>(this->m_wndTab.GetCurSel());
	this->m_bNeedsUpdate = false;

	this->UpdateData(FALSE);
	*pResult = 0;
	ASSERT(this->m_nType == CBoxPropertiesDialogBar::BP_PRISM);
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::UpdatePrism(CZoneActor *pZoneActor, bool bForceUpdate /* = FALSE */)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (Prism *p = dynamic_cast<Prism*>(pZoneActor->GetPolyhedron()))
	{
		*this->m_pds[PRISM_TOP]       = p->top;
		*this->m_pds[PRISM_PERIMETER] = p->perimeter;
		*this->m_pds[PRISM_BOTTOM]    = p->bottom;

		// constants (PRISM_PERIMETER not used)

		// top constant
		CZone defzone;
		this->m_pView->GetDocument()->GetDefaultZone(defzone);
		if (this->m_pds[PRISM_TOP]->Get_source_type() == Data_source::CONSTANT)
		{
			this->m_dConstant[PRISM_TOP] = this->m_pds[PRISM_TOP]->Get_points().begin()->z();
		}
		else
		{
			this->m_dConstant[PRISM_TOP] = defzone.z2;
		}

		// bottom constant
		if (this->m_pds[PRISM_BOTTOM]->Get_source_type() == Data_source::CONSTANT)
		{
			this->m_dConstant[PRISM_BOTTOM] = this->m_pds[PRISM_BOTTOM]->Get_points().begin()->z();
		}
		else
		{
			this->m_dConstant[PRISM_BOTTOM] = defzone.z1;
		}

		for (int i = 0; i < 3; ++i)
		{
			// arcraster
			if (this->m_pds[i]->Get_source_type() == Data_source::ARCRASTER)
			{
				this->m_sArcraster[i] = this->m_pds[i]->Get_file_name().c_str();
			}
			else
			{
				if ((this->m_pProp3D != pZoneActor) || bForceUpdate)
				{
					this->m_sArcraster[i] = "";
				}
			}

			// shape
			if (this->m_pds[i]->Get_source_type() == Data_source::SHAPE)
			{
				this->m_sShapefile[i]      = this->m_pds[i]->Get_file_name().c_str();
				this->m_nShapeAttribute[i] = this->m_pds[i]->Get_attribute();
			}
			else
			{
				// TODO do similar for other inputs
				if ((this->m_pProp3D != pZoneActor) || bForceUpdate)
				{
					this->m_sShapefile[i]      = "";
					this->m_nShapeAttribute[i] = -1;
				}
			}
		}


		// points
		for (int i = 0; i < 3; ++i)
		{
			this->m_listCoord[i].clear();
			if (this->m_pds[i]->Get_source_type() == Data_source::POINTS)
			{
				std::vector<Point>::iterator it = this->m_pds[i]->Get_points().begin();
				coord c;
				for (; it != this->m_pds[i]->Get_points().end(); ++it)
				{
#if 999
					if (this->m_pds[i]->Get_user_coordinate_system() == PHAST_Transform::MAP)
					{
						Point pt((*it).x(), (*it).y(), (*it).z());
						pZoneActor->GetPhastTransform().Inverse_transform(pt);
						c.c[0] = pt.x();
						c.c[1] = pt.y();
						c.c[2] = pt.z();
					}
					else
					{
#endif
						c.c[0] = (*it).x();
						c.c[1] = (*it).y();
						c.c[2] = (*it).z();
#if 999
					}
#endif
					this->m_listCoord[i].push_back(c);
				}
			}
			else if (this->m_pds[i]->Get_source_type() == Data_source::NONE ||
				this->m_pds[i]->Get_source_type() == Data_source::CONSTANT ||
				this->m_pds[i]->Get_source_type() == Data_source::ARCRASTER
				)
			{
				zone *z = &defzone; // TODO when opening wphast file no bounding_box exists??
				ASSERT(z->zone_defined);
				if (i == PRISM_TOP)
				{
					this->m_listCoord[i].push_back(coord(z->x1, z->y1, defzone.z2));
					this->m_listCoord[i].push_back(coord(z->x2, z->y1, defzone.z2));
					this->m_listCoord[i].push_back(coord(z->x2, z->y2, defzone.z2));
					this->m_listCoord[i].push_back(coord(z->x1, z->y2, defzone.z2));
				}
				else if (i == PRISM_PERIMETER)
				{
					this->m_listCoord[i].push_back(coord(defzone.x1, defzone.y1));
					this->m_listCoord[i].push_back(coord(defzone.x2, defzone.y1));
					this->m_listCoord[i].push_back(coord(defzone.x2, defzone.y2));
					this->m_listCoord[i].push_back(coord(defzone.x1, defzone.y2));
				}
				else if (i == PRISM_BOTTOM)
				{
					this->m_listCoord[i].push_back(coord(z->x1, z->y1, defzone.z1));
					this->m_listCoord[i].push_back(coord(z->x2, z->y1, defzone.z1));
					this->m_listCoord[i].push_back(coord(z->x2, z->y2, defzone.z1));
					this->m_listCoord[i].push_back(coord(z->x1, z->y2, defzone.z1));
				}
			}
			else if (this->m_pds[i]->Get_source_type() == Data_source::SHAPE)
			{
				zone *z = this->m_pds[i]->Get_bounding_box();
				ASSERT(z->zone_defined);
				if (i == PRISM_TOP)
				{
					this->m_listCoord[i].push_back(coord(z->x1, z->y1, z->z2));
					this->m_listCoord[i].push_back(coord(z->x2, z->y1, z->z2));
					this->m_listCoord[i].push_back(coord(z->x2, z->y2, z->z2));
					this->m_listCoord[i].push_back(coord(z->x1, z->y2, z->z2));
				}
				else if (i == PRISM_PERIMETER)
				{
					this->m_listCoord[i].push_back(coord(defzone.x1, defzone.y1));
					this->m_listCoord[i].push_back(coord(defzone.x2, defzone.y1));
					this->m_listCoord[i].push_back(coord(defzone.x2, defzone.y2));
					this->m_listCoord[i].push_back(coord(defzone.x1, defzone.y2));
				}
				else if (i == PRISM_BOTTOM)
				{
					this->m_listCoord[i].push_back(coord(z->x1, z->y1, z->z1));
					this->m_listCoord[i].push_back(coord(z->x2, z->y1, z->z1));
					this->m_listCoord[i].push_back(coord(z->x2, z->y2, z->z1));
					this->m_listCoord[i].push_back(coord(z->x1, z->y2, z->z1));
				}
			}
		}

// COMMENT: {7/23/2008 2:36:40 PM}		// top points
// COMMENT: {7/23/2008 2:36:40 PM}		this->m_listCoord[PRISM_TOP].clear();
// COMMENT: {7/23/2008 2:36:40 PM}		if (this->m_pds[PRISM_TOP]->Get_source_type() == Data_source::POINTS)
// COMMENT: {7/23/2008 2:36:40 PM}		{
// COMMENT: {7/23/2008 2:36:40 PM}			std::vector<Point>::iterator it = this->m_pds[PRISM_TOP]->Get_points().begin();
// COMMENT: {7/23/2008 2:36:40 PM}			coord c;
// COMMENT: {7/23/2008 2:36:40 PM}			for (; it != this->m_pds[PRISM_TOP]->Get_points().end(); ++it)
// COMMENT: {7/23/2008 2:36:40 PM}			{					
// COMMENT: {7/23/2008 2:36:40 PM}				c.c[0] = (*it).x();
// COMMENT: {7/23/2008 2:36:40 PM}				c.c[1] = (*it).y();
// COMMENT: {7/23/2008 2:36:40 PM}				c.c[2] = (*it).z();
// COMMENT: {7/23/2008 2:36:40 PM}				this->m_listCoord[PRISM_TOP].push_back(c);
// COMMENT: {7/23/2008 2:36:40 PM}			}
// COMMENT: {7/23/2008 2:36:40 PM}		}
// COMMENT: {7/23/2008 2:36:40 PM}		// perimeter points
// COMMENT: {7/23/2008 2:36:40 PM}		this->m_listCoord[PRISM_PERIMETER].clear();
// COMMENT: {7/23/2008 2:36:40 PM}		if (this->m_pds[PRISM_PERIMETER]->Get_source_type() == Data_source::POINTS)
// COMMENT: {7/23/2008 2:36:40 PM}		{
// COMMENT: {7/23/2008 2:36:40 PM}			std::vector<Point>::iterator it = this->m_pds[PRISM_PERIMETER]->Get_points().begin();
// COMMENT: {7/23/2008 2:36:40 PM}			coord c;
// COMMENT: {7/23/2008 2:36:40 PM}			for (; it != this->m_pds[PRISM_PERIMETER]->Get_points().end(); ++it)
// COMMENT: {7/23/2008 2:36:40 PM}			{					
// COMMENT: {7/23/2008 2:36:40 PM}				c.c[0] = (*it).x();
// COMMENT: {7/23/2008 2:36:40 PM}				c.c[1] = (*it).y();
// COMMENT: {7/23/2008 2:36:40 PM}				c.c[2] = (*it).z();
// COMMENT: {7/23/2008 2:36:40 PM}				this->m_listCoord[PRISM_PERIMETER].push_back(c);
// COMMENT: {7/23/2008 2:36:40 PM}			}
// COMMENT: {7/23/2008 2:36:40 PM}		}
// COMMENT: {7/23/2008 2:36:40 PM}		// bottom points
// COMMENT: {7/23/2008 2:36:40 PM}		this->m_listCoord[PRISM_BOTTOM].clear();
// COMMENT: {7/23/2008 2:36:40 PM}		if (this->m_pds[PRISM_BOTTOM]->Get_source_type() == Data_source::POINTS)
// COMMENT: {7/23/2008 2:36:40 PM}		{
// COMMENT: {7/23/2008 2:36:40 PM}			std::vector<Point>::iterator it = this->m_pds[PRISM_BOTTOM]->Get_points().begin();
// COMMENT: {7/23/2008 2:36:40 PM}			coord c;
// COMMENT: {7/23/2008 2:36:40 PM}			for (; it != this->m_pds[PRISM_BOTTOM]->Get_points().end(); ++it)
// COMMENT: {7/23/2008 2:36:40 PM}			{					
// COMMENT: {7/23/2008 2:36:40 PM}				c.c[0] = (*it).x();
// COMMENT: {7/23/2008 2:36:40 PM}				c.c[1] = (*it).y();
// COMMENT: {7/23/2008 2:36:40 PM}				c.c[2] = (*it).z();
// COMMENT: {7/23/2008 2:36:40 PM}				this->m_listCoord[PRISM_BOTTOM].push_back(c);
// COMMENT: {7/23/2008 2:36:40 PM}			}
// COMMENT: {7/23/2008 2:36:40 PM}		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnBnClickedData_source()
{
	TRACE("%s, in\n", __FUNCTION__);
	
	this->UpdatePrismControls();
	double d;
	switch (this->GetCheckedRadioButton(IDC_RADIO_NONE, IDC_RADIO_SHAPE))
	{
	case IDC_RADIO_NONE:
		this->OnApply();
		break;
	case IDC_RADIO_CONSTANT:
		if (IsValidFloatFormat(this, IDC_EDIT_CONSTANT, d))
		{
			this->OnApply();
		}
		break;
	case IDC_RADIO_ARCRASTER:
		if (CGlobal::IsValidArcraster(this->m_sArcraster[this->m_nPrismPart]))
		{
			this->OnApply();
		}
		break;
	case IDC_RADIO_SHAPE:
		//if (CGlobal::IsValidShapefile(this->m_sShapefile[this->m_nPrismPart]))
		{
// COMMENT: {7/28/2008 5:02:10 PM}			ASSERT(CGlobal::IsValidShapefile(this->m_sShapefile[this->m_nPrismPart]));
			CString str;
			this->GetDlgItemTextA(IDC_EDIT_SHAPEFILE, str);
			ASSERT(str.IsEmpty() || CGlobal::IsValidShapefile(this->m_sShapefile[this->m_nPrismPart]));
			if (CGlobal::IsValidShapefile(this->m_sShapefile[this->m_nPrismPart])
				&& (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER || this->m_wndShapeCombo.GetCurSel() != 0))
			{
				this->OnApply();
			}
			else
			{
				this->EnableShape(TRUE);
				this->EnablePrismRadios(TRUE);
			}
		}
		break;
	case IDC_RADIO_POINTS:
		this->OnApply();
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::EnablePointsGrid(BOOL bEnable)
{
	for (int iRow = this->m_wndPointsGrid.GetFixedRowCount(); iRow < this->m_wndPointsGrid.GetRowCount(); ++iRow)
	{
		for (int iCol = 0; iCol < this->m_wndPointsGrid.GetColumnCount(); ++iCol)
		{
			if (bEnable)
			{
				this->m_wndPointsGrid.EnableCell(iRow, iCol);
			}
			else
			{
				this->m_wndPointsGrid.DisableCell(iRow, iCol);
			}
		}
	}
	this->m_wndPointsGrid.Invalidate();

}

void CBoxPropertiesDialogBar::DoDataExchangePrism(CDataExchange *pDX, Data_source *pData_source)
{
// COMMENT: {7/28/2008 12:15:04 PM}	this->m_wndShapeCombo.ResetContent();
// COMMENT: {7/28/2008 12:15:04 PM}	this->m_wndShapeCombo.EnableWindow(1);
// COMMENT: {7/28/2008 12:15:04 PM}	this->m_wndShapeCombo.EnableWindow(0);

// COMMENT: {7/21/2008 11:27:54 PM}	this->m_wndPointsGrid.SetRowCount(0);

	if (pDX->m_bSaveAndValidate)
	{
		switch (this->GetCheckedRadioButton(IDC_RADIO_NONE, IDC_RADIO_SHAPE))
		{
		case IDC_RADIO_NONE:
			this->m_nDataSourceType = Data_source::NONE;
			break;
		case IDC_RADIO_CONSTANT:
			this->m_nDataSourceType = Data_source::CONSTANT;
			break;
		case IDC_RADIO_ARCRASTER:
			this->m_nDataSourceType = Data_source::ARCRASTER;
			break;
		case IDC_RADIO_POINTS:
			this->m_nDataSourceType = Data_source::POINTS;
			break;
		case IDC_RADIO_SHAPE:
			this->m_nDataSourceType = Data_source::SHAPE;
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		// constant
		if (this->m_nDataSourceType == Data_source::CONSTANT)
		{
			DDX_Text(pDX, IDC_EDIT_CONSTANT, this->m_dConstant[this->m_nPrismPart]);
		}

		// arcraster
		if (this->m_nDataSourceType == Data_source::ARCRASTER)
		{
			CString str;
			DDX_Text(pDX, IDC_EDIT_ARCRASTER, str);
			if (str.IsEmpty())
			{
				::AfxMessageBox("Please select an ARCRASTER file.");
				pDX->Fail();
			}
			ASSERT(CGlobal::IsValidArcraster(this->m_sArcraster[this->m_nPrismPart]));
		}

		// shape
		if (this->m_nDataSourceType == Data_source::SHAPE)
		{
			CString str;
			DDX_Text(pDX, IDC_EDIT_SHAPEFILE, str);
			if (str.IsEmpty())
			{
				ASSERT(this->m_sShapefile[this->m_nPrismPart].IsEmpty());
				::AfxMessageBox("Please select a shapefile.");
				pDX->Fail();
			}

			CGlobal::IsValidShapefile(this->m_sShapefile[this->m_nPrismPart], pDX);

			if (this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER)
			{
				this->m_nShapeAttribute[this->m_nPrismPart] = this->m_wndShapeCombo.GetCurSel() - 1;
				if (this->m_nShapeAttribute[this->m_nPrismPart] == -1)
				{
					::AfxMessageBox("Please select a shapefile attribute.");
					pDX->PrepareCtrl(IDC_COMBO_SHAPE);
					pDX->Fail();
				}
			}
		}

		// points
		if (this->m_nDataSourceType == Data_source::POINTS)
		{
			vtkPoints *points = vtkPoints::New();
			std::list<coord> new_list;
			try
			{
				double x, y, z;
				//this->m_listCoord[this->m_nPrismPart].clear();
				z = 0;
				for (int row = 1; row < this->m_wndPointsGrid.GetRowCount(); ++row)
				{
					DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 0, x);
					DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 1, y);
					if (this->m_nPrismPart == this->PRISM_PERIMETER)
					{
						//this->m_listCoord[this->m_nPrismPart].push_back(coord(x, y));
						new_list.push_back(coord(x, y));
					}
					else
					{
						DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 2, z);
						//this->m_listCoord[this->m_nPrismPart].push_back(coord(x, y, z));
						new_list.push_back(coord(x, y, z));
					}
// COMMENT: {7/25/2008 9:47:57 PM}					if (this->m_nPrismPart == this->PRISM_PERIMETER)
// COMMENT: {7/25/2008 9:47:57 PM}					{
// COMMENT: {7/25/2008 9:47:57 PM}						vtkFloatingPointType p1[3];
// COMMENT: {7/25/2008 9:47:57 PM}						vtkFloatingPointType p2[3];
// COMMENT: {7/25/2008 9:47:57 PM}						vtkFloatingPointType x1[3];
// COMMENT: {7/25/2008 9:47:57 PM}						vtkFloatingPointType x2[3];
// COMMENT: {7/25/2008 9:47:57 PM}						vtkFloatingPointType u;
// COMMENT: {7/25/2008 9:47:57 PM}						vtkFloatingPointType v;
// COMMENT: {7/25/2008 9:47:57 PM}						p1[0] = x;
// COMMENT: {7/25/2008 9:47:57 PM}						p1[1] = y;
// COMMENT: {7/25/2008 9:47:57 PM}						p1[2] = z;
// COMMENT: {7/25/2008 9:47:57 PM}						if (3 < points->GetNumberOfPoints())
// COMMENT: {7/25/2008 9:47:57 PM}						{
// COMMENT: {7/25/2008 9:47:57 PM}							points->GetPoint(points->GetNumberOfPoints() - 1, p2);
// COMMENT: {7/25/2008 9:47:57 PM}							for (vtkIdType i = 3; i < points->GetNumberOfPoints(); ++i)
// COMMENT: {7/25/2008 9:47:57 PM}							{
// COMMENT: {7/25/2008 9:47:57 PM}								points->GetPoint(i-2, x1);
// COMMENT: {7/25/2008 9:47:57 PM}								points->GetPoint(i, x2);
// COMMENT: {7/25/2008 9:47:57 PM}								if (vtkLine::Intersection(p1, p2, x1, x2, u, v) == 2)
// COMMENT: {7/25/2008 9:47:57 PM}								{
// COMMENT: {7/25/2008 9:47:57 PM}									char msg[] = "Perimeter must not cross itself.";
// COMMENT: {7/25/2008 9:47:57 PM}									//this->m_wndPointsGrid.SetSelectedRange(row, 0, row, 1, TRUE);
// COMMENT: {7/25/2008 9:47:57 PM}									DDX_GridControlFail(pDX, IDC_GRID_POINTS, row, 0, row, 1, msg);
// COMMENT: {7/25/2008 9:47:57 PM}									//::AfxMessageBox("Perimeter must not cross itself.");
// COMMENT: {7/25/2008 9:47:57 PM}									//pDX->Fail();
// COMMENT: {7/25/2008 9:47:57 PM}								}
// COMMENT: {7/25/2008 9:47:57 PM}							}
// COMMENT: {7/25/2008 9:47:57 PM}						}
// COMMENT: {7/25/2008 9:47:57 PM}						points->InsertNextPoint(x, y, z);
// COMMENT: {7/25/2008 9:47:57 PM}					}
				}
				points->Delete();
			}
			catch(...)
			{
				points->Delete();
				throw;
			}
			this->m_listCoord[this->m_nPrismPart] = new_list;
		}
	}
	else
	{
		// select radio button
		switch(pData_source->Get_source_type())
		{
		case Data_source::NONE:
			this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_SHAPE, IDC_RADIO_NONE);
			break;
		case Data_source::CONSTANT:
			if (pData_source->Get_user_source_type() == Data_source::CONSTANT)
			{
				this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_SHAPE, IDC_RADIO_CONSTANT);
			}
			else
			{
				this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_SHAPE, IDC_RADIO_NONE);
			}
			break;
		case Data_source::ARCRASTER:
			this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_SHAPE, IDC_RADIO_ARCRASTER);
			break;
		case Data_source::POINTS:
			this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_SHAPE, IDC_RADIO_POINTS);
			break;
		case Data_source::SHAPE:
			this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_SHAPE, IDC_RADIO_SHAPE);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		this->UpdatePrismControls();


		// enable/disable
		if (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER)
		{
			// constant
			DDX_Text(pDX, IDC_EDIT_CONSTANT, CString("N/A"));

			// arcraster
			ASSERT(pData_source->Get_source_type() != Data_source::ARCRASTER);
			DDX_Text(pDX, IDC_EDIT_ARCRASTER, CString("N/A"));

			// shapefile
			//DDX_Text(pDX, IDC_EDIT_SHAPEFILE, this->m_sShapefile[this->m_nPrismPart]);
			::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_SHAPEFILE, this->m_sShapefile[this->m_nPrismPart]);
		}
		else
		{
			// TOP or BOTTOM

			// constant
			DDX_Text(pDX, IDC_EDIT_CONSTANT, this->m_dConstant[this->m_nPrismPart]);

			// arcraster
			//DDX_Text(pDX, IDC_EDIT_ARCRASTER, this->m_sArcraster[this->m_nPrismPart]);
			::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_ARCRASTER, this->m_sArcraster[this->m_nPrismPart]);

			// shapefile
			//DDX_Text(pDX, IDC_EDIT_SHAPEFILE, this->m_sShapefile[this->m_nPrismPart]);
			::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_SHAPEFILE, this->m_sShapefile[this->m_nPrismPart]);

			if (CGlobal::IsValidShapefile(this->m_sShapefile[this->m_nPrismPart]))
			{
				this->m_wndShapeCombo.ResetContent();

				std::string s = this->m_sShapefile[this->m_nPrismPart];
				Shapefile sf(s, PHAST_Transform::MAP);
				std::vector< std::string > headings = sf.Get_headers();

				CGlobal::InsertHeadings(this->m_wndShapeCombo, headings);

				int n = this->m_nShapeAttribute[this->m_nPrismPart];
				if (n >= -1)
				{
					this->m_wndShapeCombo.SetCurSel(n + 1);
				}
			}
		}

		// points
		if (this->m_listCoord[this->m_nPrismPart].size() > 0)
		{
			// Prepare points grid
			//
			TRY
			{
				this->m_wndPointsGrid.SetRowCount((int)(1 + this->m_listCoord[this->m_nPrismPart].size()));
				if (this->m_nPrismPart == PRISM_PERIMETER)
				{
					this->m_wndPointsGrid.SetColumnCount(2);
				}
				else
				{
					this->m_wndPointsGrid.SetColumnCount(3);
				}
				this->m_wndPointsGrid.SetFixedRowCount(1);
				this->m_wndPointsGrid.SetFixedColumnCount(0);
				this->m_wndPointsGrid.EnableTitleTips(FALSE);
			}
			CATCH (CMemoryException, e)
			{
				e->ReportError();
				e->Delete();
			}
			END_CATCH

			CString title_x(_T("X "));
			title_x += this->m_strHorizontalUnits;
			DDX_TextGridControl(pDX, IDC_GRID_POINTS, 0, 0, title_x);

			CString title_y(_T("Y "));
			title_y += this->m_strHorizontalUnits;
			DDX_TextGridControl(pDX, IDC_GRID_POINTS, 0, 1, title_y);

			if (this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER)
			{
				CString title_z(_T("Z "));
				title_z += this->m_strVerticalUnits;
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, 0, 2, title_z);
			}

			//std::vector<Point>::iterator iter = pts.begin();
			std::list<coord>::iterator iter = this->m_listCoord[this->m_nPrismPart].begin();
			for (int row = 1; row < this->m_wndPointsGrid.GetRowCount(); ++row, ++iter)
			{
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 0, (*iter).c[0]);
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 1, (*iter).c[1]);
				if (this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER)
				{
					DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 2, (*iter).c[2]);
					this->m_wndPointsGrid.RedrawCell(row, 2);
				}
				this->m_wndPointsGrid.RedrawCell(row, 0);
				this->m_wndPointsGrid.RedrawCell(row, 1);
			}
			this->m_wndPointsGrid.SetModified(FALSE);
			for (int r = 1; r < this->m_wndPointsGrid.GetRowCount(); ++r)
			{
				ASSERT(!this->m_wndPointsGrid.GetModified(r, 0));
				ASSERT(!this->m_wndPointsGrid.GetModified(r, 1));
				if (this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER)
				{
					ASSERT(!this->m_wndPointsGrid.GetModified(r, 2));
				}
			}
			this->m_wndPointsGrid.RedrawWindow();
			this->m_wndPointsGrid.ExpandColumnsToFit();
			this->m_wndPointsGrid.SetColumnWidth(0, this->m_wndPointsGrid.GetColumnWidth(0) - 1);
			this->m_wndPointsGrid.SetColumnWidth(1, this->m_wndPointsGrid.GetColumnWidth(1) - 1);
			if (this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER)
			{
				int nwidth = this->m_wndPointsGrid.GetColumnWidth(2);
				if (nwidth < 10) nwidth = 10;
				this->m_wndPointsGrid.SetColumnWidth(2, nwidth - 1);
			}
		}
		else
		{
			this->m_wndPointsGrid.SetRowCount(0);
		}

		// TODO this may be redundant
		if (pData_source->Get_source_type() == Data_source::POINTS)
		{
			this->EnablePointsGrid(TRUE);
		}
		else
		{
			this->EnablePointsGrid(FALSE);
		}
		this->RedrawWindow();
	}
}

void CBoxPropertiesDialogBar::ApplyNewPrism(CZoneActor *pZoneActor)
{
	if (Prism *prism = dynamic_cast<Prism*>(pZoneActor->GetPolyhedron()))
	{
		CWaitCursor wait;

		// Update StatusBar
		//
		if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
		{
			CString status(_T("Generating new prism..."));
			pWnd->SetWindowText(status);
		}

		Data_source new_data_source;
		if (this->m_nDataSourceType == Data_source::NONE)
		{
			// do nothing (default type is Data_source::NONE)
		}
		else
		{
			std::ostringstream oss;
			oss.precision(DBL_DIG);
			if (this->m_nDataSourceType == Data_source::CONSTANT)
			{
				ASSERT(this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER);
				Data_source ds;
				switch(this->m_nPrismPart)
				{
				case CBoxPropertiesDialogBar::PRISM_TOP:
					ds = prism->top;
					break;
				case CBoxPropertiesDialogBar::PRISM_PERIMETER:
					ds = prism->perimeter;
					break;
				case CBoxPropertiesDialogBar::PRISM_BOTTOM:
					ds = prism->bottom;
					break;
				}
				std::vector<Point> &pts = ds.Get_points();
				pts.clear();
				pts.push_back(Point(0.0, 0.0, this->m_dConstant[this->m_nPrismPart], this->m_dConstant[this->m_nPrismPart]));
				oss << ds << std::endl;
			}
			else if (this->m_nDataSourceType == Data_source::ARCRASTER)
			{
				ASSERT(this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER);
				if (CGlobal::FileExists(this->m_sArcraster[this->m_nPrismPart]))
				{
					Data_source ds;
					ds.Set_source_type(Data_source::ARCRASTER);

					PHAST_Transform::COORDINATE_SYSTEM c = PHAST_Transform::MAP;
					ds.Set_coordinate_system(c);
					ds.Set_user_coordinate_system(c);

					std::string file_name(this->m_sArcraster[this->m_nPrismPart]);
					ds.Set_file_name(file_name);
					oss << ds << std::endl;
					////oss << "ARCRASTER " << " MAP " << this->m_sArcraster[this->m_nPrismPart] << std::endl;
				}
			}
			else if (this->m_nDataSourceType == Data_source::SHAPE)
			{
				if (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER)
				{
					Data_source ds;
					ds.Set_source_type(Data_source::SHAPE);

					PHAST_Transform::COORDINATE_SYSTEM c = PHAST_Transform::MAP;
					ds.Set_coordinate_system(c);
					ds.Set_user_coordinate_system(c);

					std::string file_name(this->m_sShapefile[this->m_nPrismPart]);
					ds.Set_file_name(file_name);
					oss << ds << std::endl;
					////oss << "SHAPE" << " MAP " << this->m_sShapefile[this->m_nPrismPart] << std::endl;
				}
				else
				{
					const char* coor_name[] = {" MAP ", " GRID ", " NONE "};
					PHAST_Transform::COORDINATE_SYSTEM c = PHAST_Transform::MAP;
					oss << "SHAPE" << coor_name[c] << this->m_sShapefile[this->m_nPrismPart];
					if (this->m_nShapeAttribute[this->m_nPrismPart] != -1)
					{
						oss << " " << this->m_nShapeAttribute[this->m_nPrismPart];
					}
					oss << std::endl;
				}
			}
			else if (this->m_nDataSourceType == Data_source::POINTS)
			{
				Data_source ds;
				switch(this->m_nPrismPart)
				{
				case CBoxPropertiesDialogBar::PRISM_TOP:
					ds = prism->top;
					break;
				case CBoxPropertiesDialogBar::PRISM_PERIMETER:
					ds = prism->perimeter;
					break;
				case CBoxPropertiesDialogBar::PRISM_BOTTOM:
					ds = prism->bottom;
					break;
				}
				ASSERT(ds.Get_points().size() > 2);
				std::vector<Point> &pts = ds.Get_points();
				pts.clear();
				std::list<coord>::iterator cit = this->m_listCoord[this->m_nPrismPart].begin();
				for (; cit != this->m_listCoord[this->m_nPrismPart].end(); ++cit)
				{
					pts.push_back(Point(cit->c[0], cit->c[1], cit->c[2], cit->c[2]));
				}
				ds.Tidy(this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER);
				oss << ds << std::endl;
			}

			TRACE(oss.str().c_str());
			std::istringstream iss(oss.str());

			// read new part into Data_source
			//
			bool read_number = (this->m_nPrismPart != PRISM_PERIMETER) && (this->m_nDataSourceType == Data_source::SHAPE);
			new_data_source.Read(iss, read_number);
			new_data_source.Tidy(true);
		}

		// set new data_source
		Prism copy(*prism);
		switch(this->m_nPrismPart)
		{
		case CBoxPropertiesDialogBar::PRISM_TOP:
			copy.top = new_data_source;
			break;
		case CBoxPropertiesDialogBar::PRISM_PERIMETER:
			copy.perimeter = new_data_source;
			break;
		case CBoxPropertiesDialogBar::PRISM_BOTTOM:
			copy.bottom = new_data_source;
			break;
		}

		copy.Tidy();

		// dump new prism
		std::ostringstream prism_oss;
		prism_oss.precision(DBL_DIG);
		prism_oss << copy;
		TRACE(prism_oss.str().c_str());
		std::istringstream prism_iss(prism_oss.str());

		// remove first line (-prism)
		std::string line;
		std::getline(prism_iss, line);
		ASSERT(line.find("-prism") != std::string::npos);

		// read new prism
		Prism new_prism;
		while(new_prism.Read(prism_iss))
		{
			if (prism_iss.rdstate() & std::ios::eofbit) break;
			prism_iss.clear();
		}

		// setup domain
		this->m_pView->GetDocument()->GetDefaultZone(::domain);

		new_prism.Tidy();
		new_prism.Convert_coordinates(PHAST_Transform::GRID, &pZoneActor->GetPhastTransform());

		CZonePrismResetAction *pAction = new CZonePrismResetAction(
			this->m_pView,
			pZoneActor,
			&new_prism
			);

		this->m_pView->GetDocument()->Execute(pAction);


		// save points
		std::list<coord> save = this->m_listCoord[this->m_nPrismPart];
		this->UpdatePrism(pZoneActor);

		// restore points
		this->m_listCoord[this->m_nPrismPart] = save;

	}

	// Update StatusBar
	//
	if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
	{
		CString status;
		status.LoadStringA(AFX_IDS_IDLEMESSAGE);
		pWnd->SetWindowText(status);
	}
}

void CBoxPropertiesDialogBar::EnableNone(BOOL bEnable)
{
	// do nothing
}

void CBoxPropertiesDialogBar::EnableConstant(BOOL bEnable)
{
	if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_CONSTANT))
	{
		pEdit->EnableWindow(TRUE);
		pEdit->SetReadOnly(!bEnable);
	}
}

void CBoxPropertiesDialogBar::EnableArcraster(BOOL bEnable)
{
	if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_ARCRASTER))
	{
		pEdit->EnableWindow(TRUE);
		pEdit->SetReadOnly(TRUE);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_BUTTON_ARCRASTER))
	{
		pWnd->EnableWindow(bEnable);
	}
}

void CBoxPropertiesDialogBar::EnableShape(BOOL bEnable)
{
	if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_SHAPEFILE))
	{
		pEdit->EnableWindow(TRUE);
		pEdit->SetReadOnly(TRUE);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_BUTTON_SHAPE))
	{
		pWnd->EnableWindow(bEnable);
	}
	if (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER)
	{
		if (CWnd* pWnd = this->GetDlgItem(IDC_COMBO_SHAPE))
		{
			pWnd->EnableWindow(FALSE);
		}
	}
	else
	{
		if (CWnd* pWnd = this->GetDlgItem(IDC_COMBO_SHAPE))
		{
			pWnd->EnableWindow(bEnable);
		}
	}
}

void CBoxPropertiesDialogBar::EnablePoints(BOOL bEnable)
{
	this->EnablePointsGrid(bEnable);
}

void CBoxPropertiesDialogBar::EnablePrismRadios(BOOL bEnable)
{
	if (CWnd* pWnd = this->GetDlgItem(IDC_RADIO_NONE))
	{
		pWnd->EnableWindow(bEnable);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_RADIO_CONSTANT))
	{
		if (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER)
		{
			pWnd->EnableWindow(FALSE);
		}
		else
		{
			pWnd->EnableWindow(bEnable);
		}
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_RADIO_ARCRASTER))
	{
		if (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER)
		{
			pWnd->EnableWindow(FALSE);
		}
		else
		{
			pWnd->EnableWindow(bEnable);
		}
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_RADIO_SHAPE))
	{
		pWnd->EnableWindow(bEnable);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_RADIO_POINTS))
	{
		pWnd->EnableWindow(bEnable);
	}
}

void CBoxPropertiesDialogBar::OnBnClickedArcraster(void)
{
	static char szFilters[] =
		"ARC/INFO ASCII Grid Files (*.txt)|*.txt|"
		"All Files (*.*)|*.*|"
		"|";

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog fileDlg(TRUE, "txt", NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	if (this->m_sArcraster[this->m_nPrismPart].GetLength())
	{
		TCHAR buffer[MAX_PATH];
		::strcpy(buffer, this->m_sArcraster[this->m_nPrismPart]);
		if (::PathRemoveFileSpec(buffer))
		{
			fileDlg.m_ofn.lpstrInitialDir = buffer;
		}
	}

	if (fileDlg.DoModal() == IDOK)
	{
		this->m_sArcraster[this->m_nPrismPart] = fileDlg.GetPathName();
		::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_ARCRASTER, this->m_sArcraster[this->m_nPrismPart]);
		if (CGlobal::IsValidArcraster(this->m_sArcraster[this->m_nPrismPart]))
		{
			this->OnApply();
		}
	}
}

void CBoxPropertiesDialogBar::OnBnClickedShape(void)
{
	static char szFilters[] =
		"Shapefile Files (*.shp)|*.shp|"
		"All Files (*.*)|*.*|"
		"|";

	// Create an Open dialog; the default file name extension is ".shp".
	CFileDialog fileDlg(TRUE, "shp", NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	if (this->m_sShapefile[this->m_nPrismPart].GetLength())
	{
		TCHAR buffer[MAX_PATH];
		::strcpy(buffer, this->m_sShapefile[this->m_nPrismPart]);
		if (::PathRemoveFileSpec(buffer))
		{
			fileDlg.m_ofn.lpstrInitialDir = buffer;
		}
	}

	if (fileDlg.DoModal() == IDOK)
	{
		this->m_sShapefile[this->m_nPrismPart] = fileDlg.GetPathName();
		// TODO check if shape file has changed
		//
		PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_SHAPEFILE, this->m_sShapefile[this->m_nPrismPart]);

		// add headings
		if (CGlobal::IsValidShapefile(this->m_sShapefile[this->m_nPrismPart]))
		{
			this->m_wndShapeCombo.ResetContent();

			if (this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER)
			{
				std::string s = this->m_sShapefile[this->m_nPrismPart];
				Shapefile sf(s, PHAST_Transform::MAP);
				std::vector< std::string > headings = sf.Get_headers();

				CGlobal::InsertHeadings(this->m_wndShapeCombo, headings);

				int n = this->m_nShapeAttribute[this->m_nPrismPart];
				if (n >= -1)
				{
					this->m_wndShapeCombo.SetCurSel(n + 1);
				}
			}
			else
			{
				this->OnApply();
			}
		}
	}
}

void CBoxPropertiesDialogBar::UpdatePrismControls(void)
{
	switch(this->GetCheckedRadioButton(IDC_RADIO_NONE, IDC_RADIO_SHAPE))
	{
	case IDC_RADIO_NONE:
		this->EnableNone(TRUE);
		this->EnableConstant(FALSE);
		this->EnableArcraster(FALSE);
		this->EnableShape(FALSE);
		this->EnablePoints(FALSE);
		this->EnablePrismRadios(TRUE);
		break;
	case IDC_RADIO_CONSTANT:
		if (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER)
		{
			ASSERT(FALSE);
		}
		else
		{
			this->EnableNone(FALSE);
			this->EnableConstant(TRUE);
			this->EnableArcraster(FALSE);
			this->EnableShape(FALSE);
			this->EnablePoints(FALSE);
			this->EnablePrismRadios(TRUE);
		}
		break;
	case IDC_RADIO_ARCRASTER:
		if (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER)
		{
			ASSERT(FALSE);
		}
		else
		{
			this->EnableNone(FALSE);
			this->EnableConstant(FALSE);
			this->EnableArcraster(TRUE);
			this->EnableShape(FALSE);
			this->EnablePoints(FALSE);
			this->EnablePrismRadios(TRUE);
		}
		break;
	case IDC_RADIO_POINTS:
		this->EnableNone(FALSE);
		this->EnableConstant(FALSE);
		this->EnableArcraster(FALSE);
		this->EnableShape(FALSE);
		this->EnablePoints(TRUE);
		this->EnablePrismRadios(TRUE);
		break;
	case IDC_RADIO_SHAPE:
		if (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER)
		{
			this->EnableNone(FALSE);
			this->EnableConstant(FALSE);
			this->EnableArcraster(FALSE);
			this->EnableShape(FALSE);
			this->EnablePoints(FALSE);
			if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_SHAPEFILE))
			{
				pEdit->EnableWindow(TRUE);
				pEdit->SetReadOnly(TRUE);
			}
			this->EnablePrismRadios(FALSE);
		}
		else
		{
			this->EnableNone(FALSE);
			this->EnableConstant(FALSE);
			this->EnableArcraster(FALSE);
			this->EnableShape(TRUE);
			this->EnablePoints(FALSE);
			this->EnablePrismRadios(TRUE);
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

void CBoxPropertiesDialogBar::OnCbnSelChangeShape(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	CString str;
	this->GetDlgItemTextA(IDC_EDIT_SHAPEFILE, str);
	ASSERT(!str.IsEmpty());
	ASSERT(!this->m_sShapefile[this->m_nPrismPart].IsEmpty());
	ASSERT(CGlobal::IsValidShapefile(this->m_sShapefile[this->m_nPrismPart]));
	ASSERT(this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER);
	if (this->m_wndShapeCombo.GetCurSel() != 0)
	{
		this->OnApply();
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnEnKillfocusConstant()
{
	TRACE("%s, in\n", __FUNCTION__);
// COMMENT: {2/19/2009 9:56:17 PM}	ASSERT(this->m_nType == CBoxPropertiesDialogBar::BP_PRISM);
	if (this->m_nType == CBoxPropertiesDialogBar::BP_PRISM)
	{
		if (this->m_bNeedsUpdate)
		{
			double d;
			if (IsValidFloatFormat(this, IDC_EDIT_CONSTANT, d))
			{
				this->OnApply();
			}
			else
			{
				this->UpdateData(FALSE);
			}
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnEnChangeConstant()
{
	TRACE("%s, in\n", __FUNCTION__);
	ASSERT(this->m_nType == CBoxPropertiesDialogBar::BP_PRISM);
	double d;
	if (!(IsValidFloatFormat(this, IDC_EDIT_CONSTANT, d) && this->m_dConstant[this->m_nPrismPart] == d))
	{
		TRACE("%s, this->m_bNeedsUpdate set to true\n", __FUNCTION__);
		this->m_bNeedsUpdate = true;
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::TestPointsGrid(NMHDR *pNotifyStruct, LRESULT *result, BOOL bShowErrors)
{
	TRACE("%s, in\n", __FUNCTION__);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	double dCell;
	coord c;
	TCHAR szBuffer[40];
	if (IsValidFloatFormat(this, IDC_GRID_POINTS, pnmgv->iRow, pnmgv->iColumn, dCell))
	{
		if (this->m_nPrismPart == this->PRISM_PERIMETER)
		{
			std::vector<coord> vect;
			std::list<coord>::iterator li = this->m_listCoord[PRISM_PERIMETER].begin();
			for (; li != this->m_listCoord[PRISM_PERIMETER].end(); ++li)
			{
				vect.push_back(*li);
			}
			for (size_t i = 0; i < vect.size(); ++i)
			{
				TRACE("Points %d=%g, %g, %g\n", i, vect[i].c[0], vect[i].c[1], vect[i].c[2]);
			}

			// check original
			ASSERT(vect.size() > 2);

			double *a, *b, *c, *d;
			double rn, sn, den, r, s;
			a = vect[0].c;
			for (size_t i = 0; i < vect.size(); ++i)
			{
				b = vect[(i + 1) % vect.size()].c;
				c = vect[(i + 2) % vect.size()].c;
				for (size_t j = i+2; j < i+vect.size()-2; ++j)
				{
					d = vect[(j + 1) % vect.size()].c;
					rn = (a[1]-c[1])*(d[0]-c[0])-(a[0]-c[0])*(d[1]-c[1]);
					sn = (a[1]-c[1])*(b[0]-a[0])-(a[0]-c[0])*(b[1]-a[1]);
					den = (b[0]-a[0])*(d[1]-c[1])-(b[1]-a[1])*(d[0]-c[0]);
					if (den != 0)
					{
						r = rn/den;
						s = sn/den;
						if (r >= 0 && r <= 1 && s >= 0 && s <= 1)
						{
							ASSERT(FALSE);
							*result = 1;
							return; // return true;
						}
					}
					else if (rn == 0)
					{
						// both AB and CD are collinear (coincident)
						// project values to each axis to check for overlap
						for (i = 0; i < 2; ++i)
						{
							double minab = (a[i] < b[i]) ? a[i] : b[i]; // Math.min(a[i], b[i]);
							double maxab = (a[i] > b[i]) ? a[i] : b[i]; // Math.max(a[i], b[i]);
							if (minab <= c[i] && c[i] <= maxab)
							{
								ASSERT(FALSE);
								*result = 1;
								return; // return true;
							}
							if (minab <= d[i] && d[i] <= maxab)
							{
								ASSERT(FALSE);
								*result = 1;
								return; // return true;
							}
						}
					}
					c = d;
				}
				a = b;
			}

			_stprintf(szBuffer, _T("%.*g"), DBL_DIG, vect[pnmgv->iRow - 1].c[pnmgv->iColumn]);
			vect[pnmgv->iRow - 1].c[pnmgv->iColumn] = dCell;
			{

				double *a, *b, *c, *d;
				double rn, sn, den, r, s;
				a = vect[0].c;
				for (size_t i = 0; i < vect.size(); ++i)
				{
					b = vect[(i + 1) % vect.size()].c;
					c = vect[(i + 2) % vect.size()].c;
					for (size_t j = i+2; j < i+vect.size()-2; ++j)
					{
						d = vect[(j + 1) % vect.size()].c;
						rn = (a[1]-c[1])*(d[0]-c[0])-(a[0]-c[0])*(d[1]-c[1]);
						sn = (a[1]-c[1])*(b[0]-a[0])-(a[0]-c[0])*(b[1]-a[1]);
						den = (b[0]-a[0])*(d[1]-c[1])-(b[1]-a[1])*(d[0]-c[0]);
						if (den != 0)
						{
							r = rn/den;
							s = sn/den;
							if (r >= 0 && r <= 1 && s >= 0 && s <= 1)
							{
								if (bShowErrors)
								{
									::AfxMessageBox("Perimeter cannot cross itself. Resetting original coordinates.");
								}
								VERIFY(this->m_wndPointsGrid.SetItemText(pnmgv->iRow, pnmgv->iColumn, szBuffer));
								return;
							}
						}
						else if (rn == 0)
						{
							// both AB and CD are collinear (coincident)
							// project values to each axis to check for overlap
							for (i = 0; i < 2; ++i)
							{
								double minab = (a[i] < b[i]) ? a[i] : b[i]; // Math.min(a[i], b[i]);
								double maxab = (a[i] > b[i]) ? a[i] : b[i]; // Math.max(a[i], b[i]);
								if (minab <= c[i] && c[i] <= maxab)
								{
									if (bShowErrors)
									{
										::AfxMessageBox("Perimeter cannot cross itself. Resetting original coordinates.");
									}
									VERIFY(this->m_wndPointsGrid.SetItemText(pnmgv->iRow, pnmgv->iColumn, szBuffer));
									return;
								}
								if (minab <= d[i] && d[i] <= maxab)
								{
									if (bShowErrors)
									{
										::AfxMessageBox("Perimeter cannot cross itself. Resetting original coordinates.");
									}
									VERIFY(this->m_wndPointsGrid.SetItemText(pnmgv->iRow, pnmgv->iColumn, szBuffer));
									return;
								}
							}
						}
						c = d;
					}
					a = b;
				}
			}
		}
		_stprintf(szBuffer, _T("%.*g"), DBL_DIG, dCell);
		VERIFY(this->m_wndPointsGrid.SetItemText(pnmgv->iRow, pnmgv->iColumn, szBuffer));
		this->OnApply();
	}
	else
	{
		this->UpdateData(FALSE);
	}

	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnBeginLabelEditPointsGrid(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("%s, in\n", __FUNCTION__);
	this->m_bNeedsUpdate = true;
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnBnClickedUseMap(void)
{
	switch (this->m_nType)
	{
	case CBoxPropertiesDialogBar::BP_MIN_MAX:
		this->OnUseMapBox();
		break;
	case CBoxPropertiesDialogBar::BP_WELL:
		this->OnUseMapWell();
		break;
	case CBoxPropertiesDialogBar::BP_DRAIN:
		this->OnUseMapDrain();
		break;
	case CBoxPropertiesDialogBar::BP_RIVER:
		this->OnUseMapRiver();
		break;		
	}
}

void CBoxPropertiesDialogBar::OnUseMapWell(void)
{
	const CUnits& units = this->m_pView->GetDocument()->GetUnits();
	const CGridKeyword& gridKeyword = this->m_pView->GetDocument()->GetGridKeyword();
	PHAST_Transform t(
		gridKeyword.m_grid_origin[0],
		gridKeyword.m_grid_origin[1],
		gridKeyword.m_grid_origin[2],
		gridKeyword.m_grid_angle,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_vertical.input_to_si/units.vertical.input_to_si
		);

	Point p(
		this->m_XWell,
		this->m_YWell,
		0.0
		);

	if (this->IsDlgButtonChecked(IDC_CHECK_USE_MAP) == BST_CHECKED)
	{
		t.Inverse_transform(p);
		this->m_XWell = p.x();
		this->m_YWell = p.y();
		this->m_xy_coordinate_system_user = PHAST_Transform::MAP;
	}
	else
	{
		t.Transform(p);
		this->m_XWell = p.x();
		this->m_YWell = p.y();
		this->m_xy_coordinate_system_user = PHAST_Transform::GRID;
	}

	if (CWellActor* pWellActor = this->m_pProp3D ? CWellActor::SafeDownCast(this->m_pProp3D) : NULL)
	{
		CAction *pAction = new CWellSetPositionAction(
			pWellActor,
			this->m_pView->GetDocument(),
			this->m_XWell,
			this->m_YWell,
			this->m_xy_coordinate_system_user
			);
		if (pAction)
		{
			this->m_pView->GetDocument()->Execute(pAction);
		}
	}
}

void CBoxPropertiesDialogBar::OnUseMapBox(void)
{
	const CUnits& units = this->m_pView->GetDocument()->GetUnits();
	const CGridKeyword& gridKeyword = this->m_pView->GetDocument()->GetGridKeyword();
	PHAST_Transform map2grid(
		gridKeyword.m_grid_origin[0],
		gridKeyword.m_grid_origin[1],
		gridKeyword.m_grid_origin[2],
		gridKeyword.m_grid_angle,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_vertical.input_to_si/units.vertical.input_to_si
		);

	if (this->IsDlgButtonChecked(IDC_CHECK_USE_MAP) == BST_CHECKED)
	{
		// Grid => Map
		// Box => Prism
		//
		if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
		{
			ASSERT(pZoneActor->GetPolyhedronType() == Polyhedron::CUBE);
			if (Cube *pc = dynamic_cast<Cube*>(pZoneActor->GetPolyhedron()))
			{
				ASSERT(pc->Get_coordinate_system() == PHAST_Transform::GRID);
				Prism p1(*pc);
				Prism p2(*pc);

				ASSERT(p1.perimeter.Get_coordinate_system() == PHAST_Transform::GRID);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::GRID);
				ASSERT(p2.perimeter.Get_coordinate_system() == PHAST_Transform::GRID);
				ASSERT(p2.perimeter.Get_user_coordinate_system() == PHAST_Transform::GRID);

				p2.Convert_coordinates(PHAST_Transform::MAP, &map2grid);
				p1.perimeter.Set_user_points(p2.perimeter.Get_points());
				p1.top.Set_user_points(p2.top.Get_points());
				p1.bottom.Set_user_points(p2.bottom.Get_points());

				p1.perimeter.Set_user_coordinate_system(PHAST_Transform::MAP);
				p1.Tidy();

				CAction *pAction = new CZoneSetPolyAction(
					this->m_pView,
					pZoneActor,
					&p1
					);
				if (pAction)
				{
					this->m_pView->GetDocument()->Execute(pAction);
				}
			}
			return;
		}
	}
	else
	{
		// Map => Grid
		// Box => Prism
		//
		if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
		{
			ASSERT(pZoneActor->GetPolyhedronType() == Polyhedron::CUBE);
			if (Cube *pc = dynamic_cast<Cube*>(pZoneActor->GetPolyhedron()))
			{
				ASSERT(pc->Get_coordinate_system() == PHAST_Transform::MAP);
				Prism p(*pc);
				ASSERT(p.perimeter.Get_coordinate_system() == PHAST_Transform::MAP);
				ASSERT(p.perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP);

				p.Convert_coordinates(PHAST_Transform::GRID, &map2grid);

				p.perimeter.Set_user_coordinate_system(PHAST_Transform::GRID);
				p.top.Set_user_coordinate_system(PHAST_Transform::GRID);
				p.bottom.Set_user_coordinate_system(PHAST_Transform::GRID);
				p.perimeter.Set_user_points(p.perimeter.Get_points());

				CAction *pAction = new CZoneSetPolyAction(
					this->m_pView,
					pZoneActor,
					&p
					);
				if (pAction)
				{
					this->m_pView->GetDocument()->Execute(pAction);
				}
			}
		}
	}
}

void CBoxPropertiesDialogBar::OnUseMapDrain(void)
{
	if (CDrainActor* pDrainActor = this->m_pProp3D ? CDrainActor::SafeDownCast(this->m_pProp3D) : NULL)
	{
		if (CWPhastDoc *pWPhastDoc = this->m_pView->GetDocument())
		{
			const CUnits& units = this->m_pView->GetDocument()->GetUnits();
			const CGridKeyword& gridKeyword = this->m_pView->GetDocument()->GetGridKeyword();
			PHAST_Transform map2grid(
				gridKeyword.m_grid_origin[0],
				gridKeyword.m_grid_origin[1],
				gridKeyword.m_grid_origin[2],
				gridKeyword.m_grid_angle,
				units.map_horizontal.input_to_si/units.horizontal.input_to_si,
				units.map_horizontal.input_to_si/units.horizontal.input_to_si,
				units.map_vertical.input_to_si/units.vertical.input_to_si
				);

			bool bChecked = (this->IsDlgButtonChecked(IDC_CHECK_USE_MAP2) == BST_CHECKED);
			CDrain drain = pDrainActor->GetData();
			if (bChecked)
			{
				drain.coordinate_system = PHAST_Transform::MAP;
			}
			else
			{
				drain.coordinate_system = PHAST_Transform::GRID;
			}

			std::list<CRiverPoint>::iterator it = drain.m_listPoints.begin();
			for (; it != drain.m_listPoints.end(); ++it)
			{
				Point p(
					it->x_user,
					it->y_user,
					0.0
					);

				if (bChecked)
				{
					map2grid.Inverse_transform(p);
					it->x_user = p.x();
					it->y_user = p.y();
				}
				else
				{
					map2grid.Transform(p);
					it->x_user = p.x();
					it->y_user = p.y();
				}
			}
			CAction *pAction = new CSetAction<CDrainActor, CDrain>(pDrainActor, drain, pWPhastDoc);
			pWPhastDoc->Execute(pAction);
		}
	}
}

void CBoxPropertiesDialogBar::OnUseMapRiver(void)
{
	if (CRiverActor* pRiverActor = this->m_pProp3D ? CRiverActor::SafeDownCast(this->m_pProp3D) : NULL)
	{
		if (CWPhastDoc *pWPhastDoc = this->m_pView->GetDocument())
		{
			const CUnits& units = this->m_pView->GetDocument()->GetUnits();
			const CGridKeyword& gridKeyword = this->m_pView->GetDocument()->GetGridKeyword();
			PHAST_Transform map2grid(
				gridKeyword.m_grid_origin[0],
				gridKeyword.m_grid_origin[1],
				gridKeyword.m_grid_origin[2],
				gridKeyword.m_grid_angle,
				units.map_horizontal.input_to_si/units.horizontal.input_to_si,
				units.map_horizontal.input_to_si/units.horizontal.input_to_si,
				units.map_vertical.input_to_si/units.vertical.input_to_si
				);

			bool bChecked = (this->IsDlgButtonChecked(IDC_CHECK_USE_MAP2) == BST_CHECKED);
			CRiver river = pRiverActor->GetData();
			if (bChecked)
			{
				river.coordinate_system = PHAST_Transform::MAP;
			}
			else
			{
				river.coordinate_system = PHAST_Transform::GRID;
			}

			std::list<CRiverPoint>::iterator it = river.m_listPoints.begin();
			for (; it != river.m_listPoints.end(); ++it)
			{
				Point p(
					it->x_user,
					it->y_user,
					0.0
					);

				if (bChecked)
				{
					map2grid.Inverse_transform(p);
					it->x_user = p.x();
					it->y_user = p.y();
				}
				else
				{
					map2grid.Transform(p);
					it->x_user = p.x();
					it->y_user = p.y();
				}
			}
			CAction *pAction = new CSetAction<CRiverActor, CRiver>(pRiverActor, river, pWPhastDoc);
			pWPhastDoc->Execute(pAction);
		}
	}
}
