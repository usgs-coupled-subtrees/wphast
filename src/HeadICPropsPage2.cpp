// HeadICPropsPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "HeadICPropsPage2.h"

#include "Global.h"

// CHeadICPropsPage2 dialog

const TCHAR HEAD[] = _T("Head");

const int SELECTED = 0;
const int SINGLE = 0;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CHeadICPropsPage2, CPropsPropertyPage)

CHeadICPropsPage2::CHeadICPropsPage2()
	: CPropsPropertyPage(CHeadICPropsPage2::IDD)
	, HeadProperty(this, true, false)
	, Default(false)
{
	TRACE("In %s\n", __FUNCTION__);

	// load property descriptions
	CGlobal::LoadRTFString(this->m_sDescriptionRTF, IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sHeadRTF,        IDR_IC_HEAD_HEAD_RTF);

	TRACE("Out %s\n", __FUNCTION__);
}

CHeadICPropsPage2::~CHeadICPropsPage2()
{
	TRACE("In %s\n", __FUNCTION__);
	TRACE("Out %s\n", __FUNCTION__);
}

void CHeadICPropsPage2::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);
	CPropsPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	DDX_Control(pDX, IDC_PROP_TREE, this->TreeCtrl);

	if (this->TreeCtrl.GetCount() == 0)
	{
		this->HeadProperty.treeitem = this->TreeCtrl.InsertItem(HEAD, TVI_ROOT, TVI_LAST);

		// wrap richedit to window
		this->RichEditCtrl.SetTargetDevice(NULL, 0);

		this->ItemDDX = this->HeadProperty.treeitem;
		this->TreeCtrl.SelectItem(this->ItemDDX);

		this->TreeCtrl.ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);
	}

	DDX_GridControl(pDX, IDC_POINTS_GRID, this->PointsGrid);

	this->HeadProperty.SetPointsGrid(&this->PointsGrid);

	// description
	::DDX_Text(pDX, IDC_DESC_EDIT, this->Description);

	if (pDX->m_bSaveAndValidate)
	{
		// properties
		if (this->ItemDDX == this->HeadProperty.treeitem)
		{
			this->HeadProperty.DDV_SoftValidate();
			this->HeadProperty.DDX_Single(pDX, this->Default);
		}
		else
		{
			ASSERT(FALSE);
		}

	}
	else
	{
		// properties
		CGridTimeSeries::ShowSingleProperty(this, SW_SHOW);

		if (this->ItemDDX == this->HeadProperty.treeitem)
		{
			this->HeadProperty.DDX_Single(pDX, this->Default);
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CHeadICPropsPage2::SetProperties(const CHeadIC& rHeadIC)
{
	TRACE("In %s\n", __FUNCTION__);

	// copy grid_elt
	this->HeadIC = rHeadIC;

	// single properties
	this->HeadProperty.SetProperty(rHeadIC.head);

	TRACE("Out %s\n", __FUNCTION__);
}

void CHeadICPropsPage2::GetProperties(CHeadIC& rHeadIC)const
{
	TRACE("In %s\n", __FUNCTION__);

	rHeadIC = this->HeadIC;

	// single properties
	this->HeadProperty.GetProperty(rHeadIC.head);

	TRACE("Out %s\n", __FUNCTION__);
}

BEGIN_MESSAGE_MAP(CHeadICPropsPage2, CPropsPropertyPage)
	// IDC_PROP_TREE
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTreeSelChanging)
	ON_NOTIFY(TVN_SELCHANGED,  IDC_PROP_TREE, OnTreeSelChanged)

	// IDC_DESC_EDIT
	ON_EN_SETFOCUS(IDC_DESC_EDIT, OnEnSetfocusDescEdit)

	// IDC_BUTTON_XYZ
	ON_BN_CLICKED(IDC_BUTTON_XYZ, OnBnClickedButtonXYZ)

	// IDC_COMBO_PROPTYPE
	ON_CBN_SELCHANGE(IDC_COMBO_PROPTYPE, OnCbnSelchangeComboProptype)

	// IDC_CHECK_MIXTURE
	ON_BN_CLICKED(IDC_CHECK_MIXTURE, OnBnClickedCheckMixture)

	// DDX failure
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
END_MESSAGE_MAP()


// CHeadICPropsPage2 message handlers

void CHeadICPropsPage2::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	TRACE("In %s\n", __FUNCTION__);
	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
	this->ItemDDX = pTvn->itemOld.hItem;
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->HeadProperty.treeitem)
		{
			this->HeadProperty.DDV_SoftValidate();
		}
		else
		{
			ASSERT(FALSE);
		}


		// force CInPlaceXXX to lose focus
		this->TreeCtrl.SetFocus();

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

void CHeadICPropsPage2::OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
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
			COMPARE_SET(HEAD, this->m_sHeadRTF);
		}
	}
	if (this->TreeCtrl.GetSafeHwnd())
	{
		this->TreeCtrl.SetFocus();
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CHeadICPropsPage2::OnEnSetfocusDescEdit()
{
	TRACE("In %s\n", __FUNCTION__);
	this->RichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
	TRACE("Out %s\n", __FUNCTION__);
}

LRESULT CHeadICPropsPage2::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	CWnd* pFocus = (CWnd*)wParam;
	if (pFocus && pFocus->GetSafeHwnd())
	{
		ASSERT_KINDOF(CWnd, pFocus);
		pFocus->SetFocus();
	}
	return 0;
}

void CHeadICPropsPage2::OnBnClickedButtonXYZ()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->HeadProperty.treeitem)
		{
			this->HeadProperty.OnBnClickedButtonXYZ();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

void CHeadICPropsPage2::SetUnits(const CUnits &u)
{
	this->HeadProperty.SetUnits(u);
}

void CHeadICPropsPage2::OnCbnSelchangeComboProptype()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->HeadProperty.treeitem)
		{
			this->HeadProperty.OnCbnSelchangeComboProptype();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

void CHeadICPropsPage2::OnBnClickedCheckMixture()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->HeadProperty.treeitem)
		{
			this->HeadProperty.OnBnClickedCheckMixture();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}
