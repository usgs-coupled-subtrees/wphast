#pragma once

#include "Action.h"

class CWPhastView;
class CZoneActor;
class Prism;

class CZonePrismResetAction : public CAction
{
public:
	CZonePrismResetAction(CWPhastView* pView, CZoneActor* pZoneActor, Prism *newprism);
	virtual ~CZonePrismResetAction(void);
	virtual void Execute(void);
	virtual void UnExecute(void);

protected:
	void Store(void);

protected:
	CWPhastView   *WPhastView;
	CZoneActor    *ZoneActor;
	Prism         *OriginalPrism;
	Prism         *NewPrism;
};
