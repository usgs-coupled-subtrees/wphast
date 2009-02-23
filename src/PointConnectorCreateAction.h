#pragma once
#include "Action.h"

class CWPhastDoc;

template<typename TActor, typename TData>
class CPointConnectorCreateAction : public CAction
{
public:
	CPointConnectorCreateAction(CWPhastDoc* pDoc, const TData &data, CTreeCtrlNode nodeInsertAfter = CTreeCtrlNode(TVI_LAST, NULL));
	~CPointConnectorCreateAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CWPhastDoc  *WPhastDoc;
	TActor      *Actor;
	TData       Data;

	HTREEITEM     hInsertAfter;
	DWORD_PTR     dwInsertAfter;
	CTreeCtrlNode nodeParent;
};

template<typename TActor, typename TData>
CPointConnectorCreateAction<TActor, TData>::CPointConnectorCreateAction(CWPhastDoc* pDoc, const TData &data, CTreeCtrlNode nodeInsertAfter)
: WPhastDoc(pDoc)
, Data(data)
, hInsertAfter(0)
, dwInsertAfter(0)
{
	ASSERT_VALID(this->WPhastDoc);

	this->Actor = TActor::New();

	this->Actor->SetData(this->Data, this->WPhastDoc->GetUnits(), this->WPhastDoc->GetGridKeyword());

	// tree placement
	if (nodeInsertAfter == TVI_LAST)
	{
		this->hInsertAfter = TVI_LAST;
	}
	else if (nodeInsertAfter == TVI_FIRST)
	{
		this->hInsertAfter = TVI_FIRST;
	}
	else
	{
		this->nodeParent = nodeInsertAfter.GetParent();
		this->dwInsertAfter = nodeInsertAfter.GetData();
		ASSERT(this->nodeParent && this->dwInsertAfter);
	}
}

template<typename TActor, typename TData>
CPointConnectorCreateAction<TActor, TData>::~CPointConnectorCreateAction(void)
{
	ASSERT(this->Actor);
	if (this->Actor)
	{
		this->Actor->Delete();
		this->Actor = 0;
	}
}

template<typename TActor, typename TData>
void CPointConnectorCreateAction<TActor, TData>::Execute(void)
{
	ASSERT_VALID(this->WPhastDoc);
	HTREEITEM hInsert = TVI_LAST;
	if (this->nodeParent)
	{
		// search for hInsertAfter (by data)
		CTreeCtrlNode node = this->nodeParent.GetChild();
		while (node)
		{
			if (node.GetData() == this->dwInsertAfter)
			{
				hInsert = node;
				break;
			}
			node = node.GetNextSibling();
		}
		ASSERT(node);
	}
	else
	{
		hInsert = this->hInsertAfter;
	}
	this->WPhastDoc->Add(this->Actor, hInsert);
}

template<typename TActor, typename TData>
void CPointConnectorCreateAction<TActor, TData>::UnExecute(void)
{
	ASSERT_VALID(this->WPhastDoc);
	HTREEITEM hInsert = TVI_LAST;
	if (this->nodeParent)
	{
		// search for hInsertAfter (by data)
		CTreeCtrlNode node = this->nodeParent.GetChild();
		while (node)
		{
			if (node.GetData() == this->dwInsertAfter)
			{
				hInsert = node;
				break;
			}
			node = node.GetNextSibling();
		}
		ASSERT(node);
	}
	else
	{
		hInsert = this->hInsertAfter;
	}
	this->WPhastDoc->UnAdd(this->Actor);
}
