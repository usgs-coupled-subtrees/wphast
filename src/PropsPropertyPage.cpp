// PropsPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropsPropertyPage.h"

#include "NewModelWizard.h"

// CPropsPropertyPage dialog

IMPLEMENT_DYNAMIC(CPropsPropertyPage, CPropertyPage)

CPropsPropertyPage::CPropsPropertyPage(UINT nIDTemplate, UINT nIDCaption, DWORD dwSize)
: CPropertyPage(nIDTemplate, nIDCaption, dwSize)
{
}

CPropsPropertyPage::~CPropsPropertyPage()
{
}

void CPropsPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s %s\n", __FUNCTION__, pDX->m_bSaveAndValidate ? _T("TRUE") : _T("FALSE"));
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	DDX_Control(pDX, IDC_PROP_TREE, this->TreeCtrl);
	DDX_GridControl(pDX, IDC_POINTS_GRID, this->PointsGrid);
	if (this->m_bFirstSetActive)
	{
		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);
	}
	TRACE("Out %s %s\n", __FUNCTION__, pDX->m_bSaveAndValidate ? _T("TRUE") : _T("FALSE"));
}

BEGIN_MESSAGE_MAP(CPropsPropertyPage, CPropertyPage)
	// IDC_PROP_TREE
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGED,  IDC_PROP_TREE, OnTreeSelChanged)
	ON_NOTIFY(NM_SETFOCUS, IDC_PROP_TREE, OnNMSetfocusPropTree)
END_MESSAGE_MAP()


// CPropsPropertyPage message handlers
void CPropsPropertyPage::DDV_SoftValidate()
{
	// default no-op
}

BOOL CPropsPropertyPage::OnSetActive()
{
	TRACE("In %s \n", __FUNCTION__);
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
	TRACE("Out %s \n", __FUNCTION__);
	return bRet;
}

BOOL CPropsPropertyPage::OnKillActive()
{
	TRACE("In %s \n", __FUNCTION__);
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		// skip UpdateData(TRUE)
		TRACE("Out %s \n", __FUNCTION__);
		return TRUE;
	}
	TRACE("Out %s \n", __FUNCTION__);
	return CPropertyPage::OnKillActive();
}

LRESULT CPropsPropertyPage::OnWizardNext()
{
	TRACE("In %s \n", __FUNCTION__);
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (!this->UpdateData(TRUE))
		{
			TRACE("Out %s -1\n", __FUNCTION__);
			return -1; // return –1 to prevent the page from changing 
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
	return CPropertyPage::OnWizardNext();
}

LRESULT CPropsPropertyPage::OnWizardBack()
{
	TRACE("In %s \n", __FUNCTION__);
	this->DDV_SoftValidate();
	TRACE("Out %s \n", __FUNCTION__);
	return CPropertyPage::OnWizardBack();
}

void CPropsPropertyPage::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemOld.hItem;
	if (this->ItemDDX)
	{
		this->DDV_SoftValidate(); // this may be unnecessary

		// force CInPlaceXXX to lose focus
		this->TreeCtrl.SetFocus();

		if (!this->UpdateData(TRUE))
		{
			// notify which control caused failure
			//
			CWnd* pFocus = CWnd::GetFocus();
			this->PostMessage(UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

			// disallow change
			//
			*pResult = TRUE;
			TRACE("Out %s Disallowed\n", __FUNCTION__);
			return;
		}
	}
	*pResult = 0;
	TRACE("Out Allowed %s\n", __FUNCTION__);
}

void CPropsPropertyPage::OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
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
		if (this->TreeCtrl.GetSafeHwnd())
		{
			this->SetPropertyDescription();
		}
	}
	if (this->TreeCtrl.GetSafeHwnd())
	{
		this->TreeCtrl.SetFocus();
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CPropsPropertyPage::SetPropertyDescription()
{
	// no-op
}

void CPropsPropertyPage::OnNMSetfocusPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {7/14/2009 2:17:30 PM}	NMTREEVIEW tvn;
// COMMENT: {7/14/2009 2:17:30 PM}	tvn.hdr.hwndFrom = this->TreeCtrl.GetSafeHwnd();
// COMMENT: {7/14/2009 2:17:30 PM}	tvn.itemNew.hItem = this->ItemDDX;
// COMMENT: {7/14/2009 2:17:30 PM}	tvn.itemOld.hItem = this->ItemDDX;
// COMMENT: {7/14/2009 2:17:30 PM}
// COMMENT: {7/14/2009 2:17:30 PM}	this->OnTreeSelChanged((NMHDR*)&tvn, pResult);

	//{{
	// update property description
	//
	if (this->TreeCtrl.GetSafeHwnd())
	{
		this->SetPropertyDescription();
	}
	//}}

	*pResult = 0;
	TRACE("Out %s\n", __FUNCTION__);
}
