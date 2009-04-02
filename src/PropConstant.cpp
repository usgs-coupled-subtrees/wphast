// PropConstant.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropConstant.h"


// CPropConstant dialog

IMPLEMENT_DYNAMIC(CPropConstant, CResizablePage)

CPropConstant::CPropConstant()
: CResizablePage(CPropConstant::IDD)
{
}

CPropConstant::~CPropConstant()
{
}

void CPropConstant::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE_COMBO, ComboType);

	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
		VERIFY(this->ComboType.SelectString(0, _T("Constant")) != CB_ERR);
	}
#ifdef _DEBUG
	if (pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_VALUE_EDIT, this->value);
	}
	else
	{
		DDX_Text(pDX, IDC_VALUE_EDIT, this->value);
	}
#else
	DDX_Text(pDX, IDC_VALUE_EDIT, this->value);
#endif
}


BEGIN_MESSAGE_MAP(CPropConstant, CResizablePage)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CPropConstant::OnCbnSelchangeTypeCombo)
END_MESSAGE_MAP()


// CPropConstant message handlers

BOOL CPropConstant::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  Add extra initialization here
	this->AddAnchor(IDC_TYPE_COMBO, TOP_LEFT, TOP_RIGHT);
	this->AddAnchor(IDC_VALUE_EDIT, TOP_LEFT, TOP_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CPropConstant::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CPropConstant::OnCbnSelchangeTypeCombo()
{
	// Add your control notification handler code here
	::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)ComboType.GetCurSel(), (LPARAM)0);
}
