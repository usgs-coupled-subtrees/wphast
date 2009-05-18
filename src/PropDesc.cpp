// CPropDesc.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropDesc.h"


// CPropDesc dialog

IMPLEMENT_DYNAMIC(CPropDesc, CResizablePage)

CPropDesc::CPropDesc()
: CResizablePage(CPropDesc::IDD)
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


BEGIN_MESSAGE_MAP(CPropDesc, CResizablePage)
	ON_WM_CTLCOLOR()
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

HBRUSH CPropDesc::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (this->HasWhiteBackground())
	{
		pDC->SetBkMode(TRANSPARENT);
		return ::GetSysColorBrush(COLOR_WINDOW);
	}

	// default
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}
