#pragma once

#include "PrintZoneActor.h"

class CPrintZoneXYZChemActor : public CPrintZoneActor
{
public:
	vtkTypeRevisionMacro(CPrintZoneXYZChemActor,CPrintZoneActor);
	static CPrintZoneXYZChemActor *New();
	static void Create(CWPhastDoc* pWPhastDoc, const CPrintZone& printZone);

	virtual void Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter);

	static const char szHeading[];
	static int thin_grid[3];

protected:
	CPrintZoneXYZChemActor(void);
	virtual ~CPrintZoneXYZChemActor(void);
};
