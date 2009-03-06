// Startup.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "Startup.h"

#include <uxtheme.h>


// CStartup

IMPLEMENT_DYNAMIC(CStartup, CPropertySheet)
CStartup::CStartup(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CStartup::CStartup(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CStartup::CStartup()
	:CPropertySheet()
{
}

CStartup::~CStartup()
{
}


BEGIN_MESSAGE_MAP(CStartup, CPropertySheet)
END_MESSAGE_MAP()


// CStartup message handlers

BOOL CStartup::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// Add your specialized code here
	//this->GetTabControl()->ModifyStyle(TCS_HOTTRACK|TCS_MULTILINE, TCS_BUTTONS|TCS_SINGLELINE/*|TCS_FLATBUTTONS*/);

	//SetWindowTheme(this->GetSafeHwnd(), L" ", L" ");
	//SetWindowTheme(this->GetTabControl()->GetSafeHwnd(), L" ", L" ");
	this->GetTabControl()->ShowWindow(SW_HIDE);

	return bResult;
}
