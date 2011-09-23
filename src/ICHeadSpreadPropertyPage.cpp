// ICHeadSpreadPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ICHeadSpreadPropertyPage.h"

#include "NewModelWizard.h"

#include "property.h"
#include "Global.h"

// CICHeadSpreadPropertyPage dialog
const TCHAR PSZ_HEAD[] = _T("Head");

const TCHAR HEADING_VALUE[]          = _T("Value");
const TCHAR HEADING_INTERPOLATE[]    = _T("Interpolate");
const TCHAR HEADING_DIRECTION[]      = _T("Direction");
const TCHAR HEADING_VALUE_1[]        = _T("Value 1");
const TCHAR HEADING_DISTANCE_1[]     = _T("Distance 1");
const TCHAR HEADING_VALUE_2[]        = _T("Value 2");
const TCHAR HEADING_DISTANCE_2[]     = _T("Distance 2");

IMPLEMENT_DYNAMIC(CICHeadSpreadPropertyPage, CPropertyPage)
CICHeadSpreadPropertyPage::CICHeadSpreadPropertyPage()
	: CPropertyPage(CICHeadSpreadPropertyPage::IDD)
{
	this->CommonConstruct();
}

void CICHeadSpreadPropertyPage::CommonConstruct(void)
{
	// fill empty properties
	//
	this->SetProperties(this->m_headIC);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sDescriptionRTF, IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sHeadRTF,        IDR_IC_HEAD_HEAD_RTF);
}


CICHeadSpreadPropertyPage::~CICHeadSpreadPropertyPage()
{
}

void CICHeadSpreadPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_GridControl(pDX, IDC_HEADIC_GRID, m_gridHeadIC);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	if (this->m_bFirstSetActive)
	{
		this->SetupGrids();

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sHeadRTF.c_str());
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

	int nRow = 0;

	// PSZ_SOLUTION
	CGlobal::DDX_Property(pDX, IDC_HEADIC_GRID, ++nRow, this->m_headIC.head, false);
}


BEGIN_MESSAGE_MAP(CICHeadSpreadPropertyPage, CPropertyPage)
	ON_NOTIFY(GVN_CHECKCHANGED, IDC_HEADIC_GRID, OnCheckChangedHeadIC)
	ON_NOTIFY(GVN_SELCHANGED, IDC_HEADIC_GRID, OnSelChangedHeadIC)
	ON_EN_SETFOCUS(IDC_DESC_EDIT, OnEnSetfocusDescEdit)
	ON_NOTIFY(GVN_SETFOCUS, IDC_HEADIC_GRID, OnSelChangedHeadIC)
END_MESSAGE_MAP()


// CICHeadSpreadPropertyPage message handlers

BOOL CICHeadSpreadPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CICHeadSpreadPropertyPage::GetProperties(CHeadIC& r_headIC)const
{
	r_headIC = this->m_headIC;

	// delete inactive properties
	//
	if (r_headIC.head->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&r_headIC.head, 0);
	}
}

void CICHeadSpreadPropertyPage::SetProperties(const CHeadIC& r_headIC)
{
	// copy headIC
	this->m_headIC = r_headIC;
	
	// fill empty properties
	//
	if (this->m_headIC.head == 0)
	{
		this->m_headIC.head = new Cproperty();
	}
}

BOOL CICHeadSpreadPropertyPage::SetupGrids(void)
{
	std::vector<CString> vecDirection;
	vecDirection.push_back(_T("X"));
	vecDirection.push_back(_T("Y"));
	vecDirection.push_back(_T("Z"));

	try
	{
		this->m_gridHeadIC.SetRowCount(2);
		this->m_gridHeadIC.SetColumnCount(8);
		VERIFY(this->m_gridHeadIC.SetFixedRowCount(1) == TRUE);
		this->m_gridHeadIC.SetFixedColumnCount(1);
		this->m_gridHeadIC.EnableTitleTips(FALSE);
		this->m_gridHeadIC.SetRowResize(FALSE);
		this->m_gridHeadIC.SetCurrentFocusCell(1, 1);

		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask    = GVIF_FORMAT;
		defaultFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;
		for (int row = 0; row < this->m_gridHeadIC.GetRowCount(); ++row)
		{
			defaultFormat.row = row;
			for (int col = 0; col < this->m_gridHeadIC.GetColumnCount(); ++col)
			{ 
				defaultFormat.col = col;
				this->m_gridHeadIC.SetItem(&defaultFormat);
			}
		}

		int nRow = 0;
		this->m_gridHeadIC.SetItemText(++nRow, 0, PSZ_HEAD);

		int nCol = 0;
		this->m_gridHeadIC.SetItemText(0, ++nCol, HEADING_VALUE);
		this->m_gridHeadIC.SetItemText(0, ++nCol, HEADING_INTERPOLATE);  this->m_gridHeadIC.SetColumnCheck(nCol, BST_UNCHECKED);
		this->m_gridHeadIC.SetItemText(0, ++nCol, HEADING_DIRECTION);    this->m_gridHeadIC.SetColumnOptions(nCol, vecDirection);
		this->m_gridHeadIC.SetItemText(0, ++nCol, HEADING_VALUE_1);
		this->m_gridHeadIC.SetItemText(0, ++nCol, HEADING_DISTANCE_1);
		this->m_gridHeadIC.SetItemText(0, ++nCol, HEADING_VALUE_2);
		this->m_gridHeadIC.SetItemText(0, ++nCol, HEADING_DISTANCE_2);

		this->m_gridHeadIC.SetColumnWidth(0, 140);
	}
	catch (CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return TRUE;
}


void CICHeadSpreadPropertyPage::OnCheckChangedHeadIC(NMHDR *pNotifyStruct, LRESULT *result)
{
	static const int COL_VALUE       = 1;
	static const int COL_INTERPOLATE = 2;
	static const int COL_DIRECTION   = 3;
	static const int COL_VALUE_1     = 4;
	static const int COL_DISTANCE_1  = 5;
	static const int COL_VALUE_2     = 6;
	static const int COL_DISTANCE_2  = 7;

	CModGridCtrlEx* pGrid = &this->m_gridHeadIC;

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

void CICHeadSpreadPropertyPage::OnSelChangedHeadIC(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("OnSelChangedChemIC\n");
	CCellID focus = this->m_gridHeadIC.GetFocusCell();

	if (!this->m_gridHeadIC.IsValid(focus)) return;
	
	CString strItem = this->m_gridHeadIC.GetItemText(focus.row, 0);

	// PSZ_HEAD
	if (strItem.Compare(PSZ_HEAD) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sHeadRTF.c_str());
	}
}

BOOL CICHeadSpreadPropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (pSheet->IsKindOf(RUNTIME_CLASS(CNewModelWizard)))
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
		}
		else
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
		}
	}
	return bRet;
}

BOOL CICHeadSpreadPropertyPage::OnKillActive()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
}

LRESULT CICHeadSpreadPropertyPage::OnWizardNext()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (!this->UpdateData(TRUE))
		{
			return -1; // return –1 to prevent the page from changing 
		}
	}
	return CPropertyPage::OnWizardNext();
}

void CICHeadSpreadPropertyPage::OnEnSetfocusDescEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
}
