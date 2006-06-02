// GridCoarsenPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "GridCoarsenPage.h"

#include "WPhastDoc.h"
#include "GridElementsSelector.h"
#include "GridActor.h"
#include "GridCoarsenAction.h"
#include "ModelessPropertySheet.h"


// CGridCoarsenPage dialog

IMPLEMENT_DYNAMIC(CGridCoarsenPage, CPropertyPage)
CGridCoarsenPage::CGridCoarsenPage()
	: CPropertyPage(CGridCoarsenPage::IDD)
	, Document(0)
	, Actor(0)
	, Widget(0)
	, NeedAction(false)
{
	for (int i = 0; i < 3; ++i)
	{
		this->MinOneBased[i] = 1;
		this->MaxOneBased[i] = 1;
		this->Parts[i]       = 1;
		this->AxisEnabled[i] = TRUE;
	}
}

CGridCoarsenPage::~CGridCoarsenPage()
{
}

void CGridCoarsenPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	if (this->m_bFirstSetActive)
	{
		this->CheckDlgButton(IDC_CHECK_X2, BST_CHECKED);
		this->CheckDlgButton(IDC_CHECK_Y2, BST_CHECKED);
		this->CheckDlgButton(IDC_CHECK_Z2, BST_CHECKED);
	}

	DDX_Check(pDX, IDC_CHECK_X2, this->AxisEnabled[0]);
	DDX_Check(pDX, IDC_CHECK_Y2, this->AxisEnabled[1]);
	DDX_Check(pDX, IDC_CHECK_Z2, this->AxisEnabled[2]);


	// X
	//
	if (pDX->m_bSaveAndValidate && this->AxisEnabled[0])
	{
		DDX_Text(pDX, IDC_EDIT_MIN_I, this->MinOneBased[0]);
		DDV_MinMaxInt(pDX, this->MinOneBased[0], 1, this->GridKeyword.m_grid[0].count_coord);

		DDX_Text(pDX, IDC_EDIT_MAX_I, this->MaxOneBased[0]);
		DDV_MinMaxInt(pDX, this->MinOneBased[0], 1, this->GridKeyword.m_grid[0].count_coord);

		DDX_Text(pDX, IDC_EDIT_PARTS_I, this->Parts[0]);
		DDV_MinMaxInt(pDX, this->Parts[0], 1, INT_MAX);
	}
	else if (!pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_EDIT_MIN_I, this->MinOneBased[0]);
		DDX_Text(pDX, IDC_EDIT_MAX_I, this->MaxOneBased[0]);
		DDX_Text(pDX, IDC_EDIT_PARTS_I, this->Parts[0]);
	}

	// Y
	//
	if (pDX->m_bSaveAndValidate && this->AxisEnabled[1])
	{
		DDX_Text(pDX, IDC_EDIT_MIN_J, this->MinOneBased[1]);
		DDV_MinMaxInt(pDX, this->MinOneBased[1], 1, this->GridKeyword.m_grid[1].count_coord);

		DDX_Text(pDX, IDC_EDIT_MAX_J, this->MaxOneBased[1]);
		DDV_MinMaxInt(pDX, this->MinOneBased[1], 1, this->GridKeyword.m_grid[1].count_coord);

		DDX_Text(pDX, IDC_EDIT_PARTS_J, this->Parts[1]);
		DDV_MinMaxInt(pDX, this->Parts[1], 1, INT_MAX);
	}
	else if (!pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_EDIT_MIN_J, this->MinOneBased[1]);
		DDX_Text(pDX, IDC_EDIT_MAX_J, this->MaxOneBased[1]);
		DDX_Text(pDX, IDC_EDIT_PARTS_J, this->Parts[1]);
	}

	// Z
	//
	if (pDX->m_bSaveAndValidate && this->AxisEnabled[1])
	{
		DDX_Text(pDX, IDC_EDIT_MIN_K, this->MinOneBased[2]);
		DDV_MinMaxInt(pDX, this->MinOneBased[2], 1, this->GridKeyword.m_grid[2].count_coord);

		DDX_Text(pDX, IDC_EDIT_MAX_K, this->MaxOneBased[2]);
		DDV_MinMaxInt(pDX, this->MinOneBased[2], 1, this->GridKeyword.m_grid[2].count_coord);

		DDX_Text(pDX, IDC_EDIT_PARTS_K, this->Parts[2]);
		DDV_MinMaxInt(pDX, this->Parts[2], 1, INT_MAX);
	}
	else if (!pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_EDIT_MIN_K, this->MinOneBased[2]);
		DDX_Text(pDX, IDC_EDIT_MAX_K, this->MaxOneBased[2]);
		DDX_Text(pDX, IDC_EDIT_PARTS_K, this->Parts[2]);
	}

	// spinners
	//

	DDX_Control(pDX, IDC_SPIN_MIN_I, spinMin[0]);
	DDX_Control(pDX, IDC_SPIN_MAX_I, spinMax[0]);
	DDX_Control(pDX, IDC_SPIN_PARTS_I, spinParts[0]);

	DDX_Control(pDX, IDC_SPIN_MIN_J, spinMin[1]);
	DDX_Control(pDX, IDC_SPIN_MAX_J, spinMax[1]);
	DDX_Control(pDX, IDC_SPIN_PARTS_J, spinParts[1]);

	DDX_Control(pDX, IDC_SPIN_MIN_K, spinMin[2]);
	DDX_Control(pDX, IDC_SPIN_MAX_K, spinMax[2]);
	DDX_Control(pDX, IDC_SPIN_PARTS_K, spinParts[2]);
}

void CGridCoarsenPage::SetUnits(const CUnits &units)
{
	this->Units = units;
}

CUnits CGridCoarsenPage::GetUnits()const
{
	return this->Units;
}

void CGridCoarsenPage::SetProperties(const CGridKeyword &gridKeyword)
{
	this->GridKeyword = gridKeyword;
}

void CGridCoarsenPage::GetProperties(CGridKeyword &gridKeyword)const
{
	gridKeyword = this->GridKeyword;
}

void CGridCoarsenPage::SetDocument(CWPhastDoc *document)
{
	this->Document = document;
}

CWPhastDoc* CGridCoarsenPage::GetDocument()const
{
	return this->Document;
}

void CGridCoarsenPage::SetActor(CGridActor *actor)
{
	this->Actor = actor;
	this->Actor->GetGridKeyword(this->OriginalGridKeyword);
}

CGridActor* CGridCoarsenPage::GetActor()const
{
	return this->Actor;
}

void CGridCoarsenPage::SetWidget(CGridElementsSelector *widget)
{
	this->Widget = widget;
}

CGridElementsSelector* CGridCoarsenPage::GetWidget()const
{
	return this->Widget;
}

void CGridCoarsenPage::SetMin(int min[3])
{
	for (int i = 0; i < 3; ++i)
	{
		this->MinOneBased[i] = min[i] + 1;
	}
}

void CGridCoarsenPage::GetMin(int min[3])const
{
	for (int i = 0; i < 3; ++i)
	{
		min[i] = min(this->MinOneBased[i], this->MaxOneBased[i]) - 1;
	}
}

void CGridCoarsenPage::SetMax(int max[3])
{
	for (int i = 0; i < 3; ++i)
	{
		this->MaxOneBased[i] = max[i] + 1;
	}
}

void CGridCoarsenPage::GetMax(int max[3])const
{
	for (int i = 0; i < 3; ++i)
	{
		max[i] = max(this->MinOneBased[i], this->MaxOneBased[i]) - 1;
	}
}

BEGIN_MESSAGE_MAP(CGridCoarsenPage, CPropertyPage)
	ON_EN_CHANGE(IDC_EDIT_MIN_I, OnEnChangeEditMinI)
	ON_EN_CHANGE(IDC_EDIT_MAX_I, OnEnChangeEditMaxI)
	ON_EN_CHANGE(IDC_EDIT_PARTS_I, OnEnChangeEditPartsI)

	ON_EN_CHANGE(IDC_EDIT_MIN_J, OnEnChangeEditMinJ)
	ON_EN_CHANGE(IDC_EDIT_MAX_J, OnEnChangeEditMaxJ)
	ON_EN_CHANGE(IDC_EDIT_PARTS_J, OnEnChangeEditPartsJ)

	ON_EN_CHANGE(IDC_EDIT_MIN_K, OnEnChangeEditMinK)
	ON_EN_CHANGE(IDC_EDIT_MAX_K, OnEnChangeEditMaxK)
	ON_EN_CHANGE(IDC_EDIT_PARTS_K, OnEnChangeEditPartsK)

	ON_BN_CLICKED(IDC_CHECK_X2, OnBnClickedCheckX)
	ON_BN_CLICKED(IDC_CHECK_Y2, OnBnClickedCheckY)
	ON_BN_CLICKED(IDC_CHECK_Z2, OnBnClickedCheckZ)
END_MESSAGE_MAP()


// CGridCoarsenPage message handlers

BOOL CGridCoarsenPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add extra initialization here
	this->UpdateSpinners();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGridCoarsenPage::OnEnChangeEditMinI()
{
	const int id = IDC_EDIT_MIN_I;
	const int c = 0;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT i = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated)
		{
			this->MinOneBased[c] = i;
			if (this->Widget)
			{
				int ibounds[6];
				for (int i = 0; i < 3; ++i)
				{
					ibounds[2*i] = this->MinOneBased[i] - 1;
					ibounds[2*i + 1] = this->MaxOneBased[i] - 1;
				}
				this->Widget->SetIBounds(ibounds);
			}
		}
	}
}

void CGridCoarsenPage::OnEnChangeEditMinJ()
{
	const int id = IDC_EDIT_MIN_J;
	const int c = 1;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT i = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated)
		{
			this->MinOneBased[c] = i;
			if (this->Widget)
			{
				int ibounds[6];
				for (int i = 0; i < 3; ++i)
				{
					ibounds[2*i] = this->MinOneBased[i] - 1;
					ibounds[2*i + 1] = this->MaxOneBased[i] - 1;
				}
				this->Widget->SetIBounds(ibounds);
			}
		}
	}
}

void CGridCoarsenPage::OnEnChangeEditMinK()
{
	const int id = IDC_EDIT_MIN_K;
	const int c = 2;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT i = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated)
		{
			this->MinOneBased[c] = i;
			if (this->Widget)
			{
				int ibounds[6];
				for (int i = 0; i < 3; ++i)
				{
					ibounds[2*i] = this->MinOneBased[i] - 1;
					ibounds[2*i + 1] = this->MaxOneBased[i] - 1;
				}
				this->Widget->SetIBounds(ibounds);
			}
		}
	}
}


void CGridCoarsenPage::OnEnChangeEditMaxI()
{
	const int id = IDC_EDIT_MAX_I;
	const int c = 0;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT i = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated)
		{
			this->MaxOneBased[c] = i;
			if (this->Widget)
			{
				int ibounds[6];
				for (int i = 0; i < 3; ++i)
				{
					ibounds[2*i] = this->MinOneBased[i] - 1;
					ibounds[2*i + 1] = this->MaxOneBased[i] - 1;
				}
				this->Widget->SetIBounds(ibounds);
			}
		}
	}
}

void CGridCoarsenPage::OnEnChangeEditMaxJ()
{
	const int id = IDC_EDIT_MAX_J;
	const int c = 1;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT i = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated)
		{
			this->MaxOneBased[c] = i;
			if (this->Widget)
			{
				int ibounds[6];
				for (int i = 0; i < 3; ++i)
				{
					ibounds[2*i] = this->MinOneBased[i] - 1;
					ibounds[2*i + 1] = this->MaxOneBased[i] - 1;
				}
				this->Widget->SetIBounds(ibounds);
			}
		}
	}
}

void CGridCoarsenPage::OnEnChangeEditMaxK()
{
	const int id = IDC_EDIT_MAX_K;
	const int c = 2;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT i = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated)
		{
			this->MaxOneBased[c] = i;
			if (this->Widget)
			{
				int ibounds[6];
				for (int i = 0; i < 3; ++i)
				{
					ibounds[2*i] = this->MinOneBased[i] - 1;
					ibounds[2*i + 1] = this->MaxOneBased[i] - 1;
				}
				this->Widget->SetIBounds(ibounds);
			}
		}
	}
}

void CGridCoarsenPage::OnEnChangeEditPartsI()
{
	const int id = IDC_EDIT_PARTS_I;
	const int c = 0;
	int CheckIDs[3] = 
	{
		IDC_CHECK_X2,
		IDC_CHECK_Y2,
		IDC_CHECK_Z2,
	};

	bool ActionNeeded = false;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT parts = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated && parts > 0)
		{
			this->Parts[c] = parts;
			for (int i = 0; i < 3; ++i)
			{
				if (this->Parts[i] > 1 && this->IsDlgButtonChecked(CheckIDs[i]) == BST_CHECKED)
				{
					ActionNeeded = true;
					break;
				}
			}
			if (ActionNeeded)
			{
				this->SetModified(TRUE);
				this->NeedAction = true;
			}
			else
			{
				this->SetModified(FALSE);
				this->NeedAction = false;
			}
		}
	}
}

void CGridCoarsenPage::OnEnChangeEditPartsJ()
{
	const int id = IDC_EDIT_PARTS_J;
	const int c = 1;
	int CheckIDs[3] = 
	{
		IDC_CHECK_X2,
		IDC_CHECK_Y2,
		IDC_CHECK_Z2,
	};

	bool ActionNeeded = false;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT parts = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated && parts > 0)
		{
			this->Parts[c] = parts;
			for (int i = 0; i < 3; ++i)
			{
				if (this->Parts[i] > 1 && this->IsDlgButtonChecked(CheckIDs[i]) == BST_CHECKED)
				{
					ActionNeeded = true;
					break;
				}
			}
			if (ActionNeeded)
			{
				this->SetModified(TRUE);
				this->NeedAction = true;
			}
			else
			{
				this->SetModified(FALSE);
				this->NeedAction = false;
			}
		}
	}
}

void CGridCoarsenPage::OnEnChangeEditPartsK()
{
	const int id = IDC_EDIT_PARTS_K;
	const int c = 2;
	int CheckIDs[3] = 
	{
		IDC_CHECK_X2,
		IDC_CHECK_Y2,
		IDC_CHECK_Z2,
	};

	bool ActionNeeded = false;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT parts = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated && parts > 0)
		{
			this->Parts[c] = parts;
			for (int i = 0; i < 3; ++i)
			{
				if (this->Parts[i] > 1 && this->IsDlgButtonChecked(CheckIDs[i]) == BST_CHECKED)
				{
					ActionNeeded = true;
					break;
				}
			}
			if (ActionNeeded)
			{
				this->SetModified(TRUE);
				this->NeedAction = true;
			}
			else
			{
				this->SetModified(FALSE);
				this->NeedAction = false;
			}
		}
	}
}

BOOL CGridCoarsenPage::OnApply()
{
	TRACE("In CGridCoarsenPage::OnApply\n");
	ASSERT(this->Document);
	ASSERT(this->Actor);
	CWaitCursor wait;
	if (this->NeedAction && this->UpdateData(TRUE))
	{
		if (CModelessPropertySheet *sheet = reinterpret_cast<CModelessPropertySheet*>(this->GetParent()))
		{
			int min[3];
			int max[3];
			int new_ibounds[6];
			int parts[3];
			for (int i = 0; i < 3; ++i)
			{
				min[i] = this->MinOneBased[i] - 1;
				max[i] = this->MaxOneBased[i] - 1;
				new_ibounds[2*i]     = min[i];
				new_ibounds[2*i + 1] = max[i];
				if (this->AxisEnabled[i])
				{
					parts[i] = this->Parts[i];
				}
				else
				{
					parts[i] = 1;
				}
				for (int m = min[i]; m < max[i]; ++m)
				{
					if ((m - min[i]) % (parts[i]))
					{
						--new_ibounds[2*i + 1];
					}
				}
			}

			// Undo / Redo
			CGridKeyword gridKeyword;
			this->Actor->GetGridKeyword(gridKeyword);
			CGridCoarsenAction* action = new CGridCoarsenAction(this->Document, this->Actor, min, max, parts, gridKeyword);
			action->Apply();
			sheet->AddAction(action);

			// update gridKeyword
			this->Actor->GetGridKeyword(this->GridKeyword);

			// update element selector
			this->Widget->UpdateGrid();
			this->Widget->SetIBounds(new_ibounds);

			// update maxs
			this->UpdateSpinners();
		}
		this->NeedAction = false;
	}
	// return CPropertyPage::OnApply();
	BOOL b = CPropertyPage::OnApply();
	TRACE("In CGridCoarsenPage::OnApply\n");
	return b;
}

void CGridCoarsenPage::UpdateSpinners()
{
	for (int i = 0; i < 3; ++i)
	{
		this->spinMin[i].SetRange32(1, this->GridKeyword.m_grid[i].count_coord);
		this->spinMax[i].SetRange32(1, this->GridKeyword.m_grid[i].count_coord);

		this->spinParts[i].SetRange32(1, INT_MAX);
	}
}

void CGridCoarsenPage::OnCancel()
{
	TRACE("In CGridCoarsenPage::OnCancel\n");
	this->Actor->SetGridKeyword(this->OriginalGridKeyword, this->Units);
	this->Actor->UpdateNode();
	this->Document->UpdateAllViews(0);

	// Add your specialized code here and/or call the base class

	CPropertyPage::OnCancel();
	TRACE("Out CGridCoarsenPage::OnCancel\n");
}

void CGridCoarsenPage::PostNcDestroy()
{
	TRACE("In CGridCoarsenPage::PostNcDestroy\n");
	this->Document->EndModifyGrid();
	this->Document->UpdateAllViews(0);
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::PostNcDestroy();
	TRACE("Out CGridCoarsenPage::PostNcDestroy\n");
}

BOOL CGridCoarsenPage::OnSetActive()
{
	TRACE("In CGridCoarsenPage::OnSetActive\n");
	int ibounds[6];
	this->Widget->GetIBounds(ibounds);
	for (int i = 0; i < 3; ++i)
	{
		this->MinOneBased[i] = ibounds[2*i] + 1;
		this->MaxOneBased[i] = ibounds[2*i + 1] + 1;
		this->Parts[i]       = 1;
	}
	this->NeedAction = false;
	this->SetModified(FALSE);
	this->UpdateData(FALSE);

	//return CPropertyPage::OnSetActive();
	BOOL b = CPropertyPage::OnSetActive();
	TRACE("Out CGridCoarsenPage::OnSetActive\n");
	return b;
}

BOOL CGridCoarsenPage::OnKillActive()
{
	TRACE("In CGridCoarsenPage::OnKillActive\n");
	this->SetModified(FALSE);

	// TODO: Add your specialized code here and/or call the base class

	//return CPropertyPage::OnKillActive();
	BOOL b = CPropertyPage::OnKillActive();
	TRACE("Out CGridCoarsenPage::OnKillActive\n");
	return b;
}

void CGridCoarsenPage::OnBnClickedCheckX()
{
	int Ids[] = 
	{
		IDC_STATIC_SN_X,
		IDC_EDIT_MIN_I,
		IDC_SPIN_MIN_I,
		IDC_STATIC_EN_X,
		IDC_EDIT_MAX_I,
		IDC_SPIN_MAX_I,
		IDC_STATIC_ME_X,
		IDC_EDIT_PARTS_I,
		IDC_SPIN_PARTS_I,
        IDC_STATIC_E_X,
	};

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(this->IsDlgButtonChecked(IDC_CHECK_X2) == BST_CHECKED);
		}
	}

	int CheckIDs[3] = 
	{
		IDC_CHECK_X2,
		IDC_CHECK_Y2,
		IDC_CHECK_Z2,
	};
	bool ActionNeeded = false;
	for (int i = 0; i < 3; ++i)
	{
		if (this->Parts[i] > 1 && this->IsDlgButtonChecked(CheckIDs[i]) == BST_CHECKED)
		{
			ActionNeeded = true;
			break;
		}
	}
	if (ActionNeeded)
	{
		this->SetModified(TRUE);
		this->NeedAction = true;
	}
	else
	{
		this->SetModified(FALSE);
		this->NeedAction = false;
	}
}

void CGridCoarsenPage::OnBnClickedCheckY()
{
	int Ids[] = 
	{
		IDC_STATIC_SN_Y,
		IDC_EDIT_MIN_J,
		IDC_SPIN_MIN_J,
		IDC_STATIC_EN_Y,
		IDC_EDIT_MAX_J,
		IDC_SPIN_MAX_J,
		IDC_STATIC_ME_Y,
		IDC_EDIT_PARTS_J,
		IDC_SPIN_PARTS_J,
        IDC_STATIC_E_Y,
	};

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(this->IsDlgButtonChecked(IDC_CHECK_Y2) == BST_CHECKED);
		}
	}

	int CheckIDs[3] = 
	{
		IDC_CHECK_X2,
		IDC_CHECK_Y2,
		IDC_CHECK_Z2,
	};
	bool ActionNeeded = false;
	for (int i = 0; i < 3; ++i)
	{
		if (this->Parts[i] > 1 && this->IsDlgButtonChecked(CheckIDs[i]) == BST_CHECKED)
		{
			ActionNeeded = true;
			break;
		}
	}
	if (ActionNeeded)
	{
		this->SetModified(TRUE);
		this->NeedAction = true;
	}
	else
	{
		this->SetModified(FALSE);
		this->NeedAction = false;
	}
}

void CGridCoarsenPage::OnBnClickedCheckZ()
{
	int Ids[] = 
	{
		IDC_STATIC_SN_Z,
		IDC_EDIT_MIN_K,
		IDC_SPIN_MIN_K,
		IDC_STATIC_EN_Z,
		IDC_EDIT_MAX_K,
		IDC_SPIN_MAX_K,
		IDC_STATIC_ME_Z,
		IDC_EDIT_PARTS_K,
		IDC_SPIN_PARTS_K,
        IDC_STATIC_E_Z,
	};

	for (int i = 0; i < sizeof(Ids) / sizeof(Ids[0]); ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(Ids[i]))
		{
			pWnd->EnableWindow(this->IsDlgButtonChecked(IDC_CHECK_Z2) == BST_CHECKED);
		}
	}

	int CheckIDs[3] = 
	{
		IDC_CHECK_X2,
		IDC_CHECK_Y2,
		IDC_CHECK_Z2,
	};
	bool ActionNeeded = false;
	for (int i = 0; i < 3; ++i)
	{
		if (this->Parts[i] > 1 && this->IsDlgButtonChecked(CheckIDs[i]) == BST_CHECKED)
		{
			ActionNeeded = true;
			break;
		}
	}
	if (ActionNeeded)
	{
		this->SetModified(TRUE);
		this->NeedAction = true;
	}
	else
	{
		this->SetModified(FALSE);
		this->NeedAction = false;
	}
}
