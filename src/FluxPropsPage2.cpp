// FluxPropsPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "FluxPropsPage2.h"

#include "Global.h"


// CFluxPropsPage2 dialog

const TCHAR PSZ_FLUX[]     = _T("Flux");
const TCHAR PSZ_SOLUTION[] = _T("Solution");

const TCHAR PSZ_NONE[]     = _T("None");
const TCHAR PSZ_CONSTANT[] = _T("Constant");
const TCHAR PSZ_LINEAR[]   = _T("Linear");
const TCHAR PSZ_POINTS[]   = _T("Points");
const TCHAR PSZ_XYZ[]      = _T("XYZ");

const TCHAR PSZ_SECS[] = _T("seconds");
const TCHAR PSZ_MINS[] = _T("minutes");
const TCHAR PSZ_HOUR[] = _T("hours");
const TCHAR PSZ_DAYS[] = _T("days");
const TCHAR PSZ_YEAR[] = _T("years");

const int TIME_COLUMN = 0;
const int UNIT_COLUMN = 1;
const int TYPE_COLUMN = 2;

const int MIN_ROWS = 200;

const int SELECTED = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CFluxPropsPage2, CPropertyPage)

CFluxPropsPage2::CFluxPropsPage2()
	: CPropertyPage(CFluxPropsPage2::IDD)
	, FluxSeries(this)
	, SolutionSeries(this)
{
	TRACE("In %s\n", __FUNCTION__);
	ASSERT(std::numeric_limits<double>::has_signaling_NaN);
	double d = std::numeric_limits<double>::signaling_NaN();
	ASSERT(d != d);
	ASSERT(::_isnan(d));

	ASSERT(std::numeric_limits<double>::has_infinity);
	double i = std::numeric_limits<double>::infinity();
	ASSERT(!::_finite(i));

	// load property descriptions
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,   IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sAssocSolutionRTF, IDR_BC_FLUX_ASSOC_SOL_RTF);
	CGlobal::LoadRTFString(this->m_sFluxRTF,          IDR_BC_FLUX_FLUX_RTF);
	CGlobal::LoadRTFString(this->m_sFaceRTF,          IDR_BC_FLUX_FACE_RTF);

// COMMENT: {6/25/2009 8:01:37 PM}	// flux vectors
// COMMENT: {6/25/2009 8:01:37 PM}	this->FluxSeries.validation_row = -1;
// COMMENT: {6/25/2009 8:01:37 PM}	this->FluxSeries.vv_props.push_back(&this->FluxVector);
// COMMENT: {6/25/2009 8:01:37 PM}	this->FluxSeries.vv_props.push_back(&this->FluxVectorConstant);
// COMMENT: {6/25/2009 8:01:37 PM}	this->FluxSeries.vv_props.push_back(&this->FluxVectorLinear);
// COMMENT: {6/25/2009 8:01:37 PM}
// COMMENT: {6/25/2009 8:01:37 PM}	// solution vectors
// COMMENT: {6/25/2009 8:01:37 PM}	this->SolutionSeries.validation_row = -1;
// COMMENT: {6/25/2009 8:01:37 PM}	this->SolutionSeries.vv_props.push_back(&this->SolutionVector);
// COMMENT: {6/25/2009 8:01:37 PM}	this->SolutionSeries.vv_props.push_back(&this->SolutionVectorConstant);
// COMMENT: {6/25/2009 8:01:37 PM}	this->SolutionSeries.vv_props.push_back(&this->SolutionVectorLinear);

	// init properties
// COMMENT: {6/25/2009 7:37:44 PM}	CBC bc;
// COMMENT: {6/25/2009 7:37:44 PM}	this->SetProperties(bc);
// COMMENT: {6/25/2009 10:40:41 PM}	this->DefaultUnits = PSZ_YEAR;
	this->SetFlowOnly(false);
	this->SetDefault(false);

	TRACE("Out %s\n", __FUNCTION__);
}

CFluxPropsPage2::~CFluxPropsPage2()
{
	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 9:03:30 PM}	this->FreeVectors();
	TRACE("Out %s\n", __FUNCTION__);
}

// COMMENT: {6/25/2009 9:03:50 PM}void CFluxPropsPage2::FreeVectors()
// COMMENT: {6/25/2009 9:03:50 PM}{
// COMMENT: {6/25/2009 9:03:50 PM}	std::vector<Ctime*>::iterator t;
// COMMENT: {6/25/2009 9:03:50 PM}
// COMMENT: {6/25/2009 9:03:50 PM}	t = this->SolutionSeries.v_times.begin();
// COMMENT: {6/25/2009 9:03:50 PM}	for (; t != this->SolutionSeries.v_times.end(); ++t)
// COMMENT: {6/25/2009 9:03:50 PM}	{
// COMMENT: {6/25/2009 9:03:50 PM}		delete (*t);
// COMMENT: {6/25/2009 9:03:50 PM}	}
// COMMENT: {6/25/2009 9:03:50 PM}
// COMMENT: {6/25/2009 9:03:50 PM}	t = this->FluxSeries.v_times.begin();
// COMMENT: {6/25/2009 9:03:50 PM}	for (; t != this->FluxSeries.v_times.end(); ++t)
// COMMENT: {6/25/2009 9:03:50 PM}	{
// COMMENT: {6/25/2009 9:03:50 PM}		delete (*t);
// COMMENT: {6/25/2009 9:03:50 PM}	}
// COMMENT: {6/25/2009 9:03:50 PM}
// COMMENT: {6/25/2009 9:03:50 PM}	std::vector<Cproperty*>::iterator it;
// COMMENT: {6/25/2009 9:03:50 PM}
// COMMENT: {6/25/2009 9:03:50 PM}	it = this->FluxVector.begin();
// COMMENT: {6/25/2009 9:03:50 PM}	for (; it != this->FluxVector.end(); ++it)
// COMMENT: {6/25/2009 9:03:50 PM}	{
// COMMENT: {6/25/2009 9:03:50 PM}		delete (*it);
// COMMENT: {6/25/2009 9:03:50 PM}	}
// COMMENT: {6/25/2009 9:03:50 PM}
// COMMENT: {6/25/2009 9:03:50 PM}	it = this->FluxVectorConstant.begin();
// COMMENT: {6/25/2009 9:03:50 PM}	for (; it != this->FluxVectorConstant.end(); ++it)
// COMMENT: {6/25/2009 9:03:50 PM}	{
// COMMENT: {6/25/2009 9:03:50 PM}		delete (*it);
// COMMENT: {6/25/2009 9:03:50 PM}	}
// COMMENT: {6/25/2009 9:03:50 PM}
// COMMENT: {6/25/2009 9:03:50 PM}	it = this->FluxVectorLinear.begin();
// COMMENT: {6/25/2009 9:03:50 PM}	for (; it != this->FluxVectorLinear.end(); ++it)
// COMMENT: {6/25/2009 9:03:50 PM}	{
// COMMENT: {6/25/2009 9:03:50 PM}		delete (*it);
// COMMENT: {6/25/2009 9:03:50 PM}	}
// COMMENT: {6/25/2009 9:03:50 PM}
// COMMENT: {6/25/2009 9:03:50 PM}	it = this->SolutionVector.begin();
// COMMENT: {6/25/2009 9:03:50 PM}	for (; it != this->SolutionVector.end(); ++it)
// COMMENT: {6/25/2009 9:03:50 PM}	{
// COMMENT: {6/25/2009 9:03:50 PM}		delete (*it);
// COMMENT: {6/25/2009 9:03:50 PM}	}
// COMMENT: {6/25/2009 9:03:50 PM}
// COMMENT: {6/25/2009 9:03:50 PM}	it = this->SolutionVectorConstant.begin();
// COMMENT: {6/25/2009 9:03:50 PM}	for (; it != this->SolutionVectorConstant.end(); ++it)
// COMMENT: {6/25/2009 9:03:50 PM}	{
// COMMENT: {6/25/2009 9:03:50 PM}		delete (*it);
// COMMENT: {6/25/2009 9:03:50 PM}	}
// COMMENT: {6/25/2009 9:03:50 PM}
// COMMENT: {6/25/2009 9:03:50 PM}	it = this->SolutionVectorLinear.begin();
// COMMENT: {6/25/2009 9:03:50 PM}	for (; it != this->SolutionVectorLinear.end(); ++it)
// COMMENT: {6/25/2009 9:03:50 PM}	{
// COMMENT: {6/25/2009 9:03:50 PM}		delete (*it);
// COMMENT: {6/25/2009 9:03:50 PM}	}
// COMMENT: {6/25/2009 9:03:50 PM}}

void CFluxPropsPage2::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	DDX_Control(pDX, IDC_PROP_TREE, this->TreeCtrl);
	DDX_Control(pDX, IDC_GB_PROPERTY, this->PropertyGroupBox);

	if (this->TreeCtrl.GetCount() == 0)
	{
		this->FluxSeries.treeitem     = this->TreeCtrl.InsertItem(PSZ_FLUX,     TVI_ROOT, TVI_LAST);
		this->SolutionSeries.treeitem = this->TreeCtrl.InsertItem(PSZ_SOLUTION, TVI_ROOT, TVI_LAST);

		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);
		//this->RichEditCtrl.SetWindowText(this->m_sSolutionRTF.c_str());

		this->ItemDDX = this->FluxSeries.treeitem;
		this->TreeCtrl.SelectItem(this->ItemDDX);

		this->TreeCtrl.ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);
	}

	DDX_GridControl(pDX, IDC_TIMES_GRID,    this->FluxSeries.grid);
	DDX_GridControl(pDX, IDC_GRID_SOLUTION, this->SolutionSeries.grid);

// COMMENT: {6/25/2009 7:03:50 PM}	this->InitializeGrid(pDX, this->FluxSeries.grid/*,     this->BC.m_bc_flux*/);
// COMMENT: {6/25/2009 7:03:50 PM}	this->InitializeGrid(pDX, this->SolutionSeries.grid/*, this->BC.m_bc_solution*/);
	this->FluxSeries.InitializeGrid(pDX);
	this->SolutionSeries.InitializeGrid(pDX);


	if (pDX->m_bSaveAndValidate)
	{
		// face
		//
		if (this->IsDlgButtonChecked(IDC_FACE_X_RADIO))
		{
			this->BC.face_defined = TRUE;
			this->BC.face         = 0;
		}
		if (this->IsDlgButtonChecked(IDC_FACE_Y_RADIO))
		{
			this->BC.face_defined = TRUE;
			this->BC.face         = 1;
		}
		if (this->IsDlgButtonChecked(IDC_FACE_Z_RADIO))
		{
			this->BC.face_defined = TRUE;
			this->BC.face         = 2;
		}


		if (this->ItemDDX == this->FluxSeries.treeitem)
		{
			//this->DDX_Series(pDX, this->FluxSeries.grid, this->FluxSeries.v_times, *(this->FluxSeries.vv_props[SELECTED]));
			this->FluxSeries.DDV_SoftValidate();
			this->FluxSeries.DDX_Series(pDX);
			ASSERT(!this->SolutionSeries.grid.IsWindowVisible());
			ASSERT(this->FluxSeries.grid.IsWindowVisible());
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			//this->DDX_Series(pDX, this->SolutionSeries.grid, this->SolutionSeries.v_times, *(this->SolutionSeries.vv_props[SELECTED]), !this->FlowOnly);
			this->SolutionSeries.DDV_SoftValidate();
			this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);
			ASSERT(this->SolutionSeries.grid.IsWindowVisible());
			ASSERT(!this->FluxSeries.grid.IsWindowVisible());
		}
	}
	else
	{
		// face
		//
		switch(this->BC.face)
		{
		case 0: // x
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_X_RADIO);
			break;
		case 1: // y
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_Y_RADIO);
			break;
		case 2: // z
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_Z_RADIO);
			break;
		default: // x
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_X_RADIO);
		}

		if (this->ItemDDX == this->FluxSeries.treeitem)
		{
			//this->DDX_Series(pDX, this->FluxSeries.grid, this->FluxSeries.v_times, *(this->FluxSeries.vv_props[SELECTED]));
			this->FluxSeries.DDX_Series(pDX);
			this->SolutionSeries.grid.ShowWindow(SW_HIDE);
			this->FluxSeries.grid.ShowWindow(SW_SHOW);
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			//this->DDX_Series(pDX, this->SolutionSeries.grid, this->SolutionSeries.v_times, *(this->SolutionSeries.vv_props[SELECTED]), !this->FlowOnly);
			this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);
			this->FluxSeries.grid.ShowWindow(SW_HIDE);
			this->SolutionSeries.grid.ShowWindow(SW_SHOW);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

// COMMENT: {6/25/2009 10:08:22 PM}void CFluxPropsPage2::DDX_Property(CDataExchange* pDX, Cproperty &p)
// COMMENT: {6/25/2009 10:08:22 PM}{
// COMMENT: {6/25/2009 10:08:22 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 10:08:22 PM}	CString strEmpty(_T(""));
// COMMENT: {6/25/2009 10:08:22 PM}
// COMMENT: {6/25/2009 10:08:22 PM}	if (pDX->m_bSaveAndValidate)
// COMMENT: {6/25/2009 10:08:22 PM}	{
// COMMENT: {6/25/2009 10:08:22 PM}		ASSERT(FALSE);
// COMMENT: {6/25/2009 10:08:22 PM}	}
// COMMENT: {6/25/2009 10:08:22 PM}	else
// COMMENT: {6/25/2009 10:08:22 PM}	{
// COMMENT: {6/25/2009 10:08:22 PM}		if (p.type == PROP_UNDEFINED)
// COMMENT: {6/25/2009 10:08:22 PM}		{
// COMMENT: {6/25/2009 10:08:22 PM}		}
// COMMENT: {6/25/2009 10:08:22 PM}		else if (p.type == PROP_FIXED)
// COMMENT: {6/25/2009 10:08:22 PM}		{
// COMMENT: {6/25/2009 10:08:22 PM}			CGlobal::DDX_Text_Safe(pDX, IDC_VALUE_EDIT, p.v[0]);
// COMMENT: {6/25/2009 10:08:22 PM}		}
// COMMENT: {6/25/2009 10:08:22 PM}		else if (p.type == PROP_LINEAR)
// COMMENT: {6/25/2009 10:08:22 PM}		{
// COMMENT: {6/25/2009 10:08:22 PM}			switch (p.coord)
// COMMENT: {6/25/2009 10:08:22 PM}			{
// COMMENT: {6/25/2009 10:08:22 PM}				case 'x':
// COMMENT: {6/25/2009 10:08:22 PM}					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_X_RADIO);
// COMMENT: {6/25/2009 10:08:22 PM}					break;
// COMMENT: {6/25/2009 10:08:22 PM}				case 'y':
// COMMENT: {6/25/2009 10:08:22 PM}					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_Y_RADIO);
// COMMENT: {6/25/2009 10:08:22 PM}					break;
// COMMENT: {6/25/2009 10:08:22 PM}				case 'z':
// COMMENT: {6/25/2009 10:08:22 PM}					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_Z_RADIO);
// COMMENT: {6/25/2009 10:08:22 PM}					break;
// COMMENT: {6/25/2009 10:08:22 PM}				default:
// COMMENT: {6/25/2009 10:08:22 PM}					ASSERT(FALSE);
// COMMENT: {6/25/2009 10:08:22 PM}					break;
// COMMENT: {6/25/2009 10:08:22 PM}			}
// COMMENT: {6/25/2009 10:08:22 PM}			CGlobal::DDX_Text_Safe(pDX, IDC_VALUE1_EDIT,    p.v[0]);
// COMMENT: {6/25/2009 10:08:22 PM}			CGlobal::DDX_Text_Safe(pDX, IDC_DISTANCE1_EDIT, p.dist1);
// COMMENT: {6/25/2009 10:08:22 PM}			CGlobal::DDX_Text_Safe(pDX, IDC_VALUE2_EDIT,    p.v[1]);
// COMMENT: {6/25/2009 10:08:22 PM}			CGlobal::DDX_Text_Safe(pDX, IDC_DISTANCE2_EDIT, p.dist2);
// COMMENT: {6/25/2009 10:08:22 PM}		}
// COMMENT: {6/25/2009 10:08:22 PM}	}
// COMMENT: {6/25/2009 10:08:22 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 10:08:22 PM}}

// COMMENT: {6/25/2009 2:27:43 PM}void CFluxPropsPage2::DDX_Series(CDataExchange* pDX, CModGridCtrlEx &grid, CTimeSeries<Cproperty> &props)
// COMMENT: {6/25/2009 2:27:43 PM}{
// COMMENT: {6/25/2009 2:27:43 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 2:27:43 PM}	ASSERT(!pDX->m_bSaveAndValidate);
// COMMENT: {6/25/2009 2:27:43 PM}
// COMMENT: {6/25/2009 2:27:43 PM}	CCellID cell = grid.GetFocusCell();
// COMMENT: {6/25/2009 2:27:43 PM}	CString s;
// COMMENT: {6/25/2009 2:27:43 PM}	CTimeSeries<Cproperty>::iterator fit = props.begin();
// COMMENT: {6/25/2009 2:27:43 PM}	int row = 1;
// COMMENT: {6/25/2009 2:27:43 PM}	for (; fit != props.end(); ++fit, ++row)
// COMMENT: {6/25/2009 2:27:43 PM}	{
// COMMENT: {6/25/2009 2:27:43 PM}		// time
// COMMENT: {6/25/2009 2:27:43 PM}		if (row == 1)
// COMMENT: {6/25/2009 2:27:43 PM}		{
// COMMENT: {6/25/2009 2:27:43 PM}			ASSERT((*fit).first.value == 0.);
// COMMENT: {6/25/2009 2:27:43 PM}		}
// COMMENT: {6/25/2009 2:27:43 PM}		s.Format(_T("%g"), (*fit).first.value);
// COMMENT: {6/25/2009 2:27:43 PM}		grid.SetItemText(row, TIME_COLUMN, s);
// COMMENT: {6/25/2009 2:27:43 PM}
// COMMENT: {6/25/2009 2:27:43 PM}		// time units
// COMMENT: {6/25/2009 2:27:43 PM}		if ((*fit).first.input && ::strlen((*fit).first.input))
// COMMENT: {6/25/2009 2:27:43 PM}		{
// COMMENT: {6/25/2009 2:27:43 PM}			grid.SetItemText(row, UNIT_COLUMN, (*fit).first.input);
// COMMENT: {6/25/2009 2:27:43 PM}		}
// COMMENT: {6/25/2009 2:27:43 PM}		else
// COMMENT: {6/25/2009 2:27:43 PM}		{
// COMMENT: {6/25/2009 2:27:43 PM}			grid.SetItemText(row, UNIT_COLUMN, this->DefaultUnits);
// COMMENT: {6/25/2009 2:27:43 PM}		}
// COMMENT: {6/25/2009 2:27:43 PM}		if (row == 1)
// COMMENT: {6/25/2009 2:27:43 PM}		{
// COMMENT: {6/25/2009 2:27:43 PM}			grid.DisableCell(row, UNIT_COLUMN);
// COMMENT: {6/25/2009 2:27:43 PM}		}
// COMMENT: {6/25/2009 2:27:43 PM}
// COMMENT: {6/25/2009 2:27:43 PM}		if ((*fit).second.type == PROP_UNDEFINED)
// COMMENT: {6/25/2009 2:27:43 PM}		{
// COMMENT: {6/25/2009 2:27:43 PM}			grid.SetItemText(row, TYPE_COLUMN, PSZ_NONE);
// COMMENT: {6/25/2009 2:27:43 PM}		}
// COMMENT: {6/25/2009 2:27:43 PM}		else if ((*fit).second.type == PROP_FIXED)
// COMMENT: {6/25/2009 2:27:43 PM}		{
// COMMENT: {6/25/2009 2:27:43 PM}			grid.SetItemText(row, TYPE_COLUMN, PSZ_CONSTANT);
// COMMENT: {6/25/2009 2:27:43 PM}		}
// COMMENT: {6/25/2009 2:27:43 PM}		else if ((*fit).second.type == PROP_LINEAR)
// COMMENT: {6/25/2009 2:27:43 PM}		{
// COMMENT: {6/25/2009 2:27:43 PM}			grid.SetItemText(row, TYPE_COLUMN, PSZ_LINEAR);
// COMMENT: {6/25/2009 2:27:43 PM}		}
// COMMENT: {6/25/2009 2:27:43 PM}
// COMMENT: {6/25/2009 2:27:43 PM}		if (row == cell.row)
// COMMENT: {6/25/2009 2:27:43 PM}		{
// COMMENT: {6/25/2009 2:27:43 PM}			this->DDX_Property(pDX, (*fit).second);
// COMMENT: {6/25/2009 2:27:43 PM}		}
// COMMENT: {6/25/2009 2:27:43 PM}	}
// COMMENT: {6/25/2009 2:27:43 PM}
// COMMENT: {6/25/2009 2:27:43 PM}	ASSERT(grid.GetFixedColumnCount() <= cell.col && cell.col < grid.GetColumnCount());
// COMMENT: {6/25/2009 2:27:43 PM}	ASSERT(grid.GetFixedRowCount() <= cell.row && cell.row < grid.GetRowCount());
// COMMENT: {6/25/2009 2:27:43 PM}	
// COMMENT: {6/25/2009 2:27:43 PM}	static NM_GRIDVIEW nmgv;
// COMMENT: {6/25/2009 2:27:43 PM}	nmgv.iRow         = cell.row;
// COMMENT: {6/25/2009 2:27:43 PM}	nmgv.iColumn      = cell.col;
// COMMENT: {6/25/2009 2:27:43 PM}	nmgv.hdr.hwndFrom = grid.GetSafeHwnd();
// COMMENT: {6/25/2009 2:27:43 PM}	nmgv.hdr.idFrom   = grid.GetDlgCtrlID();
// COMMENT: {6/25/2009 2:27:43 PM}	nmgv.hdr.code     = GVN_SELCHANGED;
// COMMENT: {6/25/2009 2:27:43 PM}	ASSERT(::IsWindow(nmgv.hdr.hwndFrom));
// COMMENT: {6/25/2009 2:27:43 PM}
// COMMENT: {6/25/2009 2:27:43 PM}	CWnd *pOwner = grid.GetOwner();
// COMMENT: {6/25/2009 2:27:43 PM}	if (pOwner && ::IsWindow(pOwner->m_hWnd))
// COMMENT: {6/25/2009 2:27:43 PM}	{
// COMMENT: {6/25/2009 2:27:43 PM}		pOwner->PostMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
// COMMENT: {6/25/2009 2:27:43 PM}	}
// COMMENT: {6/25/2009 2:27:43 PM}	grid.RedrawWindow();
// COMMENT: {6/25/2009 2:27:43 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 2:27:43 PM}}


// COMMENT: {6/25/2009 10:08:39 PM}void CFluxPropsPage2::DDX_Series(CDataExchange* pDX, CModGridCtrlEx &grid, std::vector<Ctime*> &times, std::vector<Cproperty*> &props, bool bTimeZeroRequired)
// COMMENT: {6/25/2009 10:08:39 PM}{
// COMMENT: {6/25/2009 10:08:39 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 10:08:39 PM}	if (pDX->m_bSaveAndValidate)
// COMMENT: {6/25/2009 10:08:39 PM}	{
// COMMENT: {6/25/2009 10:08:39 PM}// COMMENT: {6/25/2009 9:22:11 PM}		std::vector<Ctime*> new_times;
// COMMENT: {6/25/2009 10:08:39 PM}// COMMENT: {6/25/2009 9:22:11 PM}		std::vector<Cproperty*> new_props;
// COMMENT: {6/25/2009 10:08:39 PM}		for (int row = grid.GetFixedRowCount(); row < grid.GetRowCount(); ++row)
// COMMENT: {6/25/2009 10:08:39 PM}		{
// COMMENT: {6/25/2009 10:08:39 PM}			CString strValue;
// COMMENT: {6/25/2009 10:08:39 PM}			double value;
// COMMENT: {6/25/2009 10:08:39 PM}			Ctime time;
// COMMENT: {6/25/2009 10:08:39 PM}			time.type = TT_UNITS;
// COMMENT: {6/25/2009 10:08:39 PM}
// COMMENT: {6/25/2009 10:08:39 PM}			strValue = grid.GetItemText(row, TIME_COLUMN);
// COMMENT: {6/25/2009 10:08:39 PM}			if (!strValue.Trim().IsEmpty())
// COMMENT: {6/25/2009 10:08:39 PM}			{
// COMMENT: {6/25/2009 10:08:39 PM}				// time value
// COMMENT: {6/25/2009 10:08:39 PM}				DDX_TextGridControl(pDX, grid.GetDlgCtrlID(), row, TIME_COLUMN, value);
// COMMENT: {6/25/2009 10:08:39 PM}				if (value < 0.0)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					::DDX_GridControlFail(pDX, grid.GetDlgCtrlID(), row, TIME_COLUMN, _T("Start time must be positive."));
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}				time.SetValue(value);
// COMMENT: {6/25/2009 10:08:39 PM}
// COMMENT: {6/25/2009 10:08:39 PM}				// time units
// COMMENT: {6/25/2009 10:08:39 PM}				DDX_TextGridControl(pDX, grid.GetDlgCtrlID(), row, UNIT_COLUMN, strValue);
// COMMENT: {6/25/2009 10:08:39 PM}				if (!strValue.IsEmpty() && time.SetInput(strValue) != OK)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					if (row != 1)
// COMMENT: {6/25/2009 10:08:39 PM}					{
// COMMENT: {6/25/2009 10:08:39 PM}						::DDX_GridControlFail(pDX, grid.GetDlgCtrlID(), row, UNIT_COLUMN, _T("Please enter the start time units."));
// COMMENT: {6/25/2009 10:08:39 PM}					}
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}
// COMMENT: {6/25/2009 10:08:39 PM}				// prop
// COMMENT: {6/25/2009 10:08:39 PM}				DDX_TextGridControl(pDX, grid.GetDlgCtrlID(), row, TYPE_COLUMN, strValue);
// COMMENT: {6/25/2009 10:08:39 PM}				if (strValue.Trim().IsEmpty() || strValue.CompareNoCase(PSZ_NONE) == 0)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					if (row == 1 && bTimeZeroRequired)
// COMMENT: {6/25/2009 10:08:39 PM}					{
// COMMENT: {6/25/2009 10:08:39 PM}						CString string("A property must be defined for time zero.");
// COMMENT: {6/25/2009 10:08:39 PM}						::DDX_GridControlFail(pDX, grid.GetDlgCtrlID(), row, TYPE_COLUMN, string);
// COMMENT: {6/25/2009 10:08:39 PM}					}
// COMMENT: {6/25/2009 10:08:39 PM}					// (*vectors[SELECTED])[row]
// COMMENT: {6/25/2009 10:08:39 PM}					/////delete (*vectors[SELECTED])[row];
// COMMENT: {6/25/2009 10:08:39 PM}					////(*vectors[SELECTED])[row] = 0;
// COMMENT: {6/25/2009 10:08:39 PM}					ASSERT(!props[row]);
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}				else if (strValue.CompareNoCase(PSZ_CONSTANT) == 0)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					CFluxPropsPage2::ModeType mt = CFluxPropsPage2::CONSTANT;
// COMMENT: {6/25/2009 10:08:39 PM}					if (props[row])
// COMMENT: {6/25/2009 10:08:39 PM}					{
// COMMENT: {6/25/2009 10:08:39 PM}						if (props[row]->v[0] != props[row]->v[0])
// COMMENT: {6/25/2009 10:08:39 PM}						{
// COMMENT: {6/25/2009 10:08:39 PM}							double d;
// COMMENT: {6/25/2009 10:08:39 PM}							grid.SetCurrentFocusCell(row, TYPE_COLUMN);
// COMMENT: {6/25/2009 10:08:39 PM}							::DDX_Text(pDX, IDC_VALUE_EDIT, d);
// COMMENT: {6/25/2009 10:08:39 PM}						}
// COMMENT: {6/25/2009 10:08:39 PM}					}
// COMMENT: {6/25/2009 10:08:39 PM}					else
// COMMENT: {6/25/2009 10:08:39 PM}					{
// COMMENT: {6/25/2009 10:08:39 PM}						ASSERT(FALSE);
// COMMENT: {6/25/2009 10:08:39 PM}					}
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}				else if (strValue.CompareNoCase(PSZ_LINEAR) == 0)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					CFluxPropsPage2::ModeType mt = CFluxPropsPage2::LINEAR;
// COMMENT: {6/25/2009 10:08:39 PM}					if (props[row])
// COMMENT: {6/25/2009 10:08:39 PM}					{
// COMMENT: {6/25/2009 10:08:39 PM}						double d;
// COMMENT: {6/25/2009 10:08:39 PM}						if (props[row]->v[0] != props[row]->v[0])
// COMMENT: {6/25/2009 10:08:39 PM}						{
// COMMENT: {6/25/2009 10:08:39 PM}							//[[this->SetCurrentFocusCell(grid, row, TYPE_COLUMN);
// COMMENT: {6/25/2009 10:08:39 PM}							::DDX_Text(pDX, IDC_VALUE1_EDIT, d);
// COMMENT: {6/25/2009 10:08:39 PM}						}
// COMMENT: {6/25/2009 10:08:39 PM}						if (props[row]->dist1 != props[row]->dist1)
// COMMENT: {6/25/2009 10:08:39 PM}						{
// COMMENT: {6/25/2009 10:08:39 PM}							//[[this->SetCurrentFocusCell(grid, row, TYPE_COLUMN);
// COMMENT: {6/25/2009 10:08:39 PM}							::DDX_Text(pDX, IDC_DISTANCE1_EDIT, d);
// COMMENT: {6/25/2009 10:08:39 PM}						}
// COMMENT: {6/25/2009 10:08:39 PM}						if (props[row]->v[1] != props[row]->v[1])
// COMMENT: {6/25/2009 10:08:39 PM}						{
// COMMENT: {6/25/2009 10:08:39 PM}							//[[this->SetCurrentFocusCell(grid, row, TYPE_COLUMN);
// COMMENT: {6/25/2009 10:08:39 PM}							::DDX_Text(pDX, IDC_VALUE2_EDIT, d);
// COMMENT: {6/25/2009 10:08:39 PM}						}
// COMMENT: {6/25/2009 10:08:39 PM}						if (props[row]->dist2 != props[row]->dist2)
// COMMENT: {6/25/2009 10:08:39 PM}						{
// COMMENT: {6/25/2009 10:08:39 PM}							//[[this->SetCurrentFocusCell(grid, row, TYPE_COLUMN);
// COMMENT: {6/25/2009 10:08:39 PM}							::DDX_Text(pDX, IDC_DISTANCE2_EDIT, d);
// COMMENT: {6/25/2009 10:08:39 PM}						}
// COMMENT: {6/25/2009 10:08:39 PM}					}
// COMMENT: {6/25/2009 10:08:39 PM}					else
// COMMENT: {6/25/2009 10:08:39 PM}					{
// COMMENT: {6/25/2009 10:08:39 PM}						ASSERT(FALSE);
// COMMENT: {6/25/2009 10:08:39 PM}					}
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}				else if (strValue.CompareNoCase(PSZ_POINTS) == 0)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					CFluxPropsPage2::ModeType mt = CFluxPropsPage2::POINTS;
// COMMENT: {6/25/2009 10:08:39 PM}					// (*vectors[SELECTED])[row]
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}				else if (strValue.CompareNoCase(PSZ_XYZ) == 0)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					CFluxPropsPage2::ModeType mt = CFluxPropsPage2::XYZ;
// COMMENT: {6/25/2009 10:08:39 PM}					// (*vectors[SELECTED])[row]
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}				//{{{{
// COMMENT: {6/25/2009 10:08:39 PM}				delete times[row];
// COMMENT: {6/25/2009 10:08:39 PM}				times[row] = new Ctime(time);
// COMMENT: {6/25/2009 10:08:39 PM}				//}}}}
// COMMENT: {6/25/2009 10:08:39 PM}			}
// COMMENT: {6/25/2009 10:08:39 PM}			////{{
// COMMENT: {6/25/2009 10:08:39 PM}			else
// COMMENT: {6/25/2009 10:08:39 PM}			{
// COMMENT: {6/25/2009 10:08:39 PM}				delete times[row];
// COMMENT: {6/25/2009 10:08:39 PM}				times[row] = 0;
// COMMENT: {6/25/2009 10:08:39 PM}			}
// COMMENT: {6/25/2009 10:08:39 PM}			////}}
// COMMENT: {6/25/2009 10:08:39 PM}		}
// COMMENT: {6/25/2009 10:08:39 PM}	}
// COMMENT: {6/25/2009 10:08:39 PM}	else
// COMMENT: {6/25/2009 10:08:39 PM}	{
// COMMENT: {6/25/2009 10:08:39 PM}		CString s;
// COMMENT: {6/25/2009 10:08:39 PM}		CCellID cell = grid.GetFocusCell();
// COMMENT: {6/25/2009 10:08:39 PM}		ASSERT(times.size() == props.size());
// COMMENT: {6/25/2009 10:08:39 PM}		for (int row = grid.GetFixedRowCount(); row < grid.GetRowCount(); ++row)
// COMMENT: {6/25/2009 10:08:39 PM}		{
// COMMENT: {6/25/2009 10:08:39 PM}			// time
// COMMENT: {6/25/2009 10:08:39 PM}			if (times[row])
// COMMENT: {6/25/2009 10:08:39 PM}			{
// COMMENT: {6/25/2009 10:08:39 PM}				if (row == 1)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					ASSERT(times[row]->value == 0.);
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}
// COMMENT: {6/25/2009 10:08:39 PM}				// time value
// COMMENT: {6/25/2009 10:08:39 PM}				s.Format(_T("%g"), times[row]->value);
// COMMENT: {6/25/2009 10:08:39 PM}				grid.SetItemText(row, TIME_COLUMN, s);
// COMMENT: {6/25/2009 10:08:39 PM}
// COMMENT: {6/25/2009 10:08:39 PM}				// time units
// COMMENT: {6/25/2009 10:08:39 PM}				if (times[row]->input && ::strlen(times[row]->input))
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					grid.SetItemText(row, UNIT_COLUMN, times[row]->input);
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}				else
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					grid.SetItemText(row, UNIT_COLUMN, this->DefaultUnits);
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}			}
// COMMENT: {6/25/2009 10:08:39 PM}
// COMMENT: {6/25/2009 10:08:39 PM}			if (row == 1)
// COMMENT: {6/25/2009 10:08:39 PM}			{
// COMMENT: {6/25/2009 10:08:39 PM}				grid.DisableCell(row, UNIT_COLUMN);
// COMMENT: {6/25/2009 10:08:39 PM}			}
// COMMENT: {6/25/2009 10:08:39 PM}
// COMMENT: {6/25/2009 10:08:39 PM}			// prop
// COMMENT: {6/25/2009 10:08:39 PM}			if (props[row])
// COMMENT: {6/25/2009 10:08:39 PM}			{
// COMMENT: {6/25/2009 10:08:39 PM}				// prop type
// COMMENT: {6/25/2009 10:08:39 PM}				if (props[row]->type == PROP_UNDEFINED)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					grid.SetItemText(row, TYPE_COLUMN, PSZ_NONE);
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}				else if (props[row]->type == PROP_FIXED)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					grid.SetItemText(row, TYPE_COLUMN, PSZ_CONSTANT);
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}				else if (props[row]->type == PROP_LINEAR)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					grid.SetItemText(row, TYPE_COLUMN, PSZ_LINEAR);
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}
// COMMENT: {6/25/2009 10:08:39 PM}				if (row == cell.row)
// COMMENT: {6/25/2009 10:08:39 PM}				{
// COMMENT: {6/25/2009 10:08:39 PM}					this->DDX_Property(pDX, *props[row]);
// COMMENT: {6/25/2009 10:08:39 PM}				}
// COMMENT: {6/25/2009 10:08:39 PM}			}
// COMMENT: {6/25/2009 10:08:39 PM}		}
// COMMENT: {6/25/2009 10:08:39 PM}
// COMMENT: {6/25/2009 10:08:39 PM}		ASSERT(grid.GetFixedColumnCount() <= cell.col && cell.col < grid.GetColumnCount());
// COMMENT: {6/25/2009 10:08:39 PM}		ASSERT(grid.GetFixedRowCount() <= cell.row && cell.row < grid.GetRowCount());
// COMMENT: {6/25/2009 10:08:39 PM}		
// COMMENT: {6/25/2009 10:08:39 PM}		static NM_GRIDVIEW nmgv;
// COMMENT: {6/25/2009 10:08:39 PM}		nmgv.iRow         = cell.row;
// COMMENT: {6/25/2009 10:08:39 PM}		nmgv.iColumn      = cell.col;
// COMMENT: {6/25/2009 10:08:39 PM}		nmgv.hdr.hwndFrom = grid.GetSafeHwnd();
// COMMENT: {6/25/2009 10:08:39 PM}		nmgv.hdr.idFrom   = grid.GetDlgCtrlID();
// COMMENT: {6/25/2009 10:08:39 PM}		nmgv.hdr.code     = GVN_SELCHANGED;
// COMMENT: {6/25/2009 10:08:39 PM}		ASSERT(::IsWindow(nmgv.hdr.hwndFrom));
// COMMENT: {6/25/2009 10:08:39 PM}
// COMMENT: {6/25/2009 10:08:39 PM}		CWnd *pOwner = grid.GetOwner();
// COMMENT: {6/25/2009 10:08:39 PM}		if (pOwner && ::IsWindow(pOwner->m_hWnd))
// COMMENT: {6/25/2009 10:08:39 PM}		{
// COMMENT: {6/25/2009 10:08:39 PM}			pOwner->PostMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
// COMMENT: {6/25/2009 10:08:39 PM}		}
// COMMENT: {6/25/2009 10:08:39 PM}		grid.RedrawWindow();
// COMMENT: {6/25/2009 10:08:39 PM}	}
// COMMENT: {6/25/2009 10:08:39 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 10:08:39 PM}}


// COMMENT: {6/16/2009 4:10:12 PM}#define NEW_PROP(P) \
// COMMENT: {6/16/2009 4:10:12 PM}do { \
// COMMENT: {6/16/2009 4:10:12 PM}	if (P == 0) { \
// COMMENT: {6/16/2009 4:10:12 PM}		P = new Cproperty(); \
// COMMENT: {6/16/2009 4:10:12 PM}	} \
// COMMENT: {6/16/2009 4:10:12 PM}} while (0)

void CFluxPropsPage2::SetProperties(const CBC& rBC)
{
	TRACE("In %s\n", __FUNCTION__);
	// copy grid_elt
	this->BC = rBC;

	//[[
	this->FluxSeries.SetSeries(rBC.m_bc_flux);
	this->SolutionSeries.SetSeries(rBC.m_bc_solution);
	//]]

// COMMENT: {6/25/2009 7:36:47 PM}	this->FreeVectors();
// COMMENT: {6/25/2009 7:36:47 PM}
// COMMENT: {6/25/2009 7:36:47 PM}	this->FluxSeries.v_times.resize(MIN_ROWS + 1, 0);
// COMMENT: {6/25/2009 7:36:47 PM}	this->FluxVector.resize(MIN_ROWS + 1, 0);
// COMMENT: {6/25/2009 7:36:47 PM}	this->FluxVectorConstant.resize(MIN_ROWS + 1, 0);
// COMMENT: {6/25/2009 7:36:47 PM}	this->FluxVectorLinear.resize(MIN_ROWS + 1, 0);
// COMMENT: {6/25/2009 7:36:47 PM}
// COMMENT: {6/25/2009 7:36:47 PM}	CTimeSeries<Cproperty>::const_iterator fit = rBC.m_bc_flux.begin();
// COMMENT: {6/25/2009 7:36:47 PM}	for (int row = 1; fit != rBC.m_bc_flux.end(); ++fit, ++row)
// COMMENT: {6/25/2009 7:36:47 PM}	{
// COMMENT: {6/25/2009 7:36:47 PM}// COMMENT: {6/25/2009 7:18:57 PM}		// Ctime
// COMMENT: {6/25/2009 7:36:47 PM}// COMMENT: {6/25/2009 7:18:57 PM}		this->FluxSeries.v_times[row] = new Ctime((*fit).first);
// COMMENT: {6/25/2009 7:36:47 PM}
// COMMENT: {6/25/2009 7:36:47 PM}		// Cproperty
// COMMENT: {6/25/2009 7:36:47 PM}		if ((*fit).second.type == PROP_UNDEFINED)
// COMMENT: {6/25/2009 7:36:47 PM}		{
// COMMENT: {6/25/2009 7:36:47 PM}			ASSERT(FALSE);
// COMMENT: {6/25/2009 7:36:47 PM}		}
// COMMENT: {6/25/2009 7:36:47 PM}		else if ((*fit).second.type == PROP_FIXED)
// COMMENT: {6/25/2009 7:36:47 PM}		{
// COMMENT: {6/25/2009 7:36:47 PM}			this->FluxVector[row] = new Cproperty((*fit).second);
// COMMENT: {6/25/2009 7:36:47 PM}			this->FluxVectorConstant[row] = new Cproperty((*fit).second);
// COMMENT: {6/25/2009 7:36:47 PM}		}
// COMMENT: {6/25/2009 7:36:47 PM}		else if ((*fit).second.type == PROP_LINEAR)
// COMMENT: {6/25/2009 7:36:47 PM}		{
// COMMENT: {6/25/2009 7:36:47 PM}			this->FluxVector[row] = new Cproperty((*fit).second);
// COMMENT: {6/25/2009 7:36:47 PM}			this->FluxVectorLinear[row] = new Cproperty((*fit).second);
// COMMENT: {6/25/2009 7:36:47 PM}		}
// COMMENT: {6/25/2009 7:36:47 PM}	}
// COMMENT: {6/25/2009 7:36:47 PM}
// COMMENT: {6/25/2009 7:36:47 PM}	this->SolutionSeries.v_times.resize(MIN_ROWS + 1, 0);
// COMMENT: {6/25/2009 7:36:47 PM}	this->SolutionVector.resize(MIN_ROWS + 1, 0);
// COMMENT: {6/25/2009 7:36:47 PM}	this->SolutionVectorConstant.resize(MIN_ROWS + 1, 0);
// COMMENT: {6/25/2009 7:36:47 PM}	this->SolutionVectorLinear.resize(MIN_ROWS + 1, 0);
// COMMENT: {6/25/2009 7:36:47 PM}
// COMMENT: {6/25/2009 7:36:47 PM}	CTimeSeries<Cproperty>::const_iterator sit = rBC.m_bc_solution.begin();
// COMMENT: {6/25/2009 7:36:47 PM}	for (int row = 1; sit != rBC.m_bc_solution.end(); ++sit, ++row)
// COMMENT: {6/25/2009 7:36:47 PM}	{
// COMMENT: {6/25/2009 7:36:47 PM}// COMMENT: {6/25/2009 7:19:02 PM}		// Ctime
// COMMENT: {6/25/2009 7:36:47 PM}// COMMENT: {6/25/2009 7:19:02 PM}		this->SolutionSeries.v_times[row] = new Ctime((*sit).first);
// COMMENT: {6/25/2009 7:36:47 PM}
// COMMENT: {6/25/2009 7:36:47 PM}		// Cproperty
// COMMENT: {6/25/2009 7:36:47 PM}		if ((*sit).second.type == PROP_UNDEFINED)
// COMMENT: {6/25/2009 7:36:47 PM}		{
// COMMENT: {6/25/2009 7:36:47 PM}			ASSERT(FALSE);
// COMMENT: {6/25/2009 7:36:47 PM}		}
// COMMENT: {6/25/2009 7:36:47 PM}		else if ((*sit).second.type == PROP_FIXED)
// COMMENT: {6/25/2009 7:36:47 PM}		{
// COMMENT: {6/25/2009 7:36:47 PM}			this->SolutionVector[row] = new Cproperty((*sit).second);
// COMMENT: {6/25/2009 7:36:47 PM}			this->SolutionVectorConstant[row] = new Cproperty((*sit).second);
// COMMENT: {6/25/2009 7:36:47 PM}		}
// COMMENT: {6/25/2009 7:36:47 PM}		else if ((*sit).second.type == PROP_LINEAR)
// COMMENT: {6/25/2009 7:36:47 PM}		{
// COMMENT: {6/25/2009 7:36:47 PM}			this->SolutionVector[row] = new Cproperty((*sit).second);
// COMMENT: {6/25/2009 7:36:47 PM}			this->SolutionVectorLinear[row] = new Cproperty((*sit).second);
// COMMENT: {6/25/2009 7:36:47 PM}		}
// COMMENT: {6/25/2009 7:36:47 PM}	}

	TRACE("Out %s\n", __FUNCTION__);
}

// COMMENT: {6/24/2009 5:48:52 PM}#define DELETE_PROP(P) \
// COMMENT: {6/24/2009 5:48:52 PM}do { \
// COMMENT: {6/24/2009 5:48:52 PM}	if (P->type == PROP_UNDEFINED) { \
// COMMENT: {6/24/2009 5:48:52 PM}		Cproperty::CopyProperty(&P, 0); \
// COMMENT: {6/24/2009 5:48:52 PM}	} \
// COMMENT: {6/24/2009 5:48:52 PM}} while (0)

void CFluxPropsPage2::GetProperties(CBC& rBC)const
{
	TRACE("In %s\n", __FUNCTION__);
	rBC = this->BC;

	rBC.m_bc_flux.clear();
	rBC.m_bc_solution.clear();

	//std::vector<Ctime*>::const_iterator ti;
	//std::vector<Cproperty*>::const_iterator ti;

	for (size_t row = 1; row < this->FluxSeries.v_times.size(); ++row)
	{
		if (this->FluxSeries.v_times[row] && (*this->FluxSeries.vv_props[SELECTED])[row])
		{
			Ctime t(*this->FluxSeries.v_times[row]);
			Cproperty p(*(*this->FluxSeries.vv_props[SELECTED])[row]);
			rBC.m_bc_flux[t] = p;
		}
	}

	for (size_t row = 1; row < this->SolutionSeries.v_times.size(); ++row)
	{
		if (this->SolutionSeries.v_times[row] && (*this->SolutionSeries.vv_props[SELECTED])[row])
		{
			Ctime t(*this->SolutionSeries.v_times[row]);
			Cproperty p(*(*this->SolutionSeries.vv_props[SELECTED])[row]);
			rBC.m_bc_solution[t] = p;
		}
	}



// COMMENT: {6/24/2009 7:15:10 PM}	ti = this->FluxSeries.v_times.begin();
// COMMENT: {6/24/2009 7:15:10 PM}	this->FluxVector[SELECTED].begin();

// COMMENT: {6/24/2009 7:07:52 PM}	std::vector<Ctime*>::iterator ti = 
// COMMENT: {6/24/2009 7:07:52 PM}	for (		this->FluxSeries.v_times[row] = new Ctime((*fit).first);
// COMMENT: {6/24/2009 7:07:52 PM}
// COMMENT: {6/24/2009 7:07:52 PM}
// COMMENT: {6/24/2009 7:07:52 PM}	rBC.m_bc_flux[t] = p;

// COMMENT: {6/24/2009 5:48:57 PM}	// delete inactive properties
// COMMENT: {6/24/2009 5:48:57 PM}	//
// COMMENT: {6/24/2009 5:48:57 PM}	DELETE_PROP(rBC.current_bc_flux);
// COMMENT: {6/24/2009 5:48:57 PM}	DELETE_PROP(rBC.current_bc_solution);
// COMMENT: {6/24/2009 5:48:57 PM}	TRACE("Out %s\n", __FUNCTION__);
}

BEGIN_MESSAGE_MAP(CFluxPropsPage2, CPropertyPage)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_TIMES_GRID, OnEndLabelEditFlux)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_SOLUTION, OnEndLabelEditSolution)

	ON_NOTIFY(GVN_SELCHANGED, IDC_TIMES_GRID,    OnSelChangedFlux)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_SOLUTION, OnSelChangedSolution)

	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGED,  IDC_PROP_TREE, OnTreeSelChanged)
	ON_EN_SETFOCUS(IDC_DESC_EDIT, &CFluxPropsPage2::OnEnSetfocusDescEdit)

	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
END_MESSAGE_MAP()


// CFluxPropsPage2 message handlers

// COMMENT: {6/25/2009 8:36:27 PM}void CFluxPropsPage2::SetMode(CFluxPropsPage2::ModeType mode)
// COMMENT: {6/25/2009 8:36:27 PM}{
// COMMENT: {6/25/2009 8:36:27 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:36:27 PM}	CString time;
// COMMENT: {6/25/2009 8:36:27 PM}	CString units;
// COMMENT: {6/25/2009 8:36:27 PM}
// COMMENT: {6/25/2009 8:36:27 PM}	if (this->ItemDDX == this->FluxSeries.treeitem)
// COMMENT: {6/25/2009 8:36:27 PM}	{
// COMMENT: {6/25/2009 8:36:27 PM}		CCellID id = this->FluxSeries.grid.GetFocusCell();
// COMMENT: {6/25/2009 8:36:27 PM}		time = this->FluxSeries.grid.GetItemText(id.row, TIME_COLUMN);
// COMMENT: {6/25/2009 8:36:27 PM}		units = this->FluxSeries.grid.GetItemText(id.row, UNIT_COLUMN);
// COMMENT: {6/25/2009 8:36:27 PM}	}
// COMMENT: {6/25/2009 8:36:27 PM}	else if (this->ItemDDX == this->SolutionSeries.treeitem)
// COMMENT: {6/25/2009 8:36:27 PM}	{
// COMMENT: {6/25/2009 8:36:27 PM}		CCellID id = this->SolutionSeries.grid.GetFocusCell();
// COMMENT: {6/25/2009 8:36:27 PM}		time = this->SolutionSeries.grid.GetItemText(id.row, TIME_COLUMN);
// COMMENT: {6/25/2009 8:36:27 PM}		units = this->SolutionSeries.grid.GetItemText(id.row, UNIT_COLUMN);
// COMMENT: {6/25/2009 8:36:27 PM}	}
// COMMENT: {6/25/2009 8:36:27 PM}
// COMMENT: {6/25/2009 8:36:27 PM}	if (!units.Trim().IsEmpty() && !time.Trim().IsEmpty())
// COMMENT: {6/25/2009 8:36:27 PM}	{
// COMMENT: {6/25/2009 8:36:27 PM}		CString str;
// COMMENT: {6/25/2009 8:36:27 PM}		str.Format("%s %s", time, units);
// COMMENT: {6/25/2009 8:36:27 PM}		time = str;
// COMMENT: {6/25/2009 8:36:27 PM}	}
// COMMENT: {6/25/2009 8:36:27 PM}
// COMMENT: {6/25/2009 8:36:27 PM}	if (mode == CFluxPropsPage2::NONE)
// COMMENT: {6/25/2009 8:36:27 PM}	{
// COMMENT: {6/25/2009 8:36:27 PM}		this->PropertyGroupBox.SetWindowText(_T(""));
// COMMENT: {6/25/2009 8:36:27 PM}		this->ShowConstant(false);
// COMMENT: {6/25/2009 8:36:27 PM}		this->ShowLinear(false);
// COMMENT: {6/25/2009 8:36:27 PM}	}
// COMMENT: {6/25/2009 8:36:27 PM}	else if (mode == CFluxPropsPage2::CONSTANT)
// COMMENT: {6/25/2009 8:36:27 PM}	{
// COMMENT: {6/25/2009 8:36:27 PM}		CString str;
// COMMENT: {6/25/2009 8:36:27 PM}		if (time.Trim().IsEmpty())
// COMMENT: {6/25/2009 8:36:27 PM}		{
// COMMENT: {6/25/2009 8:36:27 PM}			str.Format("%s", PSZ_CONSTANT);
// COMMENT: {6/25/2009 8:36:27 PM}		}
// COMMENT: {6/25/2009 8:36:27 PM}		else
// COMMENT: {6/25/2009 8:36:27 PM}		{
// COMMENT: {6/25/2009 8:36:27 PM}			str.Format("%s (t=%s)", PSZ_CONSTANT, time);
// COMMENT: {6/25/2009 8:36:27 PM}		}
// COMMENT: {6/25/2009 8:36:27 PM}		this->PropertyGroupBox.SetWindowText(str);
// COMMENT: {6/25/2009 8:36:27 PM}		this->ShowConstant(true);
// COMMENT: {6/25/2009 8:36:27 PM}		this->ShowLinear(false);
// COMMENT: {6/25/2009 8:36:27 PM}	}
// COMMENT: {6/25/2009 8:36:27 PM}	else if (mode == CFluxPropsPage2::LINEAR)
// COMMENT: {6/25/2009 8:36:27 PM}	{
// COMMENT: {6/25/2009 8:36:27 PM}		CString str;
// COMMENT: {6/25/2009 8:36:27 PM}		if (time.Trim().IsEmpty())
// COMMENT: {6/25/2009 8:36:27 PM}		{
// COMMENT: {6/25/2009 8:36:27 PM}			str.Format("%s", PSZ_LINEAR);
// COMMENT: {6/25/2009 8:36:27 PM}		}
// COMMENT: {6/25/2009 8:36:27 PM}		else
// COMMENT: {6/25/2009 8:36:27 PM}		{
// COMMENT: {6/25/2009 8:36:27 PM}			str.Format("%s (t=%s)", PSZ_LINEAR, time);
// COMMENT: {6/25/2009 8:36:27 PM}		}
// COMMENT: {6/25/2009 8:36:27 PM}		this->PropertyGroupBox.SetWindowText(str);
// COMMENT: {6/25/2009 8:36:27 PM}		this->ShowConstant(false);
// COMMENT: {6/25/2009 8:36:27 PM}		this->ShowLinear(true);
// COMMENT: {6/25/2009 8:36:27 PM}	}
// COMMENT: {6/25/2009 8:36:27 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:36:27 PM}}

// COMMENT: {6/25/2009 8:36:32 PM}void CFluxPropsPage2::ShowConstant(bool show)
// COMMENT: {6/25/2009 8:36:32 PM}{
// COMMENT: {6/25/2009 8:36:32 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:36:32 PM}	if (this->ItemDDX == this->FluxSeries.treeitem)
// COMMENT: {6/25/2009 8:36:32 PM}	{
// COMMENT: {6/25/2009 8:36:32 PM}		 this->FluxSeries.ShowConstant(show);
// COMMENT: {6/25/2009 8:36:32 PM}	}
// COMMENT: {6/25/2009 8:36:32 PM}	else if (this->ItemDDX == this->SolutionSeries.treeitem)
// COMMENT: {6/25/2009 8:36:32 PM}	{
// COMMENT: {6/25/2009 8:36:32 PM}		 this->SolutionSeries.ShowConstant(show);
// COMMENT: {6/25/2009 8:36:32 PM}	}
// COMMENT: {6/25/2009 8:36:32 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:36:32 PM}}

// COMMENT: {6/25/2009 8:36:37 PM}void CFluxPropsPage2::ShowLinear(bool show)
// COMMENT: {6/25/2009 8:36:37 PM}{
// COMMENT: {6/25/2009 8:36:37 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:36:37 PM}	if (this->ItemDDX == this->FluxSeries.treeitem)
// COMMENT: {6/25/2009 8:36:37 PM}	{
// COMMENT: {6/25/2009 8:36:37 PM}		 this->FluxSeries.ShowLinear(show);
// COMMENT: {6/25/2009 8:36:37 PM}	}
// COMMENT: {6/25/2009 8:36:37 PM}	else if (this->ItemDDX == this->SolutionSeries.treeitem)
// COMMENT: {6/25/2009 8:36:37 PM}	{
// COMMENT: {6/25/2009 8:36:37 PM}		 this->SolutionSeries.ShowLinear(show);
// COMMENT: {6/25/2009 8:36:37 PM}	}
// COMMENT: {6/25/2009 8:36:37 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:36:37 PM}}

void CFluxPropsPage2::OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:32:22 PM}	this->OnEndLabelEdit(pNotifyStruct, result, this->FluxSeries.grid, this->FluxSeries.vv_props);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->FluxSeries.OnEndLabelEdit(pnmgv->iRow, pnmgv->iColumn);
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:33:31 PM}	this->OnEndLabelEdit(pNotifyStruct, result, this->SolutionSeries.grid, this->SolutionSeries.vv_props);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->SolutionSeries.OnEndLabelEdit(pnmgv->iRow, pnmgv->iColumn);
	TRACE("Out %s\n", __FUNCTION__);
}

// COMMENT: {6/25/2009 8:34:18 PM}void CFluxPropsPage2::OnEndLabelEdit(NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors)
// COMMENT: {6/25/2009 8:34:18 PM}{
// COMMENT: {6/25/2009 8:34:18 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:34:18 PM}	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
// COMMENT: {6/25/2009 8:34:18 PM}	CString str = grid.GetItemText(pnmgv->iRow, pnmgv->iColumn);
// COMMENT: {6/25/2009 8:34:18 PM}
// COMMENT: {6/25/2009 8:34:18 PM}	// Add your control notification handler code here
// COMMENT: {6/25/2009 8:34:18 PM}	if (grid.GetSafeHwnd())
// COMMENT: {6/25/2009 8:34:18 PM}	{
// COMMENT: {6/25/2009 8:34:18 PM}		if (pnmgv->iColumn == TIME_COLUMN && !str.Trim().IsEmpty())
// COMMENT: {6/25/2009 8:34:18 PM}		{
// COMMENT: {6/25/2009 8:34:18 PM}			CString units = grid.GetItemText(pnmgv->iRow, UNIT_COLUMN);
// COMMENT: {6/25/2009 8:34:18 PM}			if (units.Trim().IsEmpty())
// COMMENT: {6/25/2009 8:34:18 PM}			{
// COMMENT: {6/25/2009 8:34:18 PM}				// look upwards to find default units
// COMMENT: {6/25/2009 8:34:18 PM}				CString u = this->DefaultUnits;
// COMMENT: {6/25/2009 8:34:18 PM}				for (int r = pnmgv->iRow - 1; r >= 0; --r)
// COMMENT: {6/25/2009 8:34:18 PM}				{
// COMMENT: {6/25/2009 8:34:18 PM}					CString d = grid.GetItemText(r, UNIT_COLUMN);
// COMMENT: {6/25/2009 8:34:18 PM}					if (!d.IsEmpty() && CGlobal::GetStdTimeUnitsSafe(d).size() > 0)
// COMMENT: {6/25/2009 8:34:18 PM}					{
// COMMENT: {6/25/2009 8:34:18 PM}						u = CGlobal::GetStdTimeUnitsSafe(d).c_str();
// COMMENT: {6/25/2009 8:34:18 PM}						break;
// COMMENT: {6/25/2009 8:34:18 PM}					}					
// COMMENT: {6/25/2009 8:34:18 PM}				}
// COMMENT: {6/25/2009 8:34:18 PM}				grid.SetItemText(pnmgv->iRow, UNIT_COLUMN, u);
// COMMENT: {6/25/2009 8:34:18 PM}			}
// COMMENT: {6/25/2009 8:34:18 PM}
// COMMENT: {6/25/2009 8:34:18 PM}			CString type = grid.GetItemText(pnmgv->iRow, TYPE_COLUMN);
// COMMENT: {6/25/2009 8:34:18 PM}			if (type.Trim().IsEmpty())
// COMMENT: {6/25/2009 8:34:18 PM}			{
// COMMENT: {6/25/2009 8:34:18 PM}				// look upwards to find default type
// COMMENT: {6/25/2009 8:34:18 PM}				for (int r = pnmgv->iRow - 1; r > 0; --r)
// COMMENT: {6/25/2009 8:34:18 PM}				{
// COMMENT: {6/25/2009 8:34:18 PM}					CString d = grid.GetItemText(r, TYPE_COLUMN);
// COMMENT: {6/25/2009 8:34:18 PM}
// COMMENT: {6/25/2009 8:34:18 PM}					if (d.CompareNoCase(PSZ_NONE) == 0)
// COMMENT: {6/25/2009 8:34:18 PM}					{
// COMMENT: {6/25/2009 8:34:18 PM}						grid.SetItemText(pnmgv->iRow, TYPE_COLUMN, d);
// COMMENT: {6/25/2009 8:34:18 PM}						this->SetMode(CFluxPropsPage2::NONE);
// COMMENT: {6/25/2009 8:34:18 PM}						break;
// COMMENT: {6/25/2009 8:34:18 PM}					}
// COMMENT: {6/25/2009 8:34:18 PM}					else if (d.CompareNoCase(PSZ_CONSTANT) == 0)
// COMMENT: {6/25/2009 8:34:18 PM}					{
// COMMENT: {6/25/2009 8:34:18 PM}						CFluxPropsPage2::ModeType mt = CFluxPropsPage2::CONSTANT;
// COMMENT: {6/25/2009 8:34:18 PM}						grid.SetItemText(pnmgv->iRow, TYPE_COLUMN, d);
// COMMENT: {6/25/2009 8:34:18 PM}						if (!(*vectors[mt])[pnmgv->iRow])
// COMMENT: {6/25/2009 8:34:18 PM}						{
// COMMENT: {6/25/2009 8:34:18 PM}							(*vectors[mt])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}						}
// COMMENT: {6/25/2009 8:34:18 PM}						delete (*vectors[SELECTED])[pnmgv->iRow];
// COMMENT: {6/25/2009 8:34:18 PM}						(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}						//{{
// COMMENT: {6/25/2009 8:34:18 PM}						CDataExchange dx(this, FALSE);
// COMMENT: {6/25/2009 8:34:18 PM}						this->DDX_Property(&dx, *(*vectors[SELECTED])[pnmgv->iRow]);
// COMMENT: {6/25/2009 8:34:18 PM}						//}}
// COMMENT: {6/25/2009 8:34:18 PM}						this->SetMode(mt);
// COMMENT: {6/25/2009 8:34:18 PM}						break;
// COMMENT: {6/25/2009 8:34:18 PM}					}
// COMMENT: {6/25/2009 8:34:18 PM}					else if (d.CompareNoCase(PSZ_LINEAR) == 0)
// COMMENT: {6/25/2009 8:34:18 PM}					{
// COMMENT: {6/25/2009 8:34:18 PM}						CFluxPropsPage2::ModeType mt = CFluxPropsPage2::LINEAR;
// COMMENT: {6/25/2009 8:34:18 PM}						grid.SetItemText(pnmgv->iRow, TYPE_COLUMN, d);
// COMMENT: {6/25/2009 8:34:18 PM}						if (!(*vectors[mt])[pnmgv->iRow])
// COMMENT: {6/25/2009 8:34:18 PM}						{
// COMMENT: {6/25/2009 8:34:18 PM}							(*vectors[mt])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}						}
// COMMENT: {6/25/2009 8:34:18 PM}						delete (*vectors[SELECTED])[pnmgv->iRow];
// COMMENT: {6/25/2009 8:34:18 PM}						(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}						//{{
// COMMENT: {6/25/2009 8:34:18 PM}						CDataExchange dx(this, FALSE);
// COMMENT: {6/25/2009 8:34:18 PM}						this->DDX_Property(&dx, *(*vectors[SELECTED])[pnmgv->iRow]);
// COMMENT: {6/25/2009 8:34:18 PM}						//}}
// COMMENT: {6/25/2009 8:34:18 PM}						this->SetMode(mt);
// COMMENT: {6/25/2009 8:34:18 PM}						break;
// COMMENT: {6/25/2009 8:34:18 PM}					}
// COMMENT: {6/25/2009 8:34:18 PM}					else if (d.CompareNoCase(PSZ_POINTS) == 0)
// COMMENT: {6/25/2009 8:34:18 PM}					{
// COMMENT: {6/25/2009 8:34:18 PM}						CFluxPropsPage2::ModeType mt = CFluxPropsPage2::POINTS;
// COMMENT: {6/25/2009 8:34:18 PM}						grid.SetItemText(pnmgv->iRow, TYPE_COLUMN, d);
// COMMENT: {6/25/2009 8:34:18 PM}						if (!(*vectors[mt])[pnmgv->iRow])
// COMMENT: {6/25/2009 8:34:18 PM}						{
// COMMENT: {6/25/2009 8:34:18 PM}							(*vectors[mt])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}						}
// COMMENT: {6/25/2009 8:34:18 PM}						delete (*vectors[SELECTED])[pnmgv->iRow];
// COMMENT: {6/25/2009 8:34:18 PM}						(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}						//{{
// COMMENT: {6/25/2009 8:34:18 PM}						CDataExchange dx(this, FALSE);
// COMMENT: {6/25/2009 8:34:18 PM}						this->DDX_Property(&dx, *(*vectors[SELECTED])[pnmgv->iRow]);
// COMMENT: {6/25/2009 8:34:18 PM}						//}}
// COMMENT: {6/25/2009 8:34:18 PM}						this->SetMode(mt);
// COMMENT: {6/25/2009 8:34:18 PM}						break;
// COMMENT: {6/25/2009 8:34:18 PM}					}
// COMMENT: {6/25/2009 8:34:18 PM}					else if (d.CompareNoCase(PSZ_XYZ) == 0)
// COMMENT: {6/25/2009 8:34:18 PM}					{
// COMMENT: {6/25/2009 8:34:18 PM}						CFluxPropsPage2::ModeType mt = CFluxPropsPage2::XYZ;
// COMMENT: {6/25/2009 8:34:18 PM}						grid.SetItemText(pnmgv->iRow, TYPE_COLUMN, d);
// COMMENT: {6/25/2009 8:34:18 PM}						if (!(*vectors[mt])[pnmgv->iRow])
// COMMENT: {6/25/2009 8:34:18 PM}						{
// COMMENT: {6/25/2009 8:34:18 PM}							(*vectors[mt])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}						}
// COMMENT: {6/25/2009 8:34:18 PM}						delete (*vectors[SELECTED])[pnmgv->iRow];
// COMMENT: {6/25/2009 8:34:18 PM}						(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}						//{{
// COMMENT: {6/25/2009 8:34:18 PM}						CDataExchange dx(this, FALSE);
// COMMENT: {6/25/2009 8:34:18 PM}						this->DDX_Property(&dx, *(*vectors[SELECTED])[pnmgv->iRow]);
// COMMENT: {6/25/2009 8:34:18 PM}						//}}
// COMMENT: {6/25/2009 8:34:18 PM}						this->SetMode(mt);
// COMMENT: {6/25/2009 8:34:18 PM}						break;
// COMMENT: {6/25/2009 8:34:18 PM}					}
// COMMENT: {6/25/2009 8:34:18 PM}				}
// COMMENT: {6/25/2009 8:34:18 PM}			}
// COMMENT: {6/25/2009 8:34:18 PM}		}
// COMMENT: {6/25/2009 8:34:18 PM}		if (pnmgv->iColumn == TYPE_COLUMN)
// COMMENT: {6/25/2009 8:34:18 PM}		{
// COMMENT: {6/25/2009 8:34:18 PM}			CFluxPropsPage2::ModeType mt = CFluxPropsPage2::NONE;
// COMMENT: {6/25/2009 8:34:18 PM}			if (str.Trim().IsEmpty() || str.Compare(PSZ_NONE) == 0)
// COMMENT: {6/25/2009 8:34:18 PM}			{
// COMMENT: {6/25/2009 8:34:18 PM}				delete (*vectors[SELECTED])[pnmgv->iRow];
// COMMENT: {6/25/2009 8:34:18 PM}				(*vectors[SELECTED])[pnmgv->iRow] = 0;
// COMMENT: {6/25/2009 8:34:18 PM}			}
// COMMENT: {6/25/2009 8:34:18 PM}			else if (str.Compare(PSZ_CONSTANT) == 0)
// COMMENT: {6/25/2009 8:34:18 PM}			{
// COMMENT: {6/25/2009 8:34:18 PM}				mt = CFluxPropsPage2::CONSTANT;
// COMMENT: {6/25/2009 8:34:18 PM}				if (!(*vectors[mt])[pnmgv->iRow])
// COMMENT: {6/25/2009 8:34:18 PM}				{
// COMMENT: {6/25/2009 8:34:18 PM}					Cproperty def(PROP_FIXED);
// COMMENT: {6/25/2009 8:34:18 PM}					for (int r = pnmgv->iRow - 1; r > 0; --r)
// COMMENT: {6/25/2009 8:34:18 PM}					{
// COMMENT: {6/25/2009 8:34:18 PM}						CString d = grid.GetItemText(r, TYPE_COLUMN);
// COMMENT: {6/25/2009 8:34:18 PM}						if (d.CompareNoCase(PSZ_CONSTANT) == 0)
// COMMENT: {6/25/2009 8:34:18 PM}						{
// COMMENT: {6/25/2009 8:34:18 PM}							ASSERT((*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}							def = (*(*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}							break;
// COMMENT: {6/25/2009 8:34:18 PM}						}
// COMMENT: {6/25/2009 8:34:18 PM}					}
// COMMENT: {6/25/2009 8:34:18 PM}					(*vectors[mt])[pnmgv->iRow] = new Cproperty(def);
// COMMENT: {6/25/2009 8:34:18 PM}				}
// COMMENT: {6/25/2009 8:34:18 PM}				delete (*vectors[SELECTED])[pnmgv->iRow];
// COMMENT: {6/25/2009 8:34:18 PM}				(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[pnmgv->iRow]);
// COMMENT: {6/25/2009 8:34:18 PM}			}
// COMMENT: {6/25/2009 8:34:18 PM}			else if (str.Compare(PSZ_LINEAR) == 0)
// COMMENT: {6/25/2009 8:34:18 PM}			{
// COMMENT: {6/25/2009 8:34:18 PM}				mt = CFluxPropsPage2::LINEAR;
// COMMENT: {6/25/2009 8:34:18 PM}				if (!(*vectors[mt])[pnmgv->iRow])
// COMMENT: {6/25/2009 8:34:18 PM}				{
// COMMENT: {6/25/2009 8:34:18 PM}					Cproperty def(PROP_LINEAR);
// COMMENT: {6/25/2009 8:34:18 PM}					for (int r = pnmgv->iRow - 1; r > 0; --r)
// COMMENT: {6/25/2009 8:34:18 PM}					{
// COMMENT: {6/25/2009 8:34:18 PM}						CString d = grid.GetItemText(r, TYPE_COLUMN);
// COMMENT: {6/25/2009 8:34:18 PM}						if (d.CompareNoCase(PSZ_LINEAR) == 0)
// COMMENT: {6/25/2009 8:34:18 PM}						{
// COMMENT: {6/25/2009 8:34:18 PM}							ASSERT((*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}							def = (*(*vectors[mt])[r]);
// COMMENT: {6/25/2009 8:34:18 PM}							break;
// COMMENT: {6/25/2009 8:34:18 PM}						}
// COMMENT: {6/25/2009 8:34:18 PM}					}
// COMMENT: {6/25/2009 8:34:18 PM}					(*vectors[mt])[pnmgv->iRow] = new Cproperty(def);
// COMMENT: {6/25/2009 8:34:18 PM}				}
// COMMENT: {6/25/2009 8:34:18 PM}				delete (*vectors[SELECTED])[pnmgv->iRow];
// COMMENT: {6/25/2009 8:34:18 PM}				(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[pnmgv->iRow]);
// COMMENT: {6/25/2009 8:34:18 PM}			}
// COMMENT: {6/25/2009 8:34:18 PM}			if (mt != CFluxPropsPage2::NONE)
// COMMENT: {6/25/2009 8:34:18 PM}			{
// COMMENT: {6/25/2009 8:34:18 PM}				this->UpdateData(FALSE);
// COMMENT: {6/25/2009 8:34:18 PM}			}
// COMMENT: {6/25/2009 8:34:18 PM}			this->SetMode(mt);
// COMMENT: {6/25/2009 8:34:18 PM}		}
// COMMENT: {6/25/2009 8:34:18 PM}	}
// COMMENT: {6/25/2009 8:34:18 PM}	grid.RedrawWindow();
// COMMENT: {6/25/2009 8:34:18 PM}
// COMMENT: {6/25/2009 8:34:18 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:34:18 PM}}

void CFluxPropsPage2::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemOld.hItem;
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->FluxSeries.treeitem)
		{
// COMMENT: {6/26/2009 3:44:07 PM}			if (this->FluxSeries.validation_row > 0)
// COMMENT: {6/26/2009 3:44:07 PM}			{
// COMMENT: {6/26/2009 3:44:07 PM}// COMMENT: {6/25/2009 7:40:50 PM}				this->DDV_SoftValidate(this->FluxSeries.grid, this->FluxSeries.vv_props, this->FluxSeries.validation_row);
				this->FluxSeries.DDV_SoftValidate();
// COMMENT: {6/26/2009 3:44:10 PM}			}
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
// COMMENT: {6/26/2009 3:44:15 PM}			if (this->SolutionSeries.validation_row > 0)
// COMMENT: {6/26/2009 3:44:15 PM}			{
// COMMENT: {6/25/2009 7:52:23 PM}				this->DDV_SoftValidate(this->SolutionSeries.grid, this->SolutionSeries.vv_props, this->SolutionSeries.validation_row);
				this->SolutionSeries.DDV_SoftValidate();
// COMMENT: {6/26/2009 3:44:19 PM}			}
		}

		//{{{6/26/2009 5:12:11 PM}
		// force CInPlaceXXX to lose focus
		this->TreeCtrl.SetFocus();
		//}}{6/26/2009 5:12:11 PM}

		if (!this->UpdateData(TRUE))
		{
			// notify which control caused failure
			//
			CWnd* pFocus = CWnd::GetFocus();
			this->PostMessage(UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

			// disallow change
			//
			*pResult = TRUE;
			TRACE("Out %s Disallowed\n", __FUNCTION__);
			return;
		}
	}
	*pResult = 0;
	TRACE("Out Allowed %s\n", __FUNCTION__);
}

#define COMPARE_SET(S, R) \
do { \
	if (strItem.Compare(S) == 0) { \
		this->RichEditCtrl.SetWindowText(R.c_str()); \
	} \
} while (0)

void CFluxPropsPage2::OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	UNREFERENCED_PARAMETER(pResult);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemNew.hItem;
	if (this->ItemDDX)
	{
		this->UpdateData(FALSE);

		// update property description
		//
		if (this->TreeCtrl.GetSafeHwnd())
		{
			CString strItem = this->TreeCtrl.GetItemText(this->ItemDDX);

			COMPARE_SET(PSZ_FLUX,     this->m_sFluxRTF);
			COMPARE_SET(PSZ_SOLUTION, this->m_sAssocSolutionRTF);
		}
	}
	if (this->TreeCtrl.GetSafeHwnd())
	{
		this->TreeCtrl.SetFocus();
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnEnSetfocusDescEdit()
{
	TRACE("In %s\n", __FUNCTION__);
	this->RichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnSelChangedFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	//{{ MOVED to OnSelChanged
// COMMENT: {6/26/2009 6:31:15 PM}	// soft-validate
// COMMENT: {6/26/2009 6:31:15 PM}	if (this->FluxSeries.validation_row > 0)
// COMMENT: {6/26/2009 6:31:15 PM}	{
// COMMENT: {6/26/2009 6:31:15 PM}		CCellID cell = this->FluxSeries.grid.GetFocusCell();
// COMMENT: {6/26/2009 6:31:15 PM}		if (cell.row != this->FluxSeries.validation_row)
// COMMENT: {6/26/2009 6:31:15 PM}		{
// COMMENT: {6/26/2009 6:31:15 PM}// COMMENT: {6/25/2009 7:53:30 PM}			this->DDV_SoftValidate(this->FluxSeries.grid, this->FluxSeries.vv_props, this->FluxSeries.validation_row);
// COMMENT: {6/26/2009 6:31:15 PM}			this->FluxSeries.DDV_SoftValidate();
// COMMENT: {6/26/2009 6:31:15 PM}		}
// COMMENT: {6/26/2009 6:31:15 PM}	}
	//}} MOVED to OnSelChanged

// COMMENT: {6/25/2009 9:00:13 PM}	this->OnSelChanged(pNotifyStruct, result, this->FluxSeries.grid);
// COMMENT: {6/25/2009 9:00:13 PM}
// COMMENT: {6/25/2009 9:00:13 PM}	// set next validation row
// COMMENT: {6/25/2009 9:00:13 PM}	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
// COMMENT: {6/25/2009 9:00:13 PM}	this->FluxSeries.validation_row = pnmgv->iRow;

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->FluxSeries.OnSelChanged(pnmgv->iRow, pnmgv->iColumn);

	//{{ MOVED to OnSelChanged
// COMMENT: {6/25/2009 11:23:41 PM}	// set next validation row
// COMMENT: {6/25/2009 11:23:41 PM}	this->FluxSeries.validation_row = pnmgv->iRow;
	//}} MOVED to OnSelChanged

	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	//{{ MOVED to OnSelChanged
	// soft-validate
// COMMENT: {6/26/2009 3:30:22 PM}	if (this->SolutionSeries.validation_row > 0)
// COMMENT: {6/26/2009 3:30:22 PM}	{
// COMMENT: {6/26/2009 3:30:22 PM}		CCellID cell = this->SolutionSeries.grid.GetFocusCell();
// COMMENT: {6/26/2009 3:30:22 PM}		if (cell.row != this->SolutionSeries.validation_row)
// COMMENT: {6/26/2009 3:30:22 PM}		{
// COMMENT: {6/26/2009 3:30:22 PM}// COMMENT: {6/25/2009 7:53:55 PM}			this->DDV_SoftValidate(this->SolutionSeries.grid, this->SolutionSeries.vv_props, this->SolutionSeries.validation_row);
// COMMENT: {6/26/2009 3:30:22 PM}			this->SolutionSeries.DDV_SoftValidate();
// COMMENT: {6/26/2009 3:30:22 PM}		}
// COMMENT: {6/26/2009 3:30:22 PM}	}
	//}} MOVED to OnSelChanged

// COMMENT: {6/25/2009 8:58:03 PM}	this->OnSelChanged(pNotifyStruct, result, this->SolutionSeries.grid);
// COMMENT: {6/25/2009 8:58:03 PM}
// COMMENT: {6/25/2009 8:58:03 PM}	// set next validation row
// COMMENT: {6/25/2009 8:58:03 PM}	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
// COMMENT: {6/25/2009 8:58:03 PM}	this->SolutionSeries.validation_row = pnmgv->iRow;

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->SolutionSeries.OnSelChanged(pnmgv->iRow, pnmgv->iColumn);

	//{{ MOVED to OnSelChanged
// COMMENT: {6/25/2009 11:24:32 PM}	// set next validation row
// COMMENT: {6/25/2009 11:24:32 PM}	this->SolutionSeries.validation_row = pnmgv->iRow;
	//}} MOVED to OnSelChanged

	TRACE("Out %s\n", __FUNCTION__);
}

// COMMENT: {6/25/2009 7:51:08 PM}void CFluxPropsPage2::DDV_SoftValidate(CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors, int row)
// COMMENT: {6/25/2009 7:51:08 PM}{
// COMMENT: {6/25/2009 7:51:08 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 7:51:08 PM}	if (grid.GetSafeHwnd())
// COMMENT: {6/25/2009 7:51:08 PM}	{
// COMMENT: {6/25/2009 7:51:08 PM}		CString str = grid.GetItemText(row, TYPE_COLUMN);
// COMMENT: {6/25/2009 7:51:08 PM}
// COMMENT: {6/25/2009 7:51:08 PM}		if (str.Trim().IsEmpty() || str.CompareNoCase(PSZ_NONE) == 0)
// COMMENT: {6/25/2009 7:51:08 PM}		{
// COMMENT: {6/25/2009 7:51:08 PM}			delete (*vectors[SELECTED])[row];
// COMMENT: {6/25/2009 7:51:08 PM}			(*vectors[SELECTED])[row] = 0;
// COMMENT: {6/25/2009 7:51:08 PM}		}
// COMMENT: {6/25/2009 7:51:08 PM}		else if (str.Compare(PSZ_CONSTANT) == 0)
// COMMENT: {6/25/2009 7:51:08 PM}		{
// COMMENT: {6/25/2009 7:51:08 PM}			CDataExchange dx(this, TRUE);
// COMMENT: {6/25/2009 7:51:08 PM}
// COMMENT: {6/25/2009 7:51:08 PM}			ASSERT((*vectors[SELECTED])[row]);
// COMMENT: {6/25/2009 7:51:08 PM}			ASSERT((*vectors[CONSTANT])[row]);
// COMMENT: {6/25/2009 7:51:08 PM}			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE_EDIT, (*vectors[SELECTED])[row]->v[0]);
// COMMENT: {6/25/2009 7:51:08 PM}			*(*vectors[CONSTANT])[row] = *(*vectors[SELECTED])[row];
// COMMENT: {6/25/2009 7:51:08 PM}			//mode = CFluxPropsPage2::CONSTANT;
// COMMENT: {6/25/2009 7:51:08 PM}		}
// COMMENT: {6/25/2009 7:51:08 PM}		else if (str.Compare(PSZ_LINEAR) == 0)
// COMMENT: {6/25/2009 7:51:08 PM}		{
// COMMENT: {6/25/2009 7:51:08 PM}			CDataExchange dx(this, TRUE);
// COMMENT: {6/25/2009 7:51:08 PM}
// COMMENT: {6/25/2009 7:51:08 PM}			ASSERT((*vectors[SELECTED])[row]);
// COMMENT: {6/25/2009 7:51:08 PM}			ASSERT((*vectors[LINEAR])[row]);
// COMMENT: {6/25/2009 7:51:08 PM}
// COMMENT: {6/25/2009 7:51:08 PM}			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE1_EDIT,    (*vectors[SELECTED])[row]->v[0]);
// COMMENT: {6/25/2009 7:51:08 PM}			CGlobal::DDX_Text_Safe(&dx, IDC_DISTANCE1_EDIT, (*vectors[SELECTED])[row]->dist1);
// COMMENT: {6/25/2009 7:51:08 PM}			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE2_EDIT,    (*vectors[SELECTED])[row]->v[1]);
// COMMENT: {6/25/2009 7:51:08 PM}			CGlobal::DDX_Text_Safe(&dx, IDC_DISTANCE2_EDIT, (*vectors[SELECTED])[row]->dist2);
// COMMENT: {6/25/2009 7:51:08 PM}
// COMMENT: {6/25/2009 7:51:08 PM}			*(*vectors[LINEAR])[row] = *(*vectors[SELECTED])[row];
// COMMENT: {6/25/2009 7:51:08 PM}
// COMMENT: {6/25/2009 7:51:08 PM}			//mode = CFluxPropsPage2::LINEAR;
// COMMENT: {6/25/2009 7:51:08 PM}		}
// COMMENT: {6/25/2009 7:51:08 PM}	}
// COMMENT: {6/25/2009 7:51:08 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 7:51:08 PM}}

// COMMENT: {6/25/2009 8:56:47 PM}void CFluxPropsPage2::OnSelChanged(NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid)
// COMMENT: {6/25/2009 8:56:47 PM}{
// COMMENT: {6/25/2009 8:56:47 PM}	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
// COMMENT: {6/25/2009 8:56:47 PM}	TRACE("In %s row = %d\n", __FUNCTION__, pnmgv->iRow);
// COMMENT: {6/25/2009 8:56:47 PM}
// COMMENT: {6/25/2009 8:56:47 PM}	if (grid.GetSafeHwnd())
// COMMENT: {6/25/2009 8:56:47 PM}	{
// COMMENT: {6/25/2009 8:56:47 PM}		CString str = grid.GetItemText(pnmgv->iRow, TYPE_COLUMN);
// COMMENT: {6/25/2009 8:56:47 PM}		if (this->ItemDDX == this->FluxSeries.treeitem)
// COMMENT: {6/25/2009 8:56:47 PM}		{
// COMMENT: {6/25/2009 8:56:47 PM}			ASSERT(&grid == &this->FluxSeries.grid);
// COMMENT: {6/25/2009 8:56:47 PM}			this->DDX_Vectors(pnmgv->iRow, str, this->FluxSeries.vv_props);
// COMMENT: {6/25/2009 8:56:47 PM}		}
// COMMENT: {6/25/2009 8:56:47 PM}		else if (this->ItemDDX == this->SolutionSeries.treeitem)
// COMMENT: {6/25/2009 8:56:47 PM}		{
// COMMENT: {6/25/2009 8:56:47 PM}			ASSERT(&grid == &this->SolutionSeries.grid);
// COMMENT: {6/25/2009 8:56:47 PM}			this->DDX_Vectors(pnmgv->iRow, str, this->SolutionSeries.vv_props);
// COMMENT: {6/25/2009 8:56:47 PM}		}
// COMMENT: {6/25/2009 8:56:47 PM}	}
// COMMENT: {6/25/2009 8:56:47 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:56:47 PM}}

// COMMENT: {6/25/2009 9:02:13 PM}void CFluxPropsPage2::DDX_Vectors(int row, CString &str, std::vector< std::vector<Cproperty*>* > &vectors)
// COMMENT: {6/25/2009 9:02:13 PM}{
// COMMENT: {6/25/2009 9:02:13 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 9:02:13 PM}	CDataExchange dx(this, FALSE);
// COMMENT: {6/25/2009 9:02:13 PM}
// COMMENT: {6/25/2009 9:02:13 PM}	CFluxPropsPage2::ModeType mode = CFluxPropsPage2::NONE;
// COMMENT: {6/25/2009 9:02:13 PM}	if (str.Compare(PSZ_CONSTANT) == 0)
// COMMENT: {6/25/2009 9:02:13 PM}	{
// COMMENT: {6/25/2009 9:02:13 PM}		mode = CFluxPropsPage2::CONSTANT;
// COMMENT: {6/25/2009 9:02:13 PM}		if ((*vectors[CONSTANT])[row])
// COMMENT: {6/25/2009 9:02:13 PM}		{
// COMMENT: {6/25/2009 9:02:13 PM}			this->DDX_Property(&dx, *(*vectors[CONSTANT])[row]);
// COMMENT: {6/25/2009 9:02:13 PM}		}
// COMMENT: {6/25/2009 9:02:13 PM}	}
// COMMENT: {6/25/2009 9:02:13 PM}	else if (str.Compare(PSZ_LINEAR) == 0)
// COMMENT: {6/25/2009 9:02:13 PM}	{
// COMMENT: {6/25/2009 9:02:13 PM}		mode = CFluxPropsPage2::LINEAR;
// COMMENT: {6/25/2009 9:02:13 PM}		if ((*vectors[LINEAR])[row])
// COMMENT: {6/25/2009 9:02:13 PM}		{
// COMMENT: {6/25/2009 9:02:13 PM}			this->DDX_Property(&dx, *(*vectors[LINEAR])[row]);
// COMMENT: {6/25/2009 9:02:13 PM}		}
// COMMENT: {6/25/2009 9:02:13 PM}	}
// COMMENT: {6/25/2009 9:02:13 PM}	this->SetMode(mode);
// COMMENT: {6/25/2009 9:02:13 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 9:02:13 PM}}

// COMMENT: {6/25/2009 9:02:42 PM}void CFluxPropsPage2::InitializeGrid(CDataExchange* pDX, CModGridCtrlEx &grid/*, CTimeSeries<Cproperty> &series*/)
// COMMENT: {6/25/2009 9:02:42 PM}{
// COMMENT: {6/25/2009 9:02:42 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 9:02:42 PM}	// Prepare Grid
// COMMENT: {6/25/2009 9:02:42 PM}	//
// COMMENT: {6/25/2009 9:02:42 PM}	if (!pDX->m_bSaveAndValidate && grid.GetColumnCount() == 0)
// COMMENT: {6/25/2009 9:02:42 PM}	{
// COMMENT: {6/25/2009 9:02:42 PM}		TRY
// COMMENT: {6/25/2009 9:02:42 PM}		{
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetRowCount(MIN_ROWS/* + this->m_well.count_depth_user + this->m_well.count_elevation_user*/);
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetColumnCount(3);
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetFixedRowCount(1);
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetFixedColumnCount(0);
// COMMENT: {6/25/2009 9:02:42 PM}			grid.EnableTitleTips(FALSE);
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetCurrentFocusCell(1, 2);
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}			///CCellRange FluxCellRange = grid.GetSelectedCellRange();
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}			// grid.EnableSelection(FALSE); // this breaks CutSelectedText
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetHighLight(GV_HIGHLIGHT_ALWAYS);
// COMMENT: {6/25/2009 9:02:42 PM}		}
// COMMENT: {6/25/2009 9:02:42 PM}		CATCH (CMemoryException, e)
// COMMENT: {6/25/2009 9:02:42 PM}		{
// COMMENT: {6/25/2009 9:02:42 PM}			e->ReportError();
// COMMENT: {6/25/2009 9:02:42 PM}			e->Delete();
// COMMENT: {6/25/2009 9:02:42 PM}		}
// COMMENT: {6/25/2009 9:02:42 PM}		END_CATCH
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}		// set default format
// COMMENT: {6/25/2009 9:02:42 PM}		for (int row = grid.GetFixedRowCount(); row < grid.GetRowCount(); ++row)
// COMMENT: {6/25/2009 9:02:42 PM}		{
// COMMENT: {6/25/2009 9:02:42 PM}			for (int col = 0; col < grid.GetColumnCount(); ++col)
// COMMENT: {6/25/2009 9:02:42 PM}			{
// COMMENT: {6/25/2009 9:02:42 PM}				grid.SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
// COMMENT: {6/25/2009 9:02:42 PM}			}
// COMMENT: {6/25/2009 9:02:42 PM}		}
// COMMENT: {6/25/2009 9:02:42 PM}		///this->SetScreenHeadings(this->m_bByDepth);
// COMMENT: {6/25/2009 9:02:42 PM}		{
// COMMENT: {6/25/2009 9:02:42 PM}			GV_ITEM Item;
// COMMENT: {6/25/2009 9:02:42 PM}			Item.mask = GVIF_TEXT;
// COMMENT: {6/25/2009 9:02:42 PM}			Item.row = 0;
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}			Item.col = 0;
// COMMENT: {6/25/2009 9:02:42 PM}			Item.szText.Format(_T("Time"));
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetItem(&Item);
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}			Item.col = 1;
// COMMENT: {6/25/2009 9:02:42 PM}			Item.szText.Format(_T("Units"));
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetItem(&Item);
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}			Item.col = 2;
// COMMENT: {6/25/2009 9:02:42 PM}			Item.szText.Format(_T("Type"));
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetItem(&Item);
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetItemText(1, 0, _T("0"));
// COMMENT: {6/25/2009 9:02:42 PM}			grid.DisableCell(1, 0);
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}			grid.DisableCell(1, 1);
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetItemText(1, 2, PSZ_NONE);
// COMMENT: {6/25/2009 9:02:42 PM}			this->SetMode(CFluxPropsPage2::NONE);
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}			// units column
// COMMENT: {6/25/2009 9:02:42 PM}			std::vector<LPCTSTR> vecTimeUnits;
// COMMENT: {6/25/2009 9:02:42 PM}			vecTimeUnits.push_back(PSZ_SECS);
// COMMENT: {6/25/2009 9:02:42 PM}			vecTimeUnits.push_back(PSZ_MINS);
// COMMENT: {6/25/2009 9:02:42 PM}			vecTimeUnits.push_back(PSZ_HOUR);
// COMMENT: {6/25/2009 9:02:42 PM}			vecTimeUnits.push_back(PSZ_DAYS);
// COMMENT: {6/25/2009 9:02:42 PM}			vecTimeUnits.push_back(PSZ_YEAR);
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetColumnOptions(1, vecTimeUnits);
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}			// type column
// COMMENT: {6/25/2009 9:02:42 PM}			std::vector<LPCTSTR> vecPropType;
// COMMENT: {6/25/2009 9:02:42 PM}			vecPropType.push_back(PSZ_NONE);
// COMMENT: {6/25/2009 9:02:42 PM}			vecPropType.push_back(PSZ_CONSTANT);
// COMMENT: {6/25/2009 9:02:42 PM}			vecPropType.push_back(PSZ_LINEAR);
// COMMENT: {6/25/2009 9:02:42 PM}// COMMENT: {6/10/2009 3:34:31 PM}			vecPropType.push_back(PSZ_POINTS);
// COMMENT: {6/25/2009 9:02:42 PM}// COMMENT: {6/10/2009 3:34:31 PM}			vecPropType.push_back(PSZ_XYZ);
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetColumnOptions(2, vecPropType);
// COMMENT: {6/25/2009 9:02:42 PM}			//}}
// COMMENT: {6/25/2009 9:02:42 PM}
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetColumnWidth(0, 71);
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetColumnWidth(1, 62);
// COMMENT: {6/25/2009 9:02:42 PM}			grid.SetColumnWidth(2, 70);
// COMMENT: {6/25/2009 9:02:42 PM}		}
// COMMENT: {6/25/2009 9:02:42 PM}	}
// COMMENT: {6/25/2009 9:02:42 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 9:02:42 PM}}

LRESULT CFluxPropsPage2::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	CWnd* pFocus = (CWnd*)wParam;
	if (pFocus && pFocus->GetSafeHwnd())
	{
		ASSERT_KINDOF(CWnd, pFocus);
		pFocus->SetFocus();
	}
	return 0;
}

// COMMENT: {6/25/2009 9:03:16 PM}void CFluxPropsPage2::SetCurrentFocusCell(CModGridCtrlEx &grid, int row, int col)
// COMMENT: {6/25/2009 9:03:16 PM}{
// COMMENT: {6/25/2009 9:03:16 PM}	// NOTE: this call doesn't work if called within DoDataExchange since
// COMMENT: {6/25/2009 9:03:16 PM}	// UpdateData blocks notifications
// COMMENT: {6/25/2009 9:03:16 PM}	//
// COMMENT: {6/25/2009 9:03:16 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 9:03:16 PM}	grid.SetCurrentFocusCell(row, col);
// COMMENT: {6/25/2009 9:03:16 PM}
// COMMENT: {6/25/2009 9:03:16 PM}	static NM_GRIDVIEW nmgv;
// COMMENT: {6/25/2009 9:03:16 PM}	nmgv.iRow         = row;
// COMMENT: {6/25/2009 9:03:16 PM}	nmgv.iColumn      = col;
// COMMENT: {6/25/2009 9:03:16 PM}	nmgv.hdr.hwndFrom = grid.GetSafeHwnd();
// COMMENT: {6/25/2009 9:03:16 PM}	nmgv.hdr.idFrom   = grid.GetDlgCtrlID();
// COMMENT: {6/25/2009 9:03:16 PM}	nmgv.hdr.code     = GVN_SELCHANGED;
// COMMENT: {6/25/2009 9:03:16 PM}	ASSERT(::IsWindow(nmgv.hdr.hwndFrom));
// COMMENT: {6/25/2009 9:03:16 PM}
// COMMENT: {6/25/2009 9:03:16 PM}	CWnd *pOwner = grid.GetOwner();
// COMMENT: {6/25/2009 9:03:16 PM}	if (pOwner && ::IsWindow(pOwner->m_hWnd))
// COMMENT: {6/25/2009 9:03:16 PM}	{
// COMMENT: {6/25/2009 9:03:16 PM}		//pOwner->PostMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
// COMMENT: {6/25/2009 9:03:16 PM}		pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
// COMMENT: {6/25/2009 9:03:16 PM}	}
// COMMENT: {6/25/2009 9:03:16 PM}	grid.RedrawWindow();
// COMMENT: {6/25/2009 9:03:16 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 9:03:16 PM}}
