#pragma once
#include "Action.h"

class CWPhastView;
class CZoneActor;
class Polyhedron;

class CZoneWidgetResizeAction : public CAction
{
public:
	CZoneWidgetResizeAction(CWPhastView* pView, CZoneActor* pZoneActor);
	virtual ~CZoneWidgetResizeAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void Store();

protected:
	CWPhastView   *m_pView;
	CZoneActor    *m_pActor;
	Polyhedron *m_OrigPolyHedron;
	Polyhedron *m_NewPolyHedron;
};
