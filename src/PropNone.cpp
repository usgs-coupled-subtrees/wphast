// PropNone.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropNone.h"


// CPropNone dialog

IMPLEMENT_DYNAMIC(CPropNone, CPropPage)

CPropNone::CPropNone()
: CPropPage(CPropNone::IDD)
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
}

CPropNone::~CPropNone()
{
	TRACE("%s(%p)\n", __FUNCTION__, this);
}

void CPropNone::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE_COMBO, ComboType);

	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
		VERIFY(this->ComboType.SelectString(0, _T("None")) != CB_ERR);
	}
}


BEGIN_MESSAGE_MAP(CPropNone, CPropPage)
END_MESSAGE_MAP()


// CPropNone message handlers

BOOL CPropNone::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
	this->AddAnchor(IDC_TYPE_COMBO, TOP_LEFT, TOP_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

