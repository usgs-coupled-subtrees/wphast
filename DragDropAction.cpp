#include "StdAfx.h"
#include "DragDropAction.h"

#include "PropertyTreeControlBar.h"
#include "ZoneActor.h"


CDragDropAction::CDragDropAction(CPropertyTreeControlBar* pBar, CTreeCtrlNode dragNode, CTreeCtrlNode dropNode)
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

	this->ZoneActor = CZoneActor::SafeDownCast((vtkObject*)dragNode.GetData());
	ASSERT(this->ZoneActor);
}

CDragDropAction::~CDragDropAction(void)
{
}

void CDragDropAction::Execute(void)
{
	if (this->ZoneActor && this->NodeParent)
	{
		// search for drag node (by data)
		CTreeCtrlNode nodeDrag = this->NodeParent.GetChild();
		while (nodeDrag)
		{
			if ((vtkObject*)nodeDrag.GetData() == this->ZoneActor)
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
			this->ZoneActor->Insert(this->Bar, nodeDrop);
			VERIFY(nodeDrag.Delete());
		}
	}
}

void CDragDropAction::UnExecute(void)
{
	if (this->ZoneActor && this->NodeParent)
	{
		// search for drag node (by data)
		CTreeCtrlNode nodeDrag = this->NodeParent.GetChild();
		while (nodeDrag)
		{
			if ((vtkObject*)nodeDrag.GetData() == this->ZoneActor)
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
			this->ZoneActor->Insert(this->Bar, nodePrevCut);
			VERIFY(nodeDrag.Delete());
		}
	}
}
