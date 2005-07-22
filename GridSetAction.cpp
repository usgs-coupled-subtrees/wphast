#include "StdAfx.h"
#include "GridSetAction.h"

#include "WPhastDoc.h"
#include "GridLODActor.h"

CGridSetAction::CGridSetAction(CWPhastDoc* pDoc, CGridLODActor* pActor, const CGridKeyword& gridKeyword)
: m_pDoc(pDoc)
, m_pGridLODActor(pActor)
, m_newGridKeyword(gridKeyword)
{
	pActor->GetGridKeyword(this->m_origGridKeyword);
}

CGridSetAction::~CGridSetAction(void)
{
}

void CGridSetAction::Execute(void)
{
	this->m_pDoc->SetGridKeyword(this->m_newGridKeyword);
	this->m_pDoc->UpdateAllViews(0);
}

void CGridSetAction::UnExecute(void)
{
	this->m_pDoc->SetGridKeyword(this->m_origGridKeyword);
	this->m_pDoc->UpdateAllViews(0);
}

void CGridSetAction::Apply(const CGridKeyword& gridKeyword)
{
	this->m_newGridKeyword = gridKeyword;
	this->Execute();
}

