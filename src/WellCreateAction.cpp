#include "StdAfx.h"
#include "WellCreateAction.h"

CWellCreateAction::CWellCreateAction(CWPhastDoc* pDoc, const CWellSchedule &well, CTreeCtrlNode nodeInsertAfter)
: m_pDoc(pDoc)
, m_well(well)
, m_hInsertAfter(0)
, m_dwInsertAfter(0)
{
	this->m_pWellActor = CWellActor::New();
	ASSERT(this->m_pWellActor->IsA("CWellActor"));

	this->m_pWellActor->SetWell(well, this->m_pDoc->GetUnits());

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

CWellCreateAction::~CWellCreateAction(void)
{
	ASSERT(this->m_pWellActor);
	if (this->m_pWellActor)
	{
		this->m_pWellActor->Delete();
		this->m_pWellActor = 0;
	}
}

void CWellCreateAction::Execute()
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
	this->m_pDoc->Add(this->m_pWellActor, hInsertAfter);
}

void CWellCreateAction::UnExecute()
{
	ASSERT_VALID(this->m_pDoc);
	this->m_pDoc->UnAdd(this->m_pWellActor);
}

