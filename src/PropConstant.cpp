// PropConstant.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropConstant.h"


// CPropConstant dialog

IMPLEMENT_DYNAMIC(CPropConstant, CResizablePage)

CPropConstant::CPropConstant()
: CResizablePage(CPropConstant::IDD)
, bSkipUpdateData(false)
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

	CResizablePage::DoDataExchange(pDX);

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


BEGIN_MESSAGE_MAP(CPropConstant, CResizablePage)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CPropConstant::OnCbnSelchangeTypeCombo)
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

HBRUSH CPropConstant::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (this->HasWhiteBackground())
	{
		pDC->SetBkMode(TRANSPARENT);
		return ::GetSysColorBrush(COLOR_WINDOW);
	}

	// default
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CPropConstant::OnCbnSelchangeTypeCombo()
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
	// Add your control notification handler code here
	ASSERT(!bSkipUpdateData);
	bSkipUpdateData = true;
	::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)ComboType.GetCurSel(), (LPARAM)0);
	ASSERT(bSkipUpdateData);
	bSkipUpdateData = false;
}

BOOL CPropConstant::OnSetActive()
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
	ASSERT_VALID(this);

	if (m_bFirstSetActive)
	{
		m_bFirstSetActive = FALSE;
	}
	else
	{
		UpdateData(FALSE);
	}
	return TRUE;
}

BOOL CPropConstant::OnKillActive()
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
	ASSERT_VALID(this);

#if defined(__MULTIPLE__)
	for (int col = 0; col < this->m_wndTimesGrid.GetColumnCount(); ++col)
	{
		TRACE("Column(%d) = %d \n", col, this->m_wndTimesGrid.GetColumnWidth(col));
	}
#endif

	if (!this->bSkipUpdateData)
	{
		if (!UpdateData())
		{
			TRACE(traceAppMsg, 0, "UpdateData failed during page deactivation\n");
			return FALSE;
		}
	}
	return TRUE;
}

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
