#include "StdAfx.h"
#include "ResizeGridAction.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

#include <list>
#include "Zone.h"

CResizeGridAction::CResizeGridAction(CWPhastDoc* pDoc, CGrid* grids, CGridLODActor* pActor)
: m_pDoc(pDoc)
{
	ASSERT(pDoc);
	this->Reset(grids);
	if (pActor) {
		this->Store(pActor);
	}
}

CResizeGridAction::~CResizeGridAction(void)
{
}

void CResizeGridAction::Reset(CGrid* grids)
{
	this->m_Grid[0] = grids[0];
	this->m_Grid[1] = grids[1];
	this->m_Grid[2] = grids[2];
}

void CResizeGridAction::Store(CGridLODActor* pActor)
{
	ASSERT(pActor);
	/////pActor->GetGrid(&this->m_OriginalGrid[0], &this->m_OriginalGrid[1], &this->m_OriginalGrid[2]);
	pActor->GetGrid(this->m_OriginalGrid[0], this->m_OriginalGrid[1], this->m_OriginalGrid[2]);
	//this->m_OriginalGrid[0].coord[1] = this->m_OriginalGrid[0].coord[this->m_OriginalGrid[0].count_coord - 1];
	//this->m_OriginalGrid[1].coord[1] = this->m_OriginalGrid[1].coord[this->m_OriginalGrid[1].count_coord - 1];
	//this->m_OriginalGrid[2].coord[1] = this->m_OriginalGrid[2].coord[this->m_OriginalGrid[2].count_coord - 1];
}

void CResizeGridAction::Execute(void)
{
	this->m_pDoc->ResizeGrid(this->m_Grid[0], this->m_Grid[1], this->m_Grid[2]);
	this->m_pDoc->UpdateAllViews(0);
}

void CResizeGridAction::UnExecute(void)
{
	this->m_pDoc->ResizeGrid(this->m_OriginalGrid[0], this->m_OriginalGrid[1], this->m_OriginalGrid[2]);
	this->m_pDoc->UpdateAllViews(0);
}


