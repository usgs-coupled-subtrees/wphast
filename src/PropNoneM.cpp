// PropNone.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropNoneM.h"


// CPropNoneM dialog

IMPLEMENT_DYNAMIC(CPropNoneM, CPropPage)

CPropNoneM::CPropNoneM()
: CPropPage(CPropNoneM::IDD)
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
}

CPropNoneM::~CPropNoneM()
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
}

void CPropNoneM::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
// COMMENT: {6/3/2009 3:01:48 PM}	DDX_Control(pDX, IDC_TYPE_COMBO, ComboType);
// COMMENT: {6/3/2009 3:01:48 PM}
// COMMENT: {6/3/2009 3:01:48 PM}	if (pDX->m_bSaveAndValidate)
// COMMENT: {6/3/2009 3:01:48 PM}	{
// COMMENT: {6/3/2009 3:01:48 PM}	}
// COMMENT: {6/3/2009 3:01:48 PM}	else
// COMMENT: {6/3/2009 3:01:48 PM}	{
// COMMENT: {6/3/2009 3:01:48 PM}		VERIFY(this->ComboType.SelectString(0, _T("None")) != CB_ERR);
// COMMENT: {6/3/2009 3:01:48 PM}	}

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

			this->m_wndTimesGrid.SetItemText(1, 2, _T("None"));

			//{{
			std::vector<CString> vecTimeUnits;
			vecTimeUnits.push_back(_T("seconds"));
			vecTimeUnits.push_back(_T("minutes"));
			vecTimeUnits.push_back(_T("hours"));
			vecTimeUnits.push_back(_T("days"));
			vecTimeUnits.push_back(_T("years"));

			this->m_wndTimesGrid.SetColumnOptions(1, vecTimeUnits);
			//}}

			//{{
			std::vector<CString> vecPropType;
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
}


BEGIN_MESSAGE_MAP(CPropNoneM, CPropPage)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_TIMES_GRID, OnEndLabelEditTimes)
	ON_NOTIFY(GVN_SELCHANGED, IDC_TIMES_GRID, OnSelChangedTimes)
END_MESSAGE_MAP()


// CPropNoneM message handlers

BOOL CPropNoneM::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
// COMMENT: {6/3/2009 3:07:36 PM}	this->AddAnchor(IDC_TYPE_COMBO, TOP_LEFT, TOP_RIGHT);
#if defined(__MULTIPLE__)
	this->AddAnchor(IDC_TIMES_GRID, TOP_LEFT, BOTTOM_LEFT);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPropNoneM::OnEndLabelEditTimes(NMHDR *pNotifyStruct, LRESULT *result)
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
		if (n != 0)
		{
			::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)n, (LPARAM)0);
		}
	}
	ASSERT(this->SkipUpdateData);
	this->SkipUpdateData = false;
}

void CPropNoneM::OnSelChangedTimes(NMHDR *pNotifyStruct, LRESULT *result)
{
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	TRACE("CPropNoneM::OnSelChangedFlux row = %d\n", pnmgv->iRow);

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
		if (n != 0)
		{
			::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)n, (LPARAM)0);
		}
	}
	ASSERT(this->SkipUpdateData);
	this->SkipUpdateData = false;
}
