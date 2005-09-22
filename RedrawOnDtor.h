#pragma once

class CRedrawOnDtor
{
public:
	CRedrawOnDtor(CWnd *pWnd);
	~CRedrawOnDtor(void);
protected:
	CWnd *m_pWnd;
};
