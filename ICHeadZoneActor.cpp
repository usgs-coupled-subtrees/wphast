#include "StdAfx.h"
#include "resource.h"
#include "ICHeadZoneActor.h"
#include "PropertyTreeControlBar.h"
#include "ICHeadPropertyPage.h"
#include "property.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro

vtkCxxRevisionMacro(CICHeadZoneActor, "$Revision: 1.3 $");
vtkStandardNewMacro(CICHeadZoneActor);

CICHeadZoneActor::CICHeadZoneActor(void)
{
	this->m_headIC.zone = new CZone();
	this->m_pZone = this->m_headIC.zone;
}

CICHeadZoneActor::~CICHeadZoneActor(void)
{
}

void CICHeadZoneActor::Serialize(bool bStoring, hid_t loc_id, const CUnits& units)
{
	CZoneActor::Serialize(bStoring, loc_id);

	if (bStoring)
	{
		// store head_ic
		this->m_headIC.Serialize(bStoring, loc_id);
	}
	else
	{
		// load head_ic
		this->m_headIC.Serialize(bStoring, loc_id);
		this->SetUnits(units);
	}
}

void CICHeadZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
	HTREEITEM htiIC = pTreeControlBar->GetICNode();

	CString str;
	str.Format("HEAD_IC %s", this->GetName());
	this->m_hti = pTreeCtrl->InsertItem(str, htiIC);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti);
	this->Update(pTreeCtrl, this->m_hti);
}

void CICHeadZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	CString format;

	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiParent)) {
		pTreeCtrl->DeleteItem(hChild);
	}

	// head
	if (this->m_headIC.head) {
		static_cast<Cproperty*>(this->m_headIC.head)->Insert(pTreeCtrl, htiParent, "head");
	}
}

void CICHeadZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	CString str;
	str.Format(_T("%s Properties"), this->GetName());
	CPropertySheet props(str);
	CICHeadPropertyPage icHeadProps;
	props.AddPage(&icHeadProps);
	icHeadProps.SetProperties(this->m_headIC);		
	switch (props.DoModal()) {
		case IDOK:
			icHeadProps.GetProperties(this->m_headIC);
			this->Update(pTreeCtrl, this->m_hti);
			break;
		case IDCANCEL:
			break;
	}
}

CHeadIC CICHeadZoneActor::GetHeadIC(void)const
{
	return this->m_headIC;
}

void CICHeadZoneActor::SetHeadIC(const CHeadIC& rHeadIC)
{
	ASSERT(this->m_headIC.zone);
	CZone zoneSave(*this->m_headIC.zone);

	this->m_headIC = rHeadIC;
	if (this->m_headIC.zone == 0) {
		this->m_headIC.zone = new CZone();
	}
	(*this->m_headIC.zone) = zoneSave;
	this->m_pZone = this->m_headIC.zone;
}
