#pragma once
#include "Action.h"

class CWPhastView;
class CZoneActor;
class Polyhedron;

class CZoneSetPolyAction : public CAction
{
public:
	CZoneSetPolyAction(CWPhastView* pView, CZoneActor* pZoneActor, Polyhedron *newpoly);
	virtual ~CZoneSetPolyAction(void);

	virtual void Execute(void);
	virtual void UnExecute(void);

protected:
	CWPhastView   *WPhastView;
	CZoneActor    *ZoneActor;
	Polyhedron    *OriginalPoly;
	Polyhedron    *NewPoly;
	CString        OriginalName;
	CString        NewName;
};
