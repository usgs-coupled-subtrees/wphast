// ModelessPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ModelessPropertySheet.h"
#include "WPhastDoc.h"
#include "MacroAction.h"



// CModelessPropertySheet

IMPLEMENT_DYNAMIC(CModelessPropertySheet, CPropertySheet)
CModelessPropertySheet::CModelessPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	, m_pAction(0)
{
}

CModelessPropertySheet::CModelessPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_pAction(0)
{
}

CModelessPropertySheet::~CModelessPropertySheet()
{
	ASSERT(this->m_pAction == 0);
}


BEGIN_MESSAGE_MAP(CModelessPropertySheet, CPropertySheet)
	ON_COMMAND( IDOK, OnOK )
	ON_COMMAND( IDCANCEL, OnCancel )
END_MESSAGE_MAP()


// CModelessPropertySheet message handlers

BOOL CModelessPropertySheet::OnInitDialog()
{
	m_bModeless = FALSE;   
	m_nFlags |= WF_CONTINUEMODAL;

	BOOL bResult = CPropertySheet::OnInitDialog();

	// Add your specialized code here

	m_bModeless = TRUE;
	m_nFlags &= ~WF_CONTINUEMODAL;
	return bResult;
}

void CModelessPropertySheet::AddAction(CAction* pAction)
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

void CModelessPropertySheet::OnOK()
{
	TRACE("CModelessPropertySheet::OnOK()\n");
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

void CModelessPropertySheet::OnCancel()
{
	TRACE("CModelessPropertySheet::OnCancel()\n");
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

void CModelessPropertySheet::PostNcDestroy()
{
	if (this->m_pAction)
	{
		delete this->m_pAction;
		this->m_pAction = 0;
	}
	CPropertySheet::PostNcDestroy();
}
