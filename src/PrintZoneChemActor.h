#pragma once

#include "PrintZoneActor.h"

class CPrintZoneChemActor : public CPrintZoneActor
{
public:
	vtkTypeRevisionMacro(CPrintZoneChemActor,CPrintZoneActor);
	static CPrintZoneChemActor *New();
	static void Create(CWPhastDoc* pWPhastDoc, const CPrintZone& printZone);

	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter);

	static void EditThinGrid(CPropertyTreeControlBar* pTreeControlBar);

	static const char szHeading[];
	static int thin_grid[3];

protected:
	CPrintZoneChemActor(void);
	virtual ~CPrintZoneChemActor(void);
};
