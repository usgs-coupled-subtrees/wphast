// SparsifyGridPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "SparsifyGridPage.h"

#include "WPhastDoc.h"
#include "GridElementsSelector.h"
#include "GridActor.h"
#include "GridSubDivideAction.h"
#include "ModelessPropertySheet.h"

// CSparsifyGridPage dialog

IMPLEMENT_DYNAMIC(CSparsifyGridPage, CPropertyPage)
CSparsifyGridPage::CSparsifyGridPage()
	: CPropertyPage(CSparsifyGridPage::IDD)
	, Document(0)
	, Actor(0)
	, Widget(0)
	, NeedAction(false)
{
	for (int i = 0; i < 3; ++i)
	{
		this->MinOneBased[i] = 1;
		this->MaxOneBased[i] = 1;
		this->Parts[i]       = 0;
	}
}

CSparsifyGridPage::~CSparsifyGridPage()
{
}

void CSparsifyGridPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	CString str;

	// X
	//

	DDX_Text(pDX, IDC_EDIT_MIN_I, this->MinOneBased[0]);
	DDV_MinMaxInt(pDX, this->MinOneBased[0], 1, this->GridKeyword.m_grid[0].count_coord);

	DDX_Text(pDX, IDC_EDIT_MAX_I, this->MaxOneBased[0]);
	DDV_MinMaxInt(pDX, this->MinOneBased[0], 1, this->GridKeyword.m_grid[0].count_coord);

	DDX_Text(pDX, IDC_EDIT_PARTS_I, this->Parts[0]);
	DDV_MinMaxInt(pDX, this->Parts[0], 0, INT_MAX);

	str.Format("of every %d nodes", this->Parts[0] + 1);
	DDX_Text(pDX, IDC_STATIC_NODES_X, str);

	// Y
	//

	DDX_Text(pDX, IDC_EDIT_MIN_J, this->MinOneBased[1]);
	DDV_MinMaxInt(pDX, this->MinOneBased[1], 1, this->GridKeyword.m_grid[1].count_coord);

	DDX_Text(pDX, IDC_EDIT_MAX_J, this->MaxOneBased[1]);
	DDV_MinMaxInt(pDX, this->MinOneBased[1], 1, this->GridKeyword.m_grid[1].count_coord);

	DDX_Text(pDX, IDC_EDIT_PARTS_J, this->Parts[1]);
	DDV_MinMaxInt(pDX, this->Parts[1], 0, INT_MAX);

	str.Format("of every %d nodes", this->Parts[1] + 1);
	DDX_Text(pDX, IDC_STATIC_NODES_Y, str);

	// Z
	//

	DDX_Text(pDX, IDC_EDIT_MIN_K, this->MinOneBased[2]);
	DDV_MinMaxInt(pDX, this->MinOneBased[2], 1, this->GridKeyword.m_grid[2].count_coord);

	DDX_Text(pDX, IDC_EDIT_MAX_K, this->MaxOneBased[2]);
	DDV_MinMaxInt(pDX, this->MinOneBased[2], 1, this->GridKeyword.m_grid[2].count_coord);

	DDX_Text(pDX, IDC_EDIT_PARTS_K, this->Parts[2]);
	DDV_MinMaxInt(pDX, this->Parts[2], 0, INT_MAX);

	str.Format("of every %d nodes", this->Parts[2] + 1);
	DDX_Text(pDX, IDC_STATIC_NODES_Z, str);

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

void CSparsifyGridPage::SetUnits(const CUnits &units)
{
	this->Units = units;
}

CUnits CSparsifyGridPage::GetUnits()const
{
	return this->Units;
}

void CSparsifyGridPage::SetProperties(const CGridKeyword &gridKeyword)
{
	this->GridKeyword = gridKeyword;
}

void CSparsifyGridPage::GetProperties(CGridKeyword &gridKeyword)const
{
	gridKeyword = this->GridKeyword;
}

void CSparsifyGridPage::SetDocument(CWPhastDoc *document)
{
	this->Document = document;
}

CWPhastDoc* CSparsifyGridPage::GetDocument()const
{
	return this->Document;
}

void CSparsifyGridPage::SetActor(CGridActor *actor)
{
	this->Actor = actor;
	this->Actor->GetGridKeyword(this->OriginalGridKeyword);
}

CGridActor* CSparsifyGridPage::GetActor()const
{
	return this->Actor;
}

void CSparsifyGridPage::SetWidget(CGridElementsSelector *widget)
{
	this->Widget = widget;
}

CGridElementsSelector* CSparsifyGridPage::GetWidget()const
{
	return this->Widget;
}

void CSparsifyGridPage::SetMin(int min[3])
{
	for (int i = 0; i < 3; ++i)
	{
		this->MinOneBased[i] = min[i] + 1;
	}
}

void CSparsifyGridPage::GetMin(int min[3])const
{
	for (int i = 0; i < 3; ++i)
	{
		min[i] = min(this->MinOneBased[i], this->MaxOneBased[i]) - 1;
	}
}

void CSparsifyGridPage::SetMax(int max[3])
{
	for (int i = 0; i < 3; ++i)
	{
		this->MaxOneBased[i] = max[i] + 1;
	}
}

void CSparsifyGridPage::GetMax(int max[3])const
{
	for (int i = 0; i < 3; ++i)
	{
		max[i] = max(this->MinOneBased[i], this->MaxOneBased[i]) - 1;
	}
}

BEGIN_MESSAGE_MAP(CSparsifyGridPage, CPropertyPage)
	ON_EN_CHANGE(IDC_EDIT_MIN_I, OnEnChangeEditMinI)
	ON_EN_CHANGE(IDC_EDIT_MAX_I, OnEnChangeEditMaxI)
	ON_EN_CHANGE(IDC_EDIT_PARTS_I, OnEnChangeEditPartsI)

	ON_EN_CHANGE(IDC_EDIT_MIN_J, OnEnChangeEditMinJ)
	ON_EN_CHANGE(IDC_EDIT_MAX_J, OnEnChangeEditMaxJ)
	ON_EN_CHANGE(IDC_EDIT_PARTS_J, OnEnChangeEditPartsJ)

	ON_EN_CHANGE(IDC_EDIT_MIN_K, OnEnChangeEditMinK)
	ON_EN_CHANGE(IDC_EDIT_MAX_K, OnEnChangeEditMaxK)
	ON_EN_CHANGE(IDC_EDIT_PARTS_K, OnEnChangeEditPartsK)

END_MESSAGE_MAP()


// CSparsifyGridPage message handlers

BOOL CSparsifyGridPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add extra initialization here
	this->UpdateSpinners();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSparsifyGridPage::OnEnChangeEditMinI()
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

void CSparsifyGridPage::OnEnChangeEditMinJ()
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

void CSparsifyGridPage::OnEnChangeEditMinK()
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


void CSparsifyGridPage::OnEnChangeEditMaxI()
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

void CSparsifyGridPage::OnEnChangeEditMaxJ()
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

void CSparsifyGridPage::OnEnChangeEditMaxK()
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

void CSparsifyGridPage::OnEnChangeEditPartsI()
{
	const int id = IDC_EDIT_PARTS_I;
	const int c = 0;
	const int static_id = IDC_STATIC_NODES_X;

	bool ActionNeeded = false;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT parts = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated)
		{
			this->Parts[c] = parts;
			for (int i = 0; i < 3; ++i)
			{
				if (this->Parts[i] > 0)
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
			if (CWnd *pWnd = this->GetDlgItem(static_id))
			{
				CString str;
// COMMENT: {8/31/2005 9:13:56 PM}				if (this->Parts[c] > 0)
// COMMENT: {8/31/2005 9:13:56 PM}				{
// COMMENT: {8/31/2005 9:13:56 PM}					str.Format("of every %d nodes", this->Parts[c] + 1);
// COMMENT: {8/31/2005 9:13:56 PM}				}
// COMMENT: {8/31/2005 9:13:56 PM}				else
// COMMENT: {8/31/2005 9:13:56 PM}				{
// COMMENT: {8/31/2005 9:13:56 PM}					str.Format("");
// COMMENT: {8/31/2005 9:13:56 PM}				}
				str.Format("of every %d nodes", this->Parts[c] + 1);
				pWnd->SetWindowText(str);
			}
		}
	}
}

void CSparsifyGridPage::OnEnChangeEditPartsJ()
{
	const int id = IDC_EDIT_PARTS_J;
	const int c = 1;
	const int static_id = IDC_STATIC_NODES_Y;

	bool ActionNeeded = false;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT i = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated)
		{
			this->Parts[c] = i;
			for (int i = 0; i < 3; ++i)
			{
				if (this->Parts[i] > 0)
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
			if (CWnd *pWnd = this->GetDlgItem(static_id))
			{
				CString str;
// COMMENT: {8/31/2005 9:13:56 PM}				if (this->Parts[c] > 0)
// COMMENT: {8/31/2005 9:13:56 PM}				{
// COMMENT: {8/31/2005 9:13:56 PM}					str.Format("of every %d nodes", this->Parts[c] + 1);
// COMMENT: {8/31/2005 9:13:56 PM}				}
// COMMENT: {8/31/2005 9:13:56 PM}				else
// COMMENT: {8/31/2005 9:13:56 PM}				{
// COMMENT: {8/31/2005 9:13:56 PM}					str.Format("");
// COMMENT: {8/31/2005 9:13:56 PM}				}
				str.Format("of every %d nodes", this->Parts[c] + 1);
				pWnd->SetWindowText(str);
			}
		}
	}
}

void CSparsifyGridPage::OnEnChangeEditPartsK()
{
	const int id = IDC_EDIT_PARTS_K;
	const int c = 2;
	const int static_id = IDC_STATIC_NODES_Z;

	bool ActionNeeded = false;

	if (!this->m_bFirstSetActive)
	{
		BOOL bTranslated;
		UINT i = this->GetDlgItemInt(id, &bTranslated, FALSE);
		if (bTranslated)
		{
			this->Parts[c] = i;
			for (int i = 0; i < 3; ++i)
			{
				if (this->Parts[i] > 0)
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
			if (CWnd *pWnd = this->GetDlgItem(static_id))
			{
				CString str;
// COMMENT: {8/31/2005 9:13:56 PM}				if (this->Parts[c] > 0)
// COMMENT: {8/31/2005 9:13:56 PM}				{
// COMMENT: {8/31/2005 9:13:56 PM}					str.Format("of every %d nodes", this->Parts[c] + 1);
// COMMENT: {8/31/2005 9:13:56 PM}				}
// COMMENT: {8/31/2005 9:13:56 PM}				else
// COMMENT: {8/31/2005 9:13:56 PM}				{
// COMMENT: {8/31/2005 9:13:56 PM}					str.Format("");
// COMMENT: {8/31/2005 9:13:56 PM}				}
				str.Format("of every %d nodes", this->Parts[c] + 1);
				pWnd->SetWindowText(str);
			}
		}
	}
}

BOOL CSparsifyGridPage::OnApply()
{
	TRACE("In CSparsifyGridPage::OnApply\n");
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
			for (int i = 0; i < 3; ++i)
			{
				min[i] = this->MinOneBased[i] - 1;
				max[i] = this->MaxOneBased[i] - 1;
				new_ibounds[2*i]     = min[i];
				new_ibounds[2*i + 1] = max[i];
				for (int m = min[i]; m < max[i]; ++m)
				{
					if ((m - min[i]) % (this->Parts[i] + 1))
					{
						--new_ibounds[2*i + 1];
					}
				}
			}
// COMMENT: {8/30/2005 7:35:34 PM}			CGridSubDivideAction* action = new CGridSubDivideAction(this->Document, this->Actor, min, max, this->Parts);
// COMMENT: {8/30/2005 7:35:34 PM}			action->Apply();
// COMMENT: {8/30/2005 7:35:34 PM}			sheet->AddAction(action);
			//{{
			{
				CGridKeyword gridKeyword;
				this->Actor->GetGridKeyword(gridKeyword);

				CUnits units;
				this->Document->GetUnits(units);

				for (int i = 0; i < 3; ++i)
				{
					//this->WasUniform[i] = (gridKeyword.m_grid[i].uniform != 0);
					//gridKeyword.m_grid[i].Coarsen(this->Min[i], this->Max[i], this->Parts[i]);
					gridKeyword.m_grid[i].Sparsify(min[i], max[i], this->Parts[i]);
				}
				this->Actor->SetGridKeyword(gridKeyword, units);
				this->Actor->UpdateNode();
				this->Document->UpdateAllViews(0);
			}
			//}}

			// update gridKeyword
			this->Actor->GetGridKeyword(this->GridKeyword);

			// update element selector
			this->Widget->UpdateGrid();
			this->Widget->SetIBounds(new_ibounds);

			// update maxs
			this->UpdateSpinners();

			// this->Parts updated in OnKillActive
// COMMENT: {8/31/2005 9:41:51 PM}			for (int i = 0; i < 3; ++i)
// COMMENT: {8/31/2005 9:41:51 PM}			{
// COMMENT: {8/31/2005 9:41:51 PM}				// Min and Max are updated by OnSetActive and the above call to SetIBounds
// COMMENT: {8/31/2005 9:41:51 PM}				this->Parts[i] = 0;
// COMMENT: {8/31/2005 9:41:51 PM}			}
// COMMENT: {8/31/2005 9:41:51 PM}			this->UpdateData(FALSE);
		}
		this->NeedAction = false;
	}
	// return CPropertyPage::OnApply();
	BOOL b = CPropertyPage::OnApply();
	TRACE("In CSparsifyGridPage::OnApply\n");
	return b;
}

void CSparsifyGridPage::UpdateSpinners()
{
	for (int i = 0; i < 3; ++i)
	{
		this->spinMin[i].SetRange32(1, this->GridKeyword.m_grid[i].count_coord);
		this->spinMax[i].SetRange32(1, this->GridKeyword.m_grid[i].count_coord);

		this->spinParts[i].SetRange32(0, INT_MAX);
// COMMENT: {8/31/2005 5:14:09 PM}		//{{
// COMMENT: {8/31/2005 5:14:09 PM}		int min = this->spinMin[i].GetPos32();
// COMMENT: {8/31/2005 5:14:09 PM}		int max = this->spinMax[i].GetPos32();
// COMMENT: {8/31/2005 5:14:09 PM}		this->spinParts[i].SetRange32(1, max - min);
// COMMENT: {8/31/2005 5:14:09 PM}		//}}
	}
}

void CSparsifyGridPage::OnCancel()
{
	TRACE("In CSparsifyGridPage::OnCancel\n");
	this->Actor->SetGridKeyword(this->OriginalGridKeyword, this->Units);
	this->Actor->UpdateNode();
	this->Document->UpdateAllViews(0);

	// Add your specialized code here and/or call the base class

	CPropertyPage::OnCancel();
	TRACE("Out CSparsifyGridPage::OnCancel\n");
}

void CSparsifyGridPage::PostNcDestroy()
{
	TRACE("In CSparsifyGridPage::PostNcDestroy\n");
	this->Document->EndModifyGrid();
	this->Document->UpdateAllViews(0);
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::PostNcDestroy();
	TRACE("Out CSparsifyGridPage::PostNcDestroy\n");
}

BOOL CSparsifyGridPage::OnSetActive()
{
	TRACE("In CSparsifyGridPage::OnSetActive\n");
	int ibounds[6];
	this->Widget->GetIBounds(ibounds);
	for (int i = 0; i < 3; ++i)
	{
		this->MinOneBased[i] = ibounds[2*i] + 1;
		this->MaxOneBased[i] = ibounds[2*i + 1] + 1;
		this->Parts[i]       = 0;
	}
	this->NeedAction = false;
	this->SetModified(FALSE);
	this->UpdateData(FALSE);

	//return CPropertyPage::OnSetActive();
	BOOL b = CPropertyPage::OnSetActive();
	TRACE("Out CSparsifyGridPage::OnSetActive\n");
	return b;
}

BOOL CSparsifyGridPage::OnKillActive()
{
	TRACE("In CSparsifyGridPage::OnKillActive\n");
	this->SetModified(FALSE);

	// TODO: Add your specialized code here and/or call the base class

	//return CPropertyPage::OnKillActive();
	BOOL b = CPropertyPage::OnKillActive();
	TRACE("Out CSparsifyGridPage::OnKillActive\n");
	return b;
}
