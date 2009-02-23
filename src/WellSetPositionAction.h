#pragma once
#include "Action.h"
#include "srcinput/PHAST_Transform.h"

class CWellActor;
class CWPhastDoc;

class CWellSetPositionAction : public CAction
{
public:
	CWellSetPositionAction(CWellActor *pWellActor, CWPhastDoc *pWPhastDoc, double xPos, double yPos, PHAST_Transform::COORDINATE_SYSTEM cs);
	virtual ~CWellSetPositionAction(void);

	virtual void Execute(void);
	virtual void UnExecute(void);

	void SetPosition(double x, double y);

protected:
	CWellActor    *m_pWellActor;
	CWPhastDoc    *m_pWPhastDoc;
	double         m_newX;
	double         m_newY;
	double         m_oldX;
	double         m_oldY;
	PHAST_Transform::COORDINATE_SYSTEM m_newCoorSys;
	PHAST_Transform::COORDINATE_SYSTEM m_oldCoorSys;
};
