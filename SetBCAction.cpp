#include "StdAfx.h"
#include "SetBCAction.h"

#include "BCZoneActor.h"

CSetBCAction::CSetBCAction(CBCZoneActor* pBCZoneActor, CTreeCtrl* pTreeCtrl, const CBC& newBC, int nStressPeriod)
	: m_pBCZoneActor(pBCZoneActor)
	, m_pTreeCtrl(pTreeCtrl)
	, m_oldBC(pBCZoneActor->GetBC(nStressPeriod))
	, m_newBC(newBC)
	, m_nStressPeriod(nStressPeriod)
{
}

CSetBCAction::~CSetBCAction(void)
{
}

void CSetBCAction::Execute(void)
{
	this->SetBC(this->m_newBC);
}

void CSetBCAction::UnExecute(void)
{
	this->SetBC(this->m_oldBC);
}

void CSetBCAction::SetBC(const CBC& rBC)
{
	this->m_pBCZoneActor->SetBC(rBC, this->m_nStressPeriod);
	this->m_pBCZoneActor->Update(this->m_pTreeCtrl, this->m_nStressPeriod);

	// update bc_solution_type if changed in stress period 1
	if (this->m_nStressPeriod == 1) {
		if (this->m_oldBC.bc_solution_type != this->m_newBC.bc_solution_type) {
			int nSimCount = this->m_pBCZoneActor->GetStressPeriodCount();
			for (int nSim = 2; nSim <= nSimCount; ++nSim) {
				CBC bc(rBC);
				bc.ClearProperties();
				bc.Merge(this->m_pBCZoneActor->GetBC(nSim));
				this->m_pBCZoneActor->SetBC(bc, nSim);
				this->m_pBCZoneActor->Update(this->m_pTreeCtrl, nSim);
			}
		}
	}
}

