// FlowOnlyPropertyPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "FlowOnlyPropertyPage2.h"


// CFlowOnlyPropertyPage2 dialog

IMPLEMENT_DYNAMIC(CFlowOnlyPropertyPage2, CPropertyPage)
CFlowOnlyPropertyPage2::CFlowOnlyPropertyPage2()
	: CPropertyPage(CFlowOnlyPropertyPage2::IDD)
{
}

CFlowOnlyPropertyPage2::~CFlowOnlyPropertyPage2()
{
}

void CFlowOnlyPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFlowOnlyPropertyPage2, CPropertyPage)
END_MESSAGE_MAP()


// CFlowOnlyPropertyPage2 message handlers

BOOL CFlowOnlyPropertyPage2::OnSetActive()
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
