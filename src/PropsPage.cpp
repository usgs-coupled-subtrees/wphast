// PropsPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropsPage.h"

#include "PropStruct.h"
#include "Global.h"


// CPropsPage dialog

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
		HTREEITEM hti;

		// Active
		hti = pTree->InsertItem(_T("Active"), TVI_ROOT, TVI_LAST);

		// Kx
		hti = pTree->InsertItem(_T("Kx"), TVI_ROOT, TVI_LAST);

		// Ky
		hti = pTree->InsertItem(_T("Ky"), TVI_ROOT, TVI_LAST);

		// Kz
		hti = pTree->InsertItem(_T("Kz"), TVI_ROOT, TVI_LAST);

		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);
		this->RichEditCtrl.SetWindowText(this->m_sActiveRTF.c_str());

		this->ItemDDX = pTree->GetFirstVisibleItem();
		pTree->SelectItem(this->ItemDDX);
	}

	if (this->ItemDDX)
	{
		CString strItem = pTree->GetItemText(this->ItemDDX);

		std::vector<CPropertyPage*> pages;
		pages.push_back(&this->PropNone);
		pages.push_back(&this->PropConstant);

		// Active
		if (strItem.Compare(_T("Active")) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.active));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.active));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.active));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
		// Kx
		else if (strItem.Compare(_T("Kx")) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.kx));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.kx));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.kx));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
		// Ky
		else if (strItem.Compare(_T("Ky")) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.ky));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.ky));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.ky));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
		// Kz
		else if (strItem.Compare(_T("Kz")) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->GridElt.kz));
			props.push_back(static_cast<Cproperty*>(this->GridEltFixed.kz));
			props.push_back(static_cast<Cproperty*>(this->GridEltLinear.kz));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}
	}


	// COMMENT: {4/1/2009 6:48:07 PM}	this->SheetTop.SetActivePage(1);
	TRACE("Out %s\n", __FUNCTION__);

	// COMMENT: {4/1/2009 6:47:58 PM}	this->PropConstant.DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropsPage, CResizablePage)
	ON_NOTIFY(TVN_SELCHANGINGA, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGINGW, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGEDA,  0x7EEE, OnPageTreeSelChanged)
	ON_NOTIFY(TVN_SELCHANGEDW,  0x7EEE, OnPageTreeSelChanged)
	//{{USER
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	//}}USER
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
// COMMENT: {4/1/2009 7:52:42 PM}	this->SheetTop.AddPage(&this->PropNone);
// COMMENT: {4/1/2009 7:52:42 PM}	this->SheetTop.AddPage(&this->PropConstant);
// COMMENT: {4/1/2009 7:52:42 PM}
// COMMENT: {4/1/2009 7:52:42 PM}	this->SheetTop.SetTreeViewMode(TRUE, FALSE, FALSE);
// COMMENT: {4/1/2009 7:52:42 PM}	this->SheetTop.SetIsResizable(true);
// COMMENT: {4/1/2009 7:52:42 PM}
// COMMENT: {4/1/2009 7:52:42 PM}	this->RegisterSheet(IDC_TREE_PROPS, this->SheetTop);
// COMMENT: {4/1/2009 7:52:42 PM}
// COMMENT: {4/1/2009 7:52:42 PM}	VERIFY(this->Splitter.Create(this));
// COMMENT: {4/1/2009 7:52:42 PM}	this->Splitter.SetPane(0, &this->SheetTop);
// COMMENT: {4/1/2009 7:52:42 PM}	this->Splitter.SetPane(1, this->GetDlgItem(IDC_DESC_RICHEDIT));
// COMMENT: {4/1/2009 7:52:42 PM}
// COMMENT: {4/1/2009 7:52:42 PM}	const int splitter_sizes[2] = {190, 89};
// COMMENT: {4/1/2009 7:52:42 PM}	this->Splitter.SetPaneSizes(splitter_sizes);
// COMMENT: {4/1/2009 7:52:42 PM}
// COMMENT: {4/1/2009 7:52:42 PM}	this->AddAnchor(this->Splitter, TOP_LEFT, BOTTOM_RIGHT );

// COMMENT: {3/31/2009 10:13:35 PM}	//{{
// COMMENT: {3/31/2009 10:13:35 PM}	if (CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl())
// COMMENT: {3/31/2009 10:13:35 PM}	{
// COMMENT: {3/31/2009 10:13:35 PM}		pTree->DeleteAllItems();
// COMMENT: {3/31/2009 10:13:35 PM}
// COMMENT: {3/31/2009 10:13:35 PM}		HTREEITEM hItem = pTree->InsertItem("Active");
// COMMENT: {3/31/2009 10:13:35 PM}		pTree->InsertItem("constant 0", hItem);
// COMMENT: {3/31/2009 10:13:35 PM}
// COMMENT: {3/31/2009 10:13:35 PM}		pTree->InsertItem("Kx  constant 90");
// COMMENT: {3/31/2009 10:13:35 PM}		pTree->InsertItem("Ky  constant 90");
// COMMENT: {3/31/2009 10:13:35 PM}		pTree->InsertItem("Kz  constant 20");
// COMMENT: {3/31/2009 10:13:35 PM}		pTree->InsertItem("Porosity");
// COMMENT: {3/31/2009 10:13:35 PM}		pTree->InsertItem("Specific storage");
// COMMENT: {3/31/2009 10:13:35 PM}		pTree->InsertItem("Longitudinal dispersivity");
// COMMENT: {3/31/2009 10:13:35 PM}		pTree->InsertItem("Horizontal dispersivity");
// COMMENT: {3/31/2009 10:13:35 PM}		pTree->InsertItem("Vertical dispersivity");
// COMMENT: {3/31/2009 10:13:35 PM}
// COMMENT: {3/31/2009 10:13:35 PM}		pTree->ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);
// COMMENT: {3/31/2009 10:13:35 PM}	}
// COMMENT: {3/31/2009 10:13:35 PM}	//}}


// COMMENT: {3/31/2009 10:24:41 PM}	std::list<CPropStruct> items;
// COMMENT: {3/31/2009 10:24:41 PM}	CPropStruct p;
// COMMENT: {3/31/2009 10:24:41 PM}
// COMMENT: {3/31/2009 10:24:41 PM}	p.title = "Active";
// COMMENT: {3/31/2009 10:24:41 PM}	p.prop  = Cproperty(0);
// COMMENT: {3/31/2009 10:24:41 PM}	items.push_back(p);
// COMMENT: {3/31/2009 10:24:41 PM}
// COMMENT: {3/31/2009 10:24:41 PM}	p.title = "Kx";
// COMMENT: {3/31/2009 10:24:41 PM}	p.prop  = Cproperty(90);
// COMMENT: {3/31/2009 10:24:41 PM}	items.push_back(p);
// COMMENT: {3/31/2009 10:24:41 PM}
// COMMENT: {3/31/2009 10:24:41 PM}	p.title = "Ky";
// COMMENT: {3/31/2009 10:24:41 PM}	p.prop  = Cproperty(90);
// COMMENT: {3/31/2009 10:24:41 PM}	items.push_back(p);
// COMMENT: {3/31/2009 10:24:41 PM}
// COMMENT: {3/31/2009 10:24:41 PM}	p.title = "Kz";
// COMMENT: {3/31/2009 10:24:41 PM}	p.prop  = Cproperty(20);
// COMMENT: {3/31/2009 10:24:41 PM}	items.push_back(p);

// COMMENT: {4/1/2009 7:53:47 PM}	if (CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl())
// COMMENT: {4/1/2009 7:53:47 PM}	{
// COMMENT: {4/1/2009 7:53:47 PM}		pTree->ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);
// COMMENT: {4/1/2009 7:53:47 PM}		pTree->DeleteAllItems();
// COMMENT: {4/1/2009 7:53:47 PM}
// COMMENT: {4/1/2009 7:53:47 PM}		std::vector<CPropStruct>::iterator it = this->Items.begin();
// COMMENT: {4/1/2009 7:53:47 PM}		for (; it != this->Items.end(); ++it)
// COMMENT: {4/1/2009 7:53:47 PM}		{
// COMMENT: {4/1/2009 7:53:47 PM}			pTree->InsertItem((*it).title);
// COMMENT: {4/1/2009 7:53:47 PM}		}
// COMMENT: {4/1/2009 7:53:47 PM}		pTree->SelectItem(pTree->GetFirstVisibleItem());
// COMMENT: {4/1/2009 7:53:47 PM}// COMMENT: {4/1/2009 4:02:51 PM}		this->SheetTop.SetFocus();
// COMMENT: {4/1/2009 7:53:47 PM}// COMMENT: {4/1/2009 4:02:51 PM}		pTree->SetFocus();
// COMMENT: {4/1/2009 7:53:47 PM}// COMMENT: {4/1/2009 4:02:51 PM}		this->Splitter.SetActivePane(0);
// COMMENT: {4/1/2009 7:53:47 PM}// COMMENT: {4/1/2009 4:02:51 PM}		this->SheetTop.GetSplitterObject()->SetActivePane(0);
// COMMENT: {4/1/2009 7:53:47 PM}// COMMENT: {4/1/2009 4:02:51 PM}		pTree->SetFocus();
// COMMENT: {4/1/2009 7:53:47 PM}	}

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

			// Active
			if (strItem.Compare(_T("Active")) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sActiveRTF.c_str());
			}
			// Kx
			else if (strItem.Compare(_T("Kx")) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sKxRTF.c_str());
			}
			// Ky
			else if (strItem.Compare(_T("Ky")) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sKyRTF.c_str());
			}
			// Kz
			else if (strItem.Compare(_T("Kz")) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sKzRTF.c_str());
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
	this->GridEltFixed = this->GridEltLinear = this->GridElt;
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
