#pragma once

#include "Action.h"
#include "srcWedgeSource.h"

class CWPhastView;
class CZoneActor;

class WedgeChangeChopTypeAction : public CAction
{
public:
	WedgeChangeChopTypeAction(CWPhastView* pView, CZoneActor* pZone, srcWedgeSource::ChopType ct);
	virtual ~WedgeChangeChopTypeAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastView             *pView;
	CZoneActor              *pZone;
	srcWedgeSource::ChopType OriginalChopType;
	srcWedgeSource::ChopType NewChopType;
};
