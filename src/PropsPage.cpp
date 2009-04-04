// PropsPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropsPage.h"

#include "PropStruct.h"
#include "Global.h"


// CPropsPage dialog

const TCHAR ACTIVE[]            = _T("Active");
const TCHAR KX[]                = _T("Kx");
const TCHAR KY[]                = _T("Ky");
const TCHAR KZ[]                = _T("Kz");
const TCHAR POROSITY[]          = _T("Porosity");
const TCHAR STORAGE[]           = _T("Specific storage");
const TCHAR ALPHA_LONG[]        = _T("Longitudinal dispersivity");
const TCHAR ALPHA_HORIZONTAL[]  = _T("Horizontal dispersivity");
const TCHAR ALPHA_VERTICAL[]    = _T("Vertical dispersivity");

IMPLEMENT_DYNAMIC(CPropsPage, CResizablePage)

CPropsPage::CPropsPage()
: CResizablePage(CPropsPage::IDD)
, Splitter(2, SSP_VERT, 10, 5)
{
	this->CommonConstruct();
}

void CPropsPage::CommonConstruct(void)
{
	this->m_psp.dwFlags &= (~PSP_HASHELP); // remove help button

	this->SetFlowOnly(false);
	this->SetDefault(false);

	// init properties
	//
	this->SetProperties(this->GridElt);

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

CPropsPage::~CPropsPage()
{
}

void CPropsPage::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);
	CResizablePage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, RichEditCtrl);

	CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl();

	if (pTree && pTree->GetCount() == 0)
	{
		htiACTIVE           = pTree->InsertItem(ACTIVE,           TVI_ROOT, TVI_LAST);
		htiKX               = pTree->InsertItem(KX,               TVI_ROOT, TVI_LAST);
		htiKY               = pTree->InsertItem(KY,               TVI_ROOT, TVI_LAST);
		htiKZ               = pTree->InsertItem(KZ,               TVI_ROOT, TVI_LAST);
		htiPOROSITY         = pTree->InsertItem(POROSITY,         TVI_ROOT, TVI_LAST);
		htiSTORAGE          = pTree->InsertItem(STORAGE,          TVI_ROOT, TVI_LAST);
		htiALPHA_LONG       = pTree->InsertItem(ALPHA_LONG,       TVI_ROOT, TVI_LAST);
		htiALPHA_HORIZONTAL = pTree->InsertItem(ALPHA_HORIZONTAL, TVI_ROOT, TVI_LAST);
		htiALPHA_VERTICAL   = pTree->InsertItem(ALPHA_VERTICAL,   TVI_ROOT, TVI_LAST);

		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);
		this->RichEditCtrl.SetWindowText(this->m_sActiveRTF.c_str());

		this->ItemDDX = htiACTIVE;
		pTree->SelectItem(this->ItemDDX);
	}

	if (this->ItemDDX)
	{
		CString strItem = pTree->GetItemText(this->ItemDDX);

		std::vector<CPropertyPage*> pages;
		pages.push_back(&this->PropNone);
		pages.push_back(&this->PropConstant);
		pages.push_back(&this->PropLinear);
		pages.push_back(&this->PropPoints);
		pages.push_back(&this->PropXYZ);		

		// Active
		if (strItem.Compare(ACTIVE) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.active));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.active));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.active));
			props.push_back(static_cast<Cproperty*>(this->GridEltPoints.active));
			props.push_back(static_cast<Cproperty*>(this->GridEltXYZ.active));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
		// Kx
		else if (strItem.Compare(KX) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.kx));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.kx));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.kx));
			props.push_back(static_cast<Cproperty*>(this->GridEltPoints.kx));
			props.push_back(static_cast<Cproperty*>(this->GridEltXYZ.kx));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
		// Ky
		else if (strItem.Compare(KY) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.ky));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.ky));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.ky));
			props.push_back(static_cast<Cproperty*>(this->GridEltPoints.ky));
			props.push_back(static_cast<Cproperty*>(this->GridEltXYZ.ky));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
		// Kz
		else if (strItem.Compare(KZ) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.kz));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.kz));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.kz));
			props.push_back(static_cast<Cproperty*>(this->GridEltPoints.kz));
			props.push_back(static_cast<Cproperty*>(this->GridEltXYZ.kz));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
		else if (strItem.Compare(POROSITY) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.porosity));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.porosity));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.porosity));
			props.push_back(static_cast<Cproperty*>(this->GridEltPoints.porosity));
			props.push_back(static_cast<Cproperty*>(this->GridEltXYZ.porosity));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
		else if (strItem.Compare(STORAGE) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.storage));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.storage));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.storage));
			props.push_back(static_cast<Cproperty*>(this->GridEltPoints.storage));
			props.push_back(static_cast<Cproperty*>(this->GridEltXYZ.storage));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
		else if (strItem.Compare(ALPHA_LONG) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.alpha_long));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.alpha_long));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.alpha_long));
			props.push_back(static_cast<Cproperty*>(this->GridEltPoints.alpha_long));
			props.push_back(static_cast<Cproperty*>(this->GridEltXYZ.alpha_long));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
		else if (strItem.Compare(ALPHA_HORIZONTAL) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.alpha_horizontal));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.alpha_horizontal));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.alpha_horizontal));
			props.push_back(static_cast<Cproperty*>(this->GridEltPoints.alpha_horizontal));
			props.push_back(static_cast<Cproperty*>(this->GridEltXYZ.alpha_horizontal));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
		else if (strItem.Compare(ALPHA_VERTICAL) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.alpha_vertical));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.alpha_vertical));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.alpha_vertical));
			props.push_back(static_cast<Cproperty*>(this->GridEltPoints.alpha_vertical));
			props.push_back(static_cast<Cproperty*>(this->GridEltXYZ.alpha_vertical));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
// COMMENT: {4/2/2009 5:29:37 PM}		else
// COMMENT: {4/2/2009 5:29:37 PM}		{
// COMMENT: {4/2/2009 5:29:37 PM}			ASSERT(FALSE);
// COMMENT: {4/2/2009 5:29:37 PM}		}

		// ACTIVE
		if (this->bDefault && pDX->m_bSaveAndValidate && this->GridElt.active->type == PROP_UNDEFINED)
		{
			CString str;
			pTree->SelectItem(this->htiACTIVE);
			str.Format("A default \"%s\" must be defined.", ACTIVE);
			pDX->Fail();
		}
		// KX
		if (this->bDefault && pDX->m_bSaveAndValidate && this->GridElt.kx->type == PROP_UNDEFINED)
		{
			CString str;
			pTree->SelectItem(this->htiKX);
			str.Format("A default \"%s\" must be defined.", KX);
			::AfxMessageBox(str);
			pDX->Fail();
		}
		// KY
		if (this->bDefault && pDX->m_bSaveAndValidate && this->GridElt.ky->type == PROP_UNDEFINED)
		{
			CString str;
			pTree->SelectItem(this->htiKY);
			str.Format("A default \"%s\" must be defined.", KY);
			::AfxMessageBox(str);
			pDX->Fail();
		}
		// KZ
		if (this->bDefault && pDX->m_bSaveAndValidate && this->GridElt.kz->type == PROP_UNDEFINED)
		{
			CString str;
			pTree->SelectItem(this->htiKZ);
			str.Format("A default \"%s\" must be defined.", KZ);
			::AfxMessageBox(str);
			pDX->Fail();
		}
		// POROSITY
		if (this->bDefault && pDX->m_bSaveAndValidate && this->GridElt.porosity->type == PROP_UNDEFINED)
		{
			CString str;
			pTree->SelectItem(this->htiPOROSITY);
			str.Format("A default \"%s\" must be defined.", POROSITY);
			::AfxMessageBox(str);
			pDX->Fail();
		}
		// STORAGE
		if (this->bDefault && pDX->m_bSaveAndValidate && this->GridElt.storage->type == PROP_UNDEFINED)
		{
			CString str;
			pTree->SelectItem(this->htiSTORAGE);
			str.Format("A default \"%s\" must be defined.", STORAGE);
			::AfxMessageBox(str);
			pDX->Fail();
		}
		// ALPHA_LONG
		if (this->bDefault && !this->bFlowOnly && pDX->m_bSaveAndValidate && this->GridElt.alpha_long->type == PROP_UNDEFINED)
		{
			CString str;
			pTree->SelectItem(this->htiALPHA_LONG);
			str.Format("A default \"%s\" must be defined.", ALPHA_LONG);
			::AfxMessageBox(str);
			pDX->Fail();
		}
		// ALPHA_HORIZONTAL
		if (this->bDefault && !this->bFlowOnly && pDX->m_bSaveAndValidate && this->GridElt.alpha_horizontal->type == PROP_UNDEFINED)
		{
			CString str;
			pTree->SelectItem(this->htiALPHA_HORIZONTAL);
			str.Format("A default \"%s\" must be defined.", ALPHA_HORIZONTAL);
			::AfxMessageBox(str);
			pDX->Fail();
		}
		// ALPHA_VERTICAL
		if (this->bDefault && !this->bFlowOnly && pDX->m_bSaveAndValidate && this->GridElt.alpha_vertical->type == PROP_UNDEFINED)
		{
			CString str;
			pTree->SelectItem(this->htiALPHA_VERTICAL);
			str.Format("A default \"%s\" must be defined.", ALPHA_VERTICAL);
			::AfxMessageBox(str);
			pDX->Fail();
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}


BEGIN_MESSAGE_MAP(CPropsPage, CResizablePage)
	ON_NOTIFY(TVN_SELCHANGINGA, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGINGW, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGEDA,  0x7EEE, OnPageTreeSelChanged)
	ON_NOTIFY(TVN_SELCHANGEDW,  0x7EEE, OnPageTreeSelChanged)
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
END_MESSAGE_MAP()

// CPropsPage message handlers

bool CPropsPage::RegisterSheet(UINT nID, CPropertySheet& rSheet)
{
	TRACE("In %s\n", __FUNCTION__);

 	CWnd* pWnd = this->GetDlgItem( nID );
	if(!pWnd)
	{
		ASSERT(FALSE);
		TRACE("Out %s\n", __FUNCTION__);
		return false;
	}

	VERIFY(rSheet.Create(this, WS_CHILD|WS_VISIBLE, 0));

	if(0 != rSheet.GetPageCount())
	{
		rSheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
		rSheet.ModifyStyle(0, WS_TABSTOP);

		CRect rcSheet;
		pWnd->GetWindowRect(&rcSheet);
		this->ScreenToClient(&rcSheet);
		rSheet.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
	}

	TRACE("Out %s\n", __FUNCTION__);
	return true;
}

BOOL CPropsPage::OnInitDialog()
{
	TRACE("In %s\n", __FUNCTION__);

	this->SheetTop.AddPage(&this->PropNone);
	this->SheetTop.AddPage(&this->PropConstant);
	this->SheetTop.AddPage(&this->PropLinear);
	this->SheetTop.AddPage(&this->PropPoints);
	this->SheetTop.AddPage(&this->PropXYZ);
	
	this->SheetTop.SetTreeViewMode(TRUE, FALSE, FALSE);
	this->SheetTop.SetIsResizable(true);

	this->RegisterSheet(IDC_TREE_PROPS, this->SheetTop);

	VERIFY(this->Splitter.Create(this));
	this->Splitter.SetPane(0, &this->SheetTop);
	this->Splitter.SetPane(1, this->GetDlgItem(IDC_DESC_RICHEDIT));

	const int splitter_sizes[2] = {190, 89};
	this->Splitter.SetPaneSizes(splitter_sizes);

	this->AddAnchor(this->Splitter, TOP_LEFT, BOTTOM_RIGHT );

	if (CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl())
	{
		pTree->ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);
		pTree->DeleteAllItems();
	}

	__super::OnInitDialog();

	// Add extra initialization here

	TRACE("Out %s\n", __FUNCTION__);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPropsPage::OnPageTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemOld.hItem;
	if (this->ItemDDX)
	{
		if (!this->UpdateData(TRUE))
		{
			// notify which control caused failure
			//
			CWnd* pFocus = CWnd::GetFocus();
			::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

			// disallow change
			//
			*pResult = TRUE;
			return;
		}
	}
	*pResult = 0;
}

void CPropsPage::OnPageTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	UNREFERENCED_PARAMETER(pResult);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemNew.hItem;
	if (this->ItemDDX)
	{
		this->UpdateData(FALSE);

		// update property description
		//
		CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl();
		if (pTree && pTree->GetSafeHwnd())
		{
			CString strItem = pTree->GetItemText(this->ItemDDX);

			// ACTIVE
			if (strItem.Compare(ACTIVE) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sActiveRTF.c_str());
			}
			// KX
			else if (strItem.Compare(KX) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sKxRTF.c_str());
			}
			// KY
			else if (strItem.Compare(KY) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sKyRTF.c_str());
			}
			// KZ
			else if (strItem.Compare(KZ) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sKzRTF.c_str());
			}
			// ALPHA_LONG
			else if (strItem.Compare(ALPHA_LONG) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sAlphaLongRTF.c_str());
			}
			// POROSITY
			else if (strItem.Compare(POROSITY) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sPorosityRTF.c_str());
			}
			// STORAGE
			else if (strItem.Compare(STORAGE) == 0)
			{
				// this->m_wndRichEditCtrl.SetWindowText(this->m_sSpecStorageRTF.c_str());
				this->RichEditCtrl.SetWindowText(this->m_sStorageRTF.c_str());
			}
			// ALPHA_HORIZONTAL
			else if (strItem.Compare(ALPHA_HORIZONTAL) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sAlphaHorizontalRTF.c_str());
			}
			// ALPHA_VERTICAL
			else if (strItem.Compare(ALPHA_VERTICAL) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sAlphaVerticalRTF.c_str());
			}

		}
	}
}

LRESULT CPropsPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	return 0;
}

void CPropsPage::SetProperties(const CGridElt& rGridElt)
{
	// copy grid_elt
	this->GridElt = rGridElt;
	
	// fill empty properties
	//
	if (this->GridElt.active == 0)
	{
		this->GridElt.active = new Cproperty();
	}
	if (this->GridElt.alpha_long == 0)
	{
		this->GridElt.alpha_long = new Cproperty();
	}
	if (this->GridElt.alpha_horizontal == 0)
	{
		this->GridElt.alpha_horizontal = new Cproperty();
	}
	if (this->GridElt.alpha_vertical == 0)
	{
		this->GridElt.alpha_vertical = new Cproperty();
	}
	if (this->GridElt.kx == 0)
	{
		this->GridElt.kx = new Cproperty();
	}
	if (this->GridElt.ky == 0)
	{
		this->GridElt.ky = new Cproperty();
	}
	if (this->GridElt.kz == 0)
	{
		this->GridElt.kz = new Cproperty();
	}
	if (this->GridElt.porosity == 0)
	{
		this->GridElt.porosity = new Cproperty();
	}
	if (this->GridElt.storage == 0)
	{
		this->GridElt.storage = new Cproperty();
	}

	// copy to all
	//
	this->GridEltXYZ = this->GridEltPoints = this->GridEltFixed = this->GridEltLinear = this->GridElt;
}

void CPropsPage::GetProperties(CGridElt& rGridElt)const
{
	rGridElt = this->GridElt;

	// delete inactive properties
	//
	if (rGridElt.active->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&rGridElt.active, 0);
	}
	if (rGridElt.alpha_long->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&rGridElt.alpha_long, 0);
	}
	if (rGridElt.alpha_horizontal->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&rGridElt.alpha_horizontal, 0);
	}
	if (rGridElt.alpha_vertical->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&rGridElt.alpha_vertical, 0);
	}
	if (rGridElt.kx->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&rGridElt.kx, 0);
	}
	if (rGridElt.ky->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&rGridElt.ky, 0);
	}
	if (rGridElt.kz->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&rGridElt.kz, 0);
	}
	if (rGridElt.porosity->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&rGridElt.porosity, 0);
	}
	if (rGridElt.storage->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&rGridElt.storage, 0);
	}
}
