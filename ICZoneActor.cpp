#include "StdAfx.h"
#include "resource.h"
#include "ICZoneActor.h"
#include "PropertyTreeControlBar.h"
#include "ICHeadPropertyPage.h"
#include "ChemICPropertyPage.h"

#include "property.h"
#include "Global.h"

#include "WPhastDoc.h"
#include "SetHeadICAction.h"
#include "SetChemICAction.h"
#include "ZoneCreateAction.h"
#include "FlowOnly.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
#include <vtkPropAssembly.h>


vtkCxxRevisionMacro(CICZoneActor, "$Revision: 1.11 $");
vtkStandardNewMacro(CICZoneActor);

CICZoneActor::CICZoneActor(void)
{
	this->m_type  = CICZoneActor::IC_UNDEF;
	this->m_pZone = new CZone();
}

CICZoneActor::~CICZoneActor(void)
{
	delete this->m_pZone;
}

void CICZoneActor::Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CHeadIC& headIC)
{
	CZoneCreateAction<CICZoneActor>* pAction = new CZoneCreateAction<CICZoneActor>(
		pWPhastDoc,
		pWPhastDoc->GetNextZoneName(),
		zone.x1,
		zone.x2,
		zone.y1,
		zone.y2,
		zone.z1,
		zone.z2
		);
	pAction->GetZoneActor()->SetHeadIC(headIC);
	pWPhastDoc->Execute(pAction);
}

void CICZoneActor::Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CChemIC& chemIC)
{
	CZoneCreateAction<CICZoneActor>* pAction = new CZoneCreateAction<CICZoneActor>(
		pWPhastDoc,
		pWPhastDoc->GetNextZoneName(),
		zone.x1,
		zone.x2,
		zone.y1,
		zone.y2,
		zone.z1,
		zone.z2
		);
	pAction->GetZoneActor()->SetChemIC(chemIC);
	pWPhastDoc->Execute(pAction);
}

LPCTSTR CICZoneActor::GetSerialName(void)//const
{
	this->m_serialName = this->m_name;
	switch (this->m_type) {
		case CICZoneActor::IC_CHEM:
			this->m_serialName.append(":IC_CHEM");
			break;
		case CICZoneActor::IC_HEAD:
			this->m_serialName += ":IC_HEAD";
			break;
		default: case CICZoneActor::IC_UNDEF:
			ASSERT(FALSE);
			this->m_serialName += ":IC_UNDEF";
			break;
	}
	return this->m_serialName.c_str();
}

void CICZoneActor::SetSerialName(LPCTSTR name)
{
	ASSERT(this->m_name.empty()); // this may need to change to a warning

	this->m_name = name;
	std::string::size_type pos = this->m_name.find_first_of(":");
	if (pos != std::string::npos) {
		this->m_name.resize(pos);
	}
}

void CICZoneActor::Serialize(bool bStoring, hid_t loc_id, const CUnits& units)
{
	//{{
	static const char szICType[] = "IC_TYPE";
	herr_t status;

	ASSERT(this->GetName()); // must have name

	CZoneActor::Serialize(bStoring, loc_id);

	// serialize IC_TYPE
	status = CGlobal::HDFSerialize(bStoring, loc_id, szICType, H5T_NATIVE_INT, 1, &this->m_type);
	ASSERT(status >= 0);

	if (bStoring)
	{
		if (this->GetType() == CICZoneActor::IC_HEAD) {
			// store head_ic
			ASSERT(this->m_headIC.zone == 0);
			ASSERT(this->m_pZone != 0);
			this->m_headIC.zone = this->m_pZone;
			this->m_headIC.Serialize(bStoring, loc_id);
			this->m_headIC.zone = 0;
		}
		if (this->GetType() == CICZoneActor::IC_CHEM) {
			// store chem_ic
			ASSERT(this->m_chemIC.zone == 0);
			ASSERT(this->m_pZone != 0);
			this->m_chemIC.zone = this->m_pZone;
			this->m_chemIC.Serialize(bStoring, loc_id);
			this->m_chemIC.zone = 0;
		}
	}
	else
	{
		if (this->GetType() == CICZoneActor::IC_HEAD) {
			// load head_ic
			ASSERT(this->m_headIC.zone == 0);
			ASSERT(this->m_pZone != 0);
			this->m_headIC.zone = this->m_pZone;
			this->m_headIC.Serialize(bStoring, loc_id);
			this->m_pZone = this->m_headIC.zone;
			this->m_headIC.zone = 0;
		}
		if (this->GetType() == CICZoneActor::IC_CHEM) {
			// load chem_ic
			ASSERT(this->m_chemIC.zone == 0);
			ASSERT(this->m_pZone != 0);
			this->m_chemIC.zone = this->m_pZone;
			this->m_chemIC.Serialize(bStoring, loc_id);
			this->m_pZone = this->m_chemIC.zone;
			this->m_chemIC.zone = 0;
		}
		this->SetUnits(units);
	}
	//}}

	//CZoneActor::Serialize(bStoring, loc_id);

	//if (bStoring)
	//{
	//	// store head_ic
	//	this->m_headIC.Serialize(bStoring, loc_id);
	//}
	//else
	//{
	//	// load head_ic
	//	this->m_headIC.Serialize(bStoring, loc_id);
	//	this->SetUnits(units);
	//}
}

void CICZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
	HTREEITEM htiIC = pTreeControlBar->GetICNode();
	this->InsertAt(pTreeCtrl, htiIC, TVI_LAST);
	//{{HACK
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
	//}}HACK
}

void CICZoneActor::InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	CString str;
	switch (this->GetType()) {
		case CICZoneActor::IC_HEAD:
			str.Format("HEAD_IC %s", this->GetName());
			break;
		case CICZoneActor::IC_CHEM:
			str.Format("CHEMISTRY_IC %s", this->GetName());
			break;
		default:
			ASSERT(FALSE);
			str.Format("UNKNOWN_IC %s", this->GetName());
			break;
	}
	this->m_hti = pTreeCtrl->InsertItem(str, hParent, hInsertAfter);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti); // might want to move this
	this->Update(pTreeCtrl, this->m_hti);
}

void CICZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiParent)) {
		pTreeCtrl->DeleteItem(hChild);
	}

	if (this->GetType() == CICZoneActor::IC_HEAD) {
		// head
		if (this->m_headIC.head) {
			static_cast<Cproperty*>(this->m_headIC.head)->Insert(pTreeCtrl, htiParent, "head");
		}
	}
	else if (this->GetType() == CICZoneActor::IC_CHEM) {
		// solution
		if (this->m_chemIC.solution) {
			static_cast<Cproperty*>(this->m_chemIC.solution)->Insert(pTreeCtrl, htiParent, "solution");
		}

		// equilibrium_phases
		if (this->m_chemIC.equilibrium_phases) {
			static_cast<Cproperty*>(this->m_chemIC.equilibrium_phases)->Insert(pTreeCtrl, htiParent, "equilibrium_phases");
		}

		// exchange
		if (this->m_chemIC.exchange) {
			static_cast<Cproperty*>(this->m_chemIC.exchange)->Insert(pTreeCtrl, htiParent, "exchange");
		}

		// surface
		if (this->m_chemIC.surface) {
			static_cast<Cproperty*>(this->m_chemIC.surface)->Insert(pTreeCtrl, htiParent, "surface");
		}

		// gas_phase
		if (this->m_chemIC.gas_phase) {
			static_cast<Cproperty*>(this->m_chemIC.gas_phase)->Insert(pTreeCtrl, htiParent, "gas_phase");
		}

		// solid_solutions
		if (this->m_chemIC.solid_solutions) {
			static_cast<Cproperty*>(this->m_chemIC.solid_solutions)->Insert(pTreeCtrl, htiParent, "solid_solutions");
		}

		// kinetics
		if (this->m_chemIC.kinetics) {
			static_cast<Cproperty*>(this->m_chemIC.kinetics)->Insert(pTreeCtrl, htiParent, "kinetics");
		}
	}
	else {
		ASSERT(FALSE);
	}
}

void CICZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	CString str;
	str.Format(_T("%s Properties"), this->GetName());
	CPropertySheet props(str);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);


	if (this->GetType() == CICZoneActor::IC_HEAD)
	{
		CICHeadPropertyPage icHeadProps;
		icHeadProps.SetProperties(this->m_headIC);		

		props.AddPage(&icHeadProps);
		if (props.DoModal() == IDOK) {
			CHeadIC headIC;
			icHeadProps.GetProperties(headIC);
			pDoc->Execute(new CSetHeadICAction(this, pTreeCtrl, headIC));
		}
	}
	else if (this->GetType() == CICZoneActor::IC_CHEM)
	{
		CChemICPropertyPage chemICProps;
		chemICProps.SetProperties(this->m_chemIC);
		if (this->GetDefault()) {
			chemICProps.SetFlowOnly(bool(pDoc->GetFlowOnly()));
		}

		props.AddPage(&chemICProps);
		if (props.DoModal() == IDOK) {
			CChemIC chemIC;
			chemICProps.GetProperties(chemIC);
			pDoc->Execute(new CSetChemICAction(this, pTreeCtrl, chemIC));
		}
	}
	else
	{
		ASSERT(FALSE);
	}
}

CICZoneActor::IC_TYPE CICZoneActor::GetType(void)const
{
	return this->m_type;
}

CHeadIC CICZoneActor::GetHeadIC(void)const
{
	ASSERT(this->GetType() == CICZoneActor::IC_HEAD);

	CHeadIC headIC(this->m_headIC);

	ASSERT(this->m_pZone != 0);
	ASSERT(this->m_headIC.zone == 0);
	ASSERT(this->m_chemIC.zone == 0);
	headIC.zone = new CZone(*this->m_pZone);

	return headIC;
}

void CICZoneActor::SetHeadIC(const CHeadIC& rHeadIC)
{
	ASSERT(this->GetType() == CICZoneActor::IC_UNDEF || this->GetType() == CICZoneActor::IC_HEAD);

	this->m_type = CICZoneActor::IC_HEAD;
	ASSERT(this->m_headIC.zone == 0);
	ASSERT(this->m_chemIC.zone == 0);

	this->m_headIC = rHeadIC;
	if (this->m_headIC.zone != 0) {
		delete this->m_headIC.zone;
		this->m_headIC.zone = 0;
	}
}

CChemIC CICZoneActor::GetChemIC(void)const
{
	ASSERT(this->GetType() == CICZoneActor::IC_CHEM);

	CChemIC chemIC(this->m_chemIC);

	ASSERT(this->m_pZone != 0);
	ASSERT(this->m_headIC.zone == 0);
	ASSERT(this->m_chemIC.zone == 0);
	chemIC.zone = new CZone(*this->m_pZone);

	return chemIC;
}

void CICZoneActor::SetChemIC(const CChemIC& chemIC)
{
	ASSERT(this->GetType() == CICZoneActor::IC_UNDEF || this->GetType() == CICZoneActor::IC_CHEM);

	this->m_type = CICZoneActor::IC_CHEM;
	ASSERT(this->m_headIC.zone == 0);
	ASSERT(this->m_chemIC.zone == 0);

	this->m_chemIC = chemIC;
	if (this->m_chemIC.zone != 0) {
		delete this->m_chemIC.zone;
		this->m_chemIC.zone = 0;
	}
}

HTREEITEM CICZoneActor::GetHTreeItem(void)const
{
	return this->m_hti;
}

void CICZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyIC()) {
		pPropAssembly->AddPart(this);
		if (!pWPhastDoc->GetPropCollection()->IsItemPresent(pPropAssembly)) {
			pWPhastDoc->GetPropCollection()->AddItem(pPropAssembly);
		}
	}
#ifdef _DEBUG
	else ASSERT(FALSE);
#endif
}

void CICZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyIC()) {
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

