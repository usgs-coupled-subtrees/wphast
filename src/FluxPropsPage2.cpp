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

const int MIN_ROWS = 20;

const int SELECTED = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CFluxPropsPage2, CPropertyPage)

CFluxPropsPage2::CFluxPropsPage2()
	: CPropertyPage(CFluxPropsPage2::IDD)
{
	ASSERT(std::numeric_limits<double>::has_signaling_NaN);
	double d = std::numeric_limits<double>::signaling_NaN();
	ASSERT(d != d);
	ASSERT(::_isnan(d));

	ASSERT(std::numeric_limits<double>::has_infinity);
	double i = std::numeric_limits<double>::infinity();
	ASSERT(!::_finite(i));

	this->SetFlowOnly(false);
	this->SetDefault(false);

	// init properties
	//
// COMMENT: {6/5/2009 8:01:18 PM}	this->SetProperties(this->BC);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,   IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sAssocSolutionRTF, IDR_BC_FLUX_ASSOC_SOL_RTF);
	CGlobal::LoadRTFString(this->m_sFluxRTF,          IDR_BC_FLUX_FLUX_RTF);
	CGlobal::LoadRTFString(this->m_sFaceRTF,          IDR_BC_FLUX_FACE_RTF);

	this->DefaultUnits = _T("years");

	this->FluxVectors.push_back(&this->FluxVector);
	this->FluxVectors.push_back(&this->FluxVectorConstant);
	this->FluxVectors.push_back(&this->FluxVectorLinear);

	this->SolutionVectors.push_back(&this->SolutionVector);
	this->SolutionVectors.push_back(&this->SolutionVectorConstant);
	this->SolutionVectors.push_back(&this->SolutionVectorLinear);
}

CFluxPropsPage2::~CFluxPropsPage2()
{
	std::vector<Ctime*>::iterator t;

	t = this->SolutionTimes.begin();
	for (; t != this->SolutionTimes.end(); ++t)
	{
		delete (*t);
	}

	t = this->FluxTimes.begin();
	for (; t != this->FluxTimes.end(); ++t)
	{
		delete (*t);
	}

	std::vector<Cproperty*>::iterator it;

	it = this->FluxVector.begin();
	for (; it != this->FluxVector.end(); ++it)
	{
		delete (*it);
	}

	it = this->FluxVectorConstant.begin();
	for (; it != this->FluxVectorConstant.end(); ++it)
	{
		delete (*it);
	}

	it = this->FluxVectorLinear.begin();
	for (; it != this->FluxVectorLinear.end(); ++it)
	{
		delete (*it);
	}

	it = this->SolutionVector.begin();
	for (; it != this->SolutionVector.end(); ++it)
	{
		delete (*it);
	}

	it = this->SolutionVectorConstant.begin();
	for (; it != this->SolutionVectorConstant.end(); ++it)
	{
		delete (*it);
	}

	it = this->SolutionVectorLinear.begin();
	for (; it != this->SolutionVectorLinear.end(); ++it)
	{
		delete (*it);
	}
}

void CFluxPropsPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	DDX_Control(pDX, IDC_PROP_TREE, this->TreeCtrl);
	DDX_Control(pDX, IDC_GB_PROPERTY, this->PropertyGroupBox);

	if (this->TreeCtrl.GetCount() == 0)
	{
		this->htiFLUX     = this->TreeCtrl.InsertItem(PSZ_FLUX,     TVI_ROOT, TVI_LAST);
		this->htiSOLUTION = this->TreeCtrl.InsertItem(PSZ_SOLUTION, TVI_ROOT, TVI_LAST);

		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);
		//this->RichEditCtrl.SetWindowText(this->m_sSolutionRTF.c_str());

		this->ItemDDX = this->htiFLUX;
		this->TreeCtrl.SelectItem(this->ItemDDX);

		this->TreeCtrl.ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);
	}

	DDX_GridControl(pDX, IDC_TIMES_GRID,    this->GridFlux);
	DDX_GridControl(pDX, IDC_GRID_SOLUTION, this->GridSolution);

	this->InitializeGrid(pDX, this->GridFlux,     this->BC.m_bc_flux);
	this->InitializeGrid(pDX, this->GridSolution, this->BC.m_bc_solution);

	// COMMENT: {6/16/2009 3:31:50 PM}	// Prepare Times Grid
	// COMMENT: {6/16/2009 3:31:50 PM}	//
	// COMMENT: {6/16/2009 3:31:50 PM}	if (!pDX->m_bSaveAndValidate && this->GridFlux.GetColumnCount() == 0)
	// COMMENT: {6/16/2009 3:31:50 PM}	{
	// COMMENT: {6/16/2009 3:31:50 PM}		TRY
	// COMMENT: {6/16/2009 3:31:50 PM}		{
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetRowCount(MIN_ROWS/* + this->m_well.count_depth_user + this->m_well.count_elevation_user*/);
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetColumnCount(3);
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetFixedRowCount(1);
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetFixedColumnCount(0);
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.EnableTitleTips(FALSE);
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetCurrentFocusCell(1, 2);
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			///CCellRange FluxCellRange = this->GridFlux.GetSelectedCellRange();
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			// this->GridFlux.EnableSelection(FALSE); // this breaks CutSelectedText
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetHighLight(GV_HIGHLIGHT_ALWAYS);
	// COMMENT: {6/16/2009 3:31:50 PM}		}
	// COMMENT: {6/16/2009 3:31:50 PM}		CATCH (CMemoryException, e)
	// COMMENT: {6/16/2009 3:31:50 PM}		{
	// COMMENT: {6/16/2009 3:31:50 PM}			e->ReportError();
	// COMMENT: {6/16/2009 3:31:50 PM}			e->Delete();
	// COMMENT: {6/16/2009 3:31:50 PM}		}
	// COMMENT: {6/16/2009 3:31:50 PM}		END_CATCH
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}		// set default format
	// COMMENT: {6/16/2009 3:31:50 PM}		for (int row = 0; row < this->GridFlux.GetRowCount(); ++row)
	// COMMENT: {6/16/2009 3:31:50 PM}		{
	// COMMENT: {6/16/2009 3:31:50 PM}			for (int col = 0; col < this->GridFlux.GetColumnCount(); ++col)
	// COMMENT: {6/16/2009 3:31:50 PM}			{
	// COMMENT: {6/16/2009 3:31:50 PM}				this->GridFlux.SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
	// COMMENT: {6/16/2009 3:31:50 PM}			}
	// COMMENT: {6/16/2009 3:31:50 PM}		}
	// COMMENT: {6/16/2009 3:31:50 PM}		///this->SetScreenHeadings(this->m_bByDepth);
	// COMMENT: {6/16/2009 3:31:50 PM}		{
	// COMMENT: {6/16/2009 3:31:50 PM}			GV_ITEM Item;
	// COMMENT: {6/16/2009 3:31:50 PM}			Item.mask = GVIF_TEXT;
	// COMMENT: {6/16/2009 3:31:50 PM}			Item.row = 0;
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			Item.col = 0;
	// COMMENT: {6/16/2009 3:31:50 PM}			Item.szText.Format(_T("Time"));
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetItem(&Item);
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			Item.col = 1;
	// COMMENT: {6/16/2009 3:31:50 PM}			Item.szText.Format(_T("Units"));
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetItem(&Item);
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			Item.col = 2;
	// COMMENT: {6/16/2009 3:31:50 PM}			Item.szText.Format(_T("Type"));
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetItem(&Item);
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetItemText(1, 0, _T("0"));
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.DisableCell(1, 0);
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.DisableCell(1, 1);
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetItemText(1, 2, PSZ_NONE);
	// COMMENT: {6/16/2009 3:31:50 PM}			this->SetMode(CFluxPropsPage2::NONE);
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			//{{
	// COMMENT: {6/16/2009 3:31:50 PM}			std::vector<LPCTSTR> vecTimeUnits;
	// COMMENT: {6/16/2009 3:31:50 PM}			vecTimeUnits.push_back(PSZ_SECS);
	// COMMENT: {6/16/2009 3:31:50 PM}			vecTimeUnits.push_back(PSZ_MINS);
	// COMMENT: {6/16/2009 3:31:50 PM}			vecTimeUnits.push_back(PSZ_HOUR);
	// COMMENT: {6/16/2009 3:31:50 PM}			vecTimeUnits.push_back(PSZ_DAYS);
	// COMMENT: {6/16/2009 3:31:50 PM}			vecTimeUnits.push_back(PSZ_YEAR);
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetColumnOptions(1, vecTimeUnits);
	// COMMENT: {6/16/2009 3:31:50 PM}			//}}
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			//{{
	// COMMENT: {6/16/2009 3:31:50 PM}			std::vector<LPCTSTR> vecPropType;
	// COMMENT: {6/16/2009 3:31:50 PM}			vecPropType.push_back(PSZ_NONE);
	// COMMENT: {6/16/2009 3:31:50 PM}			vecPropType.push_back(PSZ_CONSTANT);
	// COMMENT: {6/16/2009 3:31:50 PM}			vecPropType.push_back(PSZ_LINEAR);
	// COMMENT: {6/16/2009 3:31:50 PM}// COMMENT: {6/10/2009 3:34:31 PM}			vecPropType.push_back(PSZ_POINTS);
	// COMMENT: {6/16/2009 3:31:50 PM}// COMMENT: {6/10/2009 3:34:31 PM}			vecPropType.push_back(PSZ_XYZ);
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetColumnOptions(2, vecPropType);
	// COMMENT: {6/16/2009 3:31:50 PM}			//}}
	// COMMENT: {6/16/2009 3:31:50 PM}
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetColumnWidth(0, 71);
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetColumnWidth(1, 62);
	// COMMENT: {6/16/2009 3:31:50 PM}			this->GridFlux.SetColumnWidth(2, 70);
	// COMMENT: {6/16/2009 3:31:50 PM}		}
	// COMMENT: {6/16/2009 3:31:50 PM}		///this->GridFlux.ExpandColumnsToFit();
	// COMMENT: {6/16/2009 3:31:50 PM}	}

	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
		if (this->ItemDDX == this->htiFLUX)
		{
			//this->DDX_Series(pDX, this->GridFlux, this->BC.m_bc_flux);
			this->DDX_Series(pDX, this->GridFlux, this->FluxTimes, *(this->FluxVectors[SELECTED]));
			this->GridSolution.ShowWindow(SW_HIDE);
			this->GridFlux.ShowWindow(SW_SHOW);
		}
		else if (this->ItemDDX == this->htiSOLUTION)
		{
			//this->DDX_Series(pDX, this->GridSolution, this->BC.m_bc_solution);
			this->DDX_Series(pDX, this->GridSolution, this->SolutionTimes, *(this->SolutionVectors[SELECTED]));
			this->GridFlux.ShowWindow(SW_HIDE);
			this->GridSolution.ShowWindow(SW_SHOW);
		}



		// COMMENT: {6/16/2009 4:06:44 PM}		//{{
		// COMMENT: {6/16/2009 4:06:44 PM}		ASSERT(this->GridFlux.GetFixedColumnCount() <= cell.col && cell.col < this->GridFlux.GetColumnCount());
		// COMMENT: {6/16/2009 4:06:44 PM}		ASSERT(this->GridFlux.GetFixedRowCount() <= cell.row && cell.row < this->GridFlux.GetRowCount());
		// COMMENT: {6/16/2009 4:06:44 PM}		
		// COMMENT: {6/16/2009 4:06:44 PM}		////this->GridFlux.SendMessageToParent(cell.row, cell.col, GVN_SELCHANGED);
		// COMMENT: {6/16/2009 4:06:44 PM}		static NM_GRIDVIEW nmgv;
		// COMMENT: {6/16/2009 4:06:44 PM}		nmgv.iRow         = cell.row;
		// COMMENT: {6/16/2009 4:06:44 PM}		nmgv.iColumn      = cell.col;
		// COMMENT: {6/16/2009 4:06:44 PM}		nmgv.hdr.hwndFrom = this->GridFlux.GetSafeHwnd();
		// COMMENT: {6/16/2009 4:06:44 PM}		nmgv.hdr.idFrom   = this->GridFlux.GetDlgCtrlID();
		// COMMENT: {6/16/2009 4:06:44 PM}		nmgv.hdr.code     = GVN_SELCHANGED;
		// COMMENT: {6/16/2009 4:06:44 PM}		ASSERT(::IsWindow(nmgv.hdr.hwndFrom));
		// COMMENT: {6/16/2009 4:06:44 PM}
		// COMMENT: {6/16/2009 4:06:44 PM}		CWnd *pOwner = this->GridFlux.GetOwner();
		// COMMENT: {6/16/2009 4:06:44 PM}		if (pOwner && ::IsWindow(pOwner->m_hWnd))
		// COMMENT: {6/16/2009 4:06:44 PM}		{
		// COMMENT: {6/16/2009 4:06:44 PM}			//pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
		// COMMENT: {6/16/2009 4:06:44 PM}			pOwner->PostMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
		// COMMENT: {6/16/2009 4:06:44 PM}		}
		// COMMENT: {6/16/2009 4:06:44 PM}		//}}
		// COMMENT: {6/16/2009 4:06:44 PM}		this->GridFlux.RedrawWindow();
	}
}

void CFluxPropsPage2::DDX_Property(CDataExchange* pDX, Cproperty &p)
{
	CString strEmpty(_T(""));

	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
		if (p.type == PROP_UNDEFINED)
		{
		}
		else if (p.type == PROP_FIXED)
		{
			if (p.v[0] != p.v[0])
			{
				DDX_Text(pDX, IDC_VALUE_EDIT, strEmpty);
			}
			else
			{
				DDX_Text(pDX, IDC_VALUE_EDIT, p.v[0]);
			}
		}
		else if (p.type == PROP_LINEAR)
		{
			//this->GridFlux.SetItemText(row, TYPE_COLUMN, PSZ_LINEAR);
			switch (p.coord)
			{
				case 'x':
					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_X_RADIO);
					break;
				case 'y':
					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_Y_RADIO);
					break;
				case 'z':
					pDX->m_pDlgWnd->CheckRadioButton(IDC_X_RADIO, IDC_Z_RADIO, IDC_Z_RADIO);
					break;
				default:
					ASSERT(FALSE);
					break;
			}
			if (p.v[0] != p.v[0])
			{
				DDX_Text(pDX, IDC_VALUE1_EDIT, strEmpty);
			}
			else
			{
				DDX_Text(pDX, IDC_VALUE1_EDIT, p.v[0]);
			}

			if (p.dist1 != p.dist1)
			{
				DDX_Text(pDX, IDC_DISTANCE1_EDIT, strEmpty);
			}
			else
			{
				DDX_Text(pDX, IDC_DISTANCE1_EDIT, p.dist1);
			}

			if (p.v[1] != p.v[1])
			{
				DDX_Text(pDX, IDC_VALUE2_EDIT, strEmpty);
			}
			else
			{
				DDX_Text(pDX, IDC_VALUE2_EDIT, p.v[1]);
			}

			if (p.dist2 != p.dist2)
			{
				DDX_Text(pDX, IDC_DISTANCE2_EDIT, strEmpty);
			}
			else
			{
				DDX_Text(pDX, IDC_DISTANCE2_EDIT, p.dist2);
			}
		}
	}
}

void CFluxPropsPage2::DDX_Series(CDataExchange* pDX, CModGridCtrlEx &grid, CTimeSeries<Cproperty> &props)
{
	ASSERT(!pDX->m_bSaveAndValidate);

	CCellID cell = grid.GetFocusCell();
	CString s;
	CTimeSeries<Cproperty>::iterator fit = props.begin();
	int row = 1;
	for (; fit != props.end(); ++fit, ++row)
	{
		// time
		if (row == 1)
		{
			ASSERT((*fit).first.value == 0.);
		}
		s.Format(_T("%g"), (*fit).first.value);
		grid.SetItemText(row, TIME_COLUMN, s);

		// time units
		if ((*fit).first.input && ::strlen((*fit).first.input))
		{
			grid.SetItemText(row, UNIT_COLUMN, (*fit).first.input);
		}
		else
		{
			grid.SetItemText(row, UNIT_COLUMN, this->DefaultUnits);
		}
		if (row == 1)
		{
			grid.DisableCell(row, UNIT_COLUMN);
		}

		if ((*fit).second.type == PROP_UNDEFINED)
		{
			grid.SetItemText(row, TYPE_COLUMN, PSZ_NONE);
		}
		else if ((*fit).second.type == PROP_FIXED)
		{
			grid.SetItemText(row, TYPE_COLUMN, PSZ_CONSTANT);
		}
		else if ((*fit).second.type == PROP_LINEAR)
		{
			grid.SetItemText(row, TYPE_COLUMN, PSZ_LINEAR);
		}

		if (row == cell.row)
		{
			this->DDX_Property(pDX, (*fit).second);
		}
	}

	ASSERT(grid.GetFixedColumnCount() <= cell.col && cell.col < grid.GetColumnCount());
	ASSERT(grid.GetFixedRowCount() <= cell.row && cell.row < grid.GetRowCount());
	
	static NM_GRIDVIEW nmgv;
	nmgv.iRow         = cell.row;
	nmgv.iColumn      = cell.col;
	nmgv.hdr.hwndFrom = grid.GetSafeHwnd();
	nmgv.hdr.idFrom   = grid.GetDlgCtrlID();
	nmgv.hdr.code     = GVN_SELCHANGED;
	ASSERT(::IsWindow(nmgv.hdr.hwndFrom));

	CWnd *pOwner = grid.GetOwner();
	if (pOwner && ::IsWindow(pOwner->m_hWnd))
	{
		pOwner->PostMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
	}
	grid.RedrawWindow();
}


void CFluxPropsPage2::DDX_Series(CDataExchange* pDX, CModGridCtrlEx &grid, std::vector<Ctime*> &times, std::vector<Cproperty*> &props)
{
	ASSERT(!pDX->m_bSaveAndValidate);

	CString s;
	CCellID cell = grid.GetFocusCell();
	//CTimeSeries<Cproperty>::iterator fit = props.begin();
	//for (int row = 1; fit != props.end(); ++fit, ++row)
	ASSERT(times.size() == props.size());
	for (int row = 1; row < grid.GetRowCount(); ++row)
	{
		// time
		if (times[row])
		{
			if (row == 1)
			{
				ASSERT(times[row]->value == 0.);
			}

			// time value
			s.Format(_T("%g"), times[row]->value);
			grid.SetItemText(row, TIME_COLUMN, s);

			// time units
			if (times[row]->input && ::strlen(times[row]->input))
			{
				grid.SetItemText(row, UNIT_COLUMN, times[row]->input);
			}
			else
			{
				grid.SetItemText(row, UNIT_COLUMN, this->DefaultUnits);
			}
		}


		if (row == 1)
		{
			grid.DisableCell(row, UNIT_COLUMN);
		}

		// prop
		if (props[row])
		{
			// prop type
			if (props[row]->type == PROP_UNDEFINED)
			{
				grid.SetItemText(row, TYPE_COLUMN, PSZ_NONE);
			}
			else if (props[row]->type == PROP_FIXED)
			{
				grid.SetItemText(row, TYPE_COLUMN, PSZ_CONSTANT);
			}
			else if (props[row]->type == PROP_LINEAR)
			{
				grid.SetItemText(row, TYPE_COLUMN, PSZ_LINEAR);
			}

			if (row == cell.row)
			{
				this->DDX_Property(pDX, *props[row]);
			}
		}
	}

	ASSERT(grid.GetFixedColumnCount() <= cell.col && cell.col < grid.GetColumnCount());
	ASSERT(grid.GetFixedRowCount() <= cell.row && cell.row < grid.GetRowCount());
	
	static NM_GRIDVIEW nmgv;
	nmgv.iRow         = cell.row;
	nmgv.iColumn      = cell.col;
	nmgv.hdr.hwndFrom = grid.GetSafeHwnd();
	nmgv.hdr.idFrom   = grid.GetDlgCtrlID();
	nmgv.hdr.code     = GVN_SELCHANGED;
	ASSERT(::IsWindow(nmgv.hdr.hwndFrom));

	CWnd *pOwner = grid.GetOwner();
	if (pOwner && ::IsWindow(pOwner->m_hWnd))
	{
		pOwner->PostMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
	}
	grid.RedrawWindow();
}


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

// COMMENT: {6/15/2009 4:28:33 PM}	NEW_PROP(this->BC.current_bc_flux);
// COMMENT: {6/15/2009 4:28:33 PM}	NEW_PROP(this->BC.current_bc_solution);

	ASSERT(this->FluxTimes.size() == 0);
	ASSERT(this->FluxVector.size() == 0);
	ASSERT(this->FluxVectorConstant.size() == 0);
	ASSERT(this->FluxVectorLinear.size() == 0);

	this->FluxTimes.resize(MIN_ROWS + 1, 0);
	this->FluxVector.resize(MIN_ROWS + 1, 0);
	this->FluxVectorConstant.resize(MIN_ROWS + 1, 0);
	this->FluxVectorLinear.resize(MIN_ROWS + 1, 0);

	CTimeSeries<Cproperty>::iterator fit = this->BC.m_bc_flux.begin();
	for (int row = 1; fit != this->BC.m_bc_flux.end(); ++fit, ++row)
	{
		// Ctime
		this->FluxTimes[row] = new Ctime((*fit).first);

		// Cproperty
		if ((*fit).second.type == PROP_UNDEFINED)
		{
			ASSERT(FALSE);
		}
		else if ((*fit).second.type == PROP_FIXED)
		{
			this->FluxVector[row] = new Cproperty((*fit).second);
			this->FluxVectorConstant[row] = new Cproperty((*fit).second);
		}
		else if ((*fit).second.type == PROP_LINEAR)
		{
			this->FluxVector[row] = new Cproperty((*fit).second);
			this->FluxVectorLinear[row] = new Cproperty((*fit).second);
		}
	}

	ASSERT(this->SolutionTimes.size() == 0);
	ASSERT(this->SolutionVector.size() == 0);
	ASSERT(this->SolutionVectorConstant.size() == 0);
	ASSERT(this->SolutionVectorLinear.size() == 0);

	this->SolutionTimes.resize(MIN_ROWS + 1, 0);
	this->SolutionVector.resize(MIN_ROWS + 1, 0);
	this->SolutionVectorConstant.resize(MIN_ROWS + 1, 0);
	this->SolutionVectorLinear.resize(MIN_ROWS + 1, 0);

	CTimeSeries<Cproperty>::iterator sit = this->BC.m_bc_solution.begin();
	for (int row = 1; sit != this->BC.m_bc_solution.end(); ++sit, ++row)
	{
		// Ctime
		this->SolutionTimes[row] = new Ctime((*sit).first);

		// Cproperty
		if ((*sit).second.type == PROP_UNDEFINED)
		{
			ASSERT(FALSE);
		}
		else if ((*sit).second.type == PROP_FIXED)
		{
			this->SolutionVector[row] = new Cproperty((*sit).second);
			this->SolutionVectorConstant[row] = new Cproperty((*sit).second);
		}
		else if ((*sit).second.type == PROP_LINEAR)
		{
			this->SolutionVector[row] = new Cproperty((*sit).second);
			this->SolutionVectorLinear[row] = new Cproperty((*sit).second);
		}
	}

	TRACE("Out %s\n", __FUNCTION__);
}

#define DELETE_PROP(P) \
do { \
	if (P->type == PROP_UNDEFINED) { \
		Cproperty::CopyProperty(&P, 0); \
	} \
} while (0)

void CFluxPropsPage2::GetProperties(CBC& rBC)const
{
	TRACE("In %s\n", __FUNCTION__);
	rBC = this->BC;

	// delete inactive properties
	//
	DELETE_PROP(rBC.current_bc_flux);
	DELETE_PROP(rBC.current_bc_solution);
	TRACE("Out %s\n", __FUNCTION__);
}

BEGIN_MESSAGE_MAP(CFluxPropsPage2, CPropertyPage)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_TIMES_GRID, OnEndLabelEditFlux)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_SOLUTION, OnEndLabelEditSolution)

	ON_NOTIFY(GVN_SELCHANGED, IDC_TIMES_GRID,    OnSelChangedFlux)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_SOLUTION, OnSelChangedSolution)


	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGED,  IDC_PROP_TREE, OnTreeSelChanged)
	ON_EN_SETFOCUS(IDC_DESC_EDIT, &CFluxPropsPage2::OnEnSetfocusDescEdit)
END_MESSAGE_MAP()


// CFluxPropsPage2 message handlers

void CFluxPropsPage2::SetMode(CFluxPropsPage2::ModeType mode)
{
	CString time;
	CString units;

	if (this->ItemDDX == this->htiFLUX)
	{
		CCellID id = this->GridFlux.GetFocusCell();
		time = this->GridFlux.GetItemText(id.row, TIME_COLUMN);
		units = this->GridFlux.GetItemText(id.row, UNIT_COLUMN);
	}
	else if (this->ItemDDX == this->htiSOLUTION)
	{
		CCellID id = this->GridSolution.GetFocusCell();
		time = this->GridSolution.GetItemText(id.row, TIME_COLUMN);
		units = this->GridSolution.GetItemText(id.row, UNIT_COLUMN);
	}

	if (!units.Trim().IsEmpty() && !time.Trim().IsEmpty())
	{
		CString str;
		str.Format("%s %s", time, units);
		time = str;
	}

	if (mode == CFluxPropsPage2::NONE)
	{
		this->PropertyGroupBox.SetWindowText(_T(""));
		this->ShowConstant(false);
		this->ShowLinear(false);
	}
	else if (mode == CFluxPropsPage2::CONSTANT)
	{
		CString str;
		if (time.Trim().IsEmpty())
		{
			str.Format("%s", PSZ_CONSTANT);
		}
		else
		{
			str.Format("%s (t=%s)", PSZ_CONSTANT, time);
		}
		this->PropertyGroupBox.SetWindowText(str);
		this->ShowConstant(true);
		this->ShowLinear(false);
	}
	else if (mode == CFluxPropsPage2::LINEAR)
	{
		CString str;
		if (time.Trim().IsEmpty())
		{
			str.Format("%s", PSZ_LINEAR);
		}
		else
		{
			str.Format("%s (t=%s)", PSZ_LINEAR, time);
		}
		this->PropertyGroupBox.SetWindowText(str);
		this->ShowConstant(false);
		this->ShowLinear(true);
	}
}

void CFluxPropsPage2::ShowConstant(bool show)
{
	if (show)
	{
		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE_STATIC))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE_EDIT))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE_STATIC))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE_EDIT))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
	}
}

void CFluxPropsPage2::ShowLinear(bool show)
{
	if (show)
	{
		if (CWnd* pWnd = this->GetDlgItem(IDC_STATIC_DIRECTION))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_X_RADIO))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_Y_RADIO))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_Z_RADIO))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		
		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE1_STATIC))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE1_EDIT))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_DISTANCE1_STATIC))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_DISTANCE1_EDIT))
		{
			pWnd->ShowWindow(SW_SHOW);
		}

		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE2_STATIC))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE2_EDIT))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_DISTANCE2_STATIC))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_DISTANCE2_EDIT))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if (CWnd* pWnd = this->GetDlgItem(IDC_STATIC_DIRECTION))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_X_RADIO))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_Y_RADIO))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_Z_RADIO))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		
		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE1_STATIC))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE1_EDIT))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_DISTANCE1_STATIC))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_DISTANCE1_EDIT))
		{
			pWnd->ShowWindow(SW_HIDE);
		}

		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE2_STATIC))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_VALUE2_EDIT))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_DISTANCE2_STATIC))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->GetDlgItem(IDC_DISTANCE2_EDIT))
		{
			pWnd->ShowWindow(SW_HIDE);
		}

	}
}

void CFluxPropsPage2::OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	this->OnEndLabelEdit(pNotifyStruct, result, this->GridFlux, this->FluxVectors);
}

void CFluxPropsPage2::OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	this->OnEndLabelEdit(pNotifyStruct, result, this->GridSolution, this->SolutionVectors);
}

void CFluxPropsPage2::OnEndLabelEdit(NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors)
{
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	CString str = grid.GetItemText(pnmgv->iRow, pnmgv->iColumn);

	// Add your control notification handler code here
	if (grid.GetSafeHwnd())
	{
		if (pnmgv->iColumn == TIME_COLUMN)
		{
			CString units = grid.GetItemText(pnmgv->iRow, UNIT_COLUMN);
			if (units.Trim().IsEmpty())
			{
				// look upwards to find default units
				CString u = this->DefaultUnits;
				for (int r = pnmgv->iRow - 1; r >= 0; --r)
				{
					CString d = grid.GetItemText(r, UNIT_COLUMN);
					if (!d.IsEmpty() && CGlobal::GetStdTimeUnitsSafe(d).size() > 0)
					{
						u = CGlobal::GetStdTimeUnitsSafe(d).c_str();
						break;
					}					
				}
				grid.SetItemText(pnmgv->iRow, UNIT_COLUMN, u);
			}

			CString type = grid.GetItemText(pnmgv->iRow, TYPE_COLUMN);
			if (type.Trim().IsEmpty())
			{
				// look upwards to find default type
				for (int r = pnmgv->iRow - 1; r > 0; --r)
				{
					CString d = grid.GetItemText(r, TYPE_COLUMN);

					if (d.CompareNoCase(PSZ_NONE) == 0)
					{
						grid.SetItemText(pnmgv->iRow, TYPE_COLUMN, d);
						this->SetMode(CFluxPropsPage2::NONE);
						break;
					}
					else if (d.CompareNoCase(PSZ_CONSTANT) == 0)
					{
						CFluxPropsPage2::ModeType mt = CFluxPropsPage2::CONSTANT;
						grid.SetItemText(pnmgv->iRow, TYPE_COLUMN, d);
						if (!(*vectors[mt])[pnmgv->iRow])
						{
							(*vectors[mt])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
						}
						///{{
						delete (*vectors[SELECTED])[pnmgv->iRow];
						(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
						///}}
						this->SetMode(mt);
						break;
					}
					else if (d.CompareNoCase(PSZ_LINEAR) == 0)
					{
						CFluxPropsPage2::ModeType mt = CFluxPropsPage2::LINEAR;
						grid.SetItemText(pnmgv->iRow, TYPE_COLUMN, d);
						if (!(*vectors[mt])[pnmgv->iRow])
						{
							(*vectors[mt])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
						}
						///{{
						delete (*vectors[SELECTED])[pnmgv->iRow];
						(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
						///}}
						this->SetMode(mt);
						break;
					}
					else if (d.CompareNoCase(PSZ_POINTS) == 0)
					{
						CFluxPropsPage2::ModeType mt = CFluxPropsPage2::POINTS;
						grid.SetItemText(pnmgv->iRow, TYPE_COLUMN, d);
						if (!(*vectors[mt])[pnmgv->iRow])
						{
							(*vectors[mt])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
						}
						///{{
						delete (*vectors[SELECTED])[pnmgv->iRow];
						(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
						///}}
						this->SetMode(mt);
						break;
					}
					else if (d.CompareNoCase(PSZ_XYZ) == 0)
					{
						CFluxPropsPage2::ModeType mt = CFluxPropsPage2::XYZ;
						grid.SetItemText(pnmgv->iRow, TYPE_COLUMN, d);
						if (!(*vectors[mt])[pnmgv->iRow])
						{
							(*vectors[mt])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
						}
						///{{
						delete (*vectors[SELECTED])[pnmgv->iRow];
						(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[r]);
						///}}
						this->SetMode(mt);
						break;
					}
				}
			}
		}
		if (pnmgv->iColumn == TYPE_COLUMN)
		{
			CFluxPropsPage2::ModeType mt = CFluxPropsPage2::NONE;
			if (str.Compare(PSZ_CONSTANT) == 0)
			{
				mt = CFluxPropsPage2::CONSTANT;
				if (!(*vectors[mt])[pnmgv->iRow])
				{
					Cproperty def(PROP_FIXED);
					for (int r = pnmgv->iRow - 1; r > 0; --r)
					{
						CString d = grid.GetItemText(r, TYPE_COLUMN);
						if (d.CompareNoCase(PSZ_CONSTANT) == 0)
						{
							ASSERT((*vectors[mt])[r]);
							def = (*(*vectors[mt])[r]);
							break;
						}
					}
					(*vectors[mt])[pnmgv->iRow] = new Cproperty(def);
					///{{
					delete (*vectors[SELECTED])[pnmgv->iRow];
					(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(def);
					///}}
				}
			}
			else if (str.Compare(PSZ_LINEAR) == 0)
			{
				mt = CFluxPropsPage2::LINEAR;
				if (!(*vectors[mt])[pnmgv->iRow])
				{
					Cproperty def(PROP_LINEAR);
					for (int r = pnmgv->iRow - 1; r > 0; --r)
					{
						CString d = grid.GetItemText(r, TYPE_COLUMN);
						if (d.CompareNoCase(PSZ_LINEAR) == 0)
						{
							ASSERT((*vectors[mt])[r]);
							def = (*(*vectors[mt])[r]);
							break;
						}
					}
					(*vectors[mt])[pnmgv->iRow] = new Cproperty(def);
					///{{
					delete (*vectors[SELECTED])[pnmgv->iRow];
					(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(def);
					///}}
				}
			}
			//{{
			this->UpdateData(FALSE);
			//}}
			this->SetMode(mt);
		}
	}
}

void CFluxPropsPage2::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemOld.hItem;
	if (this->ItemDDX)
	{
		if (!this->UpdateData(TRUE))
		{
// COMMENT: {6/9/2009 8:49:59 PM}			// notify which control caused failure
// COMMENT: {6/9/2009 8:49:59 PM}			//
// COMMENT: {6/9/2009 8:49:59 PM}			CWnd* pFocus = CWnd::GetFocus();
// COMMENT: {6/9/2009 8:49:59 PM}			::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

			// disallow change
			//
			*pResult = TRUE;
			TRACE("Out Disallowed %s\n", __FUNCTION__);
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
	this->RichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
}

void CFluxPropsPage2::OnSelChangedFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	this->OnSelChanged(pNotifyStruct, result, this->GridFlux);
}

void CFluxPropsPage2::OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	this->OnSelChanged(pNotifyStruct, result, this->GridSolution);
}

void CFluxPropsPage2::OnSelChanged(NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid)
{
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	TRACE("CFluxPropsPage2::OnSelChanged row = %d\n", pnmgv->iRow);

	if (grid.GetSafeHwnd())
	{
		CString str = grid.GetItemText(pnmgv->iRow, TYPE_COLUMN);
		if (this->ItemDDX == this->htiFLUX)
		{
			ASSERT(&grid == &this->GridFlux);
			this->DDX_Vectors(pnmgv->iRow, str, this->FluxVectors);
		}
		else if (this->ItemDDX == this->htiSOLUTION)
		{
			ASSERT(&grid == &this->GridSolution);
			this->DDX_Vectors(pnmgv->iRow, str, this->SolutionVectors);
		}
	}
}

void CFluxPropsPage2::DDX_Vectors(int row, CString &str, std::vector< std::vector<Cproperty*>* > &vectors)
{
	CDataExchange dx(this, FALSE);

	CFluxPropsPage2::ModeType mode = CFluxPropsPage2::NONE;
	if (str.Compare(PSZ_CONSTANT) == 0)
	{
		mode = CFluxPropsPage2::CONSTANT;
		if ((*vectors[CONSTANT])[row])
		{
			this->DDX_Property(&dx, *(*vectors[CONSTANT])[row]);
		}
	}
	else if (str.Compare(PSZ_LINEAR) == 0)
	{
		mode = CFluxPropsPage2::LINEAR;
		if ((*vectors[LINEAR])[row])
		{
			this->DDX_Property(&dx, *(*vectors[LINEAR])[row]);
		}
	}
	this->SetMode(mode);
}

void CFluxPropsPage2::InitializeGrid(CDataExchange* pDX, CModGridCtrlEx &grid, CTimeSeries<Cproperty> &series)
{
	// Prepare Grid
	//
	if (!pDX->m_bSaveAndValidate && grid.GetColumnCount() == 0)
	{
		TRY
		{
			grid.SetRowCount(MIN_ROWS/* + this->m_well.count_depth_user + this->m_well.count_elevation_user*/);
			grid.SetColumnCount(3);
			grid.SetFixedRowCount(1);
			grid.SetFixedColumnCount(0);
			grid.EnableTitleTips(FALSE);
			grid.SetCurrentFocusCell(1, 2);

			///CCellRange FluxCellRange = grid.GetSelectedCellRange();

			// grid.EnableSelection(FALSE); // this breaks CutSelectedText
			grid.SetHighLight(GV_HIGHLIGHT_ALWAYS);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

		// set default format
		for (int row = 0; row < grid.GetRowCount(); ++row)
		{
			for (int col = 0; col < grid.GetColumnCount(); ++col)
			{
				grid.SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
			}
		}
		///this->SetScreenHeadings(this->m_bByDepth);
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = 0;

			Item.col = 0;
			Item.szText.Format(_T("Time"));
			grid.SetItem(&Item);

			Item.col = 1;
			Item.szText.Format(_T("Units"));
			grid.SetItem(&Item);

			Item.col = 2;
			Item.szText.Format(_T("Type"));
			grid.SetItem(&Item);

			grid.SetItemText(1, 0, _T("0"));
			grid.DisableCell(1, 0);

			grid.DisableCell(1, 1);

			grid.SetItemText(1, 2, PSZ_NONE);
			this->SetMode(CFluxPropsPage2::NONE);

			//{{
			std::vector<LPCTSTR> vecTimeUnits;
			vecTimeUnits.push_back(PSZ_SECS);
			vecTimeUnits.push_back(PSZ_MINS);
			vecTimeUnits.push_back(PSZ_HOUR);
			vecTimeUnits.push_back(PSZ_DAYS);
			vecTimeUnits.push_back(PSZ_YEAR);

			grid.SetColumnOptions(1, vecTimeUnits);
			//}}

			//{{
			std::vector<LPCTSTR> vecPropType;
			vecPropType.push_back(PSZ_NONE);
			vecPropType.push_back(PSZ_CONSTANT);
			vecPropType.push_back(PSZ_LINEAR);
// COMMENT: {6/10/2009 3:34:31 PM}			vecPropType.push_back(PSZ_POINTS);
// COMMENT: {6/10/2009 3:34:31 PM}			vecPropType.push_back(PSZ_XYZ);
			grid.SetColumnOptions(2, vecPropType);
			//}}

			grid.SetColumnWidth(0, 71);
			grid.SetColumnWidth(1, 62);
			grid.SetColumnWidth(2, 70);
		}
		///grid.ExpandColumnsToFit();
// COMMENT: {6/19/2009 3:05:26 PM}		//{{
// COMMENT: {6/19/2009 3:05:26 PM}		this->DDX_Series(pDX, grid, series);
// COMMENT: {6/19/2009 3:05:26 PM}		//}}
	}
}

