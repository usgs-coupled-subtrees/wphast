#include "StdAfx.h"
#include "FreeSurface.h"

#include <ostream>
#include "Global.h"

#include "WPhastDoc.h"
#include "resource.h"
#include "FreeSurfacePropertyPage.h"
#include "SetFreeSurfaceAction.h"

CFreeSurface::CFreeSurface(void)
: m_bFreeSurface(false)
{
}

CFreeSurface::CFreeSurface(bool bFreeSurface)
: m_bFreeSurface(bFreeSurface)
{
}

CFreeSurface::~CFreeSurface(void)
{
}

CFreeSurface& CFreeSurface::operator=(const bool& rhs)
{
	this->m_bFreeSurface = rhs;
	return *this;
}

void CFreeSurface::Insert(CTreeCtrl* pTreeCtrl, HTREEITEM htiFreeSurface)const
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiFreeSurface)) {
		pTreeCtrl->DeleteItem(hChild);
	}

	if (this->m_bFreeSurface) {
		pTreeCtrl->SetItemText(htiFreeSurface, "FREE_SURFACE true");
	}
	else {
		///pTreeCtrl->SetItemText(htiFreeSurface, "CONFINED_FLOW true");
		pTreeCtrl->SetItemText(htiFreeSurface, "Confined flow true");
	}
	pTreeCtrl->SetItemData(htiFreeSurface, (DWORD_PTR)this);
}

void CFreeSurface::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szFreeSurface[]    = "FreeSurface";

	hid_t group_id = 0;
	herr_t status;

	if (bStoring)
	{
		// Create the szFlowOnly group
		group_id = ::H5Gcreate(loc_id, szFreeSurface, 0); // always created even if empty
		ASSERT(group_id > 0);
	}
	else
	{
		// Open the szFreeSurface group
		group_id = ::H5Gopen(loc_id, szFreeSurface);
		if (group_id <= 0) TRACE("WARNING unable to open group %s\n", szFreeSurface);
	}

	if (group_id > 0) {
		status = CGlobal::HDFSerializeBool(bStoring, group_id, szFreeSurface, this->m_bFreeSurface);
		ASSERT(status >= 0);

		// close the szFreeSurface group
		status = ::H5Gclose(group_id);
		ASSERT(status >= 0);
	}
}

void CFreeSurface::Edit(CTreeCtrl* pTreeCtrl)
{
	ASSERT(this);
	ASSERT(pTreeCtrl);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	CFreeSurfacePropertyPage freeSurfacePage;
	freeSurfacePage.SetFreeSurface(*this);

	CPropertySheet propSheet(_T("Free Surface Properties"), ::AfxGetMainWnd());
	propSheet.AddPage(&freeSurfacePage);

	switch (propSheet.DoModal()) {
		case IDOK: case ID_WIZFINISH:
			{
				CFreeSurface freeSurface = freeSurfacePage.GetFreeSurface();
				pDoc->Execute(new CSetFreeSurfaceAction(pDoc, freeSurface));
			}
			break;
		case IDCANCEL:
			break;
		default:
			ASSERT(FALSE);
	}
}

std::ostream& operator<< (std::ostream &os, const CFreeSurface &a)
{
	if (a.m_bFreeSurface) {
		os << "FREE_SURFACE_BC true\n";
	}
	else {
		os << "FREE_SURFACE_BC false\n";
	}
	return os;
}

