#include "StdAfx.h"
#include "FlowOnly.h"

#include <ostream>

#include "resource.h"
#include "WPhastDoc.h"

#include "ChemICPropertyPage.h"
#include "FlowOnlyPropertyPage.h"
#include "FlowOnlyPropertyPage2.h"
#include "Global.h"
#include "ICZoneActor.h"
#include "MacroAction.h"
#include "MediaPropertyPage.h"
#include "MediaZoneActor.h"
#include "SetChemICAction.h"
#include "SetFlowOnlyAction.h"
#include "SetMediaAction.h"


CFlowOnly::CFlowOnly(void)
: m_bFlowOnly(false)
, m_dFluidDiffusivity(1e-9)
{
}

CFlowOnly::CFlowOnly(bool bFlowOnly, double dFluidDiffusivity)
: m_bFlowOnly(bFlowOnly)
, m_dFluidDiffusivity(dFluidDiffusivity)
{
}


CFlowOnly::~CFlowOnly(void)
{
}

CFlowOnly& CFlowOnly::operator=(const bool& rhs)
{
	this->m_bFlowOnly = rhs;
	return *this;
}

void CFlowOnly::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiFlowOnly)const
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiFlowOnly)) {
		pTreeCtrl->DeleteItem(hChild);
	}

	if (this->m_bFlowOnly) {
		pTreeCtrl->SetItemText(htiFlowOnly, "FLOW_ONLY true");
	}
	else {
		///pTreeCtrl->SetItemText(htiFlowOnly, "FLOW_ONLY false");
		///pTreeCtrl->SetItemText(htiFlowOnly, "SOLUTE_TRANSPORT true");
		pTreeCtrl->SetItemText(htiFlowOnly, "Solute transport true");
		CString str;
		str.Format("-diffusivity %g", this->m_dFluidDiffusivity);
		pTreeCtrl->InsertItem(str, htiFlowOnly);
	}
	pTreeCtrl->SetItemData(htiFlowOnly, (DWORD_PTR)this);
}

void CFlowOnly::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szFlowOnly[]    = "FlowOnly";
	static const char szDiffusivity[] = "FluidDiffusivity";

	hid_t group_id = 0;
	herr_t status;

	if (bStoring)
	{
		// Create the szFlowOnly group
		group_id = ::H5Gcreate(loc_id, szFlowOnly, 0); // always created even if empty
		ASSERT(group_id > 0);
	}
	else
	{
		// Open the szFlowOnly group
		group_id = ::H5Gopen(loc_id, szFlowOnly);
		if (group_id <= 0) TRACE("WARNING unable to open group %s\n", szFlowOnly);
	}

	if (group_id > 0)
	{
		status = CGlobal::HDFSerializeBool(bStoring, group_id, szFlowOnly, this->m_bFlowOnly);
		ASSERT(status >= 0);

		status = CGlobal::HDFSerialize(bStoring, group_id, szDiffusivity, H5T_NATIVE_DOUBLE, 1, &this->m_dFluidDiffusivity);
		if (status < 0) TRACE("WARNING unable to Serialize %s\n", szDiffusivity);

		// close the szFlowOnly group
		status = ::H5Gclose(group_id);
		ASSERT(status >= 0);
	}
}

std::ostream& operator<< (std::ostream &os, const CFlowOnly &a)
{
	if (a.m_bFlowOnly) {
		os << "FLOW_ONLY true\n";
	}
	else {
		os << "FLOW_ONLY false\n";
		os << "    -diffusivity  " << a.m_dFluidDiffusivity << "\n";
	}
	return os;
}

void CFlowOnly::Edit(CTreeCtrl* pTreeCtrl)
{
	ASSERT(this);
	ASSERT(pTreeCtrl);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	CFlowOnlyPropertyPage flowOnlyPage;
	flowOnlyPage.SetFlowOnly(*this);

	CPropertySheet propSheet(_T("Flow Only Properties"), ::AfxGetMainWnd());
	propSheet.AddPage(&flowOnlyPage);

	switch (propSheet.DoModal()) {
		case IDOK: case ID_WIZFINISH:
			{
				CFlowOnly flowOnly = flowOnlyPage.GetFlowOnly();
				pDoc->Execute(new CSetFlowOnlyAction(pDoc, flowOnly));
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}

void CFlowOnly::Edit(CTreeCtrl* pTreeCtrl, CMediaZoneActor* pMediaZoneActor, CICZoneActor *pICZoneActor)
{
	ASSERT(this);
	ASSERT(pTreeCtrl);
	ASSERT(pMediaZoneActor);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	CFlowOnlyPropertyPage flowOnlyPage;
	flowOnlyPage.SetFlowOnly(*this);

	CFlowOnlyPropertyPage2 flowOnlyPage2;

	CPropertySheet propSheet(_T("Flow Only Properties"), ::AfxGetMainWnd());
	propSheet.AddPage(&flowOnlyPage);
	propSheet.AddPage(&flowOnlyPage2);

// COMMENT: {6/14/2004 2:41:45 PM}	//{{WIZ
	propSheet.SetWizardMode();
// COMMENT: {6/14/2004 2:41:45 PM}
// COMMENT: {6/14/2004 2:41:45 PM}	// grid_elt (MEDIA)
// COMMENT: {6/14/2004 2:41:45 PM}	CMediaPropertyPage mediaPage("Small", CMediaPropertyPage::IDD_INTERIOR);
// COMMENT: {6/14/2004 2:41:45 PM}	mediaPage.SetProperties(pMediaZoneActor->GetGridElt());
// COMMENT: {6/14/2004 2:41:45 PM}	mediaPage.SetFlowOnlyWizard(true);
// COMMENT: {6/14/2004 2:41:45 PM}	mediaPage.SetShowFlowOnlyMessage(true);
// COMMENT: {6/14/2004 2:41:45 PM}	propSheet.AddPage(&mediaPage);
// COMMENT: {6/14/2004 2:41:45 PM}
// COMMENT: {6/14/2004 2:41:45 PM}	// chem_ic
// COMMENT: {6/14/2004 2:41:45 PM}	CChemICPropertyPage chemICPage(CChemICPropertyPage::IDD_INTERIOR);
// COMMENT: {6/14/2004 2:41:45 PM}	chemICPage.SetProperties(pICZoneActor->GetChemIC());
// COMMENT: {6/14/2004 2:41:45 PM}	chemICPage.SetFlowOnlyWizard(true);
// COMMENT: {6/14/2004 2:41:45 PM}	chemICPage.SetShowFlowOnlyMessage(true);
// COMMENT: {6/14/2004 2:41:45 PM}	propSheet.AddPage(&chemICPage);
// COMMENT: {6/14/2004 2:41:45 PM}
// COMMENT: {6/14/2004 2:41:45 PM}	//}}WIZ

	switch (propSheet.DoModal()) {
		case IDOK: case ID_WIZFINISH:
			{
				CMacroAction *macro = new CMacroAction();

				CFlowOnly flowOnly = flowOnlyPage.GetFlowOnly();
				macro->Add(new CSetFlowOnlyAction(pDoc, flowOnly));

// COMMENT: {6/14/2004 2:42:36 PM}				CGridElt grid_elt;
// COMMENT: {6/14/2004 2:42:36 PM}				mediaPage.GetProperties(grid_elt);
// COMMENT: {6/14/2004 2:42:36 PM}				macro->Add(new CSetMediaAction(pMediaZoneActor, pTreeCtrl, grid_elt));
// COMMENT: {6/14/2004 2:42:36 PM}
// COMMENT: {6/14/2004 2:42:36 PM}				CChemIC chemIC;
// COMMENT: {6/14/2004 2:42:36 PM}				chemICPage.GetProperties(chemIC);
// COMMENT: {6/14/2004 2:42:36 PM}				macro->Add(new CSetChemICAction(pICZoneActor, pTreeCtrl, chemIC));

				pDoc->Execute(macro);
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}
