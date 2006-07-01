#include "StdAfx.h"
#include "BCSpecHeadZoneActor.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro

#include "ETSLayout/ETSLayout.h"
#include "WPhastDoc.h"
#include "SetBCAction.h"
#include "BCSpecifiedHeadPropertyPage.h"
#include "FlowOnly.h"


vtkCxxRevisionMacro(CBCSpecHeadZoneActor, "$Revision: 966 $");
vtkStandardNewMacro(CBCSpecHeadZoneActor);

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBCSpecHeadZoneActor::CBCSpecHeadZoneActor(void)
{
}

CBCSpecHeadZoneActor::~CBCSpecHeadZoneActor(void)
{
}

void CBCSpecHeadZoneActor::InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	CString str = this->GetTreeHeading();
	this->m_hti = pTreeCtrl->InsertItem(str, hParent, hInsertAfter);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti); // might want to move this
	this->Update(pTreeCtrl, this->m_hti);
}

void CBCSpecHeadZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	// remove all previous items
	//
	while (HTREEITEM hChild = pTreeCtrl->GetChildItem(htiParent))
	{
		pTreeCtrl->DeleteItem(hChild);
	}

	// update description
	//
	pTreeCtrl->SetItemText(htiParent, this->GetTreeHeading());

	ASSERT(this->m_bc.bc_type == SPECIFIED);
	// case SPECIFIED:
	// head
	this->m_bc.m_bc_head.InsertItem(pTreeCtrl, "head", htiParent);

	// associated_solution
	if (this->m_bc.bc_solution_type == ASSOCIATED)
	{
		this->m_bc.m_bc_solution.InsertItem(pTreeCtrl, "associated_solution", htiParent);
	}

	// fixed_solution
	if (this->m_bc.bc_solution_type == FIXED)
	{
		this->m_bc.m_bc_solution.InsertItem(pTreeCtrl, "fixed_solution", htiParent);
	}
}


void CBCSpecHeadZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	CString str;
	str.Format(_T("%s Properties"), this->GetName());
	ETSLayoutPropertySheet props(str);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	ASSERT(this->m_bc.bc_type == SPECIFIED);

	CBCSpecifiedHeadPropertyPage specified;
	props.AddPage(&specified);
	specified.SetProperties(this->GetBC());		
	specified.SetFlowOnly(pDoc->GetFlowOnly());
	specified.SetDesc(this->GetDesc());
	if (props.DoModal() == IDOK)
	{
		CBC bc;
		specified.GetProperties(bc);
		pDoc->Execute(new CSetBCAction(this, pTreeCtrl, bc, specified.GetDesc()));
	}		
}
