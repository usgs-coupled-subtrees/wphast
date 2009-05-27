// PropConstant.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropConstant.h"


// CPropConstant dialog

IMPLEMENT_DYNAMIC(CPropConstant, CPropPage)

CPropConstant::CPropConstant()
: CPropPage(CPropConstant::IDD)
// COMMENT: {5/26/2009 8:48:57 PM}, bSkipUpdateData(false)
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
}

CPropConstant::~CPropConstant()
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
}

void CPropConstant::DoDataExchange(CDataExchange* pDX)
{
	TRACE("%s(%p)\n", __FUNCTION__, this);

	__super::DoDataExchange(pDX);

#if !defined(__MULTIPLE__)
	DDX_Control(pDX, IDC_TYPE_COMBO, ComboType);
	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
		VERIFY(this->ComboType.SelectString(0, _T("Constant")) != CB_ERR);
	}
#endif

#if defined(__MULTIPLE__)
	DDX_GridControl(pDX, IDC_TIMES_GRID, m_wndTimesGrid);

	// Prepare Times Grid
	//
	if (!pDX->m_bSaveAndValidate && this->m_wndTimesGrid.GetColumnCount() == 0)
	{
		const int MIN_ROWS = 100;
		TRY
		{
			this->m_wndTimesGrid.SetRowCount(MIN_ROWS/* + this->m_well.count_depth_user + this->m_well.count_elevation_user*/);
			this->m_wndTimesGrid.SetColumnCount(3);
			this->m_wndTimesGrid.SetFixedRowCount(1);
			this->m_wndTimesGrid.SetFixedColumnCount(0);
			this->m_wndTimesGrid.EnableTitleTips(FALSE);
			this->m_wndTimesGrid.SetCurrentFocusCell(1, 0);
			// this->m_wndTimesGrid.EnableSelection(FALSE); // this breaks CutSelectedText
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

		// set default format
		for (int row = 0; row < this->m_wndTimesGrid.GetRowCount(); ++row)
		{
			for (int col = 0; col < this->m_wndTimesGrid.GetColumnCount(); ++col)
			{ 
				this->m_wndTimesGrid.SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS);
			}
		}
		///this->SetScreenHeadings(this->m_bByDepth);
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT;
			Item.row = 0;

			Item.col = 0;
			Item.szText.Format(_T("Time"));
			this->m_wndTimesGrid.SetItem(&Item);

			Item.col = 1;
			Item.szText.Format(_T("Units"));
			this->m_wndTimesGrid.SetItem(&Item);

			Item.col = 2;
			Item.szText.Format(_T("Type"));
			this->m_wndTimesGrid.SetItem(&Item);

			this->m_wndTimesGrid.SetItemText(1, 0, _T("0"));
			this->m_wndTimesGrid.DisableCell(1, 0);

			this->m_wndTimesGrid.DisableCell(1, 1);

			this->m_wndTimesGrid.SetItemText(1, 2, _T("Constant"));

			//{{
			std::vector<LPCTSTR> vecTimeUnits;
			vecTimeUnits.push_back(_T("seconds"));
			vecTimeUnits.push_back(_T("minutes"));
			vecTimeUnits.push_back(_T("hours"));
			vecTimeUnits.push_back(_T("days"));
			vecTimeUnits.push_back(_T("years"));

			this->m_wndTimesGrid.SetColumnOptions(1, vecTimeUnits);
			//}}

			//{{
			std::vector<LPCTSTR> vecPropType;
			vecPropType.push_back(_T("None"));
			vecPropType.push_back(_T("Constant"));
			vecPropType.push_back(_T("Linear"));
			vecPropType.push_back(_T("Points"));
			vecPropType.push_back(_T("XYZ"));

			this->m_wndTimesGrid.SetColumnOptions(2, vecPropType);
			//}}

			this->m_wndTimesGrid.SetColumnWidth(0, 71);
			this->m_wndTimesGrid.SetColumnWidth(1, 62);
			this->m_wndTimesGrid.SetColumnWidth(2, 70);
		}
		///this->m_wndTimesGrid.ExpandColumnsToFit();
	}

#endif


// COMMENT: {4/22/2009 4:30:24 PM}#ifdef _DEBUG
// COMMENT: {4/22/2009 4:30:24 PM}	if (pDX->m_bSaveAndValidate)
// COMMENT: {4/22/2009 4:30:24 PM}	{
// COMMENT: {4/22/2009 4:30:24 PM}		DDX_Text(pDX, IDC_VALUE_EDIT, this->value);
// COMMENT: {4/22/2009 4:30:24 PM}	}
// COMMENT: {4/22/2009 4:30:24 PM}	else
// COMMENT: {4/22/2009 4:30:24 PM}	{
// COMMENT: {4/22/2009 4:30:24 PM}		DDX_Text(pDX, IDC_VALUE_EDIT, this->value);
// COMMENT: {4/22/2009 4:30:24 PM}	}
// COMMENT: {4/22/2009 4:30:24 PM}#else
// COMMENT: {4/22/2009 4:30:24 PM}	DDX_Text(pDX, IDC_VALUE_EDIT, this->value);
// COMMENT: {4/22/2009 4:30:24 PM}#endif

	if (pDX->m_bSaveAndValidate)
	{
		TRACE("%s(%p) TRUE\n", __FUNCTION__, this);

		double d;
		DDX_Text(pDX, IDC_VALUE_EDIT, d);
		this->prop = Cproperty(d);
	}
	else
	{
		TRACE("%s(%p) FALSE\n", __FUNCTION__, this);

		if (this->prop.type == PROP_FIXED)
		{
			DDX_Text(pDX, IDC_VALUE_EDIT, this->prop.v[0]);
		}
		else
		{
			CString empty;
			DDX_Text(pDX, IDC_VALUE_EDIT, empty);
		}
	}
}


BEGIN_MESSAGE_MAP(CPropConstant, CPropPage)
// COMMENT: {5/26/2009 9:15:52 PM}	ON_WM_CTLCOLOR()
// COMMENT: {5/26/2009 9:15:52 PM}	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CPropConstant::OnCbnSelchangeTypeCombo)
END_MESSAGE_MAP()


// CPropConstant message handlers

BOOL CPropConstant::OnInitDialog()
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
#if !defined(__MULTIPLE__)
	this->AddAnchor(IDC_TYPE_COMBO, TOP_LEFT, TOP_RIGHT);
#endif
	this->AddAnchor(IDC_VALUE_EDIT, TOP_LEFT, TOP_RIGHT);
#if defined(__MULTIPLE__)
	this->AddAnchor(IDC_TIMES_GRID, TOP_LEFT, BOTTOM_LEFT);
#endif


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// COMMENT: {5/26/2009 8:51:03 PM}HBRUSH CPropConstant::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
// COMMENT: {5/26/2009 8:51:03 PM}{
// COMMENT: {5/26/2009 8:51:03 PM}	if (this->HasWhiteBackground())
// COMMENT: {5/26/2009 8:51:03 PM}	{
// COMMENT: {5/26/2009 8:51:03 PM}		pDC->SetBkMode(TRANSPARENT);
// COMMENT: {5/26/2009 8:51:03 PM}		return ::GetSysColorBrush(COLOR_WINDOW);
// COMMENT: {5/26/2009 8:51:03 PM}	}
// COMMENT: {5/26/2009 8:51:03 PM}
// COMMENT: {5/26/2009 8:51:03 PM}	// default
// COMMENT: {5/26/2009 8:51:03 PM}	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
// COMMENT: {5/26/2009 8:51:03 PM}	return hbr;
// COMMENT: {5/26/2009 8:51:03 PM}}

// COMMENT: {5/26/2009 8:50:22 PM}void CPropConstant::OnCbnSelchangeTypeCombo()
// COMMENT: {5/26/2009 8:50:22 PM}{
// COMMENT: {5/26/2009 8:50:22 PM}	TRACE("%s(%p)\n", __FUNCTION__, this);
// COMMENT: {5/26/2009 8:50:22 PM}	// Add your control notification handler code here
// COMMENT: {5/26/2009 8:50:22 PM}	ASSERT(!bSkipUpdateData);
// COMMENT: {5/26/2009 8:50:22 PM}	bSkipUpdateData = true;
// COMMENT: {5/26/2009 8:50:22 PM}	::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)ComboType.GetCurSel(), (LPARAM)0);
// COMMENT: {5/26/2009 8:50:22 PM}	ASSERT(bSkipUpdateData);
// COMMENT: {5/26/2009 8:50:22 PM}	bSkipUpdateData = false;
// COMMENT: {5/26/2009 8:50:22 PM}}

// COMMENT: {5/26/2009 8:54:30 PM}BOOL CPropConstant::OnSetActive()
// COMMENT: {5/26/2009 8:54:30 PM}{
// COMMENT: {5/26/2009 8:54:30 PM}	return __super::OnSetActive();
// COMMENT: {5/26/2009 8:54:30 PM}
// COMMENT: {5/26/2009 8:54:30 PM}	TRACE("%s(%p)\n", __FUNCTION__, this);
// COMMENT: {5/26/2009 8:54:30 PM}	ASSERT_VALID(this);
// COMMENT: {5/26/2009 8:54:30 PM}
// COMMENT: {5/26/2009 8:54:30 PM}	if (m_bFirstSetActive)
// COMMENT: {5/26/2009 8:54:30 PM}	{
// COMMENT: {5/26/2009 8:54:30 PM}		m_bFirstSetActive = FALSE;
// COMMENT: {5/26/2009 8:54:30 PM}	}
// COMMENT: {5/26/2009 8:54:30 PM}	else
// COMMENT: {5/26/2009 8:54:30 PM}	{
// COMMENT: {5/26/2009 8:54:30 PM}		UpdateData(FALSE);
// COMMENT: {5/26/2009 8:54:30 PM}	}
// COMMENT: {5/26/2009 8:54:30 PM}	return TRUE;
// COMMENT: {5/26/2009 8:54:30 PM}}

// COMMENT: {5/26/2009 8:50:40 PM}BOOL CPropConstant::OnKillActive()
// COMMENT: {5/26/2009 8:50:40 PM}{
// COMMENT: {5/26/2009 8:50:40 PM}	TRACE("%s(%p)\n", __FUNCTION__, this);
// COMMENT: {5/26/2009 8:50:40 PM}	ASSERT_VALID(this);
// COMMENT: {5/26/2009 8:50:40 PM}
// COMMENT: {5/26/2009 8:50:40 PM}#if defined(__MULTIPLE__)
// COMMENT: {5/26/2009 8:50:40 PM}	for (int col = 0; col < this->m_wndTimesGrid.GetColumnCount(); ++col)
// COMMENT: {5/26/2009 8:50:40 PM}	{
// COMMENT: {5/26/2009 8:50:40 PM}		TRACE("Column(%d) = %d \n", col, this->m_wndTimesGrid.GetColumnWidth(col));
// COMMENT: {5/26/2009 8:50:40 PM}	}
// COMMENT: {5/26/2009 8:50:40 PM}#endif
// COMMENT: {5/26/2009 8:50:40 PM}
// COMMENT: {5/26/2009 8:50:40 PM}	if (!this->bSkipUpdateData)
// COMMENT: {5/26/2009 8:50:40 PM}	{
// COMMENT: {5/26/2009 8:50:40 PM}		if (!UpdateData())
// COMMENT: {5/26/2009 8:50:40 PM}		{
// COMMENT: {5/26/2009 8:50:40 PM}			TRACE(traceAppMsg, 0, "UpdateData failed during page deactivation\n");
// COMMENT: {5/26/2009 8:50:40 PM}			return FALSE;
// COMMENT: {5/26/2009 8:50:40 PM}		}
// COMMENT: {5/26/2009 8:50:40 PM}	}
// COMMENT: {5/26/2009 8:50:40 PM}	return TRUE;
// COMMENT: {5/26/2009 8:50:40 PM}}

Cproperty CPropConstant::GetProperty()
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
	return this->prop;
}

void CPropConstant::SetProperty(Cproperty p)
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
	this->prop = p;
}
