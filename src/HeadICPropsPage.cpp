// PropsPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "HeadICPropsPage.h"

#include "PropStruct.h"
#include "Global.h"


// CHeadICPropsPage dialog

const TCHAR HEAD[] = _T("Head");

IMPLEMENT_DYNAMIC(CHeadICPropsPage, CResizablePage)

CHeadICPropsPage::CHeadICPropsPage()
: CResizablePage(CHeadICPropsPage::IDD)
, Splitter(3, SSP_VERT, 33, 3)
, ItemDDX(0)
{
	TRACE("%s\n", __FUNCTION__);
	this->CommonConstruct();
}

void CHeadICPropsPage::CommonConstruct(void)
{
	this->m_psp.dwFlags &= (~PSP_HASHELP); // remove help button

// COMMENT: {5/27/2009 4:44:48 PM}	this->SetFlowOnly(false);
	this->SetDefault(false);

	// init properties
	//
	this->SetProperties(this->HeadIC);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sMaskRTF, IDR_IC_HEAD_MASK_RTF);
	CGlobal::LoadRTFString(this->m_sHeadRTF, IDR_IC_HEAD_HEAD_RTF);
}

CHeadICPropsPage::~CHeadICPropsPage()
{
	TRACE("%s\n", __FUNCTION__);
}

void CHeadICPropsPage::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);
	CResizablePage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, RichEditCtrl);


	// description
	if (pDX->m_bSaveAndValidate)
	{
		this->PageDesc.UpdateData(TRUE);
	}

	CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl();

	if (pTree && pTree->GetCount() == 0)
	{
		this->htiHEAD = pTree->InsertItem(HEAD, TVI_ROOT, TVI_LAST);

		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);
		this->RichEditCtrl.SetWindowText(this->m_sHeadRTF.c_str());

		this->ItemDDX = this->htiHEAD;
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

		// Head
		if (strItem.Compare(HEAD) == 0)
		{
			std::vector<Cproperty*> props;
			props.push_back(static_cast<Cproperty*>(this->HeadIC.head));
			props.push_back(static_cast<Cproperty*>(this->HeadICConstant.head));
			props.push_back(static_cast<Cproperty*>(this->HeadICLinear.head));
			props.push_back(static_cast<Cproperty*>(this->HeadICPoints.head));
			props.push_back(static_cast<Cproperty*>(this->HeadICXYZ.head));

			CGlobal::DDX_Property(
				pDX, 
				&this->SheetTop,
				this->ItemDDX,
				props,
				pages
				);
		}

		// Head
		if (this->Default && pDX->m_bSaveAndValidate && this->HeadIC.head->type == PROP_UNDEFINED)
		{
			CString str;
			pTree->SelectItem(this->htiHEAD);
			str.Format("A default \"%s\" must be defined.", HEAD);
			::AfxMessageBox(str);
			pDX->Fail();
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}


BEGIN_MESSAGE_MAP(CHeadICPropsPage, CResizablePage)
	ON_NOTIFY(TVN_SELCHANGINGA, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGINGW, 0x7EEE, OnPageTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGEDA,  0x7EEE, OnPageTreeSelChanged)
	ON_NOTIFY(TVN_SELCHANGEDW,  0x7EEE, OnPageTreeSelChanged)
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
END_MESSAGE_MAP()

// CHeadICPropsPage message handlers

bool CHeadICPropsPage::RegisterSheet(UINT nID, CPropertySheet& rSheet)
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

BOOL CHeadICPropsPage::OnInitDialog()
{
	TRACE("In %s\n", __FUNCTION__);

	this->SheetTop.AddPage(&this->PropNone);
	this->SheetTop.AddPage(&this->PropConstant);
	this->SheetTop.AddPage(&this->PropLinear);
	this->SheetTop.AddPage(&this->PropPoints);
	this->SheetTop.AddPage(&this->PropXYZ);
	
	this->SheetTop.SetTreeViewMode(TRUE, FALSE, FALSE);
	this->SheetTop.SetIsResizable(true);

	if (CWnd* pWnd = this->GetDlgItem(IDC_DESC_PAGE))
	{
		VERIFY(this->PageDesc.Create(IDD_PROP_DESC, this));
		ASSERT(this->PageDesc.GetStyle() | DS_CONTROL);			
		ASSERT(this->PageDesc.GetExStyle() | WS_EX_CONTROLPARENT);

		CRect rcSheet;
		pWnd->GetWindowRect(&rcSheet);
		this->ScreenToClient(&rcSheet);
		this->PageDesc.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		this->PageDesc.ShowWindow(SW_SHOW);
	}
	else
	{
		ASSERT(FALSE);
	}

	this->RegisterSheet(IDC_TREE_PROPS, this->SheetTop);

	VERIFY(this->Splitter.Create(this));
	this->Splitter.SetPane(0, &this->PageDesc);
	this->Splitter.SetPane(1, &this->SheetTop);
	this->Splitter.SetPane(2, this->GetDlgItem(IDC_DESC_RICHEDIT));

	const int splitter_sizes[3] = {33, 164, 80};
	this->Splitter.SetPaneSizes(splitter_sizes);

	const bool frozen[3] = {true, false, false};
	this->Splitter.SetFrozenPanes(frozen);

	this->AddAnchor(this->Splitter, TOP_LEFT, BOTTOM_RIGHT);

	if (CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl())
	{
		pTree->ModifyStyle(TVS_TRACKSELECT|TVS_LINESATROOT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);
		pTree->DeleteAllItems();
	}

	__super::OnInitDialog();

	// Add extra initialization here

	TRACE("Out %s\n", __FUNCTION__);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CHeadICPropsPage::OnPageTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
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
			TRACE("Out Disallowed %s\n", __FUNCTION__);
			return;
		}
	}
	*pResult = 0;
	TRACE("Out Allowed %s\n", __FUNCTION__);
}

void CHeadICPropsPage::OnPageTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
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

			// HEAD
			if (strItem.Compare(HEAD) == 0)
			{
				this->RichEditCtrl.SetWindowText(this->m_sHeadRTF.c_str());
			}
		}
	}
	CTreeCtrl *pTree = this->SheetTop.GetPageTreeControl();
	if (pTree && pTree->GetSafeHwnd())
	{
		pTree->SetFocus();
	}
	TRACE("Out %s\n", __FUNCTION__);
}

LRESULT CHeadICPropsPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	TRACE("In %s\n", __FUNCTION__);
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	TRACE("Out %s\n", __FUNCTION__);
	return 0;
}

void CHeadICPropsPage::SetProperties(const CHeadIC& rHeadIC)
{
	TRACE("In %s\n", __FUNCTION__);
	// copy grid_elt
	this->HeadIC = rHeadIC;
	
	// fill empty properties
	//
	if (this->HeadIC.head == 0)
	{
		this->HeadIC.head = new Cproperty();
	}

	// copy to all
	//
	this->HeadICXYZ = this->HeadICPoints = this->HeadICLinear = this->HeadICConstant = this->HeadIC;
	TRACE("Out %s\n", __FUNCTION__);
}

void CHeadICPropsPage::GetProperties(CHeadIC& rHeadIC)const
{
	TRACE("In %s\n", __FUNCTION__);
	rHeadIC = this->HeadIC;

	// delete inactive properties
	//
	if (rHeadIC.head->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&rHeadIC.head, 0);
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CHeadICPropsPage::SetDesc(LPCTSTR desc)
{
	this->PageDesc.SetDesc(desc);
}

LPCTSTR CHeadICPropsPage::GetDesc()const
{
	return this->PageDesc.GetDesc();
}
