#include "StdAfx.h"
#include "SteadyFlow.h"

#include <ostream>
#include "resource.h"
#include "WPhastDoc.h"
#include "Global.h"
#include "SteadyFlowPropertyPage.h"
#include "SetSteadyFlowAction.h"

CSteadyFlow::CSteadyFlow(void)
: m_bSteadyFlow(false)
{
	
}

CSteadyFlow::CSteadyFlow(bool bSteadyFlow)
: m_bSteadyFlow(bSteadyFlow)
{
}

CSteadyFlow::~CSteadyFlow(void)
{
}

CSteadyFlow& CSteadyFlow::operator=(const bool& rhs)
{
	this->m_bSteadyFlow = rhs;
	return *this;
}

void CSteadyFlow::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiSteadyFlow)const
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiSteadyFlow)) {
		pTreeCtrl->DeleteItem(hChild);
	}

	if (this->m_bSteadyFlow) {
		pTreeCtrl->SetItemText(htiSteadyFlow, "STEADY_FLOW true");
	}
	else {
		///pTreeCtrl->SetItemText(htiSteadyFlow, "TRANSIENT_FLOW true");
		pTreeCtrl->SetItemText(htiSteadyFlow, "Transient flow true");
	}
	pTreeCtrl->SetItemData(htiSteadyFlow, (DWORD_PTR)this);
}

void CSteadyFlow::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szSteadyFlow[] = "SteadyFlow";

	hid_t group_id = 0;
	herr_t status;

	if (bStoring)
	{
		// Create the szSteadyFlow group
		group_id = ::H5Gcreate(loc_id, szSteadyFlow, 0); // always created even if empty
		ASSERT(group_id > 0);
	}
	else
	{
		// Open the szSteadyFlow group
		group_id = ::H5Gopen(loc_id, szSteadyFlow);
		if (group_id <= 0) TRACE("WARNING unable to open group %s\n", szSteadyFlow);
	}

	if (group_id > 0) {
		status = CGlobal::HDFSerializeBool(bStoring, group_id, szSteadyFlow, this->m_bSteadyFlow);
		ASSERT(status >= 0);

		// close the szFlowOnly group
		status = ::H5Gclose(group_id);
		ASSERT(status >= 0);
	}
}

void CSteadyFlow::Edit(CTreeCtrl* pTreeCtrl)
{
	ASSERT(this);
	ASSERT(pTreeCtrl);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	CSteadyFlowPropertyPage steadyFlowPage;
	steadyFlowPage.SetSteadyFlow(*this);

	CPropertySheet propSheet(_T("Steady Flow Properties"), ::AfxGetMainWnd());
	propSheet.AddPage(&steadyFlowPage);

	switch (propSheet.DoModal()) {
		case IDOK: case ID_WIZFINISH:
			{
				CSteadyFlow steadyFlow = steadyFlowPage.GetSteadyFlow();
				pDoc->Execute(new CSetSteadyFlowAction(pDoc, steadyFlow));
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}

std::ostream& operator<< (std::ostream &os, const CSteadyFlow &a)
{
	if (a.m_bSteadyFlow) {
		os << "STEADY_FLOW true\n";
	}
	else {
		os << "STEADY_FLOW false\n";
	}
	return os;
}
