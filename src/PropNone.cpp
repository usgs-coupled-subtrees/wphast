// PropNone.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropNone.h"


// CPropNone dialog

IMPLEMENT_DYNAMIC(CPropNone, CPropPage)

CPropNone::CPropNone()
: CPropPage(CPropNone::IDD)
// COMMENT: {5/26/2009 8:35:58 PM}, bSkipUpdateData(false)
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
// COMMENT: {5/26/2009 9:16:04 PM}	ON_WM_CTLCOLOR()
// COMMENT: {5/26/2009 9:16:04 PM}	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CPropNone::OnCbnSelchangeTypeCombo)
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

// COMMENT: {5/26/2009 8:36:12 PM}HBRUSH CPropNone::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
// COMMENT: {5/26/2009 8:36:12 PM}{
// COMMENT: {5/26/2009 8:36:12 PM}	if (this->HasWhiteBackground())
// COMMENT: {5/26/2009 8:36:12 PM}	{
// COMMENT: {5/26/2009 8:36:12 PM}		pDC->SetBkMode(TRANSPARENT);
// COMMENT: {5/26/2009 8:36:12 PM}		return ::GetSysColorBrush(COLOR_WINDOW);
// COMMENT: {5/26/2009 8:36:12 PM}	}
// COMMENT: {5/26/2009 8:36:12 PM}
// COMMENT: {5/26/2009 8:36:12 PM}	// default
// COMMENT: {5/26/2009 8:36:12 PM}	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
// COMMENT: {5/26/2009 8:36:12 PM}	return hbr;
// COMMENT: {5/26/2009 8:36:12 PM}}

// COMMENT: {5/26/2009 8:36:16 PM}void CPropNone::OnCbnSelchangeTypeCombo()
// COMMENT: {5/26/2009 8:36:16 PM}{
// COMMENT: {5/26/2009 8:36:16 PM}	// Add your control notification handler code here
// COMMENT: {5/26/2009 8:36:16 PM}	ASSERT(!bSkipUpdateData);
// COMMENT: {5/26/2009 8:36:16 PM}	bSkipUpdateData = true;
// COMMENT: {5/26/2009 8:36:16 PM}	::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)ComboType.GetCurSel(), (LPARAM)0);
// COMMENT: {5/26/2009 8:36:16 PM}	ASSERT(bSkipUpdateData);
// COMMENT: {5/26/2009 8:36:16 PM}	bSkipUpdateData = false;
// COMMENT: {5/26/2009 8:36:16 PM}}

// COMMENT: {5/26/2009 8:36:22 PM}BOOL CPropNone::OnKillActive()
// COMMENT: {5/26/2009 8:36:22 PM}{
// COMMENT: {5/26/2009 8:36:22 PM}	ASSERT_VALID(this);
// COMMENT: {5/26/2009 8:36:22 PM}
// COMMENT: {5/26/2009 8:36:22 PM}	if (!this->bSkipUpdateData)
// COMMENT: {5/26/2009 8:36:22 PM}	{
// COMMENT: {5/26/2009 8:36:22 PM}		if (!UpdateData())
// COMMENT: {5/26/2009 8:36:22 PM}		{
// COMMENT: {5/26/2009 8:36:22 PM}			TRACE(traceAppMsg, 0, "UpdateData failed during page deactivation\n");
// COMMENT: {5/26/2009 8:36:22 PM}			return FALSE;
// COMMENT: {5/26/2009 8:36:22 PM}		}
// COMMENT: {5/26/2009 8:36:22 PM}	}
// COMMENT: {5/26/2009 8:36:22 PM}	return TRUE;
// COMMENT: {5/26/2009 8:36:22 PM}}
