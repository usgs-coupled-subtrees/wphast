// RiverPropertyPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "RiverPropertyPage2.h"

#include "Global.h"
#include "DelayRedraw.h"

#define strText szText

// CRiverPropertyPage2 dialog

IMPLEMENT_DYNAMIC(CRiverPropertyPage2, CPropertyPage)
CRiverPropertyPage2::CRiverPropertyPage2()
	: CPropertyPage(CRiverPropertyPage2::IDD)
	, m_bFullVerify(true)
	, m_pointIndex(0)
	, m_bFlowOnly(true)
{
	this->m_strHorizontalUnits.Format("(%s)", this->m_units.horizontal.si);
	this->m_strHeadUnits.Format("(%s)", this->m_units.head.si);
	this->m_strVerticalUnits.Format("(%s)", this->m_units.vertical.si);
	this->m_strRiverBedKUnits.Format("(%s)", this->m_units.river_bed_k.si);
	CGlobal::MinimizeTimeUnits(this->m_strRiverBedKUnits);
	this->m_strRiverBedThicknessUnits.Format("(%s)", this->m_units.river_bed_thickness.si);
}

CRiverPropertyPage2::~CRiverPropertyPage2()
{
}

void CRiverPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROP_TREE, this->m_wndTreeCtrl);
	DDX_GridControl(pDX, IDC_GRID_SCHEDULES, this->m_wndScheduleGrid);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, this->m_wndRichEditCtrl);

	DDX_Text(pDX, IDC_X_UNITS_STATIC, this->m_strHorizontalUnits);
	DDX_Text(pDX, IDC_Y_UNITS_STATIC, this->m_strHorizontalUnits);
	DDX_Text(pDX, IDC_WIDTH_UNITS_STATIC, this->m_strHorizontalUnits);
	DDX_Text(pDX, IDC_DEPTH_UNITS_STATIC, this->m_strVerticalUnits);	
	DDX_Text(pDX, IDC_BOTTOM_UNITS_STATIC, this->m_strVerticalUnits);
	DDX_Text(pDX, IDC_RIVER_K_UNITS_STATIC, this->m_strRiverBedKUnits);
	DDX_Text(pDX, IDC_RIVER_THICK_UNITS_STATIC, this->m_strRiverBedThicknessUnits);

	if (this->m_wndTreeCtrl.GetCount() != this->m_river.m_listPoints.size())
	{
		CDelayRedraw delay(&this->m_wndTreeCtrl);
		this->m_wndTreeCtrl.DeleteAllItems();

		CString str;
		std::list<CRiverPoint>::iterator iter = this->m_river.m_listPoints.begin();
		for (size_t i = 0; iter != this->m_river.m_listPoints.end(); ++i, ++iter)
		{
			str.Format("Point %d", i + 1);
			CTreeCtrlNode node = this->m_wndTreeCtrl.InsertItem(str);
			node.SetData((DWORD_PTR)&(*iter));
		}
	}

	if (this->m_bFirstSetActive)
	{
		VERIFY(this->m_wndTreeCtrl.SelectSetFirstVisible(this->m_wndTreeCtrl.GetFirstVisibleItem()));
		this->m_wndTreeCtrl.SetFocus();

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

				std::vector<LPCTSTR> options;
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
	}

	// n_user
	//
	DDX_Text(pDX, IDC_NUSER_EDIT, this->m_river.n_user);
	if (pDX->m_bSaveAndValidate)
	{
		std::set<int>::iterator iter = this->m_usedRiverNumbers.find(this->m_river.n_user);
		if (iter != this->m_usedRiverNumbers.end())
		{
			::AfxMessageBox("This river number is already in use. Please choose a different number.");
			pDX->Fail();        // throws exception
		}
	}

	// description
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_DESC_EDIT, str);
		this->m_river.description = str;
	}
	else
	{
		CString str(this->m_river.description.c_str());
		DDX_Text(pDX, IDC_DESC_EDIT, str);
		this->m_river.description = str;
	}

	if (this->m_bFirstSetActive && this->m_pointIndex)
	{
		// select given point index
		//
		ASSERT(this->m_pointIndex < this->m_wndTreeCtrl.GetCount());
		CTreeCtrlNode item = this->m_wndTreeCtrl.GetRootItem();
		for (UINT i = 0; item; ++i)
		{
			if (i == this->m_pointIndex)
			{
				item.Select();
				break;
			}
			item = item.GetNextSibling();
		}
	}

	// point
	//
	CRiverPoint* pPt = (CRiverPoint*)this->m_wndTreeCtrl.GetSelectedItem().GetData();
	ASSERT(pPt);

	bool bFirstOrLastPoint = false;
	if (pPt == &this->m_river.m_listPoints.front() || pPt == &this->m_river.m_listPoints.back())
	{
		bFirstOrLastPoint = true;
	}

	// X
	//
	if (pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_X_EDIT, pPt->x);
		pPt->x_defined = TRUE;
	}
	else
	{
		ASSERT(pPt->x_defined);
		DDX_Text(pDX, IDC_X_EDIT, pPt->x);
	}

	// Y
	//
	if (pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_Y_EDIT, pPt->y);
		pPt->y_defined = TRUE;
	}
	else
	{
		ASSERT(pPt->y_defined);
		DDX_Text(pDX, IDC_Y_EDIT, pPt->y);
	}

	// Width
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_WIDTH_EDIT, str);
		pPt->width_defined = FALSE;
		if (str.IsEmpty())
		{
			if (bFirstOrLastPoint)
			{
				::AfxMessageBox("Width must be defined for at least the first and the last river points.");
				pDX->Fail();
			}
		}
		else
		{
			DDX_Text(pDX, IDC_WIDTH_EDIT, pPt->width);
			pPt->width_defined = TRUE;
		}
	}
	else
	{
		if (pPt->width_defined)
		{
			DDX_Text(pDX, IDC_WIDTH_EDIT, pPt->width);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_WIDTH_EDIT, empty);
		}
	}

	// Depth / Bottom radio
	//
	if (pDX->m_bSaveAndValidate)
	{
		if (this->GetCheckedRadioButton(IDC_RADIO_BOTTOM, IDC_RADIO_DEPTH) == IDC_RADIO_BOTTOM)
		{
			// Bottom
			//
			CString str;
			DDX_Text(pDX, IDC_BOTTOM_EDIT, str);
			pPt->z_defined = FALSE;
			if (str.IsEmpty())
			{
				if (bFirstOrLastPoint)
				{
					::AfxMessageBox("Either depth or bottom must be defined for the first and the last river points.");
					pDX->Fail();
				}
			}
			else
			{
				DDX_Text(pDX, IDC_BOTTOM_EDIT, pPt->z);
				pPt->z_defined = TRUE;
			}
			pPt->depth_defined = FALSE;
		}
		else
		{
			// Depth
			//
			CString str;
			DDX_Text(pDX, IDC_DEPTH_EDIT, str);
			pPt->depth_defined = FALSE;
			if (str.IsEmpty())
			{
				if (bFirstOrLastPoint)
				{
					::AfxMessageBox("Either depth or bottom must be defined for the first and the last river points.");
					pDX->Fail();
				}
			}
			else
			{
				DDX_Text(pDX, IDC_DEPTH_EDIT, pPt->depth);
				pPt->depth_defined = TRUE;
			}
			pPt->z_defined = FALSE;
		}
	}
	else
	{
		// Bottom
		//
		if (pPt->z_defined)
		{
			ASSERT(!pPt->depth_defined);
			this->CheckRadioButton(IDC_RADIO_BOTTOM, IDC_RADIO_DEPTH, IDC_RADIO_BOTTOM);
			this->OnBnClickedRadioBottom();

			DDX_Text(pDX, IDC_BOTTOM_EDIT, pPt->z);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_BOTTOM_EDIT, empty);
		}

		// Depth
		//
		if (pPt->depth_defined)
		{
			ASSERT(!pPt->z_defined);
			this->CheckRadioButton(IDC_RADIO_BOTTOM, IDC_RADIO_DEPTH, IDC_RADIO_DEPTH);
			this->OnBnClickedRadioDepth();

			DDX_Text(pDX, IDC_DEPTH_EDIT, pPt->depth);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_DEPTH_EDIT, empty);
		}

		if (!pPt->z_defined && !pPt->depth_defined)
		{
			ASSERT(!pPt->z_defined);
			this->CheckRadioButton(IDC_RADIO_BOTTOM, IDC_RADIO_DEPTH, IDC_RADIO_DEPTH);
			this->OnBnClickedRadioDepth();
		}
	}

	// Bed hydraulic conductivity
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_RIVER_K_EDIT, str);
		pPt->k_defined = FALSE;
		if (str.IsEmpty())
		{
			if (bFirstOrLastPoint)
			{
				::AfxMessageBox("Bed hydraulic conductivity must be defined for at least the first and the last river points.");
				pDX->Fail();
			}
		}
		else
		{
			DDX_Text(pDX, IDC_RIVER_K_EDIT, pPt->k);
			pPt->k_defined = TRUE;
		}
	}
	else
	{
		if (pPt->k_defined)
		{
			DDX_Text(pDX, IDC_RIVER_K_EDIT, pPt->k);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_RIVER_K_EDIT, empty);
		}
	}

	// Bed thickness
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_RIVER_THICK_EDIT, str);
		pPt->thickness_defined = FALSE;
		if (str.IsEmpty())
		{
			if (bFirstOrLastPoint)
			{
				::AfxMessageBox("Bed thickness must be defined for at least the first and the last river points.");
				pDX->Fail();
			}
		}
		else
		{
			DDX_Text(pDX, IDC_RIVER_THICK_EDIT, pPt->thickness);
			pPt->thickness_defined = TRUE;
		}
	}
	else
	{
		if (pPt->thickness_defined)
		{
			DDX_Text(pDX, IDC_RIVER_THICK_EDIT, pPt->thickness);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_RIVER_THICK_EDIT, empty);
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
				if (!strValue.IsEmpty())
				{
					if (time.SetInput(strValue) != OK)
					{
						::DDX_GridControlFail(pDX, IDC_GRID_SCHEDULES, row, 1, _T("Please enter the start time units."));
					}
				}

				// head
				DDX_TextGridControl(pDX, IDC_GRID_SCHEDULES, row, 2, strValue);
				strValue.Trim();
				if (!strValue.IsEmpty())
				{
					DDX_TextGridControl(pDX, IDC_GRID_SCHEDULES, row, 2, value);
					state.SetHead(value);
				}
				if (row == 1 && !state.head_defined && bFirstOrLastPoint)
				{
					CString string("A head must be defined for time zero.");
					::DDX_GridControlFail(pDX, IDC_GRID_SCHEDULES, row, 2, string);
				}

				// solution
				DDX_TextGridControl(pDX, IDC_GRID_SCHEDULES, row, 3, strValue);
				strValue.Trim();
				if (!strValue.IsEmpty())
				{
					DDX_TextGridControl(pDX, IDC_GRID_SCHEDULES, row, 3, solution);
					state.SetSolution(solution);
				}
				if (row == 1 && !this->m_bFlowOnly && !state.solution_defined && bFirstOrLastPoint)
				{
					CString string("A solution must be defined for time zero.");
					::DDX_GridControlFail(pDX, IDC_GRID_SCHEDULES, row, 3, string);
				}

				if (state.head_defined || state.solution_defined)
				{
					pPt->Insert(time, state);
				}
			}
		}
	}
	else
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT;		

		// CRiverStates
		//
		const CTimeSeries<CRiverState>& map = pPt->GetRiverSched();
		CTimeSeries<CRiverState>::const_iterator iter = map.begin();
		int i = 1;
		for (; iter != map.end(); ++iter, ++i)
		{
			Item.row = i;

			// start time
			Item.col = 0;
			Item.strText.Format(_T("%g"), (*iter).first.value);
			this->m_wndScheduleGrid.SetItem(&Item);

			// start time units
			if ((*iter).first.type == UNDEFINED)
			{
				///ASSERT(this->m_units.time.input && ::strlen(this->m_units.time.input));
				///Item.strText = CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str();
				Item.strText.Empty();
			}
			else if ((*iter).first.type == STEP)
			{
				ASSERT(FALSE);
			}
			else if ((*iter).first.type == UNITS)
			{
				ASSERT((*iter).first.input && ::strlen((*iter).first.input));
				if ((*iter).first.input)
				{
					Item.strText = CGlobal::GetStdTimeUnits((*iter).first.input).c_str();
				}
				else
				{
					///ASSERT(this->m_units.time.input && ::strlen(this->m_units.time.input));
					///Item.strText = CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str();
					Item.strText.Empty();
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
		// clear remaining rows
		for (int j = i; j < this->m_wndScheduleGrid.GetRowCount(); ++j)
		{
			if (j == 1)
			{
				// start
				Item.strText.Format(_T("%g"), 0.0);
				this->m_wndScheduleGrid.SetItemText(j, 0, Item.strText);

				// start units
				///ASSERT(this->m_units.time.input && ::strlen(this->m_units.time.input));
				///Item.strText = CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str();
				///this->m_wndScheduleGrid.SetItemText(j, 1, Item.strText);
				this->m_wndScheduleGrid.SetItemText(j, 1, _T(""));
			}
			else
			{
				this->m_wndScheduleGrid.SetItemText(j, 0, _T(""));
                this->m_wndScheduleGrid.SetItemText(j, 1, _T(""));
			}
			this->m_wndScheduleGrid.SetItemText(j, 2, _T(""));
			this->m_wndScheduleGrid.SetItemText(j, 3, _T(""));
		}
	}

	if (pDX->m_bSaveAndValidate && this->m_bFullVerify)
	{
        bool head_defined;
		bool solution_defined;
		CTimeSeries<CRiverState>::iterator it;

		// verify first point
		//
        head_defined = false;
		solution_defined = false;
		it = this->m_river.m_listPoints.front().m_riverSchedule.begin();
		for (; it != this->m_river.m_listPoints.front().m_riverSchedule.end(); ++it)
		{
			if (it->second.head_defined)     head_defined = true;
			if (it->second.solution_defined) solution_defined = true;
		}
		if ((!this->m_river.m_listPoints.front().depth_defined && !this->m_river.m_listPoints.front().z_defined)
			||
			!this->m_river.m_listPoints.front().k_defined
			||
			!this->m_river.m_listPoints.front().thickness_defined
			||
			!this->m_river.m_listPoints.front().width_defined
			||
			this->m_river.m_listPoints.front().m_riverSchedule.empty()
			||
			!head_defined
			||
			(!solution_defined && !this->m_bFlowOnly)
			)
		{
			CTreeCtrlNode first = this->m_wndTreeCtrl.GetRootItem();
			::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)0, (LPARAM)(HTREEITEM)first);
			pDX->Fail();
		}


		// verify last point
		//
        head_defined = false;
		solution_defined = false;
		it = this->m_river.m_listPoints.back().m_riverSchedule.begin();
		for (; it != this->m_river.m_listPoints.back().m_riverSchedule.end(); ++it)
		{
			if (it->second.head_defined)     head_defined = true;
			if (it->second.solution_defined) solution_defined = true;
		}
		if ((!this->m_river.m_listPoints.back().depth_defined && !this->m_river.m_listPoints.back().z_defined)
			||
			!this->m_river.m_listPoints.back().k_defined
			||
			!this->m_river.m_listPoints.back().thickness_defined
			||
			!this->m_river.m_listPoints.back().width_defined
			||
			this->m_river.m_listPoints.back().m_riverSchedule.empty()
			||
			!head_defined
			||
			(!solution_defined && !this->m_bFlowOnly)
			)
		{
			CTreeCtrlNode last = this->m_wndTreeCtrl.GetRootItem();
			while(last.GetNextSibling())
			{
				last = last.GetNextSibling();
			}			
			::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)0, (LPARAM)(HTREEITEM)last);
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(CRiverPropertyPage2, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTvnSelchangingPropTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROP_TREE, OnTvnSelchangedPropTree)
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	ON_BN_CLICKED(IDC_RADIO_DEPTH, OnBnClickedRadioDepth)
	ON_BN_CLICKED(IDC_RADIO_BOTTOM, OnBnClickedRadioBottom)
END_MESSAGE_MAP()


// CRiverPropertyPage2 message handlers

void CRiverPropertyPage2::SetUnits(const CUnits& units)
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

void CRiverPropertyPage2::SetProperties(const CRiver& river)
{
	this->m_river = river;
#if defined(_DEBUG)
	if (!this->m_river.m_listPoints.front().depth_defined)
	{
		this->m_river.m_listPoints.front().depth = 1;
		this->m_river.m_listPoints.front().depth_defined = TRUE;
	}
	if (!this->m_river.m_listPoints.front().k_defined)
	{
		this->m_river.m_listPoints.front().k = 1;
		this->m_river.m_listPoints.front().k_defined = TRUE;
	}
	if (!this->m_river.m_listPoints.front().thickness_defined)
	{
		this->m_river.m_listPoints.front().thickness = 1;
		this->m_river.m_listPoints.front().thickness_defined = TRUE;
	}
	if (!this->m_river.m_listPoints.front().width_defined)
	{
		this->m_river.m_listPoints.front().width = 1;
		this->m_river.m_listPoints.front().width_defined = TRUE;
	}

	if (!this->m_river.m_listPoints.back().depth_defined)
	{
		this->m_river.m_listPoints.back().depth = this->m_river.m_listPoints.size();
		this->m_river.m_listPoints.back().depth_defined = TRUE;
	}
	if (!this->m_river.m_listPoints.back().k_defined)
	{
		this->m_river.m_listPoints.back().k = this->m_river.m_listPoints.size();
		this->m_river.m_listPoints.back().k_defined = TRUE;
	}
	if (!this->m_river.m_listPoints.back().thickness_defined)
	{
		this->m_river.m_listPoints.back().thickness = this->m_river.m_listPoints.size();
		this->m_river.m_listPoints.back().thickness_defined = TRUE;
	}
	if (!this->m_river.m_listPoints.back().width_defined)
	{
		this->m_river.m_listPoints.back().width = this->m_river.m_listPoints.size();
		this->m_river.m_listPoints.back().width_defined = TRUE;
	}
#endif
}

void CRiverPropertyPage2::GetProperties(CRiver& river)const
{
	river = this->m_river;
}

void CRiverPropertyPage2::SetPoint(int index)
{
	this->m_pointIndex = index;
}

void CRiverPropertyPage2::OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// validate itemOld
	//
	this->m_bFullVerify = false;
	if (!this->UpdateData(TRUE))
	{
		this->m_bFullVerify = true;

		// notify which control caused failure
		//
		CWnd* pFocus = CWnd::GetFocus();
		::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

		// disallow change
		//
		*pResult = TRUE;
		return;
	}

	this->m_bFullVerify = true;
	*pResult = 0;
}

void CRiverPropertyPage2::OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	this->UpdateData(FALSE);
	this->m_wndScheduleGrid.RedrawWindow();
	*pResult = 0;
}

LRESULT CRiverPropertyPage2::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	if (pFocus)
	{
		ASSERT_KINDOF(CWnd, pFocus);
		pFocus->SetFocus();
	}
	else if (lParam)
	{
		HTREEITEM hItem = (HTREEITEM)lParam;
		this->m_wndTreeCtrl.Select(hItem, TVGN_CARET);
		this->UpdateData(TRUE);
	}
	return 0;
}

void CRiverPropertyPage2::OnBnClickedRadioDepth()
{
	if (CWnd* pWnd = this->GetDlgItem(IDC_DEPTH_EDIT))
	{
		pWnd->EnableWindow(TRUE);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_DEPTH_UNITS_STATIC))
	{
		pWnd->EnableWindow(TRUE);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_BOTTOM_EDIT))
	{
		pWnd->EnableWindow(FALSE);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_BOTTOM_UNITS_STATIC))
	{
		pWnd->EnableWindow(FALSE);
	}	
}

void CRiverPropertyPage2::OnBnClickedRadioBottom()
{
	if (CWnd* pWnd = this->GetDlgItem(IDC_DEPTH_EDIT))
	{
		pWnd->EnableWindow(FALSE);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_DEPTH_UNITS_STATIC))
	{
		pWnd->EnableWindow(FALSE);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_BOTTOM_EDIT))
	{
		pWnd->EnableWindow(TRUE);
	}
	if (CWnd* pWnd = this->GetDlgItem(IDC_BOTTOM_UNITS_STATIC))
	{
		pWnd->EnableWindow(TRUE);
	}
}

void CRiverPropertyPage2::SetUsedRiverNumbers(const std::set<int>& used)
{
	this->m_usedRiverNumbers = used;
}
