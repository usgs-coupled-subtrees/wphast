#pragma once
#include "Action.h"

class CWPhastDoc;

template<typename TActor>
class CPointConnectorMovePointAction : public CAction
{
public:
	CPointConnectorMovePointAction(TActor *pActor, CWPhastDoc *pWPhastDoc, vtkIdType id, double xPos, double yPos);
	~CPointConnectorMovePointAction(void);

	void SetPoint(double x, double y);
	TActor* GetActor(void) { return Actor; }

	virtual void Execute(void);
	virtual void UnExecute(void);

protected:
	CWPhastDoc    *WPhastDoc;
	TActor        *Actor;
	vtkIdType      Id;
	double         newX;
	double         newY;
	double         oldX;
	double         oldY;
};


template<typename TActor>
CPointConnectorMovePointAction<TActor>::CPointConnectorMovePointAction(TActor *pActor, CWPhastDoc *pWPhastDoc, vtkIdType id, double xPos, double yPos)
: Actor(pActor)
, WPhastDoc(pWPhastDoc)
, Id(id)
, oldX(xPos)
, oldY(yPos)
{
	ASSERT(this->Actor);
}

template<typename TActor>
CPointConnectorMovePointAction<TActor>::~CPointConnectorMovePointAction(void)
{
}

template<typename TActor>
void CPointConnectorMovePointAction<TActor>::SetPoint(double x, double y)
{
	this->newX = x;
	this->newY = y;
}

template<typename TActor>
void CPointConnectorMovePointAction<TActor>::Execute(void)
{
	this->Actor->MovePoint(this->Id, this->newX, this->newY);
	this->WPhastDoc->Select(this->Actor);
}

template<typename TActor>
void CPointConnectorMovePointAction<TActor>::UnExecute(void)
{
	this->Actor->MovePoint(this->Id, this->oldX, this->oldY);
	this->WPhastDoc->Select(this->Actor);
}
