// BCFluxPropertyPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "BCFluxPropertyPage2.h"

#include "Global.h"

// CBCFluxPropertyPage2 dialog

IMPLEMENT_DYNAMIC(CBCFluxPropertyPage2, baseCBCFluxPropertyPage2)
CBCFluxPropertyPage2::CBCFluxPropertyPage2()
	: baseCBCFluxPropertyPage2(CBCFluxPropertyPage2::IDD)
{
}

CBCFluxPropertyPage2::~CBCFluxPropertyPage2()
{
}

void CBCFluxPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	baseCBCFluxPropertyPage2::DoDataExchange(pDX);

	DDX_GridControl(pDX, IDC_FLUX_GRID, m_gridFlux);
	DDX_GridControl(pDX, IDC_SOLUTION_GRID, m_gridSolution);

	if (this->m_bFirstSetActive)
	{
		this->SetupGrids();

		// flux
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_FLUX_GRID, this->m_bc.m_bc_flux);

		// solution
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_SOLUTION_GRID, this->m_bc.m_bc_solution);

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
		bc.bc_type = SPECIFIED;

		// flux time series
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_FLUX_GRID, bc.m_bc_flux);

		// solution time series
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_SOLUTION_GRID, bc.m_bc_solution);

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

BOOL CBCFluxPropertyPage2::SetupGrids(void)
{
	const int MIN_ROW_COUNT = 100;

	int nFluxRows = (int)this->m_bc.m_bc_flux.size() + MIN_ROW_COUNT;
	int nSolnRows = (int)this->m_bc.m_bc_solution.size() + MIN_ROW_COUNT;

	try
	{
		this->m_gridFlux.SetRowCount(nFluxRows);
		this->m_gridFlux.SetColumnCount(9);
		this->m_gridFlux.SetFixedRowCount(1);
		this->m_gridFlux.SetFixedColumnCount(0);
		this->m_gridFlux.EnableTitleTips(FALSE);

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
	for (int row = 0; row < nFluxRows; ++row)
	{
		defaultFormat.row = row;
		for (int col = 0; col < this->m_gridFlux.GetColumnCount(); ++col)
		{ 
			defaultFormat.col = col;
			this->m_gridFlux.SetItem(&defaultFormat);
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
	this->m_gridFlux.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 1;
	Item.szText = _T("Units");
	this->m_gridFlux.SetItem(&Item);
	this->m_gridFlux.SetColumnOptions(Item.col, vecTimeUnits);
	this->m_gridSolution.SetItem(&Item);
	this->m_gridSolution.SetColumnOptions(Item.col, vecTimeUnits);

	Item.col = 2;
	Item.szText = _T("Flux");
	this->m_gridFlux.SetItem(&Item);
	Item.szText = _T("Solution");
	this->m_gridSolution.SetItem(&Item);

	Item.col = 3;
	Item.szText = _T("Interpolate");
	this->m_gridFlux.SetItem(&Item);
	this->m_gridFlux.SetColumnCheck(Item.col, BST_UNCHECKED);
	this->m_gridSolution.SetItem(&Item);
	this->m_gridSolution.SetColumnCheck(Item.col, BST_UNCHECKED);

	Item.col = 4;
	Item.szText = _T("Direction");
	this->m_gridFlux.SetItem(&Item);
	this->m_gridFlux.SetColumnOptions(Item.col, vecDirection);
	this->m_gridSolution.SetItem(&Item);
	this->m_gridSolution.SetColumnOptions(Item.col, vecDirection);

	Item.col = 5;
	Item.szText = _T("Value 1");
	this->m_gridFlux.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 6;
	Item.szText = _T("Distance 1");
	this->m_gridFlux.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 7;
	Item.szText = _T("Value 2");
	this->m_gridFlux.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 8;
	Item.szText = _T("Distance 2");
	this->m_gridFlux.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	this->m_gridFlux.SetItemText(1, 0, _T("0"));
	this->m_gridFlux.DisableCell(1, 0);
	this->m_gridSolution.SetItemText(1, 0, _T("0"));
	this->m_gridSolution.DisableCell(1, 0);

	this->m_gridFlux.DisableCell(1, 1);
	this->m_gridSolution.DisableCell(1, 1);

	for (int iRow = this->m_gridFlux.GetFixedRowCount(); iRow < this->m_gridFlux.GetRowCount(); ++iRow)
	{
		for (int iCol = 4; iCol < 9; ++iCol)
		{
			this->m_gridFlux.DisableCell(iRow, iCol);
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



BEGIN_MESSAGE_MAP(CBCFluxPropertyPage2, baseCBCFluxPropertyPage2)
	ON_NOTIFY(GVN_ITEMCHANGED, IDC_FLUX_GRID, OnItemChangedFlux)
	ON_NOTIFY(GVN_ITEMCHANGED, IDC_SOLUTION_GRID, OnItemChangedSolution)
END_MESSAGE_MAP()


// CBCFluxPropertyPage2 message handlers

BOOL CBCFluxPropertyPage2::OnInitDialog()
{
	baseCBCFluxPropertyPage2::OnInitDialog();

	// Add extra initialization here

	// Layout controls
	this->CreateRoot(VERTICAL)
		<< item(IDC_FLUX_GRID, GREEDY)
		<< item(IDC_SOLUTION_GRID, GREEDY)
		<< itemFixed(VERTICAL, 3)
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

void CBCFluxPropertyPage2::SetProperties(const CBC& r_bc)
{
	this->m_bc = r_bc;
}

void CBCFluxPropertyPage2::GetProperties(CBC& r_bc)
{
	r_bc = this->m_bc;
}

void CBCFluxPropertyPage2::OnItemChangedFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	if (pnmgv->iColumn == 3)
	{
		ASSERT(this->m_gridFlux.IsCheckMarkCell(pnmgv->iRow, pnmgv->iColumn));
		if (this->m_gridFlux.GetCheck(pnmgv->iRow, pnmgv->iColumn) == BST_CHECKED)
		{
			this->m_gridFlux.DisableCell(pnmgv->iRow, 2);
			for (int col = 4; col < 9; ++col)
			{
				this->m_gridFlux.EnableCell(pnmgv->iRow, col);
			}
		}
		else
		{
			this->m_gridFlux.EnableCell(pnmgv->iRow, 2);
			for (int col = 4; col < 9; ++col)
			{
				this->m_gridFlux.DisableCell(pnmgv->iRow, col);
			}
		}
		this->m_gridFlux.Invalidate();
	}
}

void CBCFluxPropertyPage2::OnItemChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
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
