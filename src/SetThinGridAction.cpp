#include "StdAfx.h"
#include "SetThinGridAction.h"

#include <vector>
#include <sstream>

CSetThinGridAction::CSetThinGridAction(CPropertyTreeControlBar *pBar, CTreeCtrlNode node, int thinGrid[3], int (&ptrThinGrid)[3])
: PropertyTreeControlBar(pBar)
, PrintLocNode(node)
, PtrThinGrid(ptrThinGrid)
{
	for (int i = 0; i < 3; ++i)
	{
		this->NewThinGrid[i] = thinGrid[i];
		this->OldThinGrid[i] = this->PtrThinGrid[i];
	}
}

CSetThinGridAction::~CSetThinGridAction(void)
{
}

void CSetThinGridAction::Execute(void)
{
	this->SetThinGrid(this->NewThinGrid);
}

void CSetThinGridAction::UnExecute(void)
{
	this->SetThinGrid(this->OldThinGrid);
}

void CSetThinGridAction::SetThinGrid(int thin_grid[])
{
	for (int i = 0; i < 3; ++i)
	{
		this->PtrThinGrid[i] = thin_grid[i];
	}

	// collect non-actor nodes (sample nodes)
	std::vector<CTreeCtrlNode> remove;
	int count = this->PrintLocNode.GetChildCount();
	for (int j = 0; j < count; ++j)
	{
		CTreeCtrlNode child = this->PrintLocNode.GetChildAt(j);
		if (child.GetData() == 0)
		{
			remove.push_back(child);
		}
	}
	// remove non-actor nodes (sample nodes)
	for(std::vector<CTreeCtrlNode>::size_type j = 0; j < remove.size(); ++j)
	{
		remove[j].Delete();
	}

	// add back sampled nodes
	char coor[] = {'X', 'Y', 'Z'};
	for(int j = 2; j >= 0; --j)
	{
		if (this->PtrThinGrid[j] > 0)
		{
			std::ostringstream oss;
			oss << "sample " << coor[j] << " " << this->PtrThinGrid[j];
			this->PrintLocNode.AddHead(oss.str().c_str());
		}
	}
}

