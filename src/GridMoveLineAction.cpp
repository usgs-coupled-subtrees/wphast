#include "StdAfx.h"
#include "GridMoveLineAction.h"

#include "WPhastDoc.h"
#include "GridActor.h"

//CGridMoveLineAction::CGridMoveLineAction(CGridActor *pGridActor, CWPhastDoc *pWPhastDoc, int nGridIndex, int nPlaneIndex, double dValue, bool bSkipFirstExecute)
CGridMoveLineAction::CGridMoveLineAction(CGridActor *pGridActor, CWPhastDoc *pWPhastDoc, GridLineMoveMemento memento, bool bSkipFirstExecute)
: m_pGridActor(pGridActor)
, m_pWPhastDoc(pWPhastDoc)
, m_memento(memento)
, m_bSkipFirstExecute(bSkipFirstExecute)
{
	ASSERT(this->m_bSkipFirstExecute || this->m_memento.OriginalPlaneIndex != -1);
	ASSERT(this->m_pGridActor && this->m_pGridActor->IsA("CGridActor"));
	ASSERT( (0 <= this->m_memento.AxisIndex) && (this->m_memento.AxisIndex < 3) );
}

CGridMoveLineAction::~CGridMoveLineAction(void)
{
	ASSERT( (0 <= this->m_memento.AxisIndex) && (this->m_memento.AxisIndex < 3) );
}

void CGridMoveLineAction::Execute()
{
	ASSERT( (0 <= this->m_memento.AxisIndex) && (this->m_memento.AxisIndex < 3) );
	if (!this->m_bSkipFirstExecute)
	{
		CGridKeyword grid;
		this->m_pGridActor->GetGridKeyword(grid);
		this->m_memento.Uniform = grid.m_grid[this->m_memento.AxisIndex].uniform;

		VERIFY(this->m_pGridActor->DeleteLine(this->m_memento.AxisIndex, this->m_memento.OriginalPlaneIndex));
		this->m_memento.OriginalCoord = this->m_pGridActor->GetLastDeletedValue();   // nec only when ctor(bSkipFirstExecute = false)
		this->m_memento.NewPlaneIndex = this->m_pGridActor->InsertLine(this->m_memento.AxisIndex, this->m_memento.NewCoord);
	}
	this->m_bSkipFirstExecute = false;

	// do this everytime
	//
	this->m_pWPhastDoc->UpdateGridDomain();
}

void CGridMoveLineAction::UnExecute()
{
	ASSERT( (0 <= this->m_memento.AxisIndex) && (this->m_memento.AxisIndex < 3) );
	ASSERT(this->m_memento.NewPlaneIndex != -1);
	if (this->m_memento.NewPlaneIndex != -1)
	{
		VERIFY(this->m_pGridActor->DeleteLine(this->m_memento.AxisIndex, this->m_memento.NewPlaneIndex));
	}
	VERIFY(this->m_pGridActor->InsertLine(this->m_memento.AxisIndex, this->m_memento.OriginalCoord) != -1);
	if (this->m_memento.Uniform)
	{
		CGridKeyword grid;
		this->m_pGridActor->GetGridKeyword(grid);
		int i = this->m_memento.AxisIndex;
		grid.m_grid[i].uniform = 1;
		grid.m_grid[i].coord[1] = grid.m_grid[i].coord[grid.m_grid[i].count_coord - 1];
		grid.m_grid[i].uniform_expanded = 0;
		this->m_pGridActor->SetGridKeyword(grid, m_pWPhastDoc->GetUnits());
		this->m_pGridActor->UpdateNode();
	}
	this->m_pWPhastDoc->UpdateGridDomain();
}
