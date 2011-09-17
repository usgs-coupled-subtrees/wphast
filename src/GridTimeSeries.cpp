#include "StdAfx.h"
#include "GridTimeSeries.h"

#include "Global.h"
#include "Units.h"
#include "FakeFiledata.h"

const TCHAR PSZ_NONE[]     = _T("None");
const TCHAR PSZ_CONSTANT[] = _T("Constant");
const TCHAR PSZ_LINEAR[]   = _T("Linear");
const TCHAR PSZ_POINTS[]   = _T("Points");
const TCHAR PSZ_XYZ[]      = _T("XYZ");
const TCHAR PSZ_XYZT[]     = _T("XYZT");


const TCHAR PSZ_SECS[] = _T("seconds");
const TCHAR PSZ_MINS[] = _T("minutes");
const TCHAR PSZ_HOUR[] = _T("hours");
const TCHAR PSZ_DAYS[] = _T("days");
const TCHAR PSZ_YEAR[] = _T("years");

const TCHAR PSZ_UNDEFINED[] = _T("UNDEFINED");

const int ITEM_COLUMN = 0;
const int TIME_COLUMN = 1;
const int UNIT_COLUMN = 2;
const int TYPE_COLUMN = 3;

const int X_COLUMN = 1;
const int Y_COLUMN = 2;
const int Z_COLUMN = 3;
const int V_COLUMN = 4;

const int MIN_ROWS = 500;
const int MIN_POINTS = 500;

const int SELECTED = 0;

#if defined(NEW_SINGLE_PROPERTY)
const int SINGLE = 0;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGridTimeSeries::CGridTimeSeries(CWnd* pWnd)
: DlgWnd(pWnd)
, validation_row(-1)
#if defined(NEW_SINGLE_PROPERTY)
, bSingleProperty(false)
#endif
{
	ASSERT(std::numeric_limits<double>::has_signaling_NaN);
	double d = std::numeric_limits<double>::signaling_NaN();
	ASSERT(d != d);
	ASSERT(::_isnan(d));

	ASSERT(std::numeric_limits<double>::has_infinity);
	double i = std::numeric_limits<double>::infinity();
	ASSERT(!::_finite(i));

	// times
	this->v_times.resize(MIN_ROWS + 1, 0);

	// property vectors
	this->v_selected.resize(MIN_ROWS + 1, 0);
	this->v_constant.resize(MIN_ROWS + 1, 0);
	this->v_linear.resize(MIN_ROWS + 1, 0);
	this->v_points.resize(MIN_ROWS + 1, 0);
	this->v_xyz.resize(MIN_ROWS + 1, 0);
	this->v_xyzt.resize(MIN_ROWS + 1, 0);

	// vector of property vectors
	this->vv_props.push_back(&this->v_selected);
	this->vv_props.push_back(&this->v_constant);
	this->vv_props.push_back(&this->v_linear);
	this->vv_props.push_back(&this->v_points);
	this->vv_props.push_back(&this->v_xyz);
	this->vv_props.push_back(&this->v_xyzt);

	this->DefaultTimeUnits = PSZ_SECS;
}

#if defined(NEW_SINGLE_PROPERTY)
CGridTimeSeries::CGridTimeSeries(CWnd* pWnd, bool bSingle, bool bMixture)
: DlgWnd(pWnd)
, validation_row(-1)
, bSingleProperty(bSingle)
, bEnableMixture(bMixture)
{
	ASSERT(std::numeric_limits<double>::has_signaling_NaN);
	double d = std::numeric_limits<double>::signaling_NaN();
	ASSERT(d != d);
	ASSERT(::_isnan(d));

	ASSERT(std::numeric_limits<double>::has_infinity);
	double i = std::numeric_limits<double>::infinity();
	ASSERT(!::_finite(i));

	if (this->bSingleProperty)
	{
		// property vectors
		this->v_selected.resize(1, 0);
		this->v_constant.resize(1, 0);
		this->v_linear.resize(1, 0);
		this->v_points.resize(1, 0);
		this->v_xyz.resize(1, 0);
		this->v_xyzt.resize(1, 0);

		// vector of property vectors
		this->vv_props.push_back(&this->v_selected);
		this->vv_props.push_back(&this->v_constant);
		this->vv_props.push_back(&this->v_linear);
		this->vv_props.push_back(&this->v_points);
		this->vv_props.push_back(&this->v_xyz);
		this->vv_props.push_back(&this->v_xyzt);

		this->validation_row = 1;
	}
	else
	{
		// times
		this->v_times.resize(MIN_ROWS + 1, 0);

		// property vectors
		this->v_selected.resize(MIN_ROWS + 1, 0);
		this->v_constant.resize(MIN_ROWS + 1, 0);
		this->v_linear.resize(MIN_ROWS + 1, 0);
		this->v_points.resize(MIN_ROWS + 1, 0);
		this->v_xyz.resize(MIN_ROWS + 1, 0);
		this->v_xyzt.resize(MIN_ROWS + 1, 0);

		// vector of property vectors
		this->vv_props.push_back(&this->v_selected);
		this->vv_props.push_back(&this->v_constant);
		this->vv_props.push_back(&this->v_linear);
		this->vv_props.push_back(&this->v_points);
		this->vv_props.push_back(&this->v_xyz);
		this->vv_props.push_back(&this->v_xyzt);
	}

	this->DefaultTimeUnits = PSZ_SECS;
}
#endif


CGridTimeSeries::~CGridTimeSeries(void)
{
	this->FreeVectors();
}

void CGridTimeSeries::FreeVectors()
{
	std::vector<Ctime*>::iterator t = this->v_times.begin();
	for (; t != this->v_times.end(); ++t)
	{
		delete (*t);
		(*t) = 0;
	}

	std::vector< std::vector<Cproperty*>* >::iterator vv = this->vv_props.begin();
	for (; vv != this->vv_props.end(); ++vv)
	{
		std::vector<Cproperty*>::iterator v = (*(*vv)).begin();
		for (; v != (*(*vv)).end(); ++v)
		{
			delete (*v);
			(*v) = 0;
		}
	}
}

void CGridTimeSeries::InitializeGrid(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);

#if defined(NEW_SINGLE_PROPERTY)
	if (this->bSingleProperty)
	{
		ASSERT(FALSE);
	}
#endif
	// Prepare Grid
	//
	if (!pDX->m_bSaveAndValidate && this->grid.GetColumnCount() == 0)
	{
		TRY
		{
// COMMENT: {7/8/2009 7:31:37 PM}			this->grid.SetColumnCount(3);
			this->grid.SetRowCount(MIN_ROWS+1);
			this->grid.SetColumnCount(4);
			this->grid.SetFixedRowCount(1);
// COMMENT: {7/8/2009 7:37:41 PM}			this->grid.SetFixedColumnCount(0);
			this->grid.SetFixedColumnCount(1);
			this->grid.EnableTitleTips(FALSE);
			this->grid.SetCurrentFocusCell(1, TYPE_COLUMN);
			this->validation_row = 1;

			this->grid.SetHighLight(GV_HIGHLIGHT_ALWAYS);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

// COMMENT: {7/8/2009 7:42:05 PM}		// set default format
// COMMENT: {7/8/2009 7:42:05 PM}		for (int row = this->grid.GetFixedRowCount(); row < this->grid.GetRowCount(); ++row)
// COMMENT: {7/8/2009 7:42:05 PM}		{
// COMMENT: {7/8/2009 7:42:05 PM}			for (int col = 0; col < this->grid.GetColumnCount(); ++col)
// COMMENT: {7/8/2009 7:42:05 PM}			{
// COMMENT: {7/8/2009 7:42:05 PM}				this->grid.SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
// COMMENT: {7/8/2009 7:42:05 PM}			}
// COMMENT: {7/8/2009 7:42:05 PM}		}

		//{{
		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask    = GVIF_FORMAT;
		defaultFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;

		GV_ITEM timeFormat;
		timeFormat.mask    = GVIF_FORMAT;
		timeFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;
		timeFormat.col     = TIME_COLUMN;

		GV_ITEM headItem;
		headItem.mask    = GVIF_TEXT|GVIF_FORMAT;
		headItem.nFormat = DT_CENTER|DT_VCENTER|DT_END_ELLIPSIS;
		headItem.col     = 0;

		for (int row = 0; row < this->grid.GetRowCount(); ++row)
		{
			defaultFormat.row = row;
			timeFormat.row = row;
			for (int col = 0; col < this->grid.GetColumnCount(); ++col)
			{ 
				if (col == ITEM_COLUMN)
				{
					headItem.row = row+1;
					headItem.szText.Format("%d", row+1);
					this->grid.SetItem(&headItem);
				}
				else if (col == TIME_COLUMN)
				{
					this->grid.SetItem(&timeFormat);
				}
				else
				{
					defaultFormat.col = col;
					this->grid.SetItem(&defaultFormat);
				}
			}
		}
		//}}

		{
			GV_ITEM Item;
// COMMENT: {7/8/2009 7:50:12 PM}			Item.mask = GVIF_TEXT;
			//{{
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;
			//}}
			Item.row = 0;

			Item.col = TIME_COLUMN;
			Item.szText.Format(_T("Time"));
			this->grid.SetItem(&Item);

			Item.col = UNIT_COLUMN;
			Item.szText.Format(_T("Units"));
			this->grid.SetItem(&Item);

			Item.col = TYPE_COLUMN;
			Item.szText.Format(_T("Type"));
			this->grid.SetItem(&Item);

			this->grid.SetItemText(1, TIME_COLUMN, _T("0"));
			this->grid.DisableCell(1, TIME_COLUMN);

			CString u = CGlobal::GetStdTimeUnitsSafe(this->DefaultTimeUnits).c_str();
			this->grid.SetItemText(1, UNIT_COLUMN, u);
			this->grid.DisableCell(1, UNIT_COLUMN);

			this->grid.SetItemText(1, TYPE_COLUMN, PSZ_NONE);

			// units column
			std::vector<LPCTSTR> vecTimeUnits;
			vecTimeUnits.push_back(PSZ_SECS);
			vecTimeUnits.push_back(PSZ_MINS);
			vecTimeUnits.push_back(PSZ_HOUR);
			vecTimeUnits.push_back(PSZ_DAYS);
			vecTimeUnits.push_back(PSZ_YEAR);
			this->grid.SetColumnOptions(UNIT_COLUMN, vecTimeUnits);

			// type column
			std::vector<LPCTSTR> vecPropType;
			vecPropType.push_back(PSZ_NONE);
			vecPropType.push_back(PSZ_CONSTANT);
			vecPropType.push_back(PSZ_LINEAR);
			vecPropType.push_back(PSZ_POINTS);
			vecPropType.push_back(PSZ_XYZ);
			vecPropType.push_back(PSZ_XYZT);
			this->grid.SetColumnOptions(TYPE_COLUMN, vecPropType);

// COMMENT: {7/8/2009 7:35:18 PM}			this->grid.SetColumnWidth(TIME_COLUMN, 71);
// COMMENT: {7/8/2009 7:35:18 PM}			this->grid.SetColumnWidth(UNIT_COLUMN, 62);
// COMMENT: {7/8/2009 7:35:18 PM}			this->grid.SetColumnWidth(TYPE_COLUMN, 70);
			//{{
			this->grid.SetColumnWidth(ITEM_COLUMN, 28);
			this->grid.SetColumnWidth(TIME_COLUMN, 71);
			this->grid.SetColumnWidth(UNIT_COLUMN, 62);
			this->grid.SetColumnWidth(TYPE_COLUMN, 70);
			//this->grid.ResetScrollBars();
			this->grid.SetRedraw(TRUE, TRUE); // force a reset scroll bars
			//}}
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::ShowConstant(bool show)
{
	TRACE("In %s\n", __FUNCTION__);

	static UINT IDs[] =
	{
		IDC_VALUE_STATIC,
		IDC_VALUE_EDIT,
	};

	size_t n = sizeof(IDs) / sizeof(IDs[0]);
	CWnd *pWnd;
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = this->DlgWnd->GetDlgItem(IDs[i]);
		ASSERT(pWnd && pWnd->GetSafeHwnd());
		if (pWnd)
		{
			if (show)
			{
				pWnd->ShowWindow(SW_SHOW);
			}
			else
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::ShowLinear(bool show)
{
	TRACE("In %s\n", __FUNCTION__);

	static UINT IDs[] =
	{
		IDC_STATIC_DIRECTION,
		IDC_X_RADIO,
		IDC_Y_RADIO,
		IDC_Z_RADIO,
		IDC_VALUE1_STATIC,
		IDC_VALUE1_EDIT,
		IDC_DISTANCE1_STATIC,
		IDC_DISTANCE1_EDIT,
		IDC_VALUE2_STATIC,
		IDC_VALUE2_EDIT,
		IDC_DISTANCE2_STATIC,
		IDC_DISTANCE2_EDIT,
	};

	size_t n = sizeof(IDs) / sizeof(IDs[0]);
	CWnd *pWnd;
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = this->DlgWnd->GetDlgItem(IDs[i]);
		ASSERT(pWnd && pWnd->GetSafeHwnd());
		if (pWnd)
		{
			if (show)
			{
				pWnd->ShowWindow(SW_SHOW);
			}
			else
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::ShowPoints(bool show)
{
	TRACE("In %s\n", __FUNCTION__);

	static UINT IDs[] =
	{
		IDC_POINTS_GRID,
		IDC_USE_MAP_COOR,
	};

	size_t n = sizeof(IDs) / sizeof(IDs[0]);

	CWnd *pWnd;
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = this->DlgWnd->GetDlgItem(IDs[i]);
		ASSERT(pWnd && pWnd->GetSafeHwnd());
		if (pWnd)
		{
			if (show)
			{
				pWnd->ShowWindow(SW_SHOW);
			}
			else
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::ShowXYZ(bool show)
{
	static UINT IDs[] =
	{
		IDC_STATIC_FN,
		IDC_EDIT_XYZ,
		IDC_BUTTON_XYZ,
		IDC_CHECK_USE_MAP,
	};

	size_t n = sizeof(IDs) / sizeof(IDs[0]);

	CWnd *pWnd;
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = this->DlgWnd->GetDlgItem(IDs[i]);
		ASSERT(pWnd && pWnd->GetSafeHwnd());
		if (pWnd)
		{
			if (show)
			{
				pWnd->ShowWindow(SW_SHOW);
			}
			else
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::ShowSingleProperty(CWnd *pDlgWnd, int nShow)
{
	static UINT IDs[] =
	{
		IDC_GB_PROPTYPE,
		IDC_COMBO_PROPTYPE,
		IDC_STATIC_PROPTYPE,
	};

	size_t n = sizeof(IDs) / sizeof(IDs[0]);

	CWnd *pWnd;
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = pDlgWnd->GetDlgItem(IDs[i]);
		if (pWnd)
		{
			pWnd->ShowWindow(nShow);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::ShowMixture(bool show)
{
	static UINT IDs[] =
	{
		IDC_GB_MIXTURE,
		IDC_CHECK_MIXTURE,
		IDC_STATIC_I,
		IDC_EDIT_I,
		IDC_STATIC_J,
		IDC_EDIT_J,
	};

	size_t n = sizeof(IDs) / sizeof(IDs[0]);

	CWnd *pWnd;
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = this->DlgWnd->GetDlgItem(IDs[i]);
		ASSERT(pWnd && pWnd->GetSafeHwnd());
		if (pWnd)
		{
			if (show)
			{
				pWnd->ShowWindow(SW_SHOW);
			}
			else
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::SetSeries(const CTimeSeries<Cproperty> &series)
{
	this->FreeVectors();

	std::string fname;
	bool bLastWasXYZT = false;

	CTimeSeries<Cproperty>::const_iterator fit = series.begin();
	for (int row = 1; fit != series.end(); ++fit)
	{
		ASSERT((*fit).second.type != PROP_UNDEFINED);
		if ((*fit).second.type == PROP_UNDEFINED) continue;
		if (bLastWasXYZT && (*fit).second.type == PROP_XYZT && (fname == (*fit).second.data_source->Get_file_name())) continue;

		// Ctime
		this->v_times[row] = new Ctime((*fit).first);

		// Cproperty
		CGridTimeSeries::ModeType mt = NONE;
		if ((*fit).second.type == PROP_UNDEFINED)
		{
			ASSERT(FALSE);
		}
		else if ((*fit).second.type == PROP_FIXED)
		{
			mt = CONSTANT;
			(*this->vv_props[SELECTED])[row] = new Cproperty((*fit).second);
		}
		else if ((*fit).second.type == PROP_LINEAR)
		{
			mt = LINEAR;
			(*this->vv_props[SELECTED])[row] = new Cproperty((*fit).second);
		}
		else if ((*fit).second.type == PROP_POINTS)
		{
			mt = POINTS;
			(*this->vv_props[SELECTED])[row] = new Cproperty((*fit).second);
		}
		else if ((*fit).second.type == PROP_XYZ)
		{
			mt = XYZ;
			(*this->vv_props[SELECTED])[row] = new Cproperty((*fit).second);
		}
		else if ((*fit).second.type == PROP_XYZT)
		{
			mt = XYZT;
			(*this->vv_props[SELECTED])[row] = new Cproperty((*fit).second);
		}
		else if ((*fit).second.type == PROP_ZONE)
		{
			ASSERT(FALSE);
			continue;  // for safety
		}
		else
		{
			ASSERT(FALSE);
			continue;  // for safety
		}

		// mix
		if (!(*this->vv_props[SELECTED])[row]->mix)
		{
			if ((*this->vv_props[SELECTED])[row]->mix1 == -1)
			{
				(*this->vv_props[SELECTED])[row]->mix1 = std::numeric_limits<double>::signaling_NaN();
			}
			if ((*this->vv_props[SELECTED])[row]->mix2 == -1)
			{
				(*this->vv_props[SELECTED])[row]->mix2 = std::numeric_limits<double>::signaling_NaN();
			}
		}

		// copy to save vectors
		if (mt != NONE)
		{
			(*this->vv_props[mt])[row] = new Cproperty(*(*this->vv_props[SELECTED])[row]);
		}

		if ((*fit).second.type == PROP_XYZT)
		{
			fname = (*fit).second.data_source->Get_file_name();
			ASSERT(fname.size());
			bLastWasXYZT = true;
		}
		else
		{
			bLastWasXYZT = false;
		}
		++row;
	}
}

void CGridTimeSeries::GetSeries(CTimeSeries<Cproperty> &series)const
{
	series.clear();
	for (size_t row = 1; row < this->v_times.size(); ++row)
	{
		if (this->v_times[row] && (*this->vv_props[SELECTED])[row])
		{
			if ((*this->vv_props[SELECTED])[row]->type != PROP_UNDEFINED)
			{
				Ctime t(*this->v_times[row]);
				Cproperty p(*(*this->vv_props[SELECTED])[row]);
				series[t] = p;
			}
		}
	}
}

#if defined(NEW_SINGLE_PROPERTY)
void CGridTimeSeries::SetProperty(const property *prop)
{
	this->FreeVectors();
	if (prop)
	{
		(*this->vv_props[SELECTED])[SINGLE] = new Cproperty(*prop);
		if (!(*this->vv_props[SELECTED])[SINGLE]->mix)
		{
			if ((*this->vv_props[SELECTED])[SINGLE]->mix1 == -1)
			{
				(*this->vv_props[SELECTED])[SINGLE]->mix1 = std::numeric_limits<double>::signaling_NaN();
			}
			if ((*this->vv_props[SELECTED])[SINGLE]->mix2 == -1)
			{
				(*this->vv_props[SELECTED])[SINGLE]->mix2 = std::numeric_limits<double>::signaling_NaN();
			}
		}

		// copy to save vectors
		if (prop->type == PROP_UNDEFINED)
		{
// COMMENT: {7/10/2009 3:00:40 PM}			ASSERT(FALSE);
		}
		else if (prop->type == PROP_FIXED)
		{
			(*this->vv_props[CONSTANT])[SINGLE] = new Cproperty(*(*this->vv_props[SELECTED])[SINGLE]);
		}
		else if (prop->type == PROP_LINEAR)
		{
			(*this->vv_props[LINEAR])[SINGLE] = new Cproperty(*(*this->vv_props[SELECTED])[SINGLE]);
		}
		else if (prop->type == PROP_POINTS)
		{
			(*this->vv_props[POINTS])[SINGLE] = new Cproperty(*(*this->vv_props[SELECTED])[SINGLE]);
		}
		else if (prop->type == PROP_XYZ)
		{
			(*this->vv_props[XYZ])[SINGLE] = new Cproperty(*(*this->vv_props[SELECTED])[SINGLE]);
		}
		else if (prop->type == PROP_XYZT)
		{
			(*this->vv_props[XYZT])[SINGLE] = new Cproperty(*(*this->vv_props[SELECTED])[SINGLE]);
		}
	}
}

void CGridTimeSeries::GetProperty(property *&prop)const
{
	delete static_cast<Cproperty*>(prop);
	if ((*this->vv_props[SELECTED])[SINGLE])
	{
		prop = new Cproperty(*(*this->vv_props[SELECTED])[SINGLE]);
	}
	else
	{
		prop = 0;
	}
}
#endif

void CGridTimeSeries::DDX_Mixture(CDataExchange* pDX, Cproperty &p)
{
	if (this->bEnableMixture)
	{
		if (pDX->m_bSaveAndValidate)
		{
			if (this->DlgWnd->IsDlgButtonChecked(IDC_CHECK_MIXTURE))
			{
				p.mix = true;
			}
			else
			{
				p.mix = false;
			}
			CGlobal::DDX_Text_Safe(pDX, IDC_EDIT_I, p.mix1);
			CGlobal::DDX_Text_Safe(pDX, IDC_EDIT_J, p.mix2);
		}
		else
		{
			if (p.mix)
			{
				this->DlgWnd->CheckDlgButton(IDC_CHECK_MIXTURE, BST_CHECKED);
			}
			else
			{
				this->DlgWnd->CheckDlgButton(IDC_CHECK_MIXTURE, BST_UNCHECKED);
			}
			CGlobal::DDX_Text_Safe(pDX, IDC_EDIT_I, p.mix1);
			CGlobal::DDX_Text_Safe(pDX, IDC_EDIT_J, p.mix2);
			this->OnBnClickedCheckMixture();
		}
	}
	else
	{
		this->DlgWnd->CheckDlgButton(IDC_CHECK_MIXTURE, BST_UNCHECKED);
		double d = std::numeric_limits<double>::signaling_NaN();
		if (!pDX->m_bSaveAndValidate)
		{
			CGlobal::DDX_Text_Safe(pDX, IDC_EDIT_I, d);
			CGlobal::DDX_Text_Safe(pDX, IDC_EDIT_J, d);
			this->OnBnClickedCheckMixture();
		}
	}
}

void CGridTimeSeries::DDX_Property(CDataExchange* pDX, Cproperty &p)
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
			this->DDX_Mixture(pDX, p);
		}
		else if (p.type == PROP_LINEAR)
		{
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
			this->DDX_Mixture(pDX, p);
		}
		else if (p.type == PROP_POINTS)
		{
			if (p.data_source)
			{
				// points
				std::vector<Point>::const_iterator citer = p.data_source->Get_user_points().begin();
				int row = 1;
				for (; citer != p.data_source->Get_user_points().end(); ++citer, ++row)
				{
					double x = (*citer).x();
					double y = (*citer).y();
					double z = (*citer).z();
					double v = (*citer).get_v();
					DDX_TextGridControl_Safe(pDX, IDC_POINTS_GRID, row, X_COLUMN, x);
					DDX_TextGridControl_Safe(pDX, IDC_POINTS_GRID, row, Y_COLUMN, y);
					DDX_TextGridControl_Safe(pDX, IDC_POINTS_GRID, row, Z_COLUMN, z);
					DDX_TextGridControl_Safe(pDX, IDC_POINTS_GRID, row, V_COLUMN, v);
				}

				// blank remaining rows
				CString blank;
				for (; row < this->PointsGrid->GetRowCount(); ++row)
				{
					DDX_TextGridControl(pDX, IDC_POINTS_GRID, row, X_COLUMN, blank);
					DDX_TextGridControl(pDX, IDC_POINTS_GRID, row, Y_COLUMN, blank);
					DDX_TextGridControl(pDX, IDC_POINTS_GRID, row, Z_COLUMN, blank);
					DDX_TextGridControl(pDX, IDC_POINTS_GRID, row, V_COLUMN, blank);
				}
				this->PointsGrid->RedrawWindow();


				// MAP or GRID
				int state = BST_UNCHECKED;
				if (p.data_source->Get_user_coordinate_system() == PHAST_Transform::MAP)
				{
					state = BST_CHECKED;
				}
				DDX_Check(pDX, IDC_USE_MAP_COOR, state);

				// MIXTURE
				this->DDX_Mixture(pDX, p);
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		else if (p.type == PROP_XYZ)
		{
			if (p.data_source)
			{
				ASSERT(p.type == PROP_XYZ);
				ASSERT(p.count_v == 0);
				ASSERT(p.data_source->Get_defined() == true);
				ASSERT(p.data_source->Get_source_type() == Data_source::XYZ);
				ASSERT(p.data_source->Get_user_source_type() == Data_source::XYZ);

				// file name (Note: filename cannot be empty)
				ASSERT(p.data_source->Get_file_name().size());
				if (p.data_source->Get_file_name().compare(PSZ_UNDEFINED) == 0)
				{
					CString empty;
					DDX_Text(pDX, IDC_EDIT_XYZ, empty);
				}
				else
				{
					CString str(p.data_source->Get_file_name().c_str());
					DDX_Text(pDX, IDC_EDIT_XYZ, str);
				}

				// MAP or GRID
				int state = BST_UNCHECKED;
				if (p.data_source->Get_user_coordinate_system() == PHAST_Transform::MAP)
				{
					state = BST_CHECKED;
				}
				DDX_Check(pDX, IDC_CHECK_USE_MAP, state);

				// MIXTURE
				this->DDX_Mixture(pDX, p);
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		else if (p.type == PROP_XYZT)
		{
			if (p.data_source)
			{
				ASSERT(p.type == PROP_XYZT);
				ASSERT(p.count_v == 0);
				ASSERT(p.data_source->Get_defined() == true);
				ASSERT(p.data_source->Get_source_type() == Data_source::XYZT);
				ASSERT(p.data_source->Get_user_source_type() == Data_source::XYZT);

				// file name (Note: filename cannot be empty)
				ASSERT(p.data_source->Get_file_name().size());
				if (p.data_source->Get_file_name().compare(PSZ_UNDEFINED) == 0)
				{
					CString empty;
					DDX_Text(pDX, IDC_EDIT_XYZ, empty);
				}
				else
				{
					CString str(p.data_source->Get_file_name().c_str());
					DDX_Text(pDX, IDC_EDIT_XYZ, str);
				}

				// MAP or GRID
				int state = BST_UNCHECKED;
				if (p.data_source->Get_user_coordinate_system() == PHAST_Transform::MAP)
				{
					state = BST_CHECKED;
				}
				DDX_Check(pDX, IDC_CHECK_USE_MAP, state);

				// MIXTURE
				this->DDX_Mixture(pDX, p);
			}
			else
			{
				ASSERT(FALSE);
			}
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::DDV_SoftValidate()
{
	TRACE("In %s validation_row = %d\n", __FUNCTION__, this->validation_row);
	if (this->validation_row < 0)
	{
		return;
	}

// COMMENT: {7/6/2009 8:46:42 PM}	if (this->grid.GetSafeHwnd())
	if (true)
	{
#if !defined(NEW_SINGLE_PROPERTY)
		int row = this->validation_row;

		CString str = this->grid.GetItemText(row, TYPE_COLUMN);
#else
		int row;
		CString str;
		if (this->bSingleProperty)
		{
			row = SINGLE;
			if (CComboBox *pComboBox = (CComboBox*)this->DlgWnd->GetDlgItem(IDC_COMBO_PROPTYPE))
			{
				int index = pComboBox->GetCurSel();
				if (index == CB_ERR) return;
				pComboBox->GetLBText(index, str);
			}
			else
			{
				ASSERT(FALSE);
				return;
			}
		}
		else
		{
			if (!this->grid.GetSafeHwnd())
			{
				ASSERT(FALSE);
				return;
			}
			row = this->validation_row;
			str = this->grid.GetItemText(row, TYPE_COLUMN);
		}
#endif

		if (str.Trim().IsEmpty() || str.CompareNoCase(PSZ_NONE) == 0)
		{
			//delete (*this->vv_props[SELECTED])[row];
			//(*this->vv_props[SELECTED])[row] = 0;
			if ((*this->vv_props[SELECTED])[row])
			{
				(*this->vv_props[SELECTED])[row]->type = PROP_UNDEFINED;

				// mixture
				CDataExchange dx(this->DlgWnd, TRUE);
				this->DDX_Mixture(&dx, *(*this->vv_props[SELECTED])[row]);
			}
		}
		else if (str.Compare(PSZ_CONSTANT) == 0)
		{
			CDataExchange dx(this->DlgWnd, TRUE);

			// value
			ASSERT((*this->vv_props[SELECTED])[row]);
			ASSERT((*this->vv_props[CONSTANT])[row]);
			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE_EDIT, (*this->vv_props[SELECTED])[row]->v[0]);

			// mixture
			this->DDX_Mixture(&dx, *(*this->vv_props[SELECTED])[row]);

			*(*this->vv_props[CONSTANT])[row] = *(*this->vv_props[SELECTED])[row];
		}
		else if (str.Compare(PSZ_LINEAR) == 0)
		{
			CDataExchange dx(this->DlgWnd, TRUE);

			ASSERT((*this->vv_props[SELECTED])[row]);
			ASSERT((*this->vv_props[LINEAR])[row]);

			// value1 dist1 value2 dist2
			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE1_EDIT,    (*this->vv_props[SELECTED])[row]->v[0]);
			CGlobal::DDX_Text_Safe(&dx, IDC_DISTANCE1_EDIT, (*this->vv_props[SELECTED])[row]->dist1);
			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE2_EDIT,    (*this->vv_props[SELECTED])[row]->v[1]);
			CGlobal::DDX_Text_Safe(&dx, IDC_DISTANCE2_EDIT, (*this->vv_props[SELECTED])[row]->dist2);

			// mixture
			this->DDX_Mixture(&dx, *(*this->vv_props[SELECTED])[row]);

			*(*this->vv_props[LINEAR])[row] = *(*this->vv_props[SELECTED])[row];
		}
		else if (str.Compare(PSZ_POINTS) == 0)
		{
			CDataExchange dx(this->DlgWnd, TRUE);

			ASSERT((*this->vv_props[SELECTED])[row]);
			ASSERT((*this->vv_props[POINTS])[row]);

			Cproperty p;
			p.type = PROP_POINTS;
			p.data_source = new Data_source;
			std::vector<Point> pts;

			// points
			CString sx, sy, sz, sv;			
			for (int r = 1; r < this->PointsGrid->GetRowCount(); ++r)
			{
				double x, y, z, v;
				sx = this->PointsGrid->GetItemText(r, X_COLUMN);
				sy = this->PointsGrid->GetItemText(r, Y_COLUMN);
				sz = this->PointsGrid->GetItemText(r, Z_COLUMN);
				sv = this->PointsGrid->GetItemText(r, V_COLUMN);
				if (!sx.IsEmpty() || !sy.IsEmpty() ||!sz.IsEmpty() ||!sv.IsEmpty())
				{
					DDX_TextGridControl_Safe(&dx, IDC_POINTS_GRID, r, X_COLUMN, x);
					DDX_TextGridControl_Safe(&dx, IDC_POINTS_GRID, r, Y_COLUMN, y);
					DDX_TextGridControl_Safe(&dx, IDC_POINTS_GRID, r, Z_COLUMN, z);
					DDX_TextGridControl_Safe(&dx, IDC_POINTS_GRID, r, V_COLUMN, v);

					Point pt(x, y, z, v);
					pts.push_back(pt);
				}
			}
			p.data_source->Set_points(pts);
			p.data_source->Set_source_type(Data_source::POINTS);
			p.data_source->Set_user_source_type(Data_source::POINTS);

			// MAP or GRID
			int state;
			DDX_Check(&dx, IDC_USE_MAP_COOR, state);
			if (state == BST_CHECKED)
			{
				p.data_source->Set_user_coordinate_system(PHAST_Transform::MAP);
			}
			else
			{
				p.data_source->Set_user_coordinate_system(PHAST_Transform::GRID);
			}

			p.data_source->Set_defined(true);

			// mixture
			this->DDX_Mixture(&dx, p);

			*(*this->vv_props[POINTS])[row] = p;
			*(*this->vv_props[SELECTED])[row] = p;
		}
		else if (str.Compare(PSZ_XYZ) == 0)
		{
			CDataExchange dx(this->DlgWnd, TRUE);

			ASSERT((*this->vv_props[SELECTED])[row]);
			ASSERT((*this->vv_props[XYZ])[row]);

			Cproperty p;
			p.type = PROP_XYZ;
			p.data_source = new Data_source;
			p.data_source->Set_defined(true);
			p.data_source->Set_source_type(Data_source::XYZ);
			p.data_source->Set_user_source_type(Data_source::XYZ);

			// file name
			//
			CString str;
			DDX_Text(&dx, IDC_EDIT_XYZ, str);

			std::string file_name;
			if (str.IsEmpty())
			{
				file_name = PSZ_UNDEFINED;
			}
			else
			{
				file_name = str;
			}
			p.data_source->Set_file_name(file_name);

			// MAP or GRID
			int state;
			DDX_Check(&dx, IDC_CHECK_USE_MAP, state);
			if (state == BST_CHECKED)
			{
				p.data_source->Set_coordinate_system(PHAST_Transform::MAP);
				p.data_source->Set_user_coordinate_system(PHAST_Transform::MAP);
				Data_source::DATA_SOURCE_TYPE ds = p.data_source->Get_source_type();
				ASSERT(ds == Data_source::XYZ);
				{
					ASSERT(!p.data_source->Get_filedata()); // new property
					ASSERT(p.data_source->Get_attribute() == -1);
					p.data_source->Set_filedata(FakeFiledata::New(file_name, PHAST_Transform::MAP, -1));
				}
				ASSERT(p.data_source->Get_filedata());
				p.data_source->Get_filedata()->Set_coordinate_system(PHAST_Transform::MAP);
			}
			else
			{
				p.data_source->Set_coordinate_system(PHAST_Transform::GRID);
				p.data_source->Set_user_coordinate_system(PHAST_Transform::GRID);
				Data_source::DATA_SOURCE_TYPE ds = p.data_source->Get_source_type();
				ASSERT(ds == Data_source::XYZ);
				{
					ASSERT(!p.data_source->Get_filedata()); // new property
					ASSERT(p.data_source->Get_attribute() == -1);
					p.data_source->Set_filedata(FakeFiledata::New(file_name, PHAST_Transform::GRID, -1));
				}
				ASSERT(p.data_source->Get_filedata());
				p.data_source->Get_filedata()->Set_coordinate_system(PHAST_Transform::GRID);
			}

			// mixture
			this->DDX_Mixture(&dx, p);

			*(*this->vv_props[XYZ])[row] = p;
			*(*this->vv_props[SELECTED])[row] = p;
		}
		else if (str.Compare(PSZ_XYZT) == 0)
		{
			CDataExchange dx(this->DlgWnd, TRUE);

			ASSERT((*this->vv_props[SELECTED])[row]);
			ASSERT((*this->vv_props[XYZT])[row]);

			Cproperty p;
			p.type = PROP_XYZT;
			p.data_source = new Data_source;
			p.data_source->Set_defined(true);
			p.data_source->Set_source_type(Data_source::XYZT);
			p.data_source->Set_user_source_type(Data_source::XYZT);

			// file name
			//
			CString str;
			DDX_Text(&dx, IDC_EDIT_XYZ, str);

			std::string file_name;
			if (str.IsEmpty())
			{
				file_name = PSZ_UNDEFINED;
			}
			else
			{
				file_name = str;
			}
			p.data_source->Set_file_name(file_name);

			// MAP or GRID
			int state;
			DDX_Check(&dx, IDC_CHECK_USE_MAP, state);
			if (state == BST_CHECKED)
			{
				p.data_source->Set_coordinate_system(PHAST_Transform::MAP);
				p.data_source->Set_user_coordinate_system(PHAST_Transform::MAP);
				Data_source::DATA_SOURCE_TYPE ds = p.data_source->Get_source_type();
				ASSERT(ds == Data_source::XYZT);
				{
					ASSERT(!p.data_source->Get_filedata()); // new property
					ASSERT(p.data_source->Get_attribute() == -1);
					p.data_source->Set_filedata(FakeFiledata::New(file_name, PHAST_Transform::GRID, -1));
				}
				ASSERT(p.data_source->Get_filedata());
				p.data_source->Get_filedata()->Set_coordinate_system(PHAST_Transform::MAP);
			}
			else
			{
				p.data_source->Set_coordinate_system(PHAST_Transform::GRID);
				p.data_source->Set_user_coordinate_system(PHAST_Transform::GRID);
				Data_source::DATA_SOURCE_TYPE ds = p.data_source->Get_source_type();
				ASSERT(ds == Data_source::XYZT);
				{
					ASSERT(!p.data_source->Get_filedata()); // new property
					ASSERT(p.data_source->Get_attribute() == -1);
					p.data_source->Set_filedata(FakeFiledata::New(file_name, PHAST_Transform::GRID, -1));
				}
				ASSERT(p.data_source->Get_filedata());
				p.data_source->Get_filedata()->Set_coordinate_system(PHAST_Transform::GRID);
			}

			// mixture
			this->DDX_Mixture(&dx, p);

			*(*this->vv_props[XYZT])[row] = p;
			*(*this->vv_props[SELECTED])[row] = p;
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::OnEndLabelEdit(int nRow, int nCol)
{
	TRACE("In %s row = %d col = %d\n", __FUNCTION__, nRow, nCol);
	CString str = this->grid.GetItemText(nRow, nCol);

	// Add your control notification handler code here
	if (this->grid.GetSafeHwnd())
	{
		if (nCol == TIME_COLUMN && !str.Trim().IsEmpty())
		{
			CString units = this->grid.GetItemText(nRow, UNIT_COLUMN);
			if (units.Trim().IsEmpty())
			{
				// look upwards to find default units
				CString u = this->DefaultTimeUnits;
				for (int r = nRow - 1; r >= 0; --r)
				{
					CString d = this->grid.GetItemText(r, UNIT_COLUMN);
					if (!d.IsEmpty() && CGlobal::GetStdTimeUnitsSafe(d).size() > 0)
					{
						u = CGlobal::GetStdTimeUnitsSafe(d).c_str();
						break;
					}					
				}
				this->grid.SetItemText(nRow, UNIT_COLUMN, u);
			}

			CString type = this->grid.GetItemText(nRow, TYPE_COLUMN);
			if (type.Trim().IsEmpty())
			{
				// look upwards to find default type
				for (int r = nRow - 1; r > 0; --r)
				{
					CString d = this->grid.GetItemText(r, TYPE_COLUMN);

					if (d.CompareNoCase(PSZ_NONE) == 0)
					{
						this->grid.SetItemText(nRow, TYPE_COLUMN, d);
						this->SetMode(CGridTimeSeries::NONE);
						break;
					}
					else if (d.CompareNoCase(PSZ_CONSTANT) == 0)
					{
						CGridTimeSeries::ModeType mt = CGridTimeSeries::CONSTANT;
						this->grid.SetItemText(nRow, TYPE_COLUMN, d);
						if (!(*this->vv_props[mt])[nRow])
						{
							(*this->vv_props[mt])[nRow] = new Cproperty(*(*this->vv_props[mt])[r]);
						}
						delete (*this->vv_props[SELECTED])[nRow];
						(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[r]);

						CDataExchange dx(this->DlgWnd, FALSE);
						this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
						this->SetMode(mt);
						break;
					}
					else if (d.CompareNoCase(PSZ_LINEAR) == 0)
					{
						CGridTimeSeries::ModeType mt = CGridTimeSeries::LINEAR;
						this->grid.SetItemText(nRow, TYPE_COLUMN, d);
						if (!(*this->vv_props[mt])[nRow])
						{
							(*this->vv_props[mt])[nRow] = new Cproperty(*(*this->vv_props[mt])[r]);
						}
						delete (*this->vv_props[SELECTED])[nRow];
						(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[r]);

						CDataExchange dx(this->DlgWnd, FALSE);
						this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
						this->SetMode(mt);
						break;
					}
					else if (d.CompareNoCase(PSZ_POINTS) == 0)
					{
						CGridTimeSeries::ModeType mt = CGridTimeSeries::POINTS;
						this->grid.SetItemText(nRow, TYPE_COLUMN, d);
						if (!(*this->vv_props[mt])[nRow])
						{
							(*this->vv_props[mt])[nRow] = new Cproperty(*(*this->vv_props[mt])[r]);
						}
						delete (*this->vv_props[SELECTED])[nRow];
						(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[r]);

						CDataExchange dx(this->DlgWnd, FALSE);
						this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
						this->SetMode(mt);
						break;
					}
					else if (d.CompareNoCase(PSZ_XYZ) == 0)
					{
						CGridTimeSeries::ModeType mt = CGridTimeSeries::XYZ;
						this->grid.SetItemText(nRow, TYPE_COLUMN, d);
						if (!(*this->vv_props[mt])[nRow])
						{
							(*this->vv_props[mt])[nRow] = new Cproperty(*(*this->vv_props[mt])[r]);
						}
						delete (*this->vv_props[SELECTED])[nRow];
						(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[r]);

						CDataExchange dx(this->DlgWnd, FALSE);
						this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
						this->SetMode(mt);
						break;
					}
					else if (d.CompareNoCase(PSZ_XYZT) == 0)
					{
						CGridTimeSeries::ModeType mt = CGridTimeSeries::XYZT;
						this->grid.SetItemText(nRow, TYPE_COLUMN, d);
						if (!(*this->vv_props[mt])[nRow])
						{
							(*this->vv_props[mt])[nRow] = new Cproperty(*(*this->vv_props[mt])[r]);
						}
						delete (*this->vv_props[SELECTED])[nRow];
						(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[r]);

						CDataExchange dx(this->DlgWnd, FALSE);
						this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
						this->SetMode(mt);
						break;
					}
				}
			}
		}
		if (nCol == TYPE_COLUMN)
		{
			CGridTimeSeries::ModeType mt = CGridTimeSeries::NONE;
			if (str.Trim().IsEmpty() || str.Compare(PSZ_NONE) == 0)
			{
				//delete (*this->vv_props[SELECTED])[nRow];
				//(*this->vv_props[SELECTED])[nRow] = 0;
				if ((*this->vv_props[SELECTED])[nRow])
				{
					(*this->vv_props[SELECTED])[nRow]->type = PROP_UNDEFINED;
				}
			}
			else if (str.Compare(PSZ_CONSTANT) == 0)
			{
				mt = CGridTimeSeries::CONSTANT;
				if (!(*this->vv_props[mt])[nRow])
				{
					Cproperty def(PROP_FIXED);
					for (int r = nRow - 1; r > 0; --r)
					{
						CString d = this->grid.GetItemText(r, TYPE_COLUMN);
						if (d.CompareNoCase(PSZ_CONSTANT) == 0)
						{
							ASSERT((*this->vv_props[mt])[r]);
							def = (*(*this->vv_props[mt])[r]);
							break;
						}
					}
					(*this->vv_props[mt])[nRow] = new Cproperty(def);
				}
				// save mix settings
				if ((*this->vv_props[SELECTED])[nRow])
				{
					(*this->vv_props[mt])[nRow]->mix  = (*this->vv_props[SELECTED])[nRow]->mix;
					(*this->vv_props[mt])[nRow]->mix1 = (*this->vv_props[SELECTED])[nRow]->mix1;
					(*this->vv_props[mt])[nRow]->mix2 = (*this->vv_props[SELECTED])[nRow]->mix2;
				}
				delete (*this->vv_props[SELECTED])[nRow];
				(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[nRow]);

				// update data
				CDataExchange dx(this->DlgWnd, FALSE);
				this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
			}
			else if (str.Compare(PSZ_LINEAR) == 0)
			{
				mt = CGridTimeSeries::LINEAR;
				if (!(*this->vv_props[mt])[nRow])
				{
					Cproperty def(PROP_LINEAR);
					for (int r = nRow - 1; r > 0; --r)
					{
						CString d = this->grid.GetItemText(r, TYPE_COLUMN);
						if (d.CompareNoCase(PSZ_LINEAR) == 0)
						{
							ASSERT((*this->vv_props[mt])[r]);
							def = (*(*this->vv_props[mt])[r]);
							break;
						}
					}
					(*this->vv_props[mt])[nRow] = new Cproperty(def);
				}
				// save mix settings
				if ((*this->vv_props[SELECTED])[nRow])
				{
					(*this->vv_props[mt])[nRow]->mix  = (*this->vv_props[SELECTED])[nRow]->mix;
					(*this->vv_props[mt])[nRow]->mix1 = (*this->vv_props[SELECTED])[nRow]->mix1;
					(*this->vv_props[mt])[nRow]->mix2 = (*this->vv_props[SELECTED])[nRow]->mix2;
				}
				delete (*this->vv_props[SELECTED])[nRow];
				(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[nRow]);

				// update data
				CDataExchange dx(this->DlgWnd, FALSE);
				this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
			}
			else if (str.Compare(PSZ_POINTS) == 0)
			{
				mt = CGridTimeSeries::POINTS;
				if (!(*this->vv_props[mt])[nRow])
				{
					Cproperty def(PROP_POINTS);
					for (int r = nRow - 1; r > 0; --r)
					{
						CString d = this->grid.GetItemText(r, TYPE_COLUMN);
						if (d.CompareNoCase(PSZ_POINTS) == 0)
						{
							ASSERT((*this->vv_props[mt])[r]);
							def = (*(*this->vv_props[mt])[r]);
							break;
						}
					}
					(*this->vv_props[mt])[nRow] = new Cproperty(def);
				}
				// save mix settings
				if ((*this->vv_props[SELECTED])[nRow])
				{
					(*this->vv_props[mt])[nRow]->mix  = (*this->vv_props[SELECTED])[nRow]->mix;
					(*this->vv_props[mt])[nRow]->mix1 = (*this->vv_props[SELECTED])[nRow]->mix1;
					(*this->vv_props[mt])[nRow]->mix2 = (*this->vv_props[SELECTED])[nRow]->mix2;
				}
				delete (*this->vv_props[SELECTED])[nRow];
				(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[nRow]);

				// update data
				CDataExchange dx(this->DlgWnd, FALSE);
				this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
			}
			else if (str.Compare(PSZ_XYZ) == 0)
			{
				mt = CGridTimeSeries::XYZ;
				if (!(*this->vv_props[mt])[nRow])
				{
					Cproperty def(PROP_XYZ);
					for (int r = nRow - 1; r > 0; --r)
					{
						CString d = this->grid.GetItemText(r, TYPE_COLUMN);
						if (d.CompareNoCase(PSZ_XYZ) == 0)
						{
							ASSERT((*this->vv_props[mt])[r]);
							def = (*(*this->vv_props[mt])[r]);
							break;
						}
					}
					(*this->vv_props[mt])[nRow] = new Cproperty(def);
				}
				// save mix settings
				if ((*this->vv_props[SELECTED])[nRow])
				{
					(*this->vv_props[mt])[nRow]->mix  = (*this->vv_props[SELECTED])[nRow]->mix;
					(*this->vv_props[mt])[nRow]->mix1 = (*this->vv_props[SELECTED])[nRow]->mix1;
					(*this->vv_props[mt])[nRow]->mix2 = (*this->vv_props[SELECTED])[nRow]->mix2;
				}
				delete (*this->vv_props[SELECTED])[nRow];
				(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[nRow]);
	
				// update data
				CDataExchange dx(this->DlgWnd, FALSE);
				this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
			}
			else if (str.Compare(PSZ_XYZT) == 0)
			{
				mt = CGridTimeSeries::XYZT;
				if (!(*this->vv_props[mt])[nRow])
				{
					Cproperty def(PROP_XYZT);
					for (int r = nRow - 1; r > 0; --r)
					{
						CString d = this->grid.GetItemText(r, TYPE_COLUMN);
						if (d.CompareNoCase(PSZ_XYZT) == 0)
						{
							ASSERT((*this->vv_props[mt])[r]);
							def = (*(*this->vv_props[mt])[r]);
							break;
						}
					}
					(*this->vv_props[mt])[nRow] = new Cproperty(def);
				}
				// save mix settings
				if ((*this->vv_props[SELECTED])[nRow])
				{
					(*this->vv_props[mt])[nRow]->mix  = (*this->vv_props[SELECTED])[nRow]->mix;
					(*this->vv_props[mt])[nRow]->mix1 = (*this->vv_props[SELECTED])[nRow]->mix1;
					(*this->vv_props[mt])[nRow]->mix2 = (*this->vv_props[SELECTED])[nRow]->mix2;
				}
				delete (*this->vv_props[SELECTED])[nRow];
				(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[nRow]);
	
				// update data
				CDataExchange dx(this->DlgWnd, FALSE);
				this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
			}
			this->SetMode(mt);
		}
	}
	this->grid.RedrawWindow();

	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::SetType(PROP_TYPE pt)
{
	switch (pt)
	{
	case PROP_FIXED:
		this->SetMode(CGridTimeSeries::CONSTANT);
		break;
	case PROP_LINEAR:
		this->SetMode(CGridTimeSeries::LINEAR);
		break;
	case PROP_POINTS:
		this->SetMode(CGridTimeSeries::POINTS);
		break;
	case PROP_XYZ:
		this->SetMode(CGridTimeSeries::XYZ);
		break;
	case PROP_XYZT:
		this->SetMode(CGridTimeSeries::XYZT);
		break;
	default:
		this->SetMode(CGridTimeSeries::NONE);
	}
}

void CGridTimeSeries::SetMode(CGridTimeSeries::ModeType mode)
{
	TRACE("In %s\n", __FUNCTION__);

#if !defined(NEW_SINGLE_PROPERTY)
	CCellID id = this->grid.GetFocusCell();
	CString time = this->grid.GetItemText(id.row, TIME_COLUMN);
	CString units = this->grid.GetItemText(id.row, UNIT_COLUMN);
#else
	CString time;
	CString units;
	if (!this->bSingleProperty)
	{
		CCellID id = this->grid.GetFocusCell();
		time = this->grid.GetItemText(id.row, TIME_COLUMN);
		units = this->grid.GetItemText(id.row, UNIT_COLUMN);
	}
#endif

	if (!units.Trim().IsEmpty() && !time.Trim().IsEmpty())
	{
		CString str;
		str.Format("%s %s", time, units);
		time = str;
	}

	if (mode == CGridTimeSeries::NONE)
	{
		if (CWnd *pWnd = this->DlgWnd->GetDlgItem(IDC_GB_PROPERTY))
		{
			pWnd->SetWindowText(PSZ_NONE);
		}
		this->ShowConstant(false);
		this->ShowLinear(false);
		this->ShowPoints(false);		
		this->ShowXYZ(false);
		this->ShowMixture(false);
// COMMENT: {7/6/2009 6:14:40 PM}		this->ShowSingleProperty(this->bSingleProperty);
	}
	else if (mode == CGridTimeSeries::CONSTANT)
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
		if (CWnd *pWnd = this->DlgWnd->GetDlgItem(IDC_GB_PROPERTY))
		{
			pWnd->SetWindowText(str);
		}
		this->ShowConstant(true);
		this->ShowLinear(false);
		this->ShowPoints(false);
		this->ShowXYZ(false);
		this->ShowMixture(true);
		this->EnableMixture(this->bEnableMixture);
	}
	else if (mode == CGridTimeSeries::LINEAR)
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
		if (CWnd *pWnd = this->DlgWnd->GetDlgItem(IDC_GB_PROPERTY))
		{
			pWnd->SetWindowText(str);
		}
		this->ShowConstant(false);
		this->ShowLinear(true);
		this->ShowPoints(false);
		this->ShowXYZ(false);
		this->ShowMixture(true);
		this->EnableMixture(this->bEnableMixture);
	}
	else if (mode == CGridTimeSeries::POINTS)
	{
		CString str;
		if (time.Trim().IsEmpty())
		{
			str.Format("%s", PSZ_POINTS);
		}
		else
		{
			str.Format("%s (t=%s)", PSZ_POINTS, time);
		}
		if (CWnd *pWnd = this->DlgWnd->GetDlgItem(IDC_GB_PROPERTY))
		{
			pWnd->SetWindowText(str);
		}
		this->ShowConstant(false);
		this->ShowLinear(false);
		this->ShowPoints(true);
		this->ShowXYZ(false);
		this->ShowMixture(true);
		this->EnableMixture(this->bEnableMixture);
	}
	else if (mode == CGridTimeSeries::XYZ)
	{
		CString str;
		if (time.Trim().IsEmpty())
		{
			str.Format("%s", PSZ_XYZ);
		}
		else
		{
			str.Format("%s (t=%s)", PSZ_XYZ, time);
		}
		if (CWnd *pWnd = this->DlgWnd->GetDlgItem(IDC_GB_PROPERTY))
		{
			pWnd->SetWindowText(str);
		}
		this->ShowConstant(false);
		this->ShowLinear(false);
		this->ShowPoints(false);
		this->ShowXYZ(true);
		this->ShowMixture(true);
		this->EnableMixture(this->bEnableMixture);
	}
	else if (mode == CGridTimeSeries::XYZT)
	{
		CString str;
		if (time.Trim().IsEmpty())
		{
			str.Format("%s", PSZ_XYZT);
		}
		else
		{
			str.Format("%s (t=%s)", PSZ_XYZT, time);
		}
		if (CWnd *pWnd = this->DlgWnd->GetDlgItem(IDC_GB_PROPERTY))
		{
			pWnd->SetWindowText(str);
		}
		this->ShowConstant(false);
		this->ShowLinear(false);
		this->ShowPoints(false);
		this->ShowXYZ(true);
		this->ShowMixture(true);
		this->EnableMixture(this->bEnableMixture);
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::OnSelChanged(int nRow, int nCol)
{
	TRACE("In %s row = %d col = %d\n", __FUNCTION__, nRow, nCol);

	if (this->validation_row > 0)
	{
		CCellID cell = this->grid.GetFocusCell();
		if (cell.row != this->validation_row)
		{
			this->DDV_SoftValidate();
		}
	}

	if (this->grid.GetSafeHwnd())
	{
		CString str = this->grid.GetItemText(nRow, TYPE_COLUMN);
		this->DDX_Vectors(nRow, str);

		// set next validation row
		this->validation_row = nRow;
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::DDX_Vectors(int row, CString &str)
{
	TRACE("In %s row = %d\n", __FUNCTION__, row);
	CDataExchange dx(this->DlgWnd, FALSE);

	CGridTimeSeries::ModeType mode = CGridTimeSeries::NONE;
	if (str.Compare(PSZ_CONSTANT) == 0)
	{
		mode = CONSTANT;
		if ((*this->vv_props[mode])[row])
		{
			this->DDX_Property(&dx, *(*this->vv_props[mode])[row]);
			// ASSERT(*(*this->vv_props[SELECTED])[row] == *(*this->vv_props[mode])[row]);
		}
	}
	else if (str.Compare(PSZ_LINEAR) == 0)
	{
		mode = LINEAR;
		if ((*this->vv_props[mode])[row])
		{
			this->DDX_Property(&dx, *(*this->vv_props[mode])[row]);
			// ASSERT(*(*this->vv_props[SELECTED])[row] == *(*this->vv_props[mode])[row]);
		}
	}
	else if (str.Compare(PSZ_POINTS) == 0)
	{
		mode = POINTS;
		if ((*this->vv_props[mode])[row])
		{
			this->DDX_Property(&dx, *(*this->vv_props[mode])[row]);
			// ASSERT(*(*this->vv_props[SELECTED])[row] == *(*this->vv_props[mode])[row]);
		}
	}
	else if (str.Compare(PSZ_XYZ) == 0)
	{
		mode = XYZ;
		if ((*this->vv_props[mode])[row])
		{
			this->DDX_Property(&dx, *(*this->vv_props[mode])[row]);
			// ASSERT(*(*this->vv_props[SELECTED])[row] == *(*this->vv_props[mode])[row]);
		}
	}
	else if (str.Compare(PSZ_XYZT) == 0)
	{
		mode = XYZT;
		if ((*this->vv_props[mode])[row])
		{
			this->DDX_Property(&dx, *(*this->vv_props[mode])[row]);
			// ASSERT(*(*this->vv_props[SELECTED])[row] == *(*this->vv_props[mode])[row]);
		}
	}

	this->SetMode(mode);
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::DDX_Series(CDataExchange* pDX, bool bTimeZeroRequired)
{
	TRACE("In %s\n", __FUNCTION__);
	if (pDX->m_bSaveAndValidate)
	{
		for (int row = this->grid.GetFixedRowCount(); row < this->grid.GetRowCount(); ++row)
		{
			CString strValue;
			double value;
			Ctime time;
			time.type = TT_UNITS;

			strValue = this->grid.GetItemText(row, TIME_COLUMN);
			if (!strValue.Trim().IsEmpty())
			{
				// time value
				DDX_TextGridControl(pDX, this->grid.GetDlgCtrlID(), row, TIME_COLUMN, value);
				if (value < 0.0)
				{
					::DDX_GridControlFail(pDX, this->grid.GetDlgCtrlID(), row, TIME_COLUMN, _T("Start time must be positive."));
				}
				time.SetValue(value);

				// time units
				DDX_TextGridControl(pDX, this->grid.GetDlgCtrlID(), row, UNIT_COLUMN, strValue);
				if (!strValue.IsEmpty() && time.SetInput(strValue) != OK)
				{
					if (row != 1)
					{
						::DDX_GridControlFail(pDX, this->grid.GetDlgCtrlID(), row, UNIT_COLUMN, _T("Please enter the start time units."));
					}
				}

				// prop
				DDX_TextGridControl(pDX, this->grid.GetDlgCtrlID(), row, TYPE_COLUMN, strValue);
				if (strValue.Trim().IsEmpty() || strValue.CompareNoCase(PSZ_NONE) == 0)
				{
					if (row == 1 && bTimeZeroRequired)
					{
						CString string("A property must be defined for time zero.");
						::DDX_GridControlFail(pDX, this->grid.GetDlgCtrlID(), row, TYPE_COLUMN, string);
					}
					ASSERT(!(*this->vv_props[SELECTED])[row] || ((*this->vv_props[SELECTED])[row]->type == PROP_UNDEFINED));
				}
				else if (strValue.CompareNoCase(PSZ_CONSTANT) == 0)
				{
					CGridTimeSeries::ModeType mt = CGridTimeSeries::CONSTANT;
					if ((*this->vv_props[SELECTED])[row])
					{
						double d;
						if ((*this->vv_props[SELECTED])[row]->v[0] != (*this->vv_props[SELECTED])[row]->v[0])
						{
							this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							this->OnSelChanged(row, TYPE_COLUMN);
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
					CGridTimeSeries::ModeType mt = CGridTimeSeries::LINEAR;
					if ((*this->vv_props[SELECTED])[row])
					{
						double d;
						if ((*this->vv_props[SELECTED])[row]->v[0] != (*this->vv_props[SELECTED])[row]->v[0])
						{
							this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							this->OnSelChanged(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_VALUE1_EDIT, d);
						}
						if ((*this->vv_props[SELECTED])[row]->dist1 != (*this->vv_props[SELECTED])[row]->dist1)
						{
							this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							this->OnSelChanged(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_DISTANCE1_EDIT, d);
						}
						if ((*this->vv_props[SELECTED])[row]->v[1] != (*this->vv_props[SELECTED])[row]->v[1])
						{
							this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							this->OnSelChanged(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_VALUE2_EDIT, d);
						}
						if ((*this->vv_props[SELECTED])[row]->dist2 != (*this->vv_props[SELECTED])[row]->dist2)
						{
							this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							this->OnSelChanged(row, TYPE_COLUMN);
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
					CGridTimeSeries::ModeType mt = CGridTimeSeries::POINTS;
					if ((*this->vv_props[SELECTED])[row])
					{
						// HACK {{
						// sync up grid with points (in case blank rows were entered)
						CDataExchange dx(this->DlgWnd, FALSE);
						this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[row]);
						// HACK }}

						// points
						ASSERT((*this->vv_props[SELECTED])[row]->data_source);
						std::vector<Point>::const_iterator citer = (*this->vv_props[SELECTED])[row]->data_source->Get_user_points().begin();
						for (int r = 1; citer != (*this->vv_props[SELECTED])[row]->data_source->Get_user_points().end(); ++citer, ++r)
						{
							double x = (*citer).x();
							double y = (*citer).y();
							double z = (*citer).z();
							double v = (*citer).get_v();

							double d;
							if (x != x)
							{
								this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
								this->OnSelChanged(row, TYPE_COLUMN);
								::DDX_TextGridControl(pDX, IDC_POINTS_GRID, r, 0, d);
							}
							if (y != y)
							{
								this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
								this->OnSelChanged(row, TYPE_COLUMN);
								::DDX_TextGridControl(pDX, IDC_POINTS_GRID, r, 1, d);
							}
							if (z != z)
							{
								this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
								this->OnSelChanged(row, TYPE_COLUMN);
								::DDX_TextGridControl(pDX, IDC_POINTS_GRID, r, 2, d);
							}
							if (v != v)
							{
								this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
								this->OnSelChanged(row, TYPE_COLUMN);
								::DDX_TextGridControl(pDX, IDC_POINTS_GRID, r, 3, d);
							}
						}
					}
					else
					{
						ASSERT(FALSE);
					}
				}
				else if (strValue.CompareNoCase(PSZ_XYZ) == 0)
				{
					CGridTimeSeries::ModeType mt = CGridTimeSeries::XYZ;
					if ((*this->vv_props[SELECTED])[row])
					{
						ASSERT((*this->vv_props[SELECTED])[row]->data_source);
						ASSERT((*this->vv_props[SELECTED])[row]->type == PROP_XYZ);
						ASSERT((*this->vv_props[SELECTED])[row]->count_v == 0);
						ASSERT((*this->vv_props[SELECTED])[row]->data_source->Get_defined() == true);
						ASSERT((*this->vv_props[SELECTED])[row]->data_source->Get_source_type() == Data_source::XYZ);
						ASSERT((*this->vv_props[SELECTED])[row]->data_source->Get_user_source_type() == Data_source::XYZ);
						ASSERT((*this->vv_props[SELECTED])[row]->data_source->Get_file_name().size());

						CString str;
						if ((*this->vv_props[SELECTED])[row]->data_source->Get_file_name().compare(PSZ_UNDEFINED) == 0)
						{
							this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							this->OnSelChanged(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_EDIT_XYZ, str);
							CGlobal::IsValidXYZFile(str, pDX);
						}
					}
					else
					{
						ASSERT(FALSE);
					}
				}
				else if (strValue.CompareNoCase(PSZ_XYZT) == 0)
				{
					CGridTimeSeries::ModeType mt = CGridTimeSeries::XYZT;
					if ((*this->vv_props[SELECTED])[row])
					{
						ASSERT((*this->vv_props[SELECTED])[row]->data_source);
						ASSERT((*this->vv_props[SELECTED])[row]->type == PROP_XYZT);
						ASSERT((*this->vv_props[SELECTED])[row]->count_v == 0);
						ASSERT((*this->vv_props[SELECTED])[row]->data_source->Get_defined() == true);
						ASSERT((*this->vv_props[SELECTED])[row]->data_source->Get_source_type() == Data_source::XYZT);
						ASSERT((*this->vv_props[SELECTED])[row]->data_source->Get_user_source_type() == Data_source::XYZT);
						ASSERT((*this->vv_props[SELECTED])[row]->data_source->Get_file_name().size());

						CString str;
						if ((*this->vv_props[SELECTED])[row]->data_source->Get_file_name().compare(PSZ_UNDEFINED) == 0)
						{
							this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							this->OnSelChanged(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_EDIT_XYZ, str);
							CGlobal::IsValidXYZTFile(str, pDX);
						}
					}
					else
					{
						ASSERT(FALSE);
					}
				}

				// mixture
				//				
				if (this->bEnableMixture)
				{
					if ((*this->vv_props[SELECTED])[row])
					{
						if ((*this->vv_props[SELECTED])[row]->mix)
						{
							double d;
							if ((*this->vv_props[SELECTED])[row]->mix1 != (*this->vv_props[SELECTED])[row]->mix1)
							{
								this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
								this->OnSelChanged(row, TYPE_COLUMN);
								::DDX_Text(pDX, IDC_EDIT_I, d);
							}
							if ((*this->vv_props[SELECTED])[row]->mix2 != (*this->vv_props[SELECTED])[row]->mix2)
							{
								this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
								this->OnSelChanged(row, TYPE_COLUMN);
								::DDX_Text(pDX, IDC_EDIT_J, d);
							}
						}
					}
				}

				delete this->v_times[row];
				this->v_times[row] = new Ctime(time);
			}
			else
			{
				delete this->v_times[row];
				this->v_times[row] = 0;
			}
		}
	}
	else
	{
		CString s;
		CCellID cell = this->grid.GetFocusCell();
		ASSERT(this->v_times.size() == (*this->vv_props[SELECTED]).size());
		for (int row = this->grid.GetFixedRowCount(); row < this->grid.GetRowCount(); ++row)
		{
			// time
			if (this->v_times[row])
			{
				if (row == 1)
				{
					ASSERT(this->v_times[row]->value == 0.);
				}

				// time value
				s.Format(_T("%g"), this->v_times[row]->value);
				this->grid.SetItemText(row, TIME_COLUMN, s);

				// time units
				if (this->v_times[row]->input && ::strlen(this->v_times[row]->input))
				{
					this->grid.SetItemText(row, UNIT_COLUMN, this->v_times[row]->input);
				}
				else
				{
					this->grid.SetItemText(row, UNIT_COLUMN, this->DefaultTimeUnits);
				}
			}

			if (row == 1)
			{
				this->grid.DisableCell(row, UNIT_COLUMN);
			}

			// prop
			if ((*this->vv_props[SELECTED])[row])
			{
				// prop type
				if ((*this->vv_props[SELECTED])[row]->type == PROP_UNDEFINED)
				{
					this->grid.SetItemText(row, TYPE_COLUMN, PSZ_NONE);
				}
				else if ((*this->vv_props[SELECTED])[row]->type == PROP_FIXED)
				{
					this->grid.SetItemText(row, TYPE_COLUMN, PSZ_CONSTANT);
				}
				else if ((*this->vv_props[SELECTED])[row]->type == PROP_LINEAR)
				{
					this->grid.SetItemText(row, TYPE_COLUMN, PSZ_LINEAR);
				}
				else if ((*this->vv_props[SELECTED])[row]->type == PROP_POINTS)
				{
					this->grid.SetItemText(row, TYPE_COLUMN, PSZ_POINTS);
				}
				else if ((*this->vv_props[SELECTED])[row]->type == PROP_XYZ)
				{
					this->grid.SetItemText(row, TYPE_COLUMN, PSZ_XYZ);
				}
				else if ((*this->vv_props[SELECTED])[row]->type == PROP_XYZT)
				{
					this->grid.SetItemText(row, TYPE_COLUMN, PSZ_XYZT);
				}

				if (row == cell.row)
				{
					this->DDX_Property(pDX, *(*this->vv_props[SELECTED])[row]);
				}
			}
		}

		ASSERT(this->grid.GetFixedColumnCount() <= cell.col && cell.col < this->grid.GetColumnCount());
		ASSERT(this->grid.GetFixedRowCount() <= cell.row && cell.row < this->grid.GetRowCount());
		
		this->OnSelChanged(cell.row, cell.col);

		this->grid.RedrawWindow();
	}
	TRACE("Out %s\n", __FUNCTION__);
}

#if defined(NEW_SINGLE_PROPERTY)
void CGridTimeSeries::DDX_Single(CDataExchange* pDX, bool bRequired /* = true*/)
{
	TRACE("In %s\n", __FUNCTION__);
	ASSERT(this->bSingleProperty);

	if (pDX->m_bSaveAndValidate)
	{
		if (CComboBox *pComboBox = (CComboBox*)this->DlgWnd->GetDlgItem(IDC_COMBO_PROPTYPE))
		{
			CString strValue;

			int index = pComboBox->GetCurSel();
			if (index == CB_ERR) return;
			pComboBox->GetLBText(index, strValue);

			if (strValue.Trim().IsEmpty() || strValue.CompareNoCase(PSZ_NONE) == 0)
			{
// COMMENT: {7/10/2009 5:28:46 PM}				ASSERT(!(*this->vv_props[SELECTED])[SINGLE]);
				if (bRequired)
				{
					::AfxMessageBox("Property must be defined.");
					pDX->PrepareCtrl(IDC_COMBO_PROPTYPE);
					pDX->Fail();
				}
			}
			else if (strValue.CompareNoCase(PSZ_CONSTANT) == 0)
			{
				CGridTimeSeries::ModeType mt = CGridTimeSeries::CONSTANT;
				if ((*this->vv_props[SELECTED])[SINGLE])
				{
					double d;
					if ((*this->vv_props[SELECTED])[SINGLE]->v[0] != (*this->vv_props[SELECTED])[SINGLE]->v[0])
					{
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
				CGridTimeSeries::ModeType mt = CGridTimeSeries::LINEAR;
				if ((*this->vv_props[SELECTED])[SINGLE])
				{
					double d;
					if ((*this->vv_props[SELECTED])[SINGLE]->v[0] != (*this->vv_props[SELECTED])[SINGLE]->v[0])
					{
						::DDX_Text(pDX, IDC_VALUE1_EDIT, d);
					}
					if ((*this->vv_props[SELECTED])[SINGLE]->dist1 != (*this->vv_props[SELECTED])[SINGLE]->dist1)
					{
						::DDX_Text(pDX, IDC_DISTANCE1_EDIT, d);
					}
					if ((*this->vv_props[SELECTED])[SINGLE]->v[1] != (*this->vv_props[SELECTED])[SINGLE]->v[1])
					{
						::DDX_Text(pDX, IDC_VALUE2_EDIT, d);
					}
					if ((*this->vv_props[SELECTED])[SINGLE]->dist2 != (*this->vv_props[SELECTED])[SINGLE]->dist2)
					{
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
				CGridTimeSeries::ModeType mt = CGridTimeSeries::POINTS;
				if ((*this->vv_props[SELECTED])[SINGLE])
				{
					// HACK {{
					// sync up grid with points (in case blank SINGLEs were entered)
					CDataExchange dx(this->DlgWnd, FALSE);
					this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[SINGLE]);
					// HACK }}

					// points
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->data_source);
					std::vector<Point>::const_iterator citer = (*this->vv_props[SELECTED])[SINGLE]->data_source->Get_user_points().begin();
					for (int r = 1; citer != (*this->vv_props[SELECTED])[SINGLE]->data_source->Get_user_points().end(); ++citer, ++r)
					{
						double x = (*citer).x();
						double y = (*citer).y();
						double z = (*citer).z();
						double v = (*citer).get_v();

						double d;
						if (x != x)
						{
							::DDX_TextGridControl(pDX, IDC_POINTS_GRID, r, 0, d);
						}
						if (y != y)
						{
							::DDX_TextGridControl(pDX, IDC_POINTS_GRID, r, 1, d);
						}
						if (z != z)
						{
							::DDX_TextGridControl(pDX, IDC_POINTS_GRID, r, 2, d);
						}
						if (v != v)
						{
							::DDX_TextGridControl(pDX, IDC_POINTS_GRID, r, 3, d);
						}
					}
				}
				else
				{
					ASSERT(FALSE);
				}
			}
			else if (strValue.CompareNoCase(PSZ_XYZ) == 0)
			{
				CGridTimeSeries::ModeType mt = CGridTimeSeries::XYZ;
				if ((*this->vv_props[SELECTED])[SINGLE])
				{
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->data_source);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->type == PROP_XYZ);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->count_v == 0);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->data_source->Get_defined() == true);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->data_source->Get_source_type() == Data_source::XYZ);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->data_source->Get_user_source_type() == Data_source::XYZ);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->data_source->Get_file_name().size());

					CString str;
					if ((*this->vv_props[SELECTED])[SINGLE]->data_source->Get_file_name().compare(PSZ_UNDEFINED) == 0)
					{
						::DDX_Text(pDX, IDC_EDIT_XYZ, str);
						CGlobal::IsValidXYZFile(str, pDX);
					}
				}
				else
				{
					ASSERT(FALSE);
				}
			}
			else if (strValue.CompareNoCase(PSZ_XYZT) == 0)
			{
				CGridTimeSeries::ModeType mt = CGridTimeSeries::XYZT;
				if ((*this->vv_props[SELECTED])[SINGLE])
				{
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->data_source);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->type == PROP_XYZT);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->count_v == 0);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->data_source->Get_defined() == true);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->data_source->Get_source_type() == Data_source::XYZT);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->data_source->Get_user_source_type() == Data_source::XYZT);
					ASSERT((*this->vv_props[SELECTED])[SINGLE]->data_source->Get_file_name().size());

					CString str;
					if ((*this->vv_props[SELECTED])[SINGLE]->data_source->Get_file_name().compare(PSZ_UNDEFINED) == 0)
					{
						::DDX_Text(pDX, IDC_EDIT_XYZ, str);
						CGlobal::IsValidXYZTFile(str, pDX);
					}
				}
				else
				{
					ASSERT(FALSE);
				}
			}
		}
	}
	else
	{
		if ((*this->vv_props[SELECTED])[SINGLE])
		{
			if (CComboBox *pComboBox = (CComboBox*)this->DlgWnd->GetDlgItem(IDC_COMBO_PROPTYPE))
			{
				if ((*this->vv_props[SELECTED])[SINGLE]->type == PROP_UNDEFINED)
				{
					VERIFY(pComboBox->SelectString(0, PSZ_NONE) != CB_ERR);
					this->SetMode(NONE);
				}
				else if ((*this->vv_props[SELECTED])[SINGLE]->type == PROP_FIXED)
				{
					VERIFY(pComboBox->SelectString(0, PSZ_CONSTANT) != CB_ERR);
					this->SetMode(CONSTANT);
				}
				else if ((*this->vv_props[SELECTED])[SINGLE]->type == PROP_LINEAR)
				{
					VERIFY(pComboBox->SelectString(0, PSZ_LINEAR) != CB_ERR);
					this->SetMode(LINEAR);
				}
				else if ((*this->vv_props[SELECTED])[SINGLE]->type == PROP_POINTS)
				{
					VERIFY(pComboBox->SelectString(0, PSZ_POINTS) != CB_ERR);
					this->SetMode(POINTS);
				}
				else if ((*this->vv_props[SELECTED])[SINGLE]->type == PROP_XYZ)
				{
					VERIFY(pComboBox->SelectString(0, PSZ_XYZ) != CB_ERR);
					this->SetMode(XYZ);
				}
				else if ((*this->vv_props[SELECTED])[SINGLE]->type == PROP_XYZT)
				{
					VERIFY(pComboBox->SelectString(0, PSZ_XYZT) != CB_ERR);
					this->SetMode(XYZT);
				}
			}

			//
			this->DDX_Property(pDX, *(*this->vv_props[SELECTED])[SINGLE]);
		}
		else
		{
			if (CComboBox *pComboBox = (CComboBox*)this->DlgWnd->GetDlgItem(IDC_COMBO_PROPTYPE))
			{
				VERIFY(pComboBox->SelectString(0, PSZ_NONE) != CB_ERR);
				this->SetMode(NONE);
			}
		}
	}

	TRACE("Out %s\n", __FUNCTION__);
}
#endif

void CGridTimeSeries::SetPointsGrid(CModGridCtrlEx *grid)
{
	this->PointsGrid = grid;
	if (this->PointsGrid->GetColumnCount() == 0)
	{
		TRY
		{
			this->PointsGrid->SetRowCount(MIN_POINTS+1);
			this->PointsGrid->SetColumnCount(5);
			this->PointsGrid->SetFixedRowCount(1);
			this->PointsGrid->SetFixedColumnCount(1);
			this->PointsGrid->EnableTitleTips(FALSE);
			this->PointsGrid->SetCurrentFocusCell(1, 0);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH


		// set row headings
		GV_ITEM headItem;
		headItem.mask    = GVIF_TEXT|GVIF_FORMAT;
		headItem.nFormat = DT_CENTER|DT_VCENTER|DT_END_ELLIPSIS;
		headItem.col     = ITEM_COLUMN;
		for (int row = this->PointsGrid->GetFixedRowCount(); row < this->PointsGrid->GetRowCount(); ++row)
		{
			headItem.row = row;
			headItem.szText.Format("%d", row);
			this->PointsGrid->SetItem(&headItem);
		}


		// set default format
		for (int row = this->PointsGrid->GetFixedRowCount(); row < this->PointsGrid->GetRowCount(); ++row)
		{
			for (int col = this->PointsGrid->GetFixedColumnCount(); col < this->PointsGrid->GetColumnCount(); ++col)
			{
				this->PointsGrid->SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
			}
		}

		// headings
		GV_ITEM Item;
		Item.mask = GVIF_TEXT;
		Item.row = 0;

		Item.col = 1;
		Item.szText.Format(_T("X"));
		this->PointsGrid->SetItem(&Item);

		Item.col = 2;
		Item.szText.Format(_T("Y"));
		this->PointsGrid->SetItem(&Item);

		Item.col = 3;
		Item.szText.Format(_T("Z"));
		this->PointsGrid->SetItem(&Item);

		Item.col = 4;
		Item.szText.Format(_T("Value"));
		this->PointsGrid->SetItem(&Item);

		this->PointsGrid->SetColumnWidth(0, 28);
		this->PointsGrid->SetColumnWidth(1, 60);
		this->PointsGrid->SetColumnWidth(2, 60);
		this->PointsGrid->SetColumnWidth(3, 60);
		this->PointsGrid->SetColumnWidth(4, 60);
		this->PointsGrid->SetRedraw(TRUE, TRUE);
	}
}

void CGridTimeSeries::OnBnClickedButtonXYZ()
{
	static char szFilters[] =
		"All Files (*.*)|*.*|"
		"|";

	// Create an Open dialog
	CFileDialog fileDlg(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this->DlgWnd);


	TCHAR buffer[MAX_PATH];
	if (this->DlgWnd->GetDlgItemText(IDC_EDIT_XYZ, buffer, MAX_PATH))
	{
		if (::PathRemoveFileSpec(buffer))
		{
			fileDlg.m_ofn.lpstrInitialDir = buffer;
		}
	}
	if (fileDlg.DoModal() == IDOK)
	{
		this->DlgWnd->SetDlgItemText(IDC_EDIT_XYZ, fileDlg.GetPathName());
	}
}

void CGridTimeSeries::SetUnits(const CUnits &u)
{
	if (u.time.defined)
	{
		this->DefaultTimeUnits = CGlobal::GetStdTimeUnits(u.time.input).c_str();
	}
	else
	{
		this->DefaultTimeUnits = CGlobal::GetStdTimeUnits(u.time.si).c_str();
	}
}

void CGridTimeSeries::OnCbnSelchangeComboProptype()
{
	if (CComboBox *pComboBox = (CComboBox*)this->DlgWnd->GetDlgItem(IDC_COMBO_PROPTYPE))
	{
		int index = pComboBox->GetCurSel();
		if (index == CB_ERR) return;

		CString str;
		pComboBox->GetLBText(index, str);

		CGridTimeSeries::ModeType mt = CGridTimeSeries::NONE;
		if (str.Trim().IsEmpty() || str.Compare(PSZ_NONE) == 0)
		{
			delete (*this->vv_props[SELECTED])[SINGLE];
			(*this->vv_props[SELECTED])[SINGLE] = 0;
		}
		else if (str.Compare(PSZ_CONSTANT) == 0)
		{
			mt = CGridTimeSeries::CONSTANT;
			if (!(*this->vv_props[mt])[SINGLE])
			{
				Cproperty def(PROP_FIXED);
				(*this->vv_props[mt])[SINGLE] = new Cproperty(def);
			}
			delete (*this->vv_props[SELECTED])[SINGLE];
			(*this->vv_props[SELECTED])[SINGLE] = new Cproperty(*(*this->vv_props[mt])[SINGLE]);

			// update data
			CDataExchange dx(this->DlgWnd, FALSE);
			this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[SINGLE]);
		}
		else if (str.Compare(PSZ_LINEAR) == 0)
		{
			mt = CGridTimeSeries::LINEAR;
			if (!(*this->vv_props[mt])[SINGLE])
			{
				Cproperty def(PROP_LINEAR);
				(*this->vv_props[mt])[SINGLE] = new Cproperty(def);
			}
			delete (*this->vv_props[SELECTED])[SINGLE];
			(*this->vv_props[SELECTED])[SINGLE] = new Cproperty(*(*this->vv_props[mt])[SINGLE]);

			// update data
			CDataExchange dx(this->DlgWnd, FALSE);
			this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[SINGLE]);
		}
		else if (str.Compare(PSZ_POINTS) == 0)
		{
			mt = CGridTimeSeries::POINTS;
			if (!(*this->vv_props[mt])[SINGLE])
			{
				Cproperty def(PROP_POINTS);
				(*this->vv_props[mt])[SINGLE] = new Cproperty(def);
			}
			delete (*this->vv_props[SELECTED])[SINGLE];
			(*this->vv_props[SELECTED])[SINGLE] = new Cproperty(*(*this->vv_props[mt])[SINGLE]);

			// update data
			CDataExchange dx(this->DlgWnd, FALSE);
			this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[SINGLE]);
		}
		else if (str.Compare(PSZ_XYZ) == 0)
		{
			mt = CGridTimeSeries::XYZ;
			if (!(*this->vv_props[mt])[SINGLE])
			{
				Cproperty def(PROP_XYZ);
				(*this->vv_props[mt])[SINGLE] = new Cproperty(def);
			}
			delete (*this->vv_props[SELECTED])[SINGLE];
			(*this->vv_props[SELECTED])[SINGLE] = new Cproperty(*(*this->vv_props[mt])[SINGLE]);

			// update data
			CDataExchange dx(this->DlgWnd, FALSE);
			this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[SINGLE]);
		}
		else if (str.Compare(PSZ_XYZT) == 0)
		{
			mt = CGridTimeSeries::XYZT;
			if (!(*this->vv_props[mt])[SINGLE])
			{
				Cproperty def(PROP_XYZT);
				(*this->vv_props[mt])[SINGLE] = new Cproperty(def);
			}
			delete (*this->vv_props[SELECTED])[SINGLE];
			(*this->vv_props[SELECTED])[SINGLE] = new Cproperty(*(*this->vv_props[mt])[SINGLE]);

			// update data
			CDataExchange dx(this->DlgWnd, FALSE);
			this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[SINGLE]);
		}
		this->SetMode(mt);
	}
}

#if defined(NEW_SINGLE_PROPERTY)
void CGridTimeSeries::OnBnClickedCheckMixture()
{
	static UINT IDs[] =
	{
		IDC_GB_MIXTURE,
		IDC_STATIC_I,
		IDC_EDIT_I,
		IDC_STATIC_J,
		IDC_EDIT_J,
	};

	size_t n = sizeof(IDs) / sizeof(IDs[0]);

	CWnd *pWnd;
	BOOL bEnable = this->DlgWnd->IsDlgButtonChecked(IDC_CHECK_MIXTURE);
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = this->DlgWnd->GetDlgItem(IDs[i]);
		ASSERT(pWnd && pWnd->GetSafeHwnd());
		if (pWnd)
		{
			pWnd->EnableWindow(bEnable);
		}
	}
}

void CGridTimeSeries::EnableMixture(BOOL bEnable)
{
	static UINT IDs[] =
	{
		IDC_GB_MIXTURE,
		IDC_STATIC_I,
		IDC_EDIT_I,
		IDC_STATIC_J,
		IDC_EDIT_J,
	};

	CWnd *pWnd = this->DlgWnd->GetDlgItem(IDC_CHECK_MIXTURE);
	if (pWnd)
	{
		pWnd->EnableWindow(bEnable);
	}
	size_t n = sizeof(IDs) / sizeof(IDs[0]);
	BOOL b = this->DlgWnd->IsDlgButtonChecked(IDC_CHECK_MIXTURE);
	for (size_t i = 0; i < n; ++i)
	{
		pWnd = this->DlgWnd->GetDlgItem(IDs[i]);
		ASSERT(pWnd && pWnd->GetSafeHwnd());
		if (pWnd)
		{
			pWnd->EnableWindow(b);
		}
	}
}
#endif
