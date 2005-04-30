// PrintFreqPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PrintFreqPropertyPage.h"

#include "Global.h"

// CPrintFreqPropertyPage dialog

IMPLEMENT_DYNAMIC(CPrintFreqPropertyPage, baseCPrintFreqPropertyPage)
CPrintFreqPropertyPage::CPrintFreqPropertyPage()
	: baseCPrintFreqPropertyPage(CPrintFreqPropertyPage::IDD)
{
// COMMENT: {6/10/2004 3:43:10 PM}	//{{ HACK
// COMMENT: {6/10/2004 3:43:10 PM}	this->m_printFreq.print_hdf_chem.SetValue(2000);
// COMMENT: {6/10/2004 3:43:10 PM}	this->m_printFreq.print_hdf_chem.SetInput("years");
// COMMENT: {6/10/2004 3:43:10 PM}
// COMMENT: {6/10/2004 3:43:10 PM}	this->m_printFreq.print_hdf_head.SetValue(2000);
// COMMENT: {6/10/2004 3:43:10 PM}	this->m_printFreq.print_hdf_head.SetInput("years");
// COMMENT: {6/10/2004 3:43:10 PM}
// COMMENT: {6/10/2004 3:43:10 PM}	this->m_printFreq.print_hdf_velocity.SetValue(2000);
// COMMENT: {6/10/2004 3:43:10 PM}	this->m_printFreq.print_hdf_velocity.SetInput("years");
// COMMENT: {6/10/2004 3:43:10 PM}	//}} HACK
}

CPrintFreqPropertyPage::~CPrintFreqPropertyPage()
{
}

void CPrintFreqPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	baseCPrintFreqPropertyPage::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_GRID, m_Grid);

	if (this->m_Grid.GetColumnCount() == 0)
	{
		try
		{
			this->m_Grid.SetRowCount(19);
			this->m_Grid.SetColumnCount(4);
			this->m_Grid.SetFixedRowCount(1);
			this->m_Grid.SetFixedColumnCount(2);
			this->m_Grid.EnableTitleTips(FALSE);

			std::vector<LPCTSTR> options;
			options.push_back(_T("default"));
			options.push_back(_T("seconds"));
			options.push_back(_T("minutes"));
			options.push_back(_T("hours"));
			options.push_back(_T("days"));
			options.push_back(_T("years"));
			options.push_back(_T("step"));
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
		this->m_Grid.SetItemText(2, 1, _T("*.O.BCF"));
		
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

// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX,  1, this->m_printFreq.print_bc_flow);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX,  2, this->m_printFreq.print_comp);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX,  3, this->m_printFreq.print_conductances);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX,  4, this->m_printFreq.print_flow_balance);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX,  5, this->m_printFreq.print_force_chem);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX,  6, this->m_printFreq.print_hdf_chem);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX,  7, this->m_printFreq.print_hdf_head);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX,  8, this->m_printFreq.print_hdf_velocity);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX,  9, this->m_printFreq.print_head);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX, 10, this->m_printFreq.print_statistics);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX, 11, this->m_printFreq.print_velocity);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX, 12, this->m_printFreq.print_wells);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX, 13, this->m_printFreq.print_xyz_chem);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX, 14, this->m_printFreq.print_xyz_comp);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX, 15, this->m_printFreq.print_xyz_head);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX, 16, this->m_printFreq.print_xyz_velocity);
// COMMENT: {2/23/2005 1:03:13 PM}	this->DDX_PRINT(pDX, 17, this->m_printFreq.print_xyz_wells);
}

void CPrintFreqPropertyPage::DDX_PRINT(CDataExchange* pDX, int nRow, Ctime& time)
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

		// Print every
		DDX_TextGridControl(pDX, IDC_GRID, nRow, 2, str);
		if (str.IsEmpty())
		{
			time.type = UNDEFINED;
			time.value_defined = FALSE;
			return;
		}
		else
		{
			DDX_TextGridControl(pDX, IDC_GRID, nRow, 2, dVal);
			time.SetValue(dVal);
		}

		// UNITS
		DDX_TextGridControl(pDX, IDC_GRID, nRow, 3, str);			
		if (str.IsEmpty())
		{
			// TODO
			ASSERT(FALSE); // should never be empty
		}
		else
		{
			if (str.Compare("default") == 0)
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
				time.type = STEP;
			}
			else
			{
				ASSERT(FALSE);
			}

// COMMENT: {6/10/2004 3:54:28 PM}			std::vector<LPCTSTR> options;
// COMMENT: {6/10/2004 3:54:28 PM}			options.push_back(_T("default"));
// COMMENT: {6/10/2004 3:54:28 PM}			options.push_back(_T("seconds"));
// COMMENT: {6/10/2004 3:54:28 PM}			options.push_back(_T("minutes"));
// COMMENT: {6/10/2004 3:54:28 PM}			options.push_back(_T("hours"));
// COMMENT: {6/10/2004 3:54:28 PM}			options.push_back(_T("days"));
// COMMENT: {6/10/2004 3:54:28 PM}			options.push_back(_T("years"));
// COMMENT: {6/10/2004 3:54:28 PM}			options.push_back(_T("step"));
			//}}
		}
	}
	else
	{
		if (time.type == UNDEFINED)
		{
			DDX_TextGridControl(pDX, IDC_GRID, nRow, 2, empty);
			DDX_TextGridControl(pDX, IDC_GRID, nRow, 3, deflt);
		}
		else if (time.type == STEP)
		{
			dVal = time.value;
			DDX_TextGridControl(pDX, IDC_GRID, nRow, 2, dVal);
			DDX_TextGridControl(pDX, IDC_GRID, nRow, 3, step);			
		}
		else if (time.type == UNITS)
		{
			dVal = time.value;
			ASSERT(this->m_units.time.si);
			// ASSERT(this->m_units.time.input || this->m_units.time.defined); 
			// ASSERT(this->m_units.time.defined != 0);
			if (time.input != NULL)
			{
				units = CGlobal::GetStdTimeUnits(time.input).c_str();
			}
			else
			{
// COMMENT: {6/10/2004 4:17:02 PM}				if (this->m_units.time.defined)
// COMMENT: {6/10/2004 4:17:02 PM}				{
// COMMENT: {6/10/2004 4:17:02 PM}					ASSERT(this->m_units.time.input != NULL);
// COMMENT: {6/10/2004 4:17:02 PM}					units = CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str();
// COMMENT: {6/10/2004 4:17:02 PM}				}
// COMMENT: {6/10/2004 4:17:02 PM}				else
// COMMENT: {6/10/2004 4:17:02 PM}				{
// COMMENT: {6/10/2004 4:17:02 PM}					// ASSERT(FALSE);
// COMMENT: {6/10/2004 4:17:02 PM}					units = CGlobal::GetStdTimeUnits(this->m_units.time.si).c_str();
// COMMENT: {6/10/2004 4:17:02 PM}				}
				units = deflt;
			}
			// units = (time.input) ? time.input : (this->m_units.time.defined) ? this->m_units.time.input : this->m_units.time.si;
			DDX_TextGridControl(pDX, IDC_GRID, nRow, 2, dVal);
			DDX_TextGridControl(pDX, IDC_GRID, nRow, 3, units);			
		}
	}
}


BEGIN_MESSAGE_MAP(CPrintFreqPropertyPage, baseCPrintFreqPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
END_MESSAGE_MAP()


// CPrintFreqPropertyPage message handlers

void CPrintFreqPropertyPage::OnBnClickedButtonAdd()
{
	this->m_Grid.InsertColumn(_T("Print every"), DT_RIGHT|DT_BOTTOM|DT_END_ELLIPSIS);
	this->m_Grid.InsertColumn(_T("Units"), DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);

	int nCol = this->m_Grid.GetColumnCount() - 1;

	std::vector<LPCTSTR> options;
	options.push_back(_T("default"));
	options.push_back(_T("seconds"));
	options.push_back(_T("minutes"));
	options.push_back(_T("hours"));
	options.push_back(_T("days"));
	options.push_back(_T("years"));
	options.push_back(_T("step"));
	this->m_Grid.SetColumnOptions(nCol, options);

	std::vector<LPCTSTR> times;
	times.push_back(_T("default"));
	times.push_back(_T("seconds"));
	times.push_back(_T("minutes"));
	times.push_back(_T("hours"));
	times.push_back(_T("days"));
	times.push_back(_T("years"));
	this->m_Grid.SetCellOptions(1, nCol, times);

	this->m_Grid.SetColumnWidth(nCol, 64);
	this->m_Grid.SetItemFormat(0, nCol - 1, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
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

	std::vector<LPCTSTR> options;
	options.push_back(_T("default"));
	options.push_back(_T("seconds"));
	options.push_back(_T("minutes"));
	options.push_back(_T("hours"));
	options.push_back(_T("days"));
	options.push_back(_T("years"));
	options.push_back(_T("step"));
	this->m_Grid.SetColumnOptions(nColumn + 1, options);

	std::vector<LPCTSTR> times;
	times.push_back(_T("default"));
	times.push_back(_T("seconds"));
	times.push_back(_T("minutes"));
	times.push_back(_T("hours"));
	times.push_back(_T("days"));
	times.push_back(_T("years"));
	this->m_Grid.SetCellOptions(1, nColumn + 1, times);

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
			)
		<< item(IDC_DESC_RICHEDIT, ABSOLUTE_VERT)		
		;
	UpdateLayout();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
