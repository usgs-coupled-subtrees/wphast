#include "StdAfx.h"
#include "GridSetAction.h"

#include "WPhastDoc.h"
#include "GridActor.h"

CGridSetAction::CGridSetAction(CWPhastDoc* pDoc, CGridActor* pActor, const CGridKeyword& gridKeyword)
: m_pDoc(pDoc)
, m_pGridActor(pActor)
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

