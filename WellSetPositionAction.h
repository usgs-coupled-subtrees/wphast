#pragma once
#include "Action.h"

class CWellActor;
class CWPhastDoc;

class CWellSetPositionAction : public CAction
{
public:
	CWellSetPositionAction(CWellActor *pWellActor, CWPhastDoc *pWPhastDoc, double xPos, double yPos);
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
};
