// TempPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "TempPropertyPage.h"


// CTempPropertyPage dialog

IMPLEMENT_DYNAMIC(CTempPropertyPage, CPropertyPage)
CTempPropertyPage::CTempPropertyPage()
	: CPropertyPage(CTempPropertyPage::IDD)
	, m_strHeightUnits(_T(""))
{
}

CTempPropertyPage::~CTempPropertyPage()
{
}

void CTempPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HEIGHT_UNITS, m_strHeightUnits);
}


BEGIN_MESSAGE_MAP(CTempPropertyPage, CPropertyPage)
	ON_BN_CLICKED(IDC_APPLY, OnBnClickedApply)
END_MESSAGE_MAP()


// CTempPropertyPage message handlers

void CTempPropertyPage::OnBnClickedApply()
{
	// TODO: Add your control notification handler code here
}
