#include "StdAfx.h"
#include "resource.h"
#include "PrintZoneActor.h"
#include "PropertyTreeControlBar.h"
#include "ZoneCreateAction.h"
// COMMENT: {2/2/2016 5:55:06 PM}#include "ZoneFlowRatePropertyPage.h"
#include "property.h"

#include "WPhastDoc.h"
#include "Protect.h"
#include "Global.h"
#include "GridKeyword.h"
#include "WPhastMacros.h"
// COMMENT: {2/2/2016 5:55:14 PM}#include "SetZoneBudgetAction.h"

#include <vtkPropAssembly.h>
#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro

vtkCxxRevisionMacro(CPrintZoneActor, "$Revision: 3286 $");
vtkStandardNewMacro(CPrintZoneActor);

const char CPrintZoneActor::szHeading[] = "ZoneFlowRates";
double CPrintZoneActor::s_color[3] = {0., 0., 0.};
vtkProperty* CPrintZoneActor::s_Property = 0;
vtkProperty* CPrintZoneActor::s_OutlineProperty = 0;

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPrintZoneActor::CPrintZoneActor(void)
{
	static StaticInit init; // constructs/destructs s_Property/s_OutlineProperty

	// cube
	this->CubeActor->SetProperty(CPrintZoneActor::s_Property);

	// outline
	this->OutlineActor->SetProperty(CPrintZoneActor::s_OutlineProperty);

	//{{ HACK
// COMMENT: {2/2/2016 5:59:29 PM}	if (this->m_zone_budget.Get_n_user() == 0)
// COMMENT: {2/2/2016 5:59:29 PM}	{
// COMMENT: {2/2/2016 5:59:29 PM}		if (CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd)
// COMMENT: {2/2/2016 5:59:29 PM}		{
// COMMENT: {2/2/2016 5:59:29 PM}			ASSERT_VALID(pFrame);
// COMMENT: {2/2/2016 5:59:29 PM}			if (CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument()))
// COMMENT: {2/2/2016 5:59:29 PM}			{
// COMMENT: {2/2/2016 5:59:29 PM}				ASSERT_VALID(pDoc);
// COMMENT: {2/2/2016 5:59:29 PM}				this->m_zone_budget.Set_n_user(pDoc->GetNextZoneFlowRatesNumber());
// COMMENT: {2/2/2016 5:59:29 PM}			}
// COMMENT: {2/2/2016 5:59:29 PM}		}
// COMMENT: {2/2/2016 5:59:29 PM}	}
	//}} HACK
}

CPrintZoneActor::~CPrintZoneActor(void)
{
}

Polyhedron*& CPrintZoneActor::GetPolyhedron(void)
{
	return this->m_printZone.polyh;
}

void CPrintZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
	HTREEITEM htiParent = pTreeControlBar->GetPLChemNode();
	this->InsertAt(pTreeCtrl, htiParent, hInsertAfter);
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
}

void CPrintZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	CZoneActor::Update(pTreeCtrl, htiParent);

// COMMENT: {2/2/2016 6:02:31 PM}	Zone_budget relative(this->m_zone_budget);
	CPrintZone relative(this->m_printZone);
	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	if (pFrame)
	{
		ASSERT_VALID(pFrame);
		CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
		ASSERT_VALID(pDoc);

		CGlobal::PathsAbsoluteToRelative(pDoc->GetDefaultPathName(), pDoc, relative);
	}

// COMMENT: {2/2/2016 6:24:00 PM}	// update description
// COMMENT: {2/2/2016 6:24:00 PM}	//
// COMMENT: {2/2/2016 6:24:00 PM}	CString str;
// COMMENT: {2/2/2016 6:24:00 PM}	if (relative.Get_description().size())
// COMMENT: {2/2/2016 6:24:00 PM}	{
// COMMENT: {2/2/2016 6:24:00 PM}		str.Format("Zone flow rate %d - %s", relative.Get_n_user(), relative.Get_description().c_str());
// COMMENT: {2/2/2016 6:24:00 PM}	}
// COMMENT: {2/2/2016 6:24:00 PM}	else
// COMMENT: {2/2/2016 6:24:00 PM}	{
// COMMENT: {2/2/2016 6:24:00 PM}		str.Format("Zone flow rate %d", relative.Get_n_user());
// COMMENT: {2/2/2016 6:24:00 PM}	}
// COMMENT: {2/2/2016 6:24:00 PM}	pTreeCtrl->SetItemText(htiParent, str);

// COMMENT: {2/2/2016 6:24:26 PM}	if (relative.combo.size())
// COMMENT: {2/2/2016 6:24:26 PM}	{
// COMMENT: {2/2/2016 6:24:26 PM}		CString str("combination");
// COMMENT: {2/2/2016 6:24:26 PM}		CString app;
// COMMENT: {2/2/2016 6:24:26 PM}		std::vector<int>::const_iterator cit = relative.combo.begin();
// COMMENT: {2/2/2016 6:24:26 PM}		for (; cit != relative.combo.end(); ++cit)
// COMMENT: {2/2/2016 6:24:26 PM}		{
// COMMENT: {2/2/2016 6:24:26 PM}			app.Format(" %d", (*cit));
// COMMENT: {2/2/2016 6:24:26 PM}			str += app;
// COMMENT: {2/2/2016 6:24:26 PM}		}
// COMMENT: {2/2/2016 6:24:26 PM}		pTreeCtrl->InsertItem(str, htiParent);
// COMMENT: {2/2/2016 6:24:26 PM}	}
// COMMENT: {2/2/2016 6:24:26 PM}
// COMMENT: {2/2/2016 6:24:26 PM}	if (relative.Get_write_heads())
// COMMENT: {2/2/2016 6:24:26 PM}	{
// COMMENT: {2/2/2016 6:24:26 PM}		CString str;
// COMMENT: {2/2/2016 6:24:26 PM}		str.Format("write_heads_xyzt  %s", relative.Get_filename_heads().c_str());
// COMMENT: {2/2/2016 6:24:26 PM}		pTreeCtrl->InsertItem(str, htiParent);
// COMMENT: {2/2/2016 6:24:26 PM}	}
}

void CPrintZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

// COMMENT: {2/2/2016 6:26:17 PM}	std::set<int> usedNums;
// COMMENT: {2/2/2016 6:26:17 PM}	pDoc->GetUsedZoneFlowRatesNumbers(usedNums);
// COMMENT: {2/2/2016 6:26:17 PM}
// COMMENT: {2/2/2016 6:26:17 PM}	// remove this well number from used list
// COMMENT: {2/2/2016 6:26:17 PM}	std::set<int>::iterator iter = usedNums.find(this->m_zone_budget.Get_n_user());
// COMMENT: {2/2/2016 6:26:17 PM}	ASSERT(iter != usedNums.end());
// COMMENT: {2/2/2016 6:26:17 PM}	if (iter != usedNums.end())
// COMMENT: {2/2/2016 6:26:17 PM}	{
// COMMENT: {2/2/2016 6:26:17 PM}		usedNums.erase(iter);
// COMMENT: {2/2/2016 6:26:17 PM}	}
// COMMENT: {2/2/2016 6:26:17 PM}
// COMMENT: {2/2/2016 6:26:17 PM}	CZoneFlowRatePropertyPage page;
// COMMENT: {2/2/2016 6:26:17 PM}	page.SetUsedZoneFlowRates(usedNums);
// COMMENT: {2/2/2016 6:26:17 PM}	page.SetProperties(this->m_zone_budget);
// COMMENT: {2/2/2016 6:26:17 PM}
// COMMENT: {2/2/2016 6:26:17 PM}	CString str;
// COMMENT: {2/2/2016 6:26:17 PM}	str.Format(_T("Zone Flow Rate %d Properties"), this->m_zone_budget.Get_n_user());
// COMMENT: {2/2/2016 6:26:17 PM}	CPropertySheet props(str);
// COMMENT: {2/2/2016 6:26:17 PM}	props.AddPage(&page);
// COMMENT: {2/2/2016 6:26:17 PM}
// COMMENT: {2/2/2016 6:26:17 PM}	if (props.DoModal() == IDOK)
// COMMENT: {2/2/2016 6:26:17 PM}	{
// COMMENT: {2/2/2016 6:26:17 PM}		Zone_budget zone_budget;
// COMMENT: {2/2/2016 6:26:17 PM}		page.GetProperties(zone_budget);
// COMMENT: {2/2/2016 6:26:17 PM}		ASSERT(zone_budget.polyh);
// COMMENT: {2/2/2016 6:26:17 PM}// COMMENT: {9/2/2011 8:47:40 PM}		//pDoc->Execute(new CSetMediaAction(this, pTreeCtrl, grid_elt, mediaSpreadProps.GetDesc()));
// COMMENT: {2/2/2016 6:26:17 PM}// COMMENT: {9/2/2011 8:47:40 PM}		//{{
// COMMENT: {2/2/2016 6:26:17 PM}// COMMENT: {9/2/2011 8:47:40 PM}		this->SetData(zone_budget);
// COMMENT: {2/2/2016 6:26:17 PM}// COMMENT: {9/2/2011 8:47:40 PM}		this->Update(pTreeCtrl, this->m_hti);
// COMMENT: {2/2/2016 6:26:17 PM}// COMMENT: {9/2/2011 8:47:40 PM}		//}}
// COMMENT: {2/2/2016 6:26:17 PM}		pDoc->Execute(new CSetZoneBudgetAction(this, pTreeCtrl, zone_budget));
// COMMENT: {2/2/2016 6:26:17 PM}	}
}

void CPrintZoneActor::InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	this->m_hti = pTreeCtrl->InsertItem(this->GetNameDesc(), hParent, hInsertAfter);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti); // might want to move this -- this causes the tree control to redraw
	this->Update(pTreeCtrl, this->m_hti);
}

void CPrintZoneActor::Remove(CPropertyTreeControlBar* pTreeControlBar)
{
	CZoneActor::Remove(pTreeControlBar);
}

void CPrintZoneActor::UnRemove(CPropertyTreeControlBar* pTreeControlBar)
{
	HTREEITEM htiZFR = pTreeControlBar->GetZoneFlowRatesNode();
	ASSERT(this->m_hParent      == htiZFR);
	ASSERT(this->m_hInsertAfter != 0);

	vtkPropAssembly *pPropAssembly = pTreeControlBar->GetDocument()->GetPropAssemblyPrintLocations();
	pPropAssembly->AddPart(this);

	CZoneActor::UnRemove(pTreeControlBar);
}

void CPrintZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyPrintLocations())
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

void CPrintZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyPrintLocations())
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

void CPrintZoneActor::SetStaticColor(COLORREF cr)
{
	CPrintZoneActor::s_color[0] = (double)GetRValue(cr)/255.;
	CPrintZoneActor::s_color[1] = (double)GetGValue(cr)/255.;
	CPrintZoneActor::s_color[2] = (double)GetBValue(cr)/255.;	
	if (CPrintZoneActor::s_Property)
	{
		CPrintZoneActor::s_Property->SetColor(CPrintZoneActor::s_color);
	}
	if (CPrintZoneActor::s_OutlineProperty)
	{
		CPrintZoneActor::s_OutlineProperty->SetColor(CPrintZoneActor::s_color);
		CPrintZoneActor::s_OutlineProperty->SetEdgeColor(CPrintZoneActor::s_color);
		CPrintZoneActor::s_OutlineProperty->SetAmbientColor(CPrintZoneActor::s_color);
	}
}

void CPrintZoneActor::SetData(const CPrintZone& printZone)
{
	// NOTE: Use SetBounds to set the zone
	//
	CProtect protect(this->m_printZone.polyh);
// COMMENT: {2/2/2016 9:35:54 PM}	this->m_zone_budget = zone_budget;
	this->m_printZone = printZone;
	//{{ HACK
// COMMENT: {2/2/2016 9:36:48 PM}	if (this->m_zone_budget.Get_n_user() == 0)
// COMMENT: {2/2/2016 9:36:48 PM}	{
// COMMENT: {2/2/2016 9:36:48 PM}		if (CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd)
// COMMENT: {2/2/2016 9:36:48 PM}		{
// COMMENT: {2/2/2016 9:36:48 PM}			ASSERT_VALID(pFrame);
// COMMENT: {2/2/2016 9:36:48 PM}			if (CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument()))
// COMMENT: {2/2/2016 9:36:48 PM}			{
// COMMENT: {2/2/2016 9:36:48 PM}				ASSERT_VALID(pDoc);
// COMMENT: {2/2/2016 9:36:48 PM}				this->m_zone_budget.Set_n_user(pDoc->GetNextZoneFlowRatesNumber());
// COMMENT: {2/2/2016 9:36:48 PM}			}
// COMMENT: {2/2/2016 9:36:48 PM}		}
// COMMENT: {2/2/2016 9:36:48 PM}	}
	//}} HACK
}

CPrintZone CPrintZoneActor::GetData(void)const
{
	return this->m_printZone;
}

void CPrintZoneActor::Create(CWPhastDoc* pWPhastDoc, const CPrintZone& printZone)
{
	ASSERT(printZone.polyh && ::AfxIsValidAddress(printZone.polyh, sizeof(Polyhedron)));

	CGridKeyword gk;
	pWPhastDoc->GetGridKeyword(gk);
	CZoneCreateAction<CPrintZoneActor>* pAction = new CZoneCreateAction<CPrintZoneActor>(
		pWPhastDoc,
		printZone.polyh,
		gk.m_grid_origin,
		gk.m_grid_angle,
		""
		);
	pAction->GetZoneActor()->SetData(printZone);
	pWPhastDoc->Execute(pAction);
}

void CPrintZoneActor::Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc)
{
	CZoneActor::Serialize(bStoring, loc_id, pWPhastDoc);

	if (bStoring)
	{
		// store zone_budget
		this->m_printZone.Serialize(bStoring, loc_id);
	}
	else
	{
		// load zone_budget
		this->m_printZone.Serialize(bStoring, loc_id);

		// color
		if (CPrintZoneActor::s_Property)
		{
			this->CubeActor->SetProperty(CPrintZoneActor::s_Property);
		}
	}
}

HTREEITEM CPrintZoneActor::GetHTreeItem(void)const
{
	ASSERT(this->m_hti);
	return this->m_hti;
}

// COMMENT: {2/2/2016 5:48:58 PM}void Zone_budget::Serialize(bool bStoring, hid_t loc_id)
// COMMENT: {2/2/2016 5:48:58 PM}{
// COMMENT: {2/2/2016 5:48:58 PM}	HDF_GETSET_MACRO(n_user, H5T_NATIVE_INT);
// COMMENT: {2/2/2016 5:48:58 PM}	HDF_STD_STRING_MACRO(description);
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}	static const char szCombination[] = "combination";
// COMMENT: {2/2/2016 5:48:58 PM}	static const char szWriteHeads[]  = "write_heads";
// COMMENT: {2/2/2016 5:48:58 PM}	static const char szFNHeads[]     = "filename_heads";
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}	herr_t status;
// COMMENT: {2/2/2016 5:48:58 PM}	hsize_t count;
// COMMENT: {2/2/2016 5:48:58 PM}	if (bStoring)
// COMMENT: {2/2/2016 5:48:58 PM}	{
// COMMENT: {2/2/2016 5:48:58 PM}		if (this->combo.size())
// COMMENT: {2/2/2016 5:48:58 PM}		{
// COMMENT: {2/2/2016 5:48:58 PM}			count = this->combo.size();
// COMMENT: {2/2/2016 5:48:58 PM}			int *numbers = new int[count];
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}			std::vector<int>::const_iterator cit = this->combo.begin();
// COMMENT: {2/2/2016 5:48:58 PM}			for (size_t i = 0; cit != this->combo.end(); ++cit, ++i)
// COMMENT: {2/2/2016 5:48:58 PM}			{
// COMMENT: {2/2/2016 5:48:58 PM}				numbers[i] = *cit;
// COMMENT: {2/2/2016 5:48:58 PM}			}
// COMMENT: {2/2/2016 5:48:58 PM}			status = CGlobal::HDFSerializeWithSize(bStoring, loc_id, szCombination, H5T_NATIVE_INT, count, numbers);
// COMMENT: {2/2/2016 5:48:58 PM}			delete[] numbers;
// COMMENT: {2/2/2016 5:48:58 PM}		}
// COMMENT: {2/2/2016 5:48:58 PM}	}
// COMMENT: {2/2/2016 5:48:58 PM}	else
// COMMENT: {2/2/2016 5:48:58 PM}	{
// COMMENT: {2/2/2016 5:48:58 PM}		ASSERT(this->combo.size() == 0);
// COMMENT: {2/2/2016 5:48:58 PM}		this->combo.clear();
// COMMENT: {2/2/2016 5:48:58 PM}		int *numbers = NULL;
// COMMENT: {2/2/2016 5:48:58 PM}		status = CGlobal::HDFSerializeAllocate(bStoring, loc_id, szCombination, count, &numbers);
// COMMENT: {2/2/2016 5:48:58 PM}		if (status >= 0)
// COMMENT: {2/2/2016 5:48:58 PM}		{
// COMMENT: {2/2/2016 5:48:58 PM}			for (size_t i = 0; i < count; ++i)
// COMMENT: {2/2/2016 5:48:58 PM}			{
// COMMENT: {2/2/2016 5:48:58 PM}				this->combo.push_back(numbers[i]);
// COMMENT: {2/2/2016 5:48:58 PM}			}
// COMMENT: {2/2/2016 5:48:58 PM}		}
// COMMENT: {2/2/2016 5:48:58 PM}		delete[] numbers;
// COMMENT: {2/2/2016 5:48:58 PM}	}
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}	// write_heads_xyzt
// COMMENT: {2/2/2016 5:48:58 PM}	status = CGlobal::HDFSerializeBool(bStoring, loc_id, szWriteHeads, this->write_heads);
// COMMENT: {2/2/2016 5:48:58 PM}	ASSERT(!bStoring || status >= 0); // if storing assert status is valid
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}	// filename_heads
// COMMENT: {2/2/2016 5:48:58 PM}	if (this->write_heads)
// COMMENT: {2/2/2016 5:48:58 PM}	{
// COMMENT: {2/2/2016 5:48:58 PM}		status = CGlobal::HDFSerializeString(bStoring, loc_id, szFNHeads, this->filename_heads);
// COMMENT: {2/2/2016 5:48:58 PM}		ASSERT(!bStoring || status >= 0); // if storing assert status is valid
// COMMENT: {2/2/2016 5:48:58 PM}	}
// COMMENT: {2/2/2016 5:48:58 PM}}
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}void Zone_budget::Serialize(CArchive& ar)
// COMMENT: {2/2/2016 5:48:58 PM}{
// COMMENT: {2/2/2016 5:48:58 PM}	static const char szZone_budget[] = "Zone_budget";
// COMMENT: {2/2/2016 5:48:58 PM}	static int version = 3;
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}	CString type;
// COMMENT: {2/2/2016 5:48:58 PM}	int ver = version;
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}	// type and version header
// COMMENT: {2/2/2016 5:48:58 PM}	//
// COMMENT: {2/2/2016 5:48:58 PM}	if (ar.IsStoring())
// COMMENT: {2/2/2016 5:48:58 PM}	{
// COMMENT: {2/2/2016 5:48:58 PM}		// store type as string
// COMMENT: {2/2/2016 5:48:58 PM}		//
// COMMENT: {2/2/2016 5:48:58 PM}		type = szZone_budget;
// COMMENT: {2/2/2016 5:48:58 PM}		ar << type;
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}		// store version in case changes need to be made
// COMMENT: {2/2/2016 5:48:58 PM}		ar << version;
// COMMENT: {2/2/2016 5:48:58 PM}	}
// COMMENT: {2/2/2016 5:48:58 PM}	else
// COMMENT: {2/2/2016 5:48:58 PM}	{
// COMMENT: {2/2/2016 5:48:58 PM}		// read type as string
// COMMENT: {2/2/2016 5:48:58 PM}		//
// COMMENT: {2/2/2016 5:48:58 PM}		ar >> type;
// COMMENT: {2/2/2016 5:48:58 PM}		ASSERT(type.Compare(szZone_budget) == 0);
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}		// read version in case changes need to be made
// COMMENT: {2/2/2016 5:48:58 PM}		ar >> ver;
// COMMENT: {2/2/2016 5:48:58 PM}	}
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}	CGlobal::Serialize(&(this->polyh), ar);
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}	if (version >= 2)
// COMMENT: {2/2/2016 5:48:58 PM}	{
// COMMENT: {2/2/2016 5:48:58 PM}		if (ar.IsStoring())
// COMMENT: {2/2/2016 5:48:58 PM}		{
// COMMENT: {2/2/2016 5:48:58 PM}			ar << this->write_heads;
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}			CString str(this->filename_heads.c_str());
// COMMENT: {2/2/2016 5:48:58 PM}			ar << str;
// COMMENT: {2/2/2016 5:48:58 PM}		}
// COMMENT: {2/2/2016 5:48:58 PM}		else
// COMMENT: {2/2/2016 5:48:58 PM}		{
// COMMENT: {2/2/2016 5:48:58 PM}			ar >> this->write_heads;
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}			CString str;
// COMMENT: {2/2/2016 5:48:58 PM}			ar >> str;
// COMMENT: {2/2/2016 5:48:58 PM}			this->filename_heads = str;
// COMMENT: {2/2/2016 5:48:58 PM}		}
// COMMENT: {2/2/2016 5:48:58 PM}	}
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}	if (version >= 3)
// COMMENT: {2/2/2016 5:48:58 PM}	{
// COMMENT: {2/2/2016 5:48:58 PM}		if (ar.IsStoring())
// COMMENT: {2/2/2016 5:48:58 PM}		{
// COMMENT: {2/2/2016 5:48:58 PM}			type = "<combos>";
// COMMENT: {2/2/2016 5:48:58 PM}			ar << type;
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}			unsigned int n = (unsigned int)this->Get_combo().size();
// COMMENT: {2/2/2016 5:48:58 PM}			ar << n;
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}			if (this->Get_combo().size())
// COMMENT: {2/2/2016 5:48:58 PM}			{
// COMMENT: {2/2/2016 5:48:58 PM}				std::vector<int>::const_iterator cit = this->Get_combo().begin();
// COMMENT: {2/2/2016 5:48:58 PM}				for (; cit != this->Get_combo().end(); ++cit)
// COMMENT: {2/2/2016 5:48:58 PM}				{
// COMMENT: {2/2/2016 5:48:58 PM}					int n = (*cit);
// COMMENT: {2/2/2016 5:48:58 PM}					ar << n;
// COMMENT: {2/2/2016 5:48:58 PM}				}
// COMMENT: {2/2/2016 5:48:58 PM}			}
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}			type = "</combos>";
// COMMENT: {2/2/2016 5:48:58 PM}			ar << type;
// COMMENT: {2/2/2016 5:48:58 PM}		}
// COMMENT: {2/2/2016 5:48:58 PM}		else
// COMMENT: {2/2/2016 5:48:58 PM}		{
// COMMENT: {2/2/2016 5:48:58 PM}			ar >> type;
// COMMENT: {2/2/2016 5:48:58 PM}			ASSERT(type.Compare("<combos>") == 0);
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}			unsigned int n;
// COMMENT: {2/2/2016 5:48:58 PM}			ar >> n;
// COMMENT: {2/2/2016 5:48:58 PM}			if (n > 0)
// COMMENT: {2/2/2016 5:48:58 PM}			{
// COMMENT: {2/2/2016 5:48:58 PM}				int v;
// COMMENT: {2/2/2016 5:48:58 PM}				for (unsigned int i = 0; i < n; ++i)
// COMMENT: {2/2/2016 5:48:58 PM}				{					
// COMMENT: {2/2/2016 5:48:58 PM}					ar >> v;
// COMMENT: {2/2/2016 5:48:58 PM}					this->Get_combo().push_back(v);
// COMMENT: {2/2/2016 5:48:58 PM}				}
// COMMENT: {2/2/2016 5:48:58 PM}			}
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}			ar >> type;
// COMMENT: {2/2/2016 5:48:58 PM}			ASSERT(type.Compare("</combos>") == 0);
// COMMENT: {2/2/2016 5:48:58 PM}		}
// COMMENT: {2/2/2016 5:48:58 PM}	}
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}	// type and version footer
// COMMENT: {2/2/2016 5:48:58 PM}	//
// COMMENT: {2/2/2016 5:48:58 PM}	if (ar.IsStoring())
// COMMENT: {2/2/2016 5:48:58 PM}	{
// COMMENT: {2/2/2016 5:48:58 PM}		// store type as string
// COMMENT: {2/2/2016 5:48:58 PM}		//
// COMMENT: {2/2/2016 5:48:58 PM}		type = szZone_budget;
// COMMENT: {2/2/2016 5:48:58 PM}		ar << type;
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}		// store version in case changes need to be made
// COMMENT: {2/2/2016 5:48:58 PM}		ar << version;
// COMMENT: {2/2/2016 5:48:58 PM}	}
// COMMENT: {2/2/2016 5:48:58 PM}	else
// COMMENT: {2/2/2016 5:48:58 PM}	{
// COMMENT: {2/2/2016 5:48:58 PM}		// read type as string
// COMMENT: {2/2/2016 5:48:58 PM}		//
// COMMENT: {2/2/2016 5:48:58 PM}		ar >> type;
// COMMENT: {2/2/2016 5:48:58 PM}		ASSERT(type.Compare(szZone_budget) == 0);
// COMMENT: {2/2/2016 5:48:58 PM}
// COMMENT: {2/2/2016 5:48:58 PM}		// read version in case changes need to be made
// COMMENT: {2/2/2016 5:48:58 PM}		ar >> ver;
// COMMENT: {2/2/2016 5:48:58 PM}	}
// COMMENT: {2/2/2016 5:48:58 PM}}
