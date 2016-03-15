#include "StdAfx.h"
#include "PrintZoneXYZChemActor.h"

#include "PropertyTreeControlBar.h"
#include "SamplePg.h"

#include "WPhastDoc.h"
//#include "Protect.h"
#include "Global.h"
#include "GridKeyword.h"
//#include "WPhastMacros.h"
#include "ZoneCreateAction.h"
#include "SetThinGridAction.h"


vtkCxxRevisionMacro(CPrintZoneXYZChemActor, "$Revision$");
vtkStandardNewMacro(CPrintZoneXYZChemActor);

///const char CPrintZoneXYZChemActor::szHeading[] = "PrintLocationsXYZChem";
const char CPrintZoneXYZChemActor::szHeading[] = "xyz_chemistry";
int CPrintZoneXYZChemActor::thin_grid[3] = {0, 0, 0};

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

void CPrintZoneXYZChemActor::EditThinGrid(CPropertyTreeControlBar* pTreeControlBar)
{
	CPropertySheet sheet(_T(""), NULL, 0, NULL, false);

	CSamplePg sample;
	sample.m_psp.dwFlags |= PSP_USETITLE;
	sample.m_psp.pszTitle = _T("XYZ_CHEMISTRY");
	for (int i = 0; i < 3; ++i)
	{
		sample.thin_grid[i] = CPrintZoneXYZChemActor::thin_grid[i];
	}
	sheet.AddPage(&sample);

	if (sheet.DoModal() == IDOK)
	{
		CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
		ASSERT_VALID(pFrame);
		CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
		ASSERT_VALID(pDoc);

		pDoc->Execute(new CSetThinGridAction(pTreeControlBar, pTreeControlBar->GetPLXYZChemNode(), sample.thin_grid, CPrintZoneXYZChemActor::thin_grid));
	}
}