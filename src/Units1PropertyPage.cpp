// Units1PropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "Units1PropertyPage.h"

#include "Global.h"


// CUnits1PropertyPage dialog

IMPLEMENT_DYNAMIC(CUnits1PropertyPage, CPropertyPage)

CUnits1PropertyPage::CUnits1PropertyPage()
	: CPropertyPage(CUnits1PropertyPage::IDD)
	, m_idSetFocus(0)
{
}

CUnits1PropertyPage::CUnits1PropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CUnits1PropertyPage::IDD, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
	, m_idSetFocus(0)
{
}

CUnits1PropertyPage::~CUnits1PropertyPage()
{
}

void CUnits1PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIME_COMBO, m_wndTimeCombo);
	DDX_Control(pDX, IDC_HORZ_COMBO, m_wndHorizontalCombo);
	DDX_Control(pDX, IDC_VERT_COMBO, m_wndVerticalCombo);
	DDX_Control(pDX, IDC_HORZ_MAP_COMBO, m_wndHorizontalMapCombo);
	DDX_Control(pDX, IDC_VERT_MAP_COMBO, m_wndVerticalMapCombo);
	DDX_Control(pDX, IDC_HEAD_COMBO, m_wndHeadCombo);

	// time
	if (m_wndTimeCombo.GetCount() == 0)
	{
		CGlobal::AddTimeUnits(&m_wndTimeCombo);
	}
	// horizontal
	if (m_wndHorizontalCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndHorizontalCombo);
	}
	// vertical
	if (m_wndVerticalCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndVerticalCombo);
	}
	// map_horizontal
	if (m_wndHorizontalMapCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndHorizontalMapCombo);
	}
	// map_vertical
	if (m_wndVerticalMapCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndVerticalMapCombo);
	}
	// head
	if (m_wndHeadCombo.GetCount() == 0)
	{
		CGlobal::AddLengthUnits(&m_wndHeadCombo);
	}

	if (pDX->m_bSaveAndValidate)
	{
		CString numer;
		CString denom;
		CString value;

		// time
		this->m_wndTimeCombo.GetLBText(this->m_wndTimeCombo.GetCurSel(), numer);
		VERIFY(this->m_units.time.set_input(numer) == OK);

		// horizontal
		this->m_wndHorizontalCombo.GetLBText(this->m_wndHorizontalCombo.GetCurSel(), numer);
		VERIFY(this->m_units.horizontal.set_input(numer) == OK);

		// vertical
		this->m_wndVerticalCombo.GetLBText(this->m_wndVerticalCombo.GetCurSel(), numer);
		VERIFY(this->m_units.vertical.set_input(numer) == OK);

		// map_horizontal
		this->m_wndHorizontalMapCombo.GetLBText(this->m_wndHorizontalMapCombo.GetCurSel(), numer);
		VERIFY(this->m_units.map_horizontal.set_input(numer) == OK);

		// map_vertical
		this->m_wndVerticalMapCombo.GetLBText(this->m_wndVerticalMapCombo.GetCurSel(), numer);
		VERIFY(this->m_units.map_vertical.set_input(numer) == OK);

		// head
		this->m_wndHeadCombo.GetLBText(this->m_wndHeadCombo.GetCurSel(), numer);
		VERIFY(this->m_units.head.set_input(numer) == OK);
	}
	else
	{
		// time
		if (this->m_units.time.defined)
		{
			VERIFY(this->m_wndTimeCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndTimeCombo.SelectString(0, CGlobal::GetStdTimeUnits(this->m_units.time.si).c_str()) != CB_ERR);
		}

		// horizontal
		if (this->m_units.horizontal.defined)
		{
			VERIFY(this->m_wndHorizontalCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.horizontal.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndHorizontalCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.horizontal.si).c_str()) != CB_ERR);
		}

		// vertical
		if (this->m_units.vertical.defined)
		{
			VERIFY(this->m_wndVerticalCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.vertical.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndVerticalCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.vertical.si).c_str()) != CB_ERR);
		}

		// map_horizontal
		if (this->m_units.map_horizontal.defined)
		{
			VERIFY(this->m_wndHorizontalMapCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.map_horizontal.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndHorizontalMapCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.map_horizontal.si).c_str()) != CB_ERR);
		}

		// map_vertical
		if (this->m_units.map_vertical.defined)
		{
			VERIFY(this->m_wndVerticalMapCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.map_vertical.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndVerticalMapCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.map_vertical.si).c_str()) != CB_ERR);
		}

		// head
		if (this->m_units.head.defined)
		{
			VERIFY(this->m_wndHeadCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.head.input).c_str()) != CB_ERR);
		}
		else
		{
			VERIFY(this->m_wndHeadCombo.SelectString(0, CGlobal::GetStdLengthUnits(this->m_units.head.si).c_str()) != CB_ERR);
		}
	}
}

void CUnits1PropertyPage::SetProperties(const CUnits& r_units)
{
	this->m_units = r_units;
}

void CUnits1PropertyPage::GetProperties(CUnits& r_units)const
{
	// time
	r_units.time           = this->m_units.time;
	// horizontal
	r_units.horizontal     = this->m_units.horizontal;
	// vertical
	r_units.vertical       = this->m_units.vertical;
	// map_horizontal
	r_units.map_horizontal = this->m_units.map_horizontal;
	// map_vertical
	r_units.map_vertical   = this->m_units.map_vertical;
	// head
	r_units.head           = this->m_units.head;
}

BEGIN_MESSAGE_MAP(CUnits1PropertyPage, CPropertyPage)
    ON_MESSAGE(WM_SETPAGEFOCUS, OnSetPageFocus)
END_MESSAGE_MAP()


// CUnits1PropertyPage message handlers

BOOL CUnits1PropertyPage::Contains(int nID)const
{
	switch (nID)
	{
		case IDC_TIME_COMBO:
		case IDC_HORZ_COMBO:
		case IDC_VERT_COMBO:
		case IDC_HORZ_MAP_COMBO:
		case IDC_VERT_MAP_COMBO:
		case IDC_HEAD_COMBO:
			return TRUE;
	}
	return FALSE;
}

void CUnits1PropertyPage::SetControlFocus(int nID)
{
	if (this->Contains(nID))
	{
		this->m_idSetFocus = nID;
	}
	else
	{
		this->m_idSetFocus = 0;
	}
}

BOOL CUnits1PropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();

	CPropertySheet* pSheet = (CPropertySheet*) this->GetParent();   
	if (pSheet->IsWizard())
	{
		pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	}
	else
	{
		this->PostMessage(WM_SETPAGEFOCUS, 0, 0L);
	}
	return bRet;
}

LRESULT CUnits1PropertyPage::OnSetPageFocus(WPARAM, LPARAM)
{
	if (this->m_idSetFocus != 0)
	{
		if (CWnd* pWnd = this->GetDlgItem(this->m_idSetFocus))
		{
			pWnd->SetFocus();
		}
	}
	return 0;
}

BOOL CUnits1PropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
