#include "StdAfx.h"
#include "ZoneRemoveAction.h"

#include "WPhastDoc.h"
#include "ZoneActor.h"
#include <vtkPropCollection.h>

CZoneRemoveAction::CZoneRemoveAction(CWPhastDoc* pDoc, CZoneActor* pZoneActor, CPropertyTreeControlBar* pTreeControlBar)
: m_pDoc(pDoc)
, m_pZoneActor(pZoneActor)
, m_pTreeControlBar(pTreeControlBar)
{
	ASSERT_VALID(this->m_pDoc);
	ASSERT(this->m_pZoneActor);
}

CZoneRemoveAction::~CZoneRemoveAction(void)
{
}

void CZoneRemoveAction::Execute()
{
	this->m_pDoc->Delete(this->m_pZoneActor);
}

void CZoneRemoveAction::UnExecute()
{
	this->m_pDoc->UnDelete(this->m_pZoneActor);
}

