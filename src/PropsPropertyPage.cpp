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
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropsPropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CPropsPropertyPage message handlers
void CPropsPropertyPage::DDV_SoftValidate()
{
	// default no-op
}

BOOL CPropsPropertyPage::OnSetActive()
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

BOOL CPropsPropertyPage::OnKillActive()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		// skip UpdateData(TRUE)
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
}

LRESULT CPropsPropertyPage::OnWizardNext()
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

LRESULT CPropsPropertyPage::OnWizardBack()
{
	this->DDV_SoftValidate();
	return CPropertyPage::OnWizardBack();
}
