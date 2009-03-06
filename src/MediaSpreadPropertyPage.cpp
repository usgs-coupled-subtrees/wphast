// MediaSpreadPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "MediaSpreadPropertyPage.h"

#include "NewModelWizard.h"

#include "Global.h"
#include "property.h"


// CMediaSpreadPropertyPage dialog

const TCHAR ACTIVE[]            = _T("Active");
const TCHAR KX[]                = _T("Kx");
const TCHAR KY[]                = _T("Ky");
const TCHAR KZ[]                = _T("Kz");
const TCHAR POROSITY[]          = _T("Porosity");
const TCHAR STORAGE[]           = _T("Specific storage");
const TCHAR ALPHA_LONG[]        = _T("Longitudinal dispersivity");
const TCHAR ALPHA_HORIZONTAL[]  = _T("Horizonal dispersivity");
const TCHAR ALPHA_VERTICAL[]    = _T("Vertical dispersivity");

const TCHAR HEADING_VALUE[]          = _T("Value");
const TCHAR HEADING_INTERPOLATE[]    = _T("Interpolate");
const TCHAR HEADING_DIRECTION[]      = _T("Direction");
const TCHAR HEADING_VALUE_1[]        = _T("Value 1");
const TCHAR HEADING_DISTANCE_1[]     = _T("Distance 1");
const TCHAR HEADING_VALUE_2[]        = _T("Value 2");
const TCHAR HEADING_DISTANCE_2[]     = _T("Distance 2");


IMPLEMENT_DYNAMIC(CMediaSpreadPropertyPage, CPropertyPage)
CMediaSpreadPropertyPage::CMediaSpreadPropertyPage()
	: CPropertyPage(CMediaSpreadPropertyPage::IDD)
{
	this->CommonConstruct();
}

void CMediaSpreadPropertyPage::CommonConstruct(void)
{
	this->SetFlowOnly(false);
	this->SetDefault(false);

	// init properties
	//
	this->SetProperties(this->m_gridElt);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,     IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sActiveRTF,          IDR_MEDIA_ACTIVE_RTF);
	CGlobal::LoadRTFString(this->m_sKxRTF,              IDR_MEDIA_KX_RTF);
	CGlobal::LoadRTFString(this->m_sKyRTF,              IDR_MEDIA_KY_RTF);
	CGlobal::LoadRTFString(this->m_sKzRTF,              IDR_MEDIA_KZ_RTF);
	CGlobal::LoadRTFString(this->m_sPorosityRTF,        IDR_MEDIA_POROSITY_RTF);
	CGlobal::LoadRTFString(this->m_sStorageRTF,         IDR_MEDIA_SPEC_STORAGE_RTF);
	CGlobal::LoadRTFString(this->m_sAlphaLongRTF,       IDR_MEDIA_LONG_DISP_RTF);
	CGlobal::LoadRTFString(this->m_sAlphaHorizontalRTF, IDR_MEDIA_ALPHA_HORZ_RTF);
	CGlobal::LoadRTFString(this->m_sAlphaVerticalRTF,   IDR_MEDIA_ALPHA_VERT_RTF);
}


CMediaSpreadPropertyPage::~CMediaSpreadPropertyPage()
{
}

void CMediaSpreadPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	static const int COL_VALUE       = 1;
	static const int COL_INTERPOLATE = 2;

	CPropertyPage::DoDataExchange(pDX);

	DDX_GridControl(pDX, IDC_MEDIA_GRID, m_gridMedia);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	if (this->m_bFirstSetActive)
	{
		this->SetupGrids();

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sActiveRTF.c_str());
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

	// ACTIVE
	CGlobal::DDX_Property(pDX, IDC_MEDIA_GRID, ++nRow, this->m_gridElt.active, false);
	if (this->m_bDefault && pDX->m_bSaveAndValidate && this->m_gridElt.active->type == PROP_UNDEFINED)
	{
		CString str;
		str.Format("A default \"%s\" must be defined.", ACTIVE);
		if (this->m_gridMedia.GetCheck(nRow, COL_INTERPOLATE) == BST_CHECKED)
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_INTERPOLATE, str);
		}
		else
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_VALUE, str);
		}
	}

	// KX
	CGlobal::DDX_Property(pDX, IDC_MEDIA_GRID, ++nRow, this->m_gridElt.kx, false);
	if (this->m_bDefault && pDX->m_bSaveAndValidate && this->m_gridElt.kx->type == PROP_UNDEFINED)
	{
		CString str;
		str.Format("A default \"%s\" must be defined.", KX);
		if (this->m_gridMedia.GetCheck(nRow, COL_INTERPOLATE) == BST_CHECKED)
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_INTERPOLATE, str);
		}
		else
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_VALUE, str);
		}
	}


	// KY
	CGlobal::DDX_Property(pDX, IDC_MEDIA_GRID, ++nRow, this->m_gridElt.ky, false);
	if (this->m_bDefault && pDX->m_bSaveAndValidate && this->m_gridElt.ky->type == PROP_UNDEFINED)
	{
		CString str;
		str.Format("A default \"%s\" must be defined.", KY);
		if (this->m_gridMedia.GetCheck(nRow, COL_INTERPOLATE) == BST_CHECKED)
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_INTERPOLATE, str);
		}
		else
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_VALUE, str);
		}
	}

	// KZ
	CGlobal::DDX_Property(pDX, IDC_MEDIA_GRID, ++nRow, this->m_gridElt.kz, false);
	if (this->m_bDefault && pDX->m_bSaveAndValidate && this->m_gridElt.kz->type == PROP_UNDEFINED)
	{
		CString str;
		str.Format("A default \"%s\" must be defined.", KZ);
		if (this->m_gridMedia.GetCheck(nRow, COL_INTERPOLATE) == BST_CHECKED)
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_INTERPOLATE, str);
		}
		else
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_VALUE, str);
		}
	}

	// POROSITY
	CGlobal::DDX_Property(pDX, IDC_MEDIA_GRID, ++nRow, this->m_gridElt.porosity, false);
	if (this->m_bDefault && pDX->m_bSaveAndValidate && this->m_gridElt.porosity->type == PROP_UNDEFINED)
	{
		CString str;
		str.Format("A default \"%s\" must be defined.", POROSITY);
		if (this->m_gridMedia.GetCheck(nRow, COL_INTERPOLATE) == BST_CHECKED)
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_INTERPOLATE, str);
		}
		else
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_VALUE, str);
		}
	}

	// STORAGE
	CGlobal::DDX_Property(pDX, IDC_MEDIA_GRID, ++nRow, this->m_gridElt.storage, false);
	if (this->m_bDefault && pDX->m_bSaveAndValidate && this->m_gridElt.storage->type == PROP_UNDEFINED)
	{
		CString str;
		str.Format("A default \"%s\" must be defined.", STORAGE);
		if (this->m_gridMedia.GetCheck(nRow, COL_INTERPOLATE) == BST_CHECKED)
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_INTERPOLATE, str);
		}
		else
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_VALUE, str);
		}
	}
	// ALPHA_LONG
	CGlobal::DDX_Property(pDX, IDC_MEDIA_GRID, ++nRow, this->m_gridElt.alpha_long, false);
	if (this->m_bDefault && !this->m_bFlowOnly && pDX->m_bSaveAndValidate && this->m_gridElt.alpha_long->type == PROP_UNDEFINED)
	{
		CString str;
		str.Format("A default \"%s\" must be defined.", ALPHA_LONG);
		if (this->m_gridMedia.GetCheck(nRow, COL_INTERPOLATE) == BST_CHECKED)
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_INTERPOLATE, str);
		}
		else
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_VALUE, str);
		}
	}

	// ALPHA_HORIZONTAL
	CGlobal::DDX_Property(pDX, IDC_MEDIA_GRID, ++nRow, this->m_gridElt.alpha_horizontal, false);
	if (this->m_bDefault && !this->m_bFlowOnly && pDX->m_bSaveAndValidate && this->m_gridElt.alpha_horizontal->type == PROP_UNDEFINED)
	{
		CString str;
		str.Format("A default \"%s\" must be defined.", ALPHA_HORIZONTAL);
		if (this->m_gridMedia.GetCheck(nRow, COL_INTERPOLATE) == BST_CHECKED)
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_INTERPOLATE, str);
		}
		else
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_VALUE, str);
		}
	}

	// ALPHA_VERTICAL
	CGlobal::DDX_Property(pDX, IDC_MEDIA_GRID, ++nRow, this->m_gridElt.alpha_vertical, false);
	if (this->m_bDefault && !this->m_bFlowOnly && pDX->m_bSaveAndValidate && this->m_gridElt.alpha_vertical->type == PROP_UNDEFINED)
	{
		CString str;
		str.Format("A default \"%s\" must be defined.", ALPHA_VERTICAL);
		if (this->m_gridMedia.GetCheck(nRow, COL_INTERPOLATE) == BST_CHECKED)
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_INTERPOLATE, str);
		}
		else
		{
			::DDX_GridControlFail(pDX, IDC_MEDIA_GRID, nRow, COL_VALUE, str);
		}
	}
}


BEGIN_MESSAGE_MAP(CMediaSpreadPropertyPage, CPropertyPage)
	ON_NOTIFY(GVN_CHECKCHANGED, IDC_MEDIA_GRID, OnCheckChangedMedia)
	ON_NOTIFY(GVN_SELCHANGED, IDC_MEDIA_GRID, OnSelChangedMedia)
	ON_EN_SETFOCUS(IDC_DESC_EDIT, OnEnSetfocusDescEdit)
END_MESSAGE_MAP()


// CMediaSpreadPropertyPage message handlers

BOOL CMediaSpreadPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMediaSpreadPropertyPage::SetProperties(const CGridElt& r_gridElt)
{
	this->m_gridElt = r_gridElt;

	// fill empty properties
	//
	if (this->m_gridElt.active == NULL)
	{
		this->m_gridElt.active = new Cproperty();
	}
	if (this->m_gridElt.alpha_long == NULL)
	{
		this->m_gridElt.alpha_long = new Cproperty();
	}
	if (this->m_gridElt.alpha_horizontal == NULL)
	{
		this->m_gridElt.alpha_horizontal = new Cproperty();
	}
	if (this->m_gridElt.alpha_vertical == NULL)
	{
		this->m_gridElt.alpha_vertical = new Cproperty();
	}
	if (this->m_gridElt.kx == NULL)
	{
		this->m_gridElt.kx = new Cproperty();
	}
	if (this->m_gridElt.ky == NULL)
	{
		this->m_gridElt.ky = new Cproperty();
	}
	if (this->m_gridElt.kz == NULL)
	{
		this->m_gridElt.kz = new Cproperty();
	}
	if (this->m_gridElt.porosity == NULL)
	{
		this->m_gridElt.porosity = new Cproperty();
	}
	if (this->m_gridElt.storage == NULL)
	{
		this->m_gridElt.storage = new Cproperty();
	}
}

void CMediaSpreadPropertyPage::GetProperties(CGridElt& r_gridElt)
{
	r_gridElt = this->m_gridElt;

	// delete inactive properties
	//
	if (r_gridElt.active->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&r_gridElt.active, 0);
	}
	if (r_gridElt.alpha_long->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&r_gridElt.alpha_long, 0);
	}
	if (r_gridElt.alpha_horizontal->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&r_gridElt.alpha_horizontal, 0);
	}
	if (r_gridElt.alpha_vertical->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&r_gridElt.alpha_vertical, 0);
	}
	if (r_gridElt.kx->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&r_gridElt.kx, 0);
	}
	if (r_gridElt.ky->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&r_gridElt.ky, 0);
	}
	if (r_gridElt.kz->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&r_gridElt.kz, 0);
	}
	if (r_gridElt.porosity->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&r_gridElt.porosity, 0);
	}
	if (r_gridElt.storage->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&r_gridElt.storage, 0);
	}

}

BOOL CMediaSpreadPropertyPage::SetupGrids(void)
{
	std::vector<LPCTSTR> vecDirection;
	vecDirection.push_back(_T("X"));
	vecDirection.push_back(_T("Y"));
	vecDirection.push_back(_T("Z"));

	try
	{
		this->m_gridMedia.SetRowCount(10);
		this->m_gridMedia.SetColumnCount(8);
		VERIFY(this->m_gridMedia.SetFixedRowCount(1) == TRUE);
		this->m_gridMedia.SetFixedColumnCount(1);
		this->m_gridMedia.EnableTitleTips(FALSE);
		this->m_gridMedia.SetRowResize(FALSE);
		this->m_gridMedia.SetCurrentFocusCell(1, 1);


		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask    = GVIF_FORMAT;
		defaultFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;
		for (int row = 0; row < this->m_gridMedia.GetRowCount(); ++row)
		{
			defaultFormat.row = row;
			for (int col = 0; col < this->m_gridMedia.GetColumnCount(); ++col)
			{ 
				defaultFormat.col = col;
				this->m_gridMedia.SetItem(&defaultFormat);
			}
		}

		int nRow = 0;
		this->m_gridMedia.SetItemText(++nRow, 0, ACTIVE);
		this->m_gridMedia.SetItemText(++nRow, 0, KX);
		this->m_gridMedia.SetItemText(++nRow, 0, KY);
		this->m_gridMedia.SetItemText(++nRow, 0, KZ);
		this->m_gridMedia.SetItemText(++nRow, 0, POROSITY);
		this->m_gridMedia.SetItemText(++nRow, 0, STORAGE);
		this->m_gridMedia.SetItemText(++nRow, 0, ALPHA_LONG);
		this->m_gridMedia.SetItemText(++nRow, 0, ALPHA_HORIZONTAL);
		this->m_gridMedia.SetItemText(++nRow, 0, ALPHA_VERTICAL);

		int nCol = 0;
		this->m_gridMedia.SetItemText(0, ++nCol, HEADING_VALUE);
		this->m_gridMedia.SetItemText(0, ++nCol, HEADING_INTERPOLATE);  this->m_gridMedia.SetColumnCheck(nCol, BST_UNCHECKED);
		this->m_gridMedia.SetItemText(0, ++nCol, HEADING_DIRECTION);    this->m_gridMedia.SetColumnOptions(nCol, vecDirection);
		this->m_gridMedia.SetItemText(0, ++nCol, HEADING_VALUE_1);
		this->m_gridMedia.SetItemText(0, ++nCol, HEADING_DISTANCE_1);
		this->m_gridMedia.SetItemText(0, ++nCol, HEADING_VALUE_2);
		this->m_gridMedia.SetItemText(0, ++nCol, HEADING_DISTANCE_2);

		this->m_gridMedia.SetColumnWidth(0, 140);
	}
	catch (CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return TRUE;
}

void CMediaSpreadPropertyPage::OnCheckChangedMedia(NMHDR *pNotifyStruct, LRESULT *result)
{
	static const int COL_VALUE       = 1;
	static const int COL_INTERPOLATE = 2;
	static const int COL_DIRECTION   = 3;
	static const int COL_VALUE_1     = 4;
	static const int COL_DISTANCE_1  = 5;
	static const int COL_VALUE_2     = 6;
	static const int COL_DISTANCE_2  = 7;

	CModGridCtrlEx* pGrid = &this->m_gridMedia;

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

void CMediaSpreadPropertyPage::OnSelChangedMedia(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("OnSelChangedMedia\n");
	CCellID focus = this->m_gridMedia.GetFocusCell();

	if (!this->m_gridMedia.IsValid(focus)) return;
	
	CString strItem = this->m_gridMedia.GetItemText(focus.row, 0);

	// ACTIVE
	if (strItem.Compare(ACTIVE) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sActiveRTF.c_str());
	}
	// KX
	else if (strItem.Compare(KX) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sKxRTF.c_str());
	}
	// KY
	else if (strItem.Compare(KY) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sKyRTF.c_str());
	}
	// KZ
	else if (strItem.Compare(KZ) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sKzRTF.c_str());
	}
	// ALPHA_LONG
	else if (strItem.Compare(ALPHA_LONG) == 0)
	{
		// this->m_wndRichEditCtrl.SetWindowText(this->m_sLongDispRTF.c_str());
		this->m_wndRichEditCtrl.SetWindowText(this->m_sAlphaLongRTF.c_str());
	}
	// POROSITY
	else if (strItem.Compare(POROSITY) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sPorosityRTF.c_str());
	}
	// STORAGE
	else if (strItem.Compare(STORAGE) == 0)
	{
		// this->m_wndRichEditCtrl.SetWindowText(this->m_sSpecStorageRTF.c_str());
		this->m_wndRichEditCtrl.SetWindowText(this->m_sStorageRTF.c_str());
	}
	// ALPHA_HORIZONTAL
	else if (strItem.Compare(ALPHA_HORIZONTAL) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sAlphaHorizontalRTF.c_str());
	}
	// ALPHA_VERTICAL
	else if (strItem.Compare(ALPHA_VERTICAL) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sAlphaVerticalRTF.c_str());
	}
	// ERROR
	else
	{
		ASSERT(FALSE);
		this->m_wndRichEditCtrl.SetWindowText("");
	}

}

BOOL CMediaSpreadPropertyPage::OnSetActive()
{
// COMMENT: {4/10/2006 9:40:04 PM}	// Add your specialized code here and/or call the base class
// COMMENT: {4/10/2006 9:40:04 PM}
// COMMENT: {4/10/2006 9:40:04 PM}	return CPropertyPage::OnSetActive();
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

BOOL CMediaSpreadPropertyPage::OnKillActive()
{
// COMMENT: {4/10/2006 9:41:33 PM}	// Add your specialized code here and/or call the base class
// COMMENT: {4/10/2006 9:41:33 PM}
// COMMENT: {4/10/2006 9:41:33 PM}	return CPropertyPage::OnKillActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
}

LRESULT CMediaSpreadPropertyPage::OnWizardNext()
{
// COMMENT: {4/10/2006 9:41:51 PM}	// Add your specialized code here and/or call the base class
// COMMENT: {4/10/2006 9:41:51 PM}
// COMMENT: {4/10/2006 9:41:51 PM}	return CPropertyPage::OnWizardNext();
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

void CMediaSpreadPropertyPage::OnEnSetfocusDescEdit()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
}
