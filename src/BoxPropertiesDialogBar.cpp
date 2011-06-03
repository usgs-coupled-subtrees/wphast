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

#include "PropertyTreeControlBar.h"

#include "PointSelectionObject.h"

#include "Units.h"
#include "Global.h"

static int PrismIDs[] = {
	IDC_RADIO_NONE,
	IDC_RADIO_CONSTANT,
	IDC_RADIO_ARCRASTER,
	IDC_RADIO_SHAPE,
	IDC_RADIO_XYZ,
	IDC_RADIO_POINTS,

	IDC_EDIT_CONSTANT,
	IDC_EDIT_ARCRASTER,
	IDC_EDIT_SHAPEFILE,
	IDC_EDIT_XYZ,

	IDC_BUTTON_ARCRASTER,
	IDC_BUTTON_SHAPE,
	IDC_BUTTON_XYZ,

	IDC_COMBO_SHAPE,

	IDC_BUTTON_ADD_POINT,
	IDC_BUTTON_DELETE_POINT,
	IDC_GRID_POINTS,

	//{{
	IDC_CHECK_USE_MAP2
	//}}
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
	ON_BN_CLICKED(IDC_RADIO_XYZ, OnBnClickedData_source)	
	ON_BN_CLICKED(IDC_RADIO_POINTS, OnBnClickedData_source)		
	
	// file browse buttons
	ON_BN_CLICKED(IDC_BUTTON_ARCRASTER, OnBnClickedArcraster)
	ON_BN_CLICKED(IDC_BUTTON_SHAPE, OnBnClickedShape)
	ON_BN_CLICKED(IDC_BUTTON_XYZ, OnBnClickedXYZ)

	// grid buttons
	ON_BN_CLICKED(IDC_BUTTON_ADD_POINT, OnBnClickedAddPoint)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_POINT, OnBnClickedDeletePoint)	

	ON_CBN_SELCHANGE(IDC_COMBO_SHAPE, OnCbnSelChangeShape)
	ON_EN_CHANGE(IDC_EDIT_CONSTANT, OnEnChangeConstant)
	ON_EN_KILLFOCUS(IDC_EDIT_CONSTANT, OnEnKillfocusConstant)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_POINTS, OnEndLabelEditPointsGrid)
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID_POINTS, OnBeginLabelEditPointsGrid)	

	// grid coords
	ON_BN_CLICKED(IDC_RADIO_CS_GRID, OnChangeCoorSys)
	ON_BN_CLICKED(IDC_RADIO_CS_MAP, OnChangeCoorSys)
END_MESSAGE_MAP()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
		if (CWnd *pWnd = this->GetFocus())
		{
			if (pWnd->GetParent() == &this->m_wndPointsGrid)
			{
				// req'd to end edit
				this->m_wndPointsGrid.SetFocus();
			}
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
					if (this->m_nType == CBoxPropertiesDialogBar::BP_RIVER || this->m_nType == CBoxPropertiesDialogBar::BP_DRAIN)
					{
						this->HideRiverControls();
					}
					else if (this->m_nType == CBoxPropertiesDialogBar::BP_WELL)
					{
						this->HideWellControls();
					}

					switch(pZoneActor->GetPolyhedronType())
					{
					case Polyhedron::WEDGE:
						if (this->m_nType == CBoxPropertiesDialogBar::BP_MIN_MAX)
						{
							this->HideZoneControls();
							this->HideCheckUseMap();
						}
						else if (this->m_nType == CBoxPropertiesDialogBar::BP_PRISM)
						{
							this->HidePrismControls();
						}

						if (this->m_nType != CBoxPropertiesDialogBar::BP_WEDGE)
						{
							this->m_nType = CBoxPropertiesDialogBar::BP_WEDGE;
							this->ShowWedgeControls();
						}
						break;
					case Polyhedron::CUBE:
					case Polyhedron::GRID_DOMAIN:
						if (this->m_nType == CBoxPropertiesDialogBar::BP_WEDGE)
						{
							this->HideWedgeControls();
						}
						else if (this->m_nType == CBoxPropertiesDialogBar::BP_PRISM)
						{
							this->HidePrismControls();
						}

						if (this->m_nType != CBoxPropertiesDialogBar::BP_MIN_MAX)
						{
							this->m_nType = CBoxPropertiesDialogBar::BP_MIN_MAX;
							this->ShowBoxControls();
							//this->ShowCheckUseMap();
						}
						break;
					case Polyhedron::PRISM:
						if (this->m_nType == CBoxPropertiesDialogBar::BP_MIN_MAX)
						{
							this->HideZoneControls();
							this->HideCheckUseMap();
						}
						else if (this->m_nType == CBoxPropertiesDialogBar::BP_WEDGE)
						{
							this->HideWedgeControls();
						}

						if (this->m_nType != CBoxPropertiesDialogBar::BP_PRISM)
						{
							this->m_nType = CBoxPropertiesDialogBar::BP_PRISM;
							this->ShowPrismControls();
						}
						break;
					default:
						ASSERT(FALSE);
					}
				}
				this->Set(pView, pZoneActor, pView->GetDocument()->GetUnits());

				if (this->m_nType == CBoxPropertiesDialogBar::BP_PRISM)
				{
					if (CPropertyTreeControlBar *pBar = dynamic_cast<CPropertyTreeControlBar*>(pSender))
					{
						if (CTreeCtrlEx* pTree = pBar->GetTreeCtrlEx())
						{
							CTreeCtrlNode node = pTree->GetSelectedItem();
							CString item = node.GetText().Left(3);
							if (item.CompareNoCase(_T("Per")) == 0)
							{
								this->m_wndTab.SetCurSel(1);
								this->m_bNeedsUpdate = false;
								this->m_nPrismPart = CBoxPropertiesDialogBar::PRISM_PERIMETER;
								this->UpdateData(FALSE);
							}
							else if (item.CompareNoCase(_T("Top")) == 0)
							{
								this->m_wndTab.SetCurSel(0);
								this->m_bNeedsUpdate = false;
								this->m_nPrismPart = CBoxPropertiesDialogBar::PRISM_TOP;
								this->UpdateData(FALSE);
							}
							else if (item.CompareNoCase(_T("Bot")) == 0)
							{
								this->m_wndTab.SetCurSel(2);
								this->m_bNeedsUpdate = false;
								this->m_nPrismPart = CBoxPropertiesDialogBar::PRISM_BOTTOM;
								this->UpdateData(FALSE);
							}
						}
					}
				}

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
					this->ShowCheckUseMap();
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
					this->HideCheckUseMap();
					this->HidePrismControls();
					this->ShowRiverControls();
					this->ShowApply();
				}

				this->Set(pView, pRiverActor, pView->GetDocument()->GetUnits());
				if (pHint)
				{
					if (pHint->IsKindOf(RUNTIME_CLASS(CPointSelectionObject)))
					{
						CPointSelectionObject *pt = (CPointSelectionObject*)pHint;
						int row = pt->GetPoint() + 1;
						this->m_wndRiverGrid.SetCurrentFocusCell(-1, -1);
						CCellRange range(row, this->m_wndRiverGrid.GetFixedColumnCount(), row, this->m_wndRiverGrid.GetColumnCount() - 1);
						this->m_wndRiverGrid.SetSelectedRange(range, TRUE);
					}
				}
				else
				{
					this->m_wndRiverGrid.SetCurrentFocusCell(-1, -1);
					this->m_wndRiverGrid.SetSelectedRange(-1, -1, -1, -1, TRUE);
				}
			}
			else if (CDrainActor* pDrainActor = CDrainActor::SafeDownCast(pProp))
			{
				this->m_nType = CBoxPropertiesDialogBar::BP_DRAIN;

				if (pObject != this->m_pProp3D)
				{
					this->HideWedgeControls();
					this->HideWellControls();
					this->HideCheckUseMap();
					this->HidePrismControls();
					this->ShowRiverControls();
					this->ShowApply();
				}

				this->Set(pView, pDrainActor, pView->GetDocument()->GetUnits());
				if (pHint)
				{
					if (pHint->IsKindOf(RUNTIME_CLASS(CPointSelectionObject)))
					{
						CPointSelectionObject *pt = (CPointSelectionObject*)pHint;
						int row = pt->GetPoint() + 1;
						this->m_wndRiverGrid.SetCurrentFocusCell(-1, -1);
						CCellRange range(row, this->m_wndRiverGrid.GetFixedColumnCount(), row, this->m_wndRiverGrid.GetColumnCount() - 1);
						this->m_wndRiverGrid.SetSelectedRange(range, TRUE);
					}
				}
				else
				{
					this->m_wndRiverGrid.SetCurrentFocusCell(-1, -1);
					this->m_wndRiverGrid.SetSelectedRange(-1, -1, -1, -1, TRUE);
				}
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
			this->HideCheckUseMap();
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
	case WPN_DOMAIN_CHANGED:
		TRACE("\t%s, WPN_DOMAIN_CHANGED\n", __FUNCTION__);
		this->Update(0, WPN_SELCHANGED, 0, this->m_pProp3D);
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
		case Polyhedron::GRID_DOMAIN:
			this->m_nType = CBoxPropertiesDialogBar::BP_MIN_MAX;
			if (Cube *cube = dynamic_cast<Cube*>(pZone->GetPolyhedron()))
			{
				this->m_xy_coordinate_system_user = cube->Get_user_coordinate_system();
			}
			if (pZone->GetDefault() || pZone->GetPolyhedronType() == Polyhedron::GRID_DOMAIN)
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
			this->m_wndRiverGrid.SetHighLight(GV_HIGHLIGHT_ALWAYS);
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
			//{{
			if (this->GetCheckedRadioButton(IDC_RADIO_CS_GRID, IDC_RADIO_CS_MAP) == IDC_RADIO_CS_MAP)
			{
				ASSERT(this->m_xy_coordinate_system_user == PHAST_Transform::MAP);
				this->m_xy_coordinate_system_user = PHAST_Transform::MAP;
			}
			else
			{
				ASSERT(this->m_xy_coordinate_system_user == PHAST_Transform::GRID);
				this->m_xy_coordinate_system_user = PHAST_Transform::GRID;
			}
			//}}
		}
		//{{
		else
		{
			switch (this->m_xy_coordinate_system_user)
			{
			case PHAST_Transform::MAP:
				this->CheckRadioButton(IDC_RADIO_CS_GRID, IDC_RADIO_CS_MAP, IDC_RADIO_CS_MAP);
				break;
			case PHAST_Transform::GRID:
				this->CheckRadioButton(IDC_RADIO_CS_GRID, IDC_RADIO_CS_MAP, IDC_RADIO_CS_GRID);
				break;
			default:
				ASSERT(FALSE);
				this->CheckRadioButton(IDC_RADIO_CS_GRID, IDC_RADIO_CS_MAP, IDC_RADIO_CS_GRID);
				break;
			}
		}
		//}}
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

		// coordinate system
		//
		if (pDX->m_bSaveAndValidate)
		{
			if (this->GetCheckedRadioButton(IDC_RADIO_CS_GRID, IDC_RADIO_CS_MAP) == IDC_RADIO_CS_MAP)
			{
				this->m_xy_coordinate_system_user = PHAST_Transform::MAP;
			}
			else
			{
				this->m_xy_coordinate_system_user = PHAST_Transform::GRID;
			}
		}
		else
		{			
			switch (this->m_xy_coordinate_system_user)
			{
			case PHAST_Transform::MAP:
				this->CheckRadioButton(IDC_RADIO_CS_GRID, IDC_RADIO_CS_MAP, IDC_RADIO_CS_MAP);
				break;
			case PHAST_Transform::GRID:
				this->CheckRadioButton(IDC_RADIO_CS_GRID, IDC_RADIO_CS_MAP, IDC_RADIO_CS_GRID);
				break;
			default:
				ASSERT(FALSE);
				this->CheckRadioButton(IDC_RADIO_CS_GRID, IDC_RADIO_CS_MAP, IDC_RADIO_CS_GRID);
				break;
			}
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
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_SHAPE))
		{
			pWnd->EnableWindow(bEnable);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_XYZ))
		{
			pWnd->EnableWindow(bEnable);
		}
		if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_POINTS))
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
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_XYZ))
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
		if (CWnd *pWnd = this->GetDlgItem(IDC_BUTTON_XYZ))
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
		this->HideCheckUseMap();
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
				if (this->GetCheckedRadioButton(IDC_RADIO_NONE, IDC_RADIO_POINTS) != IDC_RADIO_POINTS)
				{
					if (::IsEditCtrl(pMsg->hwnd))
					{
						CWnd *pWnd = CWnd::FromHandle(pMsg->hwnd);
						int nID = pWnd->GetDlgCtrlID();
						{
							ASSERT(nID == IDC_EDIT_CONSTANT);
							double d;
							if (::IsValidFloatFormat(this, pMsg->hwnd, d))
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
					if (::IsValidFloatFormat(this, pMsg->hwnd, d))
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
// COMMENT: {7/15/2009 10:57:53 PM}	IDC_CHECK_USE_MAP,
	IDC_GB_COOR_SYS,
    IDC_RADIO_CS_GRID,
	IDC_RADIO_CS_MAP
};

void CBoxPropertiesDialogBar::ShowBoxControls()
{
	TRACE("%s, in\n", __FUNCTION__);

	CRect rect;
	this->GetClientRect(&rect);
	this->SizeZoneControls(rect.Width(), rect.Height());

	for (int i = 0; i < sizeof(ZoneIDs)/sizeof(ZoneIDs[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(ZoneIDs[i]))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
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

void CBoxPropertiesDialogBar::SizeZoneControls(int cx, int cy)
{
	CRect rcMin(54, 24, 72, 37);
	CRect rcMax(134, 24, 155, 37);

	CRect rcXmin(29, 47, 98, 67);
	CRect rcYmin(29, 80, 98, 100);
	CRect rcZmin(29, 114, 98, 134);

	CRect rcXmax(110, 47, 179, 67);
	CRect rcYmax(110, 80, 179, 100);
	CRect rcZmax(110, 114, 179, 134);

	CRect rcXU(186, 50, 216, 63);
	CRect rcYU(186, 83, 216, 96);
	CRect rcZU(186, 117, 216, 130);

	CRect rcCS;
	if (CWnd *pWnd = this->GetDlgItem(IDC_GB_COOR_SYS))
	{
		pWnd->GetWindowRect(&rcCS);
		this->ScreenToClient(&rcCS);
	}
	CRect rcG;
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CS_GRID))
	{
		pWnd->GetWindowRect(&rcG);
		this->ScreenToClient(&rcG);
	}
	CRect rcM;
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CS_MAP))
	{
		pWnd->GetWindowRect(&rcM);
		this->ScreenToClient(&rcM);
	}

	// row headings
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_X))
	{
		pWnd->MoveWindow(11, 50, 12, 13, TRUE);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_Y))
	{
		pWnd->MoveWindow(11, 83, 12, 13, TRUE);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_Z))
	{
		pWnd->MoveWindow(11, 117, 12, 13, TRUE);
	}

	// column headings
	const int mpad = 15;
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_MIN))
	{
		pWnd->MoveWindow(rcXmin.left + (cx/2 - mpad - rcXmin.left)/2 - 8, rcMin.top, rcMin.Width(), rcMin.Height());
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_MAX))
	{
		pWnd->MoveWindow(cx/2 + (cx/2 - mpad - rcXmin.left)/2 - 13, rcMax.top, rcMax.Width(), rcMax.Height());
	}

	// min
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_XMIN))
	{
		pWnd->MoveWindow(rcXmin.left, rcXmin.top, cx/2 - mpad - rcXmin.left, rcXmin.Height());
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_YMIN))
	{
		pWnd->MoveWindow(rcYmin.left, rcYmin.top, cx/2 - mpad - rcYmin.left, rcYmin.Height());
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_ZMIN))
	{
		pWnd->MoveWindow(rcZmin.left, rcZmin.top, cx/2 - mpad - rcZmin.left, rcZmin.Height());
	}

	// max
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_XMAX))
	{
		pWnd->MoveWindow(cx/2 - 3, rcXmax.top, cx/2 - mpad - rcXmin.left, rcXmax.Height());
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_YMAX))
	{
		pWnd->MoveWindow(cx/2 - 3, rcYmax.top, cx/2 - mpad - rcYmin.left, rcYmax.Height());
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_ZMAX))
	{
		pWnd->MoveWindow(cx/2 - 3, rcZmax.top, cx/2 - mpad - rcZmin.left, rcZmax.Height());
	}

	// units
	const int upad = 10;
	if (CWnd *pWnd = this->GetDlgItem(IDC_X_UNITS_STATIC2))
	{
		pWnd->MoveWindow(cx - rcXU.Width() - upad, rcXU.top, rcXU.Width(), rcXU.Height());
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_Y_UNITS_STATIC2))
	{
		pWnd->MoveWindow(cx - rcYU.Width() - upad, rcYU.top, rcYU.Width(), rcYU.Height());
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_Z_UNITS_STATIC2))
	{
		pWnd->MoveWindow(cx - rcZU.Width() - upad, rcZU.top, rcZU.Width(), rcZU.Height());
	}

	// use map coordinates
	CRect rcMC;
	if (CWnd *pWnd = this->GetDlgItem(IDC_CHECK_USE_MAP))
	{
		pWnd->GetWindowRect(&rcMC);
		this->ScreenToClient(&rcMC);
	}

	// use map coordinates
	if (CWnd *pWnd = this->GetDlgItem(IDC_CHECK_USE_MAP))
	{
		pWnd->MoveWindow((cx - rcMC.Width()) / 2, rcMC.top, rcMC.Width(), rcMC.Height());
	}

	// coordinate system groupbox
	const int pad = 9;
	if (CWnd *pWnd = this->GetDlgItem(IDC_GB_COOR_SYS))
	{
		pWnd->MoveWindow(rcCS.left, cy - rcCS.Height() - 5, cx - 2 * rcCS.left, rcCS.Height());
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CS_GRID))
	{
		pWnd->MoveWindow(50, cy - rcG.Height() - pad, rcG.Width(), rcG.Height());
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CS_MAP))
	{
		pWnd->MoveWindow(cx - rcM.Width() - 50, cy - rcM.Height() - pad, rcM.Width(), rcM.Height());
	}
}


//{{{7/15/2009 10:54:00 PM}
void CBoxPropertiesDialogBar::ShowCheckUseMap()
{
	if (CWnd *pWnd = this->GetDlgItem(IDC_CHECK_USE_MAP))
	{
		pWnd->ShowWindow(SW_SHOW);
	}
}

void CBoxPropertiesDialogBar::HideCheckUseMap()
{
	if (CWnd *pWnd = this->GetDlgItem(IDC_CHECK_USE_MAP))
	{
		pWnd->ShowWindow(SW_HIDE);
	}
}
//}}{7/15/2009 10:54:00 PM}


static int WellIDs[] = {
//	IDC_STATIC_LOC,
	IDC_STATIC_WELL_X,
	IDC_STATIC_WELL_Y,
	IDC_EDIT_WELL_X,
	IDC_EDIT_WELL_Y,
	IDC_X_UNITS_STATIC,
	IDC_Y_UNITS_STATIC,
// COMMENT: {7/15/2009 10:54:00 PM}	IDC_CHECK_USE_MAP,
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

	CRect rect;
	this->GetClientRect(&rect);
	this->SizeRiverControls(rect.Width(), rect.Height());
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

void CBoxPropertiesDialogBar::SizeRiverControls(int cx, int cy)
{
// COMMENT: {5/17/2011 3:28:12 PM}	CRect rcX;
// COMMENT: {5/17/2011 3:28:12 PM}	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_XYZ))
// COMMENT: {5/17/2011 3:28:12 PM}	{
// COMMENT: {5/17/2011 3:28:12 PM}		pWnd->GetWindowRect(&rcX);
// COMMENT: {5/17/2011 3:28:12 PM}		this->ScreenToClient(&rcX);
// COMMENT: {5/17/2011 3:28:12 PM}	}
// COMMENT: {5/17/2011 3:28:12 PM}
// COMMENT: {5/17/2011 3:28:12 PM}	CRect rcP;
// COMMENT: {5/17/2011 3:28:12 PM}	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_POINTS))
// COMMENT: {5/17/2011 3:28:12 PM}	{
// COMMENT: {5/17/2011 3:28:12 PM}		pWnd->GetWindowRect(&rcP);
// COMMENT: {5/17/2011 3:28:12 PM}		this->ScreenToClient(&rcP);
// COMMENT: {5/17/2011 3:28:12 PM}	}

	CRect rc;
	this->m_wndRiverGrid.GetWindowRect(&rc);
	this->ScreenToClient(&rc);

	CRect rcMC;
	if (CWnd *pWnd = this->GetDlgItem(IDC_CHECK_USE_MAP2))
	{
		pWnd->GetWindowRect(&rcMC);
		this->ScreenToClient(&rcMC);
	}
	this->m_wndRiverGrid.GetWindowRect(&rc);
	this->ScreenToClient(&rc);

	const int x = rc.left;
	int y = rc.top;
	this->m_wndRiverGrid.MoveWindow(x, y, cx-2*x, cy - rcMC.Height() - 10 - x /*rc.Height()*//*cy-y-x*/);

	if (CWnd *pWnd = this->GetDlgItem(IDC_CHECK_USE_MAP2))
	{
		pWnd->MoveWindow((cx - rcMC.Width()) / 2/*rcMC.left*/, cy - rcMC.Height() - 5, rcMC.Width(), rcMC.Height());
	}
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
	IDC_GB_COOR_SYS,
	IDC_RADIO_CS_GRID,
	IDC_RADIO_CS_MAP
};

void CBoxPropertiesDialogBar::ShowWedgeControls()
{
	TRACE("%s, in\n", __FUNCTION__);

	CRect rect;
	this->GetClientRect(&rect);
	this->SizeWedgeControls(rect.Width(), rect.Height());



// COMMENT: {5/19/2011 11:08:20 PM}	const int D1 = 14;
// COMMENT: {5/19/2011 11:08:20 PM}	const int D2 = 22;
// COMMENT: {5/19/2011 11:08:20 PM}	const int D3 = 30;
// COMMENT: {5/19/2011 11:08:20 PM}	const int D4 = 39;
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_MIN))
	{
// COMMENT: {5/19/2011 10:20:46 PM}		pWnd->MoveWindow(54, 24-D1, 18, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_MAX))
	{
// COMMENT: {5/19/2011 10:20:50 PM}		pWnd->MoveWindow(134, 24-D1, 21, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_X))
	{
// COMMENT: {5/19/2011 10:20:52 PM}		pWnd->MoveWindow(11, 50-D2, 12, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_Y))
	{
// COMMENT: {5/19/2011 10:20:56 PM}		pWnd->MoveWindow(11, 83-D3, 12, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_STATIC_Z))
	{
// COMMENT: {5/19/2011 10:21:00 PM}		pWnd->MoveWindow(11, 117-D4, 12, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_XMIN))
	{
// COMMENT: {5/19/2011 10:27:56 PM}		pWnd->MoveWindow(29, 47-D2, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_YMIN))
	{
// COMMENT: {5/19/2011 10:28:00 PM}		pWnd->MoveWindow(29, 80-D3, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_ZMIN))
	{
// COMMENT: {5/19/2011 10:28:03 PM}		pWnd->MoveWindow(29, 114-D4, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_XMAX))
	{
// COMMENT: {5/19/2011 10:28:06 PM}		pWnd->MoveWindow(110, 47-D2, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_YMAX))
	{
// COMMENT: {5/19/2011 10:28:09 PM}		pWnd->MoveWindow(110, 80-D3, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_ZMAX))
	{
// COMMENT: {5/19/2011 10:28:11 PM}		pWnd->MoveWindow(110, 114-D4, 69, 20, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_X_UNITS_STATIC2))
	{
// COMMENT: {5/19/2011 10:28:13 PM}		pWnd->MoveWindow(186, 50-D2, 30, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_Y_UNITS_STATIC2))
	{
// COMMENT: {5/19/2011 10:28:16 PM}		pWnd->MoveWindow(186, 83-D3, 30, 13, TRUE);
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_Z_UNITS_STATIC2))
	{
// COMMENT: {5/19/2011 10:28:18 PM}		pWnd->MoveWindow(186, 117-D4, 30, 13, TRUE);
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

	if (CWnd *pWnd = this->GetDlgItem(IDC_GB_COOR_SYS))
	{
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CS_GRID))
	{
		pWnd->ShowWindow(SW_SHOW);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CS_MAP))
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

void CBoxPropertiesDialogBar::SizeWedgeControls(int cx, int cy)
{
	this->SizeZoneControls(cx, cy);

	int bp = 15;

	CRect rcZ;
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_ZMIN))
	{
		pWnd->GetWindowRect(&rcZ);
		this->ScreenToClient(&rcZ);
	}

	//
	// Axis groupbox
	//

	CRect rcA;
	if (CWnd *pWnd = this->GetDlgItem(IDC_GB_AXIS))
	{
		pWnd->GetWindowRect(&rcA);
		this->ScreenToClient(&rcA);
		int w = .5 * (cx - 3 * bp);
		pWnd->MoveWindow(bp, rcZ.bottom + 10, w, rcA.Height(), TRUE);
		pWnd->GetWindowRect(&rcA);
		this->ScreenToClient(&rcA);
	}

	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_X))
	{
		CRect rc;
		pWnd->GetWindowRect(&rc);
		this->ScreenToClient(&rc);
		int hpad = (rcA.Width() - rc.Width()) / 2;
		int vpad = (rcA.Height() - 3 * rc.Height()) / 5;
		int x = rcA.left + hpad;
		int y = rcA.top + vpad + .5 * rc.Height();
		pWnd->MoveWindow(x, y, rc.Width(), rc.Height(), TRUE);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_Y))
	{
		CRect rc;
		pWnd->GetWindowRect(&rc);
		this->ScreenToClient(&rc);
		int hpad = (rcA.Width() - rc.Width()) / 2;
		int vpad = (rcA.Height() - 3 * rc.Height()) / 5;
		int x = rcA.left + hpad;
		int y = rcA.top + 2*vpad + 1.5 * rc.Height();
		pWnd->MoveWindow(x, y, rc.Width(), rc.Height(), TRUE);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_Z))
	{
		CRect rc;
		pWnd->GetWindowRect(&rc);
		this->ScreenToClient(&rc);
		int hpad = (rcA.Width() - rc.Width()) / 2;
		int vpad = (rcA.Height() - 3 * rc.Height()) / 5;
		int x = rcA.left + hpad;
		int y = rcA.top + 3*vpad + 2.5 * rc.Height();
		pWnd->MoveWindow(x, y, rc.Width(), rc.Height(), TRUE);
	}

	//
	// Corner groupbox
	//

	CRect rcC;
	if (CWnd *pWnd = this->GetDlgItem(IDC_GB_CORNER))
	{
		pWnd->GetWindowRect(&rcC);
		this->ScreenToClient(&rcC);
		afxDump << "Corner " << rcC << "\n";
		int w = .5 * (cx - 3 * bp);
		pWnd->MoveWindow(rcA.right + bp, rcZ.bottom + 10, w, rcC.Height(), TRUE);
		pWnd->GetWindowRect(&rcC);
		this->ScreenToClient(&rcC);
	}

	int xpad = 3;
	int ypad = 10;

	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_4))
	{
		CRect rc;
		pWnd->GetWindowRect(&rc);
		this->ScreenToClient(&rc);
		int x = rcC.left + rcC.Width() / 3 - .5 * rc.Width();
		int y = rcC.top + rcC.Height() / 3 - .5 * rc.Width();
		pWnd->MoveWindow(x+xpad, y+ypad, rc.Width(), rc.Height(), TRUE);
	}

	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_3))
	{
		CRect rc;
		pWnd->GetWindowRect(&rc);
		this->ScreenToClient(&rc);
		int x = rcC.left + 2 * (rcC.Width() / 3) - .5 * rc.Width();
		int y = rcC.top + rcC.Height() / 3 - .5 * rc.Width();
		pWnd->MoveWindow(x+xpad+6, y+ypad, rc.Width(), rc.Height(), TRUE);
	}

	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_1))
	{
		CRect rc;
		pWnd->GetWindowRect(&rc);
		this->ScreenToClient(&rc);
		int x = rcC.left + rcC.Width() / 3 - .5 * rc.Width();
		int y = rcC.top + 2 * rcC.Height() / 3 - .5 * rc.Width();
		pWnd->MoveWindow(x+xpad, y+ypad, rc.Width(), rc.Height(), TRUE);
	}

	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_2))
	{
		CRect rc;
		pWnd->GetWindowRect(&rc);
		this->ScreenToClient(&rc);
		int x = rcC.left + 2 * (rcC.Width() / 3) - .5 * rc.Width();
		int y = rcC.top + 2 * rcC.Height() / 3 - .5 * rc.Width();
		pWnd->MoveWindow(x+xpad+6, y+ypad, rc.Width(), rc.Height(), TRUE);
	}

	//
	// Coordinate system groupbox
	//

	CRect rcCS;
	if (CWnd *pWnd = this->GetDlgItem(IDC_GB_COOR_SYS))
	{
		pWnd->GetWindowRect(&rcCS);
		this->ScreenToClient(&rcCS);
	}
	CRect rcG;
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CS_GRID))
	{
		pWnd->GetWindowRect(&rcG);
		this->ScreenToClient(&rcG);
	}
	CRect rcM;
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CS_MAP))
	{
		pWnd->GetWindowRect(&rcM);
		this->ScreenToClient(&rcM);
	}

	const int pad = 4;
	if (CWnd *pWnd = this->GetDlgItem(IDC_GB_COOR_SYS))
	{
		pWnd->MoveWindow(bp, rcA.bottom + bp, cx - 2 * bp, rcCS.Height());
		pWnd->GetWindowRect(&rcCS);
		this->ScreenToClient(&rcCS);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CS_GRID))
	{
		pWnd->MoveWindow(50, rcCS.CenterPoint().y + pad - .5 * rcG.Height(), rcG.Width(), rcG.Height());
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CS_MAP))
	{
		pWnd->MoveWindow(cx - rcM.Width() - 50, rcCS.CenterPoint().y + pad - .5 * rcG.Height(), rcM.Width(), rcM.Height());
	}
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
	if (::IsValidFloatFormat(this, IDC_GRID_RIVER, pnmgv->iRow, pnmgv->iColumn, d))
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
	BOOL b = ::IsValidFloatFormat(pWnd, hWndCtrl, d);
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
		if (::IsValidFloatFormat(this, nID, d))
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
					if (!(::IsValidFloatFormat(this, aIDs[i], d) && bounds[i] == (float)d))
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
					if (!(::IsValidFloatFormat(this, IDC_EDIT_WELL_X, d) && pWell->GetWell().x_user == (float)d))
					{
						bNeedsUpdate = true;
						break;
					}
					if (!(::IsValidFloatFormat(this, IDC_EDIT_WELL_Y, d) && pWell->GetWell().y_user == (float)d))
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
					if (!(::IsValidFloatFormat(this, IDC_GRID_RIVER, r, 0, d) && iter->x_user == d))
					{
						bNeedsUpdate = true;
						break;
					}
					if (!(::IsValidFloatFormat(this, IDC_GRID_RIVER, r, 1, d) && iter->y_user == d))
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
					if (!(::IsValidFloatFormat(this, IDC_GRID_RIVER, r, 0, d) && iter->x_user == d))
					{
						bNeedsUpdate = true;
						break;
					}
					if (!(::IsValidFloatFormat(this, IDC_GRID_RIVER, r, 1, d) && iter->y_user == d))
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
					if (!(::IsValidFloatFormat(this, aIDs[i], d) && bounds[i] == (float)d))
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
		this->UpdateBackgroundBrush();    // reqd to redraw radios
	}

	if (this->m_nType == CBoxPropertiesDialogBar::BP_RIVER)
	{
		this->SizeRiverControls(cx, cy);
	}
	if (this->m_nType == CBoxPropertiesDialogBar::BP_PRISM)
	{
		this->SizePrismControls(cx, cy);
	}
	if (this->m_nType == CBoxPropertiesDialogBar::BP_MIN_MAX)
	{
		this->SizeZoneControls(cx, cy);
	}
	if (this->m_nType == CBoxPropertiesDialogBar::BP_WEDGE)
	{
		this->SizeWedgeControls(cx, cy);
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
		this->m_hBrush = ::CreatePatternBrush(hBmp);

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

#if defined(_DEBUG)
	afxDump << "cx = " << cx << " cy = " << cy << "\n";
#endif

	CRect rcRC;
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_CONSTANT))
	{
		pWnd->GetWindowRect(&rcRC);
		this->ScreenToClient(&rcRC);
#if defined(_DEBUG)
		afxDump << "Radio Constant " << rcRC << "\n";
#endif
	}
	CRect rcC;
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_CONSTANT))
	{
		pWnd->GetWindowRect(&rcC);
		this->ScreenToClient(&rcC);
#if defined(_DEBUG)
		afxDump << "Edit Constant " << rcC << "\n";
#endif
	}
	CRect rcES;
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_SHAPEFILE))
	{
		pWnd->GetWindowRect(&rcES);
		this->ScreenToClient(&rcES);
	}

	CRect rcX;
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_XYZ))
	{
		pWnd->GetWindowRect(&rcX);
		this->ScreenToClient(&rcX);
	}

	CRect rcP;
	if (CWnd *pWnd = this->GetDlgItem(IDC_RADIO_POINTS))
	{
		pWnd->GetWindowRect(&rcP);
		this->ScreenToClient(&rcP);
	}

	// use map coordinates
	CRect rcMC;
	if (CWnd *pWnd = this->GetDlgItem(IDC_CHECK_USE_MAP2))
	{
		pWnd->GetWindowRect(&rcMC);
		this->ScreenToClient(&rcMC);
	}

	const int x = 5;
	//int y = rcP.top + rcP.top - rcX.top;
	int y = rcP.top + rcP.top - rcX.top + rcMC.Height();
	this->m_wndPointsGrid.MoveWindow(x, y, cx-2*x, cy-y-x);

	const int border = 3;

	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_CONSTANT))
	{
		pWnd->MoveWindow(rcC.left, rcC.top, cx - rcC.left - 2 * border, rcC.Height());
	}


	CRect rcBS;
	if (CWnd *pWnd = this->GetDlgItem(IDC_BUTTON_SHAPE))
	{
		pWnd->GetWindowRect(&rcBS);
		this->ScreenToClient(&rcBS);
	}

	CRect rcCS;
	if (CWnd *pWnd = this->GetDlgItem(IDC_COMBO_SHAPE))
	{
		pWnd->GetWindowRect(&rcCS);
		this->ScreenToClient(&rcCS);
	}

	// shape
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_SHAPEFILE))
	{
		rcES.InflateRect(0, 0, cx - rcBS.Width() - rcCS.Width() - rcES.right - 4 * border, 0);
		pWnd->MoveWindow(rcES);
		::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_SHAPEFILE, this->m_sShapefile[this->m_nPrismPart]);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_BUTTON_SHAPE))
	{
		pWnd->MoveWindow(rcES.right + border, rcBS.top, rcBS.Width(), rcBS.Height());
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_COMBO_SHAPE))
	{
		pWnd->MoveWindow(rcES.right + border + rcBS.Width() + border, rcCS.top, rcCS.Width(), rcCS.Height());
	}

	// arcraster
	CRect rcBA;
	if (CWnd *pWnd = this->GetDlgItem(IDC_BUTTON_ARCRASTER))
	{
		pWnd->GetWindowRect(&rcBA);
		this->ScreenToClient(&rcBA);
	}
	CRect rcEA;
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_ARCRASTER))
	{
		pWnd->GetWindowRect(&rcEA);
		this->ScreenToClient(&rcEA);

		rcEA.InflateRect(0, 0, cx - rcBA.Width() - rcEA.right - 3 * border, 0);
		pWnd->MoveWindow(rcEA);
		::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_ARCRASTER, this->m_sArcraster[this->m_nPrismPart]);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_BUTTON_ARCRASTER))
	{
		pWnd->MoveWindow(rcEA.right + border, rcBA.top, rcBA.Width(), rcBA.Height());
	}

	// XYZ
	CRect rcBX;
	if (CWnd *pWnd = this->GetDlgItem(IDC_BUTTON_XYZ))
	{
		pWnd->GetWindowRect(&rcBX);
		this->ScreenToClient(&rcBX);
	}
	CRect rcEX;
	if (CWnd *pWnd = this->GetDlgItem(IDC_EDIT_XYZ))
	{
		pWnd->GetWindowRect(&rcEX);
		this->ScreenToClient(&rcEX);

		rcEX.InflateRect(0, 0, cx - rcBX.Width() - rcEX.right - 3 * border, 0);
		pWnd->MoveWindow(rcEX);
		::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_XYZ, this->m_sXYZ[this->m_nPrismPart]);
	}
	if (CWnd *pWnd = this->GetDlgItem(IDC_BUTTON_XYZ))
	{
		pWnd->MoveWindow(rcEX.right + border, rcBX.top, rcBX.Width(), rcBX.Height());
	}

	// use map coordinates
	if (CWnd *pWnd = this->GetDlgItem(IDC_CHECK_USE_MAP2))
	{
		//pWnd->MoveWindow(rcP.right + border, rcBX.top, rcBX.Width(), rcBX.Height());
		pWnd->MoveWindow((cx - rcMC.Width()) / 2, rcP.bottom + 5, rcMC.Width(), rcMC.Height());
	}
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

			// XYZ
			if (this->m_pds[i]->Get_source_type() == Data_source::XYZ)
			{
				this->m_sXYZ[i] = this->m_pds[i]->Get_file_name().c_str();
			}
			else
			{
				if ((this->m_pProp3D != pZoneActor) || bForceUpdate)
				{
					this->m_sXYZ[i] = "";
				}
			}
		}

		// points
		this->m_listPoint[PRISM_PERIMETER].clear();
		this->m_listPoint[PRISM_BOTTOM].clear();
		this->m_listPoint[PRISM_TOP].clear();

#ifdef SAME_POINTS_AS_PERIMETER
		if (this->m_pds[PRISM_PERIMETER]->Get_source_type() == Data_source::POINTS)
		{
			ASSERT(this->m_listPoint[PRISM_PERIMETER].size() == 0);
			ASSERT(this->m_listPoint[PRISM_BOTTOM].size() == 0);
			ASSERT(this->m_listPoint[PRISM_TOP].size() == 0);

			Data_source::DATA_SOURCE_TYPE n_bot = this->m_pds[PRISM_BOTTOM]->Get_source_type();
			Data_source::DATA_SOURCE_TYPE n_top = this->m_pds[PRISM_TOP]->Get_source_type();

			zone *zb = this->m_pds[PRISM_BOTTOM]->Get_bounding_box();
			ASSERT(zb->zone_defined);

			zone *zt = this->m_pds[PRISM_TOP]->Get_bounding_box();
			ASSERT(zt->zone_defined);

			ASSERT(this->m_listPoint[PRISM_PERIMETER].size() == 0);
			std::vector<Point>::iterator it = this->m_pds[PRISM_PERIMETER]->Get_points().begin();
			for (; it != this->m_pds[PRISM_PERIMETER]->Get_points().end(); ++it)
			{
				// perimeter
				Point p(*it);
				this->m_listPoint[PRISM_PERIMETER].push_back(p);

				// bottom
				if (n_bot == Data_source::NONE || n_bot == Data_source::CONSTANT || n_bot == Data_source::ARCRASTER)
				{
					this->m_listPoint[PRISM_BOTTOM].push_back(Point(it->x(), it->y(), defzone.z1));
				}
				else if (n_bot == Data_source::SHAPE)
				{
					this->m_listPoint[PRISM_BOTTOM].push_back(Point(it->x(), it->y(), zb->z1));
				}

				// top
				if (n_top == Data_source::NONE || n_top == Data_source::CONSTANT || n_top == Data_source::ARCRASTER)
				{
					this->m_listPoint[PRISM_TOP].push_back(Point(it->x(), it->y(), defzone.z2));
				}
				else if (n_bot == Data_source::SHAPE)
				{
					this->m_listPoint[PRISM_TOP].push_back(Point(it->x(), it->y(), zt->z1));
				}
			}

			// bottom
			if (n_bot == Data_source::POINTS)
			{
				ASSERT(this->m_listPoint[PRISM_BOTTOM].size() == 0);
				std::vector<Point>::iterator it = this->m_pds[PRISM_BOTTOM]->Get_points().begin();
				for (; it != this->m_pds[PRISM_BOTTOM]->Get_points().end(); ++it)
				{
					Point p(*it);
					this->m_listPoint[PRISM_BOTTOM].push_back(p);
				}
			}

			// top
			if (n_top == Data_source::POINTS)
			{
				ASSERT(this->m_listPoint[PRISM_TOP].size() == 0);
				std::vector<Point>::iterator it = this->m_pds[PRISM_TOP]->Get_points().begin();
				for (; it != this->m_pds[PRISM_TOP]->Get_points().end(); ++it)
				{
					Point p(*it);
					this->m_listPoint[PRISM_TOP].push_back(p);
				}
			}
		}
		else
#endif SAME_POINTS_AS_PERIMETER
		{
			for (int i = 0; i < 3; ++i)
			{
				if (this->m_pds[i]->Get_source_type() == Data_source::POINTS)
				{
					std::vector<Point>::iterator it = this->m_pds[i]->Get_points().begin();
					for (; it != this->m_pds[i]->Get_points().end(); ++it)
					{
						Point p(*it);
						this->m_listPoint[i].push_back(p);
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
						this->m_listPoint[i].push_back(Point(z->x1, z->y1, defzone.z2));
						this->m_listPoint[i].push_back(Point(z->x2, z->y1, defzone.z2));
						this->m_listPoint[i].push_back(Point(z->x2, z->y2, defzone.z2));
						this->m_listPoint[i].push_back(Point(z->x1, z->y2, defzone.z2));
					}
					else if (i == PRISM_PERIMETER)
					{
						this->m_listPoint[i].push_back(Point(defzone.x1, defzone.y1, 0.0));
						this->m_listPoint[i].push_back(Point(defzone.x2, defzone.y1, 0.0));
						this->m_listPoint[i].push_back(Point(defzone.x2, defzone.y2, 0.0));
						this->m_listPoint[i].push_back(Point(defzone.x1, defzone.y2, 0.0));
					}
					else if (i == PRISM_BOTTOM)
					{
						this->m_listPoint[i].push_back(Point(z->x1, z->y1, defzone.z1));
						this->m_listPoint[i].push_back(Point(z->x2, z->y1, defzone.z1));
						this->m_listPoint[i].push_back(Point(z->x2, z->y2, defzone.z1));
						this->m_listPoint[i].push_back(Point(z->x1, z->y2, defzone.z1));
					}
				}
				else if (this->m_pds[i]->Get_source_type() == Data_source::SHAPE)
				{
					zone *z = this->m_pds[i]->Get_bounding_box();
					ASSERT(z->zone_defined);
					if (i == PRISM_TOP)
					{
						this->m_listPoint[i].push_back(Point(z->x1, z->y1, z->z2));
						this->m_listPoint[i].push_back(Point(z->x2, z->y1, z->z2));
						this->m_listPoint[i].push_back(Point(z->x2, z->y2, z->z2));
						this->m_listPoint[i].push_back(Point(z->x1, z->y2, z->z2));
					}
					else if (i == PRISM_PERIMETER)
					{
						this->m_listPoint[i].push_back(Point(defzone.x1, defzone.y1, 0.0));
						this->m_listPoint[i].push_back(Point(defzone.x2, defzone.y1, 0.0));
						this->m_listPoint[i].push_back(Point(defzone.x2, defzone.y2, 0.0));
						this->m_listPoint[i].push_back(Point(defzone.x1, defzone.y2, 0.0));
					}
					else if (i == PRISM_BOTTOM)
					{
						this->m_listPoint[i].push_back(Point(z->x1, z->y1, z->z1));
						this->m_listPoint[i].push_back(Point(z->x2, z->y1, z->z1));
						this->m_listPoint[i].push_back(Point(z->x2, z->y2, z->z1));
						this->m_listPoint[i].push_back(Point(z->x1, z->y2, z->z1));
					}
				}
			}
		}

	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnBnClickedData_source()
{
	TRACE("%s, in\n", __FUNCTION__);
	
	this->UpdatePrismControls();
	double d;
	switch (this->GetCheckedRadioButton(IDC_RADIO_NONE, IDC_RADIO_POINTS))
	{
	case IDC_RADIO_NONE:
		this->OnApply();
		break;
	case IDC_RADIO_CONSTANT:
		if (::IsValidFloatFormat(this, IDC_EDIT_CONSTANT, d))
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
		{
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
	case IDC_RADIO_XYZ:
		if (CGlobal::IsValidXYZFile(this->m_sXYZ[this->m_nPrismPart]))
		{
			this->OnApply();
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
	if (pDX->m_bSaveAndValidate)
	{
		switch (this->GetCheckedRadioButton(IDC_RADIO_NONE, IDC_RADIO_POINTS))
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
		case IDC_RADIO_SHAPE:
			this->m_nDataSourceType = Data_source::SHAPE;
			break;
		case IDC_RADIO_XYZ:
			this->m_nDataSourceType = Data_source::XYZ;
			break;
		case IDC_RADIO_POINTS:
			this->m_nDataSourceType = Data_source::POINTS;
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
				::AfxMessageBox(_T("Please select an ARCRASTER file."));
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
				::AfxMessageBox(_T("Please select a shapefile."));
				pDX->Fail();
			}

			CGlobal::IsValidShapefile(this->m_sShapefile[this->m_nPrismPart], pDX);

			if (this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER)
			{
				this->m_nShapeAttribute[this->m_nPrismPart] = this->m_wndShapeCombo.GetCurSel() - 1;
				if (this->m_nShapeAttribute[this->m_nPrismPart] == -1)
				{
					::AfxMessageBox(_T("Please select a shapefile attribute."));
					pDX->PrepareCtrl(IDC_COMBO_SHAPE);
					pDX->Fail();
				}
			}
		}

		// XYZ
		if (this->m_nDataSourceType == Data_source::XYZ)
		{
			CString str;
			DDX_Text(pDX, IDC_EDIT_XYZ, str);
			if (str.IsEmpty())
			{
				::AfxMessageBox(_T("Please select an XYZ file."));
				pDX->Fail();
			}
			ASSERT(CGlobal::IsValidXYZFile(this->m_sXYZ[this->m_nPrismPart]));
		}

		// points
		if (this->m_nDataSourceType == Data_source::POINTS)
		{
			std::list<Point> new_points;
			try // try block probably not needed
			{
				double x, y, z;
				z = 0;
				for (int row = 1; row < this->m_wndPointsGrid.GetRowCount(); ++row)
				{
					DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 0, x);
					DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 1, y);
					if (this->m_nPrismPart == this->PRISM_PERIMETER)
					{
						new_points.push_back(Point(x, y, 0.0));
					}
					else
					{
						DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 2, z);
						new_points.push_back(Point(x, y, z));
					}
				}
			}
			catch(...)
			{
				throw;
			}
			this->m_listPoint[this->m_nPrismPart] = new_points;
		}
	}
	else
	{
		// select radio button
		switch(pData_source->Get_user_source_type())
		{
		case Data_source::NONE:
			this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_POINTS, IDC_RADIO_NONE);
			break;
		case Data_source::CONSTANT:
			this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_POINTS, IDC_RADIO_CONSTANT);
			break;
		case Data_source::ARCRASTER:
			this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_POINTS, IDC_RADIO_ARCRASTER);
			break;
		case Data_source::SHAPE:
			this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_POINTS, IDC_RADIO_SHAPE);
			break;
		case Data_source::XYZ:
			this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_POINTS, IDC_RADIO_XYZ);
			break;
		case Data_source::POINTS:
			this->CheckRadioButton(IDC_RADIO_NONE, IDC_RADIO_POINTS, IDC_RADIO_POINTS);
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
			::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_SHAPEFILE, this->m_sShapefile[this->m_nPrismPart]);

			// XYZ
			::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_XYZ, this->m_sXYZ[this->m_nPrismPart]);
		}
		else
		{
			// TOP or BOTTOM

			// constant
			DDX_Text(pDX, IDC_EDIT_CONSTANT, this->m_dConstant[this->m_nPrismPart]);

			// arcraster
			::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_ARCRASTER, this->m_sArcraster[this->m_nPrismPart]);

			// shapefile
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

			// XYZ
			::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_XYZ, this->m_sXYZ[this->m_nPrismPart]);
		}

		// points
		if (this->m_listPoint[this->m_nPrismPart].size() > 0)
		{
			// Prepare points grid
			//
			TRY
			{
				this->m_wndPointsGrid.SetRowCount((int)(1 + this->m_listPoint[this->m_nPrismPart].size()));
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
			std::list<Point>::iterator iter = this->m_listPoint[this->m_nPrismPart].begin();
			for (int row = 1; row < this->m_wndPointsGrid.GetRowCount(); ++row, ++iter)
			{
				double *c = (*iter).get_coord();
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 0, c[0]);
				DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 1, c[1]);
				if (this->m_nPrismPart != CBoxPropertiesDialogBar::PRISM_PERIMETER)
				{
					DDX_TextGridControl(pDX, IDC_GRID_POINTS, row, 2, c[2]);
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
		if (pData_source->Get_user_source_type() == Data_source::POINTS)
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
				ds.Set_source_type(Data_source::CONSTANT);
				ds.Set_user_source_type(Data_source::CONSTANT);
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
					ds.Set_user_source_type(Data_source::ARCRASTER);

					PHAST_Transform::COORDINATE_SYSTEM c = PHAST_Transform::MAP;
					ds.Set_coordinate_system(c);
					ds.Set_user_coordinate_system(c);

					std::string file_name(this->m_sArcraster[this->m_nPrismPart]);
					ds.Set_file_name(file_name);
					oss << ds << std::endl;
				}
			}
			else if (this->m_nDataSourceType == Data_source::SHAPE)
			{
				if (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER)
				{
					Data_source ds;
					ds.Set_source_type(Data_source::SHAPE);
					ds.Set_user_source_type(Data_source::SHAPE);

					PHAST_Transform::COORDINATE_SYSTEM c = PHAST_Transform::MAP;
					ds.Set_coordinate_system(c);
					ds.Set_user_coordinate_system(c);

					std::string file_name(this->m_sShapefile[this->m_nPrismPart]);
					ds.Set_file_name(file_name);
					oss << ds << std::endl;
				}
				else
				{
					Data_source ds;
					ds.Set_source_type(Data_source::SHAPE);
					ds.Set_user_source_type(Data_source::SHAPE);

					PHAST_Transform::COORDINATE_SYSTEM c = PHAST_Transform::MAP;
					ds.Set_coordinate_system(c);
					ds.Set_user_coordinate_system(c);

					std::string file_name(this->m_sShapefile[this->m_nPrismPart]);
					ds.Set_file_name(file_name);
					if (this->m_nShapeAttribute[this->m_nPrismPart] != -1)
					{
						ds.Set_attribute(this->m_nShapeAttribute[this->m_nPrismPart]);
					}
					oss << ds << std::endl;
				}
			}
			else if (this->m_nDataSourceType == Data_source::XYZ)
			{
				if (CGlobal::FileExists(this->m_sXYZ[this->m_nPrismPart]))
				{
					Data_source ds;
					ds.Set_source_type(Data_source::XYZ);
					ds.Set_user_source_type(Data_source::XYZ);

					// TODO read coor system from dialog
					PHAST_Transform::COORDINATE_SYSTEM c = PHAST_Transform::MAP;
					ds.Set_coordinate_system(c);
					ds.Set_user_coordinate_system(c);

					std::string file_name(this->m_sXYZ[this->m_nPrismPart]);
					ds.Set_file_name(file_name);
					oss << ds << std::endl;
				}
			}
			else if (this->m_nDataSourceType == Data_source::POINTS)
			{
				Data_source ds;
				ds.Set_source_type(Data_source::POINTS);
				ds.Set_user_source_type(Data_source::POINTS);

				PHAST_Transform::COORDINATE_SYSTEM c = PHAST_Transform::MAP;
				ds.Set_coordinate_system(c);
				ds.Set_user_coordinate_system(c);

				std::vector<Point> &pts = ds.Get_points();
				std::vector<Point> &upts = ds.Get_user_points();
				pts.clear();
				upts.clear();
				std::list<Point>::iterator cit = this->m_listPoint[this->m_nPrismPart].begin();
				for (; cit != this->m_listPoint[this->m_nPrismPart].end(); ++cit)
				{
					pts.push_back(*cit);
					upts.push_back(*cit);
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

		// load new prism
		Prism new_prism;
		CGlobal::DumpAndLoadPrism(copy, new_prism);
		copy.Tidy();


		// setup domain
		this->m_pView->GetDocument()->GetDefaultZone(::domain);

		new_prism.Tidy();

		CZonePrismResetAction *pAction = new CZonePrismResetAction(
			this->m_pView,
			pZoneActor,
			&new_prism
			);

		this->m_pView->GetDocument()->Execute(pAction);


		// save points
		std::list<Point> save = this->m_listPoint[this->m_nPrismPart];
		this->UpdatePrism(pZoneActor);

		// restore points
		this->m_listPoint[this->m_nPrismPart] = save;

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
		pEdit->EnableWindow(bEnable);
		pEdit->SetReadOnly(!bEnable);
	}
}

void CBoxPropertiesDialogBar::EnableArcraster(BOOL bEnable)
{
	if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_ARCRASTER))
	{
		pEdit->EnableWindow(FALSE);
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
		pEdit->EnableWindow(FALSE);
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

void CBoxPropertiesDialogBar::EnableXYZ(BOOL bEnable)
{
	if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_XYZ))
	{
		pEdit->EnableWindow(FALSE);
		pEdit->SetReadOnly(TRUE);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_BUTTON_XYZ))
	{
		pWnd->EnableWindow(bEnable);
	}
}

void CBoxPropertiesDialogBar::EnablePoints(BOOL bEnable)
{
	if (CWnd* pWnd = this->GetDlgItem(IDC_BUTTON_ADD_POINT))
	{
		pWnd->EnableWindow(bEnable);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_BUTTON_DELETE_POINT))
	{
		pWnd->EnableWindow(bEnable);
	}
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
	if (CWnd* pWnd = this->GetDlgItem(IDC_RADIO_XYZ))
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

void CBoxPropertiesDialogBar::OnBnClickedXYZ(void)
{
	static char szFilters[] =
		"All Files (*.*)|*.*|"
		"|";

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog fileDlg(TRUE, "", NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	if (this->m_sXYZ[this->m_nPrismPart].GetLength())
	{
		TCHAR buffer[MAX_PATH];
		::strcpy(buffer, this->m_sXYZ[this->m_nPrismPart]);
		if (::PathRemoveFileSpec(buffer))
		{
			fileDlg.m_ofn.lpstrInitialDir = buffer;
		}
	}

	if (fileDlg.DoModal() == IDOK)
	{
		this->m_sXYZ[this->m_nPrismPart] = fileDlg.GetPathName();
		::PathSetDlgItemPath(this->GetSafeHwnd(), IDC_EDIT_XYZ, this->m_sXYZ[this->m_nPrismPart]);
		if (CGlobal::IsValidXYZFile(this->m_sXYZ[this->m_nPrismPart]))
		{
			this->OnApply();
		}
	}
}

void CBoxPropertiesDialogBar::UpdatePrismControls(void)
{
	switch(this->GetCheckedRadioButton(IDC_RADIO_NONE, IDC_RADIO_POINTS))
	{
	case IDC_RADIO_NONE:
		this->EnableNone(TRUE);
		this->EnableConstant(FALSE);
		this->EnableArcraster(FALSE);
		this->EnableShape(FALSE);
		this->EnableXYZ(FALSE);
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
			this->EnableXYZ(FALSE);
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
			this->EnableXYZ(FALSE);
			this->EnablePoints(FALSE);
			this->EnablePrismRadios(TRUE);
		}
		break;
	case IDC_RADIO_SHAPE:
		if (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER)
		{
			this->EnableNone(FALSE);
			this->EnableConstant(FALSE);
			this->EnableArcraster(FALSE);
			this->EnableShape(FALSE);
			if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_SHAPEFILE))
			{
				pEdit->EnableWindow(TRUE);
				pEdit->SetReadOnly(TRUE);
			}
			this->EnableXYZ(FALSE);
			this->EnablePoints(FALSE);
			this->EnablePrismRadios(FALSE);
		}
		else
		{
			this->EnableNone(FALSE);
			this->EnableConstant(FALSE);
			this->EnableArcraster(FALSE);
			this->EnableShape(TRUE);
			this->EnableXYZ(FALSE);
			this->EnablePoints(FALSE);
			this->EnablePrismRadios(TRUE);
		}
		break;
	case IDC_RADIO_XYZ:
		this->EnableNone(FALSE);
		this->EnableConstant(FALSE);
		this->EnableArcraster(FALSE);
		this->EnableShape(FALSE);
		this->EnableXYZ(TRUE);
		this->EnablePoints(FALSE);
		this->EnablePrismRadios(TRUE);
		break;
	case IDC_RADIO_POINTS:
		this->EnableNone(FALSE);
		this->EnableConstant(FALSE);
		this->EnableArcraster(FALSE);
		this->EnableShape(FALSE);
		this->EnableXYZ(FALSE);
		this->EnablePoints(TRUE);
		this->EnablePrismRadios(TRUE);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

void CBoxPropertiesDialogBar::OnCbnSelChangeShape(void)
{
	TRACE("%s, in\n", __FUNCTION__);
	if (this->m_nType == CBoxPropertiesDialogBar::BP_PRISM)
	{
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
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::OnEnKillfocusConstant()
{
	TRACE("%s, in\n", __FUNCTION__);
	if (this->m_nType == CBoxPropertiesDialogBar::BP_PRISM)
	{
		if (this->m_bNeedsUpdate)
		{
			double d;
			if (::IsValidFloatFormat(this, IDC_EDIT_CONSTANT, d))
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
	if (!(::IsValidFloatFormat(this, IDC_EDIT_CONSTANT, d) && this->m_dConstant[this->m_nPrismPart] == d))
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
	if (::IsValidFloatFormat(this, IDC_GRID_POINTS, pnmgv->iRow, pnmgv->iColumn, dCell))
	{
		if (this->m_nPrismPart == this->PRISM_PERIMETER)
		{
			std::vector<Point> vect;
			std::list<Point>::iterator li = this->m_listPoint[PRISM_PERIMETER].begin();
			for (; li != this->m_listPoint[PRISM_PERIMETER].end(); ++li)
			{
				vect.push_back(*li);
			}
			for (size_t i = 0; i < vect.size(); ++i)
			{
				double *c = vect[i].get_coord();
				TRACE("Points %d=%g, %g, %g\n", i, c[0], c[1], c[2]);
			}

			// check original
			ASSERT(vect.size() > 2);
			if (Prism::Polygon_intersects_self(vect))
			{
				ASSERT(FALSE);
				*result = 1;
				return;
			}
			_stprintf(szBuffer, _T("%.*g"), DBL_DIG, vect[pnmgv->iRow - 1].get_coord()[pnmgv->iColumn]);

			// check new
			vect[pnmgv->iRow - 1].get_coord()[pnmgv->iColumn] = dCell;
			if (Prism::Polygon_intersects_self(vect))
			{
				if (bShowErrors)
				{
					::AfxMessageBox("Perimeter cannot cross itself. Resetting original coordinates.");
				}
				VERIFY(this->m_wndPointsGrid.SetItemText(pnmgv->iRow, pnmgv->iColumn, szBuffer));
				return;
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

	double scale_h = units.map_horizontal.input_to_si/units.horizontal.input_to_si;
	double scale_v = units.map_vertical.input_to_si/units.vertical.input_to_si;

	vtkTransform *trans = vtkTransform::New();
	trans->Scale(scale_h, scale_h, scale_v);
	trans->RotateZ(-gridKeyword.m_grid_angle);
	trans->Translate(-gridKeyword.m_grid_origin[0], -gridKeyword.m_grid_origin[1], -gridKeyword.m_grid_origin[2]);
	
	double pt[3];
	pt[0] = this->m_XWell;
	pt[1] = this->m_YWell;
	pt[2] = 0.0;

	if (this->IsDlgButtonChecked(IDC_CHECK_USE_MAP) == BST_CHECKED)
	{
		trans->Inverse();
		trans->TransformPoint(pt, pt);
		this->m_XWell = pt[0];
		this->m_YWell = pt[1];
		this->m_xy_coordinate_system_user = PHAST_Transform::MAP;
	}
	else
	{
		trans->TransformPoint(pt, pt);
		this->m_XWell = pt[0];
		this->m_YWell = pt[1];
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
	trans->Delete();
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
		// Cube => Prism
		//
		if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
		{
			ASSERT(pZoneActor->GetPolyhedronType() == Polyhedron::CUBE);
			if (Cube *pc = dynamic_cast<Cube*>(pZoneActor->GetPolyhedron()))
			{
				ASSERT(pc->Get_coordinate_system() == PHAST_Transform::GRID);
				Prism p1(*pc);
				Prism p2(*pc);

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::GRID);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::GRID);
				ASSERT(p2.perimeter.Get_coordinate_system()      == PHAST_Transform::GRID);
				ASSERT(p2.perimeter.Get_user_coordinate_system() == PHAST_Transform::GRID);

				p2.Convert_coordinates(PHAST_Transform::MAP, &map2grid);

				p1.perimeter.Set_points(p2.perimeter.Get_points());
				p1.top.Set_points(p2.top.Get_points());
				p1.bottom.Set_points(p2.bottom.Get_points());

				p1.perimeter.Set_coordinate_system(PHAST_Transform::MAP);
				p1.perimeter.Set_user_coordinate_system(PHAST_Transform::MAP);
				p1.Tidy();

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::MAP);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP);

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
				Prism p1(*pc);
				Prism p2(*pc);

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::MAP);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP);
				ASSERT(p2.perimeter.Get_coordinate_system()      == PHAST_Transform::MAP);
				ASSERT(p2.perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP);

				p2.Convert_coordinates(PHAST_Transform::GRID, &map2grid);

				p1.perimeter.Set_points(p2.perimeter.Get_points());
				p1.top.Set_points(p2.top.Get_points());
				p1.bottom.Set_points(p2.bottom.Get_points());

				p1.perimeter.Set_coordinate_system(PHAST_Transform::GRID);
				p1.perimeter.Set_user_coordinate_system(PHAST_Transform::GRID);
				p1.Tidy();

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::GRID);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::GRID);

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

void CBoxPropertiesDialogBar::OnChangeCoorSys(void)
{
	switch (this->m_nType)
	{
	case CBoxPropertiesDialogBar::BP_MIN_MAX:
		this->OnChangeCoorSysBox();
		break;
	case CBoxPropertiesDialogBar::BP_WEDGE:
		this->OnChangeCoorSysWedge();
		break;
	case CBoxPropertiesDialogBar::BP_WELL:
		ASSERT(FALSE); // TODO
		break;
	case CBoxPropertiesDialogBar::BP_DRAIN:
		ASSERT(FALSE); // TODO
		break;
	case CBoxPropertiesDialogBar::BP_RIVER:
		ASSERT(FALSE); // TODO
		break;
	}
}

void CBoxPropertiesDialogBar::OnChangeCoorSysBox(void)
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

	if (this->GetCheckedRadioButton(IDC_RADIO_CS_GRID, IDC_RADIO_CS_MAP) == IDC_RADIO_CS_MAP)
	{
		// Grid => Map
		// Cube => Prism
		//
		if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
		{
			ASSERT(pZoneActor->GetPolyhedronType() == Polyhedron::CUBE);
			if (Cube *pc = dynamic_cast<Cube*>(pZoneActor->GetPolyhedron()))
			{
				ASSERT(pc->Get_coordinate_system() == PHAST_Transform::GRID);
				Prism p1(*pc);
				Prism p2(*pc);

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::GRID);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::GRID);
				ASSERT(p2.perimeter.Get_coordinate_system()      == PHAST_Transform::GRID);
				ASSERT(p2.perimeter.Get_user_coordinate_system() == PHAST_Transform::GRID);

				p2.Convert_coordinates(PHAST_Transform::MAP, &map2grid);

				p1.perimeter.Set_points(p2.perimeter.Get_points());
				p1.top.Set_points(p2.top.Get_points());
				p1.bottom.Set_points(p2.bottom.Get_points());

				p1.perimeter.Set_coordinate_system(PHAST_Transform::MAP);
				p1.perimeter.Set_user_coordinate_system(PHAST_Transform::MAP);
				p1.Tidy();

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::MAP);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP);

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
		// Cube => Prism
		//
		if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
		{
			ASSERT(pZoneActor->GetPolyhedronType() == Polyhedron::CUBE);
			if (Cube *pc = dynamic_cast<Cube*>(pZoneActor->GetPolyhedron()))
			{
				ASSERT(pc->Get_coordinate_system() == PHAST_Transform::MAP);
				Prism p1(*pc);
				Prism p2(*pc);

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::MAP);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP);
				ASSERT(p2.perimeter.Get_coordinate_system()      == PHAST_Transform::MAP);
				ASSERT(p2.perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP);

				p2.Convert_coordinates(PHAST_Transform::GRID, &map2grid);

				p1.perimeter.Set_points(p2.perimeter.Get_points());
				p1.top.Set_points(p2.top.Get_points());
				p1.bottom.Set_points(p2.bottom.Get_points());

				p1.perimeter.Set_coordinate_system(PHAST_Transform::GRID);
				p1.perimeter.Set_user_coordinate_system(PHAST_Transform::GRID);
				p1.Tidy();

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::GRID);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::GRID);

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
		}
	}
}

void CBoxPropertiesDialogBar::OnChangeCoorSysWedge(void)
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

	if (this->GetCheckedRadioButton(IDC_RADIO_CS_GRID, IDC_RADIO_CS_MAP) == IDC_RADIO_CS_MAP)
	{
		// Grid => Map
		// Wedge => Prism
		//
		if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
		{
			ASSERT(pZoneActor->GetPolyhedronType() == Polyhedron::WEDGE);
			if (Wedge *pw = dynamic_cast<Wedge*>(pZoneActor->GetPolyhedron()))
			{
				ASSERT(pw->Get_coordinate_system() == PHAST_Transform::GRID);
				Prism p1(*pw);
				Prism p2(*pw);

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::GRID);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::GRID);
				ASSERT(p2.perimeter.Get_coordinate_system()      == PHAST_Transform::GRID);
				ASSERT(p2.perimeter.Get_user_coordinate_system() == PHAST_Transform::GRID);

				p2.Convert_coordinates(PHAST_Transform::MAP, &map2grid);

				p1.perimeter.Set_points(p2.perimeter.Get_points());
				p1.top.Set_points(p2.top.Get_points());
				p1.bottom.Set_points(p2.bottom.Get_points());

				p1.perimeter.Set_coordinate_system(PHAST_Transform::MAP);
				p1.perimeter.Set_user_coordinate_system(PHAST_Transform::MAP);
				p1.Tidy();

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::MAP);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP);

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
		// Wedge => Prism
		//
		if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : NULL)
		{
			ASSERT(pZoneActor->GetPolyhedronType() == Polyhedron::WEDGE);
			if (Wedge *pw = dynamic_cast<Wedge*>(pZoneActor->GetPolyhedron()))
			{
				ASSERT(pw->Get_coordinate_system() == PHAST_Transform::MAP);
				Prism p1(*pw);
				Prism p2(*pw);

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::MAP);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP);
				ASSERT(p2.perimeter.Get_coordinate_system()      == PHAST_Transform::MAP);
				ASSERT(p2.perimeter.Get_user_coordinate_system() == PHAST_Transform::MAP);

				p2.Convert_coordinates(PHAST_Transform::GRID, &map2grid);

				p1.perimeter.Set_points(p2.perimeter.Get_points());
				p1.top.Set_points(p2.top.Get_points());
				p1.bottom.Set_points(p2.bottom.Get_points());

				p1.perimeter.Set_coordinate_system(PHAST_Transform::GRID);
				p1.perimeter.Set_user_coordinate_system(PHAST_Transform::GRID);
				p1.Tidy();

				ASSERT(p1.perimeter.Get_coordinate_system()      == PHAST_Transform::GRID);
				ASSERT(p1.perimeter.Get_user_coordinate_system() == PHAST_Transform::GRID);

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
		}
	}
}

void CBoxPropertiesDialogBar::OnBnClickedAddPoint(void)
{
	TRACE("OnBnClickedAddPoint\n");

	ASSERT(this->m_nType == CBoxPropertiesDialogBar::BP_PRISM);
	ASSERT(this->m_wndPointsGrid.GetRowCount() - this->m_wndPointsGrid.GetFixedRowCount() == this->m_listPoint[this->m_nPrismPart].size());

	if (this->m_nPrismPart == CBoxPropertiesDialogBar::PRISM_PERIMETER)
	{
		if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : 0)
		{
			int min_row = this->m_wndPointsGrid.GetSelectedCellRange().GetMinRow();
			int max_row = this->m_wndPointsGrid.GetSelectedCellRange().GetMaxRow();

			if (!(this->m_wndPointsGrid.GetFixedRowCount() <= min_row && min_row < this->m_wndPointsGrid.GetRowCount()))
			{
				min_row = max_row = -1;
			}

			int a = max_row - min_row + 1;
			int count = a;
			Point pt1;
			Point pt2;

			std::list<Point>::const_iterator it = this->m_listPoint[this->m_nPrismPart].begin();
			if (min_row == this->m_wndPointsGrid.GetFixedRowCount())
			{
				// first row selected
				pt1 = this->m_listPoint[this->m_nPrismPart].back();
				pt2 = this->m_listPoint[this->m_nPrismPart].front();
			}
			else if (min_row == -1)
			{
				// no rows selected (ADD AS LAST ROW)
				pt1 = this->m_listPoint[this->m_nPrismPart].front();
				pt2 = this->m_listPoint[this->m_nPrismPart].back();
			}
			else
			{
				for (int i = 0; i < min_row - 2; ++i)
				{
					++it;
				}
				pt1 = (*it);
				++it;
				pt2 = (*it);
			}

			TCHAR szBuffer[80];
			double value;
			for (int row = min_row; row <= max_row; ++row, --a)
			{
				int i = this->m_wndPointsGrid.InsertRow("", min_row);
				this->m_wndPointsGrid.SetRowHeight(i, this->m_wndPointsGrid.GetRowHeight(0));

				value = pt1.x() + a * (pt2.x() - pt1.x()) / (count + 1);
				_stprintf(szBuffer, _T("%.*g"), DBL_DIG, value);
				VERIFY(this->m_wndPointsGrid.SetItemText(i, 0, szBuffer));

				value = pt1.y() + a * (pt2.y() - pt1.y()) / (count + 1);
				_stprintf(szBuffer, _T("%.*g"), DBL_DIG, value);
				VERIFY(this->m_wndPointsGrid.SetItemText(i, 1, szBuffer));
			}

			this->m_wndPointsGrid.Invalidate();

			if (this->UpdateData(TRUE))
			{
				this->ApplyNewPrism(pZoneActor);
			}
		}
	}
	else
	{
		if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : 0)
		{
			int min_row = this->m_wndPointsGrid.GetSelectedCellRange().GetMinRow();
			int max_row = this->m_wndPointsGrid.GetSelectedCellRange().GetMaxRow();

			if (!(this->m_wndPointsGrid.GetFixedRowCount() <= min_row && min_row < this->m_wndPointsGrid.GetRowCount()))
			{
				min_row = max_row = -1;
			}

			int a = max_row - min_row + 1;
			int count = a;
			Point pt1;
			Point pt2;

			std::list<Point>::const_iterator it = this->m_listPoint[this->m_nPrismPart].begin();
			if (min_row == this->m_wndPointsGrid.GetFixedRowCount())
			{
				// first row selected
				pt1 = this->m_listPoint[this->m_nPrismPart].back();
				pt2 = this->m_listPoint[this->m_nPrismPart].front();
			}
			else if (min_row == -1)
			{
				// no rows selected (ADD AS LAST ROW)
				pt1 = this->m_listPoint[this->m_nPrismPart].front();
				pt2 = this->m_listPoint[this->m_nPrismPart].back();
			}
			else
			{
				for (int i = 0; i < min_row - 2; ++i)
				{
					++it;
				}
				pt1 = (*it);
				++it;
				pt2 = (*it);
			}

			TCHAR szBuffer[80];
			double value;
			for (int row = min_row; row <= max_row; ++row, --a)
			{
				int i = this->m_wndPointsGrid.InsertRow("", min_row);
				this->m_wndPointsGrid.SetRowHeight(i, this->m_wndPointsGrid.GetRowHeight(0));

				value = pt1.x() + a * (pt2.x() - pt1.x()) / (count + 1);
				_stprintf(szBuffer, _T("%.*g"), DBL_DIG, value);
				VERIFY(this->m_wndPointsGrid.SetItemText(i, 0, szBuffer));

				value = pt1.y() + a * (pt2.y() - pt1.y()) / (count + 1);
				_stprintf(szBuffer, _T("%.*g"), DBL_DIG, value);
				VERIFY(this->m_wndPointsGrid.SetItemText(i, 1, szBuffer));

				value = (pt1.z() + pt2.z())/2;
				_stprintf(szBuffer, _T("%.*g"), DBL_DIG, value);
				VERIFY(this->m_wndPointsGrid.SetItemText(i, 2, szBuffer));
			}

			this->m_wndPointsGrid.Invalidate();

			if (this->UpdateData(TRUE))
			{
				this->ApplyNewPrism(pZoneActor);
			}
		}
	}
}

void CBoxPropertiesDialogBar::OnBnClickedDeletePoint(void)
{
	TRACE("OnBnClickedDeletePoint\n");

	ASSERT(this->m_nType == CBoxPropertiesDialogBar::BP_PRISM);

	if (this->m_nPrismPart == this->PRISM_PERIMETER)
	{
		ASSERT(this->m_wndPointsGrid.GetRowCount() - this->m_wndPointsGrid.GetFixedRowCount() == this->m_listPoint[this->m_nPrismPart].size());

		if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : 0)
		{
			int min_row = this->m_wndPointsGrid.GetSelectedCellRange().GetMinRow();
			int max_row = this->m_wndPointsGrid.GetSelectedCellRange().GetMaxRow();

			if (!(this->m_wndPointsGrid.GetFixedRowCount() <= min_row && min_row < this->m_wndPointsGrid.GetRowCount()))
			{
				::AfxMessageBox(_T("No points selected."));
				return;
			}

			if (!(this->m_wndPointsGrid.GetFixedRowCount() <= max_row && max_row < this->m_wndPointsGrid.GetRowCount()))
			{
				::AfxMessageBox(_T("No points selected."));
				return;
			}

			int resulting_pt_count = (this->m_wndPointsGrid.GetRowCount() - this->m_wndPointsGrid.GetFixedRowCount()) - (max_row - min_row + 1);
			if (resulting_pt_count < 3)
			{
				::AfxMessageBox(_T("Perimeters must contain at least three points."));
				return;
			}

			std::vector<Point> vect;
			std::list<Point>::iterator li = this->m_listPoint[this->m_nPrismPart].begin();
			for (int row = this->m_wndPointsGrid.GetFixedRowCount(); li != this->m_listPoint[this->m_nPrismPart].end(); ++li, ++row)
			{
				if (min_row <= row && row <= max_row)
				{
					continue;
				}
				vect.push_back(*li);
			}
			ASSERT(vect.size() == resulting_pt_count);

			if (Prism::Polygon_intersects_self(vect))
			{
				::AfxMessageBox(_T("Deleting this point would cause the perimeter to cross itself."));
				return;
			}

			for (int row = max_row; row >= min_row; --row)
			{
				VERIFY(this->m_wndPointsGrid.DeleteRow(row));
			}

			if (this->UpdateData(TRUE))
			{
				this->ApplyNewPrism(pZoneActor);
			}
		}
	}
	else
	{
		ASSERT(this->m_wndPointsGrid.GetRowCount() - this->m_wndPointsGrid.GetFixedRowCount() == this->m_listPoint[this->m_nPrismPart].size());

		if (CZoneActor* pZoneActor = this->m_pProp3D ? CZoneActor::SafeDownCast(this->m_pProp3D) : 0)
		{
			int min_row = this->m_wndPointsGrid.GetSelectedCellRange().GetMinRow();
			int max_row = this->m_wndPointsGrid.GetSelectedCellRange().GetMaxRow();

			if (!(this->m_wndPointsGrid.GetFixedRowCount() <= min_row && min_row < this->m_wndPointsGrid.GetRowCount()))
			{
				::AfxMessageBox(_T("No points selected."));
				return;
			}

			if (!(this->m_wndPointsGrid.GetFixedRowCount() <= max_row && max_row < this->m_wndPointsGrid.GetRowCount()))
			{
				::AfxMessageBox(_T("No points selected."));
				return;
			}

			int resulting_pt_count = (this->m_wndPointsGrid.GetRowCount() - this->m_wndPointsGrid.GetFixedRowCount()) - (max_row - min_row + 1);
			if (resulting_pt_count < 1)
			{
				::AfxMessageBox(_T("Must have at least one point."));
				return;
			}

			std::vector<Point> vect;
			std::list<Point>::iterator li = this->m_listPoint[this->m_nPrismPart].begin();
			for (int row = this->m_wndPointsGrid.GetFixedRowCount(); li != this->m_listPoint[this->m_nPrismPart].end(); ++li, ++row)
			{
				if (min_row <= row && row <= max_row)
				{
					continue;
				}
				vect.push_back(*li);
			}
			ASSERT(vect.size() == resulting_pt_count);

			for (int row = max_row; row >= min_row; --row)
			{
				VERIFY(this->m_wndPointsGrid.DeleteRow(row));
			}

			if (this->UpdateData(TRUE))
			{
				this->ApplyNewPrism(pZoneActor);
			}
		}
	}
}
