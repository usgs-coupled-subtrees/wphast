// BCLeakyPropertyPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "BCLeakyPropertyPage2.h"

#include "Global.h"

#define strText szText

// CBCLeakyPropertyPage2 dialog

IMPLEMENT_DYNAMIC(CBCLeakyPropertyPage2, baseCBCLeakyPropertyPage2)
CBCLeakyPropertyPage2::CBCLeakyPropertyPage2()
	: baseCBCLeakyPropertyPage2(CBCLeakyPropertyPage2::IDD)
{
}

CBCLeakyPropertyPage2::~CBCLeakyPropertyPage2()
{
}

void CBCLeakyPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	baseCBCLeakyPropertyPage2::DoDataExchange(pDX);

	DDX_GridControl(pDX, IDC_HEAD_GRID, m_gridHead);
	DDX_GridControl(pDX, IDC_SOLUTION_GRID, m_gridSolution);
	DDX_GridControl(pDX, IDC_SINGLE_GRID, m_gridSingle);

	if (this->m_bFirstSetActive)
	{
		this->SetupGrids();

		// head
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_HEAD_GRID, this->m_bc.m_bc_head);

		// solution
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_SOLUTION_GRID, this->m_bc.m_bc_solution);

		// thickness
		//
		CGlobal::DDX_Property(pDX, IDC_SINGLE_GRID, 1, this->m_bc.bc_thick, true);

		// hydraulic_conductivity
		//
		CGlobal::DDX_Property(pDX, IDC_SINGLE_GRID, 2, this->m_bc.bc_k, true);

		// face
		//
		switch(this->m_bc.face)
		{
		case 0: // x
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_X_RADIO);
			break;
		case 1: // y
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_Y_RADIO);
			break;
		case 2: // z
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_Z_RADIO);
			break;
		default: // x
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_X_RADIO);
		}
	}

	if (pDX->m_bSaveAndValidate)
	{
		CBC bc;

		// bc_type
		bc.bc_type = LEAKY;

		// head time series
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_HEAD_GRID, bc.m_bc_head);

		// solution time series
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_SOLUTION_GRID, bc.m_bc_solution);
		if (bc.m_bc_solution.size())
		{
			bc.bc_solution_type = ASSOCIATED;
		}

		// thickness
		//
		Cproperty thick;
		CGlobal::DDX_Property(pDX, IDC_SINGLE_GRID, 1, &thick, true);
		if (thick.type != UNDEFINED)
		{
			Cproperty::CopyProperty(&bc.bc_thick, &thick);
		}

		// hydraulic_conductivity
		Cproperty k;
		CGlobal::DDX_Property(pDX, IDC_SINGLE_GRID, 2, &k, true);
		if (k.type != UNDEFINED)
		{
			Cproperty::CopyProperty(&bc.bc_k, &k);
		}

		// face
		//
		if (this->IsDlgButtonChecked(IDC_FACE_X_RADIO))
		{
			bc.face_defined = TRUE;
			bc.face         = 0;
		}
		if (this->IsDlgButtonChecked(IDC_FACE_Y_RADIO))
		{
			bc.face_defined = TRUE;
			bc.face         = 1;
		}
		if (this->IsDlgButtonChecked(IDC_FACE_Z_RADIO))
		{
			bc.face_defined = TRUE;
			bc.face         = 2;
		}
		this->m_bc = bc;
	}
}


BEGIN_MESSAGE_MAP(CBCLeakyPropertyPage2, baseCBCLeakyPropertyPage2)
	ON_NOTIFY(GVN_CHECKCHANGED, IDC_HEAD_GRID, OnCheckChangedHead)
	ON_NOTIFY(GVN_CHECKCHANGED, IDC_SOLUTION_GRID, OnCheckChangedSolution)
	ON_NOTIFY(GVN_CHECKCHANGED, IDC_SINGLE_GRID, OnCheckChangedSingle)
END_MESSAGE_MAP()


// CBCLeakyPropertyPage2 message handlers

BOOL CBCLeakyPropertyPage2::OnInitDialog()
{
	baseCBCLeakyPropertyPage2::OnInitDialog();

	// Add extra initialization here

	// Layout controls
	this->CreateRoot(VERTICAL)
		<< item(IDC_HEAD_GRID, GREEDY)
		<< item(IDC_SOLUTION_GRID, GREEDY)
		<< item(IDC_SINGLE_GRID, ABSOLUTE_VERT)
		//<< itemFixed(VERTICAL, 3)
		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
			<< item(IDC_FACE_STATIC, NORESIZE, 0, 0, 0, 0)
			<< itemFixed(HORIZONTAL, 3)
			<< item(IDC_FACE_X_RADIO, NORESIZE, 0, 0, 0, 0)
			<< item(IDC_FACE_Y_RADIO, NORESIZE, 0, 0, 0, 0)
			<< item(IDC_FACE_Z_RADIO, NORESIZE, 0, 0, 0, 0)
			)
		<< item(IDC_DESC_RICHEDIT, ABSOLUTE_VERT)		
		;
	UpdateLayout();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CBCLeakyPropertyPage2::SetProperties(const CBC& r_bc)
{
	this->m_bc = r_bc;
}

void CBCLeakyPropertyPage2::GetProperties(CBC& r_bc)
{
	r_bc = this->m_bc;
}

BOOL CBCLeakyPropertyPage2::SetupGrids(void)
{
	// TODO simplify this
	//
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
		this->m_gridHead.SetCurrentFocusCell(1, 2);

		this->m_gridSolution.SetRowCount(nSolnRows);
		this->m_gridSolution.SetColumnCount(9);
		this->m_gridSolution.SetFixedRowCount(1);
		this->m_gridSolution.SetFixedColumnCount(0);
		this->m_gridSolution.EnableTitleTips(FALSE);
		this->m_gridSolution.SetCurrentFocusCell(1, 2);

		this->m_gridSingle.SetRowCount(3);
		this->m_gridSingle.SetColumnCount(8);
		VERIFY(this->m_gridSingle.SetFixedRowCount(1) == TRUE);
		this->m_gridSingle.SetFixedColumnCount(1);
		this->m_gridSingle.EnableTitleTips(FALSE);
		this->m_gridSingle.SetRowResize(FALSE);
		this->m_gridSingle.SetCurrentFocusCell(1, 1);

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
	for (int row = 0; row < 3; ++row)
	{
		defaultFormat.row = row;
		for (int col = 0; col < this->m_gridSingle.GetColumnCount(); ++col)
		{ 
			defaultFormat.col = col;
			this->m_gridSingle.SetItem(&defaultFormat);
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
	Item.strText = _T("Time");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 1;
	Item.strText = _T("Units");
	this->m_gridHead.SetItem(&Item);
	this->m_gridHead.SetColumnOptions(Item.col, vecTimeUnits);
	this->m_gridSolution.SetItem(&Item);
	this->m_gridSolution.SetColumnOptions(Item.col, vecTimeUnits);

	Item.col = 2;
	Item.strText = _T("Head");
	this->m_gridHead.SetItem(&Item);
	Item.strText = _T("Solution");
	this->m_gridSolution.SetItem(&Item);
	//{{
	Item.col = 1;
	Item.strText = _T("Value");
	this->m_gridSingle.SetItem(&Item);
	//}}

	Item.col = 3;
	Item.strText = _T("Interpolate");
	this->m_gridHead.SetItem(&Item);
	this->m_gridHead.SetColumnCheck(Item.col, BST_UNCHECKED);
	this->m_gridSolution.SetItem(&Item);
	this->m_gridSolution.SetColumnCheck(Item.col, BST_UNCHECKED);
	//{{
	Item.col = 2;
	this->m_gridSingle.SetItem(&Item);
	this->m_gridSingle.SetColumnCheck(Item.col, BST_UNCHECKED);
	//}}

	Item.col = 4;
	Item.strText = _T("Direction");
	this->m_gridHead.SetItem(&Item);
	this->m_gridHead.SetColumnOptions(Item.col, vecDirection);
	this->m_gridSolution.SetItem(&Item);
	this->m_gridSolution.SetColumnOptions(Item.col, vecDirection);
	//{{
	Item.col = 3;
	this->m_gridSingle.SetItem(&Item);
	this->m_gridSingle.SetColumnOptions(Item.col, vecDirection);
	//}}

	Item.col = 5;
	Item.strText = _T("Value 1");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);
	//{{
	Item.col = 4;
	this->m_gridSingle.SetItem(&Item);
	//}}

	Item.col = 6;
	Item.strText = _T("Distance 1");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);
	//{{
	Item.col = 5;
	this->m_gridSingle.SetItem(&Item);
	//}}

	Item.col = 7;
	Item.strText = _T("Value 2");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);
	//{{
	Item.col = 6;
	this->m_gridSingle.SetItem(&Item);
	//}}

	Item.col = 8;
	Item.strText = _T("Distance 2");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);
	//{{
	Item.col = 7;
	this->m_gridSingle.SetItem(&Item);
	//}}

	this->m_gridSingle.SetItemText(1, 0, _T("Thickness"));
	this->m_gridSingle.SetItemText(2, 0, _T("Hydraulic conductivity"));
	//this->m_gridSingle.AutoSizeColumn(0);
	this->m_gridSingle.SetColumnWidth(0, 132);


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

void CBCLeakyPropertyPage2::OnCheckChangedHead(NMHDR *pNotifyStruct, LRESULT *result)
{
	static const int COL_VALUE       = 2;
	static const int COL_INTERPOLATE = 3;
	static const int COL_DIRECTION   = 4;
	static const int COL_VALUE_1     = 5;
	static const int COL_DISTANCE_1  = 6;
	static const int COL_VALUE_2     = 7;
	static const int COL_DISTANCE_2  = 8;

	CModGridCtrlEx* pGrid = &this->m_gridHead;

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	if (pnmgv->iColumn == COL_INTERPOLATE)
	{
		ASSERT(pGrid->IsCheckMarkCell(pnmgv->iRow, pnmgv->iColumn));
		if (pGrid->GetCheck(pnmgv->iRow, pnmgv->iColumn) == BST_CHECKED)
		{
			// disable
			pGrid->DisableCell(pnmgv->iRow, COL_VALUE);
			// enable
			pGrid->EnableCell(pnmgv->iRow,  COL_DIRECTION);
			pGrid->EnableCell(pnmgv->iRow,  COL_VALUE_1);
			pGrid->EnableCell(pnmgv->iRow,  COL_DISTANCE_1);
			pGrid->EnableCell(pnmgv->iRow,  COL_VALUE_2);
			pGrid->EnableCell(pnmgv->iRow,  COL_DISTANCE_2);
		}
		else
		{
			// enable
			pGrid->EnableCell(pnmgv->iRow, COL_VALUE);
			// disable
			pGrid->DisableCell(pnmgv->iRow,  COL_DIRECTION);
			pGrid->DisableCell(pnmgv->iRow,  COL_VALUE_1);
			pGrid->DisableCell(pnmgv->iRow,  COL_DISTANCE_1);
			pGrid->DisableCell(pnmgv->iRow,  COL_VALUE_2);
			pGrid->DisableCell(pnmgv->iRow,  COL_DISTANCE_2);
		}
		pGrid->Invalidate();
	}
}

void CBCLeakyPropertyPage2::OnCheckChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	static const int COL_VALUE       = 2;
	static const int COL_INTERPOLATE = 3;
	static const int COL_DIRECTION   = 4;
	static const int COL_VALUE_1     = 5;
	static const int COL_DISTANCE_1  = 6;
	static const int COL_VALUE_2     = 7;
	static const int COL_DISTANCE_2  = 8;

	CModGridCtrlEx* pGrid = &this->m_gridSolution;

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	if (pnmgv->iColumn == COL_INTERPOLATE)
	{
		ASSERT(pGrid->IsCheckMarkCell(pnmgv->iRow, pnmgv->iColumn));
		if (pGrid->GetCheck(pnmgv->iRow, pnmgv->iColumn) == BST_CHECKED)
		{
			// disable
			pGrid->DisableCell(pnmgv->iRow, COL_VALUE);
			// enable
			pGrid->EnableCell(pnmgv->iRow,  COL_DIRECTION);
			pGrid->EnableCell(pnmgv->iRow,  COL_VALUE_1);
			pGrid->EnableCell(pnmgv->iRow,  COL_DISTANCE_1);
			pGrid->EnableCell(pnmgv->iRow,  COL_VALUE_2);
			pGrid->EnableCell(pnmgv->iRow,  COL_DISTANCE_2);
		}
		else
		{
			// enable
			pGrid->EnableCell(pnmgv->iRow, COL_VALUE);
			// disable
			pGrid->DisableCell(pnmgv->iRow,  COL_DIRECTION);
			pGrid->DisableCell(pnmgv->iRow,  COL_VALUE_1);
			pGrid->DisableCell(pnmgv->iRow,  COL_DISTANCE_1);
			pGrid->DisableCell(pnmgv->iRow,  COL_VALUE_2);
			pGrid->DisableCell(pnmgv->iRow,  COL_DISTANCE_2);
		}
		pGrid->Invalidate();
	}
}

void CBCLeakyPropertyPage2::OnCheckChangedSingle(NMHDR *pNotifyStruct, LRESULT *result)
{
	static const int COL_VALUE       = 1;
	static const int COL_INTERPOLATE = 2;
	static const int COL_DIRECTION   = 3;
	static const int COL_VALUE_1     = 4;
	static const int COL_DISTANCE_1  = 5;
	static const int COL_VALUE_2     = 6;
	static const int COL_DISTANCE_2  = 7;

	CModGridCtrlEx* pGrid = &this->m_gridSingle;

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	if (pnmgv->iColumn == COL_INTERPOLATE)
	{
		ASSERT(pGrid->IsCheckMarkCell(pnmgv->iRow, pnmgv->iColumn));
		if (pGrid->GetCheck(pnmgv->iRow, pnmgv->iColumn) == BST_CHECKED)
		{
			// disable
			pGrid->DisableCell(pnmgv->iRow, COL_VALUE);
			// enable
			pGrid->EnableCell(pnmgv->iRow,  COL_DIRECTION);
			pGrid->EnableCell(pnmgv->iRow,  COL_VALUE_1);
			pGrid->EnableCell(pnmgv->iRow,  COL_DISTANCE_1);
			pGrid->EnableCell(pnmgv->iRow,  COL_VALUE_2);
			pGrid->EnableCell(pnmgv->iRow,  COL_DISTANCE_2);
		}
		else
		{
			// enable
			pGrid->EnableCell(pnmgv->iRow, COL_VALUE);
			// disable
			pGrid->DisableCell(pnmgv->iRow,  COL_DIRECTION);
			pGrid->DisableCell(pnmgv->iRow,  COL_VALUE_1);
			pGrid->DisableCell(pnmgv->iRow,  COL_DISTANCE_1);
			pGrid->DisableCell(pnmgv->iRow,  COL_VALUE_2);
			pGrid->DisableCell(pnmgv->iRow,  COL_DISTANCE_2);
		}
		pGrid->Invalidate();
	}
}

