#include "StdAfx.h"
#include "ICHeadZoneActor.h"

#include "resource.h"
#include "PropertyTreeControlBar.h"
#include "ICHeadSpreadPropertyPage.h"

#include "property.h"
#include "Global.h"

#include "WPhastDoc.h"
#include "SetHeadICAction.h"
#include "ZoneCreateAction.h"
#include "FlowOnly.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
#include <vtkPropAssembly.h>

vtkCxxRevisionMacro(CICHeadZoneActor, "$Revision$");
vtkStandardNewMacro(CICHeadZoneActor);

const char CICHeadZoneActor::szHeading[] = "ICHead";

CICHeadZoneActor::CICHeadZoneActor(void)
{
}

CICHeadZoneActor::~CICHeadZoneActor(void)
{
}

void CICHeadZoneActor::Create(CWPhastDoc* pWPhastDoc, const CZone& zone, const CHeadIC& headIC)
{
	CZoneCreateAction<CICHeadZoneActor>* pAction = new CZoneCreateAction<CICHeadZoneActor>(
		pWPhastDoc,
		pWPhastDoc->GetNextZoneName(),
		zone.x1,
		zone.x2,
		zone.y1,
		zone.y2,
		zone.z1,
		zone.z2
		);
	pAction->GetZoneActor()->SetData(headIC);
	pWPhastDoc->Execute(pAction);
}

CHeadIC CICHeadZoneActor::GetData(void)const
{
	CHeadIC headIC(this->m_headIC);

	ASSERT(this->m_pZone != 0);
	ASSERT(this->m_headIC.zone == 0);
	headIC.zone = new CZone(*this->m_pZone);

	return headIC;
}

void CICHeadZoneActor::SetData(const CHeadIC& rHeadIC)
{
	ASSERT(this->m_headIC.zone == 0);

	this->m_headIC = rHeadIC;
	if (this->m_headIC.zone != 0)
	{
		delete this->m_headIC.zone;
		this->m_headIC.zone = 0;
	}
}

void CICHeadZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();

	HTREEITEM htiIC = pTreeControlBar->GetICHeadNode();;

	this->InsertAt(pTreeCtrl, htiIC, hInsertAfter);
	//{{HACK
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
	//}}HACK
}

void CICHeadZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiParent))
	{
		pTreeCtrl->DeleteItem(hChild);
	}

	// head
	if (this->m_headIC.head)
	{
		static_cast<Cproperty*>(this->m_headIC.head)->Insert(pTreeCtrl, htiParent, "head");
	}
}

void CICHeadZoneActor::Serialize(bool bStoring, hid_t loc_id, const CUnits& units)
{
	CZoneActor::Serialize(bStoring, loc_id);

	if (bStoring)
	{
		// store head_ic
		ASSERT(this->m_headIC.zone == 0);
		ASSERT(this->m_pZone != 0);
		this->m_headIC.zone = this->m_pZone;
		this->m_headIC.Serialize(bStoring, loc_id);
		this->m_headIC.zone = 0;
	}
	else
	{
		// load head_ic
		ASSERT(this->m_headIC.zone == 0);
		ASSERT(this->m_pZone != 0);
		this->m_headIC.zone = this->m_pZone;
		this->m_headIC.Serialize(bStoring, loc_id);
		this->m_pZone = this->m_headIC.zone;
		this->m_headIC.zone = 0;

		this->SetUnits(units);
	}
}

void CICHeadZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	CString str;
	str.Format(_T("%s Properties"), this->GetName());
	CPropertySheet props(str);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	CICHeadSpreadPropertyPage icHeadProps;
	icHeadProps.SetProperties(this->GetData());		

	props.AddPage(&icHeadProps);
	if (props.DoModal() == IDOK)
	{
		CHeadIC headIC;
		icHeadProps.GetProperties(headIC);
		pDoc->Execute(new CSetHeadICAction(this, pTreeCtrl, headIC));
	}
}

void CICHeadZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyICHead())
	{
		pPropAssembly->AddPart(this);
		if (!pWPhastDoc->GetPropCollection()->IsItemPresent(pPropAssembly))
		{
			////pWPhastDoc->GetPropCollection()->AddItem(pPropAssembly);
		}
	}
#ifdef _DEBUG
	else ASSERT(FALSE);
#endif
}

void CICHeadZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyICHead())
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
