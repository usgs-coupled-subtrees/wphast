// ScalePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ScalePropertyPage.h"

#include "WPhastDoc.h"
#include "ModelessPropertySheet.h"
#include "SetScaleAction.h"


// CScalePropertyPage dialog

IMPLEMENT_DYNAMIC(CScalePropertyPage, CPropertyPage)
CScalePropertyPage::CScalePropertyPage()
	: CPropertyPage(CScalePropertyPage::IDD)
	, m_XScale(1.0)
	, m_YScale(1.0)
	, m_ZScale(1.0)
	, m_pDoc(0)
	, m_bNeedAction(FALSE)
	, m_pAction(0)
	, m_bModifiedDoc(false)
{
}

CScalePropertyPage::~CScalePropertyPage()
{
	ASSERT(this->m_pAction == 0); // should have been reset in PostNcDestroy
}

void CScalePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_XSCALE, m_XScale);
	DDX_Text(pDX, IDC_YSCALE, m_YScale);
	DDX_Text(pDX, IDC_ZSCALE, m_ZScale);

	pDX->PrepareEditCtrl(IDC_XSCALE);
	if (pDX->m_bSaveAndValidate)
	{
		if (m_XScale <= 0)
		{
			AfxMessageBox("Please enter a positive number.", MB_ICONEXCLAMATION);
			pDX->Fail();
		}
	}
	pDX->PrepareEditCtrl(IDC_YSCALE);
	if (pDX->m_bSaveAndValidate)
	{
		if (m_YScale <= 0)
		{
			AfxMessageBox("Please enter a positive number.", MB_ICONEXCLAMATION);
			pDX->Fail();
		}
	}
	pDX->PrepareEditCtrl(IDC_ZSCALE);
	if (pDX->m_bSaveAndValidate)
	{
		if (m_ZScale <= 0)
		{
			AfxMessageBox("Please enter a positive number.", MB_ICONEXCLAMATION);
			pDX->Fail();
		}
	}

}


BEGIN_MESSAGE_MAP(CScalePropertyPage, CPropertyPage)
	ON_EN_CHANGE(IDC_XSCALE, OnEnChange)
	ON_EN_CHANGE(IDC_YSCALE, OnEnChange)
	ON_EN_CHANGE(IDC_ZSCALE, OnEnChange)
END_MESSAGE_MAP()


// CScalePropertyPage message handlers

BOOL CScalePropertyPage::OnApply()
{
	// Add your specialized code here and/or call the base class
	if (this->m_bNeedAction && this->UpdateData(TRUE))
	{
// COMMENT: {3/8/2004 8:37:31 PM}		CSetScaleAction* pSetScaleAction = new CSetScaleAction(this->m_pDoc, m_XScale, m_YScale, m_ZScale);
// COMMENT: {3/8/2004 8:37:31 PM}		this->m_pDoc->Execute(pSetScaleAction);
// COMMENT: {3/8/2004 8:37:31 PM}		////// this->CancelToClose(); // broken
// COMMENT: {3/8/2004 8:37:31 PM}		//////this->GetParent()->PostMessage(PSM_CANCELTOCLOSE);
// COMMENT: {3/8/2004 8:37:31 PM}		this->m_bNeedAction = false;
		//{{
		if (this->m_pAction == 0)
		{
			ASSERT(this->m_pDoc);
			this->m_pAction = new CSetScaleAction(this->m_pDoc, m_XScale, m_YScale, m_ZScale);
			if (this->m_pAction == 0) ::AfxMessageBox("Out of Memory");
			if (CModelessPropertySheet *pSheet = reinterpret_cast<CModelessPropertySheet*>(this->GetParent()))
			{
				if (this->m_pAction)
				{
					if (!this->m_pDoc->IsModified())
					{
						this->m_bModifiedDoc = true;
						this->m_pDoc->SetModifiedFlag(TRUE);
					}
					this->m_pAction->Execute();
					pSheet->AddAction(this->m_pAction);
				}
			}
		}
		else
		{
			this->m_pAction->Apply(this->m_XScale, this->m_YScale, this->m_ZScale);
		}
		this->m_bNeedAction = false;
		//}}
	}
	return CPropertyPage::OnApply();
}

void CScalePropertyPage::OnEnChange()
{
	// Add your control notification handler code here
	this->SetModified(TRUE);
	this->m_bNeedAction = TRUE;
}

void CScalePropertyPage::OnOK()
{
	// Add your specialized code here and/or call the base class

	CPropertyPage::OnOK();
}

void CScalePropertyPage::OnCancel()
{
	// Add your specialized code here and/or call the base class
	if (this->m_pAction)
	{
		if (this->m_bModifiedDoc && this->m_pDoc)
		{
			ASSERT(this->m_pDoc->IsModified());
			this->m_pDoc->SetModifiedFlag(FALSE);
		}
		this->m_pAction->UnExecute();
	}

	CPropertyPage::OnCancel();
}

BOOL CScalePropertyPage::OnQueryCancel()
{
	// Add your specialized code here and/or call the base class

	return CPropertyPage::OnQueryCancel();
}

void CScalePropertyPage::OnReset()
{
	// Add your specialized code here and/or call the base class

	CPropertyPage::OnReset();
}

BOOL CScalePropertyPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// Add your specialized code here and/or call the base class
	UINT nID = (UINT)wParam;
	NMHDR* pnmhdr = (NMHDR*)lParam;
	TRACE("nID = 0x%04X\n", nID);
	TRACE("pnmhdr->code = 0x%04X\n", pnmhdr->code);
	TRACE("PSN_APPLY       = 0x%04X\n", (UINT)PSN_APPLY);
	TRACE("PSN_KILLACTIVE  = 0x%04X\n", (UINT)PSN_KILLACTIVE);
	TRACE("PSN_QUERYCANCEL = 0x%04X\n", (UINT)PSN_QUERYCANCEL);
	TRACE("PSN_RESET       = 0x%04X\n", (UINT)PSN_RESET);	
	TRACE("PSN_SETACTIVE   = 0x%04X\n", (UINT)PSN_SETACTIVE);
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

void CScalePropertyPage::PostNcDestroy()
{
	// Add your specialized code here and/or call the base class
	this->m_pAction = 0;
	CPropertyPage::PostNcDestroy();
}
