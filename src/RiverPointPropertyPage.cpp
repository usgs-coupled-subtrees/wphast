// RiverPointPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "RiverPointPropertyPage.h"

#include "Global.h"

#define strText szText

// CRiverPointPropertyPage dialog

IMPLEMENT_DYNAMIC(CRiverPointPropertyPage, CPropertyPage)
CRiverPointPropertyPage::CRiverPointPropertyPage()
: baseCRiverPointPropertyPage(CRiverPointPropertyPage::IDD)
, m_bFirstOrLastPoint(true)
{
	this->m_strHorizontalUnits.Format("(%s)", this->m_units.horizontal.si);
	this->m_strHeadUnits.Format("(%s)", this->m_units.head.si);
	this->m_strVerticalUnits.Format("(%s)", this->m_units.vertical.si);
	this->m_strRiverBedKUnits.Format("(%s)", this->m_units.river_bed_k.si);
	CGlobal::MinimizeTimeUnits(this->m_strRiverBedKUnits);
	this->m_strRiverBedThicknessUnits.Format("(%s)", this->m_units.river_bed_thickness.si);
}

CRiverPointPropertyPage::~CRiverPointPropertyPage()
{
}

void CRiverPointPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	baseCRiverPointPropertyPage::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_GRID_SCHEDULES, this->m_wndScheduleGrid);

	DDX_Text(pDX, IDC_X_UNITS_STATIC, this->m_strHorizontalUnits);
	DDX_Text(pDX, IDC_Y_UNITS_STATIC, this->m_strHorizontalUnits);
	DDX_Text(pDX, IDC_WIDTH_UNITS_STATIC, this->m_strHorizontalUnits);
	DDX_Text(pDX, IDC_DEPTH_UNITS_STATIC, this->m_strVerticalUnits);	
	DDX_Text(pDX, IDC_RIVER_K_UNITS_STATIC, this->m_strRiverBedKUnits);
	DDX_Text(pDX, IDC_RIVER_THICK_UNITS_STATIC, this->m_strRiverBedThicknessUnits);

	// Prepare Pump Sched. Grid
	//
	if (!pDX->m_bSaveAndValidate && this->m_wndScheduleGrid.GetColumnCount() == 0)
	{
		const int MIN_ROWS = 100;
		TRY
		{
			this->m_wndScheduleGrid.SetRowCount(MIN_ROWS);
			this->m_wndScheduleGrid.SetColumnCount(4);
			this->m_wndScheduleGrid.SetFixedRowCount(1);
			this->m_wndScheduleGrid.SetFixedColumnCount(0);
			this->m_wndScheduleGrid.EnableTitleTips(FALSE);
			// this->m_wndScheduleGrid.EnableSelection(FALSE); // this breaks CutSelectedText
			this->m_wndScheduleGrid.SetCurrentFocusCell(1, 2);

			std::vector<CString> options;
			options.push_back(_T("seconds"));
			options.push_back(_T("minutes"));
			options.push_back(_T("hours"));
			options.push_back(_T("days"));
			options.push_back(_T("years"));

			this->m_wndScheduleGrid.SetColumnOptions(1, options);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

			// set default format
			for (int row = 0; row < this->m_wndScheduleGrid.GetRowCount(); ++row)
			{
				for (int col = 0; col < this->m_wndScheduleGrid.GetColumnCount(); ++col)
				{ 
					this->m_wndScheduleGrid.SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
				}
			}

			CString str;
			str.Format(_T("Head %s"), m_strHeadUnits);

			VERIFY(this->m_wndScheduleGrid.SetItemText(0, 0, _T("Start")));
			VERIFY(this->m_wndScheduleGrid.SetItemText(0, 1, _T("Units")));
			VERIFY(this->m_wndScheduleGrid.SetItemText(0, 2, str));
			VERIFY(this->m_wndScheduleGrid.SetItemText(0, 3, _T("Solution")));

			VERIFY(this->m_wndScheduleGrid.SetItemText(1, 0, _T("0.0")));

			VERIFY(this->m_wndScheduleGrid.DisableCell(1, 0));
			VERIFY(this->m_wndScheduleGrid.DisableCell(1, 1));

			this->m_wndScheduleGrid.RedrawWindow();

			this->m_wndScheduleGrid.ExpandColumnsToFit();
	}

	// X
	//
	if (pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_X_EDIT, this->m_riverPt.x_user);
		this->m_riverPt.x_user_defined = TRUE;
	}
	else
	{
		ASSERT(this->m_riverPt.x_user_defined);
		DDX_Text(pDX, IDC_X_EDIT, this->m_riverPt.x_user);
	}

	// Y
	//
	if (pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_Y_EDIT, this->m_riverPt.y_user);
		this->m_riverPt.y_user_defined = TRUE;
	}
	else
	{
		ASSERT(this->m_riverPt.y_user_defined);
		DDX_Text(pDX, IDC_Y_EDIT, this->m_riverPt.y_user);
	}

	// Depth
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_DEPTH_EDIT, str);
		this->m_riverPt.depth_user_defined = FALSE;
		if (str.IsEmpty())
		{
			if (this->m_bFirstOrLastPoint)
			{
				::AfxMessageBox("Depth must be defined for at least the first and the last river points.");
				pDX->Fail();
			}
		}
		else
		{
			DDX_Text(pDX, IDC_DEPTH_EDIT, this->m_riverPt.depth_user);
			this->m_riverPt.depth_user_defined = TRUE;
		}
	}
	else
	{
		if (this->m_riverPt.depth_user_defined)
		{
			DDX_Text(pDX, IDC_DEPTH_EDIT, this->m_riverPt.depth_user);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_DEPTH_EDIT, empty);
		}
	}

	// bed_hydraulic_conductivity
	//	
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_RIVER_K_UNITS_EDIT, str);
		this->m_riverPt.k_defined = FALSE;
		if (str.IsEmpty())
		{
			if (this->m_bFirstOrLastPoint)
			{
				::AfxMessageBox("Bed hydraulic conductivity must be defined for at least the first and the last river points.");
				pDX->Fail();
			}
		}
		else
		{
			DDX_Text(pDX, IDC_RIVER_K_UNITS_EDIT, this->m_riverPt.k);
			m_riverPt.k_defined = TRUE;
		}
	}
	else
	{
		if (m_riverPt.k_defined)
		{
			DDX_Text(pDX, IDC_RIVER_K_UNITS_EDIT, this->m_riverPt.k);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_RIVER_K_UNITS_EDIT, empty);
		}
	}

	// width
	//	
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_WIDTH_EDIT, str);
		this->m_riverPt.width_user_defined = FALSE;
		if (str.IsEmpty())
		{
			if (this->m_bFirstOrLastPoint)
			{
				::AfxMessageBox("Width must be defined for at least the first and the last river points.");
				pDX->Fail();
			}
		}
		else
		{
			DDX_Text(pDX, IDC_WIDTH_EDIT, this->m_riverPt.width_user);
			m_riverPt.width_user_defined = TRUE;
		}
	}
	else
	{
		if (m_riverPt.width_user_defined)
		{
			DDX_Text(pDX, IDC_WIDTH_EDIT, this->m_riverPt.width_user);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_WIDTH_EDIT, empty);
		}
	}

	// bed_thickness
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_RIVER_THICK_UNITS_EDIT, str);
		this->m_riverPt.thickness_defined = FALSE;
		if (str.IsEmpty())
		{
			if (this->m_bFirstOrLastPoint)
			{
				::AfxMessageBox("Bed thickness must be defined for at least the first and the last river points.");
				pDX->Fail();
			}
		}
		else
		{
			DDX_Text(pDX, IDC_RIVER_THICK_UNITS_EDIT, this->m_riverPt.thickness);
			this->m_riverPt.thickness_defined = TRUE;
		}
	}
	else
	{
		if (this->m_riverPt.thickness_defined)
		{
			DDX_Text(pDX, IDC_RIVER_THICK_UNITS_EDIT, this->m_riverPt.thickness);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_RIVER_THICK_UNITS_EDIT, empty);
		}
	}

	// river states
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString strValue;
		Ctime time;

		if (this->m_units.time.defined)
		{
			time.SetInput(this->m_units.time.input);
		}
		else
		{
			time.SetInput(this->m_units.time.si);
		}

		for (int row = 1; row < this->m_wndScheduleGrid.GetRowCount(); ++row)
		{
			int solution;
			double value;
			CRiverState state;

			// start time
			strValue = this->m_wndScheduleGrid.GetItemText(row, 0);
			strValue.Trim();
			if (!strValue.IsEmpty())
			{
				DDX_TextGridControl(pDX, IDC_GRID_SCHEDULES, row, 0, value);
				if (value < 0.0)
				{
					::DDX_GridControlFail(pDX, IDC_GRID_SCHEDULES, row, 0, _T("Start time must be positive."));
				}
				time.SetValue(value);

				// time units
				DDX_TextGridControl(pDX, IDC_GRID_SCHEDULES, row, 1, strValue);
				if (strValue.IsEmpty() || time.SetInput(strValue) != OK)
				{
					if (row != 1)
					{
						::DDX_GridControlFail(pDX, IDC_GRID_SCHEDULES, row, 1, _T("Please enter the start time units."));
					}
				}

				// head
				strValue = this->m_wndScheduleGrid.GetItemText(row, 2);
				strValue.Trim();
				if (!strValue.IsEmpty())
				{
					DDX_TextGridControl(pDX, IDC_GRID_SCHEDULES, row, 2, value);
					state.SetHead(value);
				}

				// solution
				strValue = this->m_wndScheduleGrid.GetItemText(row, 3);
				strValue.Trim();
				if (!strValue.IsEmpty())
				{
					DDX_TextGridControl(pDX, IDC_GRID_SCHEDULES, row, 3, solution);
					state.SetSolution(solution);
				}
				this->m_riverPt.Insert(time, state);
			}
		}
	}
	else
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT;		

		// CRiverStates
		//
		const CTimeSeries<CRiverState>& map = this->m_riverPt.GetRiverSched();
		CTimeSeries<CRiverState>::const_iterator iter = map.begin();
		for (int i = 0; iter != map.end(); ++iter, ++i)
		{
			Item.row = i + 1;

			// start time
			Item.col = 0;
			Item.strText.Format(_T("%g"), (*iter).first.value);
			this->m_wndScheduleGrid.SetItem(&Item);

			// start time units
			if ((*iter).first.type == TT_UNDEFINED)
			{
				ASSERT(this->m_units.time.input && ::strlen(this->m_units.time.input));
				Item.strText = CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str();
			}
			else if ((*iter).first.type == TT_STEP)
			{
				ASSERT(FALSE);
			}
			else if ((*iter).first.type == TT_UNITS)
			{
				ASSERT((*iter).first.input && ::strlen((*iter).first.input));
				if ((*iter).first.input)
				{
					Item.strText = CGlobal::GetStdTimeUnits((*iter).first.input).c_str();
				}
				else
				{
					ASSERT(this->m_units.time.input && ::strlen(this->m_units.time.input));
					Item.strText = CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str();
				}
			}
			Item.col = 1;
			this->m_wndScheduleGrid.SetItem(&Item);

			// head
			if ((*iter).second.head_defined)
			{
				Item.col = 2;
				Item.strText.Format(_T("%g"), (*iter).second.head);
				this->m_wndScheduleGrid.SetItem(&Item);
			}
			// solution
			if ((*iter).second.solution_defined)
			{
				Item.col = 3;
				Item.strText.Format(_T("%d"), (*iter).second.solution);
				this->m_wndScheduleGrid.SetItem(&Item);
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CRiverPointPropertyPage, baseCRiverPointPropertyPage)
END_MESSAGE_MAP()


// CRiverPointPropertyPage message handlers

void CRiverPointPropertyPage::SetUnits(const CUnits& units)
{
	this->m_units = units;

	this->m_strHorizontalUnits.Format("(%s)", this->m_units.horizontal.defined ? this->m_units.horizontal.input : this->m_units.horizontal.si);
	CGlobal::MinimizeLengthUnits(this->m_strHorizontalUnits);

	this->m_strVerticalUnits.Format("(%s)", this->m_units.vertical.defined ? this->m_units.vertical.input : this->m_units.vertical.si);
	CGlobal::MinimizeLengthUnits(this->m_strVerticalUnits);

	this->m_strHeadUnits.Format("(%s)", this->m_units.head.defined ? this->m_units.head.input : this->m_units.head.si);
	CGlobal::MinimizeLengthUnits(this->m_strHeadUnits);

	this->m_strRiverBedKUnits.Format("(%s)", this->m_units.river_bed_k.defined ? this->m_units.river_bed_k.input : this->m_units.river_bed_k.si);
	CGlobal::MinimizeLengthUnits(this->m_strRiverBedKUnits);
	CGlobal::MinimizeTimeUnits(this->m_strRiverBedKUnits);	

	this->m_strRiverBedThicknessUnits.Format("(%s)", this->m_units.river_bed_thickness.defined ? this->m_units.river_bed_thickness.input : this->m_units.river_bed_thickness.si);
	CGlobal::MinimizeLengthUnits(this->m_strRiverBedThicknessUnits);
}

void CRiverPointPropertyPage::GetProperties(CRiverPoint& riverPt)const
{
	riverPt = this->m_riverPt;
}

void CRiverPointPropertyPage::SetProperties(const CRiverPoint& riverPt)
{
	this->m_riverPt = riverPt;
}
