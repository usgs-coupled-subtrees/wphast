// PrintInitialPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PrintInitialPropertyPage.h"

#include "Global.h"


// CPrintInitialPropertyPage dialog

IMPLEMENT_DYNAMIC(CPrintInitialPropertyPage, CPropertyPage)
CPrintInitialPropertyPage::CPrintInitialPropertyPage()
	: CPropertyPage(CPrintInitialPropertyPage::IDD)
{
}

CPrintInitialPropertyPage::~CPrintInitialPropertyPage()
{
}

#define SET_ROW_MACRO1(GRID, TITLE, SUFFIX, DATA) \
	do { \
		++nRow; \
		this->GRID.SetItemText(nRow, 0, TITLE); \
		this->GRID.SetItemText(nRow, 1, SUFFIX); \
	} while(0)

#define SET_ROW_MACRO2(GRID, TITLE, SUFFIX, DATA) \
	do { \
		++nRow; \
		this->GRID.SetCheck(nRow, 2, this->m_PrintInput.DATA); \
	} while(0)

#define GET_ROW_MACRO(GRID, TITLE, SUFFIX, DATA) \
	do { \
		++nRow; \
		this->m_PrintInput.DATA = this->GRID.GetCheck(nRow, 2); \
	} while(0)

#define EXPAND_LEFT_MACRO(GRID, MACRO) \
	MACRO(GRID, _T("Boundary conditions"),        _T("*.O.probdef"), bc); \
	MACRO(GRID, _T("Components"),                 _T("*.O.comps"),   comp); \
	MACRO(GRID, _T("Conductance"),                _T("*.O.kd"),      conductances); \
	MACRO(GRID, _T("Echo input"),                 _T("*.log"),       echo_input); \
	MACRO(GRID, _T("Fluid properties"),           _T("*.O.probdef"), fluid); \
	MACRO(GRID, _T("Force chemistry print"),      _T("*.O.chem"),    force_chem); \
	MACRO(GRID, _T("HDF chemistry"),              _T("*.h5"),        hdf_chem); \
	MACRO(GRID, _T("HDF heads"),                  _T("*.h5"),        hdf_head); \
	MACRO(GRID, _T("HDF steady flow velocities"), _T("*.h5"),        hdf_ss_vel); \
	MACRO(GRID, _T("Heads"),                      _T("*.O.head"),    head); \

#define EXPAND_RIGHT_MACRO(GRID, MACRO) \
	MACRO(GRID, _T("Media properties"),           _T("*.O.probdef"), media); \
	MACRO(GRID, _T("Solution method"),            _T("*.O.probdef"), method); \
	MACRO(GRID, _T("Steady flow velocities"),     _T("*.O.vel"),     ss_vel); \
	MACRO(GRID, _T("Wells"),                      _T("*.O.wel"),     wells); \
	MACRO(GRID, _T("XYZ chemistry"),              _T("*.xyz.chem"),  xyz_chem); \
	MACRO(GRID, _T("XYZ components"),             _T("*.xyz.comps"), xyz_comp); \
	MACRO(GRID, _T("XYZ heads"),                  _T("*.xyz.head"),  xyz_head); \
	MACRO(GRID, _T("XYZ steady flow velocities"), _T("*.xyz.vel"),   xyz_ss_vel); \
	MACRO(GRID, _T("XYZ wells"),                  _T("*.xyz.wel"),   xyz_wells); \

#define START_EXPAND_MACRO()  { int nRow = 0
#define END_EXPAND_MACRO() }


void CPrintInitialPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_GridControl(pDX, IDC_GRID_LEFT, this->m_GridLeft);
	DDX_GridControl(pDX, IDC_GRID_RIGHT, this->m_GridRight);

	if (this->m_GridLeft.GetColumnCount() == 0)
	{
		try
		{
			this->m_GridLeft.SetRowCount(11);
			this->m_GridLeft.SetColumnCount(3);
			this->m_GridLeft.SetFixedRowCount(1);
			this->m_GridLeft.SetFixedColumnCount(2);
			this->m_GridLeft.EnableTitleTips(FALSE);
			this->m_GridLeft.SetRowResize(FALSE);
			this->m_GridLeft.SetCurrentFocusCell(1, 2);
		}
		catch (CMemoryException *e)
		{
			e->ReportError();
			e->Delete();
		}

		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask = GVIF_FORMAT;
		for (int row = 0; row < this->m_GridLeft.GetRowCount(); ++row)
		{
			defaultFormat.row = row;
			for (int col = 0; col < this->m_GridLeft.GetColumnCount(); ++col)
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
				this->m_GridLeft.SetItem(&defaultFormat);
			}
		}

		this->m_GridLeft.SetItemText(0, 0, _T("Print initial"));
		this->m_GridLeft.SetItemText(0, 1, _T("Output file"));

		START_EXPAND_MACRO();
		EXPAND_LEFT_MACRO(m_GridLeft, SET_ROW_MACRO1);
		END_EXPAND_MACRO();

		this->m_GridLeft.AutoSizeColumns();

		// manually resize grid
		this->m_GridLeft.SetColumnWidth(0, this->m_GridLeft.GetColumnWidth(0) * 0.85);
		this->m_GridLeft.SetColumnWidth(2, this->m_GridLeft.GetColumnWidth(2) * 4.45);
	}
	if (this->m_GridRight.GetColumnCount() == 0)
	{
		try
		{
			this->m_GridRight.SetRowCount(11);
			this->m_GridRight.SetColumnCount(3);
			this->m_GridRight.SetFixedRowCount(1);
			this->m_GridRight.SetFixedColumnCount(2);
			this->m_GridRight.EnableTitleTips(FALSE);
			this->m_GridRight.SetRowResize(FALSE);
			this->m_GridRight.SetCurrentFocusCell(2, 2);
		}
		catch (CMemoryException *e)
		{
			e->ReportError();
			e->Delete();
		}

		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask = GVIF_FORMAT;
		for (int row = 0; row < this->m_GridRight.GetRowCount(); ++row)
		{
			defaultFormat.row = row;
			for (int col = 0; col < this->m_GridRight.GetColumnCount(); ++col)
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
				this->m_GridRight.SetItem(&defaultFormat);
			}
		}

		this->m_GridRight.SetItemText(0, 0, _T("Print initial"));
		this->m_GridRight.SetItemText(0, 1, _T("Output file"));

		START_EXPAND_MACRO();
		EXPAND_RIGHT_MACRO(m_GridRight, SET_ROW_MACRO1);
		END_EXPAND_MACRO();

		this->m_GridRight.AutoSizeColumns();

		// manually resize grid
		this->m_GridRight.SetColumnWidth(0, this->m_GridRight.GetColumnWidth(0) * 0.85);
		this->m_GridRight.SetColumnWidth(2, this->m_GridRight.GetColumnWidth(2) * 4.5);
	}

	if (pDX->m_bSaveAndValidate)
	{
		START_EXPAND_MACRO();
		EXPAND_LEFT_MACRO(m_GridLeft, GET_ROW_MACRO);
		END_EXPAND_MACRO();

		START_EXPAND_MACRO();
		EXPAND_RIGHT_MACRO(m_GridRight, GET_ROW_MACRO);
		END_EXPAND_MACRO();
	}
	else
	{
		START_EXPAND_MACRO();
		EXPAND_LEFT_MACRO(m_GridLeft, SET_ROW_MACRO2);
		END_EXPAND_MACRO();

		START_EXPAND_MACRO();
		EXPAND_RIGHT_MACRO(m_GridRight, SET_ROW_MACRO2);
		END_EXPAND_MACRO();
	}
}


BEGIN_MESSAGE_MAP(CPrintInitialPropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CPrintInitialPropertyPage message handlers
