// PrintFreqPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PrintFreqPropertyPage.h"

#include "Global.h"
#include <set>

// CPrintFreqPropertyPage dialog

IMPLEMENT_DYNAMIC(CPrintFreqPropertyPage, baseCPrintFreqPropertyPage)
CPrintFreqPropertyPage::CPrintFreqPropertyPage()
	: baseCPrintFreqPropertyPage(CPrintFreqPropertyPage::IDD)
{
}

CPrintFreqPropertyPage::~CPrintFreqPropertyPage()
{
}

#define COLLECT_TIMES_MACRO(name) \
	do { \
		pSeries = &name; \
		for (iter = pSeries->begin(); iter != pSeries->end(); ++iter) times.insert(iter->first); \
	} while(0)

void CPrintFreqPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	baseCPrintFreqPropertyPage::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_GRID, m_Grid);
	
	int nState;
	if (!pDX->m_bSaveAndValidate)
	{
		if (this->m_printFreq.save_final_heads == TRUE)
		{
			nState = BST_CHECKED;
		}
		else
		{
			nState = BST_UNCHECKED;
		}
	}
	DDX_Check(pDX, IDC_CHECK_SAVE_HEADS, nState);
	if (pDX->m_bSaveAndValidate)
	{
		if (nState == BST_CHECKED)
		{
			this->m_printFreq.save_final_heads = TRUE;
		}
		else
		{
			this->m_printFreq.save_final_heads = FALSE;
		}
	}

	if (this->m_Grid.GetColumnCount() == 0)
	{
		try
		{
			this->m_Grid.SetRowCount(20);
			this->m_Grid.SetColumnCount(4);
			this->m_Grid.SetFixedRowCount(1);
			this->m_Grid.SetFixedColumnCount(2);
			this->m_Grid.EnableTitleTips(FALSE);
			this->m_Grid.SetRowResize(FALSE);
			this->m_Grid.SetCurrentFocusCell(2, 2);

			std::vector<LPCTSTR> options;
			options.push_back(_T("default"));
			options.push_back(_T("seconds"));
			options.push_back(_T("minutes"));
			options.push_back(_T("hours"));
			options.push_back(_T("days"));
			options.push_back(_T("years"));
			options.push_back(_T("step"));
			options.push_back(_T("end"));
			this->m_Grid.SetColumnOptions(3, options);

			std::vector<LPCTSTR> times;
			times.push_back(_T("default"));
			times.push_back(_T("seconds"));
			times.push_back(_T("minutes"));
			times.push_back(_T("hours"));
			times.push_back(_T("days"));
			times.push_back(_T("years"));
			this->m_Grid.SetCellOptions(1, 3, times);
		}
		catch (CMemoryException *e)
		{
			e->ReportError();
			e->Delete();
		}

		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask = GVIF_FORMAT;
		for (int row = 0; row < this->m_Grid.GetRowCount(); ++row)
		{
			defaultFormat.row = row;
			for (int col = 0; col < this->m_Grid.GetColumnCount(); ++col)
			{ 
				defaultFormat.col = col;
				if (col == 2 && row != 0)
				{
					defaultFormat.nFormat = DT_RIGHT|DT_BOTTOM|DT_END_ELLIPSIS;
				}
				else
				{
					defaultFormat.nFormat = DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS;
				}
				this->m_Grid.SetItem(&defaultFormat);
			}
		}

		this->m_Grid.SetItemText(0, 0, _T("Output"));
		this->m_Grid.SetItemText(0, 1, _T("Output file"));

		this->m_Grid.SetItemText(0, 2, _T("Print every"));
		this->m_Grid.SetItemText(0, 3, _T("Units"));

		this->m_Grid.SetItemText(1, 0, _T("Time"));
		
		this->m_Grid.SetItemText(2, 0, _T("Flow rates in boundary condition cells"));		
		this->m_Grid.SetItemText(2, 1, _T("*.O.bcf"));
		
		this->m_Grid.SetItemText(3, 0, _T("Components"));
		this->m_Grid.SetItemText(3, 1, _T("*.O.comps"));
		
		this->m_Grid.SetItemText(4, 0, _T("Conductance"));
		this->m_Grid.SetItemText(4, 1, _T("*.O.kd"));
		
		this->m_Grid.SetItemText(5, 0, _T("Flow balance"));
		this->m_Grid.SetItemText(5, 1, _T("*.O.bal"));
		
		this->m_Grid.SetItemText(6, 0, _T("Force chemistry print"));
		this->m_Grid.SetItemText(6, 1, _T("*.O.chem"));
		
		this->m_Grid.SetItemText(7, 0, _T("HDF chemistry"));
		this->m_Grid.SetItemText(7, 1, _T("*.h5"));
		
		this->m_Grid.SetItemText(8, 0, _T("HDF heads"));
		this->m_Grid.SetItemText(8, 1, _T("*.h5"));
		
		this->m_Grid.SetItemText(9, 0, _T("HDF velocities"));
		this->m_Grid.SetItemText(9, 1, _T("*.h5"));
		
		this->m_Grid.SetItemText(10, 0, _T("Heads"));
		this->m_Grid.SetItemText(10, 1, _T("*.O.head"));
		
		this->m_Grid.SetItemText(11, 0, _T("Progress statistics"));
		this->m_Grid.SetItemText(11, 1, _T("*.log"));
		
		this->m_Grid.SetItemText(12, 0, _T("Velocities"));
		this->m_Grid.SetItemText(12, 1, _T("*.O.vel"));
		
		this->m_Grid.SetItemText(13, 0, _T("Wells"));
		this->m_Grid.SetItemText(13, 1, _T("*.O.wel"));
		
		this->m_Grid.SetItemText(14, 0, _T("XYZ chemistry"));
		this->m_Grid.SetItemText(14, 1, _T("*.xyz.chem"));
		
		this->m_Grid.SetItemText(15, 0, _T("XYZ components"));
		this->m_Grid.SetItemText(15, 1, _T("*.xyz.comps"));
		
		this->m_Grid.SetItemText(16, 0, _T("XYZ heads"));
		this->m_Grid.SetItemText(16, 1, _T("*.xyz.head"));
		
		this->m_Grid.SetItemText(17, 0, _T("XYZ velocities"));
		this->m_Grid.SetItemText(17, 1, _T("*.xyz.vel"));
		
		this->m_Grid.SetItemText(18, 0, _T("XYZ wells"));
		this->m_Grid.SetItemText(18, 1, _T("*.xyz.wel"));

		this->m_Grid.SetItemText(19, 0, _T("Boundary conditions"));
		this->m_Grid.SetItemText(19, 1, _T("*.O.probdef"));
		this->m_Grid.SetCheck(19, 2, BST_UNCHECKED);
		this->m_Grid.DisableCell(19, 3);

		// this->m_Grid.ExpandColumnsToFit();
		this->m_Grid.AutoSizeColumns();

		// manually resize grid
		this->m_Grid.SetColumnWidth(0, this->m_Grid.GetColumnWidth(0) * 0.85);
		this->m_Grid.SetColumnWidth(3, this->m_Grid.GetColumnWidth(3) * 1.5);

		this->m_Grid.SetItemText(1, 2, _T("0"));
		this->m_Grid.DisableCell(1, 2);

		this->m_Grid.SetItemText(1, 3, _T(""));
		this->m_Grid.DisableCell(1, 3);
	}

	//{{
	if (pDX->m_bSaveAndValidate)
	{
		this->m_printFreq.print_bc_flow.clear();
		this->m_printFreq.print_comp.clear();
		this->m_printFreq.print_conductances.clear();
		this->m_printFreq.print_flow_balance.clear();
		this->m_printFreq.print_force_chem.clear();
		this->m_printFreq.print_hdf_chem.clear();
		this->m_printFreq.print_hdf_head.clear();
		this->m_printFreq.print_hdf_velocity.clear();
		this->m_printFreq.print_head.clear();
		this->m_printFreq.print_statistics.clear();
		this->m_printFreq.print_velocity.clear();
		this->m_printFreq.print_wells.clear();
		this->m_printFreq.print_xyz_chem.clear();
		this->m_printFreq.print_xyz_comp.clear();
		this->m_printFreq.print_xyz_head.clear();
		this->m_printFreq.print_xyz_velocity.clear();
		this->m_printFreq.print_xyz_wells.clear();
		this->m_printFreq.print_bc.clear();
	}
	//}}


	Ctime zero;
	zero.SetValue(0.0);
	this->DDX_PRINT(pDX,  2, 2, this->m_printFreq.print_bc_flow[zero]);
	this->DDX_PRINT(pDX,  3, 2, this->m_printFreq.print_comp[zero]);
	this->DDX_PRINT(pDX,  4, 2, this->m_printFreq.print_conductances[zero]);
	this->DDX_PRINT(pDX,  5, 2, this->m_printFreq.print_flow_balance[zero]);
	this->DDX_PRINT(pDX,  6, 2, this->m_printFreq.print_force_chem[zero]);
	this->DDX_PRINT(pDX,  7, 2, this->m_printFreq.print_hdf_chem[zero]);
	this->DDX_PRINT(pDX,  8, 2, this->m_printFreq.print_hdf_head[zero]);
	this->DDX_PRINT(pDX,  9, 2, this->m_printFreq.print_hdf_velocity[zero]);
	this->DDX_PRINT(pDX, 10, 2, this->m_printFreq.print_head[zero]);
	this->DDX_PRINT(pDX, 11, 2, this->m_printFreq.print_statistics[zero]);
	this->DDX_PRINT(pDX, 12, 2, this->m_printFreq.print_velocity[zero]);
	this->DDX_PRINT(pDX, 13, 2, this->m_printFreq.print_wells[zero]);
	this->DDX_PRINT(pDX, 14, 2, this->m_printFreq.print_xyz_chem[zero]);
	this->DDX_PRINT(pDX, 15, 2, this->m_printFreq.print_xyz_comp[zero]);
	this->DDX_PRINT(pDX, 16, 2, this->m_printFreq.print_xyz_head[zero]);
	this->DDX_PRINT(pDX, 17, 2, this->m_printFreq.print_xyz_velocity[zero]);
	this->DDX_PRINT(pDX, 18, 2, this->m_printFreq.print_xyz_wells[zero]);
	this->DDX_PRINT(pDX, 19, 2, this->m_printFreq.print_bc[zero]);

	if (this->m_bFirstSetActive)
	{
		std::set<Ctime> times;

		CTimeSeries<Ctime> *pSeries;
		CTimeSeries<Ctime>::const_iterator iter;

		COLLECT_TIMES_MACRO(this->m_printFreq.print_bc_flow);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_comp);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_conductances);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_flow_balance);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_force_chem);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_hdf_chem);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_hdf_head);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_hdf_velocity);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_head);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_statistics);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_velocity);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_wells);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_xyz_chem);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_xyz_comp);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_xyz_head);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_xyz_velocity);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_xyz_wells);

		CTimeSeries<int> *pTSI = &this->m_printFreq.print_bc;
		CTimeSeries<int>::const_iterator ti = pTSI->begin();
		for (ti = pTSI->begin(); ti != pTSI->end(); ++ti)
		{
			times.insert(ti->first);
		}

		std::set<Ctime>::const_iterator s = times.begin();
		for (++s; s != times.end(); ++s)
		{
			OnBnClickedButtonAdd();
			CString str;
			int nCols = this->m_Grid.GetColumnCount() - 2;

			// time
			str.Format("%g", s->value);
			this->m_Grid.SetItemText(1, nCols, str);

			// [units]
			if (s->input)
			{
				this->m_Grid.SetItemText(1, nCols + 1, s->input);
			}

			this->DDX_PRINT(pDX,  2, nCols, this->m_printFreq.print_bc_flow, *s);
			this->DDX_PRINT(pDX,  3, nCols, this->m_printFreq.print_comp, *s);
			this->DDX_PRINT(pDX,  4, nCols, this->m_printFreq.print_conductances, *s);
			this->DDX_PRINT(pDX,  5, nCols, this->m_printFreq.print_flow_balance, *s);
			this->DDX_PRINT(pDX,  6, nCols, this->m_printFreq.print_force_chem, *s);
			this->DDX_PRINT(pDX,  7, nCols, this->m_printFreq.print_hdf_chem, *s);
			this->DDX_PRINT(pDX,  8, nCols, this->m_printFreq.print_hdf_head, *s);
			this->DDX_PRINT(pDX,  9, nCols, this->m_printFreq.print_hdf_velocity, *s);
			this->DDX_PRINT(pDX, 10, nCols, this->m_printFreq.print_head, *s);
			this->DDX_PRINT(pDX, 11, nCols, this->m_printFreq.print_statistics, *s);
			this->DDX_PRINT(pDX, 12, nCols, this->m_printFreq.print_velocity, *s);
			this->DDX_PRINT(pDX, 13, nCols, this->m_printFreq.print_wells, *s);
			this->DDX_PRINT(pDX, 14, nCols, this->m_printFreq.print_xyz_chem, *s);
			this->DDX_PRINT(pDX, 15, nCols, this->m_printFreq.print_xyz_comp, *s);
			this->DDX_PRINT(pDX, 16, nCols, this->m_printFreq.print_xyz_head, *s);
			this->DDX_PRINT(pDX, 17, nCols, this->m_printFreq.print_xyz_velocity, *s);
			this->DDX_PRINT(pDX, 18, nCols, this->m_printFreq.print_xyz_wells, *s);
			this->DDX_PRINT(pDX, 19, nCols, this->m_printFreq.print_bc, *s);
		}	
	}

	if (pDX->m_bSaveAndValidate)
	{
		int nCols = this->m_Grid.GetColumnCount();
		for (int nCol = 4; nCol < nCols; nCol+=2)
		{
			Ctime time;
			CString str;

			// time
			DDX_TextGridControl(pDX, IDC_GRID, 1, nCol, str);
			if (str.IsEmpty())
			{
				continue;
			}
			else
			{
				double dVal;
				DDX_TextGridControl(pDX, IDC_GRID, 1, nCol, dVal);
				time.SetValue(dVal);
			}

			// units
			DDX_TextGridControl(pDX, IDC_GRID, 1, nCol + 1, str);			
			if (str.IsEmpty() || str.Compare("default") == 0)
			{
				time.type = UNITS;
				if (time.input) delete[] time.input;
				time.input = NULL;
			}
			else
			{
				time.SetInput(str.GetString());
				ASSERT(time.type == UNITS);
			}

			this->DDX_PRINT(pDX,  2, nCol, this->m_printFreq.print_bc_flow, time);
			this->DDX_PRINT(pDX,  3, nCol, this->m_printFreq.print_comp, time);
			this->DDX_PRINT(pDX,  4, nCol, this->m_printFreq.print_conductances, time);
			this->DDX_PRINT(pDX,  5, nCol, this->m_printFreq.print_flow_balance, time);
			this->DDX_PRINT(pDX,  6, nCol, this->m_printFreq.print_force_chem, time);
			this->DDX_PRINT(pDX,  7, nCol, this->m_printFreq.print_hdf_chem, time);
			this->DDX_PRINT(pDX,  8, nCol, this->m_printFreq.print_hdf_head, time);
			this->DDX_PRINT(pDX,  9, nCol, this->m_printFreq.print_hdf_velocity, time);
			this->DDX_PRINT(pDX, 10, nCol, this->m_printFreq.print_head, time);
			this->DDX_PRINT(pDX, 11, nCol, this->m_printFreq.print_statistics, time);
			this->DDX_PRINT(pDX, 12, nCol, this->m_printFreq.print_velocity, time);
			this->DDX_PRINT(pDX, 13, nCol, this->m_printFreq.print_wells, time);
			this->DDX_PRINT(pDX, 14, nCol, this->m_printFreq.print_xyz_chem, time);
			this->DDX_PRINT(pDX, 15, nCol, this->m_printFreq.print_xyz_comp, time);
			this->DDX_PRINT(pDX, 16, nCol, this->m_printFreq.print_xyz_head, time);
			this->DDX_PRINT(pDX, 17, nCol, this->m_printFreq.print_xyz_velocity, time);
			this->DDX_PRINT(pDX, 18, nCol, this->m_printFreq.print_xyz_wells, time);
			this->DDX_PRINT(pDX, 19, nCol, this->m_printFreq.print_bc, time);
		}
		//this->m_printFreq.print_bc_flow.Minimize();
	}
}

void CPrintFreqPropertyPage::DDX_PRINT(CDataExchange* pDX, int nRow, int nCol, CTimeSeries<Ctime>& timeSeries, const Ctime& time)
{
	if (pDX->m_bSaveAndValidate)
	{
		this->DDX_PRINT(pDX, nRow, nCol, timeSeries[time]);
		//Ctime t;
		//this->DDX_PRINT(pDX, nRow, nCol, t);
		//timeSeries.lower_bound
	}
	else
	{
		CTimeSeries<Ctime>::const_iterator it = timeSeries.find(time);
		if (it != timeSeries.end())
		{
			this->DDX_PRINT(pDX, nRow, nCol, timeSeries[time]);
		}
	}
}

void CPrintFreqPropertyPage::DDX_PRINT(CDataExchange* pDX, int nRow, int nCol, CTimeSeries<int>& timeSeries, const Ctime& time)
{
	if (pDX->m_bSaveAndValidate)
	{
		this->DDX_PRINT(pDX, nRow, nCol, timeSeries[time]);
		//Ctime t;
		//this->DDX_PRINT(pDX, nRow, nCol, t);
		//timeSeries.lower_bound
	}
	else
	{
		CTimeSeries<int>::const_iterator it = timeSeries.find(time);
		if (it != timeSeries.end())
		{
			this->DDX_PRINT(pDX, nRow, nCol, timeSeries[time]);
		}
	}
}

void CPrintFreqPropertyPage::DDX_PRINT(CDataExchange* pDX, int nRow, int nCol, int& value)
{
	int nState;
	if (pDX->m_bSaveAndValidate)
	{
		DDX_GridCheck(pDX, IDC_GRID, nRow, nCol, nState);
		if (nState == BST_CHECKED)
		{
			value = TRUE;
		}
		else
		{
			value = FALSE;
		}
	}
	else
	{
		if (value == TRUE)
		{
			nState = BST_CHECKED;
		}
		else
		{
			nState = BST_UNCHECKED;
		}
		DDX_GridCheck(pDX, IDC_GRID, nRow, nCol, nState);
	}
}

///void CPrintFreqPropertyPage::DDX_PRINT(CDataExchange* pDX, int nRow, Ctime& time)
void CPrintFreqPropertyPage::DDX_PRINT(CDataExchange* pDX, int nRow, int nCol, Ctime& time)
{
	double dVal;
	CString empty("");
	CString step("step");
	CString deflt("default");
	CString units;

	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		double dVal;

		if (this->m_Grid.IsCellEnabled(nRow, nCol))
		{
			// Print every
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, str);
			if (str.IsEmpty())
			{
				time = Ctime();
				return;
			}
			else
			{
				DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, dVal);
				time.SetValue(dVal);
			}
		}

		// UNITS
		DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol + 1, str);			
		if (str.IsEmpty() || str.Compare("default") == 0)
		{
			time.type = UNITS;
			if (time.input) delete[] time.input;
			time.input = NULL;
		}
		else if (str.Compare("seconds") == 0)
		{
			time.SetInput(str.GetString());
			ASSERT(time.type == UNITS);
		}
		else if (str.Compare("minutes") == 0)
		{
			time.SetInput(str.GetString());
			ASSERT(time.type == UNITS);
		}
		else if (str.Compare("hours") == 0)
		{
			time.SetInput(str.GetString());
			ASSERT(time.type == UNITS);
		}
		else if (str.Compare("days") == 0)
		{
			time.SetInput(str.GetString());
			ASSERT(time.type == UNITS);
		}
		else if (str.Compare("years") == 0)
		{
			time.SetInput(str.GetString());
			ASSERT(time.type == UNITS);
		}
		else if (str.Compare("step") == 0)
		{
			if (time.value_defined != FALSE)
			{
				VERIFY(time.SetInput("step") == OK);
				ASSERT(time.type == STEP);
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		else if (str.Compare("end") == 0)
		{
			time.type = UNDEFINED;
			time.value_defined = FALSE;
			if (time.input) delete[] time.input;
			time.input = NULL;
			ASSERT(!this->m_Grid.IsCellEnabled(nRow, nCol));
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	else
	{
		switch (time.type)
		{
		case UNDEFINED:
			if (time.value_defined)
			{
				DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, empty);
				DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol + 1, deflt);
			}
			else
			{
				CString end(_T("end"));
				DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol, empty);
				DDX_GridDisableCell(pDX, IDC_GRID, nRow, nCol);
				DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol + 1, end);
			}
			break;
		case STEP:
			dVal = time.value;
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol,     dVal);
			DDX_GridEnableCell(pDX,  IDC_GRID, nRow, nCol);
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol + 1, step);
			break;
		case UNITS:
			dVal = time.value;
			if (time.input != NULL)
			{
				units = CGlobal::GetStdTimeUnits(time.input).c_str();
			}
			else
			{
				units = deflt;
			}
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol,      dVal);
			DDX_GridEnableCell(pDX,  IDC_GRID, nRow, nCol);
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol + 1, units);			
			break;
		default:
			ASSERT(FALSE);
		}
	}
}


BEGIN_MESSAGE_MAP(CPrintFreqPropertyPage, baseCPrintFreqPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)

	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnEndLabelEditGrid)
END_MESSAGE_MAP()


// CPrintFreqPropertyPage message handlers

void CPrintFreqPropertyPage::OnBnClickedButtonAdd()
{
	this->m_Grid.InsertColumn(_T("Print every"), DT_RIGHT|DT_BOTTOM|DT_END_ELLIPSIS);
	this->m_Grid.InsertColumn(_T("Units"), DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);

	int nCol = this->m_Grid.GetColumnCount() - 1;

	GV_ITEM item;
	item.mask = GVIF_TEXT | GVIF_STATE | GVIF_PARAM;
	for (item.row = 1; item.row < this->m_Grid.GetRowCount(); ++item.row)
	{
		item.col = nCol - 2;
		this->m_Grid.GetItem(&item);
		item.col = nCol;
		this->m_Grid.SetItem(&item);

		item.col = nCol - 3;
		this->m_Grid.GetItem(&item);
		item.col = nCol - 1;
		this->m_Grid.SetItem(&item);
	}
	this->m_Grid.SetItemText(1, nCol - 1, _T(""));
	this->m_Grid.EnableCell(1, nCol - 1);
	this->m_Grid.EnableCell(1, nCol);
	if (this->m_Grid.GetItemText(1, nCol).IsEmpty())
	{
		this->m_Grid.SetItemText(1, nCol, _T("default"));
	}

	this->m_Grid.SetColumnWidth(nCol, 64);
	this->m_Grid.SetItemFormat(0, nCol - 1, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
	this->m_Grid.SetCurrentFocusCell(1, nCol - 1);
	this->m_Grid.SetFocus();
	this->m_Grid.RedrawWindow();
}

void CPrintFreqPropertyPage::OnBnClickedButtonInsert()
{
	CCellID focusCell = this->m_Grid.GetFocusCell();

	int nColumn;

	if (focusCell.col < 4)
	{
		nColumn = 4;
	}
	else
	{
		nColumn = focusCell.col - (focusCell.col % 2);
	}

    if (nColumn >= this->m_Grid.GetColumnCount())
	{
		nColumn = this->m_Grid.InsertColumn(_T("Print every"), DT_RIGHT|DT_BOTTOM|DT_END_ELLIPSIS, -1);
		this->m_Grid.InsertColumn(_T("Units"), DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS, -1);
	}
	else
	{
		nColumn = this->m_Grid.InsertColumn(_T("Print every"), DT_RIGHT|DT_BOTTOM|DT_END_ELLIPSIS, nColumn);
		this->m_Grid.InsertColumn(_T("Units"), DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS, nColumn + 1);
	}

// COMMENT: {5/3/2005 5:25:37 PM}	std::vector<LPCTSTR> options;
// COMMENT: {5/3/2005 5:25:37 PM}	options.push_back(_T("default"));
// COMMENT: {5/3/2005 5:25:37 PM}	options.push_back(_T("seconds"));
// COMMENT: {5/3/2005 5:25:37 PM}	options.push_back(_T("minutes"));
// COMMENT: {5/3/2005 5:25:37 PM}	options.push_back(_T("hours"));
// COMMENT: {5/3/2005 5:25:37 PM}	options.push_back(_T("days"));
// COMMENT: {5/3/2005 5:25:37 PM}	options.push_back(_T("years"));
// COMMENT: {5/3/2005 5:25:37 PM}	options.push_back(_T("step"));
// COMMENT: {5/3/2005 5:25:37 PM}	this->m_Grid.SetColumnOptions(nColumn + 1, options);
// COMMENT: {5/3/2005 5:25:37 PM}
// COMMENT: {5/3/2005 5:25:37 PM}	std::vector<LPCTSTR> times;
// COMMENT: {5/3/2005 5:25:37 PM}	times.push_back(_T("default"));
// COMMENT: {5/3/2005 5:25:37 PM}	times.push_back(_T("seconds"));
// COMMENT: {5/3/2005 5:25:37 PM}	times.push_back(_T("minutes"));
// COMMENT: {5/3/2005 5:25:37 PM}	times.push_back(_T("hours"));
// COMMENT: {5/3/2005 5:25:37 PM}	times.push_back(_T("days"));
// COMMENT: {5/3/2005 5:25:37 PM}	times.push_back(_T("years"));
// COMMENT: {5/3/2005 5:25:37 PM}	this->m_Grid.SetCellOptions(1, nColumn + 1, times);

	GV_ITEM item;
	item.mask = GVIF_TEXT | GVIF_STATE | GVIF_PARAM;
	for (item.row = 1; item.row < this->m_Grid.GetRowCount(); ++item.row)
	{
		// Units
		item.col = nColumn - 1;
		this->m_Grid.GetItem(&item);
		item.col = nColumn + 1;
		this->m_Grid.SetItem(&item);

		// Print every
		item.col = nColumn - 2;
		this->m_Grid.GetItem(&item);
		item.col = nColumn;
		this->m_Grid.SetItem(&item);
	}
	this->m_Grid.SetItemText(1, nColumn, _T(""));
	this->m_Grid.EnableCell(1, nColumn);
	this->m_Grid.EnableCell(1, nColumn + 1);
	if (this->m_Grid.GetItemText(1, nColumn + 1).IsEmpty())
	{
		this->m_Grid.SetItemText(1, nColumn + 1, _T("default"));
	}

	this->m_Grid.SetColumnWidth(nColumn + 1, 64);
	this->m_Grid.SetItemFormat(0, nColumn, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
	this->m_Grid.RedrawWindow();
}

void CPrintFreqPropertyPage::OnBnClickedButtonDelete()
{
	CCellID focusCell = this->m_Grid.GetFocusCell();

	int nColumn;

	if (focusCell.col < 4)
	{
		::AfxMessageBox("Time 0 cannot be deleted");
		this->m_Grid.SetFocus();
	}
	else
	{
		nColumn = focusCell.col - (focusCell.col % 2);

// COMMENT: {7/12/2005 2:21:36 PM}		CTime time;
// COMMENT: {7/12/2005 2:21:36 PM}		this->DoDataExchange()
// COMMENT: {7/12/2005 2:21:36 PM}		this->UpdateData()
// COMMENT: {7/12/2005 2:21:36 PM}
// COMMENT: {7/12/2005 2:21:36 PM}		// time
// COMMENT: {7/12/2005 2:21:36 PM}		CString str;
// COMMENT: {7/12/2005 2:21:36 PM}		str = this->m_Grid.GetItemText(1, nColumn);
// COMMENT: {7/12/2005 2:21:36 PM}
// COMMENT: {7/12/2005 2:21:36 PM}		DDX_TextGridControl(pDX, IDC_GRID, 1, nCol, str);
// COMMENT: {7/12/2005 2:21:36 PM}		if (str.IsEmpty())
// COMMENT: {7/12/2005 2:21:36 PM}		{
// COMMENT: {7/12/2005 2:21:36 PM}			continue;
// COMMENT: {7/12/2005 2:21:36 PM}		}
// COMMENT: {7/12/2005 2:21:36 PM}		else
// COMMENT: {7/12/2005 2:21:36 PM}		{
// COMMENT: {7/12/2005 2:21:36 PM}			double dVal;
// COMMENT: {7/12/2005 2:21:36 PM}			DDX_TextGridControl(pDX, IDC_GRID, 1, nCol, dVal);
// COMMENT: {7/12/2005 2:21:36 PM}			time.SetValue(dVal);
// COMMENT: {7/12/2005 2:21:36 PM}		}
// COMMENT: {7/12/2005 2:21:36 PM}
// COMMENT: {7/12/2005 2:21:36 PM}		// units
// COMMENT: {7/12/2005 2:21:36 PM}		DDX_TextGridControl(pDX, IDC_GRID, 1, nCol + 1, str);			
// COMMENT: {7/12/2005 2:21:36 PM}		if (str.IsEmpty() || str.Compare("default") == 0)
// COMMENT: {7/12/2005 2:21:36 PM}		{
// COMMENT: {7/12/2005 2:21:36 PM}			time.type = UNITS;
// COMMENT: {7/12/2005 2:21:36 PM}			if (time.input) delete[] time.input;
// COMMENT: {7/12/2005 2:21:36 PM}			time.input = NULL;
// COMMENT: {7/12/2005 2:21:36 PM}		}
// COMMENT: {7/12/2005 2:21:36 PM}		else
// COMMENT: {7/12/2005 2:21:36 PM}		{
// COMMENT: {7/12/2005 2:21:36 PM}			time.SetInput(str.GetString());
// COMMENT: {7/12/2005 2:21:36 PM}			ASSERT(time.type == UNITS);
// COMMENT: {7/12/2005 2:21:36 PM}		}


		VERIFY(this->m_Grid.DeleteColumn(nColumn));
		VERIFY(this->m_Grid.DeleteColumn(nColumn));
		this->m_Grid.RedrawWindow();		
	}
}

BOOL CPrintFreqPropertyPage::OnInitDialog()
{
	baseCPrintFreqPropertyPage::OnInitDialog();

	// Add extra initialization here

	// Layout controls
	this->CreateRoot(VERTICAL)
		<< item(IDC_GRID, GREEDY)
		//<< itemFixed(VERTICAL, 3)
		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
			// << itemFixed(HORIZONTAL, 3)
			<< item(IDC_BUTTON_ADD, NORESIZE, 0, 0, 0, 0)
			<< item(IDC_BUTTON_INSERT, NORESIZE, 0, 0, 0, 0)
			<< item(IDC_BUTTON_DELETE, NORESIZE, 0, 0, 0, 0)
			<< itemGrowing(HORIZONTAL)
			<< item(IDC_CHECK_SAVE_HEADS, NORESIZE, 0, 0, 0, 0)			
			)
		<< item(IDC_DESC_RICHEDIT, ABSOLUTE_VERT)		
		;
	UpdateLayout();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPrintFreqPropertyPage::OnEndLabelEditGrid(NMHDR *pNotifyStruct, LRESULT *result)
{
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	CString str = this->m_Grid.GetItemText(pnmgv->iRow, pnmgv->iColumn);
	if (str.Compare(_T("end")) == 0 && pnmgv->iColumn % 2)
	{
		this->m_Grid.DisableCell(pnmgv->iRow, pnmgv->iColumn - 1);
		this->m_Grid.RedrawCell(pnmgv->iRow, pnmgv->iColumn - 1);
	}
	else
	{
		if (pnmgv->iRow > 1)
		{
			this->m_Grid.EnableCell(pnmgv->iRow, pnmgv->iColumn - 1);
			this->m_Grid.RedrawCell(pnmgv->iRow, pnmgv->iColumn - 1);
		}
	}
}
