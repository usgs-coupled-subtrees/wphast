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

#define SET_ROW_MACRO1(GRID, TITLE, SUFFIX, DATA, ID) \
	do { \
		++nRow; \
		this->GRID.SetItemText(nRow, 0, TITLE); \
		this->GRID.SetItemText(nRow, 1, SUFFIX); \
	} while(0)

#define SET_ROW_MACRO2(GRID, TITLE, SUFFIX, DATA, ID) \
	do { \
		++nRow; \
		this->GRID.SetCheck(nRow, 2, this->m_PrintInput.DATA); \
	} while(0)

#define GET_ROW_MACRO(GRID, TITLE, SUFFIX, DATA, ID) \
	do { \
		++nRow; \
		this->m_PrintInput.DATA = this->GRID.GetCheck(nRow, 2); \
	} while(0)


#define EXPAND_LEFT_MACRO(GRID, MACRO) \
	MACRO(GRID, _T("Boundary conditions"),        _T("*.probdef.txt"),   bc,            IDR_PR_INIT_BC_RTF); \
	MACRO(GRID, _T("Components"),                 _T("*.comps.txt"),     comp,          IDR_PR_INIT_COMPS_RTF); \
	MACRO(GRID, _T("Conductance"),                _T("*.kd.txt"),        conductances,  IDR_PR_INIT_COND_RTF); \
	MACRO(GRID, _T("Echo input"),                 _T("*.log.txt"),       echo_input,    IDR_PR_INIT_ECHO_RTF); \
	MACRO(GRID, _T("Fluid properties"),           _T("*.probdef.txt"),   fluid,         IDR_PR_INIT_FLUID_RTF); \
	MACRO(GRID, _T("Force chemistry print"),      _T("*.chem.txt"),      force_chem,    IDR_PR_INIT_FORCE_RTF); \
	MACRO(GRID, _T("HDF chemistry"),              _T("*.h5"),            hdf_chem,      IDR_PR_INIT_HDF_CHEM_RTF); \
	MACRO(GRID, _T("HDF heads"),                  _T("*.h5"),            hdf_head,      IDR_PR_INIT_HDF_HEAD_RTF); \
	MACRO(GRID, _T("HDF media"),                  _T("*.h5"),            hdf_media,     IDR_PR_INIT_HDF_MEDIA_RTF); \
	MACRO(GRID, _T("HDF steady flow velocities"), _T("*.h5"),            hdf_ss_vel,    IDR_PR_INIT_HDF_VEL_RTF); \

#define EXPAND_RIGHT_MACRO(GRID, MACRO) \
	MACRO(GRID, _T("Heads"),                      _T("*.head.txt"),      head,          IDR_PR_INIT_HEADS_RTF); \
	MACRO(GRID, _T("Media properties"),           _T("*.probdef.txt"),   media,         IDR_PR_INIT_MEDIA_RTF); \
	MACRO(GRID, _T("Solution method"),            _T("*.probdef.txt"),   method,        IDR_PR_INIT_SOL_METH_RTF); \
	MACRO(GRID, _T("Steady flow velocities"),     _T("*.vel.txt"),       ss_vel,        IDR_PR_INIT_SSFLOW_RTF); \
	MACRO(GRID, _T("Wells"),                      _T("*.wel.txt"),       wells,         IDR_PR_INIT_WELLS_RTF); \
	MACRO(GRID, _T("XYZ chemistry"),              _T("*.chem.xyz.tsv"),  xyz_chem,      IDR_PR_INIT_XYZ_CHEM_RTF); \
	MACRO(GRID, _T("XYZ components"),             _T("*.comps.xyz.tsv"), xyz_comp,      IDR_PR_INIT_XYZ_COMPS_RTF); \
	MACRO(GRID, _T("XYZ heads"),                  _T("*.head.xyz.tsv"),  xyz_head,      IDR_PR_INIT_XYZ_HEADS_RTF); \
	MACRO(GRID, _T("XYZ steady flow velocities"), _T("*.vel.xyz.tsv"),   xyz_ss_vel,    IDR_PR_INIT_XYZ_SSVEL_RTF); \
	MACRO(GRID, _T("XYZ wells"),                  _T("*.wel.xyz.tsv"),   xyz_wells,     IDR_PR_INIT_XYZ_WELLS_RTF); \

#define START_EXPAND_MACRO()  { int nRow = 0
#define END_EXPAND_MACRO() }

#define RTF_MACRO(GRID, TITLE, SUFFIX, DATA, ID) \
	do { \
		++nRow; \
		if (nRow == this->GRID.GetFocusCell().row) { \
			CGlobal::LoadRTFString(s, ID); \
			this->m_wndRichEditCtrl.SetWindowText(s.c_str()); \
		} \
	} while(0)

#define START_RTF_MACRO()  { int nRow = 0; static std::string s
#define END_RTF_MACRO() }

void CPrintInitialPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_GridControl(pDX, IDC_GRID_LEFT, this->m_GridLeft);
	DDX_GridControl(pDX, IDC_GRID_RIGHT, this->m_GridRight);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);
	if (this->m_bFirstSetActive)
	{
		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
	}

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

		// manually resize grid
		this->m_GridLeft.SetColumnWidth(0, 146);
		this->m_GridLeft.SetColumnWidth(1, 76);
		this->m_GridLeft.SetColumnWidth(2, 53);
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

		// manually resize grid
		this->m_GridRight.SetColumnWidth(0, 141);
		this->m_GridRight.SetColumnWidth(1, 86);
		this->m_GridRight.SetColumnWidth(2, 48);
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
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_LEFT, OnSelChangedLeft)
	ON_NOTIFY(GVN_SETFOCUS, IDC_GRID_LEFT, OnSelChangedLeft)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_RIGHT, OnSelChangedRight)
	ON_NOTIFY(GVN_SETFOCUS, IDC_GRID_RIGHT, OnSelChangedRight)
END_MESSAGE_MAP()

// CPrintInitialPropertyPage message handlers

void CPrintInitialPropertyPage::OnSelChangedLeft(NMHDR *pNotifyStruct, LRESULT *result)
{
	START_RTF_MACRO();
	EXPAND_LEFT_MACRO(m_GridLeft, RTF_MACRO);
	END_RTF_MACRO();
}

void CPrintInitialPropertyPage::OnSelChangedRight(NMHDR *pNotifyStruct, LRESULT *result)
{
	START_RTF_MACRO();
	EXPAND_RIGHT_MACRO(m_GridRight, RTF_MACRO);
	END_RTF_MACRO();
}
