// BCSpecifiedHeadPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "BCSpecifiedHeadPropertyPage.h"

#define strText szText

#include "Global.h"

// CBCSpecifiedHeadPropertyPage dialog

IMPLEMENT_DYNAMIC(CBCSpecifiedHeadPropertyPage, baseCBCSpecifiedHeadPropertyPage)
CBCSpecifiedHeadPropertyPage::CBCSpecifiedHeadPropertyPage()
	: baseCBCSpecifiedHeadPropertyPage(CBCSpecifiedHeadPropertyPage::IDD)
{
	this->SetFlowOnly(false);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,   IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sHeadRTF,          IDR_BC_SPECIFIED_HEAD_RTF);
	CGlobal::LoadRTFString(this->m_sSolutionAssocRTF, IDR_BC_SPECIFIED_SOL_ASSOC_RTF);
	CGlobal::LoadRTFString(this->m_sSolutionFixRTF,   IDR_BC_SPECIFIED_SOL_FIX_RTF);	
	CGlobal::LoadRTFString(this->m_sSolTypeRTF,       IDR_BC_SOL_TYPE_RTF);
}

CBCSpecifiedHeadPropertyPage::~CBCSpecifiedHeadPropertyPage()
{
}

void CBCSpecifiedHeadPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	baseCBCSpecifiedHeadPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);
	DDX_GridControl(pDX, IDC_HEAD_GRID, m_gridHead);
	DDX_GridControl(pDX, IDC_SOLUTION_GRID, m_gridSolution);

	if (this->m_bFirstSetActive)
	{
		this->SetupGrids();

		// head
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_HEAD_GRID, this->m_bc.m_bc_head);

		// solution
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_SOLUTION_GRID, this->m_bc.m_bc_solution);

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
	}

	// description
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		::DDX_Text(pDX, IDC_DESC_EDIT, str);
		this->m_desc = str;
	}
	else
	{
		CString str(this->m_desc.c_str());
		::DDX_Text(pDX, IDC_DESC_EDIT, str);
	}

	if (pDX->m_bSaveAndValidate)
	{
		CBC bc;

		// bc_type
		bc.bc_type = BC_info::BC_SPECIFIED;

		// head time series
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_HEAD_GRID, bc.m_bc_head);

		// solution time series
		//
		CGlobal::DDX_GridTimeSeries(pDX, IDC_SOLUTION_GRID, bc.m_bc_solution, !this->m_bFlowOnly);

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
	else
	{
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
	}
}


BEGIN_MESSAGE_MAP(CBCSpecifiedHeadPropertyPage, baseCBCSpecifiedHeadPropertyPage)
	ON_NOTIFY(GVN_CHECKCHANGED, IDC_HEAD_GRID, OnCheckChangedHead)
	ON_NOTIFY(GVN_CHECKCHANGED, IDC_SOLUTION_GRID, OnCheckChangedSolution)
	ON_BN_CLICKED(IDC_FIXED_RADIO, OnBnClickedFixedRadio)
	ON_BN_CLICKED(IDC_ASSOC_RADIO, OnBnClickedAssocRadio)
	ON_EN_SETFOCUS(IDC_DESC_EDIT, OnEnSetfocusDescEdit)
	ON_BN_SETFOCUS(IDC_ASSOC_RADIO, OnBnSetfocusAssocRadio)
	ON_BN_SETFOCUS(IDC_FIXED_RADIO, OnBnSetfocusFixedRadio)
	ON_NOTIFY(GVN_SELCHANGED, IDC_HEAD_GRID, OnSelChangedHead)
	ON_NOTIFY(GVN_SETFOCUS, IDC_HEAD_GRID, OnSelChangedHead)
	ON_NOTIFY(GVN_SELCHANGED, IDC_SOLUTION_GRID, OnSelChangedSolution)
	ON_NOTIFY(GVN_SETFOCUS, IDC_SOLUTION_GRID, OnSelChangedSolution)
END_MESSAGE_MAP()


// CBCSpecifiedHeadPropertyPage message handlers

BOOL CBCSpecifiedHeadPropertyPage::OnInitDialog()
{
	baseCBCSpecifiedHeadPropertyPage::OnInitDialog();

	// Add extra initialization here

	// Layout controls
	this->CreateRoot(VERTICAL)
		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
			<< item(IDC_DESC_STATIC, NORESIZE | ALIGN_VCENTER, 0, 0, 0, 0)
			<< itemFixed(HORIZONTAL, 8)
			<< item(IDC_DESC_EDIT, ABSOLUTE_VERT | ALIGN_VCENTER, 0, 0, 0, 0)
			)
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

void CBCSpecifiedHeadPropertyPage::OnCheckChangedHead(NMHDR *pNotifyStruct, LRESULT *result)
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

void CBCSpecifiedHeadPropertyPage::OnCheckChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
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
		this->m_gridHead.SetCurrentFocusCell(1, 2);

		this->m_gridSolution.SetRowCount(nSolnRows);
		this->m_gridSolution.SetColumnCount(9);
		this->m_gridSolution.SetFixedRowCount(1);
		this->m_gridSolution.SetFixedColumnCount(0);
		this->m_gridSolution.EnableTitleTips(FALSE);
		this->m_gridSolution.SetCurrentFocusCell(1, 2);

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

	Item.col = 3;
	Item.strText = _T("Interpolate");
	this->m_gridHead.SetItem(&Item);
	this->m_gridHead.SetColumnCheck(Item.col, BST_UNCHECKED);
	this->m_gridSolution.SetItem(&Item);
	this->m_gridSolution.SetColumnCheck(Item.col, BST_UNCHECKED);

	Item.col = 4;
	Item.strText = _T("Direction");
	this->m_gridHead.SetItem(&Item);
	this->m_gridHead.SetColumnOptions(Item.col, vecDirection);
	this->m_gridSolution.SetItem(&Item);
	this->m_gridSolution.SetColumnOptions(Item.col, vecDirection);

	Item.col = 5;
	Item.strText = _T("Value 1");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 6;
	Item.strText = _T("Distance 1");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 7;
	Item.strText = _T("Value 2");
	this->m_gridHead.SetItem(&Item);
	this->m_gridSolution.SetItem(&Item);

	Item.col = 8;
	Item.strText = _T("Distance 2");
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

BOOL CBCSpecifiedHeadPropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();

	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	}
	return bRet;
}

BOOL CBCSpecifiedHeadPropertyPage::OnKillActive()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
}

void CBCSpecifiedHeadPropertyPage::OnBnClickedFixedRadio()
{
	if (this->IsDlgButtonChecked(IDC_FIXED_RADIO))
	{
		this->m_bc.bc_solution_type = FIXED;
	}
	if (this->IsDlgButtonChecked(IDC_ASSOC_RADIO))
	{
		this->m_bc.bc_solution_type = ASSOCIATED;
	}
	this->OnSelChangedSolution(NULL, NULL);
}

void CBCSpecifiedHeadPropertyPage::OnBnClickedAssocRadio()
{
	if (this->IsDlgButtonChecked(IDC_FIXED_RADIO))
	{
		this->m_bc.bc_solution_type = FIXED;
	}
	if (this->IsDlgButtonChecked(IDC_ASSOC_RADIO))
	{
		this->m_bc.bc_solution_type = ASSOCIATED;
	}
	this->OnSelChangedSolution(NULL, NULL);
}

void CBCSpecifiedHeadPropertyPage::OnEnSetfocusDescEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
}

void CBCSpecifiedHeadPropertyPage::OnBnSetfocusAssocRadio()
{
	///this->m_wndRichEditCtrl.SetWindowText(this->m_sSolTypeRTF.c_str());
}

void CBCSpecifiedHeadPropertyPage::OnBnSetfocusFixedRadio()
{
	///this->m_wndRichEditCtrl.SetWindowText(this->m_sSolTypeRTF.c_str());
}

void CBCSpecifiedHeadPropertyPage::OnSelChangedHead(NMHDR *pNotifyStruct, LRESULT *result)
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sHeadRTF.c_str());
}

void CBCSpecifiedHeadPropertyPage::OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	if (this->GetCheckedRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO) == IDC_ASSOC_RADIO)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sSolutionAssocRTF.c_str());
	}
	else if (this->GetCheckedRadioButton(IDC_ASSOC_RADIO, IDC_FIXED_RADIO) == IDC_FIXED_RADIO)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sSolutionFixRTF.c_str());
	}
	else
	{
		ASSERT(FALSE);
	}
}
