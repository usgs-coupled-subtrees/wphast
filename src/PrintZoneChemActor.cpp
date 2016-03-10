#include "StdAfx.h"
#include "PrintZoneChemActor.h"

#include "PropertyTreeControlBar.h"

#include "WPhastDoc.h"
//#include "Protect.h"
#include "Global.h"
#include "GridKeyword.h"
//#include "WPhastMacros.h"
#include "ZoneCreateAction.h"

vtkCxxRevisionMacro(CPrintZoneChemActor, "$Revision$");
vtkStandardNewMacro(CPrintZoneChemActor);

///const char CPrintZoneChemActor::szHeading[] = "PrintLocationsChem";
const char CPrintZoneChemActor::szHeading[] = "chemistry";
int CPrintZoneChemActor::thin_grid[3] = {1, 1, 1};


CPrintZoneChemActor::CPrintZoneChemActor(void)
{
}

CPrintZoneChemActor::~CPrintZoneChemActor(void)
{
}

void CPrintZoneChemActor::Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
	HTREEITEM htiParent = pTreeControlBar->GetPLChemNode();
	this->InsertAt(pTreeCtrl, htiParent, hInsertAfter);
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
}

void CPrintZoneChemActor::Create(CWPhastDoc* pWPhastDoc, const CPrintZone& printZone)
{
	ASSERT(printZone.polyh && ::AfxIsValidAddress(printZone.polyh, sizeof(Polyhedron)));

	CGridKeyword gk;
	pWPhastDoc->GetGridKeyword(gk);
	CZoneCreateAction<CPrintZoneChemActor>* pAction = new CZoneCreateAction<CPrintZoneChemActor>(
		pWPhastDoc,
		printZone.polyh,
		gk.m_grid_origin,
		gk.m_grid_angle,
		""
		);
	pAction->GetZoneActor()->SetData(printZone);
	pWPhastDoc->Execute(pAction);
}
