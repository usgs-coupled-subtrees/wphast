#pragma once

class CDelayRedraw
{
public:
	CDelayRedraw(CWnd *pWnd);
	~CDelayRedraw(void);
protected:
	CWnd *m_pWnd;
};

inline CDelayRedraw::CDelayRedraw(CWnd *pWnd)
: m_pWnd(pWnd)
{
	if (m_pWnd) m_pWnd->SetRedraw(FALSE);
}

inline CDelayRedraw::~CDelayRedraw(void)
{
	if (m_pWnd) m_pWnd->SetRedraw(TRUE);
}
