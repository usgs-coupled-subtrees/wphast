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
	TRACE("In %s\n", __FUNCTION__);
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

	this->FluxValidationRow = -1;
	this->FluxVectors.push_back(&this->FluxVector);
	this->FluxVectors.push_back(&this->FluxVectorConstant);
	this->FluxVectors.push_back(&this->FluxVectorLinear);

	this->SolutionValidationRow = -1;
	this->SolutionVectors.push_back(&this->SolutionVector);
	this->SolutionVectors.push_back(&this->SolutionVectorConstant);
	this->SolutionVectors.push_back(&this->SolutionVectorLinear);
	TRACE("Out %s\n", __FUNCTION__);
}

CFluxPropsPage2::~CFluxPropsPage2()
{
	TRACE("In %s\n", __FUNCTION__);
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
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);
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

	DDX_GridControl(pDX, IDC_TIMES_GRID,    this->FluxGrid);
	DDX_GridControl(pDX, IDC_GRID_SOLUTION, this->SolutionGrid);

	this->InitializeGrid(pDX, this->FluxGrid,     this->BC.m_bc_flux);
	this->InitializeGrid(pDX, this->SolutionGrid, this->BC.m_bc_solution);


	if (pDX->m_bSaveAndValidate)
	{
		if (this->ItemDDX == this->htiFLUX)
		{
			this->DDX_Series(pDX, this->FluxGrid, this->FluxTimes, *(this->FluxVectors[SELECTED]));
			ASSERT(!this->SolutionGrid.IsWindowVisible());
			ASSERT(this->FluxGrid.IsWindowVisible());
		}
		else if (this->ItemDDX == this->htiSOLUTION)
		{
			this->DDX_Series(pDX, this->SolutionGrid, this->SolutionTimes, *(this->SolutionVectors[SELECTED]));
			ASSERT(this->SolutionGrid.IsWindowVisible());
			ASSERT(!this->FluxGrid.IsWindowVisible());
		}
	}
	else
	{
		if (this->ItemDDX == this->htiFLUX)
		{
			//this->DDX_Series(pDX, this->FluxGrid, this->BC.m_bc_flux);
			this->DDX_Series(pDX, this->FluxGrid, this->FluxTimes, *(this->FluxVectors[SELECTED]));
			this->SolutionGrid.ShowWindow(SW_HIDE);
			this->FluxGrid.ShowWindow(SW_SHOW);
		}
		else if (this->ItemDDX == this->htiSOLUTION)
		{
			//this->DDX_Series(pDX, this->SolutionGrid, this->BC.m_bc_solution);
			this->DDX_Series(pDX, this->SolutionGrid, this->SolutionTimes, *(this->SolutionVectors[SELECTED]));
			this->FluxGrid.ShowWindow(SW_HIDE);
			this->SolutionGrid.ShowWindow(SW_SHOW);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::DDX_Property(CDataExchange* pDX, Cproperty &p)
{
	TRACE("In %s\n", __FUNCTION__);
	CString strEmpty(_T(""));

	if (pDX->m_bSaveAndValidate)
	{
		ASSERT(FALSE);
	}
	else
	{
		if (p.type == PROP_UNDEFINED)
		{
		}
		else if (p.type == PROP_FIXED)
		{
			CGlobal::DDX_Text_Safe(pDX, IDC_VALUE_EDIT, p.v[0]);
		}
		else if (p.type == PROP_LINEAR)
		{
			//this->FluxGrid.SetItemText(row, TYPE_COLUMN, PSZ_LINEAR);
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
			CGlobal::DDX_Text_Safe(pDX, IDC_VALUE1_EDIT,    p.v[0]);
			CGlobal::DDX_Text_Safe(pDX, IDC_DISTANCE1_EDIT, p.dist1);
			CGlobal::DDX_Text_Safe(pDX, IDC_VALUE2_EDIT,    p.v[1]);
			CGlobal::DDX_Text_Safe(pDX, IDC_DISTANCE2_EDIT, p.dist2);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::DDX_Series(CDataExchange* pDX, CModGridCtrlEx &grid, CTimeSeries<Cproperty> &props)
{
	TRACE("In %s\n", __FUNCTION__);
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
	TRACE("Out %s\n", __FUNCTION__);
}


void CFluxPropsPage2::DDX_Series(CDataExchange* pDX, CModGridCtrlEx &grid, std::vector<Ctime*> &times, std::vector<Cproperty*> &props)
{
	TRACE("In %s\n", __FUNCTION__);
	if (pDX->m_bSaveAndValidate)
	{
		std::vector<Ctime*> new_times;
		std::vector<Cproperty*> new_props;
		for (int row = 1; row < grid.GetRowCount(); ++row)
		{
			CString strValue;
			double value;
			Ctime time;

			strValue = grid.GetItemText(row, TIME_COLUMN);
			if (!strValue.Trim().IsEmpty())
			{
				// time value
				DDX_TextGridControl(pDX, grid.GetDlgCtrlID(), row, TIME_COLUMN, value);
				if (value < 0.0)
				{
					::DDX_GridControlFail(pDX, grid.GetDlgCtrlID(), row, TIME_COLUMN, _T("Start time must be positive."));
				}
				time.SetValue(value);

				// time units
				DDX_TextGridControl(pDX, grid.GetDlgCtrlID(), row, UNIT_COLUMN, strValue);
				if (strValue.IsEmpty() || time.SetInput(strValue) != OK)
				{
					if (row != 1)
					{
						::DDX_GridControlFail(pDX, grid.GetDlgCtrlID(), row, UNIT_COLUMN, _T("Please enter the start time units."));
					}
				}

				// prop
				DDX_TextGridControl(pDX, grid.GetDlgCtrlID(), row, TYPE_COLUMN, strValue);
				if (strValue.Trim().IsEmpty() || strValue.CompareNoCase(PSZ_NONE) == 0)
				{
					if (row == 1)
					{
						CString string("A property must be defined for time zero.");
						::DDX_GridControlFail(pDX, grid.GetDlgCtrlID(), row, TYPE_COLUMN, string);
					}
					// (*vectors[SELECTED])[row]
				}
				else if (strValue.CompareNoCase(PSZ_CONSTANT) == 0)
				{
					CFluxPropsPage2::ModeType mt = CFluxPropsPage2::CONSTANT;
					if (props[row])
					{
						if (props[row]->v[0] != props[row]->v[0])
						{
							double d;
							grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_VALUE_EDIT, d);
						}
					}
					else
					{
						ASSERT(FALSE);
					}
				}
				else if (strValue.CompareNoCase(PSZ_LINEAR) == 0)
				{
					CFluxPropsPage2::ModeType mt = CFluxPropsPage2::LINEAR;
					if (props[row])
					{
						double d;
						if (props[row]->v[0] != props[row]->v[0])
						{
							grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_VALUE1_EDIT, d);
						}
						if (props[row]->dist1 != props[row]->dist1)
						{
							grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_DISTANCE1_EDIT, d);
						}
						if (props[row]->v[1] != props[row]->v[1])
						{
							grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_VALUE2_EDIT, d);
						}
						if (props[row]->dist2 != props[row]->dist2)
						{
							grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_DISTANCE2_EDIT, d);
						}
					}
					else
					{
						ASSERT(FALSE);
					}
				}
				else if (strValue.CompareNoCase(PSZ_POINTS) == 0)
				{
					CFluxPropsPage2::ModeType mt = CFluxPropsPage2::POINTS;
					// (*vectors[SELECTED])[row]
				}
				else if (strValue.CompareNoCase(PSZ_XYZ) == 0)
				{
					CFluxPropsPage2::ModeType mt = CFluxPropsPage2::XYZ;
					// (*vectors[SELECTED])[row]
				}
			}
		}
	}
	else
	{
		CString s;
		CCellID cell = grid.GetFocusCell();
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
	TRACE("Out %s\n", __FUNCTION__);
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

	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
END_MESSAGE_MAP()


// CFluxPropsPage2 message handlers

void CFluxPropsPage2::SetMode(CFluxPropsPage2::ModeType mode)
{
	TRACE("In %s\n", __FUNCTION__);
	CString time;
	CString units;

	if (this->ItemDDX == this->htiFLUX)
	{
		CCellID id = this->FluxGrid.GetFocusCell();
		time = this->FluxGrid.GetItemText(id.row, TIME_COLUMN);
		units = this->FluxGrid.GetItemText(id.row, UNIT_COLUMN);
	}
	else if (this->ItemDDX == this->htiSOLUTION)
	{
		CCellID id = this->SolutionGrid.GetFocusCell();
		time = this->SolutionGrid.GetItemText(id.row, TIME_COLUMN);
		units = this->SolutionGrid.GetItemText(id.row, UNIT_COLUMN);
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
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::ShowConstant(bool show)
{
	TRACE("In %s\n", __FUNCTION__);
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
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::ShowLinear(bool show)
{
	TRACE("In %s\n", __FUNCTION__);
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
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
	this->OnEndLabelEdit(pNotifyStruct, result, this->FluxGrid, this->FluxVectors);
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
	this->OnEndLabelEdit(pNotifyStruct, result, this->SolutionGrid, this->SolutionVectors);
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnEndLabelEdit(NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors)
{
	TRACE("In %s\n", __FUNCTION__);
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
// COMMENT: {6/24/2009 3:08:51 PM}					///{{
// COMMENT: {6/24/2009 3:08:51 PM}					delete (*vectors[SELECTED])[pnmgv->iRow];
// COMMENT: {6/24/2009 3:08:51 PM}					(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(def);
// COMMENT: {6/24/2009 3:08:51 PM}					///}}
				}
				///{{
				delete (*vectors[SELECTED])[pnmgv->iRow];
				(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[pnmgv->iRow]);
				///}}
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
// COMMENT: {6/24/2009 3:07:09 PM}					///{{
// COMMENT: {6/24/2009 3:07:09 PM}					delete (*vectors[SELECTED])[pnmgv->iRow];
// COMMENT: {6/24/2009 3:07:09 PM}					(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(def);
// COMMENT: {6/24/2009 3:07:09 PM}					///}}
				}
				///{{
				delete (*vectors[SELECTED])[pnmgv->iRow];
				(*vectors[SELECTED])[pnmgv->iRow] = new Cproperty(*(*vectors[mt])[pnmgv->iRow]);
				///}}
			}
			//{{
			////{{{{
			if (mt != CFluxPropsPage2::NONE)
			{
			////}}}}
				this->UpdateData(FALSE);
			////{{{{
			}
			////}}}}
			//}}
			this->SetMode(mt);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemOld.hItem;
	if (this->ItemDDX)
	{
		//{{{{
		if (this->ItemDDX == this->htiFLUX)
		{
			if (this->FluxValidationRow > 0)
			{
				this->DDV_SoftValidate(this->FluxGrid, this->FluxVectors, this->FluxValidationRow);
			}
		}
		else if (this->ItemDDX == this->htiSOLUTION)
		{
			if (this->SolutionValidationRow > 0)
			{
				this->DDV_SoftValidate(this->SolutionGrid, this->SolutionVectors, this->SolutionValidationRow);
			}
		}
		//}}}}
		//////{{
		if (!this->UpdateData(TRUE))
		{
			// notify which control caused failure
			//
			CWnd* pFocus = CWnd::GetFocus();
			this->PostMessage(UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);
			//::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);


			// disallow change
			//
			*pResult = TRUE;
			TRACE("Out %s Disallowed\n", __FUNCTION__);
			return;
		}
		//////}}

// COMMENT: {6/23/2009 9:16:54 PM}		if (!this->UpdateData(TRUE))
// COMMENT: {6/23/2009 9:16:54 PM}		{
// COMMENT: {6/23/2009 9:16:54 PM}// COMMENT: {6/9/2009 8:49:59 PM}			// notify which control caused failure
// COMMENT: {6/23/2009 9:16:54 PM}// COMMENT: {6/9/2009 8:49:59 PM}			//
// COMMENT: {6/23/2009 9:16:54 PM}// COMMENT: {6/9/2009 8:49:59 PM}			CWnd* pFocus = CWnd::GetFocus();
// COMMENT: {6/23/2009 9:16:54 PM}// COMMENT: {6/9/2009 8:49:59 PM}			::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);
// COMMENT: {6/23/2009 9:16:54 PM}
// COMMENT: {6/23/2009 9:16:54 PM}			// disallow change
// COMMENT: {6/23/2009 9:16:54 PM}			//
// COMMENT: {6/23/2009 9:16:54 PM}			*pResult = TRUE;
// COMMENT: {6/23/2009 9:16:54 PM}			TRACE("Out Disallowed %s\n", __FUNCTION__);
// COMMENT: {6/23/2009 9:16:54 PM}			return;
// COMMENT: {6/23/2009 9:16:54 PM}		}
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
	////{{
	// soft-validate
	if (this->FluxValidationRow > 0)
	{
		// TODO skip validation if (pnmgv->iRow == FluxValidationRow)
		this->DDV_SoftValidate(this->FluxGrid, this->FluxVectors, this->FluxValidationRow);
	}
	////}}

	this->OnSelChanged(pNotifyStruct, result, this->FluxGrid);
	//{{
	// set next validation row
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->FluxValidationRow = pnmgv->iRow;
	//}}
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
	////{{
	// soft-validate
	if (this->SolutionValidationRow > 0)
	{
		// TODO skip validation if (pnmgv->iRow == SolutionValidationRow)
		this->DDV_SoftValidate(this->SolutionGrid, this->SolutionVectors, this->SolutionValidationRow);
	}
	////}}

	this->OnSelChanged(pNotifyStruct, result, this->SolutionGrid);

	//{{
	// set next validation row
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->SolutionValidationRow = pnmgv->iRow;
	//}}
	TRACE("Out %s\n", __FUNCTION__);
}

//{{
void CFluxPropsPage2::DDV_SoftValidate(CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors, int row)
{
	TRACE("In %s\n", __FUNCTION__);
	if (grid.GetSafeHwnd())
	{
		CString str = grid.GetItemText(row, TYPE_COLUMN);

		if (str.Trim().IsEmpty() || str.CompareNoCase(PSZ_NONE) == 0)
		{
			delete (*vectors[SELECTED])[row];
			(*vectors[SELECTED])[row] = 0;
		}
		else if (str.Compare(PSZ_CONSTANT) == 0)
		{
			CDataExchange dx(this, TRUE);

			ASSERT((*vectors[SELECTED])[row]);
			ASSERT((*vectors[CONSTANT])[row]);
			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE_EDIT, (*vectors[SELECTED])[row]->v[0]);
			*(*vectors[CONSTANT])[row] = *(*vectors[SELECTED])[row];
			//mode = CFluxPropsPage2::CONSTANT;
		}
		else if (str.Compare(PSZ_LINEAR) == 0)
		{
			CDataExchange dx(this, TRUE);

			ASSERT((*vectors[SELECTED])[row]);
			ASSERT((*vectors[LINEAR])[row]);

			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE1_EDIT,    (*vectors[SELECTED])[row]->v[0]);
			CGlobal::DDX_Text_Safe(&dx, IDC_DISTANCE1_EDIT, (*vectors[SELECTED])[row]->dist1);
			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE2_EDIT,    (*vectors[SELECTED])[row]->v[1]);
			CGlobal::DDX_Text_Safe(&dx, IDC_DISTANCE2_EDIT, (*vectors[SELECTED])[row]->dist2);

			*(*vectors[LINEAR])[row] = *(*vectors[SELECTED])[row];

			//mode = CFluxPropsPage2::LINEAR;
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}
//}}

void CFluxPropsPage2::OnSelChanged(NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid)
{
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	TRACE("In %s row = %d\n", __FUNCTION__, pnmgv->iRow);

	if (grid.GetSafeHwnd())
	{
		CString str = grid.GetItemText(pnmgv->iRow, TYPE_COLUMN);
		if (this->ItemDDX == this->htiFLUX)
		{
			ASSERT(&grid == &this->FluxGrid);
			this->DDX_Vectors(pnmgv->iRow, str, this->FluxVectors);
		}
		else if (this->ItemDDX == this->htiSOLUTION)
		{
			ASSERT(&grid == &this->SolutionGrid);
			this->DDX_Vectors(pnmgv->iRow, str, this->SolutionVectors);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::DDX_Vectors(int row, CString &str, std::vector< std::vector<Cproperty*>* > &vectors)
{
	TRACE("In %s\n", __FUNCTION__);
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
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::InitializeGrid(CDataExchange* pDX, CModGridCtrlEx &grid, CTimeSeries<Cproperty> &series)
{
	TRACE("In %s\n", __FUNCTION__);
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
	TRACE("Out %s\n", __FUNCTION__);
}

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
