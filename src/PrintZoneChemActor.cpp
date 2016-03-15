#include "StdAfx.h"
#include "PrintZoneChemActor.h"

#include "PropertyTreeControlBar.h"
#include "SamplePg.h"

#include "WPhastDoc.h"
//#include "Protect.h"
#include "Global.h"
#include "GridKeyword.h"
//#include "WPhastMacros.h"
#include "ZoneCreateAction.h"
#include "SetThinGridAction.h"

vtkCxxRevisionMacro(CPrintZoneChemActor, "$Revision$");
vtkStandardNewMacro(CPrintZoneChemActor);

///const char CPrintZoneChemActor::szHeading[] = "PrintLocationsChem";
const char CPrintZoneChemActor::szHeading[] = "chemistry";
int CPrintZoneChemActor::thin_grid[3] = {0, 0, 0};


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

void CPrintZoneChemActor::EditThinGrid(CPropertyTreeControlBar* pTreeControlBar)
{
	CPropertySheet sheet(_T(""), NULL, 0, NULL, false);

	CSamplePg sample;
	sample.m_psp.dwFlags |= PSP_USETITLE;
	sample.m_psp.pszTitle = _T("CHEMISTRY");
	for (int i = 0; i < 3; ++i)
	{
		sample.thin_grid[i] = CPrintZoneChemActor::thin_grid[i];
	}
	sheet.AddPage(&sample);

	if (sheet.DoModal() == IDOK)
	{
		CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
		ASSERT_VALID(pFrame);
		CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
		ASSERT_VALID(pDoc);

		pDoc->Execute(new CSetThinGridAction(pTreeControlBar, pTreeControlBar->GetPLChemNode(), sample.thin_grid, CPrintZoneChemActor::thin_grid));
	}
}