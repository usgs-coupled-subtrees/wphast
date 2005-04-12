#include "StdAfx.h"
#include "BCZoneActor.h"
#include "resource.h"

#include "property.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
#include <vtkPropAssembly.h>

#include "PropertyTreeControlBar.h"
#include "BCFluxPropertyPage.h"
#include "BCLeakyPropertyPage.h"
// COMMENT: {4/11/2005 2:14:18 PM}#include "BCSpecifiedPropertyPage.h"
#include "BCSpecifiedHeadPropertyPage.h"
#include "ETSLayout/ETSLayout.h"

#include "WPhastDoc.h"
#include "SetBCAction.h"
#include "ZoneCreateAction.h"


vtkCxxRevisionMacro(CBCZoneActor, "$Revision: 1.20 $");
vtkStandardNewMacro(CBCZoneActor);

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBCZoneActor::CBCZoneActor(void)
{
	// must always have a zone for CZoneActor::SetBounds
	if (this->m_bc.zone == 0) {
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

// COMMENT: {4/11/2005 1:35:36 PM}	// add additional stressperiods
// COMMENT: {4/11/2005 1:35:36 PM}	int n = pTreeControlBar->GetStressPeriodCount();
// COMMENT: {4/11/2005 1:35:36 PM}	for (int i = 2; i <= n; ++i) {
// COMMENT: {4/11/2005 1:35:36 PM}		this->AddStressPeriod(pTreeControlBar, i);
// COMMENT: {4/11/2005 1:35:36 PM}	}
// COMMENT: {4/11/2005 1:35:36 PM}	ASSERT(this->GetStressPeriodCount() == pTreeControlBar->GetStressPeriodCount());
}

void CBCZoneActor::InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	CString str = this->GetTreeHeading();
	this->m_hti = pTreeCtrl->InsertItem(str, hParent, hInsertAfter);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti); // might want to move this
	this->Update(pTreeCtrl, this->m_hti);
}

void CBCZoneActor::Remove(CPropertyTreeControlBar* pTreeControlBar)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
	this->m_vecHParent.clear();
	this->m_vecHInsertAfter.clear();
	std::vector<HTREEITEM>::iterator itemIter = this->m_vecHTI.begin();
	for (; itemIter != this->m_vecHTI.end(); ++itemIter) {
		HTREEITEM hParent = pTreeCtrl->GetParentItem(*itemIter);
		HTREEITEM hInsertAfter = pTreeCtrl->GetPrevSiblingItem(*itemIter);
		if (hInsertAfter == 0) {
			hInsertAfter = TVI_FIRST;
		}
		ASSERT(hParent && hInsertAfter);
		this->m_vecHParent.push_back(hParent);
		this->m_vecHInsertAfter.push_back(hInsertAfter);
		pTreeCtrl->DeleteItem(*itemIter);
	}

	// call base class
	CZoneActor::Remove(pTreeControlBar);
}

void CBCZoneActor::UnRemove(CPropertyTreeControlBar* pTreeControlBar)
{
// COMMENT: {4/11/2005 1:37:53 PM}	ASSERT(this->GetStressPeriodCount() == pTreeControlBar->GetStressPeriodCount());
// COMMENT: {4/11/2005 1:37:53 PM}
// COMMENT: {4/11/2005 1:37:53 PM}	ASSERT(this->m_vecBC.size() == this->m_vecHParent.size());
// COMMENT: {4/11/2005 1:37:53 PM}	ASSERT(this->m_vecBC.size() == this->m_vecHInsertAfter.size());
// COMMENT: {4/11/2005 1:37:53 PM}	ASSERT(this->m_vecBC.size() == this->m_vecHTI.size());
// COMMENT: {4/11/2005 1:37:53 PM}
// COMMENT: {4/11/2005 1:37:53 PM}	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
// COMMENT: {4/11/2005 1:37:53 PM}	CString str = this->GetTreeHeading();
// COMMENT: {4/11/2005 1:37:53 PM}	size_t nsize = this->m_vecBC.size();
// COMMENT: {4/11/2005 1:37:53 PM}	for (size_t i = 0; i < nsize; ++i) {
// COMMENT: {4/11/2005 1:37:53 PM}		HTREEITEM hParent = this->m_vecHParent[i];
// COMMENT: {4/11/2005 1:37:53 PM}		HTREEITEM hInsertAfter = this->m_vecHInsertAfter[i];
// COMMENT: {4/11/2005 1:37:53 PM}
// COMMENT: {4/11/2005 1:37:53 PM}		HTREEITEM hti = pTreeCtrl->InsertItem(str, hParent, hInsertAfter);
// COMMENT: {4/11/2005 1:37:53 PM}		pTreeCtrl->SetItemData(hti, (DWORD_PTR)this);
// COMMENT: {4/11/2005 1:37:53 PM}		this->Update(pTreeCtrl, hti, this->m_vecBC[i]);
// COMMENT: {4/11/2005 1:37:53 PM}
// COMMENT: {4/11/2005 1:37:53 PM}		this->m_vecHTI[i] = hti;
// COMMENT: {4/11/2005 1:37:53 PM}	}
	ASSERT(FALSE); // may need work

	// call base class
	CZoneActor::UnRemove(pTreeControlBar);
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

void CBCZoneActor::AddStressPeriod(CPropertyTreeControlBar* pTreeControlBar, int nStressPeriod)
{
	ASSERT(FALSE); // may need work
// COMMENT: {4/11/2005 1:38:40 PM}	// Note: nStressPeriod is probably unnecessary
// COMMENT: {4/11/2005 1:38:40 PM}	ASSERT(nStressPeriod >= 2);
// COMMENT: {4/11/2005 1:38:40 PM}	ASSERT(nStressPeriod == this->m_vecBC.size() + 2); // size is 0 for nStressPeriod=2
// COMMENT: {4/11/2005 1:38:40 PM}
// COMMENT: {4/11/2005 1:38:40 PM}	CBC newBC(this->m_bc);
// COMMENT: {4/11/2005 1:38:40 PM}	// remove unnecessary properties
// COMMENT: {4/11/2005 1:38:40 PM}	//newBC.ClearProperties();
// COMMENT: {4/11/2005 1:38:40 PM}	newBC.RemoveMutableProperties();
// COMMENT: {4/11/2005 1:38:40 PM}
// COMMENT: {4/11/2005 1:38:40 PM}	this->m_vecBC.push_back(newBC);
// COMMENT: {4/11/2005 1:38:40 PM}
// COMMENT: {4/11/2005 1:38:40 PM}	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
// COMMENT: {4/11/2005 1:38:40 PM}	HTREEITEM htiBC = pTreeControlBar->GetBCNode(nStressPeriod);
// COMMENT: {4/11/2005 1:38:40 PM}	ASSERT(htiBC);
// COMMENT: {4/11/2005 1:38:40 PM}
// COMMENT: {4/11/2005 1:38:40 PM}	CString str = this->GetTreeHeading();;
// COMMENT: {4/11/2005 1:38:40 PM}	HTREEITEM hti = pTreeCtrl->InsertItem(str, htiBC, TVI_LAST);
// COMMENT: {4/11/2005 1:38:40 PM}	this->m_vecHTI.push_back(hti);
// COMMENT: {4/11/2005 1:38:40 PM}	pTreeCtrl->SetItemData(hti, (DWORD_PTR)this);
// COMMENT: {4/11/2005 1:38:40 PM}	this->Update(pTreeCtrl, hti, this->m_vecBC.back());
}

//void CBCZoneActor::ReInsertStressPeriod(CPropertyTreeControlBar* pTreeControlBar, int nStressPeriod)
//{
//	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
//	HTREEITEM htiBC = pTreeControlBar->GetBCNode(nStressPeriod);
//	ASSERT(htiBC);
//
//	CString str = this->GetTreeHeading();
//
//	//{{
//	ASSERT(this->m_vecBC.size() == this->m_vecHTI.size());
//	ASSERT(this->m_vecHParent.size() == this->m_vecBC.size());
//	ASSERT(this->m_vecHParent.size() == this->m_vecHInsertAfter.size());
//
//	HTREEITEM hti = pTreeCtrl->InsertItem(str, htiBC);
//	pTreeCtrl->SetItemData(hti, (DWORD_PTR)this);
//	this->Update(pTreeCtrl, hti, this->m_vecBC.back());
//
//	this->m_vecHTI[nStressPeriod - 2] = hti;
//
//	std::vector<HTREEITEM>::iterator parentIter = this->m_vecHParent.begin();
//	std::vector<HTREEITEM>::iterator prevSibIter = this->m_vecHInsertAfter.begin();
//	for (; parentIter != this->m_vecHParent.end(); ++parentIter, ++prevSibIter) {
//
//
//		this->m_vecHParent.push_back(pTreeCtrl->GetParentItem(*itemIter));
//		this->m_vecHInsertAfter.push_back(pTreeCtrl->GetPrevSiblingItem(*itemIter));
//		ASSERT(this->m_vecHParent.back() && this->m_vecHInsertAfter.back());
//		pTreeCtrl->DeleteItem(*itemIter);
//	}
//	//}}
//
//	HTREEITEM hti = pTreeCtrl->InsertItem(str, htiBC);
//	this->m_vecHTI.push_back(hti);
//	pTreeCtrl->SetItemData(hti, (DWORD_PTR)this);
//	this->Update(pTreeCtrl, hti, this->m_vecBC.back());
//}

void CBCZoneActor::RemoveStressPeriod(CPropertyTreeControlBar* pTreeControlBar, int nStressPeriod)
{
	ASSERT(nStressPeriod >= 2);
	ASSERT(nStressPeriod <= this->GetStressPeriodCount());

	HTREEITEM htiDelete = this->m_vecHTI[nStressPeriod - 2];
	pTreeControlBar->GetTreeCtrl()->DeleteItem(htiDelete);

	// update vectors
	std::vector<CBC>::iterator       iterBC = this->m_vecBC.begin();
	std::vector<HTREEITEM>::iterator iterHT = this->m_vecHTI.begin();
	for (int nSP = 2; nSP < nStressPeriod; ++iterBC, ++iterHT, ++nSP);
	this->m_vecBC.erase(iterBC);
	this->m_vecHTI.erase(iterHT);

	ASSERT(this->m_vecBC.size() == this->m_vecHTI.size());
}


void CBCZoneActor::UpdateTree(CPropertyTreeControlBar* pTreeControlBar, int nStressPeriod)
{
	this->Update(pTreeControlBar->GetTreeCtrl(), nStressPeriod);
}

void CBCZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	ASSERT(htiParent == this->GetHTreeItem(1));
	this->Update(pTreeCtrl, 1);
}

void CBCZoneActor::Update(CTreeCtrl* pTreeCtrl, int nStressPeriod)
{
	this->Update(pTreeCtrl, this->GetHTreeItem(nStressPeriod), this->GetBC(nStressPeriod));
}

void CBCZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, const CBC& crBC)
{
	CString str;

	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiParent)) {
		pTreeCtrl->DeleteItem(hChild);
	}

	switch (crBC.bc_type) {
		case UNDEFINED:
			ASSERT(FALSE);
			break;

		case SPECIFIED:
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
// COMMENT: {2/23/2005 1:14:03 PM}			// head
// COMMENT: {2/23/2005 1:14:03 PM}			if (crBC.bc_head && crBC.bc_head->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:14:03 PM}				static_cast<Cproperty*>(crBC.bc_head)->Insert(pTreeCtrl, htiParent, "head");
// COMMENT: {2/23/2005 1:14:03 PM}			}
			// head
			if (crBC.m_bc_head.size())
			{
				htiParent = pTreeCtrl->InsertItem("head", htiParent);
				CTimeSeries<Cproperty>::const_iterator iter = crBC.m_bc_head.begin();
				for (; iter != crBC.m_bc_head.end(); ++iter)
				{
					ASSERT((*iter).second.type != UNDEFINED);
					if ((*iter).second.type == UNDEFINED) continue;

					CString str;
					if ((*iter).first.input)
					{
						str.Format("%g %s", (*iter).first.value, (*iter).first.input);
					}
					else
					{
						str.Format("%g", (*iter).first.value);
					}
					iter->second.Insert(pTreeCtrl, htiParent, str);
				}
			}
// COMMENT: {2/23/2005 1:14:03 PM}			// associated_solution
// COMMENT: {2/23/2005 1:14:03 PM}			if (crBC.bc_solution && crBC.bc_solution->type != UNDEFINED && crBC.bc_solution_type == ASSOCIATED) {
// COMMENT: {2/23/2005 1:14:03 PM}				static_cast<Cproperty*>(crBC.bc_solution)->Insert(pTreeCtrl, htiParent, "associated_solution");
// COMMENT: {2/23/2005 1:14:03 PM}			}
// COMMENT: {2/23/2005 1:14:03 PM}			// fixed_solution
// COMMENT: {2/23/2005 1:14:03 PM}			if (crBC.bc_solution && crBC.bc_solution->type != UNDEFINED && crBC.bc_solution_type == FIXED) {
// COMMENT: {2/23/2005 1:14:03 PM}				static_cast<Cproperty*>(crBC.bc_solution)->Insert(pTreeCtrl, htiParent, "fixed_solution");
// COMMENT: {2/23/2005 1:14:03 PM}			}
			break;

		case FLUX:
			if (crBC.face_defined) {
				switch (crBC.face) {
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
			// associated_solution
// COMMENT: {2/23/2005 1:14:13 PM}			if (crBC.bc_solution && crBC.bc_solution->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:14:13 PM}				static_cast<Cproperty*>(crBC.bc_solution)->Insert(pTreeCtrl, htiParent, "associated_solution");
// COMMENT: {2/23/2005 1:14:13 PM}			}
// COMMENT: {2/23/2005 1:14:13 PM}			// flux
// COMMENT: {2/23/2005 1:14:13 PM}			if (crBC.bc_flux && crBC.bc_flux->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:14:13 PM}				static_cast<Cproperty*>(crBC.bc_flux)->Insert(pTreeCtrl, htiParent, "flux");
// COMMENT: {2/23/2005 1:14:13 PM}			}
			break;

		case LEAKY:
			if (crBC.face_defined) {
				switch (crBC.face) {
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
// COMMENT: {2/23/2005 1:14:26 PM}			// associated_solution
// COMMENT: {2/23/2005 1:14:26 PM}			if (crBC.bc_solution && crBC.bc_solution->type != UNDEFINED && crBC.bc_solution_type == ASSOCIATED) {
// COMMENT: {2/23/2005 1:14:26 PM}				static_cast<Cproperty*>(crBC.bc_solution)->Insert(pTreeCtrl, htiParent, "associated_solution");
// COMMENT: {2/23/2005 1:14:26 PM}			}
// COMMENT: {2/23/2005 1:14:26 PM}			// head
// COMMENT: {2/23/2005 1:14:26 PM}			if (crBC.bc_head && crBC.bc_head->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:14:26 PM}				static_cast<Cproperty*>(crBC.bc_head)->Insert(pTreeCtrl, htiParent, "head");
// COMMENT: {2/23/2005 1:14:26 PM}			}
// COMMENT: {2/23/2005 1:14:26 PM}			// hydraulic_conductivity
// COMMENT: {2/23/2005 1:14:26 PM}			if (crBC.bc_k && crBC.bc_k->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:14:26 PM}				static_cast<Cproperty*>(crBC.bc_k)->Insert(pTreeCtrl, htiParent, "hydraulic_conductivity");
// COMMENT: {2/23/2005 1:14:26 PM}			}
// COMMENT: {2/23/2005 1:14:26 PM}			// thickness
// COMMENT: {2/23/2005 1:14:26 PM}			if (crBC.bc_thick && crBC.bc_thick->type != UNDEFINED) {
// COMMENT: {2/23/2005 1:14:26 PM}				static_cast<Cproperty*>(crBC.bc_thick)->Insert(pTreeCtrl, htiParent, "thickness");
// COMMENT: {2/23/2005 1:14:26 PM}			}
			break;

		default:
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
// COMMENT: {4/11/2005 2:16:04 PM}	CPropertySheet props(str);
	ETSLayoutPropertySheet props(str);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	if (this->m_bc.bc_type == FLUX) {
		CBCFluxPropertyPage fluxProps;
		props.AddPage(&fluxProps);
		// TODO should SetStressPeriod and SetProperties be combined ?
		fluxProps.SetStressPeriod(nStressPeriod);		
		fluxProps.SetProperties(this->GetBC(nStressPeriod));		
		if (props.DoModal() == IDOK) {
			CBC bc;
			fluxProps.GetProperties(bc);
			pDoc->Execute(new CSetBCAction(this, pTreeCtrl, bc, nStressPeriod));
		}
	}
	else if (this->m_bc.bc_type == LEAKY) {
		CBCLeakyPropertyPage leakyProps;
		props.AddPage(&leakyProps);
		// TODO should SetStressPeriod and SetProperties be combined ?
		leakyProps.SetStressPeriod(nStressPeriod);		
		leakyProps.SetProperties(this->GetBC(nStressPeriod));		
		if (props.DoModal() == IDOK) {
			CBC bc;
			leakyProps.GetProperties(bc);
			pDoc->Execute(new CSetBCAction(this, pTreeCtrl, bc, nStressPeriod));
		}
	}
	else if (this->m_bc.bc_type == SPECIFIED)
	{
		// CBCSpecifiedPropertyPage specified;
		CBCSpecifiedHeadPropertyPage specified;
		props.AddPage(&specified);
		// TODO should SetStressPeriod and SetProperties be combined ?
// COMMENT: {4/11/2005 2:14:40 PM}		specified.SetStressPeriod(nStressPeriod);		
		specified.SetProperties(this->GetBC(nStressPeriod));		
		if (props.DoModal() == IDOK)
		{
// COMMENT: {4/11/2005 2:13:45 PM}			CBCZone bc;
			CBC bc;
			specified.GetProperties(bc);
// COMMENT: {2/25/2005 7:28:41 PM}			pDoc->Execute(new CSetBCAction(this, pTreeCtrl, bc, nStressPeriod));
			pDoc->Execute(new CSetBCAction(this, pTreeCtrl, bc, 1));
		}		
	}
	else 
	{
		ASSERT(FALSE);
	}
}

int CBCZoneActor::GetStressPeriodCount(void)const
{
	ASSERT(this->m_vecBC.size() == this->m_vecHTI.size());
	return (int) this->m_vecBC.size() + 1;
}

CBC CBCZoneActor::GetBC(int nStressPeriod)const
{
	ASSERT(0 < nStressPeriod);                              // 1-based
	ASSERT(nStressPeriod <= this->GetStressPeriodCount());  // must have called AddStressPeriod first

	if (nStressPeriod == 1) {
		return this->m_bc;
	}
	else {
		// index is nStressPeriod-2
		return this->m_vecBC[nStressPeriod - 2];
	}
}

void CBCZoneActor::SetBC(const CBC& rBC, int nStressPeriod)
{
	ASSERT(0 < nStressPeriod);                              // 1-based
	ASSERT(nStressPeriod <= this->GetStressPeriodCount());  // must have called AddStressPeriod first

	if (nStressPeriod == 1) {
		ASSERT(this->m_bc.zone);
		CZone zoneSave(*this->m_bc.zone);

		this->m_bc = rBC;
		if (this->m_bc.zone == 0) {
			this->m_bc.zone = new CZone();
		}
		(*this->m_bc.zone) = zoneSave;
		this->m_pZone = this->m_bc.zone;
	}
	else {
		// index is nStressPeriod-2
		/// this->m_vecBC[nStressPeriod - 2].Merge(rBC);
#ifdef _DEBUG
		ASSERT(this->m_bc.bc_solution_type == rBC.bc_solution_type);
#endif
		this->m_vecBC[nStressPeriod - 2] = rBC;
		ASSERT(this->m_vecBC[nStressPeriod - 2].zone);
	}
}

HTREEITEM CBCZoneActor::GetHTreeItem(int nStressPeriod)const
{
	ASSERT(0 < nStressPeriod);                              // 1-based
	ASSERT(nStressPeriod <= this->GetStressPeriodCount());  // must have called AddStressPeriod first

	if (nStressPeriod == 1) {
		return this->m_hti;
	}
	else {
		// index is nStressPeriod-2
		return this->m_vecHTI[nStressPeriod - 2];
	}
}

void CBCZoneActor::SetHTreeItem(HTREEITEM htItem, int nStressPeriod)
{
	ASSERT(0 < nStressPeriod);                              // 1-based
	ASSERT(nStressPeriod <= this->GetStressPeriodCount());  // must have called AddStressPeriod first

	if (nStressPeriod == 1) {
		this->m_hti = htItem;
	}
	else {
		// index is nStressPeriod-2
		this->m_vecHTI[nStressPeriod - 2] = htItem;
	}
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

void CBCZoneActor::SerializeStressPeriod(int nStressPeriod, bool bStoring, hid_t loc_id)
{
	ASSERT(1 < nStressPeriod);                              // use CBCZoneActor::Serialize instead
	ASSERT(nStressPeriod <= this->GetStressPeriodCount());  // must have called AddStressPeriod first

	if (bStoring)
	{
		ASSERT((nStressPeriod - 2) >= 0); // size == nStressPeriod-2
		ASSERT((nStressPeriod - 2) < (int)this->m_vecBC.size());

		// this->m_vecBC[nStressPeriod - 2].Serialize(bStoring, loc_id);
		this->GetBC(nStressPeriod).Serialize(bStoring, loc_id);
	}
	else {
		////ASSERT((nStressPeriod - 2) == (int)this->m_vecBC.size());
		////CBC bc;
		////this->m_vecBC.push_back(bc);
#ifdef PROBABLY_UNNECESSARY
		ASSERT((nStressPeriod - 2) >= 0); // size == nStressPeriod-2
		ASSERT((nStressPeriod - 1) == (int)this->m_vecBC.size()); // AddStressPeriod must be called before SerializeStressPeriod
#endif
		/// ::DebugBreak(); // check above

		// this->m_vecBC[nStressPeriod - 2].Serialize(bStoring, loc_id);
		CBC bc;
		bc.Serialize(bStoring, loc_id);
#ifdef _DEBUG
		bc.AssertValid(nStressPeriod);
#endif
		this->SetBC(bc, nStressPeriod);
	}
}

void CBCZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyBC()) {
		pPropAssembly->AddPart(this);
		if (!pWPhastDoc->GetPropCollection()->IsItemPresent(pPropAssembly)) {
			pWPhastDoc->GetPropCollection()->AddItem(pPropAssembly);
		}
	}
#ifdef _DEBUG
	else ASSERT(FALSE);
#endif
}

void CBCZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyBC()) {
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

