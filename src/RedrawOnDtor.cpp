#include "StdAfx.h"
#include "RedrawOnDtor.h"

CRedrawOnDtor::CRedrawOnDtor(CWnd *pWnd)
: m_pWnd(pWnd)
{
}

CRedrawOnDtor::~CRedrawOnDtor(void)
{
	if (this->m_pWnd) m_pWnd->RedrawWindow();
}
