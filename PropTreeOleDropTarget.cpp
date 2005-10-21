// PropTreeOleDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PropTreeOleDropTarget.h"

#include "Tree.h"
#include "PropertyTreeControlBar.h"


// CPropTreeOleDropTarget

IMPLEMENT_DYNAMIC(CPropTreeOleDropTarget, COleDropTarget)
CPropTreeOleDropTarget::CPropTreeOleDropTarget()
: Handler(NULL)
{
}

CPropTreeOleDropTarget::~CPropTreeOleDropTarget()
{
}


BEGIN_MESSAGE_MAP(CPropTreeOleDropTarget, COleDropTarget)
END_MESSAGE_MAP()

void CPropTreeOleDropTarget::SetHandler(CPropertyTreeControlBar* handler)
{
	this->Handler = handler;
}

// CPropTreeOleDropTarget message handlers

DROPEFFECT CPropTreeOleDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pWnd);
	ASSERT_KINDOF(CTreeCtrlEx, pWnd);

	if (this->Handler)
	{
		return (this->Handler)->OnDragEnter(pWnd, pDataObject, dwKeyState, point);
	}
	return COleDropTarget::OnDragEnter(pWnd, pDataObject, dwKeyState, point);
}

DROPEFFECT CPropTreeOleDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pWnd);
	ASSERT_KINDOF(CTreeCtrlEx, pWnd);

	if (this->Handler)
	{
		return (this->Handler)->OnDragOver(pWnd, pDataObject, dwKeyState, point);
	}
	return COleDropTarget::OnDragOver(pWnd, pDataObject, dwKeyState, point);
}

BOOL CPropTreeOleDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pWnd);
	ASSERT_KINDOF(CTreeCtrlEx, pWnd);

	if (this->Handler)
	{
		return (this->Handler)->OnDrop(pWnd, pDataObject, dropEffect, point);
	}
	return COleDropTarget::OnDrop(pWnd, pDataObject, dropEffect, point);
}

void CPropTreeOleDropTarget::OnDragLeave(CWnd* pWnd)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pWnd);
	ASSERT_KINDOF(CTreeCtrlEx, pWnd);

	if (this->Handler)
	{
		return (this->Handler)->OnDragLeave(pWnd);
	}
	COleDropTarget::OnDragLeave(pWnd);
}
