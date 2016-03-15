// SamplePg.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "SamplePg.h"


// CSamplePg dialog

IMPLEMENT_DYNAMIC(CSamplePg, CPropertyPage)

CSamplePg::CSamplePg()
: CPropertyPage(CSamplePg::IDD)
{
	for (int i = 0; i < 3; ++i)
	{
		this->spinMax[i]     = INT_MAX - 1;
		this->thin_grid[i]   = 0;
		this->AxisEnabled[i] = FALSE;
	}
}

CSamplePg::~CSamplePg()
{
}

void CSamplePg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (this->m_bFirstSetActive)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (this->thin_grid[i] > 0)
			{
				this->AxisEnabled[i] = TRUE;
			}
			else if (this->thin_grid[i] == 0)
			{
				this->thin_grid[i] = 1;
			}
		}
	}

	DDX_Check(pDX, IDC_CHECK_X2, this->AxisEnabled[0]);
	DDX_Check(pDX, IDC_CHECK_Y2, this->AxisEnabled[1]);
	DDX_Check(pDX, IDC_CHECK_Z2, this->AxisEnabled[2]);

	int grid[3];

	int idx;
	int idc;

	// X
	idx = 0;
	idc = IDC_EDIT_PARTS_I;
	if (pDX->m_bSaveAndValidate)
	{
		if (this->AxisEnabled[idx])
		{
			DDX_Text(pDX, idc, grid[idx]);
			DDV_MinMaxInt(pDX, grid[idx], 1, this->spinMax[idx]);
			this->thin_grid[idx] = grid[idx];
		}
		else
		{
			this->thin_grid[idx] = 0;
		}
	}
	else if (!pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, idc, this->thin_grid[idx]);
	}

	// Y
	idx = 1;
	idc = IDC_EDIT_PARTS_J;
	if (pDX->m_bSaveAndValidate)
	{
		if (this->AxisEnabled[idx])
		{
			DDX_Text(pDX, idc, grid[idx]);
			DDV_MinMaxInt(pDX, grid[idx], 1, this->spinMax[idx]);
			this->thin_grid[idx] = grid[idx];
		}
		else
		{
			this->thin_grid[idx] = 0;
		}
	}
	else if (!pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, idc, this->thin_grid[idx]);
	}

	// Z
	idx = 2;
	idc = IDC_EDIT_PARTS_K;
	if (pDX->m_bSaveAndValidate)
	{
		if (this->AxisEnabled[idx])
		{
			DDX_Text(pDX, idc, grid[idx]);
			DDV_MinMaxInt(pDX, grid[idx], 1, this->spinMax[idx]);
			this->thin_grid[idx] = grid[idx];
		}
		else
		{
			this->thin_grid[idx] = 0;
		}
	}
	else if (!pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, idc, this->thin_grid[idx]);
	}

	// spinners
	DDX_Control(pDX, IDC_SPIN_PARTS_I, this->spinFreq[0]);
	DDX_Control(pDX, IDC_SPIN_PARTS_J, this->spinFreq[1]);
	DDX_Control(pDX, IDC_SPIN_PARTS_K, this->spinFreq[2]);

	if (this->m_bFirstSetActive)
	{
		 this->OnBnClickedCheckX2();
		 this->OnBnClickedCheckY2();
		 this->OnBnClickedCheckZ2();
	}
}


BEGIN_MESSAGE_MAP(CSamplePg, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_X2, &CSamplePg::OnBnClickedCheckX2)
	ON_BN_CLICKED(IDC_CHECK_Y2, &CSamplePg::OnBnClickedCheckY2)
	ON_BN_CLICKED(IDC_CHECK_Z2, &CSamplePg::OnBnClickedCheckZ2)
END_MESSAGE_MAP()


// CSamplePg message handlers

BOOL CSamplePg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add extra initialization here
	this->UpdateSpinners();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSamplePg::UpdateSpinners()
{
	for (int i = 0; i < 3; ++i)
	{
		this->spinFreq[i].SetRange32(1, this->spinMax[i]);
	}
}

void CSamplePg::OnBnClickedCheckX2()
{
	int Ids[] = 
	{
		IDC_STATIC_ME_X,
		IDC_EDIT_PARTS_I,
		IDC_SPIN_PARTS_I
	};

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(this->IsDlgButtonChecked(IDC_CHECK_X2) == BST_CHECKED);
		}
	}
}

void CSamplePg::OnBnClickedCheckY2()
{
	int Ids[] = 
	{
		IDC_STATIC_ME_Y,
		IDC_EDIT_PARTS_J,
		IDC_SPIN_PARTS_J
	};

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(this->IsDlgButtonChecked(IDC_CHECK_Y2) == BST_CHECKED);
		}
	}
}

void CSamplePg::OnBnClickedCheckZ2()
{
	int Ids[] = 
	{
		IDC_STATIC_ME_Z,
		IDC_EDIT_PARTS_K,
		IDC_SPIN_PARTS_K
	};

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(this->IsDlgButtonChecked(IDC_CHECK_Z2) == BST_CHECKED);
		}
	}
}

