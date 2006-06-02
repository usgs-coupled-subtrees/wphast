// MapPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "MapPropertyPage.h"


// CMapPropertyPage dialog

IMPLEMENT_DYNAMIC(CMapPropertyPage, CMapPropertyPageBase)
CMapPropertyPage::CMapPropertyPage()
	: CMapPropertyPageBase(CMapPropertyPage::IDD)
{
}

CMapPropertyPage::~CMapPropertyPage()
{
}

void CMapPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CMapPropertyPageBase::DoDataExchange(pDX);
}

CRect CMapPropertyPage::GetRect() 
{ 
	//{{ORIGINAL
	//return CMapPropertyPageBase::GetRect();
	//}}ORIGINAL
	CRect r; 
	GetClientRect(r);

	if (ETSLayoutPropertySheet *pSheet = (ETSLayoutPropertySheet*)this->GetParent())
	{
		if( pSheet->HasGripper() ) 
		{
			if(pSheet->GetStatusBar() && ::IsWindow(pSheet->GetStatusBar()->GetSafeHwnd()) ) 
			{
				CRect rcSizeIcon;
				pSheet->GetStatusBar()->GetWindowRect( rcSizeIcon);
				r.bottom -= rcSizeIcon.Height();
			}
		}
	}
	return r; 
}



BEGIN_MESSAGE_MAP(CMapPropertyPage, CMapPropertyPageBase)
END_MESSAGE_MAP()

// CMapPropertyPage message handlers

BOOL CMapPropertyPage::OnInitDialog()
{
	CMapPropertyPageBase::OnInitDialog();
// COMMENT: {4/22/2004 3:19:12 PM}	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
#if defined(__IS_RESIZABLE__)
	// Create the root pane
	CreateRoot( VERTICAL );
// COMMENT: {4/22/2004 4:58:14 PM}	m_RootPane->addItem(IDC_LIST1);
	UpdateLayout();
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
