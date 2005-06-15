#pragma once
#include "Action.h"

class CRiverActor;
class CWPhastDoc;

class CRiverMovePointAction : public CAction
{
public:
	CRiverMovePointAction(CRiverActor *pRiverActor, CWPhastDoc *pWPhastDoc, vtkIdType id, double xPos, double yPos);
	~CRiverMovePointAction(void);

	void SetPoint(double x, double y);

	virtual void Execute(void);
	virtual void UnExecute(void);

protected:
	CRiverActor   *m_pRiverActor;
	CWPhastDoc    *m_pWPhastDoc;
	vtkIdType      m_id;
	double         m_newX;
	double         m_newY;
	double         m_oldX;
	double         m_oldY;
};
