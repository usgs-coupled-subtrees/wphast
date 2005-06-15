#include "StdAfx.h"
#include "RiverMovePointAction.h"

#include "RiverActor.h"
#include "WPhastDoc.h"


CRiverMovePointAction::CRiverMovePointAction(CRiverActor *pRiverActor, CWPhastDoc *pWPhastDoc, vtkIdType id, double xPos, double yPos)
: m_pRiverActor(pRiverActor)
, m_pWPhastDoc(pWPhastDoc)
, m_id(id)
, m_oldX(xPos)
, m_oldY(yPos)
{
	ASSERT(pRiverActor);
}

CRiverMovePointAction::~CRiverMovePointAction(void)
{
}

void CRiverMovePointAction::SetPoint(double x, double y)
{
	this->m_newX = x;
	this->m_newY = y;
}

void CRiverMovePointAction::Execute(void)
{
	this->m_pRiverActor->MovePoint(this->m_id, this->m_newX, this->m_newY);
	this->m_pWPhastDoc->Select(this->m_pRiverActor);
}

void CRiverMovePointAction::UnExecute(void)
{
	this->m_pRiverActor->MovePoint(this->m_id, this->m_oldX, this->m_oldY);
	this->m_pWPhastDoc->Select(this->m_pRiverActor);
}
