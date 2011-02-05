#include "StdAfx.h"
#include "WellSetPositionAction.h"

#include "WellActor.h"
#include "WPhastDoc.h"
#include "GridKeyword.h"

#define x x_user
#define y y_user

CWellSetPositionAction::CWellSetPositionAction(CWellActor *pWellActor, CWPhastDoc *pWPhastDoc, double xPos, double yPos, PHAST_Transform::COORDINATE_SYSTEM cs)
: m_pWellActor(pWellActor)
, m_pWPhastDoc(pWPhastDoc)
, m_newX(xPos)
, m_newY(yPos)
, m_oldX(pWellActor->GetWell().x)
, m_oldY(pWellActor->GetWell().y)
, m_newCoorSys(cs)
, m_oldCoorSys(pWellActor->GetWell().xy_coordinate_system_user)
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
	well.xy_coordinate_system_user = this->m_newCoorSys;
	this->m_pWellActor->SetWell(well, this->m_pWPhastDoc->GetUnits(), this->m_pWPhastDoc->GetGridKeyword());

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
	well.xy_coordinate_system_user = this->m_oldCoorSys;
	this->m_pWellActor->SetWell(well, this->m_pWPhastDoc->GetUnits(), this->m_pWPhastDoc->GetGridKeyword());

	CTreeCtrlNode top = this->m_pWellActor->GetTreeCtrlNode().GetFirstVisible();
	this->m_pWellActor->Update(this->m_pWellActor->GetTreeCtrlNode());
	top.SelectSetFirstVisible();
	this->m_pWPhastDoc->Select(this->m_pWellActor);
}

void CWellSetPositionAction::SetPosition(double x, double y, PHAST_Transform::COORDINATE_SYSTEM cs)
{
	this->m_newX       = x;
	this->m_newY       = y;
	this->m_newCoorSys = cs;
}
