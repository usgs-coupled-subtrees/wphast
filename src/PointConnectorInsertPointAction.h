#pragma once
#include "Action.h"

#include <vtkIdType.h>

class CWPhastDoc;

template <typename TActor, typename TPoint>
class CPointConnectorInsertPointAction : public CAction
{
public:
	CPointConnectorInsertPointAction(TActor *pActor, CWPhastDoc *pWPhastDoc, vtkIdType id, double x, double y, double z, bool bSkipFirstExecute = false);
	~CPointConnectorInsertPointAction(void);
	virtual void Execute(void);
	virtual void UnExecute(void);

protected:
	TActor      *Actor;
	CWPhastDoc  *WPhastDoc;
	vtkIdType    Id;
	double       X;
	double       Y;
	double       Z;
	bool         bSkipFirstExecute;
};

template <typename TActor, typename TPoint>
CPointConnectorInsertPointAction<TActor, TPoint>::CPointConnectorInsertPointAction(TActor *pActor, CWPhastDoc *pWPhastDoc, vtkIdType id, double x, double y, double z, bool bSkipFirstExecute)
: Actor(pActor)
, WPhastDoc(pWPhastDoc)
, Id(id)
, X(x)
, Y(y)
, Z(z)
, bSkipFirstExecute(bSkipFirstExecute)
{
	ASSERT(this->Actor && this->Actor->IsA("CPointConnectorActor"));
}

template <typename TActor, typename TPoint>
CPointConnectorInsertPointAction<TActor, TPoint>::~CPointConnectorInsertPointAction(void)
{
	ASSERT(this->Actor && this->Actor->IsA("CPointConnectorActor"));
}

template <typename TActor, typename TPoint>
void CPointConnectorInsertPointAction<TActor, TPoint>::Execute(void)
{
	if (!this->bSkipFirstExecute)
	{
		TPoint point;
		point.x_user = this->X;   point.x_user_defined = TRUE;
		point.y_user = this->Y;   point.y_user_defined = TRUE;
		this->Actor->InsertPoint(this->Id, point);
	}
	this->bSkipFirstExecute = false;
	this->WPhastDoc->Select(this->Actor);
}

template <typename TActor, typename TPoint>
void CPointConnectorInsertPointAction<TActor, TPoint>::UnExecute(void)
{
	this->Actor->DeletePoint(this->Id + 1);
	this->WPhastDoc->Select(this->Actor);
}

