#include "StdAfx.h"
#include "ETSLayoutPropertyPageXP.h"
#include "VisualStylesXP/VisualStylesXP.h"


IMPLEMENT_DYNCREATE(ETSLayoutPropertyPageXP, ETSLayoutPropertyPage)


ETSLayoutPropertyPageXP::ETSLayoutPropertyPageXP(void) : ETSLayoutPropertyPage()
{
}

ETSLayoutPropertyPageXP::ETSLayoutPropertyPageXP( UINT nIDTemplate, UINT nIDCaption /*= 0*/ )
	: ETSLayoutPropertyPage(nIDTemplate, nIDCaption)
{
}

ETSLayoutPropertyPageXP::~ETSLayoutPropertyPageXP()
{
}


BEGIN_MESSAGE_MAP(ETSLayoutPropertyPageXP, ETSLayoutPropertyPage)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL ETSLayoutPropertyPageXP::OnEraseBkgnd(CDC* pDC) 
{
	if (!g_xpStyle.IsAppThemed())
	{
		EraseBkgnd(pDC);
		return TRUE;
	}

	CRect rcChild;
	CWnd* pWndChild = this->GetWindow(GW_CHILD);

	TCHAR szClassName[MAX_PATH];

	while( pWndChild )
	{
		::GetClassName(pWndChild->GetSafeHwnd(), szClassName, MAX_PATH);
		DWORD dwStyle = pWndChild->GetStyle();

		// doesn't make sense for hidden children
		if(dwStyle & WS_VISIBLE)
		{
			if(_tcscmp(szClassName, _T("Button")) == 0)
			{
			}
			else if(_tcscmp(szClassName, WC_TABCONTROL) == 0)
			{
			}
			else
			{
				pWndChild->GetWindowRect(rcChild);
				this->ScreenToClient( rcChild );
				pDC->ExcludeClipRect(rcChild.left, rcChild.top, rcChild.right, rcChild.bottom);
			}
		}
		pWndChild = pWndChild->GetNextWindow();
	}

	Default();
	return TRUE;
}
