#pragma once
#include "Action.h"

class CWPhastDoc;

template<typename TActor>
class CPointConnectorDeleteAction : public CAction
{
public:
	CPointConnectorDeleteAction(CWPhastDoc* pDoc, TActor* pActor);
	~CPointConnectorDeleteAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc  *WPhastDoc;
	TActor      *Actor;
};

template<typename TActor>
CPointConnectorDeleteAction<TActor>::CPointConnectorDeleteAction(CWPhastDoc* pDoc, TActor* pActor)
: WPhastDoc(pDoc)
, Actor(pActor)
{
	ASSERT_VALID(this->WPhastDoc);
	ASSERT(this->Actor);
}

template<typename TActor>
CPointConnectorDeleteAction<TActor>::~CPointConnectorDeleteAction(void)
{
	ASSERT(this->Actor);
}

template<typename TActor>
void CPointConnectorDeleteAction<TActor>::Execute(void)
{
	ASSERT_VALID(this->WPhastDoc);
	this->WPhastDoc->Remove(this->Actor);
}

template<typename TActor>
void CPointConnectorDeleteAction<TActor>::UnExecute(void)
{
	ASSERT_VALID(this->WPhastDoc);
	this->WPhastDoc->UnRemove(this->Actor);
}
