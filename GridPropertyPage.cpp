// GridPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "GridPropertyPage.h"
#include "Global.h"

#include "ResizeGridAction.h"
#include "Units1PropertyPage.h"
#include "WPhastDoc.h"

#include <float.h>  // DBL_MAX

// CGridPropertyPage dialog

IMPLEMENT_DYNAMIC(CGridPropertyPage, CPropertyPage)
CGridPropertyPage::CGridPropertyPage()
	: CPropertyPage(CGridPropertyPage::IDD)
{
	this->CommonConstruct();
}

CGridPropertyPage::CGridPropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CGridPropertyPage::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
{
	this->CommonConstruct();
}

void CGridPropertyPage::CommonConstruct(void)
{
	for(int i = 0; i < 3; ++i)
	{
		this->m_grid[i].count_coord = 2;
		this->m_grid[i].uniform = TRUE;
		this->m_grid[i].coord[0] = 0;
		this->m_grid[i].coord[1] = 1;
	}
	this->m_grid[0].c = 'x';
	this->m_grid[1].c = 'y';
	this->m_grid[2].c = 'z';

#ifdef _DEBUG
	/*
	-uniform x
			0       90000 16
	-uniform y
			0       48000 9
	-uniform z
			0       400 5
	*/
	this->m_grid[0].SetUniformRange(0, 90000, 16);
	this->m_grid[1].SetUniformRange(0, 48000, 9);
	this->m_grid[2].SetUniformRange(0, 400, 5);
#endif

}


CGridPropertyPage::~CGridPropertyPage()
{
}

void CGridPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_X_SPIN, m_spinX);
	DDX_Control(pDX, IDC_Y_SPIN, m_spinY);
	DDX_Control(pDX, IDC_Z_SPIN, m_spinZ);

	DDX_Text(pDX, IDC_XMIN_UNITS_STATIC, m_strHorizontalUnits);
	DDX_Text(pDX, IDC_XMAX_UNITS_STATIC, m_strHorizontalUnits);
	DDX_Text(pDX, IDC_YMIN_UNITS_STATIC, m_strHorizontalUnits);
	DDX_Text(pDX, IDC_YMAX_UNITS_STATIC, m_strHorizontalUnits);
	DDX_Text(pDX, IDC_ZMIN_UNITS_STATIC, m_strVerticalUnits);
	DDX_Text(pDX, IDC_ZMAX_UNITS_STATIC, m_strVerticalUnits);

	// UNIFORM X
	DDX_Text(pDX, IDC_XMIN_EDIT, m_grid[0].coord[0]);
	DDX_Text(pDX, IDC_XMAX_EDIT, m_grid[0].coord[1]);
	// DDV_MinMaxDouble(pDX, m_grid[0].coord[1], m_grid[0].coord[0], DBL_MAX);
	if (pDX->m_bSaveAndValidate && (m_grid[0].coord[1] <= m_grid[0].coord[0]))
	{
		::AfxMessageBox("Coordinate values must be in ascending order for X grid definition");
		pDX->Fail();
	}
	DDX_Text(pDX, IDC_X_NODES_EDIT, m_grid[0].count_coord);
	DDV_MinMaxInt(pDX, m_grid[0].count_coord, 2, INT_MAX);

	// UNIFORM Y
	DDX_Text(pDX, IDC_YMIN_EDIT, m_grid[1].coord[0]);
	DDX_Text(pDX, IDC_YMAX_EDIT, m_grid[1].coord[1]);
	if (pDX->m_bSaveAndValidate && (m_grid[1].coord[1] <= m_grid[1].coord[0]))
	{
		::AfxMessageBox("Coordinate values must be in ascending order for Y grid definition");
		pDX->Fail();
	}
	DDX_Text(pDX, IDC_Y_NODES_EDIT, m_grid[1].count_coord);
	DDV_MinMaxInt(pDX, m_grid[1].count_coord, 2, INT_MAX);

	// UNIFORM Z
	DDX_Text(pDX, IDC_ZMIN_EDIT, m_grid[2].coord[0]);
	DDX_Text(pDX, IDC_ZMAX_EDIT, m_grid[2].coord[1]);
	if (pDX->m_bSaveAndValidate && (m_grid[2].coord[1] <= m_grid[2].coord[0]))
	{
		::AfxMessageBox("Coordinate values must be in ascending order for Z grid definition");
		pDX->Fail();
	}
	DDX_Text(pDX, IDC_Z_NODES_EDIT, m_grid[2].count_coord);
	DDV_MinMaxInt(pDX, m_grid[2].count_coord, 2, INT_MAX);
}


BEGIN_MESSAGE_MAP(CGridPropertyPage, CPropertyPage)
	ON_EN_CHANGE(IDC_XMIN_EDIT, OnEnChange)
	ON_EN_CHANGE(IDC_YMIN_EDIT, OnEnChange)
	ON_EN_CHANGE(IDC_ZMIN_EDIT, OnEnChange)
	ON_EN_CHANGE(IDC_XMAX_EDIT, OnEnChange)
	ON_EN_CHANGE(IDC_YMAX_EDIT, OnEnChange)
	ON_EN_CHANGE(IDC_ZMAX_EDIT, OnEnChange)
	ON_EN_CHANGE(IDC_X_NODES_EDIT, OnEnChange)
	ON_EN_CHANGE(IDC_Y_NODES_EDIT, OnEnChange)
	ON_EN_CHANGE(IDC_Z_NODES_EDIT, OnEnChange)
END_MESSAGE_MAP()


// CGridPropertyPage message handlers

BOOL CGridPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add extra initialization here
	m_spinX.SetRange32(2, INT_MAX);
	m_spinY.SetRange32(2, INT_MAX);
	m_spinZ.SetRange32(2, INT_MAX);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGridPropertyPage::SetApply(CWPhastDoc* pDoc)
{
	this->m_pDoc = pDoc;

	const CUnits& units = this->m_pDoc->GetUnits();;
	if (units.horizontal.defined)
	{
		this->m_strHorizontalUnits.Format("[%s]", units.horizontal.input);
		CGlobal::MinimizeLengthUnits(this->m_strHorizontalUnits);
	}
	else
	{
		this->m_strHorizontalUnits.Format("[%s]", units.horizontal.si);
	}
	if (units.vertical.defined)
	{
		this->m_strVerticalUnits.Format("[%s]", units.vertical.input);
		CGlobal::MinimizeLengthUnits(this->m_strVerticalUnits);
	}
	else
	{
		this->m_strVerticalUnits.Format("[%s]", units.vertical.si);
	}
}


BOOL CGridPropertyPage::OnApply()
{
	// Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->m_psh.dwFlags & PSH_NOAPPLYNOW)
	{
		TRACE("NO APPLY NOW BUTTON\n");
	}
	else
	{
		// do we know if ok or apply was pressed
		if (this->UpdateData(TRUE) && this->m_pDoc)
		{
			// unrecorded undo
			CResizeGridAction action(this->m_pDoc, this->m_grid);
			action.Execute();
		}
	}

	return CPropertyPage::OnApply();
}

void CGridPropertyPage::OnEnChange()
{
	// enable apply button
	this->SetModified(TRUE);
}

BOOL CGridPropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();

	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		// get units
		for (int i = 0; i < pSheet->GetPageCount(); ++i)
		{
			if (pSheet->GetPage(i)->IsKindOf(RUNTIME_CLASS(CUnits1PropertyPage)))
			{
				CUnits units;
				static_cast<CUnits1PropertyPage*>(pSheet->GetPage(i))->GetProperties(units);
				if (units.horizontal.defined)
				{
					this->m_strHorizontalUnits.Format("[%s]", units.horizontal.input);
					CGlobal::MinimizeLengthUnits(this->m_strHorizontalUnits);
				}
				else
				{
					this->m_strHorizontalUnits.Format("[%s]", units.horizontal.si);
				}
				if (units.vertical.defined)
				{
					this->m_strVerticalUnits.Format("[%s]", units.vertical.input);
					CGlobal::MinimizeLengthUnits(this->m_strVerticalUnits);
				}
				else
				{
					this->m_strVerticalUnits.Format("[%s]", units.vertical.si);
				}
				this->UpdateData(FALSE);
				break;
			}
		}
		pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	}

	return bRet;
}

LRESULT CGridPropertyPage::OnWizardNext()
{
	// Add your specialized code here and/or call the base class
	/*
	Return Value
	0 to automatically advance to the next page;
	–1 to prevent the page from changing.
	To jump to a page other than the next one, 
	return the identifier of the dialog to be displayed.
	*/
	return CPropertyPage::OnWizardNext();
}
