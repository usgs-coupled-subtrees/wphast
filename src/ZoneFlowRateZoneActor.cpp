#include "StdAfx.h"
#include "resource.h"
#include "ZoneFlowRateZoneActor.h"
#include "PropertyTreeControlBar.h"
#include "ZoneCreateAction.h"
#include "ZoneFlowRatePropertyPage.h"
#include "property.h"

#include "WPhastDoc.h"
#include "Protect.h"
#include "Global.h"
#include "WPhastMacros.h"

#include <vtkPropAssembly.h>
#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro

vtkCxxRevisionMacro(CZoneFlowRateZoneActor, "$Revision: 3286 $");
vtkStandardNewMacro(CZoneFlowRateZoneActor);

const char CZoneFlowRateZoneActor::szHeading[] = "ZoneFlowRates";
double CZoneFlowRateZoneActor::s_color[3] = {0., 0., 0.};
vtkProperty* CZoneFlowRateZoneActor::s_Property = 0;
vtkProperty* CZoneFlowRateZoneActor::s_OutlineProperty = 0;

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#if defined(__WPHAST__)
CLIPFORMAT Zone_budget::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:Zone_budget"));
#endif

CZoneFlowRateZoneActor::CZoneFlowRateZoneActor(void)
{
	static StaticInit init; // constructs/destructs s_Property/s_OutlineProperty

	// cube
	this->CubeActor->SetProperty(CZoneFlowRateZoneActor::s_Property);

	// outline
	this->OutlineActor->SetProperty(CZoneFlowRateZoneActor::s_OutlineProperty);

	//{{ HACK
	if (this->m_zone_budget.Get_n_user() == 0)
	{
		if (CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd)
		{
			ASSERT_VALID(pFrame);
			if (CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument()))
			{
				ASSERT_VALID(pDoc);
				this->m_zone_budget.Set_n_user(pDoc->GetNextZoneFlowRatesNumber());
			}
		}
	}
	//}} HACK
}

CZoneFlowRateZoneActor::~CZoneFlowRateZoneActor(void)
{
}

Polyhedron*& CZoneFlowRateZoneActor::GetPolyhedron(void)
{
	return this->m_zone_budget.polyh;
}

void CZoneFlowRateZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
	HTREEITEM htiZFR = pTreeControlBar->GetZoneFlowRatesNode();
	this->InsertAt(pTreeCtrl, htiZFR, hInsertAfter);
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
}

void CZoneFlowRateZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	CZoneActor::Update(pTreeCtrl, htiParent);

	// update description
	//
	CString str;
	if (this->m_zone_budget.Get_description().size())
	{
		str.Format("Zone flow rate %d - %s", this->m_zone_budget.Get_n_user(), this->m_zone_budget.Get_description().c_str());
	}
	else
	{
		str.Format("Zone flow rate %d", this->m_zone_budget.Get_n_user());
	}
	pTreeCtrl->SetItemText(htiParent, str);

	if (this->m_zone_budget.combo.size())
	{
		CString str("combination");
		CString app;
		std::vector<int>::const_iterator cit = this->m_zone_budget.combo.begin();
		for (; cit != this->m_zone_budget.combo.end(); ++cit)
		{
			app.Format(" %d", (*cit));
			str += app;
		}
		pTreeCtrl->InsertItem(str, htiParent);
	}

}

void CZoneFlowRateZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	std::set<int> usedNums;
	pDoc->GetUsedZoneFlowRates(usedNums);

	// remove this well number from used list
	std::set<int>::iterator iter = usedNums.find(this->m_zone_budget.Get_n_user());
	ASSERT(iter != usedNums.end());
	if (iter != usedNums.end())
	{
		usedNums.erase(iter);
	}

	CZoneFlowRatePropertyPage page;
	page.SetUsedZoneFlowRates(usedNums);
	page.SetProperties(this->m_zone_budget);

	CString str;
	str.Format(_T("Zone Flow Rate %d Properties"), this->m_zone_budget.Get_n_user());
	CPropertySheet props(str);
	props.AddPage(&page);

	if (props.DoModal() == IDOK)
	{
		Zone_budget zone_budget;
		page.GetProperties(zone_budget);
		ASSERT(zone_budget.polyh);
		//pDoc->Execute(new CSetMediaAction(this, pTreeCtrl, grid_elt, mediaSpreadProps.GetDesc()));
		//{{
		this->SetData(zone_budget);
		this->Update(pTreeCtrl, this->m_hti);
		//}}
	}
}

void CZoneFlowRateZoneActor::InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	this->m_hti = pTreeCtrl->InsertItem(this->GetNameDesc(), hParent, hInsertAfter);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti); // might want to move this -- this causes the tree control to redraw
	this->Update(pTreeCtrl, this->m_hti);
}

void CZoneFlowRateZoneActor::Remove(CPropertyTreeControlBar* pTreeControlBar)
{
	CZoneActor::Remove(pTreeControlBar);
}

void CZoneFlowRateZoneActor::UnRemove(CPropertyTreeControlBar* pTreeControlBar)
{
	HTREEITEM htiZFR = pTreeControlBar->GetZoneFlowRatesNode();
	ASSERT(this->m_hParent      == htiZFR);
	ASSERT(this->m_hInsertAfter != 0);

	vtkPropAssembly *pPropAssembly = pTreeControlBar->GetDocument()->GetPropAssemblyZoneFlowRates();
	pPropAssembly->AddPart(this);

	CZoneActor::UnRemove(pTreeControlBar);
}

void CZoneFlowRateZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyZoneFlowRates())
	{
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(this));
		pPropAssembly->AddPart(this);
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(this));
		pWPhastDoc->AddPropAssembly(pPropAssembly);
	}
#ifdef _DEBUG
	else ASSERT(FALSE);
#endif
}

void CZoneFlowRateZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyZoneFlowRates())
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

void CZoneFlowRateZoneActor::SetStaticColor(COLORREF cr)
{
	CZoneFlowRateZoneActor::s_color[0] = (double)GetRValue(cr)/255.;
	CZoneFlowRateZoneActor::s_color[1] = (double)GetGValue(cr)/255.;
	CZoneFlowRateZoneActor::s_color[2] = (double)GetBValue(cr)/255.;	
	if (CZoneFlowRateZoneActor::s_Property)
	{
		CZoneFlowRateZoneActor::s_Property->SetColor(CZoneFlowRateZoneActor::s_color);
	}
	if (CZoneFlowRateZoneActor::s_OutlineProperty)
	{
		CZoneFlowRateZoneActor::s_OutlineProperty->SetColor(CZoneFlowRateZoneActor::s_color);
		CZoneFlowRateZoneActor::s_OutlineProperty->SetEdgeColor(CZoneFlowRateZoneActor::s_color);
		CZoneFlowRateZoneActor::s_OutlineProperty->SetAmbientColor(CZoneFlowRateZoneActor::s_color);
	}
}

void CZoneFlowRateZoneActor::SetData(const Zone_budget& zone_budget)
{
	// NOTE: Use SetBounds to set the zone
	//
	CProtect protect(this->m_zone_budget.polyh);
	this->m_zone_budget = zone_budget;
	//{{ HACK
	if (this->m_zone_budget.Get_n_user() == 0)
	{
		if (CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd)
		{
			ASSERT_VALID(pFrame);
			if (CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument()))
			{
				ASSERT_VALID(pDoc);
				this->m_zone_budget.Set_n_user(pDoc->GetNextZoneFlowRatesNumber());
			}
		}
	}
	//}} HACK
}

Zone_budget CZoneFlowRateZoneActor::GetData(void)const
{
	return this->m_zone_budget;
}

void CZoneFlowRateZoneActor::Create(CWPhastDoc* pWPhastDoc, const Zone_budget& zone_budget)
{
	ASSERT(zone_budget.polyh && ::AfxIsValidAddress(zone_budget.polyh, sizeof(Polyhedron)));
	CZoneCreateAction<CZoneFlowRateZoneActor>* pAction = new CZoneCreateAction<CZoneFlowRateZoneActor>(
		pWPhastDoc,
		zone_budget.polyh,
		""
		);
	pAction->GetZoneActor()->SetData(zone_budget);
	pWPhastDoc->Execute(pAction);
}

void CZoneFlowRateZoneActor::Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc)
{
	CZoneActor::Serialize(bStoring, loc_id, pWPhastDoc);

	if (bStoring)
	{
		// store zone_budget
		this->m_zone_budget.Serialize(bStoring, loc_id);
	}
	else
	{
		// load zone_budget
		this->m_zone_budget.Serialize(bStoring, loc_id);

		// color
		if (CZoneFlowRateZoneActor::s_Property)
		{
			this->CubeActor->SetProperty(CZoneFlowRateZoneActor::s_Property);
		}
	}
}

void Zone_budget::Serialize(bool bStoring, hid_t loc_id)
{
	HDF_GETSET_MACRO(n_user, H5T_NATIVE_INT);
	HDF_STD_STRING_MACRO(description);

	static const char szCombination[] = "combination";
	herr_t status;
	hsize_t count;
	if (bStoring)
	{
		if (this->combo.size())
		{
			count = this->combo.size();
			int *numbers = new int[count];

			std::vector<int>::const_iterator cit = this->combo.begin();
			for (size_t i = 0; cit != this->combo.end(); ++cit, ++i)
			{
				numbers[i] = *cit;
			}
			status = CGlobal::HDFSerializeWithSize(bStoring, loc_id, szCombination, H5T_NATIVE_INT, count, numbers);
			delete[] numbers;
		}
	}
	else
	{
		ASSERT(this->combo.size() == 0);
		this->combo.clear();
		int *numbers = NULL;
		status = CGlobal::HDFSerializeAllocate(bStoring, loc_id, szCombination, count, &numbers);
		if (status >= 0)
		{
			for (size_t i = 0; i < count; ++i)
			{
				this->combo.push_back(numbers[i]);
			}
		}
		delete[] numbers;
	}
}

void Zone_budget::Serialize(CArchive& ar)
{
	static const char szZone_budget[] = "Zone_budget";
	static int version = 1;

	CString type;
	int ver = version;

	// type and version header
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szZone_budget;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szZone_budget) == 0);

		// read version in case changes need to be made
		ar >> ver;
	}

	CGlobal::Serialize(&(this->polyh), ar);

	// type and version footer
	//
	if (ar.IsStoring())
	{
		// store type as string
		//
		type = szZone_budget;
		ar << type;

		// store version in case changes need to be made
		ar << version;
	}
	else
	{
		// read type as string
		//
		ar >> type;
		ASSERT(type.Compare(szZone_budget) == 0);

		// read version in case changes need to be made
		ar >> ver;
	}
}
