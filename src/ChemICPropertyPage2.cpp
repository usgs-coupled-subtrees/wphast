// ChemICPropertyPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ChemICPropertyPage2.h"

#include "Global.h"
#include "property.h"
#include "NewModelWizard.h"


// CChemICPropertyPage2 dialog

const TCHAR PSZ_SOLUTION[]           = _T("Solution");
const TCHAR PSZ_EQUILIBRIUM_PHASES[] = _T("Equilibrium phases");
const TCHAR PSZ_EXCHANGE[]           = _T("Exchange");
const TCHAR PSZ_SURFACE[]            = _T("Surface");
const TCHAR PSZ_GAS_PHASE[]          = _T("Gas phase");
const TCHAR PSZ_SOLID_SOLUTIONS[]    = _T("Solid solutions");
const TCHAR PSZ_KINETICS[]           = _T("Kinetics");

IMPLEMENT_DYNAMIC(CChemICPropertyPage2, CPropertyPage)

CChemICPropertyPage2::CChemICPropertyPage2()
: CPropertyPage(CChemICPropertyPage2::IDD)
{
	this->CommonConstruct();
}

CChemICPropertyPage2::CChemICPropertyPage2(UINT nIDTemplate, UINT nIDCaption)
: CPropertyPage(nIDTemplate, nIDCaption)
{
	this->CommonConstruct();
}

CChemICPropertyPage2::CChemICPropertyPage2(UINT nIDTemplate, UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
: CPropertyPage(nIDTemplate, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
{
	this->CommonConstruct();
}

void CChemICPropertyPage2::CommonConstruct(void)
{
	// init properties
	//
	this->SetProperties(this->ChemIC);

	this->FirstPaint = true;
	this->SetNewModelWizard(false);
	this->SetFlowOnlyWizard(false);
	this->SetShowFlowOnlyMessage(false);


	// load property descriptions
	//
	CGlobal::LoadRTFString(this->SolutionRTF,          IDR_CHEM_IC_SOLUTION_RTF);
	CGlobal::LoadRTFString(this->EquilibriumPhasesRTF, IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF);
	CGlobal::LoadRTFString(this->SurfaceRTF,           IDR_CHEM_IC_SURFACE_RTF);
	CGlobal::LoadRTFString(this->ExchangeRTF,          IDR_CHEM_IC_EXCHANGE_RTF);
	CGlobal::LoadRTFString(this->GasPhaseRTF,          IDR_CHEM_IC_GAS_PHASE_RTF);
	CGlobal::LoadRTFString(this->SolidSolutionsRTF,    IDR_CHEM_IC_SOLID_SOLUTIONS_RTF);
	CGlobal::LoadRTFString(this->KineticsRTF,          IDR_CHEM_IC_KINETICS_RTF);
}


CChemICPropertyPage2::~CChemICPropertyPage2()
{
}

void CChemICPropertyPage2::GetProperties(CChemIC& chemIC)const
{
	chemIC = this->ChemIC;

	// delete UNDEFINED properties
	//
	if (chemIC.solution->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.solution, 0);
	}
	if (chemIC.equilibrium_phases->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.equilibrium_phases, 0);
	}
	if (chemIC.surface->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.surface, 0);
	}
	if (chemIC.gas_phase->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.gas_phase, 0);
	}
	if (chemIC.solid_solutions->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.solid_solutions, 0);
	}
	if (chemIC.kinetics->type == PROP_UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.kinetics, 0);
	}
}

void CChemICPropertyPage2::SetProperties(const CChemIC& chemIC)
{
	// copy grid_elt
	this->ChemIC = chemIC;
	
	// fill empty properties
	//
	if (this->ChemIC.solution == NULL)
	{
		this->ChemIC.solution = new Cproperty();
	}
	if (this->ChemIC.equilibrium_phases == NULL)
	{
		this->ChemIC.equilibrium_phases = new Cproperty();
	}
	if (this->ChemIC.exchange == NULL)
	{
		this->ChemIC.exchange = new Cproperty();
	}
	if (this->ChemIC.surface == NULL)
	{
		this->ChemIC.surface = new Cproperty();
	}
	if (this->ChemIC.gas_phase == NULL)
	{
		this->ChemIC.gas_phase = new Cproperty();
	}
	if (this->ChemIC.solid_solutions == NULL)
	{
		this->ChemIC.solid_solutions = new Cproperty();
	}
	if (this->ChemIC.kinetics == NULL)
	{
		this->ChemIC.kinetics = new Cproperty();
	}
}

void CChemICPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	::DDX_GridControl(pDX, IDC_GRID, this->Grid);
	::DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	this->RichEditCtrl.SetTargetDevice(NULL, 0);

	// Prepare Grid
	//
	if (!pDX->m_bSaveAndValidate && this->Grid.GetColumnCount() == 0)
	{
		TRY
		{
			this->Grid.SetRowCount(8);
			this->Grid.SetColumnCount(2);
			this->Grid.SetFixedRowCount(1);
			this->Grid.SetFixedColumnCount(1);
			this->Grid.EnableTitleTips(FALSE);
			this->Grid.SetHighLight(GV_HIGHLIGHT_ALWAYS);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;

		Item.row = 0;
		Item.col = 1;
		Item.szText.Format(_T("Value"));
		this->Grid.SetItem(&Item);

		Item.col = 0;

		Item.row = 1;
		Item.szText.Format(PSZ_SOLUTION);
		this->Grid.SetItem(&Item);

		++Item.row;
		Item.szText.Format(PSZ_EQUILIBRIUM_PHASES);
		this->Grid.SetItem(&Item);

		++Item.row;
		Item.szText.Format(PSZ_EXCHANGE);
		this->Grid.SetItem(&Item);

		++Item.row;
		Item.szText.Format(PSZ_SURFACE);
		this->Grid.SetItem(&Item);

		++Item.row;
		Item.szText.Format(PSZ_GAS_PHASE);
		this->Grid.SetItem(&Item);

		++Item.row;
		Item.szText.Format(PSZ_SOLID_SOLUTIONS);
		this->Grid.SetItem(&Item);

		++Item.row;
		Item.szText.Format(PSZ_KINETICS);
		this->Grid.SetItem(&Item);

		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask    = GVIF_FORMAT;
		defaultFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;

		for (int row = 0; row < this->Grid.GetRowCount(); ++row)
		{
			defaultFormat.row = row;
			for (int col = 0; col < this->Grid.GetColumnCount(); ++col)
			{ 
				defaultFormat.col = col;
				this->Grid.SetItem(&defaultFormat);
			}
			this->Grid.SetRowHeight(row, 17);
		}

		this->Grid.SetColumnWidth(0, 129);
		this->Grid.SetColumnWidth(1, 322);

	    this->Grid.SetScrollRange(SB_VERT, 0, 0, TRUE); // reset vertical scrollbar

		// desc
		this->RichEditCtrl.SetWindowText(this->SolutionRTF.c_str());
	}

	if (pDX->m_bSaveAndValidate)
	{
		for (int c = 0; c < this->Grid.GetColumnCount(); ++c)
		{
			TRACE("col width %d = %d\n", c, this->Grid.GetColumnWidth(c));
		}
		for (int r = 0; r < this->Grid.GetRowCount(); ++r)
		{
			TRACE("row height %d = %d\n", r, this->Grid.GetRowHeight(r));
		}
	}

	int row = 0;
	std::string units;
	CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->ChemIC.solution),           units, false);
	CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->ChemIC.equilibrium_phases), units, false);
	CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->ChemIC.exchange),           units, false);
	CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->ChemIC.surface),            units, false);
	CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->ChemIC.gas_phase),          units, false);
	CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->ChemIC.solid_solutions),    units, false);
	CGlobal::DDX_Property(pDX, IDC_GRID, ++row, static_cast<Cproperty*>(this->ChemIC.kinetics),           units, false);
}


BEGIN_MESSAGE_MAP(CChemICPropertyPage2, CPropertyPage)
	ON_WM_PAINT()
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnSelChanged)
END_MESSAGE_MAP()


// CChemICPropertyPage2 message handlers

BOOL CChemICPropertyPage2::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (this->GetNewModelWizard())
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);

			// force a default solution if not flow only
			CNewModelWizard* pWizard = static_cast<CNewModelWizard*>(pSheet);
			this->SetFlowOnly((bool)pWizard->GetFlowOnly());
		}
		else if (this->GetFlowOnlyWizard())
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
			this->SetFlowOnly(false);
		}
		else
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
		}
	}
	return bRet;
}

BOOL CChemICPropertyPage2::OnKillActive()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
}

LRESULT CChemICPropertyPage2::OnWizardNext()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (!this->UpdateData(TRUE))
		{
			return -1; // return –1 to prevent the page from changing 
		}
	}
	return CPropertyPage::OnWizardNext();
}


void CChemICPropertyPage2::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// Add your message handler code here
	// Do not call CPropertyPage::OnPaint() for painting messages
	if (this->FirstPaint)
	{
		this->FirstPaint = false;

		if (this->ShowFlowOnlyMessage)
		{
			::AfxMessageBox("Since you have turned on solute transport you may want to review the default solution.");
			this->ShowFlowOnlyMessage = false;
		}
	}
}

void CChemICPropertyPage2::OnSelChanged(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("In %s\n", __FUNCTION__);

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;

	switch (pnmgv->iRow)
	{
	case 1: // PSZ_SOLUTION
		this->RichEditCtrl.SetWindowText(this->SolutionRTF.c_str());
		break;

	case 2: // PSZ_EQUILIBRIUM_PHASES
		this->RichEditCtrl.SetWindowText(this->EquilibriumPhasesRTF.c_str());
		break;

	case 3: // PSZ_EXCHANGE
		this->RichEditCtrl.SetWindowText(this->ExchangeRTF.c_str());
		break;

	case 4: // PSZ_SURFACE
		this->RichEditCtrl.SetWindowText(this->SurfaceRTF.c_str());
		break;

	case 5: // PSZ_GAS_PHASE
		this->RichEditCtrl.SetWindowText(this->GasPhaseRTF.c_str());
		break;

	case 6: // PSZ_SOLID_SOLUTIONS
		this->RichEditCtrl.SetWindowText(this->SolidSolutionsRTF.c_str());
		break;

	case 7: // PSZ_KINETICS
		this->RichEditCtrl.SetWindowText(this->KineticsRTF.c_str());
		break;

	default:
		ASSERT(FALSE);
		this->RichEditCtrl.SetWindowText("");
		break;
	}
	TRACE("Out %s\n", __FUNCTION__);
}
