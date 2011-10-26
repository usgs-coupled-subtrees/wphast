#include "StdAfx.h"
#include "SetFreeSurfaceAction.h"

#include <map>
#include "WPhastDoc.h"
#include "WPhastView.h"

CSetFreeSurfaceAction::CSetFreeSurfaceAction(CWPhastDoc* pDoc, const CFreeSurface& freeSurface)
: m_pDoc(pDoc)
, m_oriFreeSurface(pDoc->GetFreeSurface())
, m_newFreeSurface(freeSurface)
{
}

CSetFreeSurfaceAction::~CSetFreeSurfaceAction(void)
{
}

void CSetFreeSurfaceAction::Execute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetFreeSurface(this->m_newFreeSurface);
}

void CSetFreeSurfaceAction::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->SetFreeSurface(this->m_oriFreeSurface);
}
