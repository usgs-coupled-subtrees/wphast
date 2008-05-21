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

#include "MacroAction.h"
#include "RiverMovePointAction.h"
#include "ZoneResizeAction.h"
#include "WedgeChangeChopTypeAction.h"
#include "WellSetPositionAction.h"
#include "ZoneActor.h"
#include "WellActor.h"
#include "RiverActor.h"

#include "Units.h"
#include "Global.h"

BOOL IsEditCtrl(HWND hWnd);
BOOL IsEditCtrl(CWnd* pWnd);

BOOL IsValidFloatFormat(CWnd* pWnd, int nIDC, double& d);
BOOL IsValidFloatFormat(CWnd* pWnd, HWND hWndCtrl, double& d);
BOOL IsValidFloatFormat(CWnd* pWnd, int nIDC, int nRow, int nCol, double& value);

IMPLEMENT_DYNAMIC(CBoxPropertiesDialogBar, CSizingDialogBarCFVS7);


BEGIN_MESSAGE_MAP(CBoxPropertiesDialogBar, CSizingDialogBarCFVS7)
// COMMENT: {8/1/2007 2:30:26 PM}	ON_EN_KILLFOCUS(IDC_EDIT_X, OnEnKillfocusEdit)
// COMMENT: {8/1/2007 2:30:26 PM}	ON_EN_KILLFOCUS(IDC_EDIT_Y, OnEnKillfocusEdit)
// COMMENT: {8/1/2007 2:30:26 PM}	ON_EN_KILLFOCUS(IDC_EDIT_Z, OnEnKillfocusEdit)
// COMMENT: {8/1/2007 2:30:26 PM}	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH, OnEnKillfocusEdit)
// COMMENT: {8/1/2007 2:30:26 PM}	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH, OnEnKillfocusEdit)
// COMMENT: {8/1/2007 2:30:26 PM}	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHT, OnEnKillfocusEdit)
	ON_EN_CHANGE(IDC_EDIT_X, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_Y, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_Z, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_LENGTH, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnEnChange)

	ON_EN_CHANGE(IDC_EDIT_XMIN, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_YMIN, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_ZMIN, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_XMAX, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_YMAX, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_ZMAX, OnEnChange)
	ON_BN_CLICKED(IDC_APPLY, OnBnClickedApply)
	//{{
// COMMENT: {7/31/2007 5:47:43 PM}	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
// COMMENT: {7/31/2007 5:47:43 PM}	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
// COMMENT: {7/31/2007 5:47:43 PM}	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
// COMMENT: {7/31/2007 5:47:43 PM}	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
// COMMENT: {7/31/2007 5:47:43 PM}	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
// COMMENT: {7/31/2007 5:47:43 PM}	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
// COMMENT: {7/31/2007 5:47:43 PM}	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
// COMMENT: {7/31/2007 5:47:43 PM}	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
// COMMENT: {7/31/2007 5:47:43 PM}	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
// COMMENT: {7/31/2007 5:47:43 PM}	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
// COMMENT: {7/31/2007 5:47:43 PM}	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
// COMMENT: {7/31/2007 5:47:43 PM}	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_EN_CHANGE(IDC_EDIT_WELL_X, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_WELL_Y, OnEnChange)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_RIVER, OnEndLabelEditGrid)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_XMIN, IDC_EDIT_ZMAX, OnEnKillfocusRange)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_WELL_X, IDC_EDIT_WELL_Y, OnEnKillfocusRange)
	//}}
	//{{
	ON_BN_CLICKED(IDC_RADIO_X, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_Y, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_Z, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_1, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_2, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_3, OnBnClickedUpdateWedge)
	ON_BN_CLICKED(IDC_RADIO_4, OnBnClickedUpdateWedge)
	//}}
END_MESSAGE_MAP()

CBoxPropertiesDialogBar::CBoxPropertiesDialogBar()
 : m_pView(0)
 , m_pProp3D(0)
 , m_bNeedsUpdate(false)
 , m_strHorizontalUnits("")
 , m_strVerticalUnits("")
 , m_nType(CBoxPropertiesDialogBar::BP_NONE)
{
	TRACE("%s\n", __FUNCTION__);
}

CBoxPropertiesDialogBar::~CBoxPropertiesDialogBar()
{
	TRACE("%s\n", __FUNCTION__);
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
	ASSERT_KINDOF(CFrameWnd, pFrame);
	ASSERT_VALID(pFrame);

	CWPhastView* pView = reinterpret_cast<CWPhastView*>(pFrame->GetActiveView());
	ASSERT_KINDOF(CWPhastView, pView);
	ASSERT_VALID(pView);

	switch (lHint)
	{
	case WPN_NONE:
		break;
	case WPN_SELCHANGED:
		if (vtkProp* pProp = vtkProp::SafeDownCast(pObject))
		{
			if (CZoneActor* pZoneActor = CZoneActor::SafeDownCast(pProp))
			{
				this->m_nType = CBoxPropertiesDialogBar::BP_MIN_MAX;

				if (pObject != this->m_pProp3D)
				{
					this->HideRiverControls();
					this->HideWellControls();
					this->HideWedgeControls();
					if (pZoneActor->GetChopType() == srcWedgeSource::CHOP_NONE)
					{
						this->ShowZoneControls();
					}
					else
					{
						this->m_nType = CBoxPropertiesDialogBar::BP_WEDGE;
						this->ShowWedgeControls();
					}
					this->ShowApply();
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
					this->ShowRiverControls();
					this->ShowApply();
				}

				this->Set(pView, pRiverActor, pView->GetDocument()->GetUnits());
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
		break;
	case WPN_SCALE_CHANGED:
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
		this->m_nType = CBoxPropertiesDialogBar::BP_MIN_MAX;

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

		bool bIsWedge = (pZone->GetChopType() != srcWedgeSource::CHOP_NONE);

		if (bIsWedge)
		{
			this->m_nType = CBoxPropertiesDialogBar::BP_WEDGE;
			this->m_nOrientation = srcWedgeSource::ConvertChopType(pZone->GetChopType());
			this->Enable(TRUE);
			this->UpdateData(FALSE);

			CString caption;		
			caption.Format(_T("Wedge dimensions (%s)"), pZone->GetName());
			this->SetWindowText(caption);
		}
		else
		{
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
			CString caption;		
			caption.Format(_T("Zone dimensions (%s)"), pZone->GetName());
			this->SetWindowText(caption);
		}
	}
	else if (CWellActor* pWell = this->m_pProp3D ? CWellActor::SafeDownCast(this->m_pProp3D) : NULL)
	{
		this->m_nType = CBoxPropertiesDialogBar::BP_WELL;

		this->m_XWell = pWell->GetWell().x;
		this->m_YWell = pWell->GetWell().y;

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
	else
	{
		this->m_nType = CBoxPropertiesDialogBar::BP_NONE;		
		this->Enable(FALSE);
		this->SetWindowText(_T(""));
	}

	TRACE("%s, out\n", __FUNCTION__);
}

void CBoxPropertiesDialogBar::DoDataExchange(CDataExchange* pDX)
{
	TRACE("%s, in\n", __FUNCTION__);
	
	DDX_GridControl(pDX, IDC_GRID_RIVER, this->m_wndRiverGrid);

	// Prepare Pump Sched. Grid
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

		DDX_Text(pDX, IDC_X_UNITS_STATIC2, m_strHorizontalUnits);
		DDX_Text(pDX, IDC_Y_UNITS_STATIC2, m_strHorizontalUnits);
		DDX_Text(pDX, IDC_Z_UNITS_STATIC2, m_strVerticalUnits);
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_WELL)
	{
		// wells
		DDX_Text(pDX, IDC_X_UNITS_STATIC, m_strHorizontalUnits);
		DDX_Text(pDX, IDC_Y_UNITS_STATIC, m_strHorizontalUnits);

		DDX_Text(pDX, IDC_EDIT_WELL_X, m_XWell);
		DDX_Text(pDX, IDC_EDIT_WELL_Y, m_YWell);
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_RIVER)
	{
		CRiver river = reinterpret_cast<CRiverActor*>(this->m_pProp3D)->GetRiver();
		TRY
		{
			this->m_wndRiverGrid.SetRowCount(1 + (int)river.m_listPoints.size());
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

			std::list<CRiverPoint>::iterator iter = river.m_listPoints.begin();
			for (int row = 1; row < this->m_wndRiverGrid.GetRowCount(); ++row, ++iter)
			{
				DDX_TextGridControl(pDX, IDC_GRID_RIVER, row, 0, iter->x);
				DDX_TextGridControl(pDX, IDC_GRID_RIVER, row, 1, iter->y);
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

void CBoxPropertiesDialogBar::OnBnClickedApply()
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
						this->m_YWell
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
						if (riverPt->x != pt.first || riverPt->y != pt.second)
						{
							CRiverMovePointAction* pRMPA = new CRiverMovePointAction(
								pRiverActor,
								this->m_pView->GetDocument(),
								r-1,
								riverPt->x,
								riverPt->y
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
		TRACE("%s, true in\n", __FUNCTION__);
	else
		TRACE("%s, false in\n", __FUNCTION__);


#ifdef SHOW_APPLY_BUTTON
	this->GetDlgItem(IDC_APPLY)->EnableWindow(bEnable);
#endif

	if (this->m_nType == CBoxPropertiesDialogBar::BP_POS_PLUS_LENGTH)
	{
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_X)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_Y)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_Z)) {
			pEdit->SetReadOnly(!bEnable);
		}

		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_LENGTH)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_WIDTH)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_HEIGHT)) {
			pEdit->SetReadOnly(!bEnable);
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_MIN_MAX)
	{
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_XMIN)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_YMIN)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_ZMIN)) {
			pEdit->SetReadOnly(!bEnable);
		}

		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_XMAX)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_YMAX)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_ZMAX)) {
			pEdit->SetReadOnly(!bEnable);
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_WELL)
	{
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_WELL_X)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_WELL_Y)) {
			pEdit->SetReadOnly(!bEnable);
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_RIVER)
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
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_XMIN)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_YMIN)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_ZMIN)) {
			pEdit->SetReadOnly(!bEnable);
		}

		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_XMAX)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_YMAX)) {
			pEdit->SetReadOnly(!bEnable);
		}
		if (CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_ZMAX)) {
			pEdit->SetReadOnly(!bEnable);
		}
	}
	else if (this->m_nType == CBoxPropertiesDialogBar::BP_NONE)
	{
		this->HideZoneControls();
		this->HideRiverControls();
		this->HideWedgeControls();
		this->HideWellControls();
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
			if (this->m_nType != CBoxPropertiesDialogBar::BP_RIVER)
			{
				if (::IsEditCtrl(pMsg->hwnd))
				{
					double d;
					if (IsValidFloatFormat(this, pMsg->hwnd, d))
					{
						this->OnBnClickedApply();
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
		this->OnBnClickedApply();
	}
	else
	{
		this->UpdateData(FALSE);
	}

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
			this->OnBnClickedApply();
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
					if (!(IsValidFloatFormat(this, IDC_EDIT_WELL_X, d) && pWell->GetWell().x == (float)d))
					{
						bNeedsUpdate = true;
						break;
					}
					if (!(IsValidFloatFormat(this, IDC_EDIT_WELL_Y, d) && pWell->GetWell().y == (float)d))
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
					if (!(IsValidFloatFormat(this, IDC_GRID_RIVER, r, 0, d) && iter->x == d))
					{
						bNeedsUpdate = true;
						break;
					}
					if (!(IsValidFloatFormat(this, IDC_GRID_RIVER, r, 1, d) && iter->y == d))
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

