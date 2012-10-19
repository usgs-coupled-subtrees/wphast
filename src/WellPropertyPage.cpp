// WellPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "WellPropertyPage.h"

#include "Global.h"

#define strText szText

// CWellPropertyPage dialog

IMPLEMENT_DYNAMIC(CWellPropertyPage, CPropertyPage)

CWellPropertyPage::CWellPropertyPage()
	: CPropertyPage(CWellPropertyPage::IDD)
	, m_bByDepth(FALSE)
	, m_bConvertBetweenDepthsAndElevation(FALSE)
	, m_allocate(FALSE)
	, m_bFlowOnly(true)
	, m_bUpdatePositionOnly(FALSE)
	, m_bUpdateZOnly(FALSE)
{
	this->m_well.n_user = 1;
	this->m_gridKeyword.m_grid[2].SetUniformRange(0.0, 1.0, 2);
	this->m_gridKeyword.m_grid[2].Setup();

	// RTF -- Change font to 8 and single-spaced
	// Open user.transport.format.final.rtf
	// Ctrl+A -> Right-Click -> Font... -> Size = 8
	// Ctrl+A -> Right-Click -> Paragraph... -> Line Spacing = Single
	CGlobal::LoadRTFString(this->m_sWellTimeRTF,      IDR_WELL_TIME_RTF);
	CGlobal::LoadRTFString(this->m_sWellUnitsRTF,     IDR_WELL_UNITS_RTF);
	CGlobal::LoadRTFString(this->m_sWellRateRTF,      IDR_WELL_RATE_RTF);
	CGlobal::LoadRTFString(this->m_sWellSolutionRTF,  IDR_WELL_SOLUTION_RTF);
	CGlobal::LoadRTFString(this->m_sWellAllocateRTF,  IDR_WELL_ALLOCATE_RTF);
	CGlobal::LoadRTFString(this->m_sWellDepthRTF,     IDR_WELL_DEPTH_RTF);
	CGlobal::LoadRTFString(this->m_sWellDescRTF,      IDR_WELL_DESC_RTF);
	CGlobal::LoadRTFString(this->m_sWellDiamRTF,      IDR_WELL_DIAM_RTF);
	CGlobal::LoadRTFString(this->m_sWellElevationRTF, IDR_WELL_ELEVATION_RTF);
	CGlobal::LoadRTFString(this->m_sWellLocationRTF,  IDR_WELL_LOCATION_RTF);
	CGlobal::LoadRTFString(this->m_sWellLSDRTF,       IDR_WELL_LSD_RTF);
	CGlobal::LoadRTFString(this->m_sWellNumberRTF,    IDR_WELL_NUMBER_RTF);
	CGlobal::LoadRTFString(this->m_sWellRadiusRTF,    IDR_WELL_RADIUS_RTF);
	CGlobal::LoadRTFString(this->m_sWellUseMapXYRTF,  IDR_WELL_USE_MAP_XY_RTF);
	CGlobal::LoadRTFString(this->m_sWellUseMapZRTF,   IDR_WELL_USE_MAP_Z_RTF);
}

CWellPropertyPage::~CWellPropertyPage()
{
}

void CWellPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	if (!pDX->m_bSaveAndValidate && this->m_bUpdatePositionOnly)
	{
		CWellSchedule well;
		well = this->m_well;

		// well.x_user
		//
		DDX_Text(pDX, IDC_X_EDIT, well.x_user);

		// well.y_user
		//
		DDX_Text(pDX, IDC_Y_EDIT, well.y_user);

		// grid/map coordinates
		//
		int state = BST_UNCHECKED;
		switch (well.xy_coordinate_system_user)
		{
		case PHAST_Transform::MAP:
			DDX_Text(pDX, IDC_X_UNITS_STATIC, this->m_strMapHorizontalUnits);
			DDX_Text(pDX, IDC_Y_UNITS_STATIC, this->m_strMapHorizontalUnits);
			state = BST_CHECKED;
			break;
		case PHAST_Transform::GRID:
			DDX_Text(pDX, IDC_X_UNITS_STATIC, this->m_strHorizontalUnits);
			DDX_Text(pDX, IDC_Y_UNITS_STATIC, this->m_strHorizontalUnits);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		DDX_Check(pDX, IDC_CHECK_USE_MAP, state);
		return;
	}

	if (!pDX->m_bSaveAndValidate && this->m_bUpdateZOnly)
	{
		CWellSchedule well;
		well = this->m_well;

		// grid/map coordinates
		//
		int state_z = BST_UNCHECKED;
		switch (well.z_coordinate_system_user)
		{
		case PHAST_Transform::MAP:
			DDX_Text(pDX, IDC_LSD_UNITS_STATIC, this->m_strMapHorizontalUnits);
			state_z = BST_CHECKED;
			break;
		case PHAST_Transform::GRID:
			DDX_Text(pDX, IDC_LSD_UNITS_STATIC, this->m_strHorizontalUnits);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		DDX_Check(pDX, IDC_CHECK_USE_MAP_Z, state_z);

		// screen headers
		this->SetScreenHeadings(this->m_bByDepth);

		// lsd
		DDX_Text(pDX, IDC_LSD_EDIT, well.lsd_user);
		return;
	}

	DDX_GridControl(pDX, IDC_GRID_SCREENS, m_wndScreensGrid);
	DDX_GridControl(pDX, IDC_GRID_SCHEDULES, m_wndPumpSchedGrid);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	if (this->m_bFirstSetActive)
	{
		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sWellNumberRTF.c_str());
	}

	CWellSchedule well;
	if (!pDX->m_bSaveAndValidate)
	{
		well = this->m_well;
	}

	// Prepare Screens Grid
	//
	if (!pDX->m_bSaveAndValidate && this->m_wndScreensGrid.GetColumnCount() == 0)
	{
		const int MIN_ROWS = 10;
		TRY
		{
			this->m_wndScreensGrid.SetRowCount(MIN_ROWS + this->m_well.count_depth_user + this->m_well.count_elevation_user);
			this->m_wndScreensGrid.SetColumnCount(2);
			this->m_wndScreensGrid.SetFixedRowCount(1);
			this->m_wndScreensGrid.SetFixedColumnCount(0);
			this->m_wndScreensGrid.EnableTitleTips(FALSE);
			this->m_wndScreensGrid.SetCurrentFocusCell(1, 0);
			// this->m_wndScreensGrid.EnableSelection(FALSE); // this breaks CutSelectedText
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

		// set default format
		for (int row = 0; row < this->m_wndScreensGrid.GetRowCount(); ++row)
		{
			for (int col = 0; col < this->m_wndScreensGrid.GetColumnCount(); ++col)
			{ 
				this->m_wndScreensGrid.SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
			}
		}
		this->SetScreenHeadings(this->m_bByDepth);
		this->m_wndScreensGrid.ExpandColumnsToFit();
	}

	// Prepare Pump Sched. Grid
	//
	if (!pDX->m_bSaveAndValidate && this->m_wndPumpSchedGrid.GetColumnCount() == 0)
	{
		const int MIN_ROWS = 10;
		TRY
		{
			this->m_wndPumpSchedGrid.SetRowCount(MIN_ROWS + (int)this->m_well.GetPumpSched().size());
			this->m_wndPumpSchedGrid.SetColumnCount(4);
			this->m_wndPumpSchedGrid.SetFixedRowCount(1);
			this->m_wndPumpSchedGrid.SetFixedColumnCount(0);
			this->m_wndPumpSchedGrid.EnableTitleTips(FALSE);
			// this->m_wndPumpSchedGrid.EnableSelection(FALSE); // this breaks CutSelectedText
			this->m_wndPumpSchedGrid.SetCurrentFocusCell(1, 2);

			std::vector<CString> options;
			options.push_back(_T("seconds"));
			options.push_back(_T("minutes"));
			options.push_back(_T("hours"));
			options.push_back(_T("days"));
			options.push_back(_T("years"));

			this->m_wndPumpSchedGrid.SetColumnOptions(1, options);

		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

		// set default format
		for (int row = 0; row < this->m_wndPumpSchedGrid.GetRowCount(); ++row)
		{
			for (int col = 0; col < this->m_wndPumpSchedGrid.GetColumnCount(); ++col)
			{ 
				this->m_wndPumpSchedGrid.SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
			}
		}
		this->SetPumpSchedHeadings();
		this->m_wndPumpSchedGrid.ExpandColumnsToFit();
	}

	// well.n_user
	//
	DDX_Text(pDX, IDC_NUSER_EDIT, well.n_user);
	if (pDX->m_bSaveAndValidate)
	{
		std::set<int>::iterator iter = this->m_usedWellNumbers.find(well.n_user);
		if (iter != this->m_usedWellNumbers.end())
		{
			::AfxMessageBox("This well number is already in use. Please choose a different number.");
			pDX->Fail();        // throws exception
		}
	}

	// well.description
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString description;
		DDX_Text(pDX, IDC_DESC_EDIT, description);
		if (description.IsEmpty())
		{
			delete[] well.description;
			well.description = NULL;
		}
		else
		{
			well.description = new char[::strlen(description) + 1];
			::strcpy(well.description, description);
		}
	}
	else 
	{
		CString description(well.description);
		DDX_Text(pDX, IDC_DESC_EDIT, description);
	}

	// well.x_user
	//
	DDX_Text(pDX, IDC_X_EDIT, well.x_user);
	well.x_user_defined = TRUE;

	// well.y_user
	//
	DDX_Text(pDX, IDC_Y_EDIT, well.y_user);
	well.y_user_defined = TRUE;

	// xy grid/map coordinates
	//
	int state = BST_UNCHECKED;
	switch (well.xy_coordinate_system_user)
	{
	case PHAST_Transform::MAP:
		state = BST_CHECKED;
		DDX_Text(pDX, IDC_X_UNITS_STATIC, this->m_strMapHorizontalUnits);
		DDX_Text(pDX, IDC_Y_UNITS_STATIC, this->m_strMapHorizontalUnits);
		break;
	case PHAST_Transform::GRID:
		DDX_Text(pDX, IDC_X_UNITS_STATIC, this->m_strHorizontalUnits);
		DDX_Text(pDX, IDC_Y_UNITS_STATIC, this->m_strHorizontalUnits);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	DDX_Check(pDX, IDC_CHECK_USE_MAP, state);
	if (pDX->m_bSaveAndValidate)
	{
		switch (state)
		{
		case BST_CHECKED:
			well.xy_coordinate_system_user = PHAST_Transform::MAP;
			break;
		default:
			well.xy_coordinate_system_user = PHAST_Transform::GRID;
			break;
		}
	}

	// z grid/map coordinates
	//
	int state_z = BST_UNCHECKED;
	switch (well.z_coordinate_system_user)
	{
	case PHAST_Transform::MAP:
		state_z = BST_CHECKED;
		break;
	case PHAST_Transform::GRID:
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	DDX_Check(pDX, IDC_CHECK_USE_MAP_Z, state_z);
	if (pDX->m_bSaveAndValidate)
	{
		switch (state_z)
		{
		case BST_CHECKED:
			well.z_coordinate_system_user = PHAST_Transform::MAP;
			break;
		default:
			well.z_coordinate_system_user = PHAST_Transform::GRID;
			break;
		}
	}

	// By depth or elevation
	//
	if (pDX->m_bSaveAndValidate)
	{
		if (this->GetCheckedRadioButton(IDC_ELEVATION_RADIO, IDC_DEPTH_RADIO) == IDC_ELEVATION_RADIO)
		{
			this->m_bByDepth = FALSE;
		}
		else
		{
			this->m_bByDepth = TRUE;
		}
	}
	else
	{
		if (this->m_bByDepth)
		{
			this->CheckRadioButton(IDC_ELEVATION_RADIO, IDC_DEPTH_RADIO, IDC_DEPTH_RADIO);
		}
		else
		{
			this->CheckRadioButton(IDC_ELEVATION_RADIO, IDC_DEPTH_RADIO, IDC_ELEVATION_RADIO);
		}
		this->EnableByDepth(this->m_bByDepth);
	}

	// lsd_user
	if (pDX->m_bSaveAndValidate)
	{
		if (this->m_bByDepth)
		{
			DDX_Text(pDX, IDC_LSD_EDIT, well.lsd_user);
			well.lsd_user_defined = TRUE;
		}
		else
		{
			well.lsd_user_defined = FALSE;
		}
	}
	else
	{
		if (well.lsd_user_defined)
		{
			this->m_wndWellCtrl.SetLSD(well.lsd_user);
			DDX_Text(pDX, IDC_LSD_EDIT, well.lsd_user);
		}
		else
		{
			ASSERT(!this->m_bByDepth);
			this->m_gridKeyword.m_grid[2].Setup();
			this->m_well.lsd_user = this->m_gridKeyword.m_grid[2].coord[this->m_gridKeyword.m_grid[2].count_coord - 1];
			this->m_wndWellCtrl.SetLSD(this->m_well.lsd_user);
			DDX_Text(pDX, IDC_LSD_EDIT, this->m_well.lsd_user);
		}
	}


	// Diameter or Radius
	//
	if (pDX->m_bSaveAndValidate)
	{
		if (this->IsDlgButtonChecked(IDC_DIAM_RADIO))
		{
			DDX_Text(pDX, IDC_DIAM_EDIT, well.diameter);
			well.diameter_defined = TRUE;
			well.radius_defined = FALSE;
		}
		if (this->IsDlgButtonChecked(IDC_RADIUS_RADIO))
		{
			DDX_Text(pDX, IDC_DIAM_EDIT, well.radius);
			well.diameter_defined = FALSE;
			well.radius_defined = TRUE;
		}
	}
	else 
	{
		if (this->m_well.diameter_defined)
		{
			DDX_Text(pDX, IDC_DIAM_EDIT, well.diameter);
			this->CheckRadioButton(IDC_DIAM_RADIO, IDC_RADIUS_RADIO, IDC_DIAM_RADIO);
		}
		else if (this->m_well.radius_defined)
		{
			DDX_Text(pDX, IDC_DIAM_EDIT, well.radius);
			this->CheckRadioButton(IDC_DIAM_RADIO, IDC_RADIUS_RADIO, IDC_RADIUS_RADIO);
		}
		else
		{
			this->CheckRadioButton(IDC_DIAM_RADIO, IDC_RADIUS_RADIO, IDC_DIAM_RADIO);
		}
	}

	// Setup well control
	//
	DDX_Control(pDX, IDC_WELL_STATIC, this->m_wndWellCtrl);
	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
		this->m_wndWellCtrl.SetGrid(this->m_gridKeyword.m_grid[2]);		
		this->m_wndWellCtrl.SetByDepth(this->m_bByDepth);
	}


	// Screens
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString strValue;
		Well_Interval interval;
		std::list<Well_Interval> listIntervals;
		for (int row = 1; row < this->m_wndScreensGrid.GetRowCount(); ++row)
		{
			bool valid_bottom = false;
			bool valid_top = false;

			strValue = this->m_wndScreensGrid.GetItemText(row, 0);
			if (!strValue.IsEmpty() && CGlobal::SimpleFloatParse(strValue, interval.bottom))
			{
				valid_bottom = true;
			}
			strValue = this->m_wndScreensGrid.GetItemText(row, 1);
			if (!strValue.IsEmpty() && CGlobal::SimpleFloatParse(strValue, interval.top))
			{
				valid_top = true;
			}

			if (valid_bottom && !valid_top)
			{
				DDX_TextGridControl(pDX, IDC_GRID_SCREENS, row, 1, interval.top);
			}
			if (!valid_bottom && valid_top)
			{
				DDX_TextGridControl(pDX, IDC_GRID_SCREENS, row, 0, interval.bottom);
			}
			if (valid_bottom && valid_top)
			{
				listIntervals.push_back(interval);
			}
		}
		if (listIntervals.empty())
		{
			well.count_depth_user = well.count_elevation_user = 0;
			CString string("At least one screened interval must be defined.");
			pDX->PrepareCtrl(IDC_GRID_SCREENS);
			::DDX_GridControlFail(pDX, IDC_GRID_SCREENS, 1, 0, string);
		}
		else
		{
			if (this->m_bByDepth)
			{
				well.count_depth_user = (int)listIntervals.size();
				well.count_elevation_user = 0;
				delete[] well.depth_user;
				well.depth_user = new Well_Interval[well.count_depth_user + 1];
				std::list<Well_Interval>::iterator it = listIntervals.begin();
				for (int i = 0; it != listIntervals.end(); ++it, ++i)
				{
					well.depth_user[i] = (*it);
				}
			}
			else
			{
				well.count_depth_user = 0;
				well.count_elevation_user = (int)listIntervals.size();
				delete[] well.elevation_user;
				well.elevation_user = new Well_Interval[well.count_elevation_user + 1];
				std::list<Well_Interval>::iterator it = listIntervals.begin();
				for (int i = 0; it != listIntervals.end(); ++it, ++i)
				{
					well.elevation_user[i] = (*it);
				}
			}
		}
	}
	else
	{
		// this->m_well should have been Reduce()d
		ASSERT(this->m_well.count_depth_user * this->m_well.count_elevation_user == 0);

		this->m_wndWellCtrl.RemoveAllScreens();

		GV_ITEM Item;
		Item.mask = GVIF_TEXT;

		// elevations
		//
		for (int i = 0; i < this->m_well.count_elevation_user; ++i)
		{
			// set grid bottom
			Item.row = i + 1;
			Item.col = 0;
			Item.strText.Format(_T("%g"), this->m_well.elevation_user[i].bottom);
			this->m_wndScreensGrid.SetItem(&Item);

			// set grid top
			Item.col = 1;
			Item.strText.Format(_T("%g"), this->m_well.elevation_user[i].top);
			this->m_wndScreensGrid.SetItem(&Item);

			// add screen
			this->m_wndWellCtrl.AddScreen(this->m_well.elevation_user[i].bottom, this->m_well.elevation_user[i].top);
		}

		// depths
		//
		for (int i = 0; i < this->m_well.count_depth_user; ++i)
		{
			// set grid bottom
			Item.row = i + 1;
			Item.col = 0;
			Item.strText.Format(_T("%g"), this->m_well.depth_user[i].bottom);
			this->m_wndScreensGrid.SetItem(&Item);

			// set grid top
			Item.col = 1;
			Item.strText.Format(_T("%g"), this->m_well.depth_user[i].top);
			this->m_wndScreensGrid.SetItem(&Item);

			// add screen
			this->m_wndWellCtrl.AddScreen(this->m_well.lsd_user - this->m_well.depth_user[i].bottom, this->m_well.lsd_user - this->m_well.depth_user[i].top);
		}

	}

	// pumping-rates
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
		for (int row = 1; row < this->m_wndPumpSchedGrid.GetRowCount(); ++row)
		{
			int solution;
			double value;
			CWellRate rate;

			// start time
			strValue = this->m_wndPumpSchedGrid.GetItemText(row, 0);
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

				// rate
				strValue = this->m_wndPumpSchedGrid.GetItemText(row, 2);
				strValue.Trim();
				if (!strValue.IsEmpty())
				{
					DDX_TextGridControl(pDX, IDC_GRID_SCHEDULES, row, 2, value);
					rate.SetRate(value);
				}

				// solution
				strValue = this->m_wndPumpSchedGrid.GetItemText(row, 3);
				strValue.Trim();
				if (!strValue.IsEmpty())
				{
					DDX_TextGridControl(pDX, IDC_GRID_SCHEDULES, row, 3, solution);
					rate.SetSolution(solution);
				}

				// time at zero
				// solution @ t=0.0
				if (time.value == 0.0)
				{
					ASSERT(row == 1);

					// rate
					if (!rate.q_defined)
					{
						CString string("A rate must be defined for time zero.");
						::DDX_GridControlFail(pDX, IDC_GRID_SCHEDULES, row, 2, string);
					}
					// solution
					if (!rate.solution_defined && !this->m_bFlowOnly)
					{
						ASSERT(row == 1);
						if (rate.q > 0.0)
						{							
							CString string("Injection wells require a solution definition for time zero.");
							::DDX_GridControlFail(pDX, IDC_GRID_SCHEDULES, row, 3, string);
						}
					}
				}
				well.Insert(time, rate);
			}
		}
	}
	else
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT;		

		// CWellRates
		//
		const CTimeSeries<CWellRate>& map = this->m_well.GetPumpSched();
		CTimeSeries<CWellRate>::const_iterator iter = map.begin();
		for (int i = 0; iter != map.end(); ++iter, ++i)
		{
			Item.row = i + 1;

			// start time
			Item.col = 0;
			Item.strText.Format(_T("%g"), (*iter).first.value);
			this->m_wndPumpSchedGrid.SetItem(&Item);

			// start time units
			if ((*iter).first.type == TT_UNDEFINED)
			{
				ASSERT(this->m_units.time.input && ::strlen(this->m_units.time.input));
				Item.strText = CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str();
			}
			else if ((*iter).first.type == TT_STEP)
			{
				ASSERT(FALSE);
				Item.strText = CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str();
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
			this->m_wndPumpSchedGrid.SetItem(&Item);

			// rate
			if ((*iter).second.q_defined)
			{
				Item.col = 2;
				Item.strText.Format(_T("%g"), (*iter).second.q);
				this->m_wndPumpSchedGrid.SetItem(&Item);
			}

			// solution
			if ((*iter).second.solution_defined)
			{
				Item.col = 3;
				Item.strText.Format(_T("%d"), (*iter).second.solution);
				this->m_wndPumpSchedGrid.SetItem(&Item);
			}
		}
	}

	// Line 10: -allocate_by_head_and_mobility [True or False]
	DDX_Check(pDX, IDC_ALLOCATE_BY_HEAD, well.mobility_and_pressure);

	if (pDX->m_bSaveAndValidate)
	{
		// If here ok to copy
		//
		this->SetProperties(well);
	}
}


BEGIN_MESSAGE_MAP(CWellPropertyPage, CPropertyPage)
	ON_BN_CLICKED(IDC_ELEVATION_RADIO, OnBnClickedElevationRadio)
	ON_BN_CLICKED(IDC_DEPTH_RADIO, OnBnClickedDepthRadio)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_SCREENS, OnEndLabelEditScreen)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_SCREENS, OnSelChangedScreen)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_SCHEDULES, OnSelChangedSchedules)
	ON_EN_CHANGE(IDC_LSD_EDIT, OnEnChangeLsdEdit)
	ON_NOTIFY(GVN_SETFOCUS, IDC_GRID_SCREENS, OnSelChangedScreen)
	ON_NOTIFY(GVN_SETFOCUS, IDC_GRID_SCHEDULES, OnSelChangedSchedules)
	ON_EN_SETFOCUS(IDC_NUSER_EDIT, OnEnSetfocusNuserEdit)
	ON_EN_SETFOCUS(IDC_DESC_EDIT, OnEnSetfocusDescEdit)
	ON_EN_SETFOCUS(IDC_X_EDIT, OnEnSetfocusXEdit)
	ON_EN_SETFOCUS(IDC_Y_EDIT, OnEnSetfocusYEdit)
	ON_BN_CLICKED(IDC_DIAM_RADIO, OnBnClickedDiamRadio)
	ON_BN_CLICKED(IDC_RADIUS_RADIO, OnBnClickedRadiusRadio)
	ON_EN_SETFOCUS(IDC_DIAM_EDIT, OnEnSetfocusDiamEdit)
	ON_BN_SETFOCUS(IDC_ALLOCATE_BY_HEAD, OnBnSetfocusAllocateByHead)
	ON_EN_SETFOCUS(IDC_LSD_EDIT, OnEnSetfocusLsdEdit)
	ON_BN_SETFOCUS(IDC_ELEVATION_RADIO, OnBnSetfocusElevationRadio)
	ON_BN_SETFOCUS(IDC_DEPTH_RADIO, OnBnSetfocusDepthRadio)
	ON_BN_SETFOCUS(IDC_DIAM_RADIO, OnBnSetfocusDiamRadio)
	ON_BN_SETFOCUS(IDC_RADIUS_RADIO, OnBnSetfocusRadiusRadio)
	ON_BN_CLICKED(IDC_CHECK_USE_MAP, OnBnClickedUseMap)
	ON_BN_CLICKED(IDC_CHECK_USE_MAP_Z, OnBnClickedUseMapZ)
	ON_BN_SETFOCUS(IDC_CHECK_USE_MAP, &CWellPropertyPage::OnBnSetfocusCheckUseMap)
	ON_BN_SETFOCUS(IDC_CHECK_USE_MAP_Z, &CWellPropertyPage::OnBnSetfocusCheckUseMapZ)
END_MESSAGE_MAP()


// CWellPropertyPage message handlers

void CWellPropertyPage::SetUnits(const CUnits& units)
{
	this->m_units = units;
}

void CWellPropertyPage::GetProperties(CWellSchedule& well)const
{
	well = this->m_well;
}

void CWellPropertyPage::SetProperties(const CWellSchedule& well)
{
	this->m_well = well;
	this->m_bByDepth = FALSE;
	if (this->m_well.count_depth_user > 0 && this->m_well.lsd_user_defined == TRUE)
	{
		this->m_bByDepth = TRUE;
	}
	this->m_well.Reduce();
}

void CWellPropertyPage::SetScreenHeadings(BOOL bByDepth)
{
	GV_ITEM Item;
	Item.mask = GVIF_TEXT;
	Item.row = 0;


	CString units = this->m_strVerticalUnits;
	if (this->m_well.z_coordinate_system_user == PHAST_Transform::MAP)
	{
		units = this->m_strMapVerticalUnits;
	}

	if (bByDepth)
	{
		Item.col = 0;
		Item.strText.Format(_T("Bottom depth %s"), units);
		this->m_wndScreensGrid.SetItem(&Item);

		Item.col = 1;
		Item.strText.Format(_T("Top depth %s"), units);
		this->m_wndScreensGrid.SetItem(&Item);
	}
	else
	{
		Item.col = 0;
		Item.strText.Format(_T("Bottom elevation %s"), units);
		this->m_wndScreensGrid.SetItem(&Item);

		Item.col = 1;
		Item.strText.Format(_T("Top elevation %s"), units);
		this->m_wndScreensGrid.SetItem(&Item);
	}
	this->m_wndScreensGrid.RedrawWindow();
}

// COMMENT: {8/9/2004 6:08:14 PM}void CWellPropertyPage::OnBnClickedByDepthCheck()
// COMMENT: {8/9/2004 6:08:14 PM}{
// COMMENT: {8/9/2004 6:08:14 PM}	// Add your control notification handler code here
// COMMENT: {8/9/2004 6:08:14 PM}	BOOL bEnable = (this->IsDlgButtonChecked(IDC_DEPTH_RADIO) == BST_CHECKED);
// COMMENT: {8/9/2004 6:08:14 PM}	this->GetDlgItem(IDC_LSD_STATIC)->EnableWindow(bEnable);
// COMMENT: {8/9/2004 6:08:14 PM}	this->GetDlgItem(IDC_LSD_EDIT)->EnableWindow(bEnable);
// COMMENT: {8/9/2004 6:08:14 PM}	this->GetDlgItem(IDC_LSD_UNITS_STATIC)->EnableWindow(bEnable);
// COMMENT: {8/9/2004 6:08:14 PM}	this->SetScreenHeadings(bEnable);
// COMMENT: {8/9/2004 6:08:14 PM}}

BOOL CWellPropertyPage::OnInitDialog()
{
	// Get units
	//
	this->FillUnits();

	CPropertyPage::OnInitDialog();

	// Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWellPropertyPage::SetPumpSchedHeadings(void)
{
	CString str;
	str.Format(_T("Rate %s"), this->m_strWellPumpageUnits);

	VERIFY(this->m_wndPumpSchedGrid.SetItemText(0, 0, _T("Start time")));
	VERIFY(this->m_wndPumpSchedGrid.SetItemText(0, 1, _T("Units")));
	VERIFY(this->m_wndPumpSchedGrid.SetItemText(0, 2, str));
	VERIFY(this->m_wndPumpSchedGrid.SetItemText(0, 3, _T("Solution")));

	VERIFY(this->m_wndPumpSchedGrid.SetItemText(1, 0, _T("0.0")));

	VERIFY(this->m_wndPumpSchedGrid.DisableCell(1, 0));
	VERIFY(this->m_wndPumpSchedGrid.DisableCell(1, 1));

	this->m_wndPumpSchedGrid.RedrawWindow();
}

void CWellPropertyPage::FillUnits(void)
{
	if (this->m_strHorizontalUnits.IsEmpty())
	{
		if (this->m_units.horizontal.defined)
		{
			this->m_strHorizontalUnits.Format("(%s)", this->m_units.horizontal.input);
			CGlobal::MinimizeLengthUnits(this->m_strHorizontalUnits);
		}
		else
		{
			this->m_strHorizontalUnits.Format("(%s)", this->m_units.horizontal.si);
		}
		this->GetDlgItem(IDC_X_UNITS_STATIC)->SetWindowText(this->m_strHorizontalUnits);
		this->GetDlgItem(IDC_Y_UNITS_STATIC)->SetWindowText(this->m_strHorizontalUnits);
	}

	if (this->m_strVerticalUnits.IsEmpty())
	{
		if (this->m_units.vertical.defined)
		{
			this->m_strVerticalUnits.Format("(%s)", this->m_units.vertical.input);
			CGlobal::MinimizeLengthUnits(this->m_strVerticalUnits);
		}
		else
		{
			this->m_strVerticalUnits.Format("(%s)", this->m_units.vertical.si);
		}
		this->GetDlgItem(IDC_LSD_UNITS_STATIC)->SetWindowText(this->m_strVerticalUnits);
	}

	if (this->m_strMapHorizontalUnits.IsEmpty())
	{
		if (this->m_units.map_horizontal.defined)
		{
			this->m_strMapHorizontalUnits.Format("(%s)", this->m_units.map_horizontal.input);
			CGlobal::MinimizeLengthUnits(this->m_strMapHorizontalUnits);
		}
		else
		{
			this->m_strMapHorizontalUnits.Format("(%s)", this->m_units.map_horizontal.si);
		}
	}

	if (this->m_strMapVerticalUnits.IsEmpty())
	{
		if (this->m_units.map_vertical.defined)
		{
			this->m_strMapVerticalUnits.Format("(%s)", this->m_units.map_vertical.input);
			CGlobal::MinimizeLengthUnits(this->m_strMapVerticalUnits);
		}
		else
		{
			this->m_strMapVerticalUnits.Format("(%s)", this->m_units.map_vertical.si);
		}
	}

	if (this->m_strWellDiameterUnits.IsEmpty())
	{
		if (this->m_units.well_diameter.defined)
		{
			this->m_strWellDiameterUnits.Format("(%s)", this->m_units.well_diameter.input);
			CGlobal::MinimizeLengthUnits(this->m_strWellDiameterUnits);
		}
		else
		{
			this->m_strWellDiameterUnits.Format("(%s)", this->m_units.well_diameter.si);
		}
		this->GetDlgItem(IDC_DIAMETER_UNITS_STATIC)->SetWindowText(this->m_strWellDiameterUnits);
	}	

	if (this->m_strTimeUnits.IsEmpty())
	{
		if (this->m_units.time.defined)
		{
			this->m_strTimeUnits.Format("(%s)", this->m_units.time.input);
		}
		else
		{
			this->m_strTimeUnits.Format("(%s)", this->m_units.time.si);
		}
	}	
	if (this->m_strWellPumpageUnits.IsEmpty())
	{
		if (this->m_units.well_pumpage.defined)
		{
			this->m_strWellPumpageUnits.Format("(%s)", this->m_units.well_pumpage.input);
		}
		else
		{
			this->m_strWellPumpageUnits.Format("(%s)", this->m_units.well_pumpage.si);
		}
	}	
}

void CWellPropertyPage::EnableByDepth(BOOL bByDepth)
{
	CWnd *pWnd;
	if (pWnd = this->GetDlgItem(IDC_LSD_STATIC))
	{
		pWnd->EnableWindow(bByDepth);
	}
	if (pWnd = this->GetDlgItem(IDC_LSD_EDIT))
	{
		pWnd->EnableWindow(bByDepth);
	}
	if (pWnd = this->GetDlgItem(IDC_LSD_UNITS_STATIC))
	{
		pWnd->EnableWindow(bByDepth);
	}
	this->m_wndWellCtrl.SetByDepth(bByDepth);

	if (this->m_bConvertBetweenDepthsAndElevation)
	{
		if (CWnd *pWnd = this->GetDlgItem(IDC_LSD_EDIT))
		{
			CString strLSD;
			pWnd->GetWindowText(strLSD);
			double dLSD;
			CString str;
			if (CGlobal::SimpleFloatParse(strLSD, dLSD))
			{
				Well_Interval elevation;
				for (int row = 1; row < this->m_wndScreensGrid.GetRowCount(); ++row)
				{
					CString strBottom = this->m_wndScreensGrid.GetItemText(row, 0);
					if (!strBottom.IsEmpty() && CGlobal::SimpleFloatParse(strBottom, elevation.bottom))
					{
						elevation.bottom = dLSD - elevation.bottom;
						str.Format("%g", elevation.bottom);
						this->m_wndScreensGrid.SetItemText(row, 0, str);
					}

					CString strTop = this->m_wndScreensGrid.GetItemText(row, 1);
					if (!strTop.IsEmpty() && CGlobal::SimpleFloatParse(strTop, elevation.top))
					{
						elevation.top = dLSD - elevation.top;
						str.Format("%g", elevation.top);
						this->m_wndScreensGrid.SetItemText(row, 1, str);
					}
				}
			}
		}
	}
}

void CWellPropertyPage::OnBnClickedElevationRadio()
{
	this->m_bByDepth = (this->IsDlgButtonChecked(IDC_DEPTH_RADIO) == BST_CHECKED);
	this->UpdateScreens(this->m_bByDepth);
	this->EnableByDepth(this->m_bByDepth);
	this->SetScreenHeadings(this->m_bByDepth);
}

void CWellPropertyPage::OnBnClickedDepthRadio()
{
	this->m_bByDepth = (this->IsDlgButtonChecked(IDC_DEPTH_RADIO) == BST_CHECKED);
	this->UpdateScreens(this->m_bByDepth);
	this->EnableByDepth(this->m_bByDepth);
	this->SetScreenHeadings(this->m_bByDepth);
}

void CWellPropertyPage::SetGridKeyword(const CGridKeyword& gridKeyword)
{
	this->m_gridKeyword = gridKeyword;
	this->m_gridKeyword.m_grid[2].Setup();
}

void CWellPropertyPage::OnEndLabelEditScreen(NMHDR *pNotifyStruct, LRESULT *result)
{
	/// NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	/// CString str = this->m_wndScreensGrid.GetItemText(pnmgv->iRow, pnmgv->iColumn);
	/// TRACE("%s\n", (LPCTSTR)this->m_wndScreensGrid.GetItemText(pnmgv->iRow, pnmgv->iColumn));

	BOOL bByDepth = (this->IsDlgButtonChecked(IDC_DEPTH_RADIO) == BST_CHECKED);
	this->UpdateScreens(bByDepth);
	this->m_wndScreensGrid.SetRowCount(this->m_wndScreensGrid.GetRowCount() + 1);
}

void CWellPropertyPage::OnSelChangedScreen(NMHDR *pNotifyStruct, LRESULT *result)
{
	if (this->GetCheckedRadioButton(IDC_ELEVATION_RADIO, IDC_DEPTH_RADIO) == IDC_ELEVATION_RADIO)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sWellElevationRTF.c_str());
	}
	else
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sWellDepthRTF.c_str());
	}
}

void CWellPropertyPage::OnSelChangedSchedules(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("OnSelChangedSchedules\n");
	CCellID focus = this->m_wndPumpSchedGrid.GetFocusCell();

	if (!this->m_wndPumpSchedGrid.IsValid(focus)) return;

	switch (focus.col)
	{
	case 0:
		this->m_wndRichEditCtrl.SetWindowText(this->m_sWellTimeRTF.c_str());
		break;
	case 1:
		this->m_wndRichEditCtrl.SetWindowText(this->m_sWellUnitsRTF.c_str());
		break;
	case 2:
		this->m_wndRichEditCtrl.SetWindowText(this->m_sWellRateRTF.c_str());
		break;
	case 3:
		this->m_wndRichEditCtrl.SetWindowText(this->m_sWellSolutionRTF.c_str());
		break;
	default:
		ASSERT(FALSE);
		this->m_wndRichEditCtrl.SetWindowText(_T(""));
		break;
	}
}

void CWellPropertyPage::OnEnChangeLsdEdit()
{
	BOOL bByDepth = (this->IsDlgButtonChecked(IDC_DEPTH_RADIO) == BST_CHECKED);
	this->UpdateScreens(bByDepth);
}

void CWellPropertyPage::UpdateScreens(BOOL bByDepth)
{
	this->m_wndWellCtrl.SetByDepth(bByDepth);

	if (bByDepth)
	{
		ASSERT(this->GetDlgItem(IDC_LSD_EDIT));
		if (CWnd *pWnd = this->GetDlgItem(IDC_LSD_EDIT))
		{
			CString strLSD;
			pWnd->GetWindowText(strLSD);

			double dLSD;
			if (CGlobal::SimpleFloatParse(strLSD, dLSD))
			{
				Well_Interval elevation;
				std::list<Well_Interval> listElevations;
				for (int row = 1; row < this->m_wndScreensGrid.GetRowCount(); ++row)
				{
					bool valid_bottom = false;
					bool valid_top = false;

					CString strBottom = this->m_wndScreensGrid.GetItemText(row, 0);
					if (!strBottom.IsEmpty() && CGlobal::SimpleFloatParse(strBottom, elevation.bottom))
					{
						valid_bottom = true;
						elevation.bottom = dLSD - elevation.bottom;
					}

					CString strTop = this->m_wndScreensGrid.GetItemText(row, 1);
					if (!strTop.IsEmpty() && CGlobal::SimpleFloatParse(strTop, elevation.top))
					{
						valid_top = true;
						elevation.top = dLSD - elevation.top;
					}

					if (valid_bottom && valid_top)
					{
						listElevations.push_back(elevation);
					}
				}

				// Update Well Image
				//
				this->m_wndWellCtrl.SetLSD(dLSD);
				this->m_wndWellCtrl.RemoveAllScreens();
				std::list<Well_Interval>::iterator iter = listElevations.begin();
				for (; iter !=  listElevations.end(); ++iter)
				{
					this->m_wndWellCtrl.AddScreen((*iter).bottom, (*iter).top);
				}
			}
		}
	}
	else 
	{
		Well_Interval elevation;
		std::list<Well_Interval> listElevations;
		for (int row = 1; row < this->m_wndScreensGrid.GetRowCount(); ++row)
		{
			bool valid_bottom = false;
			bool valid_top = false;

			CString strBottom = this->m_wndScreensGrid.GetItemText(row, 0);
			if (!strBottom.IsEmpty() && CGlobal::SimpleFloatParse(strBottom, elevation.bottom))
			{
				valid_bottom = true;
			}

			CString strTop = this->m_wndScreensGrid.GetItemText(row, 1);
			if (!strTop.IsEmpty() && CGlobal::SimpleFloatParse(strTop, elevation.top))
			{
				valid_top = true;
			}

			if (valid_bottom && valid_top)
			{
				listElevations.push_back(elevation);
			}
		}

		// Update Well Image
		//
		this->m_wndWellCtrl.RemoveAllScreens();
		std::list<Well_Interval>::iterator iter = listElevations.begin();
		for (; iter !=  listElevations.end(); ++iter)
		{
			this->m_wndWellCtrl.AddScreen((*iter).bottom, (*iter).top);
		}
	}
}

void CWellPropertyPage::SetUsedWellNumbers(const std::set<int>& used)
{
	this->m_usedWellNumbers = used;
}

void CWellPropertyPage::OnEnSetfocusNuserEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellNumberRTF.c_str());
}

void CWellPropertyPage::OnEnSetfocusDescEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellDescRTF.c_str());
}

void CWellPropertyPage::OnEnSetfocusXEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellLocationRTF.c_str());
}

void CWellPropertyPage::OnEnSetfocusYEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellLocationRTF.c_str());
}

void CWellPropertyPage::OnBnClickedDiamRadio()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellDiamRTF.c_str());
}

void CWellPropertyPage::OnBnClickedRadiusRadio()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellRadiusRTF.c_str());
}

void CWellPropertyPage::OnEnSetfocusDiamEdit()
{
	if (this->GetCheckedRadioButton(IDC_DIAM_RADIO, IDC_RADIUS_RADIO) == IDC_DIAM_RADIO)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sWellDiamRTF.c_str());
	}
	else if (this->GetCheckedRadioButton(IDC_DIAM_RADIO, IDC_RADIUS_RADIO) == IDC_RADIUS_RADIO)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sWellRadiusRTF.c_str());
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CWellPropertyPage::OnBnSetfocusAllocateByHead()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellAllocateRTF.c_str());
}

void CWellPropertyPage::OnEnSetfocusLsdEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellLSDRTF.c_str());
}

void CWellPropertyPage::OnBnSetfocusElevationRadio()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellElevationRTF.c_str());
}

void CWellPropertyPage::OnBnSetfocusDepthRadio()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellDepthRTF.c_str());
}

void CWellPropertyPage::OnBnSetfocusDiamRadio()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellDiamRTF.c_str());
}

void CWellPropertyPage::OnBnSetfocusRadiusRadio()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellRadiusRTF.c_str());
}

void CWellPropertyPage::OnBnClickedUseMap()
{
	const CUnits& units = this->m_units;
	const CGridKeyword& gridKeyword = this->m_gridKeyword;
	PHAST_Transform map2grid(
		gridKeyword.m_grid_origin[0],
		gridKeyword.m_grid_origin[1],
		gridKeyword.m_grid_origin[2],
		gridKeyword.m_grid_angle,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_vertical.input_to_si/units.vertical.input_to_si
		);

	Point p(
		this->m_well.x_user,
		this->m_well.y_user,
		0.0
		);

	if (this->IsDlgButtonChecked(IDC_CHECK_USE_MAP) == BST_CHECKED)
	{
		map2grid.Inverse_transform(p);
		this->m_well.x_user = p.x();
		this->m_well.y_user = p.y();
		this->m_well.xy_coordinate_system_user = PHAST_Transform::MAP;
	}
	else
	{
		map2grid.Transform(p);
		this->m_well.x_user = p.x();
		this->m_well.y_user = p.y();
		this->m_well.xy_coordinate_system_user = PHAST_Transform::GRID;
	}
	ASSERT(this->m_bUpdatePositionOnly == FALSE);
	this->m_bUpdatePositionOnly = TRUE;
	this->UpdateData(FALSE);
	this->m_bUpdatePositionOnly = FALSE;
}

void CWellPropertyPage::OnBnClickedUseMapZ()
{
	const CUnits& units = this->m_units;
	const CGridKeyword& gridKeyword = this->m_gridKeyword;
	PHAST_Transform map2grid(
		gridKeyword.m_grid_origin[0],
		gridKeyword.m_grid_origin[1],
		gridKeyword.m_grid_origin[2],
		gridKeyword.m_grid_angle,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_horizontal.input_to_si/units.horizontal.input_to_si,
		units.map_vertical.input_to_si/units.vertical.input_to_si
		);

	Point p;

	if (this->IsDlgButtonChecked(IDC_CHECK_USE_MAP_Z) == BST_CHECKED)
	{
		// convert lsd
		//
		this->m_well.z_coordinate_system_user = PHAST_Transform::MAP;
		p.set_z(this->m_well.lsd_user);
		map2grid.Inverse_transform(p);
		this->m_well.lsd_user = p.z();

		// convert grid to map units
		//
		CGrid g(gridKeyword.m_grid[2]);
		g.Setup();
		for (int j=0; j < g.count_coord; ++j)
		{
			p.set_z(g.coord[j]);
			map2grid.Inverse_transform(p);
			g.coord[j] = p.z();
		}


		// update open intervals
		//
		Well_Interval interval;
		std::list<Well_Interval> listInterval;
		for (int row = 1; row < this->m_wndScreensGrid.GetRowCount(); ++row)
		{
			bool valid_bottom = false;
			bool valid_top = false;

			GV_ITEM Item;
			Item.mask = GVIF_TEXT;		

			CString strBottom = this->m_wndScreensGrid.GetItemText(row, 0);
			if (!strBottom.IsEmpty() && CGlobal::SimpleFloatParse(strBottom, interval.bottom))
			{
				valid_bottom = true;

				p.set_z(interval.bottom);
				map2grid.Inverse_transform(p);
				interval.bottom = p.z();

				Item.row = row;
				Item.col = 0;
				Item.strText.Format(_T("%g"), interval.bottom);
				this->m_wndScreensGrid.SetItem(&Item);

				if (this->m_bByDepth)
				{
					interval.bottom = this->m_well.lsd_user - interval.bottom;
				}
			}

			CString strTop = this->m_wndScreensGrid.GetItemText(row, 1);
			if (!strTop.IsEmpty() && CGlobal::SimpleFloatParse(strTop, interval.top))
			{
				valid_top = true;

				p.set_z(interval.top);
				map2grid.Inverse_transform(p);
				interval.top = p.z();

				Item.row = row;
				Item.col = 1;
				Item.strText.Format(_T("%g"), interval.top);
				this->m_wndScreensGrid.SetItem(&Item);

				if (this->m_bByDepth)
				{
					interval.top = this->m_well.lsd_user - interval.top;
				}
			}

			if (valid_bottom && valid_top)
			{
				listInterval.push_back(interval);
			}
		}

		// set grid ctrl
		//
		this->m_wndWellCtrl.SetGrid(g);
		this->m_wndWellCtrl.SetByDepth(this->m_bByDepth);
		this->m_wndWellCtrl.RemoveAllScreens();
		this->m_wndWellCtrl.SetLSD(this->m_well.lsd_user);
		std::list<Well_Interval>::iterator iter = listInterval.begin();
		for (; iter !=  listInterval.end(); ++iter)
		{
			this->m_wndWellCtrl.AddScreen((*iter).bottom, (*iter).top);
		}
	}
	else
	{
		// convert lsd
		//
		this->m_well.z_coordinate_system_user = PHAST_Transform::GRID;
		p.set_z(this->m_well.lsd_user);
		map2grid.Transform(p);
		this->m_well.lsd_user = p.z();

		// grid already in grid units
		//
		CGrid g(gridKeyword.m_grid[2]);
		g.Setup();

		// update open intervals
		//
		Well_Interval interval;
		std::list<Well_Interval> listInterval;
		for (int row = 1; row < this->m_wndScreensGrid.GetRowCount(); ++row)
		{
			bool valid_bottom = false;
			bool valid_top = false;

			GV_ITEM Item;
			Item.mask = GVIF_TEXT;		

			CString strBottom = this->m_wndScreensGrid.GetItemText(row, 0);
			if (!strBottom.IsEmpty() && CGlobal::SimpleFloatParse(strBottom, interval.bottom))
			{
				valid_bottom = true;

				p.set_z(interval.bottom);
				map2grid.Transform(p);
				interval.bottom = p.z();

				Item.row = row;
				Item.col = 0;
				Item.strText.Format(_T("%g"), interval.bottom);
				this->m_wndScreensGrid.SetItem(&Item);

				if (this->m_bByDepth)
				{
					interval.bottom = this->m_well.lsd_user - interval.bottom;
				}
			}

			CString strTop = this->m_wndScreensGrid.GetItemText(row, 1);
			if (!strTop.IsEmpty() && CGlobal::SimpleFloatParse(strTop, interval.top))
			{
				valid_top = true;

				p.set_z(interval.top);
				map2grid.Transform(p);
				interval.top = p.z();

				Item.row = row;
				Item.col = 1;
				Item.strText.Format(_T("%g"), interval.top);
				this->m_wndScreensGrid.SetItem(&Item);

				if (this->m_bByDepth)
				{
					interval.top = this->m_well.lsd_user - interval.top;
				}
			}

			if (valid_bottom && valid_top)
			{
				listInterval.push_back(interval);
			}
		}

		// set grid ctrl
		//
		this->m_wndWellCtrl.SetGrid(g);
		this->m_wndWellCtrl.SetByDepth(this->m_bByDepth);
		this->m_wndWellCtrl.RemoveAllScreens();
		this->m_wndWellCtrl.SetLSD(this->m_well.lsd_user);
		std::list<Well_Interval>::iterator iter = listInterval.begin();
		for (; iter !=  listInterval.end(); ++iter)
		{
			this->m_wndWellCtrl.AddScreen((*iter).bottom, (*iter).top);
		}
	}

	ASSERT(this->m_bUpdateZOnly == FALSE);
	this->m_bUpdateZOnly = TRUE;
	this->UpdateData(FALSE);
	this->m_bUpdateZOnly = FALSE;
}

void CWellPropertyPage::OnBnSetfocusCheckUseMap()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellUseMapXYRTF.c_str());
}

void CWellPropertyPage::OnBnSetfocusCheckUseMapZ()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sWellUseMapZRTF.c_str());
}
