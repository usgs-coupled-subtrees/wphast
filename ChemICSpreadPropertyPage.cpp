// ChemICSpreadPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ChemICSpreadPropertyPage.h"

#include "property.h"
#include "Global.h"

// CChemICSpreadPropertyPage dialog
const TCHAR PSZ_SOLUTION[]           = _T("Solution");
const TCHAR PSZ_EQUILIBRIUM_PHASES[] = _T("Equilibrium phases");
const TCHAR PSZ_EXCHANGE[]           = _T("Exchange");
const TCHAR PSZ_SURFACE[]            = _T("Surface");
const TCHAR PSZ_GAS_PHASE[]          = _T("Gas phase");
const TCHAR PSZ_SOLID_SOLUTIONS[]    = _T("Solid solutions");
const TCHAR PSZ_KINETICS[]           = _T("Kinetics");

const TCHAR HEADING_VALUE[]          = _T("Value");
const TCHAR HEADING_INTERPOLATE[]    = _T("Interpolate");
const TCHAR HEADING_DIRECTION[]      = _T("Direction");
const TCHAR HEADING_VALUE_1[]        = _T("Value 1");
const TCHAR HEADING_DISTANCE_1[]     = _T("Distance 1");
const TCHAR HEADING_VALUE_2[]        = _T("Value 2");
const TCHAR HEADING_DISTANCE_2[]     = _T("Distance 2");


IMPLEMENT_DYNAMIC(CChemICSpreadPropertyPage, CPropertyPage)
CChemICSpreadPropertyPage::CChemICSpreadPropertyPage()
	: CPropertyPage(CChemICSpreadPropertyPage::IDD)
{
	this->CommonConstruct();
}

void CChemICSpreadPropertyPage::CommonConstruct(void)
{
	this->SetFlowOnly(false);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sSolutionRTF,          IDR_CHEM_IC_SOLUTION_RTF);
	CGlobal::LoadRTFString(this->m_sEquilibriumPhasesRTF, IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF);
	CGlobal::LoadRTFString(this->m_sSurfaceRTF,           IDR_CHEM_IC_SURFACE_RTF);
	CGlobal::LoadRTFString(this->m_sExchangeRTF,          IDR_CHEM_IC_EXCHANGE_RTF);
	CGlobal::LoadRTFString(this->m_sGasPhaseRTF,          IDR_CHEM_IC_GAS_PHASE_RTF);
	CGlobal::LoadRTFString(this->m_sSolidSolutionsRTF,    IDR_CHEM_IC_SOLID_SOLUTIONS_RTF);
	CGlobal::LoadRTFString(this->m_sKineticsRTF,          IDR_CHEM_IC_KINETICS_RTF);
}

CChemICSpreadPropertyPage::~CChemICSpreadPropertyPage()
{
}

void CChemICSpreadPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	static const int COL_VALUE       = 1;
	static const int COL_INTERPOLATE = 2;

	CPropertyPage::DoDataExchange(pDX);

	DDX_GridControl(pDX, IDC_CHEMIC_GRID, m_gridChemIC);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	if (this->m_bFirstSetActive)
	{
		this->SetupGrids();

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sSolutionRTF.c_str());
	}

	int nRow = 0;

	// PSZ_SOLUTION
	CGlobal::DDX_Property(pDX, IDC_CHEMIC_GRID, ++nRow, this->m_chemIC.solution, false);
	if (!this->m_bFlowOnly && pDX->m_bSaveAndValidate && this->m_chemIC.solution->type == UNDEFINED)
	{
		CString str;
		str.Format("A default \"%s\" must be defined.", PSZ_SOLUTION);
		if (this->m_gridChemIC.GetCheck(nRow, COL_INTERPOLATE) == BST_CHECKED)
		{
			::DDX_GridControlFail(pDX, IDC_CHEMIC_GRID, nRow, COL_INTERPOLATE, str);
		}
		else
		{
			::DDX_GridControlFail(pDX, IDC_CHEMIC_GRID, nRow, COL_VALUE, str);
		}
	}

	// PSZ_EQUILIBRIUM_PHASES
	CGlobal::DDX_Property(pDX, IDC_CHEMIC_GRID, ++nRow, this->m_chemIC.equilibrium_phases, false);

	// PSZ_EXCHANGE
	CGlobal::DDX_Property(pDX, IDC_CHEMIC_GRID, ++nRow, this->m_chemIC.exchange, false);

	// PSZ_SURFACE
	CGlobal::DDX_Property(pDX, IDC_CHEMIC_GRID, ++nRow, this->m_chemIC.surface, false);

	// PSZ_GAS_PHASE
	CGlobal::DDX_Property(pDX, IDC_CHEMIC_GRID, ++nRow, this->m_chemIC.gas_phase, false);

	// PSZ_SOLID_SOLUTIONS
	CGlobal::DDX_Property(pDX, IDC_CHEMIC_GRID, ++nRow, this->m_chemIC.solid_solutions, false);

	// PSZ_KINETICS
	CGlobal::DDX_Property(pDX, IDC_CHEMIC_GRID, ++nRow, this->m_chemIC.kinetics, false);
}


BEGIN_MESSAGE_MAP(CChemICSpreadPropertyPage, CPropertyPage)
	ON_NOTIFY(GVN_CHECKCHANGED, IDC_CHEMIC_GRID, OnCheckChangedChemIC)
	ON_NOTIFY(GVN_SELCHANGED, IDC_CHEMIC_GRID, OnSelChangedChemIC)
END_MESSAGE_MAP()


// CChemICSpreadPropertyPage message handlers

BOOL CChemICSpreadPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CChemICSpreadPropertyPage::GetProperties(CChemIC& chemIC)const
{
	chemIC = this->m_chemIC;

	// delete UNDEFINED properties
	//
	if (chemIC.solution->type == UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.solution, 0);
	}
	if (chemIC.equilibrium_phases->type == UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.equilibrium_phases, 0);
	}
	if (chemIC.surface->type == UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.surface, 0);
	}
	if (chemIC.gas_phase->type == UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.gas_phase, 0);
	}
	if (chemIC.solid_solutions->type == UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.solid_solutions, 0);
	}
	if (chemIC.kinetics->type == UNDEFINED)
	{
		Cproperty::CopyProperty(&chemIC.kinetics, 0);
	}
}

void CChemICSpreadPropertyPage::SetProperties(const CChemIC& chemIC)
{
	// copy grid_elt
	this->m_chemIC = chemIC;
	
	// fill empty properties
	//
	if (this->m_chemIC.solution == NULL)
	{
		this->m_chemIC.solution = new Cproperty();
	}
	if (this->m_chemIC.equilibrium_phases == NULL)
	{
		this->m_chemIC.equilibrium_phases = new Cproperty();
	}
	if (this->m_chemIC.exchange == NULL)
	{
		this->m_chemIC.exchange = new Cproperty();
	}
	if (this->m_chemIC.surface == NULL)
	{
		this->m_chemIC.surface = new Cproperty();
	}
	if (this->m_chemIC.gas_phase == NULL)
	{
		this->m_chemIC.gas_phase = new Cproperty();
	}
	if (this->m_chemIC.solid_solutions == NULL)
	{
		this->m_chemIC.solid_solutions = new Cproperty();
	}
	if (this->m_chemIC.kinetics == NULL)
	{
		this->m_chemIC.kinetics = new Cproperty();
	}
}

BOOL CChemICSpreadPropertyPage::SetupGrids(void)
{
	std::vector<LPCTSTR> vecDirection;
	vecDirection.push_back(_T("X"));
	vecDirection.push_back(_T("Y"));
	vecDirection.push_back(_T("Z"));

	try
	{
		this->m_gridChemIC.SetRowCount(8);
		this->m_gridChemIC.SetColumnCount(8);
		VERIFY(this->m_gridChemIC.SetFixedRowCount(1) == TRUE);
		this->m_gridChemIC.SetFixedColumnCount(1);
		this->m_gridChemIC.EnableTitleTips(FALSE);
		this->m_gridChemIC.SetRowResize(FALSE);

		// set default format
		GV_ITEM defaultFormat;
		defaultFormat.mask    = GVIF_FORMAT;
		defaultFormat.nFormat = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS;
		for (int row = 0; row < this->m_gridChemIC.GetRowCount(); ++row)
		{
			defaultFormat.row = row;
			for (int col = 0; col < this->m_gridChemIC.GetColumnCount(); ++col)
			{ 
				defaultFormat.col = col;
				this->m_gridChemIC.SetItem(&defaultFormat);
			}
		}

		int nRow = 0;

		this->m_gridChemIC.SetItemText(++nRow, 0, PSZ_SOLUTION);
		this->m_gridChemIC.SetItemText(++nRow, 0, PSZ_EQUILIBRIUM_PHASES);
		this->m_gridChemIC.SetItemText(++nRow, 0, PSZ_EXCHANGE);
		this->m_gridChemIC.SetItemText(++nRow, 0, PSZ_SURFACE);
		this->m_gridChemIC.SetItemText(++nRow, 0, PSZ_GAS_PHASE);
		this->m_gridChemIC.SetItemText(++nRow, 0, PSZ_SOLID_SOLUTIONS);
		this->m_gridChemIC.SetItemText(++nRow, 0, PSZ_KINETICS);

		int nCol = 0;
		this->m_gridChemIC.SetItemText(0, ++nCol, HEADING_VALUE);
		this->m_gridChemIC.SetItemText(0, ++nCol, HEADING_INTERPOLATE);  this->m_gridChemIC.SetColumnCheck(nCol, BST_UNCHECKED);
		this->m_gridChemIC.SetItemText(0, ++nCol, HEADING_DIRECTION);    this->m_gridChemIC.SetColumnOptions(nCol, vecDirection);
		this->m_gridChemIC.SetItemText(0, ++nCol, HEADING_VALUE_1);
		this->m_gridChemIC.SetItemText(0, ++nCol, HEADING_DISTANCE_1);
		this->m_gridChemIC.SetItemText(0, ++nCol, HEADING_VALUE_2);
		this->m_gridChemIC.SetItemText(0, ++nCol, HEADING_DISTANCE_2);

		this->m_gridChemIC.SetColumnWidth(0, 140);
	}
	catch (CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	return TRUE;
}

void CChemICSpreadPropertyPage::OnCheckChangedChemIC(NMHDR *pNotifyStruct, LRESULT *result)
{
	static const int COL_VALUE       = 1;
	static const int COL_INTERPOLATE = 2;
	static const int COL_DIRECTION   = 3;
	static const int COL_VALUE_1     = 4;
	static const int COL_DISTANCE_1  = 5;
	static const int COL_VALUE_2     = 6;
	static const int COL_DISTANCE_2  = 7;

	CModGridCtrlEx* pGrid = &this->m_gridChemIC;

	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	if (pnmgv->iColumn == COL_INTERPOLATE)
	{
		ASSERT(pGrid->IsCheckMarkCell(pnmgv->iRow, pnmgv->iColumn));
		if (pGrid->GetCheck(pnmgv->iRow, pnmgv->iColumn) == BST_CHECKED)
		{
			// disable
			pGrid->DisableCell(pnmgv->iRow, COL_VALUE);
			// enable
			pGrid->EnableCell(pnmgv->iRow,  COL_DIRECTION);
			pGrid->EnableCell(pnmgv->iRow,  COL_VALUE_1);
			pGrid->EnableCell(pnmgv->iRow,  COL_DISTANCE_1);
			pGrid->EnableCell(pnmgv->iRow,  COL_VALUE_2);
			pGrid->EnableCell(pnmgv->iRow,  COL_DISTANCE_2);
		}
		else
		{
			// enable
			pGrid->EnableCell(pnmgv->iRow, COL_VALUE);
			// disable
			pGrid->DisableCell(pnmgv->iRow,  COL_DIRECTION);
			pGrid->DisableCell(pnmgv->iRow,  COL_VALUE_1);
			pGrid->DisableCell(pnmgv->iRow,  COL_DISTANCE_1);
			pGrid->DisableCell(pnmgv->iRow,  COL_VALUE_2);
			pGrid->DisableCell(pnmgv->iRow,  COL_DISTANCE_2);
		}
		pGrid->Invalidate();
	}
}

void CChemICSpreadPropertyPage::OnSelChangedChemIC(NMHDR *pNotifyStruct, LRESULT *result)
{
	TRACE("OnSelChangedChemIC\n");
	CCellID focus = this->m_gridChemIC.GetFocusCell();

	if (!this->m_gridChemIC.IsValid(focus)) return;
	
	CString strItem = this->m_gridChemIC.GetItemText(focus.row, 0);

	// PSZ_SOLUTION
	if (strItem.Compare(PSZ_SOLUTION) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sSolutionRTF.c_str());
	}
	// PSZ_EQUILIBRIUM_PHASES
	else if (strItem.Compare(PSZ_EQUILIBRIUM_PHASES) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sEquilibriumPhasesRTF.c_str());
	}
	// PSZ_EXCHANGE
	else if (strItem.Compare(PSZ_EXCHANGE) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sExchangeRTF.c_str());
	}
	// PSZ_SURFACE
	else if (strItem.Compare(PSZ_SURFACE) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sSurfaceRTF.c_str());
	}
	// PSZ_GAS_PHASE
	else if (strItem.Compare(PSZ_GAS_PHASE) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sGasPhaseRTF.c_str());
	}
	// PSZ_SOLID_SOLUTIONS
	else if (strItem.Compare(PSZ_SOLID_SOLUTIONS) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sSolidSolutionsRTF.c_str());
	}
	// PSZ_KINETICS
	else if (strItem.Compare(PSZ_KINETICS) == 0)
	{
		this->m_wndRichEditCtrl.SetWindowText(this->m_sKineticsRTF.c_str());
	}
	// ERROR
	else
	{
		ASSERT(FALSE);
		this->m_wndRichEditCtrl.SetWindowText("");
	}
}
