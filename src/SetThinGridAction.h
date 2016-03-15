#pragma once

#include "Action.h"

#include "Tree.h"

class CPropertyTreeControlBar;

class CSetThinGridAction : public CAction
{
public:
	CSetThinGridAction(CPropertyTreeControlBar *pBar, CTreeCtrlNode node, int thinGrid[3], int (&ptrThinGrid)[3]);
	virtual ~CSetThinGridAction(void);

	virtual void Execute();
	virtual void UnExecute();

protected:
	void SetThinGrid(int thin_grid[]);

protected:
	CPropertyTreeControlBar *PropertyTreeControlBar;
	CTreeCtrlNode PrintLocNode;
	int OldThinGrid[3];
	int NewThinGrid[3];
	int (&PtrThinGrid)[3];
};
