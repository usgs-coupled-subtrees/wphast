#include "StdAfx.h"
#include "ETSLayoutModelessPropertySheet.h"

#include <map>

#include "WPhastDoc.h"
#include "MacroAction.h"


IMPLEMENT_DYNAMIC(ETSLayoutModelessPropertySheet, ETSLayoutPropertySheet)
ETSLayoutModelessPropertySheet::ETSLayoutModelessPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: ETSLayoutPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	, m_pAction(0)
{
}

ETSLayoutModelessPropertySheet::ETSLayoutModelessPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: ETSLayoutPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_pAction(0)
{
}

ETSLayoutModelessPropertySheet::~ETSLayoutModelessPropertySheet(void)
{
	ASSERT(this->m_pAction == 0);
}


BEGIN_MESSAGE_MAP(ETSLayoutModelessPropertySheet, ETSLayoutPropertySheet)
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(IDCANCEL, OnCancel)	
END_MESSAGE_MAP()


BOOL ETSLayoutModelessPropertySheet::OnInitDialog()
{
	m_bModeless = FALSE;   
	m_nFlags |= WF_CONTINUEMODAL;

	BOOL bResult = ETSLayoutPropertySheet::OnInitDialog();

	m_bModeless = TRUE;
	m_nFlags &= ~WF_CONTINUEMODAL;

	return bResult;
}


void ETSLayoutModelessPropertySheet::AddAction(CAction* pAction)
{
	if (pAction != 0)
	{
		if (this->m_pAction == 0)
		{
			this->m_pAction = new CMacroAction();
			if (this->m_pAction == 0) ::AfxMessageBox("Out of memory", MB_OK);
		}
		if (this->m_pAction != 0)
		{
			this->m_pAction->Add(pAction);
		}
	}
}

void ETSLayoutModelessPropertySheet::OnOK()
{
	TRACE("ETSLayoutModelessPropertySheet::OnOK()\n");
	Default();
	if (PropSheet_GetCurrentPageHwnd(this->m_hWnd) == 0)
	{
		if (this->m_pAction)
		{
			if (CFrameWnd *pFrame = reinterpret_cast<CFrameWnd*>(AfxGetApp()->m_pMainWnd))
			{
				ASSERT_KINDOF(CFrameWnd, pFrame);
				ASSERT_VALID(pFrame);
				if (CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument()))
				{
					ASSERT_KINDOF(CWPhastDoc, pDoc);
					ASSERT_VALID(pDoc);
					pDoc->Execute(this->m_pAction);
				}
			}
			this->m_pAction = 0;
		}
		this->EndDialog(IDOK);
	}
}

void ETSLayoutModelessPropertySheet::OnCancel()
{
	TRACE("ETSLayoutModelessPropertySheet::OnCancel()\n");
	Default();
	if (PropSheet_GetCurrentPageHwnd(this->m_hWnd) == 0)
	{
		if (this->m_pAction)
		{
			delete this->m_pAction;
			this->m_pAction = 0;
		}
		this->EndDialog(IDCANCEL);
	}
}

void ETSLayoutModelessPropertySheet::PostNcDestroy()
{
	if (this->m_pAction)
	{
		delete this->m_pAction;
		this->m_pAction = 0;
	}
	ETSLayoutPropertySheet::PostNcDestroy();
}
