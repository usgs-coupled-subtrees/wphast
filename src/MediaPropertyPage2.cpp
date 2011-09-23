// MediaPropertyPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "MediaPropertyPage2.h"

#include "Global.h"
#include "property.h"
#include "NewModelWizard.h"

const TCHAR PSZ_ACTIVE[]           = _T("Active");
const TCHAR PSZ_KX[]               = _T("Kx");
const TCHAR PSZ_KY[]               = _T("Ky");
const TCHAR PSZ_KZ[]               = _T("Kz");
const TCHAR PSZ_POROSITY[]         = _T("Porosity");
const TCHAR PSZ_STORAGE[]          = _T("Specific storage");
const TCHAR PSZ_ALPHA_LONG[]       = _T("Long dispersivity");
const TCHAR PSZ_ALPHA_HORIZONTAL[] = _T("Horizonal trans dispersivity");
const TCHAR PSZ_ALPHA_VERTICAL[]   = _T("Vertical trans dispersivity");
const TCHAR PSZ_TORTUOSITY[]       = _T("Tortuosity");

// CMediaPropertyPage2 dialog

IMPLEMENT_DYNAMIC(CMediaPropertyPage2, CPropertyPage)

CMediaPropertyPage2::CMediaPropertyPage2()
: CPropertyPage(CMediaPropertyPage2::IDD)
{
	this->CommonConstruct();
}

CMediaPropertyPage2::CMediaPropertyPage2(UINT nIDCaption)
: CPropertyPage(CMediaPropertyPage2::IDD, nIDCaption)
{
	this->CommonConstruct();
}

CMediaPropertyPage2::CMediaPropertyPage2(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
: CPropertyPage(CMediaPropertyPage2::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
{
	this->CommonConstruct();
}

CMediaPropertyPage2::CMediaPropertyPage2(LPCTSTR lpszDummy, UINT nIDTemplate)
: CPropertyPage(nIDTemplate)
{
	this->CommonConstruct();
}

CMediaPropertyPage2::~CMediaPropertyPage2()
{
}

void CMediaPropertyPage2::CommonConstruct(void)
{
	// this may be overridden in OnSetActive
	this->FlowOnly            = false;
	this->FirstPaint          = true;

	// 
	this->SetFlowOnlyWizard(false);
	this->SetNewModelWizard(false);
	this->SetShowFlowOnlyMessage(false);

	// init properties
	//
	this->SetProperties(this->GridElt);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->ActiveRTF,            IDR_MEDIA_ACTIVE_RTF);
	CGlobal::LoadRTFString(this->KxRTF,                IDR_MEDIA_KX_RTF);
	CGlobal::LoadRTFString(this->KyRTF,                IDR_MEDIA_KY_RTF);
	CGlobal::LoadRTFString(this->KzRTF,                IDR_MEDIA_KZ_RTF);
	CGlobal::LoadRTFString(this->LongDispRTF,          IDR_MEDIA_LONG_DISP_RTF);
	CGlobal::LoadRTFString(this->PorosityRTF,          IDR_MEDIA_POROSITY_RTF);
	CGlobal::LoadRTFString(this->SpecStorageRTF,       IDR_MEDIA_SPEC_STORAGE_RTF);
	CGlobal::LoadRTFString(this->TransDispRTF,         IDR_MEDIA_TRANS_DISP_RTF);
	CGlobal::LoadRTFString(this->AlphaHorizontalRTF,   IDR_MEDIA_ALPHA_HORZ_RTF);
	CGlobal::LoadRTFString(this->AlphaVerticalRTF,     IDR_MEDIA_ALPHA_VERT_RTF);
	CGlobal::LoadRTFString(this->TortuosityRTF,        IDR_MEDIA_TORTUOSITY_RTF);
}

void CMediaPropertyPage2::GetProperties(CGridElt& gridElt)const
{
	gridElt = this->GridElt;

	// delete inactive properties
	//
	if (gridElt.active->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&gridElt.active, 0);
	}
	if (gridElt.alpha_long->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&gridElt.alpha_long, 0);
	}
	if (gridElt.alpha_horizontal->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&gridElt.alpha_horizontal, 0);
	}
	if (gridElt.alpha_vertical->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&gridElt.alpha_vertical, 0);
	}
	if (gridElt.kx->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&gridElt.kx, 0);
	}
	if (gridElt.ky->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&gridElt.ky, 0);
	}
	if (gridElt.kz->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&gridElt.kz, 0);
	}
	if (gridElt.porosity->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&gridElt.porosity, 0);
	}
	if (gridElt.storage->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&gridElt.storage, 0);
	}
}

void CMediaPropertyPage2::SetProperties(const CGridElt& gridElt)
{
	// copy gridElt
	this->GridElt = gridElt;
	
	// fill empty properties
	//
	if (this->GridElt.active == NULL)
	{
		this->GridElt.active = new Cproperty();
	}
	if (this->GridElt.alpha_long == NULL)
	{
		this->GridElt.alpha_long = new Cproperty();
	}
	if (this->GridElt.alpha_horizontal == NULL)
	{
		this->GridElt.alpha_horizontal = new Cproperty();
	}
	if (this->GridElt.alpha_vertical == NULL)
	{
		this->GridElt.alpha_vertical = new Cproperty();
	}
	if (this->GridElt.kx == NULL)
	{
		this->GridElt.kx = new Cproperty();
	}
	if (this->GridElt.ky == NULL)
	{
		this->GridElt.ky = new Cproperty();
	}
	if (this->GridElt.kz == NULL)
	{
		this->GridElt.kz = new Cproperty();
	}
	if (this->GridElt.porosity == NULL)
	{
		this->GridElt.porosity = new Cproperty();
	}
	if (this->GridElt.storage == NULL)
	{
		this->GridElt.storage = new Cproperty();
	}
	if (this->GridElt.tortuosity == NULL)
	{
		this->GridElt.tortuosity = new Cproperty();
	}
}

void CMediaPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	::DDX_GridControl(pDX, IDC_GRID, this->Grid);
	::DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	this->RichEditCtrl.SetTargetDevice(NULL, 0);

	// Prepare Grid
	//
	if (!pDX->m_bSaveAndValidate && this->Grid.GetColumnCount() == 0)
	{
		TRY
		{
			this->Grid.SetRowCount(11);
			this->Grid.SetColumnCount(3);
			this->Grid.SetFixedRowCount(1);
			this->Grid.SetFixedColumnCount(1);
			this->Grid.EnableTitleTips(FALSE);
			this->Grid.SetHighLight(GV_HIGHLIGHT_ALWAYS);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;

		Item.row = 0;
		Item.col = 1;
		Item.szText.Format(_T("Value"));
		VERIFY(this->Grid.SetItem(&Item));

		Item.row = 0;
		Item.col = 2;
		Item.szText.Format(_T("Units"));
		VERIFY(this->Grid.SetItem(&Item));

		Item.col = 0;

		Item.row = 1;
		Item.szText.Format(PSZ_ACTIVE);
		VERIFY(this->Grid.SetItem(&Item));

		++Item.row;
		Item.szText.Format(PSZ_KX);
		VERIFY(this->Grid.SetItem(&Item));

		++Item.row;
		Item.szText.Format(PSZ_KY);
		VERIFY(this->Grid.SetItem(&Item));

		++Item.row;
		Item.szText.Format(PSZ_KZ);
		VERIFY(this->Grid.SetItem(&Item));

		++Item.row;
		Item.szText.Format(PSZ_POROSITY);
		VERIFY(this->Grid.SetItem(&Item));

		++Item.row;
		Item.szText.Format(PSZ_STORAGE);
		VERIFY(this->Grid.SetItem(&Item));

		++Item.row;
		Item.szText.Format(PSZ_ALPHA_LONG);
		VERIFY(this->Grid.SetItem(&Item));

		++Item.row;
		Item.szText.Format(PSZ_ALPHA_HORIZONTAL);
		VERIFY(this->Grid.SetItem(&Item));

		++Item.row;
		Item.szText.Format(PSZ_ALPHA_VERTICAL);
		VERIFY(this->Grid.SetItem(&Item));

		++Item.row;
		Item.szText.Format(PSZ_TORTUOSITY);
		VERIFY(this->Grid.SetItem(&Item));


		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask    = GVIF_FORMAT;
		defaultFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;

		for (int row = 0; row < this->Grid.GetRowCount(); ++row)
		{
			defaultFormat.row = row;
			for (int col = 0; col < this->Grid.GetColumnCount(); ++col)
			{ 
				defaultFormat.col = col;
				VERIFY(this->Grid.SetItem(&defaultFormat));
			}
			VERIFY(this->Grid.SetRowHeight(row, 17));
		}

		VERIFY(this->Grid.SetColumnWidth(0, 179));
		VERIFY(this->Grid.SetColumnWidth(1, 111));
		VERIFY(this->Grid.SetColumnWidth(2, 143));

	    this->Grid.SetScrollRange(SB_VERT, 0, 0, TRUE); // reset vertical scrollbar

		// none active, porosity, tortuosity
		std::vector<CString> vecNone;
		vecNone.push_back("none");
		this->Grid.SetCellOptions(1,  2, vecNone);
		this->Grid.SetCellOptions(5,  2, vecNone);
		this->Grid.SetCellOptions(10, 2, vecNone);

		// hyd cond
		std::vector<CString> vecHydCondUnits;
		CGlobal::InsertL_T(vecHydCondUnits);
		this->Grid.SetCellOptions(2, 2, vecHydCondUnits);
		this->Grid.SetCellOptions(3, 2, vecHydCondUnits);
		this->Grid.SetCellOptions(4, 2, vecHydCondUnits);

		// specific storage
		std::vector<CString> vecStorageUnits;
		CGlobal::Insert1_L(vecStorageUnits);
		this->Grid.SetCellOptions(6, 2, vecStorageUnits);

		// dispersivities
		std::vector<CString> vecAlphaUnits;
		CGlobal::GetLengthUnits(vecAlphaUnits);
		this->Grid.SetCellOptions(7, 2, vecAlphaUnits);
		this->Grid.SetCellOptions(8, 2, vecAlphaUnits);
		this->Grid.SetCellOptions(9, 2, vecAlphaUnits);

		// desc
		this->RichEditCtrl.SetWindowText(this->ActiveRTF.c_str());
	}

	if (pDX->m_bSaveAndValidate)
	{
		for (int c = 0; c < this->Grid.GetColumnCount(); ++c)
		{
			TRACE("col width %d = %d\n", c, this->Grid.GetColumnWidth(c));
		}
		for (int r = 0; r < this->Grid.GetRowCount(); ++r)
		{
			TRACE("row height %d = %d\n", r, this->Grid.GetRowHeight(r));
		}
	}

	int row = 0;
	if (pDX->m_bSaveAndValidate)
	{
		std::string n;
		std::string k;
		std::string s;
		std::string a;

		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.active),           n, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.kx),               k, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.ky),               k, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.kz),               k, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.porosity),         n, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.storage),          s, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.alpha_long),       a, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.alpha_horizontal), a, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.alpha_vertical),   a, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.tortuosity),       n, false);

		VERIFY(this->Units.k.set_input(k.c_str()) == OK);
		VERIFY(this->Units.s.set_input(s.c_str()) == OK);
		VERIFY(this->Units.alpha.set_input(a.c_str()) == OK);

		CNewModelWizard *parent = (CNewModelWizard*)this->GetParent();
		ASSERT(parent->IsKindOf(RUNTIME_CLASS(CNewModelWizard)));
		parent->SetUnits(this->Units);
	}
	else
	{
		CNewModelWizard *parent = (CNewModelWizard*)this->GetParent();
		ASSERT(parent->IsKindOf(RUNTIME_CLASS(CNewModelWizard)));
		this->Units = parent->GetUnits();

		std::string n("none");
		std::string k = CGlobal::GetStdL_T(        this->Units.k.defined     ? this->Units.k.input     : this->Units.k.si);
		std::string s = CGlobal::GetStd1_L(        this->Units.s.defined     ? this->Units.s.input     : this->Units.s.si);
		std::string a = CGlobal::GetStdLengthUnits(this->Units.alpha.defined ? this->Units.alpha.input : this->Units.alpha.si);

		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.active),           n, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.kx),               k, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.ky),               k, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.kz),               k, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.porosity),         n, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.storage),          s, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.alpha_long),       a, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.alpha_horizontal), a, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.alpha_vertical),   a, false);
		CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->GridElt.tortuosity),       n, false);
	}
}


BEGIN_MESSAGE_MAP(CMediaPropertyPage2, CPropertyPage)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnSelChanged)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnEndLabelEdit)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CMediaPropertyPage2 message handlers
BOOL CMediaPropertyPage2::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	if (this->GetNewModelWizard())
	{
		CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
		ASSERT_KINDOF(CPropertySheet, pSheet);
		if (pSheet->IsWizard())
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);

			// add vertical and horiz transverse dipersivity if missing
			CNewModelWizard* pWizard = static_cast<CNewModelWizard*>(pSheet);
			CFlowOnly flowOnly = pWizard->GetFlowOnly();

			this->SetFlowOnly((bool)flowOnly);

			if (this->GetFlowOnly())
			{
				// TODO check for defaults and maybe delete
			}
			else 
			{
				CGridElt gridElt = CGridElt::NewDefaults(this->GetFlowOnly());
				if (this->GridElt.alpha_long->type == PROP_UNDEFINED)
				{
					Cproperty::CopyProperty(&this->GridElt.alpha_long, gridElt.alpha_long);
				}
				if (this->GridElt.alpha_horizontal->type == PROP_UNDEFINED)
				{
					Cproperty::CopyProperty(&this->GridElt.alpha_horizontal, gridElt.alpha_horizontal);
				}
				if (this->GridElt.alpha_vertical->type == PROP_UNDEFINED)
				{
					Cproperty::CopyProperty(&this->GridElt.alpha_vertical, gridElt.alpha_vertical);
				}
			}
		}
	}
	else if (this->GetFlowOnlyWizard())
	{
		CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
		ASSERT_KINDOF(CPropertySheet, pSheet);
		if (pSheet->IsWizard())
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
			this->SetFlowOnly(false);
		}
	}
	else
	{
		CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
		ASSERT_KINDOF(CPropertySheet, pSheet);
		if (pSheet->IsWizard())
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
		}
	}
	return bRet;
}

BOOL CMediaPropertyPage2::OnKillActive()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
}

LRESULT CMediaPropertyPage2::OnWizardNext()
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

void CMediaPropertyPage2::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// Add your message handler code here
	// Do not call CPropertyPage::OnPaint() for painting messages
	if (this->FirstPaint)
	{
		this->FirstPaint = false;
		if (this->GetShowFlowOnlyMessage())
		{
			::AfxMessageBox("Since you have turned on solute transport you may want to review the default dispersivities.");
			this->SetShowFlowOnlyMessage(false);
		}
	}
}

void CMediaPropertyPage2::OnSelChanged(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;

	switch (pnmgv->iRow)
	{
	case 1: // IDR_MEDIA_ACTIVE_RTF
		this->RichEditCtrl.SetWindowText(this->ActiveRTF.c_str());
		break;

	case 2: // IDR_MEDIA_KX_RTF
		this->RichEditCtrl.SetWindowText(this->KxRTF.c_str());
		break;

	case 3: // IDR_MEDIA_KY_RTF
		this->RichEditCtrl.SetWindowText(this->KyRTF.c_str());
		break;

	case 4: // IDR_MEDIA_KZ_RTF
		this->RichEditCtrl.SetWindowText(this->KzRTF.c_str());
		break;

	///case 5: // IDR_MEDIA_LONG_DISP_RTF
	///	this->RichEditCtrl.SetWindowText(this->LongDispRTF.c_str());
	///	break;

	case 5: // IDR_MEDIA_POROSITY_RTF
		this->RichEditCtrl.SetWindowText(this->PorosityRTF.c_str());
		break;

	case 6: // IDR_MEDIA_SPEC_STORAGE_RTF
		this->RichEditCtrl.SetWindowText(this->SpecStorageRTF.c_str());
		break;

	case 7: // IDR_MEDIA_SPEC_STORAGE_RTF
		this->RichEditCtrl.SetWindowText(this->TransDispRTF.c_str());
		break;

	case 8: // IDR_MEDIA_TRANS_DISP_RTF
		this->RichEditCtrl.SetWindowText(this->AlphaHorizontalRTF.c_str());
		break;

	case 9: // IDR_MEDIA_ALPHA_HORZ_RTF
		this->RichEditCtrl.SetWindowText(this->AlphaVerticalRTF.c_str());
		break;

	case 10: // PSZ_TORTUOSITY
		this->RichEditCtrl.SetWindowText(this->TortuosityRTF.c_str());
		break;

	default:
		ASSERT(FALSE);
		this->RichEditCtrl.SetWindowText("");
		break;
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CMediaPropertyPage2::OnEndLabelEdit(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;

	// hyd cond
	if (2 <= pnmgv->iRow && pnmgv->iRow <= 4)
	{
		for (int r = 2; r <= 4; ++r)
		{
			if (r != pnmgv->iRow)
			{
				this->Grid.SetItemText(r, 2, this->Grid.GetItemText(pnmgv->iRow, 2));
				this->Grid.RedrawCell(r, 2);
			}
		}
	}

	// dispersivities
	if (7 <= pnmgv->iRow && pnmgv->iRow <= 9)
	{
		for (int r = 7; r <= 9; ++r)
		{
			if (r != pnmgv->iRow)
			{
				this->Grid.SetItemText(r, 2, this->Grid.GetItemText(pnmgv->iRow, 2));
				this->Grid.RedrawCell(r, 2);
			}
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}
