// PrintFreqPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PrintFreqPropertyPage.h"

#include "Global.h"
#include <set>

// CPrintFreqPropertyPage dialog


const int NR_CHH =  0; // heading
const int NR_TIM =  1; // time

const int NR_PRO =  2; // print_bc
const int NR_COM =  3; // print_comp
const int NR_CON =  4; // print_conductances
const int NR_EPD =  5; // print_end_of_period
const int NR_BAL =  6; // print_flow_balance
const int NR_BCF =  7; // print_bc_flow
const int NR_CHE =  8; // print_force_chem
const int NR_H5C =  9; // print_hdf_chem
const int NR_H5H = 10; // print_hdf_head
const int NR_H5V = 11; // print_hdf_velocity
const int NR_HEA = 12; // print_head
const int NR_LOG = 13; // print_statistics
const int NR_RES = 14; // print_restart
const int NR_VEL = 15; // print_velocity
const int NR_WEL = 16; // print_wells
const int NR_XYC = 17; // print_xyz_chem
const int NR_XYO = 18; // print_xyz_comp
const int NR_XYH = 19; // print_xyz_head
const int NR_XYV = 20; // print_xyz_velocity
const int NR_XYW = 21; // print_xyz_wells
const int NR_ZFH = 22; // print_zone_budget_heads
const int NR_ZFX = 23; // print_zone_budget
const int NR_ZFT = 24; // print_zone_budget_tsv

const int NROWS  = 25;


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

#define COLLECT_INTS_MACRO(name) \
	do { \
		pTSI = &name; \
		for (ti = pTSI->begin(); ti != pTSI->end(); ++ti) times.insert(ti->first); \
	} while(0)

void CPrintFreqPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	baseCPrintFreqPropertyPage::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_GRID, m_Grid);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);
	if (this->m_bFirstSetActive)
	{
		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
	}
	
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
			this->m_Grid.SetRowCount(NROWS);
			this->m_Grid.SetColumnCount(4);
			this->m_Grid.SetFixedRowCount(1);
			this->m_Grid.SetFixedColumnCount(2);
			this->m_Grid.EnableTitleTips(FALSE);
			this->m_Grid.SetRowResize(FALSE);
			this->m_Grid.SetCurrentFocusCell(2, 2);

			std::vector<CString> options;
			options.push_back(_T("default"));
			options.push_back(_T("seconds"));
			options.push_back(_T("minutes"));
			options.push_back(_T("hours"));
			options.push_back(_T("days"));
			options.push_back(_T("years"));
			options.push_back(_T("step"));
			options.push_back(_T("end"));
			this->m_Grid.SetColumnOptions(3, options);

			std::vector<CString> times;
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

		this->m_Grid.SetItemText(NR_CHH, 0, _T("Output"));
		this->m_Grid.SetItemText(NR_CHH, 1, _T("Output file"));

		this->m_Grid.SetItemText(NR_CHH, 2, _T("Print every"));
		this->m_Grid.SetItemText(NR_CHH, 3, _T("Units"));

		this->m_Grid.SetItemText(NR_TIM, 0, _T("Time"));
		
		this->m_Grid.SetItemText(NR_BCF, 0, _T("Flow rates in boundary condition cells"));		
		this->m_Grid.SetItemText(NR_BCF, 1, _T("*.bcf.txt"));

		this->m_Grid.SetItemText(NR_PRO, 0, _T("Boundary conditions"));
		this->m_Grid.SetItemText(NR_PRO, 1, _T("*.probdef.txt"));
		this->m_Grid.SetCheck(NR_PRO, 2, BST_UNCHECKED);
		this->m_Grid.DisableCell(NR_PRO, 3);
		
		this->m_Grid.SetItemText(NR_COM, 0, _T("Components"));
		this->m_Grid.SetItemText(NR_COM, 1, _T("*.comps.txt"));
		
		this->m_Grid.SetItemText(NR_CON, 0, _T("Conductance"));
		this->m_Grid.SetItemText(NR_CON, 1, _T("*.kd.txt"));

		this->m_Grid.SetItemText(NR_EPD, 0, _T("End of period default"));
		this->m_Grid.SetItemText(NR_EPD, 1, _T("(multiple)"));
		this->m_Grid.SetCheck(NR_EPD, 2, BST_CHECKED);
		this->m_Grid.DisableCell(NR_EPD, 3);
		
		this->m_Grid.SetItemText(NR_BAL, 0, _T("Flow balance"));
		this->m_Grid.SetItemText(NR_BAL, 1, _T("*.bal.txt"));
		
		this->m_Grid.SetItemText(NR_CHE, 0, _T("Force chemistry print"));
		this->m_Grid.SetItemText(NR_CHE, 1, _T("*.chem.txt"));
		
		this->m_Grid.SetItemText(NR_H5C, 0, _T("HDF chemistry"));
		this->m_Grid.SetItemText(NR_H5C, 1, _T("*.h5"));
		
		this->m_Grid.SetItemText(NR_H5H, 0, _T("HDF heads"));
		this->m_Grid.SetItemText(NR_H5H, 1, _T("*.h5"));
		
		this->m_Grid.SetItemText(NR_H5V, 0, _T("HDF velocities"));
		this->m_Grid.SetItemText(NR_H5V, 1, _T("*.h5"));
		
		this->m_Grid.SetItemText(NR_HEA, 0, _T("Heads"));
		this->m_Grid.SetItemText(NR_HEA, 1, _T("*.head.txt"));
		
		this->m_Grid.SetItemText(NR_LOG, 0, _T("Progress statistics"));
		this->m_Grid.SetItemText(NR_LOG, 1, _T("*.log.txt"));

		this->m_Grid.SetItemText(NR_RES, 0, _T("Restart file"));
		this->m_Grid.SetItemText(NR_RES, 1, _T("*.restart.tgz"));
		
		this->m_Grid.SetItemText(NR_VEL, 0, _T("Velocities"));
		this->m_Grid.SetItemText(NR_VEL, 1, _T("*.vel.txt"));
		
		this->m_Grid.SetItemText(NR_WEL, 0, _T("Wells"));
		this->m_Grid.SetItemText(NR_WEL, 1, _T("*.wel.txt"));
		
		this->m_Grid.SetItemText(NR_XYC, 0, _T("XYZ chemistry"));
		this->m_Grid.SetItemText(NR_XYC, 1, _T("*.chem.xyz.tsv"));
		
		this->m_Grid.SetItemText(NR_XYO, 0, _T("XYZ components"));
		this->m_Grid.SetItemText(NR_XYO, 1, _T("*.comps.xyz.tsv"));
		
		this->m_Grid.SetItemText(NR_XYH, 0, _T("XYZ heads"));
		this->m_Grid.SetItemText(NR_XYH, 1, _T("*.head.xyz.tsv"));
		
		this->m_Grid.SetItemText(NR_XYV, 0, _T("XYZ velocities"));
		this->m_Grid.SetItemText(NR_XYV, 1, _T("*.vel.xyz.tsv"));
		
		this->m_Grid.SetItemText(NR_XYW, 0, _T("XYZ wells"));
		this->m_Grid.SetItemText(NR_XYW, 1, _T("*.wel.xyz.tsv"));

		this->m_Grid.SetItemText(NR_ZFH, 0, _T("Zone flow xyzt"));
		this->m_Grid.SetItemText(NR_ZFH, 1, _T("(user defined)"));

		this->m_Grid.SetItemText(NR_ZFX, 0, _T("Zone flow information"));
		this->m_Grid.SetItemText(NR_ZFX, 1, _T("*.zf.txt"));

		this->m_Grid.SetItemText(NR_ZFT, 0, _T("Zone flow information tsv"));
		this->m_Grid.SetItemText(NR_ZFT, 1, _T("*.zf.tsv"));

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

	if (pDX->m_bSaveAndValidate)
	{
		this->m_printFreq.print_bc_flow.clear();
		this->m_printFreq.print_bc.clear();
		this->m_printFreq.print_comp.clear();
		this->m_printFreq.print_conductances.clear();
		this->m_printFreq.print_end_of_period.clear();
		this->m_printFreq.print_flow_balance.clear();
		this->m_printFreq.print_force_chem.clear();
		this->m_printFreq.print_hdf_chem.clear();
		this->m_printFreq.print_hdf_head.clear();
		this->m_printFreq.print_hdf_velocity.clear();
		this->m_printFreq.print_head.clear();
		this->m_printFreq.print_statistics.clear();
		this->m_printFreq.print_restart.clear();
		this->m_printFreq.print_velocity.clear();
		this->m_printFreq.print_wells.clear();
		this->m_printFreq.print_xyz_chem.clear();
		this->m_printFreq.print_xyz_comp.clear();
		this->m_printFreq.print_xyz_head.clear();
		this->m_printFreq.print_xyz_velocity.clear();
		this->m_printFreq.print_xyz_wells.clear();
		this->m_printFreq.print_zone_budget.clear();
		this->m_printFreq.print_zone_budget_xyzt.clear();
		this->m_printFreq.print_zone_budget_tsv.clear();
	}

	Ctime zero;
	zero.SetValue(0.0);
	this->DDX_PRINT(pDX, NR_BCF, 2, this->m_printFreq.print_bc_flow[zero]);
	this->DDX_PRINT(pDX, NR_PRO, 2, this->m_printFreq.print_bc[zero]);
	this->DDX_PRINT(pDX, NR_COM, 2, this->m_printFreq.print_comp[zero]);
	this->DDX_PRINT(pDX, NR_CON, 2, this->m_printFreq.print_conductances[zero]);
	this->DDX_PRINT(pDX, NR_EPD, 2, this->m_printFreq.print_end_of_period[zero]);
	this->DDX_PRINT(pDX, NR_BAL, 2, this->m_printFreq.print_flow_balance[zero]);
	this->DDX_PRINT(pDX, NR_CHE, 2, this->m_printFreq.print_force_chem[zero]);
	this->DDX_PRINT(pDX, NR_H5C, 2, this->m_printFreq.print_hdf_chem[zero]);
	this->DDX_PRINT(pDX, NR_H5H, 2, this->m_printFreq.print_hdf_head[zero]);
	this->DDX_PRINT(pDX, NR_H5V, 2, this->m_printFreq.print_hdf_velocity[zero]);
	this->DDX_PRINT(pDX, NR_HEA, 2, this->m_printFreq.print_head[zero]);
	this->DDX_PRINT(pDX, NR_LOG, 2, this->m_printFreq.print_statistics[zero]);
	this->DDX_PRINT(pDX, NR_RES, 2, this->m_printFreq.print_restart[zero]);
	this->DDX_PRINT(pDX, NR_ZFT, 2, this->m_printFreq.print_zone_budget_tsv[zero]);
	this->DDX_PRINT(pDX, NR_VEL, 2, this->m_printFreq.print_velocity[zero]);
	this->DDX_PRINT(pDX, NR_WEL, 2, this->m_printFreq.print_wells[zero]);
	this->DDX_PRINT(pDX, NR_XYC, 2, this->m_printFreq.print_xyz_chem[zero]);
	this->DDX_PRINT(pDX, NR_XYO, 2, this->m_printFreq.print_xyz_comp[zero]);
	this->DDX_PRINT(pDX, NR_XYH, 2, this->m_printFreq.print_xyz_head[zero]);
	this->DDX_PRINT(pDX, NR_XYV, 2, this->m_printFreq.print_xyz_velocity[zero]);
	this->DDX_PRINT(pDX, NR_XYW, 2, this->m_printFreq.print_xyz_wells[zero]);
	this->DDX_PRINT(pDX, NR_ZFX, 2, this->m_printFreq.print_zone_budget[zero]);
	this->DDX_PRINT(pDX, NR_ZFH, 2, this->m_printFreq.print_zone_budget_xyzt[zero]);


	if (this->m_bFirstSetActive)
	{
		std::set<Ctime> times;

		CTimeSeries<Ctime> *pSeries;
		CTimeSeries<Ctime>::const_iterator iter;

		CTimeSeries<int> *pTSI;
		CTimeSeries<int>::const_iterator ti;

		COLLECT_TIMES_MACRO(this->m_printFreq.print_bc_flow);
		COLLECT_INTS_MACRO(this->m_printFreq.print_bc);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_comp);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_conductances);
		COLLECT_INTS_MACRO(this->m_printFreq.print_end_of_period);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_flow_balance);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_force_chem);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_hdf_chem);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_hdf_head);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_hdf_velocity);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_head);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_statistics);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_restart);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_zone_budget_tsv);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_velocity);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_wells);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_xyz_chem);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_xyz_comp);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_xyz_head);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_xyz_velocity);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_xyz_wells);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_zone_budget);
		COLLECT_TIMES_MACRO(this->m_printFreq.print_zone_budget_xyzt);

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

			this->DDX_PRINT(pDX, NR_BCF, nCols, this->m_printFreq.print_bc_flow, *s);
			this->DDX_PRINT(pDX, NR_PRO, nCols, this->m_printFreq.print_bc, *s);
			this->DDX_PRINT(pDX, NR_COM, nCols, this->m_printFreq.print_comp, *s);
			this->DDX_PRINT(pDX, NR_CON, nCols, this->m_printFreq.print_conductances, *s);
			this->DDX_PRINT(pDX, NR_EPD, nCols, this->m_printFreq.print_end_of_period, *s);
			this->DDX_PRINT(pDX, NR_BAL, nCols, this->m_printFreq.print_flow_balance, *s);
			this->DDX_PRINT(pDX, NR_CHE, nCols, this->m_printFreq.print_force_chem, *s);
			this->DDX_PRINT(pDX, NR_H5C, nCols, this->m_printFreq.print_hdf_chem, *s);
			this->DDX_PRINT(pDX, NR_H5H, nCols, this->m_printFreq.print_hdf_head, *s);
			this->DDX_PRINT(pDX, NR_H5V, nCols, this->m_printFreq.print_hdf_velocity, *s);
			this->DDX_PRINT(pDX, NR_HEA, nCols, this->m_printFreq.print_head, *s);
			this->DDX_PRINT(pDX, NR_LOG, nCols, this->m_printFreq.print_statistics, *s);
			this->DDX_PRINT(pDX, NR_RES, nCols, this->m_printFreq.print_restart, *s);
			this->DDX_PRINT(pDX, NR_ZFT, nCols, this->m_printFreq.print_zone_budget_tsv, *s);
			this->DDX_PRINT(pDX, NR_VEL, nCols, this->m_printFreq.print_velocity, *s);
			this->DDX_PRINT(pDX, NR_WEL, nCols, this->m_printFreq.print_wells, *s);
			this->DDX_PRINT(pDX, NR_XYC, nCols, this->m_printFreq.print_xyz_chem, *s);
			this->DDX_PRINT(pDX, NR_XYO, nCols, this->m_printFreq.print_xyz_comp, *s);
			this->DDX_PRINT(pDX, NR_XYH, nCols, this->m_printFreq.print_xyz_head, *s);
			this->DDX_PRINT(pDX, NR_XYV, nCols, this->m_printFreq.print_xyz_velocity, *s);
			this->DDX_PRINT(pDX, NR_XYW, nCols, this->m_printFreq.print_xyz_wells, *s);
			this->DDX_PRINT(pDX, NR_ZFX, nCols, this->m_printFreq.print_zone_budget, *s);
			this->DDX_PRINT(pDX, NR_ZFH, nCols, this->m_printFreq.print_zone_budget_xyzt, *s);
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
			double dVal;
			DDX_TextGridControl(pDX, IDC_GRID, 1, nCol, dVal);
			time.SetValue(dVal);

			// units
			DDX_TextGridControl(pDX, IDC_GRID, 1, nCol + 1, str);			
			if (str.IsEmpty() || str.Compare("default") == 0)
			{
				time.type = TT_UNITS;
				if (time.input) delete[] time.input;
				time.input = NULL;
			}
			else
			{
				time.SetInput(str.GetString());
				ASSERT(time.type == TT_UNITS);
			}

			this->DDX_PRINT(pDX, NR_BCF, nCol, this->m_printFreq.print_bc_flow, time);
			this->DDX_PRINT(pDX, NR_COM, nCol, this->m_printFreq.print_comp, time);
			this->DDX_PRINT(pDX, NR_CON, nCol, this->m_printFreq.print_conductances, time);
			this->DDX_PRINT(pDX, NR_EPD, nCol, this->m_printFreq.print_end_of_period, time);
			this->DDX_PRINT(pDX, NR_BAL, nCol, this->m_printFreq.print_flow_balance, time);
			this->DDX_PRINT(pDX, NR_CHE, nCol, this->m_printFreq.print_force_chem, time);
			this->DDX_PRINT(pDX, NR_H5C, nCol, this->m_printFreq.print_hdf_chem, time);
			this->DDX_PRINT(pDX, NR_H5H, nCol, this->m_printFreq.print_hdf_head, time);
			this->DDX_PRINT(pDX, NR_H5V, nCol, this->m_printFreq.print_hdf_velocity, time);
			this->DDX_PRINT(pDX, NR_HEA, nCol, this->m_printFreq.print_head, time);
			this->DDX_PRINT(pDX, NR_LOG, nCol, this->m_printFreq.print_statistics, time);
			this->DDX_PRINT(pDX, NR_RES, nCol, this->m_printFreq.print_restart, time);
			this->DDX_PRINT(pDX, NR_ZFT, nCol, this->m_printFreq.print_zone_budget_tsv, time);
			this->DDX_PRINT(pDX, NR_VEL, nCol, this->m_printFreq.print_velocity, time);
			this->DDX_PRINT(pDX, NR_WEL, nCol, this->m_printFreq.print_wells, time);
			this->DDX_PRINT(pDX, NR_XYC, nCol, this->m_printFreq.print_xyz_chem, time);
			this->DDX_PRINT(pDX, NR_XYO, nCol, this->m_printFreq.print_xyz_comp, time);
			this->DDX_PRINT(pDX, NR_XYH, nCol, this->m_printFreq.print_xyz_head, time);
			this->DDX_PRINT(pDX, NR_XYV, nCol, this->m_printFreq.print_xyz_velocity, time);
			this->DDX_PRINT(pDX, NR_XYW, nCol, this->m_printFreq.print_xyz_wells, time);
			this->DDX_PRINT(pDX, NR_PRO, nCol, this->m_printFreq.print_bc, time);
			this->DDX_PRINT(pDX, NR_ZFX, nCol, this->m_printFreq.print_zone_budget, time);
			this->DDX_PRINT(pDX, NR_ZFH, nCol, this->m_printFreq.print_zone_budget_xyzt, time);
		}
	}
}

void CPrintFreqPropertyPage::DDX_PRINT(CDataExchange* pDX, int nRow, int nCol, CTimeSeries<Ctime>& timeSeries, const Ctime& time)
{
	if (pDX->m_bSaveAndValidate)
	{
		this->DDX_PRINT(pDX, nRow, nCol, timeSeries[time]);
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
				time.type = TT_STEP;
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
			time.type = TT_UNITS;
			if (time.input) delete[] time.input;
			time.input = NULL;
		}
		else if (str.Compare("seconds") == 0)
		{
			time.SetInput(str.GetString());
			ASSERT(time.type == TT_UNITS);
		}
		else if (str.Compare("minutes") == 0)
		{
			time.SetInput(str.GetString());
			ASSERT(time.type == TT_UNITS);
		}
		else if (str.Compare("hours") == 0)
		{
			time.SetInput(str.GetString());
			ASSERT(time.type == TT_UNITS);
		}
		else if (str.Compare("days") == 0)
		{
			time.SetInput(str.GetString());
			ASSERT(time.type == TT_UNITS);
		}
		else if (str.Compare("years") == 0)
		{
			time.SetInput(str.GetString());
			ASSERT(time.type == TT_UNITS);
		}
		else if (str.Compare("step") == 0)
		{
			if (time.value_defined != FALSE)
			{
				VERIFY(time.SetInput("step") == OK);
				ASSERT(time.type == TT_STEP);
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		else if (str.Compare("end") == 0)
		{
			time.type = TT_UNDEFINED;
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
		case TT_UNDEFINED:
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
		case TT_STEP:
			dVal = time.value;
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol,     dVal);
			DDX_GridEnableCell(pDX,  IDC_GRID, nRow, nCol);
			DDX_TextGridControl(pDX, IDC_GRID, nRow, nCol + 1, step);
			break;
		case TT_UNITS:
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
	ON_BN_SETFOCUS(IDC_CHECK_SAVE_HEADS, OnBnSetfocusCheckSaveHeads)

	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnSelChangedGrid)
	ON_NOTIFY(GVN_SETFOCUS, IDC_GRID, OnSelChangedGrid)
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

void CPrintFreqPropertyPage::OnBnSetfocusCheckSaveHeads()
{
	static std::string s;
	CGlobal::LoadRTFString(s, IDR_PR_FREQ_SAVE_HEADS_RTF);
	this->m_wndRichEditCtrl.SetWindowText(s.c_str());
}


void CPrintFreqPropertyPage::OnSelChangedGrid(NMHDR *pNotifyStruct, LRESULT *result)
{
	CCellID focus = this->m_Grid.GetFocusCell();

	if (!this->m_Grid.IsValid(focus)) return;
	
	static std::string s;

	switch (focus.row)
	{
	case NR_TIM:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_TIME_RTF);
		break;
	case NR_BCF:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_FR_BC_RTF);
		break;
	case NR_PRO:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_BC_RTF);
		break;
	case NR_COM:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_COMPS_RTF);
		break;
	case NR_CON:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_COND_RTF);
		break;
	case NR_EPD:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_EOP_DEFAULT_RTF);
		break;
	case NR_BAL:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_FLOW_BAL_RTF);
		break;
	case NR_CHE:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_FORCE_RTF);
		break;
	case NR_H5C:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_HDF_CHEM_RTF);
		break;
	case NR_H5H:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_HDF_HEAD_RTF);
		break;
	case NR_H5V:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_HDF_VEL_RTF);
		break;
	case NR_HEA:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_HEADS_RTF);
		break;
	case NR_LOG:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_PROG_RTF);
		break;
	case NR_RES:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_RESTART_RTF);
		break;
	case NR_ZFT:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_ZF_TSV_RTF);
		break;
	case NR_VEL:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_VEL_RTF);
		break;
	case NR_WEL:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_WELLS_RTF);
		break;
	case NR_XYC:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_XYZ_CHEM_RTF);
		break;
	case NR_XYO:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_XYZ_COMPS_RTF);
		break;
	case NR_XYH:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_XYZ_HEADS_RTF);
		break;
	case NR_XYV:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_XYZ_VEL_RTF);
		break;
	case NR_XYW:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_XYZ_WELLS_RTF);
		break;
	case NR_ZFX:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_ZF_RTF);
		break;
	case NR_ZFH:
		CGlobal::LoadRTFString(s, IDR_PR_FREQ_ZF_HEADS_RTF);
		break;
	}
	this->m_wndRichEditCtrl.SetWindowText(s.c_str());
}