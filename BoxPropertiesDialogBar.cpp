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

#include "ZoneResizeAction.h"
#include "ZoneActor.h"
#include "WellActor.h"
#include "RiverActor.h"

#include "Units.h"
#include "Global.h"
//// #include "Zone.h"
//// #include "ZoneLODActor.h"

IMPLEMENT_DYNAMIC(CBoxPropertiesDialogBar, CSizingDialogBarCFVS7);


BEGIN_MESSAGE_MAP(CBoxPropertiesDialogBar, CSizingDialogBarCFVS7)
	ON_EN_KILLFOCUS(IDC_EDIT_X, OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_Y, OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_Z, OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH, OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH, OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHT, OnEnKillfocusEdit)
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
END_MESSAGE_MAP()

CBoxPropertiesDialogBar::CBoxPropertiesDialogBar()
 : m_pView(0)
 , m_pProp3D(0)
 , m_bNeedsUpdate(false)
 , m_strHorizontalUnits("")
 , m_strVerticalUnits("")
 , m_nType(BP_POS_PLUS_LENGTH)
{
}

CBoxPropertiesDialogBar::~CBoxPropertiesDialogBar()
{
}

BOOL CBoxPropertiesDialogBar::Create(UINT nIDTemplate, LPCTSTR lpszWindowName,
	CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	switch (nIDTemplate) {
		case IDD_PROPS_CUBE:
			this->m_nType = CBoxPropertiesDialogBar::BP_POS_PLUS_LENGTH;
			break;
		case IDD_PROPS_CUBE2:
			this->m_nType = CBoxPropertiesDialogBar::BP_MIN_MAX;
			break;
		default:
			ASSERT(FALSE);
			break;
	}
	return this->CSizingDialogBarCFVS7::Create(nIDTemplate, lpszWindowName, pParentWnd, nID, dwStyle);
}

vtkProp3D* CBoxPropertiesDialogBar::GetProp3D(void)
{
	return this->m_pProp3D;
}

void CBoxPropertiesDialogBar::Update(IObserver* pSender, LPARAM lHint, CObject* pHint, vtkObject* pObject)
{
	ASSERT(pSender != this);
	switch (lHint)
	{
	case WPN_NONE:
		break;
	case WPN_SELCHANGED:
		if (vtkProp* pProp = vtkProp::SafeDownCast(pObject))
		{
			if (CZoneActor* pZoneActor = CZoneActor::SafeDownCast(pProp))
			{
				ASSERT(FALSE); // untested
			}
			else if (CWellActor* pWellActor = CWellActor::SafeDownCast(pProp))
			{
				CUnits units;
				this->Set(0, 0, units);
			}
			else if (CRiverActor* pWellActor = CRiverActor::SafeDownCast(pProp))
			{
				TRACE("TODO\n");
			}
			else
			{
				ASSERT(FALSE); // untested
			}
		}
		break;
	case WPN_VISCHANGED:
		// TODO ??  ASSERT(FALSE);
		break;
	case WPN_SCALE_CHANGED:
		break;
	default:
		ASSERT(FALSE);
	}
}

void CBoxPropertiesDialogBar::Set(CWPhastView* pView, vtkProp3D* pProp3D, const CUnits& units)
{
	this->m_pView = pView;
	this->m_pProp3D = pProp3D;

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

		if (units.horizontal.defined)
		{
			this->m_strHorizontalUnits.Format("[%s]", units.horizontal.input);
			CGlobal::MinimizeLengthUnits(this->m_strHorizontalUnits);
		}
		else
		{
			this->m_strHorizontalUnits.Format("[%s]", units.horizontal.si);
		}
		if (units.vertical.defined)
		{
			this->m_strVerticalUnits.Format("[%s]", units.vertical.input);
			CGlobal::MinimizeLengthUnits(this->m_strVerticalUnits);

		}
		else
		{
			this->m_strVerticalUnits.Format("[%s]", units.vertical.si);
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

		CString caption;
		caption.Format(_T("Zone dimensions (%s)"), pZone->GetName());
		this->SetWindowText(caption);
	}
	else
	{
		this->Enable(FALSE);
		this->SetWindowText(_T("Zone dimensions"));
	}

}

void CBoxPropertiesDialogBar::DoDataExchange(CDataExchange* pDX)
{
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
	}


	CSizingDialogBarCFVS7::DoDataExchange(pDX);
	if (!pDX->m_bSaveAndValidate) {
		this->m_bNeedsUpdate = false;
		this->GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
	}
}

void CBoxPropertiesDialogBar::OnBnClickedApply()
{
	if (vtkProp3D* pProp3D = this->m_pProp3D) {
		if (this->UpdateData(TRUE)) {
            ASSERT(this->m_pView);
			CAction* pAction = NULL;
			switch (this->m_nType)
			{
			case CBoxPropertiesDialogBar::BP_POS_PLUS_LENGTH:
				pAction = new CZoneResizeAction(
					this->m_pView,
					reinterpret_cast<CZoneActor*>(pProp3D), // (this->m_pProp3D),
					(this->m_XLength >= 0) ? this->m_X                   : this->m_X + this->m_XLength,
					(this->m_XLength >= 0) ? this->m_X + this->m_XLength : this->m_X,
					(this->m_YLength >= 0) ? this->m_Y                   : this->m_Y + this->m_YLength,
					(this->m_YLength >= 0) ? this->m_Y + this->m_YLength : this->m_Y,
					(this->m_ZLength >= 0) ? this->m_Z                   : this->m_Z + this->m_ZLength,
					(this->m_ZLength >= 0) ? this->m_Z + this->m_ZLength : this->m_Z
					);
				break;
			case CBoxPropertiesDialogBar::BP_MIN_MAX:
				pAction = new CZoneResizeAction(
					this->m_pView,
					reinterpret_cast<CZoneActor*>(pProp3D),
					(this->m_XMin <= this->m_XMax) ? this->m_XMin : this->m_XMax,
					(this->m_XMin <= this->m_XMax) ? this->m_XMax : this->m_XMin,
					(this->m_YMin <= this->m_YMax) ? this->m_YMin : this->m_YMax,
					(this->m_YMin <= this->m_YMax) ? this->m_YMax : this->m_YMin,
					(this->m_ZMin <= this->m_ZMax) ? this->m_ZMin : this->m_ZMax,
					(this->m_ZMin <= this->m_ZMax) ? this->m_ZMax : this->m_ZMin
					);
				break;
			default:
				ASSERT(FALSE);
				break;
			}
			if (pAction && this->m_pView) {
				this->m_pView->GetDocument()->Execute(pAction);
			}
		}
	}
}


void CBoxPropertiesDialogBar::Enable(bool bEnable)
{
	/// this->EnableWindow(bEnable);
	this->GetDlgItem(IDC_APPLY)->EnableWindow(bEnable);

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
	else
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
}

BOOL CBoxPropertiesDialogBar::PreTranslateMessage(MSG* pMsg)
{
	// Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN) {
			// Handle returns withing dialog bar
			//
			if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_X)->GetSafeHwnd()) {
				//OnEnKillfocusEdit();
				OnBnClickedApply();
			}
			if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_Y)->GetSafeHwnd()) {
				//OnEnKillfocusEdit();
				OnBnClickedApply();
			}
			if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_Z)->GetSafeHwnd()) {
				//OnEnKillfocusEdit();
				OnBnClickedApply();
			}
			if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_LENGTH)->GetSafeHwnd()) {
				//OnEnKillfocusEdit();
				OnBnClickedApply();
			}
			if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_WIDTH)->GetSafeHwnd()) {
				//OnEnKillfocusEdit();
				OnBnClickedApply();
			}
			if (pMsg->hwnd == this->GetDlgItem(IDC_EDIT_HEIGHT)->GetSafeHwnd()) {
				//OnEnKillfocusEdit();
				OnBnClickedApply();
			}
		}
	}
	return CSizingDialogBarCFVS7::PreTranslateMessage(pMsg);
}

void CBoxPropertiesDialogBar::OnEnKillfocusEdit()
{
	////if (this->m_pProp3D) {
	////	if (this->m_bNeedsUpdate) {
	////		if (UpdateData(TRUE)) {
	////			this->m_bNeedsUpdate = false;
 ////               ASSERT(this->m_pView);
	////			////////CBoxResizeAction* pAction = new CBoxResizeAction(
	////			////////	this->m_pView,
	////			////////	this->m_pProp3D,
	////			////////	this->m_XLength,
	////			////////	this->m_YLength,
	////			////////	this->m_ZLength,
	////			////////	(float)((2*this->m_X + this->m_XLength)/2),
	////			////////	(float)((2*this->m_Y + this->m_YLength)/2),
	////			////////	(float)((2*this->m_Z + this->m_ZLength)/2)
	////			////////	);
	////			////////this->m_pView->GetDocument()->Execute(pAction);
	////			// delete pAction;	
	////		}
	////	}
	////}
}

void CBoxPropertiesDialogBar::OnEnChange()
{
	this->m_bNeedsUpdate = true;
	this->GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}
