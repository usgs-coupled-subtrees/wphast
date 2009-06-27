#include "StdAfx.h"
#include "GridTimeSeries.h"

#include "Global.h"

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

CGridTimeSeries::CGridTimeSeries(CWnd* pWnd)
: DlgWnd(pWnd)
, validation_row(-1)
{
	//std::vector< std::vector<Cproperty*> > vv_props;
	// selected


	this->v_times.resize(MIN_ROWS + 1, 0);

	this->v_selected.resize(MIN_ROWS + 1, 0);
	this->v_constant.resize(MIN_ROWS + 1, 0);
	this->v_linear.resize(MIN_ROWS + 1, 0);

	this->vv_props.push_back(&this->v_selected);
	this->vv_props.push_back(&this->v_constant);
	this->vv_props.push_back(&this->v_linear);

	this->DefaultUnits = PSZ_YEAR;
}

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
	// Prepare Grid
	//
	if (!pDX->m_bSaveAndValidate && this->grid.GetColumnCount() == 0)
	{
		TRY
		{
			this->grid.SetRowCount(MIN_ROWS/* + this->m_well.count_depth_user + this->m_well.count_elevation_user*/);
			this->grid.SetColumnCount(3);
			this->grid.SetFixedRowCount(1);
			this->grid.SetFixedColumnCount(0);
			this->grid.EnableTitleTips(FALSE);
			this->grid.SetCurrentFocusCell(1, TYPE_COLUMN);
			this->validation_row = 1;

			///CCellRange FluxCellRange = this->grid.GetSelectedCellRange();

			// this->grid.EnableSelection(FALSE); // this breaks CutSelectedText
			this->grid.SetHighLight(GV_HIGHLIGHT_ALWAYS);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

		// set default format
		for (int row = this->grid.GetFixedRowCount(); row < this->grid.GetRowCount(); ++row)
		{
			for (int col = 0; col < this->grid.GetColumnCount(); ++col)
			{
				this->grid.SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
			}
		}
		///this->SetScreenHeadings(this->m_bByDepth);
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = 0;

			Item.col = 0;
			Item.szText.Format(_T("Time"));
			this->grid.SetItem(&Item);

			Item.col = 1;
			Item.szText.Format(_T("Units"));
			this->grid.SetItem(&Item);

			Item.col = 2;
			Item.szText.Format(_T("Type"));
			this->grid.SetItem(&Item);

			this->grid.SetItemText(1, 0, _T("0"));
			this->grid.DisableCell(1, 0);

			this->grid.DisableCell(1, 1);

			this->grid.SetItemText(1, 2, PSZ_NONE);
// COMMENT: {6/25/2009 4:20:58 PM}			this->SetMode(CFluxPropsPage2::NONE);

			// units column
			std::vector<LPCTSTR> vecTimeUnits;
			vecTimeUnits.push_back(PSZ_SECS);
			vecTimeUnits.push_back(PSZ_MINS);
			vecTimeUnits.push_back(PSZ_HOUR);
			vecTimeUnits.push_back(PSZ_DAYS);
			vecTimeUnits.push_back(PSZ_YEAR);
			this->grid.SetColumnOptions(1, vecTimeUnits);

			// type column
			std::vector<LPCTSTR> vecPropType;
			vecPropType.push_back(PSZ_NONE);
			vecPropType.push_back(PSZ_CONSTANT);
			vecPropType.push_back(PSZ_LINEAR);
// COMMENT: {6/10/2009 3:34:31 PM}			vecPropType.push_back(PSZ_POINTS);
// COMMENT: {6/10/2009 3:34:31 PM}			vecPropType.push_back(PSZ_XYZ);
			this->grid.SetColumnOptions(2, vecPropType);
			//}}

			this->grid.SetColumnWidth(0, 71);
			this->grid.SetColumnWidth(1, 62);
			this->grid.SetColumnWidth(2, 70);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::ShowConstant(bool show)
{
	TRACE("In %s\n", __FUNCTION__);
	if (show)
	{
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE_STATIC))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE_EDIT))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE_STATIC))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE_EDIT))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::ShowLinear(bool show)
{
	TRACE("In %s\n", __FUNCTION__);
	if (show)
	{
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_STATIC_DIRECTION))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_X_RADIO))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_Y_RADIO))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_Z_RADIO))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE1_STATIC))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE1_EDIT))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_DISTANCE1_STATIC))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_DISTANCE1_EDIT))
		{
			pWnd->ShowWindow(SW_SHOW);
		}

		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE2_STATIC))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE2_EDIT))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_DISTANCE2_STATIC))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_DISTANCE2_EDIT))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_STATIC_DIRECTION))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_X_RADIO))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_Y_RADIO))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_Z_RADIO))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE1_STATIC))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE1_EDIT))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_DISTANCE1_STATIC))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_DISTANCE1_EDIT))
		{
			pWnd->ShowWindow(SW_HIDE);
		}

		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE2_STATIC))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_VALUE2_EDIT))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_DISTANCE2_STATIC))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		if (CWnd* pWnd = this->DlgWnd->GetDlgItem(IDC_DISTANCE2_EDIT))
		{
			pWnd->ShowWindow(SW_HIDE);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::SetSeries(const CTimeSeries<Cproperty> &series)
{
	this->FreeVectors();

	CTimeSeries<Cproperty>::const_iterator fit = series.begin();
	for (int row = 1; fit != series.end(); ++fit, ++row)
	{
		// Ctime
		this->v_times[row] = new Ctime((*fit).first);

		// Cproperty
		if ((*fit).second.type == PROP_UNDEFINED)
		{
			ASSERT(FALSE);
		}
		else if ((*fit).second.type == PROP_FIXED)
		{
			(*this->vv_props[SELECTED])[row] = new Cproperty((*fit).second);
			(*this->vv_props[CONSTANT])[row] = new Cproperty((*fit).second);
		}
		else if ((*fit).second.type == PROP_LINEAR)
		{
			(*this->vv_props[SELECTED])[row] = new Cproperty((*fit).second);
			(*this->vv_props[LINEAR])[row] = new Cproperty((*fit).second);
		}
	}
}

void CGridTimeSeries::DDV_SoftValidate(/*CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors, int row*/)
{
	//{{TEMP
	if (this->validation_row < 0)
	{
		return;
	}
	//}}TEMP

	TRACE("In %s\n", __FUNCTION__);
	if (this->grid.GetSafeHwnd())
	{
		//{{TEMP
		int row = this->validation_row;
		//}}TEMP

		CString str = this->grid.GetItemText(row, TYPE_COLUMN);

		if (str.Trim().IsEmpty() || str.CompareNoCase(PSZ_NONE) == 0)
		{
// COMMENT: {6/25/2009 7:44:22 PM}			delete (*vectors[SELECTED])[row];
// COMMENT: {6/25/2009 7:44:22 PM}			(*vectors[SELECTED])[row] = 0;
			delete (*this->vv_props[SELECTED])[row];
			(*this->vv_props[SELECTED])[row] = 0;
		}
		else if (str.Compare(PSZ_CONSTANT) == 0)
		{
// COMMENT: {6/25/2009 7:47:52 PM}			CDataExchange dx(this, TRUE);
// COMMENT: {6/25/2009 7:47:52 PM}
// COMMENT: {6/25/2009 7:47:52 PM}			ASSERT((*vectors[SELECTED])[row]);
// COMMENT: {6/25/2009 7:47:52 PM}			ASSERT((*vectors[CONSTANT])[row]);
// COMMENT: {6/25/2009 7:47:52 PM}			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE_EDIT, (*vectors[SELECTED])[row]->v[0]);
// COMMENT: {6/25/2009 7:47:52 PM}			*(*vectors[CONSTANT])[row] = *(*vectors[SELECTED])[row];
// COMMENT: {6/25/2009 7:47:52 PM}			//mode = CFluxPropsPage2::CONSTANT;

			CDataExchange dx(this->DlgWnd, TRUE);

			ASSERT((*this->vv_props[SELECTED])[row]);
			ASSERT((*this->vv_props[CONSTANT])[row]);
			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE_EDIT, (*this->vv_props[SELECTED])[row]->v[0]);
			*(*this->vv_props[CONSTANT])[row] = *(*this->vv_props[SELECTED])[row];
			//mode = CFluxPropsPage2::CONSTANT;
		}
		else if (str.Compare(PSZ_LINEAR) == 0)
		{
// COMMENT: {6/25/2009 7:49:06 PM}			CDataExchange dx(this, TRUE);
// COMMENT: {6/25/2009 7:49:06 PM}
// COMMENT: {6/25/2009 7:49:06 PM}			ASSERT((*vectors[SELECTED])[row]);
// COMMENT: {6/25/2009 7:49:06 PM}			ASSERT((*vectors[LINEAR])[row]);
// COMMENT: {6/25/2009 7:49:06 PM}
// COMMENT: {6/25/2009 7:49:06 PM}			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE1_EDIT,    (*vectors[SELECTED])[row]->v[0]);
// COMMENT: {6/25/2009 7:49:06 PM}			CGlobal::DDX_Text_Safe(&dx, IDC_DISTANCE1_EDIT, (*vectors[SELECTED])[row]->dist1);
// COMMENT: {6/25/2009 7:49:06 PM}			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE2_EDIT,    (*vectors[SELECTED])[row]->v[1]);
// COMMENT: {6/25/2009 7:49:06 PM}			CGlobal::DDX_Text_Safe(&dx, IDC_DISTANCE2_EDIT, (*vectors[SELECTED])[row]->dist2);
// COMMENT: {6/25/2009 7:49:06 PM}
// COMMENT: {6/25/2009 7:49:06 PM}			*(*vectors[LINEAR])[row] = *(*vectors[SELECTED])[row];
// COMMENT: {6/25/2009 7:49:06 PM}
// COMMENT: {6/25/2009 7:49:06 PM}			//mode = CFluxPropsPage2::LINEAR;

			CDataExchange dx(this->DlgWnd, TRUE);

			ASSERT((*this->vv_props[SELECTED])[row]);
			ASSERT((*this->vv_props[LINEAR])[row]);

			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE1_EDIT,    (*this->vv_props[SELECTED])[row]->v[0]);
			CGlobal::DDX_Text_Safe(&dx, IDC_DISTANCE1_EDIT, (*this->vv_props[SELECTED])[row]->dist1);
			CGlobal::DDX_Text_Safe(&dx, IDC_VALUE2_EDIT,    (*this->vv_props[SELECTED])[row]->v[1]);
			CGlobal::DDX_Text_Safe(&dx, IDC_DISTANCE2_EDIT, (*this->vv_props[SELECTED])[row]->dist2);

			*(*this->vv_props[LINEAR])[row] = *(*this->vv_props[SELECTED])[row];

			//mode = CFluxPropsPage2::LINEAR;
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

// pnmgv->iRow    = nRow
// pnmgv->iColumn = nCol
void CGridTimeSeries::OnEndLabelEdit(int nRow, int nCol/*NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors*/)
{
	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:11:41 PM}	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
// COMMENT: {6/25/2009 8:11:41 PM}	CString str = this->grid.GetItemText(pnmgv->iRow, pnmgv->iColumn);
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
				CString u = this->DefaultUnits;
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
						//{{
						CDataExchange dx(this->DlgWnd, FALSE);
						this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
						//}}
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
						//{{
						CDataExchange dx(this->DlgWnd, FALSE);
						this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
						//}}
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
						//{{
						CDataExchange dx(this->DlgWnd, FALSE);
						this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
						//}}
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
						//{{
						CDataExchange dx(this->DlgWnd, FALSE);
						this->DDX_Property(&dx, *(*this->vv_props[SELECTED])[nRow]);
						//}}
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
				delete (*this->vv_props[SELECTED])[nRow];
				(*this->vv_props[SELECTED])[nRow] = 0;
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
				delete (*this->vv_props[SELECTED])[nRow];
				(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[nRow]);
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
				delete (*this->vv_props[SELECTED])[nRow];
				(*this->vv_props[SELECTED])[nRow] = new Cproperty(*(*this->vv_props[mt])[nRow]);
			}
			//[[if (mt != CGridTimeSeries::NONE)
			//[[{
			//[[	this->UpdateData(FALSE);
			//[[}
			this->SetMode(mt);
		}
	}
	this->grid.RedrawWindow();

	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::SetMode(CGridTimeSeries::ModeType mode)
{
	TRACE("In %s\n", __FUNCTION__);
	CString time;
	CString units;

// COMMENT: {6/25/2009 8:22:53 PM}	if (this->ItemDDX == this->FluxSeries.treeitem)
// COMMENT: {6/25/2009 8:22:53 PM}	{
// COMMENT: {6/25/2009 8:22:53 PM}		CCellID id = this->FluxSeries.grid.GetFocusCell();
// COMMENT: {6/25/2009 8:22:53 PM}		time = this->FluxSeries.grid.GetItemText(id.row, TIME_COLUMN);
// COMMENT: {6/25/2009 8:22:53 PM}		units = this->FluxSeries.grid.GetItemText(id.row, UNIT_COLUMN);
// COMMENT: {6/25/2009 8:22:53 PM}	}
// COMMENT: {6/25/2009 8:22:53 PM}	else if (this->ItemDDX == this->SolutionSeries.treeitem)
// COMMENT: {6/25/2009 8:22:53 PM}	{
// COMMENT: {6/25/2009 8:22:53 PM}		CCellID id = this->SolutionSeries.grid.GetFocusCell();
// COMMENT: {6/25/2009 8:22:53 PM}		time = this->SolutionSeries.grid.GetItemText(id.row, TIME_COLUMN);
// COMMENT: {6/25/2009 8:22:53 PM}		units = this->SolutionSeries.grid.GetItemText(id.row, UNIT_COLUMN);
// COMMENT: {6/25/2009 8:22:53 PM}	}

	CCellID id = this->grid.GetFocusCell();
	time = this->grid.GetItemText(id.row, TIME_COLUMN);
	units = this->grid.GetItemText(id.row, UNIT_COLUMN);

	if (!units.Trim().IsEmpty() && !time.Trim().IsEmpty())
	{
		CString str;
		str.Format("%s %s", time, units);
		time = str;
	}

	if (mode == CGridTimeSeries::NONE)
	{
		//[[this->PropertyGroupBox.SetWindowText(_T(""));
		if (CWnd *pWnd = this->DlgWnd->GetDlgItem(IDC_GB_PROPERTY))
		{
			pWnd->SetWindowText(_T(""));
		}
		this->ShowConstant(false);
		this->ShowLinear(false);
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
		//[[this->PropertyGroupBox.SetWindowText(str);
		if (CWnd *pWnd = this->DlgWnd->GetDlgItem(IDC_GB_PROPERTY))
		{
			pWnd->SetWindowText(str);
		}
		this->ShowConstant(true);
		this->ShowLinear(false);
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
		//[[this->PropertyGroupBox.SetWindowText(str);
		if (CWnd *pWnd = this->DlgWnd->GetDlgItem(IDC_GB_PROPERTY))
		{
			pWnd->SetWindowText(str);
		}
		this->ShowConstant(false);
		this->ShowLinear(true);
	}
	TRACE("Out %s\n", __FUNCTION__);
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
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CGridTimeSeries::OnSelChanged(int nRow, int nCol /*NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid*/)
{
// COMMENT: {6/25/2009 8:40:31 PM}	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
// COMMENT: {6/25/2009 8:40:31 PM}	TRACE("In %s row = %d\n", __FUNCTION__, pnmgv->iRow);
	TRACE("In %s row = %d\n", __FUNCTION__, nRow);

	if (this->validation_row > 0)
	{
		CCellID cell = this->grid.GetFocusCell();
		if (cell.row != this->validation_row)
		{
// COMMENT: {6/25/2009 7:53:55 PM}			this->DDV_SoftValidate(this->SolutionSeries.grid, this->SolutionSeries.vv_props, this->SolutionSeries.validation_row);
			this->DDV_SoftValidate();
		}
	}

	if (this->grid.GetSafeHwnd())
	{
// COMMENT: {6/25/2009 8:43:05 PM}		CString str = grid.GetItemText(pnmgv->iRow, TYPE_COLUMN);
// COMMENT: {6/25/2009 8:43:05 PM}		if (this->ItemDDX == this->FluxSeries.treeitem)
// COMMENT: {6/25/2009 8:43:05 PM}		{
// COMMENT: {6/25/2009 8:43:05 PM}			ASSERT(&grid == &this->FluxSeries.grid);
// COMMENT: {6/25/2009 8:43:05 PM}			this->DDX_Vectors(pnmgv->iRow, str, this->FluxSeries.vv_props);
// COMMENT: {6/25/2009 8:43:05 PM}		}
// COMMENT: {6/25/2009 8:43:05 PM}		else if (this->ItemDDX == this->SolutionSeries.treeitem)
// COMMENT: {6/25/2009 8:43:05 PM}		{
// COMMENT: {6/25/2009 8:43:05 PM}			ASSERT(&grid == &this->SolutionSeries.grid);
// COMMENT: {6/25/2009 8:43:05 PM}			this->DDX_Vectors(pnmgv->iRow, str, this->SolutionSeries.vv_props);
// COMMENT: {6/25/2009 8:43:05 PM}		}
		CString str = this->grid.GetItemText(nRow, TYPE_COLUMN);
		this->DDX_Vectors(nRow, str /*, this->FluxSeries.vv_props*/);

		//{{NEW {6/25/2009 11:22:16 PM}
		// set next validation row
		this->validation_row = nRow;
		//}}NEW {6/25/2009 11:22:16 PM}
	}
	TRACE("Out %s\n", __FUNCTION__);
}


// COMMENT: {6/25/2009 8:44:26 PM}void CGridTimeSeries::DDX_Vectors(int row, CString &str, std::vector< std::vector<Cproperty*>* > &vectors)
// COMMENT: {6/25/2009 8:44:26 PM}{
// COMMENT: {6/25/2009 8:44:26 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:44:26 PM}	CDataExchange dx(this, FALSE);
// COMMENT: {6/25/2009 8:44:26 PM}
// COMMENT: {6/25/2009 8:44:26 PM}	CFluxPropsPage2::ModeType mode = CFluxPropsPage2::NONE;
// COMMENT: {6/25/2009 8:44:26 PM}	if (str.Compare(PSZ_CONSTANT) == 0)
// COMMENT: {6/25/2009 8:44:26 PM}	{
// COMMENT: {6/25/2009 8:44:26 PM}		mode = CFluxPropsPage2::CONSTANT;
// COMMENT: {6/25/2009 8:44:26 PM}		if ((*vectors[CONSTANT])[row])
// COMMENT: {6/25/2009 8:44:26 PM}		{
// COMMENT: {6/25/2009 8:44:26 PM}			this->DDX_Property(&dx, *(*vectors[CONSTANT])[row]);
// COMMENT: {6/25/2009 8:44:26 PM}		}
// COMMENT: {6/25/2009 8:44:26 PM}	}
// COMMENT: {6/25/2009 8:44:26 PM}	else if (str.Compare(PSZ_LINEAR) == 0)
// COMMENT: {6/25/2009 8:44:26 PM}	{
// COMMENT: {6/25/2009 8:44:26 PM}		mode = CFluxPropsPage2::LINEAR;
// COMMENT: {6/25/2009 8:44:26 PM}		if ((*vectors[LINEAR])[row])
// COMMENT: {6/25/2009 8:44:26 PM}		{
// COMMENT: {6/25/2009 8:44:26 PM}			this->DDX_Property(&dx, *(*vectors[LINEAR])[row]);
// COMMENT: {6/25/2009 8:44:26 PM}		}
// COMMENT: {6/25/2009 8:44:26 PM}	}
// COMMENT: {6/25/2009 8:44:26 PM}	this->SetMode(mode);
// COMMENT: {6/25/2009 8:44:26 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {6/25/2009 8:44:26 PM}}

void CGridTimeSeries::DDX_Vectors(int row, CString &str /*, std::vector< std::vector<Cproperty*>* > &vectors*/)
{
	TRACE("In %s\n", __FUNCTION__);
	CDataExchange dx(this->DlgWnd, FALSE);

	CGridTimeSeries::ModeType mode = CGridTimeSeries::NONE;
	if (str.Compare(PSZ_CONSTANT) == 0)
	{
		mode = CGridTimeSeries::CONSTANT;
		if ((*this->vv_props[CONSTANT])[row])
		{
			this->DDX_Property(&dx, *(*this->vv_props[CONSTANT])[row]);
		}
	}
	else if (str.Compare(PSZ_LINEAR) == 0)
	{
		mode = CGridTimeSeries::LINEAR;
		if ((*this->vv_props[LINEAR])[row])
		{
			this->DDX_Property(&dx, *(*this->vv_props[LINEAR])[row]);
		}
	}
	this->SetMode(mode);
	TRACE("Out %s\n", __FUNCTION__);
}
// props = *(this->SolutionSeries.vv_props[SELECTED])
// this->DDX_Series(pDX, this->SolutionSeries.grid, this->SolutionSeries.v_times, *(this->SolutionSeries.vv_props[SELECTED]), !this->FlowOnly);
void CGridTimeSeries::DDX_Series(CDataExchange* pDX, /*CModGridCtrlEx &this->grid, std::vector<Ctime*> &times, std::vector<Cproperty*> &props,*/ bool bTimeZeroRequired)
{
	TRACE("In %s\n", __FUNCTION__);
	if (pDX->m_bSaveAndValidate)
	{
// COMMENT: {6/25/2009 9:22:11 PM}		std::vector<Ctime*> new_times;
// COMMENT: {6/25/2009 9:22:11 PM}		std::vector<Cproperty*> new_props;
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
					// (*vectors[SELECTED])[row]
					/////delete (*vectors[SELECTED])[row];
					////(*vectors[SELECTED])[row] = 0;
					//ASSERT(!(*this->vv_props[SELECTED])[row]);
				}
				else if (strValue.CompareNoCase(PSZ_CONSTANT) == 0)
				{
					CGridTimeSeries::ModeType mt = CGridTimeSeries::CONSTANT;
					if ((*this->vv_props[SELECTED])[row])
					{
						if ((*this->vv_props[SELECTED])[row]->v[0] != (*this->vv_props[SELECTED])[row]->v[0])
						{
							double d;
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
							//[[this->SetCurrentFocusCell(this->grid, row, TYPE_COLUMN);
							this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							this->OnSelChanged(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_DISTANCE1_EDIT, d);
						}
						if ((*this->vv_props[SELECTED])[row]->v[1] != (*this->vv_props[SELECTED])[row]->v[1])
						{
							//[[this->SetCurrentFocusCell(this->grid, row, TYPE_COLUMN);
							this->grid.SetCurrentFocusCell(row, TYPE_COLUMN);
							this->OnSelChanged(row, TYPE_COLUMN);
							::DDX_Text(pDX, IDC_VALUE2_EDIT, d);
						}
						if ((*this->vv_props[SELECTED])[row]->dist2 != (*this->vv_props[SELECTED])[row]->dist2)
						{
							//[[this->SetCurrentFocusCell(this->grid, row, TYPE_COLUMN);
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
					// (*vectors[SELECTED])[row]
				}
				else if (strValue.CompareNoCase(PSZ_XYZ) == 0)
				{
					CGridTimeSeries::ModeType mt = CGridTimeSeries::XYZ;
					// (*vectors[SELECTED])[row]
				}
				//{{{{
				delete this->v_times[row];
				this->v_times[row] = new Ctime(time);
				//}}}}
			}
			////{{
			else
			{
				delete this->v_times[row];
				this->v_times[row] = 0;
			}
			////}}
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
					this->grid.SetItemText(row, UNIT_COLUMN, this->DefaultUnits);
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

				if (row == cell.row)
				{
					this->DDX_Property(pDX, *(*this->vv_props[SELECTED])[row]);
				}
			}
		}

		ASSERT(this->grid.GetFixedColumnCount() <= cell.col && cell.col < this->grid.GetColumnCount());
		ASSERT(this->grid.GetFixedRowCount() <= cell.row && cell.row < this->grid.GetRowCount());
		
// COMMENT: {6/25/2009 10:15:24 PM}		static NM_GRIDVIEW nmgv;
// COMMENT: {6/25/2009 10:15:24 PM}		nmgv.iRow         = cell.row;
// COMMENT: {6/25/2009 10:15:24 PM}		nmgv.iColumn      = cell.col;
// COMMENT: {6/25/2009 10:15:24 PM}		nmgv.hdr.hwndFrom = this->grid.GetSafeHwnd();
// COMMENT: {6/25/2009 10:15:24 PM}		nmgv.hdr.idFrom   = this->grid.GetDlgCtrlID();
// COMMENT: {6/25/2009 10:15:24 PM}		nmgv.hdr.code     = GVN_SELCHANGED;
// COMMENT: {6/25/2009 10:15:24 PM}		ASSERT(::IsWindow(nmgv.hdr.hwndFrom));
// COMMENT: {6/25/2009 10:15:24 PM}
// COMMENT: {6/25/2009 10:15:24 PM}		CWnd *pOwner = this->grid.GetOwner();
// COMMENT: {6/25/2009 10:15:24 PM}		if (pOwner && ::IsWindow(pOwner->m_hWnd))
// COMMENT: {6/25/2009 10:15:24 PM}		{
// COMMENT: {6/25/2009 10:15:24 PM}			pOwner->PostMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
// COMMENT: {6/25/2009 10:15:24 PM}		}
		this->OnSelChanged(cell.row, cell.col);

		this->grid.RedrawWindow();
	}
	TRACE("Out %s\n", __FUNCTION__);
}
