#include "StdAfx.h"
#include "WellSetPositionAction.h"

#include "WellActor.h"
#include "WPhastDoc.h"


CWellSetPositionAction::CWellSetPositionAction(CWellActor *pWellActor, CWPhastDoc *pWPhastDoc, double xPos, double yPos)
: m_pWellActor(pWellActor)
, m_pWPhastDoc(pWPhastDoc)
, m_newX(xPos)
, m_newY(yPos)
, m_oldX(pWellActor->GetWell().x)
, m_oldY(pWellActor->GetWell().y)
{
	ASSERT(pWellActor);
}

CWellSetPositionAction::~CWellSetPositionAction(void)
{
}

void CWellSetPositionAction::Execute(void)
{
	CWellSchedule well = this->m_pWellActor->GetWell();
	well.x = this->m_newX;
	well.y = this->m_newY;
	this->m_pWellActor->SetWell(well, this->m_pWPhastDoc->GetUnits());

	CTreeCtrlNode top = this->m_pWellActor->GetTreeCtrlNode().GetFirstVisible();
	this->m_pWellActor->Update(this->m_pWellActor->GetTreeCtrlNode());
	top.SelectSetFirstVisible();
	this->m_pWPhastDoc->Select(this->m_pWellActor);
}

void CWellSetPositionAction::UnExecute(void)
{
	CWellSchedule well = this->m_pWellActor->GetWell();
	well.x = this->m_oldX;
	well.y = this->m_oldY;
	this->m_pWellActor->SetWell(well, this->m_pWPhastDoc->GetUnits());

	CTreeCtrlNode top = this->m_pWellActor->GetTreeCtrlNode().GetFirstVisible();
	this->m_pWellActor->Update(this->m_pWellActor->GetTreeCtrlNode());
	top.SelectSetFirstVisible();
	this->m_pWPhastDoc->Select(this->m_pWellActor);
}

void CWellSetPositionAction::SetPosition(double x, double y)
{
	this->m_newX = x;
	this->m_newY = y;
}
