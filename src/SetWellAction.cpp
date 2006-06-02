#include "StdAfx.h"
#include "SetWellAction.h"

#include "WellActor.h"
#include "WPhastDoc.h"


CSetWellAction::CSetWellAction(CWellActor *pWellActor, const CWellSchedule& newWell, CWPhastDoc *pWPhastDoc)
: m_pWellActor(pWellActor)
, m_oldWell(pWellActor->GetWell())
, m_newWell(newWell)
, m_pWPhastDoc(pWPhastDoc)
{
	ASSERT(pWellActor);
	ASSERT_VALID(pWPhastDoc);
}

CSetWellAction::~CSetWellAction(void)
{
}

void CSetWellAction::Execute(void)
{
	this->SetWell(this->m_newWell);
}

void CSetWellAction::UnExecute(void)
{
	this->SetWell(this->m_oldWell);
}

void CSetWellAction::SetWell(const CWellSchedule &well)
{
	this->m_pWellActor->SetWell(well, this->m_pWPhastDoc->GetUnits());
	CTreeCtrlNode top = this->m_pWellActor->GetTreeCtrlNode().GetFirstVisible();
	this->m_pWellActor->Update(this->m_pWellActor->GetTreeCtrlNode());
	top.SelectSetFirstVisible();
	this->m_pWPhastDoc->Select(this->m_pWellActor);
}
