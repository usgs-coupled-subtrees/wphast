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
#if !defined(__CPPUNIT__)
	this->m_pDoc->SetGridKeyword(this->m_newGridKeyword);
	this->m_pDoc->UpdateAllViews(0);
#else
	CGridKeyword gridK(this->m_newGridKeyword);
	for (int i = 0; i < 360; i+=10)
	{
		gridK.m_grid_angle -= 10;
		this->m_pDoc->SetGridKeyword(gridK);
		this->m_pDoc->UpdateAllViews(0);
	}
	this->m_pDoc->SetGridKeyword(this->m_newGridKeyword);
	this->m_pDoc->UpdateAllViews(0);
#endif
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

