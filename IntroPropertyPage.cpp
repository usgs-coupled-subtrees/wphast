// IntroPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "IntroPropertyPage.h"


// CIntroPropertyPage dialog

IMPLEMENT_DYNAMIC(CIntroPropertyPage, CPropertyPage)
CIntroPropertyPage::CIntroPropertyPage()
	: CPropertyPage(CIntroPropertyPage::IDD)
{
}

CIntroPropertyPage::~CIntroPropertyPage()
{
}

void CIntroPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CIntroPropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CIntroPropertyPage message handlers
