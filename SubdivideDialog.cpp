// SubdivideDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "SubdivideDialog.h"
#include "Utilities.h"

// CSubdivideDialog dialog

IMPLEMENT_DYNAMIC(CSubdivideDialog, CDialog)
CSubdivideDialog::CSubdivideDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSubdivideDialog::IDD, pParent)
	, numSubInterval(1)
	, multiplier(1)
	, factorType(FACTOR_MULTIPLY)
	, start(0)
	, end(0)
	, strStart("")
	, strEnd("")
{
}

CSubdivideDialog::~CSubdivideDialog()
{
}

void CSubdivideDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// controls
	//
	::DDX_Control(pDX, IDC_SECTIONS_SPIN, spinNumSubInterval);
	::DDX_Control(pDX, IDC_MULT_DIV_COMBO, factorCombo);
	//::DDX_Control(pDX, IDC_START_EDIT, startEdit);
	//::DDX_Control(pDX, IDC_END_EDIT, endEdit);

	// start
	//
	if (pDX->m_bSaveAndValidate)
	{
		::DDX_Text(pDX, IDC_START_EDIT, this->start);
	}
	else
	{
		::DDX_Text(pDX, IDC_START_EDIT, this->strStart);
	}

	// end
	//
	if (pDX->m_bSaveAndValidate)
	{
		::DDX_Text(pDX, IDC_END_EDIT, this->end);
		if (this->end <= this->start)
		{
			::AfxMessageBox("The end of the interval must be greater than the starting value.");
			pDX->Fail();
		}
	}
	else
	{
		::DDX_Text(pDX, IDC_END_EDIT, this->strEnd);
	}

	// numSubInterval
	//
	::DDX_Text(pDX, IDC_SECTIONS_EDIT, this->numSubInterval);
	if (pDX->m_bSaveAndValidate)
	{
		if (this->numSubInterval < 1)
		{
			::AfxMessageBox("The number of subintervals must be greater than 0.");
			pDX->Fail();
		}
		::DDV_MinMaxInt(pDX, this->numSubInterval, 1, INT_MAX);
	}

	// multiplier
	//
	::DDX_Text(pDX, IDC_FACTOR_EDIT, this->multiplier);
	if (pDX->m_bSaveAndValidate)
	{
		if (this->multiplier <= 0.0)
		{
			::AfxMessageBox("The multiplication or division factor must be positive.");
		}
	}

	// multiply or divide
	//
	if (pDX->m_bSaveAndValidate)
	{
		this->factorType = this->GetFactorType();
	}
	else
	{
		this->SetFactorType(this->factorType);
	}

	if (!pDX->m_bSaveAndValidate)
	{
		this->UpdateIntervals();
	}
}


BEGIN_MESSAGE_MAP(CSubdivideDialog, CDialog)
	ON_EN_CHANGE(IDC_FACTOR_EDIT, OnEnChangeFactorEdit)
	ON_EN_CHANGE(IDC_SECTIONS_EDIT, OnEnChangeSectionsEdit)
	ON_CBN_SELCHANGE(IDC_MULT_DIV_COMBO, OnCbnSelchangeMultDivCombo)
	ON_EN_CHANGE(IDC_START_EDIT, OnEnChangeStartEdit)
	ON_EN_CHANGE(IDC_END_EDIT, OnEnChangeEndEdit)
END_MESSAGE_MAP()


// CSubdivideDialog message handlers

BOOL CSubdivideDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->spinNumSubInterval.SetRange32(1, INT_MAX);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

CSubdivideDialog::FactorType CSubdivideDialog::GetFactorType()const
{
	FactorType type;
	int n = this->factorCombo.GetCurSel();
	switch (n)
	{
	case 0:
		type = FACTOR_MULTIPLY;
		break;
	case 1:
		type = FACTOR_DIVIDE;
		break;
	default:
		type = FACTOR_MULTIPLY;
		ASSERT(FALSE);
	}
	return type;
}

void CSubdivideDialog::SetFactorType(CSubdivideDialog::FactorType type)
{
	if (type == FACTOR_MULTIPLY)
	{
		this->factorCombo.SetCurSel(0);
	}
	else if (type == FACTOR_DIVIDE)
	{
		this->factorCombo.SetCurSel(1);
	}
}

void CSubdivideDialog::UpdateIntervals()
{
	double start;
	double end;

	try
	{
		start = CUtilities::GetDlgItemDouble(this, IDC_START_EDIT);
		end = CUtilities::GetDlgItemDouble(this, IDC_END_EDIT);
		if (end <= start) throw CUtilities::ParseException();

		CString str;
		str.Format("%g", end - start);
		this->SetDlgItemText(IDC_INTERVAL_STATIC, str);
	}
	catch (CUtilities::ParseException)
	{
		this->SetDlgItemText(IDC_INTERVAL_STATIC, "?");
		this->SetDlgItemText(IDC_START_WIDTH_STATIC, "?");
		this->SetDlgItemText(IDC_END_WIDTH_STATIC, "?");
		return;
	}

	try
	{
		int numSubInterval = this->GetDlgItemInt(IDC_SECTIONS_EDIT);
		if (numSubInterval <= 0) throw CUtilities::ParseException();

		double multiplier = CUtilities::GetDlgItemDouble(this, IDC_FACTOR_EDIT);
		if (multiplier <= 0.0) throw CUtilities::ParseException();
		if (this->GetFactorType() == FACTOR_DIVIDE) multiplier = 1/multiplier;

		CString str;

		std::vector<double> values;
		CUtilities::Subdivide(values, start, end, numSubInterval, multiplier);

		str.Format("%g", values[1] - values[0]);
		this->SetDlgItemText(IDC_START_WIDTH_STATIC, str);

		str.Format("%g", values[values.size() - 1] - values[values.size() - 2]);
		this->SetDlgItemText(IDC_END_WIDTH_STATIC, str);
	}
	catch (CUtilities::ParseException)
	{
		this->SetDlgItemText(IDC_START_WIDTH_STATIC, "?");
		this->SetDlgItemText(IDC_END_WIDTH_STATIC, "?");
	}
}

void CSubdivideDialog::OnEnChangeFactorEdit()
{
	this->UpdateIntervals();
}

void CSubdivideDialog::OnEnChangeSectionsEdit()
{
	this->UpdateIntervals();
}

void CSubdivideDialog::OnCbnSelchangeMultDivCombo()
{
	this->UpdateIntervals();
}

void CSubdivideDialog::OnEnChangeStartEdit()
{
	this->UpdateIntervals();
}

void CSubdivideDialog::OnEnChangeEndEdit()
{
	this->UpdateIntervals();
}
