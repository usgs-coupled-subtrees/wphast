// AxesPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "AxesPropertyPage.h"


// CAxesPropertyPage dialog

IMPLEMENT_DYNAMIC(CAxesPropertyPage, CPropertyPage)
CAxesPropertyPage::CAxesPropertyPage()
	: CPropertyPage(CAxesPropertyPage::IDD)
	, m_XPos(0)
	, m_YPos(0)
	, m_ZPos(0)
{
}

CAxesPropertyPage::~CAxesPropertyPage()
{
}

void CAxesPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_X_ORIGIN, m_XPos);
	DDX_Text(pDX, IDC_Y_ORIGIN, m_YPos);
	DDX_Text(pDX, IDC_Z_ORIGIN, m_ZPos);
}


BEGIN_MESSAGE_MAP(CAxesPropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CAxesPropertyPage message handlers
