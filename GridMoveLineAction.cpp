#include "StdAfx.h"
#include "GridMoveLineAction.h"

#include "WPhastDoc.h"
#include "GridLODActor.h"

//CGridMoveLineAction::CGridMoveLineAction(CGridLODActor *pGridLODActor, CWPhastDoc *pWPhastDoc, int nGridIndex, int nPlaneIndex, double dValue, bool bSkipFirstExecute)
CGridMoveLineAction::CGridMoveLineAction(CGridLODActor *pGridLODActor, CWPhastDoc *pWPhastDoc, GridLineMoveMemento memento, bool bSkipFirstExecute)
: m_pGridLODActor(pGridLODActor)
, m_pWPhastDoc(pWPhastDoc)
, m_memento(memento)
, m_bSkipFirstExecute(bSkipFirstExecute)
{
	ASSERT(this->m_bSkipFirstExecute || this->m_memento.OriginalPlaneIndex != -1);
	ASSERT(this->m_pGridLODActor && this->m_pGridLODActor->IsA("CGridLODActor"));
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
		VERIFY(this->m_pGridLODActor->DeleteLine(this->m_memento.AxisIndex, this->m_memento.OriginalPlaneIndex));
		this->m_memento.OriginalCoord = this->m_pGridLODActor->GetLastDeletedValue();   // nec only when ctor(bSkipFirstExecute = false)
		this->m_memento.NewPlaneIndex = this->m_pGridLODActor->InsertLine(this->m_memento.AxisIndex, this->m_memento.NewCoord);
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
		VERIFY(this->m_pGridLODActor->DeleteLine(this->m_memento.AxisIndex, this->m_memento.NewPlaneIndex));
	}
	VERIFY(this->m_pGridLODActor->InsertLine(this->m_memento.AxisIndex, this->m_memento.OriginalCoord) != -1);
	this->m_pWPhastDoc->UpdateGridDomain();
}
