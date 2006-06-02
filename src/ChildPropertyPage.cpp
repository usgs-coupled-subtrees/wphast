// ChildPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ChildPropertyPage.h"


// CChildPropertyPage dialog

IMPLEMENT_DYNAMIC(CChildPropertyPage, CPropertyPage)

CChildPropertyPage::CChildPropertyPage(UINT nIDTemplate, UINT nIDCaption, DWORD dwSize, UINT nID)
	: CPropertyPage(nIDTemplate, nIDCaption, dwSize)
	, m_nID(nID)
{
}

CChildPropertyPage::CChildPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption, DWORD dwSize, UINT nID)
	: CPropertyPage(lpszTemplateName, nIDCaption, dwSize)
	, m_nID(nID)
{
}

CChildPropertyPage::~CChildPropertyPage()
{
}

void CChildPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChildPropertyPage, CPropertyPage)
	ON_MESSAGE(PSM_QUERYSIBLINGS, OnQuerySiblings)
END_MESSAGE_MAP()


// CChildPropertyPage message handlers

void CChildPropertyPage::AddPage(CPropertyPage* pPage)
{
	ASSERT_VALID(this);
	ASSERT(pPage != NULL);
	ASSERT_KINDOF(CPropertyPage, pPage);
	ASSERT_VALID(pPage);

	// add page to internal list
	this->m_arrPages.Add(pPage);
}

CPropertyPage* CChildPropertyPage::GetActivePage() const
{
	return this->m_sheet.GetActivePage();
}

void CChildPropertyPage::OnOK()
{
	if (GetPageCount() > 0) this->m_sheet.PressButton(PSBTN_OK);
    CPropertyPage::OnOK();
}

void CChildPropertyPage::OnCancel()
{
	if (GetPageCount() > 0) this->m_sheet.PressButton(PSBTN_CANCEL);
    CPropertyPage::OnCancel();
}

LRESULT CChildPropertyPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
	CPropertyPage* pPage = this->GetActivePage();
	return (pPage) ? pPage->QuerySiblings(wParam, lParam) : 0;
}

BOOL CChildPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	if (this->GetPageCount() > 0) 
	{
		for (int i = 0; i < this->m_arrPages.GetSize(); i++)
		{
			this->m_sheet.AddPage(this->m_arrPages[i]);
		}

		// only have tabs in one row
		this->m_sheet.EnableStackedTabs(FALSE);
		// create the child sheet
		this->m_sheet.Create(this, WS_CHILD | WS_VISIBLE | WS_TABSTOP);
		// Fix for: "Child CPropertySheet Hangs If Focus Is Switched" (Q149501)
		this->m_sheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);

		if (CWnd* pWnd = (m_nID) ? GetDlgItem(m_nID) : 0)
		{
			CRect rcPage, rcSheet;
			GetWindowRect(rcPage);
			pWnd->GetWindowRect(rcSheet);
			// offset the sheet from the parent page
			rcSheet.OffsetRect(-rcPage.left, -rcPage.top);
			// move it
			this->m_sheet.MoveWindow(rcSheet);
			this->m_sheet.GetTabControl()->MoveWindow(0, 0, rcSheet.Width(), rcSheet.Height());
			// repaint it
			this->m_sheet.SetActivePage(this->m_sheet.m_psh.nStartPage);
		}
		else
		{
			CRect rcSheet;
			GetClientRect(&rcSheet);
			m_sheet.MoveWindow(rcSheet);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CChildPropertyPage::GetPageCount() const
{
	ASSERT_VALID(this);
	return (int)m_arrPages.GetSize();
}
