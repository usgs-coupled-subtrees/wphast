#include "StdAfx.h"
#include "GridCoarsenAction.h"

#include "WPhastDoc.h"
#include "GridActor.h"

CGridSparsifyAction::CGridSparsifyAction(CWPhastDoc *document, CGridActor *actor, int min[3], int max[3], int parts[3], const CGridKeyword &gridKeyword, bool skipFirstExecute)
: Document(document)
, Actor(actor)
, GridKeyword(gridKeyword)
, SkipFirstExecute(skipFirstExecute)
{
	for (int i = 0; i < 3; ++i)
	{
		this->Min[i]   = min[i];
		this->Max[i]   = max[i];
		this->Parts[i] = parts[i];
	}
}

CGridSparsifyAction::~CGridSparsifyAction(void)
{
}

void CGridSparsifyAction::Execute()
{
	if (!this->SkipFirstExecute)
	{
		CGridKeyword gridKeyword;
		this->Actor->GetGridKeyword(gridKeyword);
		for (int i = 0; i < 3; ++i)
		{
			gridKeyword.m_grid[i].Merge(this->Min[i], this->Max[i], this->Parts[i]);
		}
		this->Actor->SetGridKeyword(gridKeyword, this->Document->GetUnits());
		this->Actor->UpdateNode();
		this->Document->UpdateAllViews(0);
	}
	this->SkipFirstExecute = false;
}

void CGridSparsifyAction::UnExecute()
{
	this->Actor->SetGridKeyword(this->GridKeyword, this->Document->GetUnits());
	this->Actor->UpdateNode();
	this->Document->UpdateAllViews(0);
}

void CGridSparsifyAction::Apply(void)
{
	this->SkipFirstExecute = false;
	this->Execute();
	this->SkipFirstExecute = true;
}
