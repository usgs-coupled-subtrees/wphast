// PropConstantM.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropConstantM.h"


// CPropConstantM dialog

IMPLEMENT_DYNAMIC(CPropConstantM, CPropPage)

CPropConstantM::CPropConstantM()
: CPropPage(CPropConstantM::IDD)
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
}

CPropConstantM::~CPropConstantM()
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
}

void CPropConstantM::DoDataExchange(CDataExchange* pDX)
{
	TRACE("%s(%p)\n", __FUNCTION__, this);

#if defined(__MULTIPLE__)
	CResizablePage::DoDataExchange(pDX);
#else
	__super::DoDataExchange(pDX);
#endif

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
// COMMENT: {6/1/2009 10:02:58 PM}			vecPropType.push_back(_T("Points"));
// COMMENT: {6/1/2009 10:02:58 PM}			vecPropType.push_back(_T("XYZ"));

			this->m_wndTimesGrid.SetColumnOptions(2, vecPropType);
			//}}

			this->m_wndTimesGrid.SetColumnWidth(0, 71);
			this->m_wndTimesGrid.SetColumnWidth(1, 62);
			this->m_wndTimesGrid.SetColumnWidth(2, 70);
		}
		///this->m_wndTimesGrid.ExpandColumnsToFit();
	}
#endif

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


BEGIN_MESSAGE_MAP(CPropConstantM, CPropPage)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_TIMES_GRID, OnEndLabelEditTimes)
	ON_NOTIFY(GVN_SELCHANGED, IDC_TIMES_GRID, OnSelChangedTimes)
END_MESSAGE_MAP()


// CPropConstantM message handlers

BOOL CPropConstantM::OnInitDialog()
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

Cproperty CPropConstantM::GetProperty()
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
	return this->prop;
}

void CPropConstantM::SetProperty(Cproperty p)
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
	this->prop = p;
}

void CPropConstantM::OnEndLabelEditTimes(NMHDR *pNotifyStruct, LRESULT *result)
{
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	CString str = this->m_wndTimesGrid.GetItemText(pnmgv->iRow, pnmgv->iColumn);

	// Add your control notification handler code here
	ASSERT(!this->SkipUpdateData);
	this->SkipUpdateData = true;
	if (this->m_wndTimesGrid.GetSafeHwnd() && pnmgv->iColumn == 2)
	{
		int n = 0; // "None"
		if (str.Compare("Constant") == 0)
		{
			n = 1;
		}
		else if (str.Compare("Linear") == 0)
		{
			n = 2;
		}
		if (n != 1)
		{
			::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)n, (LPARAM)0);
		}
	}
	ASSERT(this->SkipUpdateData);
	this->SkipUpdateData = false;
}

void CPropConstantM::OnSelChangedTimes(NMHDR *pNotifyStruct, LRESULT *result)
{
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	TRACE("CPropConstantM::OnSelChangedFlux row = %d\n", pnmgv->iRow);

	ASSERT(!this->SkipUpdateData);
	this->SkipUpdateData = true;
	if (this->m_wndTimesGrid.GetSafeHwnd())
	{
		CString str = this->m_wndTimesGrid.GetItemText(pnmgv->iRow, 2);
		int n = 0; // "None"
		if (str.Compare("Constant") == 0)
		{
			n = 1;
		}
		else if (str.Compare("Linear") == 0)
		{
			n = 2;
		}
		if (n != 1)
		{
			::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)n, (LPARAM)0);
		}
	}
	ASSERT(this->SkipUpdateData);
	this->SkipUpdateData = false;
}
