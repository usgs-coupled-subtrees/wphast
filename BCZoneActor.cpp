#include "StdAfx.h"
#include "BCZoneActor.h"
#include "resource.h"

#include "property.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
#include <vtkPropAssembly.h>

#include "PropertyTreeControlBar.h"
#include "BCFluxPropertyPage.h"
#include "BCFluxPropertyPage2.h"
#include "BCLeakyPropertyPage.h"
// COMMENT: {4/11/2005 2:14:18 PM}#include "BCSpecifiedPropertyPage.h"
#include "BCSpecifiedHeadPropertyPage.h"
#include "ETSLayout/ETSLayout.h"

#include "WPhastDoc.h"
#include "SetBCAction.h"
#include "ZoneCreateAction.h"


vtkCxxRevisionMacro(CBCZoneActor, "$Revision$");
vtkStandardNewMacro(CBCZoneActor);

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBCZoneActor::CBCZoneActor(void)
{
	// must always have a zone for CZoneActor::SetBounds
	if (this->m_bc.zone == 0)
	{
		this->m_bc.zone = new struct zone();
	}
	this->m_pZone = this->m_bc.zone;
}

CBCZoneActor::~CBCZoneActor(void)
{
}

void CBCZoneActor::Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CBC& bc)
{
	CZoneCreateAction<CBCZoneActor>* pAction = new CZoneCreateAction<CBCZoneActor>(
		pWPhastDoc,
		pWPhastDoc->GetNextZoneName(),
		zone.x1,
		zone.x2,
		zone.y1,
		zone.y2,
		zone.z1,
		zone.z2
		);
	pAction->GetZoneActor()->SetBC(bc);
	pWPhastDoc->Execute(pAction);
}

void CBCZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
	HTREEITEM htiBC = pTreeControlBar->GetBCNode();
	this->InsertAt(pTreeCtrl, htiBC, TVI_LAST);

	// set visible checkmark
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
}

void CBCZoneActor::InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	CString str = this->GetTreeHeading();
	this->m_hti = pTreeCtrl->InsertItem(str, hParent, hInsertAfter);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti); // might want to move this
	this->Update(pTreeCtrl, this->m_hti);
}

CString CBCZoneActor::GetTreeHeading(void)const
{
	CString str;
	// UNDEFINED, SPECIFIED, FLUX, LEAKY
	switch (this->m_bc.bc_type)
	{
		case UNDEFINED:
			str.Format(_T("UNDEFINED %s"), this->GetName());
			break;
		case SPECIFIED:
			str.Format(_T("SPECIFIED_HEAD %s"), this->GetName());
			break;
		case FLUX:
			str.Format(_T("FLUX %s"), this->GetName());
			break;
		case LEAKY:
			str.Format(_T("LEAKY %s"), this->GetName());
			break;
		default:
			str.Format(_T("UNKNOWN_BC %s"), this->GetName());
			break;
	}
	return str;
}

void CBCZoneActor::UpdateTree(CPropertyTreeControlBar* pTreeControlBar)
{
	this->Update(pTreeControlBar->GetTreeCtrl());
}

void CBCZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	ASSERT(htiParent == this->GetHTreeItem());
	this->Update(pTreeCtrl);
}

void CBCZoneActor::Update(CTreeCtrl* pTreeCtrl)
{
	this->Update(pTreeCtrl, this->GetHTreeItem(), this->GetBC());
}

void CBCZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, const CBC& crBC)
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiParent))
	{
		pTreeCtrl->DeleteItem(hChild);
	}

	switch (crBC.bc_type)
	{
		case UNDEFINED:
			ASSERT(FALSE);
			break;

		case SPECIFIED:
			// head
			crBC.m_bc_head.InsertItem(pTreeCtrl, "head", htiParent);

			// associated_solution
			if (crBC.bc_solution_type == ASSOCIATED)
			{
				crBC.m_bc_solution.InsertItem(pTreeCtrl, "associated_solution", htiParent);
			}

			// fixed_solution
			if (crBC.bc_solution_type == FIXED)
			{
				crBC.m_bc_solution.InsertItem(pTreeCtrl, "fixed_solution", htiParent);
			}
			break;

		case FLUX:
			// flux
			crBC.m_bc_flux.InsertItem(pTreeCtrl, "flux", htiParent);

			// associated_solution
			crBC.m_bc_solution.InsertItem(pTreeCtrl, "associated_solution", htiParent);

			// face
			if (crBC.face_defined)
			{
				switch (crBC.face)
				{
					case 0:
						pTreeCtrl->InsertItem(_T("face X"), htiParent);						
						break;
					case 1:
						pTreeCtrl->InsertItem(_T("face Y"), htiParent);						
						break;
					case 2:
						pTreeCtrl->InsertItem(_T("face Z"), htiParent);						
						break;
				}
			}
			break;

		case LEAKY:
			// head
			crBC.m_bc_head.InsertItem(pTreeCtrl, "head", htiParent);

			// associated_solution
			crBC.m_bc_solution.InsertItem(pTreeCtrl, "associated_solution", htiParent);

			// thickness
			if (crBC.bc_thick && crBC.bc_thick->type != UNDEFINED)
			{
				static_cast<Cproperty*>(crBC.bc_thick)->Insert(pTreeCtrl, htiParent, "thickness");
			}

			// hydraulic_conductivity
			if (crBC.bc_k && crBC.bc_k->type != UNDEFINED)
			{
				static_cast<Cproperty*>(crBC.bc_k)->Insert(pTreeCtrl, htiParent, "hydraulic_conductivity");
			}

			// face
			if (crBC.face_defined)
			{
				switch (crBC.face)
				{
					case 0:
						pTreeCtrl->InsertItem(_T("face X"), htiParent);						
						break;
					case 1:
						pTreeCtrl->InsertItem(_T("face Y"), htiParent);						
						break;
					case 2:
						pTreeCtrl->InsertItem(_T("face Z"), htiParent);						
						break;
				}
			}
			break;

		default:
			ASSERT(FALSE);
			break;
	}
}

void CBCZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	this->Edit(pTreeCtrl, 1);
}

void CBCZoneActor::Edit(CTreeCtrl* pTreeCtrl, int nStressPeriod)
{
	CString str;
	str.Format(_T("%s Properties"), this->GetName());
	ETSLayoutPropertySheet props(str);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	if (this->m_bc.bc_type == FLUX)
	{
		CBCFluxPropertyPage2 fluxProps;
		props.AddPage(&fluxProps);
		fluxProps.SetProperties(this->GetBC());
		if (props.DoModal() == IDOK)
		{
			CBC bc;
			fluxProps.GetProperties(bc);
			pDoc->Execute(new CSetBCAction(this, pTreeCtrl, bc));
		}
	}
	else if (this->m_bc.bc_type == LEAKY)
	{
		CBCLeakyPropertyPage leakyProps;
		props.AddPage(&leakyProps);
		leakyProps.SetProperties(this->GetBC());		
		if (props.DoModal() == IDOK)
		{
			CBC bc;
			leakyProps.GetProperties(bc);
			pDoc->Execute(new CSetBCAction(this, pTreeCtrl, bc));
		}
	}
	else if (this->m_bc.bc_type == SPECIFIED)
	{
		CBCSpecifiedHeadPropertyPage specified;
		props.AddPage(&specified);
		specified.SetProperties(this->GetBC());		
		if (props.DoModal() == IDOK)
		{
			CBC bc;
			specified.GetProperties(bc);
			pDoc->Execute(new CSetBCAction(this, pTreeCtrl, bc));
		}		
	}
	else 
	{
		ASSERT(FALSE);
	}
}

CBC CBCZoneActor::GetBC(void)const
{
	return this->m_bc;
}

void CBCZoneActor::SetBC(const CBC& rBC)
{
	ASSERT(this->m_bc.zone);
	CZone zoneSave(*this->m_bc.zone);

	this->m_bc = rBC;
	if (this->m_bc.zone == 0)
	{
		this->m_bc.zone = new CZone();
	}
	(*this->m_bc.zone) = zoneSave;
	this->m_pZone = this->m_bc.zone;
}

HTREEITEM CBCZoneActor::GetHTreeItem(void)const
{
	return this->m_hti;
}

void CBCZoneActor::SetHTreeItem(HTREEITEM htItem)
{
	this->m_hti = htItem;
}

void CBCZoneActor::Serialize(bool bStoring, hid_t loc_id, const CUnits& units)
{
	ASSERT(this->GetName()); // must have name

	CZoneActor::Serialize(bStoring, loc_id);

	if (bStoring)
	{
		// serialize bc
		this->m_bc.Serialize(bStoring, loc_id);
	}
	else
	{
		// serialize bc
		this->m_bc.Serialize(bStoring, loc_id);
		this->SetUnits(units);
	}
}

void CBCZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyBC())
	{
		pPropAssembly->AddPart(this);
		if (!pWPhastDoc->GetPropCollection()->IsItemPresent(pPropAssembly))
		{
			pWPhastDoc->GetPropCollection()->AddItem(pPropAssembly);
		}
	}
#ifdef _DEBUG
	else
	{
		ASSERT(FALSE);
	}
#endif
}

void CBCZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyBC())
	{
		pPropAssembly->RemovePart(this);
		// VTK HACK
		// This is req'd because ReleaseGraphicsResources is not called when
		// vtkPropAssembly::RemovePart(vtkProp *prop) is called
		pWPhastDoc->ReleaseGraphicsResources(this);
	}
#ifdef _DEBUG
	else
	{
		ASSERT(FALSE);
	}
#endif
}

