// CPropRich.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropRich.h"


// CPropRich dialog

IMPLEMENT_DYNAMIC(CPropRich, CPropPage)

CPropRich::CPropRich()
: CPropPage(CPropRich::IDD)
{
}

CPropRich::~CPropRich()
{
}

void CPropRich::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, RichEditCtrl);

// COMMENT: {5/29/2009 4:05:30 PM}	// description
// COMMENT: {5/29/2009 4:05:30 PM}	//
// COMMENT: {5/29/2009 4:05:30 PM}	if (pDX->m_bSaveAndValidate)
// COMMENT: {5/29/2009 4:05:30 PM}	{
// COMMENT: {5/29/2009 4:05:30 PM}		CString str;
// COMMENT: {5/29/2009 4:05:30 PM}		::DDX_Text(pDX, IDC_DESC_EDIT, str);
// COMMENT: {5/29/2009 4:05:30 PM}		this->sDesc = str;
// COMMENT: {5/29/2009 4:05:30 PM}	}
// COMMENT: {5/29/2009 4:05:30 PM}	else
// COMMENT: {5/29/2009 4:05:30 PM}	{
// COMMENT: {5/29/2009 4:05:30 PM}		CString str(this->sDesc.c_str());
// COMMENT: {5/29/2009 4:05:30 PM}		::DDX_Text(pDX, IDC_DESC_EDIT, str);
// COMMENT: {5/29/2009 4:05:30 PM}	}
}


BEGIN_MESSAGE_MAP(CPropRich, CPropPage)
END_MESSAGE_MAP()


// CPropRich message handlers

BOOL CPropRich::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
	this->AddAnchor(IDC_DESC_RICHEDIT, TOP_LEFT, BOTTOM_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
