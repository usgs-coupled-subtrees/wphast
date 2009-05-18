// PropNone.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropNone.h"


// CPropNone dialog

IMPLEMENT_DYNAMIC(CPropNone, CResizablePage)

CPropNone::CPropNone()
: CResizablePage(CPropNone::IDD)
, bSkipUpdateData(false)
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


BEGIN_MESSAGE_MAP(CPropNone, CResizablePage)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CPropNone::OnCbnSelchangeTypeCombo)
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

HBRUSH CPropNone::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CPropNone::OnCbnSelchangeTypeCombo()
{
	// Add your control notification handler code here
	ASSERT(!bSkipUpdateData);
	bSkipUpdateData = true;
	::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)ComboType.GetCurSel(), (LPARAM)0);
	ASSERT(bSkipUpdateData);
	bSkipUpdateData = false;
}

BOOL CPropNone::OnKillActive()
{
	ASSERT_VALID(this);

	if (!this->bSkipUpdateData)
	{
		if (!UpdateData())
		{
			TRACE(traceAppMsg, 0, "UpdateData failed during page deactivation\n");
			return FALSE;
		}
	}
	return TRUE;
}
