// PrintFreqPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PrintFreqPropertyPage.h"

#include "Global.h"

// CPrintFreqPropertyPage dialog

IMPLEMENT_DYNAMIC(CPrintFreqPropertyPage, CPropertyPage)
CPrintFreqPropertyPage::CPrintFreqPropertyPage()
	: CPropertyPage(CPrintFreqPropertyPage::IDD)
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
	CPropertyPage::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_GRID, m_Grid);

	if (this->m_Grid.GetColumnCount() == 0) {
		TRY {
			this->m_Grid.SetRowCount(18);
			this->m_Grid.SetColumnCount(4);
			this->m_Grid.SetFixedRowCount(1);
			//this->m_Grid.SetFixedColumnCount(0);
			this->m_Grid.SetFixedColumnCount(2);
			this->m_Grid.EnableTitleTips(FALSE);
			this->m_Grid.EnableSelection(FALSE);

			std::vector<LPCTSTR> options;
			options.push_back(_T("default"));
			options.push_back(_T("seconds"));
			options.push_back(_T("minutes"));
			options.push_back(_T("hours"));
			options.push_back(_T("days"));
			options.push_back(_T("years"));
			options.push_back(_T("step"));

			this->m_Grid.SetColumnOptions(3, options);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask    = GVIF_FORMAT;
		for (int row = 0; row < this->m_Grid.GetRowCount(); ++row)
		{
			defaultFormat.row = row;
			for (int col = 0; col < this->m_Grid.GetColumnCount(); ++col)
			{ 
				defaultFormat.col = col;
				if (col == 2 && row != 0) {
					defaultFormat.nFormat = DT_RIGHT|DT_BOTTOM|DT_END_ELLIPSIS;
				}
				else {
					defaultFormat.nFormat = DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS;
				}
				this->m_Grid.SetItem(&defaultFormat);
			}
		}

		GV_ITEM Item;
		Item.mask = GVIF_TEXT;
		Item.row = 0;

		Item.col = 0;
		Item.szText = _T("Output");
		this->m_Grid.SetItem(&Item);

		Item.col = 1;
		Item.szText = _T("Output file");
		this->m_Grid.SetItem(&Item);

		Item.col = 2;
		Item.szText = _T("Print every");
		this->m_Grid.SetItem(&Item);

		Item.col = 3;
		Item.szText = _T("Units");
		this->m_Grid.SetItem(&Item);

// COMMENT: {6/9/2004 7:02:32 PM}		Item.col = 0;
// COMMENT: {6/9/2004 7:02:32 PM}		Item.row = 1;
// COMMENT: {6/9/2004 7:02:32 PM}		Item.szText = _T("Flow rates in boundary condition cells");
// COMMENT: {6/9/2004 7:02:32 PM}		this->m_Grid.SetItem(&Item);
// COMMENT: {6/9/2004 7:02:32 PM}
// COMMENT: {6/9/2004 7:02:32 PM}		Item.col = 1;
// COMMENT: {6/9/2004 7:02:32 PM}		Item.row = 1;
// COMMENT: {6/9/2004 7:02:32 PM}		Item.szText = _T("*.O.BCF");
// COMMENT: {6/9/2004 7:02:32 PM}		this->m_Grid.SetItem(&Item);

		Item.row = 1;
		Item.col = 0;
		Item.szText = _T("Flow rates in boundary condition cells");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.O.BCF");
		this->m_Grid.SetItem(&Item);

		Item.row = 2;
		Item.col = 0;
		Item.szText = _T("Components");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.O.comps");
		this->m_Grid.SetItem(&Item);

		Item.row = 3;
		Item.col = 0;
		Item.szText = _T("Conductance");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.O.kd");
		this->m_Grid.SetItem(&Item);

		Item.row = 4;
		Item.col = 0;
		Item.szText = _T("Flow balance");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.O.bal");
		this->m_Grid.SetItem(&Item);

		Item.row = 5;
		Item.col = 0;
		Item.szText = _T("Force chemistry print");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.O.chem");
		this->m_Grid.SetItem(&Item);

		Item.row = 6;
		Item.col = 0;
		Item.szText = _T("HDF chemistry");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.h5");
		this->m_Grid.SetItem(&Item);

		Item.row = 7;
		Item.col = 0;
		Item.szText = _T("HDF heads");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.h5");
		this->m_Grid.SetItem(&Item);

		Item.row = 8;
		Item.col = 0;
		Item.szText = _T("HDF velocities");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.h5");
		this->m_Grid.SetItem(&Item);

		Item.row = 9;
		Item.col = 0;
		Item.szText = _T("Heads");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.O.head");
		this->m_Grid.SetItem(&Item);

		Item.row = 10;
		Item.col = 0;
		Item.szText = _T("Progress statistics");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.log");
		this->m_Grid.SetItem(&Item);

		Item.row = 11;
		Item.col = 0;
		Item.szText = _T("Velocities");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.O.vel");
		this->m_Grid.SetItem(&Item);

		Item.row = 12;
		Item.col = 0;
		Item.szText = _T("Wells");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.O.wel");
		this->m_Grid.SetItem(&Item);

		Item.row = 13;
		Item.col = 0;
		Item.szText = _T("XYZ chemistry");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.xyz.chem");
		this->m_Grid.SetItem(&Item);

		Item.row = 14;
		Item.col = 0;
		Item.szText = _T("XYZ components");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.xyz.comps");
		this->m_Grid.SetItem(&Item);

		Item.row = 15;
		Item.col = 0;
		Item.szText = _T("XYZ heads");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.xyz.head");
		this->m_Grid.SetItem(&Item);

		Item.row = 16;
		Item.col = 0;
		Item.szText = _T("XYZ velocities");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.xyz.vel");
		this->m_Grid.SetItem(&Item);

		Item.row = 17;
		Item.col = 0;
		Item.szText = _T("XYZ wells");
		this->m_Grid.SetItem(&Item);
		Item.col = 1;
		Item.szText = _T("*.xyz.wel");
		this->m_Grid.SetItem(&Item);

		// this->m_Grid.ExpandColumnsToFit();
		this->m_Grid.AutoSizeColumns();

		Item.row = 2;
		Item.col = 3;
		Item.szText = _T("seconds");
		this->m_Grid.SetItem(&Item);

		// manually resize grid
		this->m_Grid.SetColumnWidth(0, this->m_Grid.GetColumnWidth(0) * 0.85);
		this->m_Grid.SetColumnWidth(3, this->m_Grid.GetColumnWidth(3) * 1.5);

		Item.row = 2;
		Item.col = 3;
		Item.szText = _T("");
		this->m_Grid.SetItem(&Item);
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
		if (str.IsEmpty()) {
			time.type = UNDEFINED;
			time.value_defined = FALSE;
			return;
		}
		else {
			DDX_TextGridControl(pDX, IDC_GRID, nRow, 2, dVal);
			time.SetValue(dVal);
		}

		// UNITS
		DDX_TextGridControl(pDX, IDC_GRID, nRow, 3, str);			
		if (str.IsEmpty()) {
			// TODO
			ASSERT(FALSE); // should never be empty
		}
		else
		{
			if (str.Compare("default") == 0) {
				time.type = UNITS;
				if (time.input) delete[] time.input;
				time.input = NULL;
			}
			else if (str.Compare("seconds") == 0) {
				time.SetInput(str.GetString());
				ASSERT(time.type == UNITS);
			}
			else if (str.Compare("minutes") == 0) {
				time.SetInput(str.GetString());
				ASSERT(time.type == UNITS);
			}
			else if (str.Compare("hours") == 0) {
				time.SetInput(str.GetString());
				ASSERT(time.type == UNITS);
			}
			else if (str.Compare("days") == 0) {
				time.SetInput(str.GetString());
				ASSERT(time.type == UNITS);
			}
			else if (str.Compare("years") == 0) {
				time.SetInput(str.GetString());
				ASSERT(time.type == UNITS);
			}
			else if (str.Compare("step") == 0) {
				time.type = STEP;
			}
			else {
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


BEGIN_MESSAGE_MAP(CPrintFreqPropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CPrintFreqPropertyPage message handlers
