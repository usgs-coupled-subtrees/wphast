#include "StdAfx.h"
#include "RiverCreateAction.h"

CRiverCreateAction::CRiverCreateAction(CWPhastDoc* pDoc, const CRiver &river, CTreeCtrlNode nodeInsertAfter)
: m_pDoc(pDoc)
, m_river(river)
, m_hInsertAfter(0)
, m_dwInsertAfter(0)
{
	ASSERT_VALID(this->m_pDoc);

	this->m_pRiverActor = CRiverActor::New();
	ASSERT(this->m_pRiverActor->IsA("CRiverActor"));

	this->m_pRiverActor->SetRiver(river, this->m_pDoc->GetUnits());

	// tree placement
	if (nodeInsertAfter == TVI_LAST)
	{
		this->m_hInsertAfter = TVI_LAST;
	}
	else if (nodeInsertAfter == TVI_FIRST)
	{
		this->m_hInsertAfter = TVI_FIRST;
	}
	else
	{
		this->m_nodeParent = nodeInsertAfter.GetParent();
		this->m_dwInsertAfter = nodeInsertAfter.GetData();
		ASSERT(this->m_nodeParent && this->m_dwInsertAfter);
	}
}

CRiverCreateAction::CRiverCreateAction(CWPhastDoc* pDoc, CRiverActor *pRiverActor, CTreeCtrlNode nodeInsertAfter)
: m_pDoc(pDoc)
, m_pRiverActor(pRiverActor)
, m_river(pRiverActor->GetRiver())
, m_hInsertAfter(0)
, m_dwInsertAfter(0)
{
	ASSERT_VALID(this->m_pDoc);

	ASSERT(this->m_pRiverActor);
	ASSERT(this->m_pRiverActor->IsA("CRiverActor"));

	// tree placement
	if (nodeInsertAfter == TVI_LAST)
	{
		this->m_hInsertAfter = TVI_LAST;
	}
	else if (nodeInsertAfter == TVI_FIRST)
	{
		this->m_hInsertAfter = TVI_FIRST;
	}
	else
	{
		this->m_nodeParent = nodeInsertAfter.GetParent();
		this->m_dwInsertAfter = nodeInsertAfter.GetData();
		ASSERT(this->m_nodeParent && this->m_dwInsertAfter);
	}
}

CRiverCreateAction::~CRiverCreateAction(void)
{
	ASSERT(this->m_pRiverActor);
	if (this->m_pRiverActor)
	{
		this->m_pRiverActor->Delete();
		this->m_pRiverActor = 0;
	}
}

void CRiverCreateAction::Execute()
{
	ASSERT_VALID(this->m_pDoc);
	HTREEITEM hInsertAfter = TVI_LAST;
	if (this->m_nodeParent)
	{
		// search for hInsertAfter (by data)
		CTreeCtrlNode node = this->m_nodeParent.GetChild();
		while (node)
		{
			if (node.GetData() == this->m_dwInsertAfter)
			{
				hInsertAfter = node;
				break;
			}
			node = node.GetNextSibling();
		}
		ASSERT(node);
	}
	else
	{
		hInsertAfter = this->m_hInsertAfter;
	}
	this->m_pDoc->Add(this->m_pRiverActor, hInsertAfter);
}

void CRiverCreateAction::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	HTREEITEM hInsertAfter = TVI_LAST;
	if (this->m_nodeParent)
	{
		// search for hInsertAfter (by data)
		CTreeCtrlNode node = this->m_nodeParent.GetChild();
		while (node)
		{
			if (node.GetData() == this->m_dwInsertAfter)
			{
				hInsertAfter = node;
				break;
			}
			node = node.GetNextSibling();
		}
		ASSERT(node);
	}
	else
	{
		hInsertAfter = this->m_hInsertAfter;
	}
	this->m_pDoc->UnAdd(this->m_pRiverActor);
}
