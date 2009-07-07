// FluxPropsPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "FluxPropsPage2.h"

#include "Global.h"


// CFluxPropsPage2 dialog

const TCHAR PSZ_FLUX[]     = _T("Flux");
const TCHAR PSZ_SOLUTION[] = _T("Solution");

const int SELECTED = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CFluxPropsPage2, CPropertyPage)

CFluxPropsPage2::CFluxPropsPage2()
	: CPropertyPage(CFluxPropsPage2::IDD)
	, FluxSeries(this)
	, SolutionSeries(this)
{
	TRACE("In %s\n", __FUNCTION__);

	// load property descriptions
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,   IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sAssocSolutionRTF, IDR_BC_FLUX_ASSOC_SOL_RTF);
	CGlobal::LoadRTFString(this->m_sFluxRTF,          IDR_BC_FLUX_FLUX_RTF);
	CGlobal::LoadRTFString(this->m_sFaceRTF,          IDR_BC_FLUX_FACE_RTF);

	// init properties
	this->SetFlowOnly(false);
	this->SetDefault(false);

	TRACE("Out %s\n", __FUNCTION__);
}

CFluxPropsPage2::~CFluxPropsPage2()
{
	TRACE("In %s\n", __FUNCTION__);
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	DDX_Control(pDX, IDC_PROP_TREE, this->TreeCtrl);

	if (this->TreeCtrl.GetCount() == 0)
	{
		this->FluxSeries.treeitem     = this->TreeCtrl.InsertItem(PSZ_FLUX,     TVI_ROOT, TVI_LAST);
		this->SolutionSeries.treeitem = this->TreeCtrl.InsertItem(PSZ_SOLUTION, TVI_ROOT, TVI_LAST);

		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);

		this->ItemDDX = this->FluxSeries.treeitem;
		this->TreeCtrl.SelectItem(this->ItemDDX);

		this->TreeCtrl.ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);
	}

	DDX_GridControl(pDX, IDC_POINTS_GRID, this->PointsGrid);
	DDX_GridControl(pDX, IDC_TIMES_GRID,    this->FluxSeries.grid);
	DDX_GridControl(pDX, IDC_GRID_SOLUTION, this->SolutionSeries.grid);

	this->FluxSeries.InitializeGrid(pDX);
	this->SolutionSeries.InitializeGrid(pDX);

	this->FluxSeries.SetPointsGrid(&this->PointsGrid);
	this->SolutionSeries.SetPointsGrid(&this->PointsGrid);

	// description
	::DDX_Text(pDX, IDC_DESC_EDIT, this->Description);

	if (pDX->m_bSaveAndValidate)
	{
		// face
		if (this->IsDlgButtonChecked(IDC_FACE_X_RADIO))
		{
			this->BC.face_defined = TRUE;
			this->BC.face         = 0;
		}
		if (this->IsDlgButtonChecked(IDC_FACE_Y_RADIO))
		{
			this->BC.face_defined = TRUE;
			this->BC.face         = 1;
		}
		if (this->IsDlgButtonChecked(IDC_FACE_Z_RADIO))
		{
			this->BC.face_defined = TRUE;
			this->BC.face         = 2;
		}

		// time series
		if (this->ItemDDX == this->FluxSeries.treeitem)
		{
			this->FluxSeries.DDV_SoftValidate();
			this->FluxSeries.DDX_Series(pDX);
			ASSERT(!this->SolutionSeries.grid.IsWindowVisible());
			ASSERT(this->FluxSeries.grid.IsWindowVisible());
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.DDV_SoftValidate();
			this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);
			ASSERT(this->SolutionSeries.grid.IsWindowVisible());
			ASSERT(!this->FluxSeries.grid.IsWindowVisible());
		}
	}
	else
	{
		// face
		switch(this->BC.face)
		{
		case 0: // x
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_X_RADIO);
			break;
		case 1: // y
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_Y_RADIO);
			break;
		case 2: // z
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_Z_RADIO);
			break;
		default: // x
			this->CheckRadioButton(IDC_FACE_X_RADIO, IDC_FACE_Z_RADIO, IDC_FACE_X_RADIO);
		}

		// time series
		if (this->ItemDDX == this->FluxSeries.treeitem)
		{
			this->FluxSeries.DDX_Series(pDX);
			this->SolutionSeries.grid.ShowWindow(SW_HIDE);
			this->FluxSeries.grid.ShowWindow(SW_SHOW);
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.DDX_Series(pDX, !this->FlowOnly);
			this->FluxSeries.grid.ShowWindow(SW_HIDE);
			this->SolutionSeries.grid.ShowWindow(SW_SHOW);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::SetProperties(const CBC& rBC)
{
	TRACE("In %s\n", __FUNCTION__);

	// copy grid_elt
	this->BC = rBC;

	// time series
	this->FluxSeries.SetSeries(rBC.m_bc_flux);
	this->SolutionSeries.SetSeries(rBC.m_bc_solution);

	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::GetProperties(CBC& rBC)const
{
	TRACE("In %s\n", __FUNCTION__);

	rBC = this->BC;

	rBC.m_bc_flux.clear();
	rBC.m_bc_solution.clear();

	for (size_t row = 1; row < this->FluxSeries.v_times.size(); ++row)
	{
		if (this->FluxSeries.v_times[row] && (*this->FluxSeries.vv_props[SELECTED])[row])
		{
			Ctime t(*this->FluxSeries.v_times[row]);
			Cproperty p(*(*this->FluxSeries.vv_props[SELECTED])[row]);
			rBC.m_bc_flux[t] = p;
		}
	}

	for (size_t row = 1; row < this->SolutionSeries.v_times.size(); ++row)
	{
		if (this->SolutionSeries.v_times[row] && (*this->SolutionSeries.vv_props[SELECTED])[row])
		{
			Ctime t(*this->SolutionSeries.v_times[row]);
			Cproperty p(*(*this->SolutionSeries.vv_props[SELECTED])[row]);
			rBC.m_bc_solution[t] = p;
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

BEGIN_MESSAGE_MAP(CFluxPropsPage2, CPropertyPage)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_TIMES_GRID, OnEndLabelEditFlux)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_SOLUTION, OnEndLabelEditSolution)

	ON_NOTIFY(GVN_SELCHANGED, IDC_TIMES_GRID,    OnSelChangedFlux)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_SOLUTION, OnSelChangedSolution)

	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGED,  IDC_PROP_TREE, OnTreeSelChanged)
	ON_EN_SETFOCUS(IDC_DESC_EDIT, OnEnSetfocusDescEdit)

	ON_BN_CLICKED(IDC_BUTTON_XYZ, OnBnClickedButtonXYZ)

	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
END_MESSAGE_MAP()


// CFluxPropsPage2 message handlers

void CFluxPropsPage2::OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->FluxSeries.OnEndLabelEdit(pnmgv->iRow, pnmgv->iColumn);
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->SolutionSeries.OnEndLabelEdit(pnmgv->iRow, pnmgv->iColumn);
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemOld.hItem;
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->FluxSeries.treeitem)
		{
			this->FluxSeries.DDV_SoftValidate();
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.DDV_SoftValidate();
		}

		//{{{6/26/2009 5:12:11 PM}
		// force CInPlaceXXX to lose focus
		this->TreeCtrl.SetFocus();
		//}}{6/26/2009 5:12:11 PM}

		if (!this->UpdateData(TRUE))
		{
			// notify which control caused failure
			//
			CWnd* pFocus = CWnd::GetFocus();
			this->PostMessage(UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

			// disallow change
			//
			*pResult = TRUE;
			TRACE("Out %s Disallowed\n", __FUNCTION__);
			return;
		}
	}
	*pResult = 0;
	TRACE("Out Allowed %s\n", __FUNCTION__);
}

#define COMPARE_SET(S, R) \
do { \
	if (strItem.Compare(S) == 0) { \
		this->RichEditCtrl.SetWindowText(R.c_str()); \
	} \
} while (0)

void CFluxPropsPage2::OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	UNREFERENCED_PARAMETER(pResult);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemNew.hItem;
	if (this->ItemDDX)
	{
		this->UpdateData(FALSE);

		// update property description
		//
		if (this->TreeCtrl.GetSafeHwnd())
		{
			CString strItem = this->TreeCtrl.GetItemText(this->ItemDDX);

			COMPARE_SET(PSZ_FLUX,     this->m_sFluxRTF);
			COMPARE_SET(PSZ_SOLUTION, this->m_sAssocSolutionRTF);
		}
	}
	if (this->TreeCtrl.GetSafeHwnd())
	{
		this->TreeCtrl.SetFocus();
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnEnSetfocusDescEdit()
{
	TRACE("In %s\n", __FUNCTION__);
	this->RichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnSelChangedFlux(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->FluxSeries.OnSelChanged(pnmgv->iRow, pnmgv->iColumn);

	TRACE("Out %s\n", __FUNCTION__);
}

void CFluxPropsPage2::OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	this->SolutionSeries.OnSelChanged(pnmgv->iRow, pnmgv->iColumn);

	TRACE("Out %s\n", __FUNCTION__);
}

LRESULT CFluxPropsPage2::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	CWnd* pFocus = (CWnd*)wParam;
	if (pFocus && pFocus->GetSafeHwnd())
	{
		ASSERT_KINDOF(CWnd, pFocus);
		pFocus->SetFocus();
	}
	return 0;
}

void CFluxPropsPage2::OnBnClickedButtonXYZ()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->FluxSeries.treeitem)
		{
			this->FluxSeries.OnBnClickedButtonXYZ();
		}
		else if (this->ItemDDX == this->SolutionSeries.treeitem)
		{
			this->SolutionSeries.OnBnClickedButtonXYZ();
		}
	}
}

void CFluxPropsPage2::SetUnits(const CUnits &u)
{
	this->FluxSeries.SetUnits(u);
	this->SolutionSeries.SetUnits(u);
}
