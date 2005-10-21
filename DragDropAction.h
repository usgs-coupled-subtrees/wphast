#pragma once
#include "Action.h"

#include "Tree.h"

class CPropertyTreeControlBar;
class CZoneActor;

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
	CZoneActor* ZoneActor;

	DWORD_PTR dwPrevCutData;
	DWORD_PTR dwDropData;

	bool DropNodeIsFirst;
	bool DragNodeIsFirst;
};
