#include "StdAfx.h"
#include "PrintZoneXYZChemActor.h"

#include "PropertyTreeControlBar.h"

#include "WPhastDoc.h"
//#include "Protect.h"
#include "Global.h"
#include "GridKeyword.h"
//#include "WPhastMacros.h"
#include "ZoneCreateAction.h"


vtkCxxRevisionMacro(CPrintZoneXYZChemActor, "$Revision$");
vtkStandardNewMacro(CPrintZoneXYZChemActor);

///const char CPrintZoneXYZChemActor::szHeading[] = "PrintLocationsXYZChem";
const char CPrintZoneXYZChemActor::szHeading[] = "xyz_chemistry";
int CPrintZoneXYZChemActor::thin_grid[3] = {1, 1, 1};

CPrintZoneXYZChemActor::CPrintZoneXYZChemActor(void)
{
}

CPrintZoneXYZChemActor::~CPrintZoneXYZChemActor(void)
{
}

void CPrintZoneXYZChemActor::Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
	HTREEITEM htiParent = pTreeControlBar->GetPLXYZChemNode();
	this->InsertAt(pTreeCtrl, htiParent, hInsertAfter);
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
}

void CPrintZoneXYZChemActor::Create(CWPhastDoc* pWPhastDoc, const CPrintZone& printZone)
{
	ASSERT(printZone.polyh && ::AfxIsValidAddress(printZone.polyh, sizeof(Polyhedron)));

	CGridKeyword gk;
	pWPhastDoc->GetGridKeyword(gk);
	CZoneCreateAction<CPrintZoneXYZChemActor>* pAction = new CZoneCreateAction<CPrintZoneXYZChemActor>(
		pWPhastDoc,
		printZone.polyh,
		gk.m_grid_origin,
		gk.m_grid_angle,
		""
		);
	pAction->GetZoneActor()->SetData(printZone);
	pWPhastDoc->Execute(pAction);
}
