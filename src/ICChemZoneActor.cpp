#include "StdAfx.h"
#include "ICChemZoneActor.h"

#include "resource.h"
#include "PropertyTreeControlBar.h"
#include "ChemICSpreadPropertyPage.h"
#include "ICChemPropsPage.h"
#include "ICChemPropsPage2.h"

#include "property.h"
#include "Global.h"

#include "WPhastDoc.h"
#include "SetChemICAction.h"
#include "ZoneCreateAction.h"
#include "FlowOnly.h"
#include "Protect.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
#include <vtkPropAssembly.h>

vtkCxxRevisionMacro(CICChemZoneActor, "$Revision$");
vtkStandardNewMacro(CICChemZoneActor);

const char CICChemZoneActor::szHeading[] = "ICChem";
double CICChemZoneActor::s_color[3] = {0., 0., 0.};
vtkProperty* CICChemZoneActor::s_Property = 0;
vtkProperty* CICChemZoneActor::s_OutlineProperty = 0;

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CICChemZoneActor::CICChemZoneActor(void)
{
	static StaticInit init; // constructs/destructs s_Property/s_OutlineProperty

	// cube
	this->CubeActor->SetProperty(CICChemZoneActor::s_Property);

	// outline
	this->OutlineActor->SetProperty(CICChemZoneActor::s_OutlineProperty);
}

CICChemZoneActor::~CICChemZoneActor(void)
{
}

void CICChemZoneActor::Create(CWPhastDoc* pWPhastDoc, const CChemIC& chemIC, LPCTSTR desc)
{
	ASSERT(chemIC.polyh && ::AfxIsValidAddress(chemIC.polyh, sizeof(Polyhedron)));
	CZoneCreateAction<CICChemZoneActor>* pAction = new CZoneCreateAction<CICChemZoneActor>(
		pWPhastDoc,
		chemIC.polyh,
		desc
		);
	pAction->GetZoneActor()->SetData(chemIC);
	pWPhastDoc->Execute(pAction);
}

CChemIC CICChemZoneActor::GetData(void)const
{
	return this->m_chemIC;
}

void CICChemZoneActor::SetData(const CChemIC& chemIC)
{
	CProtect p(this->m_chemIC.polyh);
	this->m_chemIC = chemIC;
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
	CZoneActor::Update(pTreeCtrl, htiParent);

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

void CICChemZoneActor::Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc)
{
	CZoneActor::Serialize(bStoring, loc_id, pWPhastDoc);

	if (bStoring)
	{
		// store chem_ic
		this->m_chemIC.Serialize(bStoring, loc_id);
	}
	else
	{
		// load chem_ic
		this->m_chemIC.Serialize(bStoring, loc_id);
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

	ASSERT(this->GetData().polyh);

	//CChemICSpreadPropertyPage chemICProps;
	//CICChemPropsPage chemICProps;
	CICChemPropsPage2 chemICProps;
	chemICProps.SetProperties(this->GetData());
	chemICProps.SetDesc(this->GetDesc());
	if (this->GetDefault())
	{
		chemICProps.SetDefault(true);		
		chemICProps.SetFlowOnly(bool(pDoc->GetFlowOnly()));
	}

	props.AddPage(&chemICProps);
	if (props.DoModal() == IDOK)
	{
		CChemIC chemIC;
		chemICProps.GetProperties(chemIC);
		ASSERT(chemIC.polyh);

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
	if (CICChemZoneActor::s_Property)
	{
		CICChemZoneActor::s_Property->SetColor(CICChemZoneActor::s_color);
	}	
	if (CICChemZoneActor::s_OutlineProperty)
	{
		CICChemZoneActor::s_OutlineProperty->SetColor(CICChemZoneActor::s_color);
		CICChemZoneActor::s_OutlineProperty->SetEdgeColor(CICChemZoneActor::s_color);	
		CICChemZoneActor::s_OutlineProperty->SetAmbientColor(CICChemZoneActor::s_color);
	}
}

Polyhedron*& CICChemZoneActor::GetPolyhedron(void)
{
	return this->m_chemIC.polyh;
}
