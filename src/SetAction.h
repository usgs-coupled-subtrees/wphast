#pragma once
#include "Action.h"

//#include "Tree.h"

class CWPhastDoc;

template<typename TActor, typename TData>
class CSetAction : public CAction
{
public:
	CSetAction(TActor *pActor, const TData& newData, CWPhastDoc *pWPhastDoc);
	virtual ~CSetAction(void);

	virtual void Execute(void);
	virtual void UnExecute(void);

protected:
	void SetData(const TData &data);

protected:
	TActor      *Actor;
	TData        OldData;
	TData        NewData;
	CWPhastDoc  *WPhastDoc;
	double       Z;
};


template<typename TActor, typename TData>
CSetAction<TActor, TData>::CSetAction(TActor *pActor, const TData& newData, CWPhastDoc *pWPhastDoc)
: Actor(pActor)
, OldData(pActor->GetData())
, NewData(newData)
, WPhastDoc(pWPhastDoc)
, Z(pActor->GetZ())
{
	ASSERT(pActor);
	ASSERT_VALID(pWPhastDoc);
}

template<typename TActor, typename TData>
CSetAction<TActor, TData>::~CSetAction(void)
{
}

template<typename TActor, typename TData>
void CSetAction<TActor, TData>::Execute(void)
{
	this->SetData(this->NewData);
}

template<typename TActor, typename TData>
void CSetAction<TActor, TData>::UnExecute(void)
{
	this->SetData(this->OldData);
}

template<typename TActor, typename TData>
void CSetAction<TActor, TData>::SetData(const TData &data)
{
	this->Actor->SetData(data, this->WPhastDoc->GetUnits(), this->WPhastDoc->GetGridKeyword());
	this->Actor->SetZ(this->Z);
	CTreeCtrlNode top = this->Actor->GetTreeCtrlNode().GetFirstVisible();
	this->Actor->Update(this->Actor->GetTreeCtrlNode());
	top.SelectSetFirstVisible();
	this->WPhastDoc->Select(this->Actor);
}
