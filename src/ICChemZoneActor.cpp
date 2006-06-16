#include "StdAfx.h"
#include "ICChemZoneActor.h"

#include "resource.h"
#include "PropertyTreeControlBar.h"
#include "ChemICSpreadPropertyPage.h"

#include "property.h"
#include "Global.h"

#include "WPhastDoc.h"
#include "SetChemICAction.h"
#include "ZoneCreateAction.h"
#include "FlowOnly.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
#include <vtkPropAssembly.h>

vtkCxxRevisionMacro(CICChemZoneActor, "$Revision$");
vtkStandardNewMacro(CICChemZoneActor);

const char CICChemZoneActor::szHeading[] = "ICChem";
float CICChemZoneActor::s_color[3];

CICChemZoneActor::CICChemZoneActor(void)
{
}

CICChemZoneActor::~CICChemZoneActor(void)
{
}

void CICChemZoneActor::Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CChemIC& chemIC, LPCTSTR desc)
{
	CZoneCreateAction<CICChemZoneActor>* pAction = new CZoneCreateAction<CICChemZoneActor>(
		pWPhastDoc,
		pWPhastDoc->GetNextZoneName(),
		zone.x1,
		zone.x2,
		zone.y1,
		zone.y2,
		zone.z1,
		zone.z2,
		desc
		);
	pAction->GetZoneActor()->SetData(chemIC);
	pWPhastDoc->Execute(pAction);
}

CChemIC CICChemZoneActor::GetData(void)const
{
	CChemIC chemIC(this->m_chemIC);

	ASSERT(this->m_pZone != 0);
	ASSERT(this->m_chemIC.zone == 0);
	chemIC.zone = new CZone(*this->m_pZone);

	return chemIC;
}

void CICChemZoneActor::SetData(const CChemIC& chemIC)
{
	this->m_chemIC = chemIC;
	if (this->m_chemIC.zone != 0)
	{
		delete this->m_chemIC.zone;
		this->m_chemIC.zone = 0;
	}
}

void CICChemZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();

	HTREEITEM htiIC = pTreeControlBar->GetICChemNode();;

	this->InsertAt(pTreeCtrl, htiIC, hInsertAfter);
	//{{HACK
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
	//}}HACK
}

void CICChemZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiParent))
	{
		pTreeCtrl->DeleteItem(hChild);
	}

	pTreeCtrl->SetItemText(htiParent, this->GetNameDesc());

	// solution
	if (this->m_chemIC.solution)
	{
		static_cast<Cproperty*>(this->m_chemIC.solution)->Insert(pTreeCtrl, htiParent, "solution");
	}

	// equilibrium_phases
	if (this->m_chemIC.equilibrium_phases)
	{
		static_cast<Cproperty*>(this->m_chemIC.equilibrium_phases)->Insert(pTreeCtrl, htiParent, "equilibrium_phases");
	}

	// exchange
	if (this->m_chemIC.exchange)
	{
		static_cast<Cproperty*>(this->m_chemIC.exchange)->Insert(pTreeCtrl, htiParent, "exchange");
	}

	// surface
	if (this->m_chemIC.surface)
	{
		static_cast<Cproperty*>(this->m_chemIC.surface)->Insert(pTreeCtrl, htiParent, "surface");
	}

	// gas_phase
	if (this->m_chemIC.gas_phase)
	{
		static_cast<Cproperty*>(this->m_chemIC.gas_phase)->Insert(pTreeCtrl, htiParent, "gas_phase");
	}

	// solid_solutions
	if (this->m_chemIC.solid_solutions)
	{
		static_cast<Cproperty*>(this->m_chemIC.solid_solutions)->Insert(pTreeCtrl, htiParent, "solid_solutions");
	}

	// kinetics
	if (this->m_chemIC.kinetics)
	{
		static_cast<Cproperty*>(this->m_chemIC.kinetics)->Insert(pTreeCtrl, htiParent, "kinetics");
	}
}

void CICChemZoneActor::Serialize(bool bStoring, hid_t loc_id, const CUnits& units)
{
	CZoneActor::Serialize(bStoring, loc_id);

	if (bStoring)
	{
		// store chem_ic
		ASSERT(this->m_chemIC.zone == 0);
		ASSERT(this->m_pZone != 0);
		this->m_chemIC.zone = this->m_pZone;
		this->m_chemIC.Serialize(bStoring, loc_id);
		this->m_chemIC.zone = 0;
	}
	else
	{
		// load chem_ic
		ASSERT(this->m_chemIC.zone == 0);
		ASSERT(this->m_pZone != 0);
		this->m_chemIC.zone = this->m_pZone;
		this->m_chemIC.Serialize(bStoring, loc_id);
		this->m_pZone = this->m_chemIC.zone;
		this->m_chemIC.zone = 0;

		this->SetUnits(units);
	}
}

void CICChemZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	CString str;
	str.Format(_T("%s Properties"), this->GetName());
	CPropertySheet props(str);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	CChemICSpreadPropertyPage chemICProps;
	chemICProps.SetProperties(this->GetData());
	chemICProps.SetDesc(this->GetDesc());
	if (this->GetDefault())
	{
		chemICProps.SetFlowOnly(bool(pDoc->GetFlowOnly()));
	}

	props.AddPage(&chemICProps);
	if (props.DoModal() == IDOK)
	{
		CChemIC chemIC;
		chemICProps.GetProperties(chemIC);
		pDoc->Execute(new CSetChemICAction(this, pTreeCtrl, chemIC, chemICProps.GetDesc()));
	}
}

void CICChemZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyIC())
	{
		pPropAssembly->AddPart(this);
		if (!pWPhastDoc->GetPropCollection()->IsItemPresent(pPropAssembly))
		{
			pWPhastDoc->GetPropCollection()->AddItem(pPropAssembly);
		}
	}
#ifdef _DEBUG
	else ASSERT(FALSE);
#endif
}

void CICChemZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyIC())
	{
		pPropAssembly->RemovePart(this);
		// VTK HACK
		// This is req'd because ReleaseGraphicsResources is not called when
		// vtkPropAssembly::RemovePart(vtkProp *prop) is called
		pWPhastDoc->ReleaseGraphicsResources(this);
	}
#ifdef _DEBUG
	else ASSERT(FALSE);
#endif
}

void CICChemZoneActor::SetStaticColor(COLORREF cr)
{
	CICChemZoneActor::s_color[0] = (double)GetRValue(cr)/255.;
	CICChemZoneActor::s_color[1] = (double)GetGValue(cr)/255.;
	CICChemZoneActor::s_color[2] = (double)GetBValue(cr)/255.;	
}

void CICChemZoneActor::Modified() // virtual
{
	this->GetProperty()->SetColor(CICChemZoneActor::s_color);
	this->Superclass::Modified();
}
