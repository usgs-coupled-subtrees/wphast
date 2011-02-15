#pragma once

#include "WPhastDoc.h"
#include "WPhastView.h"

class CDelayRedraw
{
public:
	CDelayRedraw(CWnd *pWnd);
	CDelayRedraw(CWnd *pWnd, CDocument *pDoc);
	~CDelayRedraw(void);
protected:
	CWnd *m_pWnd;
	CDocument *m_pDoc;
	BOOL m_bVisible;
};

inline CDelayRedraw::CDelayRedraw(CWnd *pWnd)
: m_pWnd(pWnd)
, m_pDoc(0)
{
	m_bVisible = m_pWnd && m_pWnd->IsWindowVisible();
	if (m_bVisible)
	{
		if (m_pWnd)
		{
			m_pWnd->SetRedraw(FALSE);
		}
	}
}

inline CDelayRedraw::CDelayRedraw(CWnd *pWnd, CDocument *pDoc)
: m_pWnd(pWnd)
, m_pDoc(pDoc)
{
	m_bVisible = m_pWnd && m_pWnd->IsWindowVisible();
	if (m_bVisible)
	{
		if (m_pWnd)
		{
			m_pWnd->SetRedraw(FALSE);
		}
	}
	if (this->m_pDoc)
	{
		if (POSITION pos = this->m_pDoc->GetFirstViewPosition())
		{
			if (CWPhastView *pView = dynamic_cast<CWPhastView*>(this->m_pDoc->GetNextView(pos)))
			{					
				pView->GetInteractor()->SetEnableRender(false);
			}
		}
	}
}

inline CDelayRedraw::~CDelayRedraw(void)
{
	if (this->m_pDoc)
	{
		if (POSITION pos = this->m_pDoc->GetFirstViewPosition())
		{
			if (CWPhastView *pView = dynamic_cast<CWPhastView*>(this->m_pDoc->GetNextView(pos)))
			{					
				pView->GetInteractor()->SetEnableRender(true);
			}
		}
	}

	if (m_bVisible)
	{
		if (m_pWnd)
		{
			m_pWnd->SetRedraw(TRUE);
			m_pWnd->Invalidate();
			m_pWnd->SendMessage(WM_NCPAINT);  // scrollbars
			m_pWnd->RedrawWindow();
		}
		if (m_pDoc)
		{
			m_pDoc->UpdateAllViews(0);
		}
	}
}
