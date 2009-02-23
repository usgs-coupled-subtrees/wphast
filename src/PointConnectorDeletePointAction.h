#pragma once
#include "Action.h"

//#include "River.h"

//class CRiverActor;
class CWPhastDoc;

template <typename TActor, typename TPoint>
class CPointConnectorDeletePointAction : public CAction
{
public:
	CPointConnectorDeletePointAction(TActor *pActor, CWPhastDoc *pWPhastDoc, vtkIdType id);
	~CPointConnectorDeletePointAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	TActor      *Actor;
	CWPhastDoc  *WPhastDoc;
	vtkIdType    Id;
	TPoint       Point;
};



template <typename TActor, typename TPoint>
CPointConnectorDeletePointAction<TActor, TPoint>::CPointConnectorDeletePointAction(TActor *pActor, CWPhastDoc *pWPhastDoc, vtkIdType id)
: Actor(pActor)
, WPhastDoc(pWPhastDoc)
, Id(id)
{
	ASSERT(this->Actor);
	ASSERT(this->Actor->GetRiverPoint(this->Id));
	if (this->Actor && this->Actor->GetRiverPoint(this->Id))
	{
		TPoint pt(*this->Actor->GetRiverPoint(this->Id));
		ASSERT(pt.x_user_defined && pt.y_user_defined);
		this->Point = pt;
	}
}

template <typename TActor, typename TPoint>
CPointConnectorDeletePointAction<TActor, TPoint>::~CPointConnectorDeletePointAction(void)
{
	ASSERT(this->Actor);
}

template <typename TActor, typename TPoint>
void CPointConnectorDeletePointAction<TActor, TPoint>::Execute()
{
	this->Actor->DeletePoint(this->Id);
	this->WPhastDoc->Select(this->Actor);
}

template <typename TActor, typename TPoint>
void CPointConnectorDeletePointAction<TActor, TPoint>::UnExecute()
{
	this->Actor->InsertPoint(this->Id - 1, this->Point);
	this->WPhastDoc->Select(this->Actor);
}

