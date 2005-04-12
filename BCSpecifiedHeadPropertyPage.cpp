// BCSpecifiedHeadPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "BCSpecifiedHeadPropertyPage.h"

#include "Global.h"

// CBCSpecifiedHeadPropertyPage dialog

IMPLEMENT_DYNAMIC(CBCSpecifiedHeadPropertyPage, baseCBCSpecifiedHeadPropertyPage)
CBCSpecifiedHeadPropertyPage::CBCSpecifiedHeadPropertyPage()
	: baseCBCSpecifiedHeadPropertyPage(CBCSpecifiedHeadPropertyPage::IDD)
{
}

CBCSpecifiedHeadPropertyPage::~CBCSpecifiedHeadPropertyPage()
{
}

void CBCSpecifiedHeadPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	baseCBCSpecifiedHeadPropertyPage::DoDataExchange(pDX);

	DDX_GridControl(pDX, IDC_HEAD_GRID, m_gridHead);
	DDX_GridControl(pDX, IDC_SOLUTION_GRID, m_gridSolution);

	if (this->m_bFirstSetActive)
	{
		this->SetupGrids();

		// head
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_HEAD_GRID, this->m_bc.m_bc_head);

		// solution type
		//
		if (this->m_bc.bc_solution_type == FIXED)
		{
			this->CheckRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO, IDC_FIXED_RADIO);
		}
		else
		{
			this->CheckRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO, IDC_ASSOC_RADIO);
		}

		// solution
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_SOLUTION_GRID, this->m_bc.m_bc_solution);
	}

	if (pDX->m_bSaveAndValidate)
	{
		CBC bc;

		// bc_type
		bc.bc_type = SPECIFIED;

		// head time series
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_HEAD_GRID, bc.m_bc_head);

		// solution time series
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_SOLUTION_GRID, bc.m_bc_solution);

		// solution type
		//
		bc.bc_solution_type = UNDEFINED;
		if (bc.m_bc_solution.size() != 0)
		{
			if (this->IsDlgButtonChecked(IDC_FIXED_RADIO))
			{
				bc.bc_solution_type = FIXED;
			}
			if (this->IsDlgButtonChecked(IDC_ASSOC_RADIO))
			{
				bc.bc_solution_type = ASSOCIATED;
			}
		}
		this->m_bc = bc;
	}
}


BEGIN_MESSAGE_MAP(CBCSpecifiedHeadPropertyPage, baseCBCSpecifiedHeadPropertyPage)
	ON_NOTIFY(GVN_ITEMCHANGED, IDC_HEAD_GRID, OnItemChangedHead)
	ON_NOTIFY(GVN_ITEMCHANGED, IDC_SOLUTION_GRID, OnItemChangedSolution)
END_MESSAGE_MAP()


// CBCSpecifiedHeadPropertyPage message handlers

BOOL CBCSpecifiedHeadPropertyPage::OnInitDialog()
{
	baseCBCSpecifiedHeadPropertyPage::OnInitDialog();

	// Add extra initialization here

	// Layout controls
	this->CreateRoot(VERTICAL)
		<< item(IDC_HEAD_GRID, GREEDY)
		<< itemFixed(VERTICAL, 3)
		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
			<< item(IDC_SOL_TYPE_STATIC, NORESIZE, 0, 0, 0, 0)
			<< itemFixed(HORIZONTAL, 3)
			<< item(IDC_ASSOC_RADIO, NORESIZE, 0, 0, 0, 0)
			<< item(IDC_FIXED_RADIO, NORESIZE, 0, 0, 0, 0)
			)
		<< item(IDC_SOLUTION_GRID, GREEDY)
		<< item(IDC_DESC_RICHEDIT, ABSOLUTE_VERT)		
		;
	UpdateLayout();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CBCSpecifiedHeadPropertyPage::SetProperties(const CBC& r_bc)
{
	this->m_bc = r_bc;
}

void CBCSpecifiedHeadPropertyPage::GetProperties(CBC& r_bc)
{
	r_bc = this->m_bc;
}

void CBCSpecifiedHeadPropertyPage::OnItemChangedHead(NMHDR *pNotifyStruct, LRESULT *result)
{
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	if (pnmgv->iColumn == 3)
	{
		ASSERT(this->m_gridHead.IsCheckMarkCell(pnmgv->iRow, pnmgv->iColumn));
		if (this->m_gridHead.GetCheck(pnmgv->iRow, pnmgv->iColumn) == BST_CHECKED)
		{
			this->m_gridHead.DisableCell(pnmgv->iRow, 2);
			for (int col = 4; col < 9; ++col)
			{
				this->m_gridHead.EnableCell(pnmgv->iRow, col);
			}
		}
		else
		{
			this->m_gridHead.EnableCell(pnmgv->iRow, 2);
			for (int col = 4; col < 9; ++col)
			{
				this->m_gridHead.DisableCell(pnmgv->iRow, col);
			}
		}
		this->m_gridHead.Invalidate();
	}
}

void CBCSpecifiedHeadPropertyPage::OnItemChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	if (pnmgv->iColumn == 3)
	{
		ASSERT(this->m_gridSolution.IsCheckMarkCell(pnmgv->iRow, pnmgv->iColumn));
		if (this->m_gridSolution.GetCheck(pnmgv->iRow, pnmgv->iColumn) == BST_CHECKED)
		{
			this->m_gridSolution.DisableCell(pnmgv->iRow, 2);
			for (int col = 4; col < 9; ++col)
			{
				this->m_gridSolution.EnableCell(pnmgv->iRow, col);
			}
		}
		else
		{
			this->m_gridSolution.EnableCell(pnmgv->iRow, 2);
			for (int col = 4; col < 9; ++col)
			{
				this->m_gridSolution.DisableCell(pnmgv->iRow, col);
			}
		}
		this->m_gridSolution.Invalidate();
	}
}

BOOL CBCSpecifiedHeadPropertyPage::SetupGrids(void)
{
	const int MIN_ROW_COUNT = 100;

	int nHeadRows = (int)this->m_bc.m_bc_head.size() + MIN_ROW_COUNT;
	int nSolnRows = (int)this->m_bc.m_bc_solution.size() + MIN_ROW_COUNT;

	try
	{
		this->m_gridHead.SetRowCount(nHeadRows);
		this->m_gridHead.SetColumnCount(9);
		this->m_gridHead.SetFixedRowCount(1);
		this->m_gridHead.SetFixedColumnCount(0);
		this->m_gridHead.EnableTitleTips(FALSE);

		this->m_gridSolution.SetRowCount(nSolnRows);
		this->m_gridSolution.SetColumnCount(9);
		this->m_gridSolution.SetFixedRowCount(1);
		this->m_gridSolution.SetFixedColumnCount(0);
		this->m_gridSolution.EnableTitleTips(FALSE);

	}
	catch (CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	// set default format
	GV_ITEM defaultFormat;
	defaultFormat.mask    = GVIF_FORMAT;
	defaultFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;
	for (int row = 0; row < nHeadRows; ++row)
	{
		defaultFormat.row = row;
		for (int col = 0; col < this->m_gridHead.GetColumnCount(); ++col)
		{ 
			defaultFormat.col = col;
			this->m_gridHead.SetItem(&defaultFormat);
		}
	}
	for (int row = 0; row < nSolnRows; ++row)
	{
		defaultFormat.row = row;
		for (int col = 0; col < this->m_gridSolution.GetColumnCount(); ++col)
		{ 
			defaultFormat.col = col;
			this->m_gridSolution.SetItem(&defaultFormat);
		}
	}

	std::vector<LPCTSTR> vecTimeUnits;
	vecTimeUnits.push_back(_T("seconds"));
	vecTimeUnits.push_back(_T("minutes"));
	vecTimeUnits.push_back(_T("hours"));
	vecTimeUnits.push_back(_T("days"));
	vecTimeUnits.push_back(_T("years"));

	std::vector<LPCTSTR> vecDirection;
	vecDirection.push_back(_T("X"));
	vecDirection.push_back(_T("Y"));
	vecDirection.push_back(_T("Z"));

	GV_ITEM Item;
	Item.mask = GVIF_TEXT;
	Item.row = 0;

	Item.col = 0;
	Item.szText = _T("Time");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 1;
	Item.szText = _T("Units");
	this->m_gridHead.SetItem(&Item);
	this->m_gridHead.SetColumnOptions(Item.col, vecTimeUnits);
	this->m_gridSolution.SetItem(&Item);
	this->m_gridSolution.SetColumnOptions(Item.col, vecTimeUnits);

	Item.col = 2;
	Item.szText = _T("Head");
	this->m_gridHead.SetItem(&Item);
	Item.szText = _T("Solution");
	this->m_gridSolution.SetItem(&Item);

	Item.col = 3;
	Item.szText = _T("Interpolate");
	this->m_gridHead.SetItem(&Item);
	this->m_gridHead.SetColumnCheck(Item.col, BST_UNCHECKED);
	this->m_gridSolution.SetItem(&Item);
	this->m_gridSolution.SetColumnCheck(Item.col, BST_UNCHECKED);

	Item.col = 4;
	Item.szText = _T("Direction");
	this->m_gridHead.SetItem(&Item);
	this->m_gridHead.SetColumnOptions(Item.col, vecDirection);
	this->m_gridSolution.SetItem(&Item);
	this->m_gridSolution.SetColumnOptions(Item.col, vecDirection);

	Item.col = 5;
	Item.szText = _T("Value 1");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 6;
	Item.szText = _T("Distance 1");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 7;
	Item.szText = _T("Value 2");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 8;
	Item.szText = _T("Distance 2");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	this->m_gridHead.SetItemText(1, 0, _T("0"));
	this->m_gridHead.DisableCell(1, 0);
	this->m_gridSolution.SetItemText(1, 0, _T("0"));
	this->m_gridSolution.DisableCell(1, 0);

	this->m_gridHead.DisableCell(1, 1);
	this->m_gridSolution.DisableCell(1, 1);

	for (int iRow = this->m_gridHead.GetFixedRowCount(); iRow < this->m_gridHead.GetRowCount(); ++iRow)
	{
		for (int iCol = 4; iCol < 9; ++iCol)
		{
			this->m_gridHead.DisableCell(iRow, iCol);
		}
	}
	for (int iRow = this->m_gridSolution.GetFixedRowCount(); iRow < this->m_gridSolution.GetRowCount(); ++iRow)
	{
		for (int iCol = 4; iCol < 9; ++iCol)
		{
			this->m_gridSolution.DisableCell(iRow, iCol);
		}
	}
	return TRUE;
}
