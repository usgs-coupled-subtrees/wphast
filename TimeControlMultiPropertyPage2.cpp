// TimeControlMultiPropertyPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "TimeControlMultiPropertyPage2.h"


// CTimeControlMultiPropertyPage2 dialog

IMPLEMENT_DYNAMIC(CTimeControlMultiPropertyPage2, baseCTimeControlMultiPropertyPage2)
CTimeControlMultiPropertyPage2::CTimeControlMultiPropertyPage2()
	: baseCTimeControlMultiPropertyPage2(CTimeControlMultiPropertyPage2::IDD)
{
}

CTimeControlMultiPropertyPage2::~CTimeControlMultiPropertyPage2()
{
}

void CTimeControlMultiPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	baseCTimeControlMultiPropertyPage2::DoDataExchange(pDX);

	DDX_GridControl(pDX, IDC_TIMEEND_GRID, m_gridTimeEnd);
	DDX_GridControl(pDX, IDC_TIMESTEP_GRID, m_gridTimeStep);

	if (this->m_bFirstSetActive)
	{
		this->SetupGrids();
	}
}


BEGIN_MESSAGE_MAP(CTimeControlMultiPropertyPage2, baseCTimeControlMultiPropertyPage2)
END_MESSAGE_MAP()


// CTimeControlMultiPropertyPage2 message handlers

BOOL CTimeControlMultiPropertyPage2::OnInitDialog()
{
	baseCTimeControlMultiPropertyPage2::OnInitDialog();

	// Layout controls
	// Layout controls
	this->CreateRoot(VERTICAL)
		<< 	( pane(HORIZONTAL, GREEDY, 0, 0, 0 )
			<< item(IDC_TIMEEND_GRID, GREEDY)
			<< itemFixed(HORIZONTAL, 20)
			<< item(IDC_TIMESTEP_GRID, GREEDY)
			)
		<< itemFixed(VERTICAL, 10)
		<< item(IDC_DESC_RICHEDIT, ABSOLUTE_VERT)		
		;
	UpdateLayout();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTimeControlMultiPropertyPage2::SetupGrids(void)
{
	const int MIN_ROW_COUNT = 100;

	int nTimeEndRows  = MIN_ROW_COUNT;
	int nTimeStepRows = MIN_ROW_COUNT;

	try
	{
		this->m_gridTimeEnd.SetRowCount(nTimeEndRows);
		this->m_gridTimeEnd.SetColumnCount(2);
		this->m_gridTimeEnd.SetFixedRowCount(1);
		this->m_gridTimeEnd.SetFixedColumnCount(0);
		this->m_gridTimeEnd.EnableTitleTips(FALSE);

		this->m_gridTimeStep.SetRowCount(nTimeStepRows);
		this->m_gridTimeStep.SetColumnCount(4);
		this->m_gridTimeStep.SetFixedRowCount(1);
		this->m_gridTimeStep.SetFixedColumnCount(0);
		this->m_gridTimeStep.EnableTitleTips(FALSE);
	}
	catch (CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	std::vector<LPCTSTR> vecTimeUnits;
	vecTimeUnits.push_back(_T("seconds"));
	vecTimeUnits.push_back(_T("minutes"));
	vecTimeUnits.push_back(_T("hours"));
	vecTimeUnits.push_back(_T("days"));
	vecTimeUnits.push_back(_T("years"));

	this->m_gridTimeEnd.SetItemText(0, 0, "Time end");
	this->m_gridTimeEnd.SetItemText(0, 1, "Units");
	this->m_gridTimeEnd.SetColumnOptions(1, vecTimeUnits);

	this->m_gridTimeEnd.SetColumnWidth(0, 106);
	this->m_gridTimeEnd.SetColumnWidth(1, 106);

	this->m_gridTimeStep.SetItemText(0, 0, "Time");
	this->m_gridTimeStep.SetItemText(0, 1, "Units");
	this->m_gridTimeStep.SetItemText(0, 2, "Time step");
	this->m_gridTimeStep.SetItemText(0, 3, "Units");

	this->m_gridTimeStep.SetColumnOptions(1, vecTimeUnits);
	this->m_gridTimeStep.SetColumnOptions(3, vecTimeUnits);

	this->m_gridTimeStep.SetColumnWidth(0, 85);
	this->m_gridTimeStep.SetColumnWidth(1, 85);
	this->m_gridTimeStep.SetColumnWidth(2, 85);
	this->m_gridTimeStep.SetColumnWidth(3, 85);


	this->m_gridTimeStep.SetItemText(1, 0, "0"); this->m_gridTimeStep.DisableCell(1, 0);
	this->m_gridTimeStep.SetItemText(1, 1, "");  this->m_gridTimeStep.DisableCell(1, 1);

	return TRUE;
}
