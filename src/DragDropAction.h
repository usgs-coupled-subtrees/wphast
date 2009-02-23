#pragma once
#include "Action.h"

#include "Tree.h"

class CPropertyTreeControlBar;
//class CZoneActor;

template<typename T>
class CDragDropAction :	public CAction
{
public:
	CDragDropAction(CPropertyTreeControlBar* pBar, CTreeCtrlNode dragNode, CTreeCtrlNode dropNode);
	~CDragDropAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	CPropertyTreeControlBar* Bar;
	CTreeCtrlNode NodeParent;
	T* Actor;

	DWORD_PTR dwPrevCutData;
	DWORD_PTR dwDropData;

	bool DropNodeIsFirst;
	bool DragNodeIsFirst;
};

// Placing the code within the header file eliminates the
// need for explicit instantiation of the template
//
// ie
//   template class CDragDropAction<CZoneActor>;
//   template class CDragDropAction<CWellActor>;
//   template class CDragDropAction<CRiverActor>;
//
// is no longer needed (DragDropAction.cpp is no longer needed)
// 

template<typename T>
CDragDropAction<T>::CDragDropAction(CPropertyTreeControlBar* pBar, CTreeCtrlNode dragNode, CTreeCtrlNode dropNode)
: Bar(pBar)
, NodeParent(dragNode.GetParent())
, DropNodeIsFirst(dropNode == TVI_FIRST)
, DragNodeIsFirst(false)
, dwPrevCutData(0)
{
	ASSERT(pBar);
	ASSERT(dragNode != dropNode);
	ASSERT(dropNode == TVI_FIRST || dragNode.GetParent() == dropNode.GetParent());
	ASSERT(dragNode.GetData());

	CTreeCtrlNode prev = dragNode.GetPrevSibling();
	if (prev)
	{
		this->dwPrevCutData = prev.GetData();
		ASSERT(this->dwPrevCutData);
	}
	else
	{
		this->DragNodeIsFirst = true;
	}

	if (!this->DropNodeIsFirst)
	{
		this->dwDropData = dropNode.GetData();
		ASSERT(this->dwDropData);
	}

	this->Actor = T::SafeDownCast((vtkObject*)dragNode.GetData());
	ASSERT(this->Actor);
}

template<typename T>
CDragDropAction<T>::~CDragDropAction(void)
{
}

template<typename T>
void CDragDropAction<T>::Execute(void)
{
	if (this->Actor && this->NodeParent)
	{
		// search for drag node (by data)
		CTreeCtrlNode nodeDrag = this->NodeParent.GetChild();
		while (nodeDrag)
		{
			if ((vtkObject*)nodeDrag.GetData() == this->Actor)
			{
				break;
			}
			nodeDrag = nodeDrag.GetNextSibling();
		}
		ASSERT(nodeDrag);


		CTreeCtrlNode nodeDrop(TVI_FIRST, this->Bar->GetTreeCtrlEx());
		if (!this->DropNodeIsFirst)
		{
			// search for prev drop node (by data)
			//
			nodeDrop = this->NodeParent.GetChild();
			while (nodeDrop)
			{
				if (nodeDrop.GetData() == this->dwDropData)
				{
					break;
				}
				nodeDrop = nodeDrop.GetNextSibling();
			}
			ASSERT(nodeDrop);
		}

		if (nodeDrag && nodeDrop)
		{
			this->Actor->Insert(this->Bar, nodeDrop);
			VERIFY(nodeDrag.Delete());
		}
	}
}

template<typename T>
void CDragDropAction<T>::UnExecute(void)
{
	if (this->Actor && this->NodeParent)
	{
		// search for drag node (by data)
		CTreeCtrlNode nodeDrag = this->NodeParent.GetChild();
		while (nodeDrag)
		{
			if ((vtkObject*)nodeDrag.GetData() == this->Actor)
			{
				break;
			}
			nodeDrag = nodeDrag.GetNextSibling();
		}
		ASSERT(nodeDrag);


		CTreeCtrlNode nodePrevCut(TVI_FIRST, this->Bar->GetTreeCtrlEx());
		if (!this->DragNodeIsFirst)
		{
			// search for prev sibling node (by data)
			//
			nodePrevCut = this->NodeParent.GetChild();
			while (nodePrevCut)
			{
				if (nodePrevCut.GetData() == this->dwPrevCutData)
				{
					break;
				}
				nodePrevCut = nodePrevCut.GetNextSibling();
			}
			ASSERT(nodePrevCut);
		}

		if (nodeDrag && nodePrevCut)
		{
			this->Actor->Insert(this->Bar, nodePrevCut);
			VERIFY(nodeDrag.Delete());
		}
	}
}
