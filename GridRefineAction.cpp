#include "StdAfx.h"
#include "GridRefineAction.h"

#include "WPhastDoc.h"
#include "GridActor.h"

CGridRefineAction::CGridRefineAction(CWPhastDoc *document, CGridActor *actor, int min[3], int max[3], int parts[3], bool skipFirstExecute)
: Document(document)
, Actor(actor)
, SkipFirstExecute(skipFirstExecute)
{
	for (int i = 0; i < 3; ++i)
	{
		this->Min[i]   = min[i];
		this->Max[i]   = max[i];
		this->Parts[i] = parts[i];
	}
}

CGridRefineAction::~CGridRefineAction(void)
{
}

void CGridRefineAction::Execute()
{
	if (!this->SkipFirstExecute)
	{
		CGridKeyword gridKeyword;
		this->Actor->GetGridKeyword(gridKeyword);

		CUnits units;
		this->Document->GetUnits(units);

		for (int i = 0; i < 3; ++i)
		{
			this->WasUniform[i] = (gridKeyword.m_grid[i].uniform != 0);
			gridKeyword.m_grid[i].Refine(this->Min[i], this->Max[i], this->Parts[i]);
		}
		this->Actor->SetGridKeyword(gridKeyword, units);
		this->Actor->UpdateNode();
		this->Document->UpdateAllViews(0);
	}
	this->SkipFirstExecute = false;
}

void CGridRefineAction::UnExecute()
{
	CGridKeyword gridKeyword;
	this->Actor->GetGridKeyword(gridKeyword);

	for (int i = 0; i < 3; ++i)
	{
		int newMax = (this->Max[i] - this->Min[i]) * this->Parts[i] + this->Min[i];
		gridKeyword.m_grid[i].Coarsen(this->Min[i], newMax, this->Parts[i]);
		if (this->WasUniform[i] && this->Parts[i] > 1)
		{
			gridKeyword.m_grid[i].uniform          = TRUE;
			gridKeyword.m_grid[i].uniform_expanded = TRUE;
		}
	}
	this->Actor->SetGridKeyword(gridKeyword, this->Document->GetUnits());
	this->Actor->UpdateNode();
	this->Document->UpdateAllViews(0);
}

void CGridRefineAction::Apply(void)
{
	this->SkipFirstExecute = false;
	this->Execute();
	this->SkipFirstExecute = true;
}
