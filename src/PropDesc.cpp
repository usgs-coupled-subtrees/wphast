// CPropDesc.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropDesc.h"


// CPropDesc dialog

IMPLEMENT_DYNAMIC(CPropDesc, CPropPage)

CPropDesc::CPropDesc()
: CPropPage(CPropDesc::IDD)
{
}

CPropDesc::~CPropDesc()
{
}

void CPropDesc::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);

	// description
	//
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		::DDX_Text(pDX, IDC_DESC_EDIT, str);
		this->sDesc = str;
	}
	else
	{
		CString str(this->sDesc.c_str());
		::DDX_Text(pDX, IDC_DESC_EDIT, str);
	}
}


BEGIN_MESSAGE_MAP(CPropDesc, CPropPage)
// COMMENT: {5/26/2009 9:23:38 PM}	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPropDesc message handlers

BOOL CPropDesc::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
	this->AddAnchor(IDC_DESC_EDIT, TOP_LEFT, TOP_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// COMMENT: {5/26/2009 9:23:50 PM}HBRUSH CPropDesc::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
// COMMENT: {5/26/2009 9:23:50 PM}{
// COMMENT: {5/26/2009 9:23:50 PM}	if (this->HasWhiteBackground())
// COMMENT: {5/26/2009 9:23:50 PM}	{
// COMMENT: {5/26/2009 9:23:50 PM}		pDC->SetBkMode(TRANSPARENT);
// COMMENT: {5/26/2009 9:23:50 PM}		return ::GetSysColorBrush(COLOR_WINDOW);
// COMMENT: {5/26/2009 9:23:50 PM}	}
// COMMENT: {5/26/2009 9:23:50 PM}
// COMMENT: {5/26/2009 9:23:50 PM}	// default
// COMMENT: {5/26/2009 9:23:50 PM}	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
// COMMENT: {5/26/2009 9:23:50 PM}	return hbr;
// COMMENT: {5/26/2009 9:23:50 PM}}
