// PropNone.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropPage.h"


// CPropNone dialog

IMPLEMENT_DYNAMIC(CPropPage, CResizablePage)

CPropPage::CPropPage()
: SkipUpdateData(false)
{
}

CPropPage::CPropPage(UINT nIDTemplate, UINT nIDCaption)
: CResizablePage(nIDTemplate, nIDCaption)
, SkipUpdateData(false)
{
}

CPropPage::CPropPage(LPCTSTR lpszTemplateName, UINT nIDCaption)
: CResizablePage(lpszTemplateName, nIDCaption)
, SkipUpdateData(false)
{
}


CPropPage::~CPropPage()
{
}

void CPropPage::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE_COMBO, ComboType);
}

BEGIN_MESSAGE_MAP(CPropPage, CResizablePage)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CPropPage::OnCbnSelchangeTypeCombo)
END_MESSAGE_MAP()


// CPropNone message handlers

HBRUSH CPropPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CPropPage::OnCbnSelchangeTypeCombo()
{
	// Add your control notification handler code here
	ASSERT(!this->SkipUpdateData);
	this->SkipUpdateData = true;
	if (this->ComboType.GetSafeHwnd())
	{
		::SendMessage(*this->GetParent(), PSM_SETCURSEL, (WPARAM)this->ComboType.GetCurSel(), (LPARAM)0);
	}
	ASSERT(this->SkipUpdateData);
	this->SkipUpdateData = false;
}

BOOL CPropPage::OnKillActive()
{
	ASSERT_VALID(this);

	if (!this->SkipUpdateData)
	{
		if (!this->UpdateData())
		{
			TRACE(traceAppMsg, 0, "UpdateData failed during page deactivation\n");
			return FALSE;
		}
	}
	return TRUE;
}
