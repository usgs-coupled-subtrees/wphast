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

	if (this->TreeCtrl.GetCount() == 0)
	{
		// setup tree
		this->HeadProperty.treeitem = this->TreeCtrl.InsertItem(HEAD, TVI_ROOT, TVI_LAST);

		// setup tree selection
		this->ItemDDX = this->HeadProperty.treeitem;
		this->TreeCtrl.SelectItem(this->ItemDDX);
		this->TreeCtrl.ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);

		// set points grid for each propety
		this->HeadProperty.SetPointsGrid(&this->PointsGrid);
	}

	// description
	::DDX_Text(pDX, IDC_DESC_EDIT, this->Description);

	// properties
	if (pDX->m_bSaveAndValidate)
	{
		this->DDV_SoftValidate();
	}
	this->DDX_Single(pDX);

	TRACE("Out %s\n", __FUNCTION__);
}

void CHeadICPropsPage2::DDV_SoftValidate()
{
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
	}
}

void CHeadICPropsPage2::DDX_Single(CDataExchange* pDX)
{
	if (this->ItemDDX)
	{
		if (!pDX->m_bSaveAndValidate)
		{
			// display single
			CGridTimeSeries::ShowSingleProperty(this, SW_SHOW);
		}

		if (this->ItemDDX == this->HeadProperty.treeitem)
		{
			this->HeadProperty.DDX_Single(pDX, this->Default);
		}
		else
		{
			ASSERT(FALSE);
		}
	}
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
// COMMENT: {7/13/2009 7:27:08 PM}	// IDC_PROP_TREE
// COMMENT: {7/13/2009 7:27:08 PM}	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTreeSelChanging)
// COMMENT: {7/13/2009 7:27:08 PM}	ON_NOTIFY(TVN_SELCHANGED,  IDC_PROP_TREE, OnTreeSelChanged)

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

// COMMENT: {7/13/2009 7:46:39 PM}void CHeadICPropsPage2::OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *pResult)
// COMMENT: {7/13/2009 7:46:39 PM}{
// COMMENT: {7/13/2009 7:46:39 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:46:39 PM}	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
// COMMENT: {7/13/2009 7:46:39 PM}	this->ItemDDX = pTvn->itemOld.hItem;
// COMMENT: {7/13/2009 7:46:39 PM}	if (this->ItemDDX)
// COMMENT: {7/13/2009 7:46:39 PM}	{
// COMMENT: {7/13/2009 7:46:39 PM}		if (this->ItemDDX == this->HeadProperty.treeitem)
// COMMENT: {7/13/2009 7:46:39 PM}		{
// COMMENT: {7/13/2009 7:46:39 PM}			this->HeadProperty.DDV_SoftValidate();
// COMMENT: {7/13/2009 7:46:39 PM}		}
// COMMENT: {7/13/2009 7:46:39 PM}		else
// COMMENT: {7/13/2009 7:46:39 PM}		{
// COMMENT: {7/13/2009 7:46:39 PM}			ASSERT(FALSE);
// COMMENT: {7/13/2009 7:46:39 PM}		}
// COMMENT: {7/13/2009 7:46:39 PM}
// COMMENT: {7/13/2009 7:46:39 PM}
// COMMENT: {7/13/2009 7:46:39 PM}		// force CInPlaceXXX to lose focus
// COMMENT: {7/13/2009 7:46:39 PM}		this->TreeCtrl.SetFocus();
// COMMENT: {7/13/2009 7:46:39 PM}
// COMMENT: {7/13/2009 7:46:39 PM}		if (!this->UpdateData(TRUE))
// COMMENT: {7/13/2009 7:46:39 PM}		{
// COMMENT: {7/13/2009 7:46:39 PM}			// notify which control caused failure
// COMMENT: {7/13/2009 7:46:39 PM}			//
// COMMENT: {7/13/2009 7:46:39 PM}			CWnd* pFocus = CWnd::GetFocus();
// COMMENT: {7/13/2009 7:46:39 PM}			this->PostMessage(UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);
// COMMENT: {7/13/2009 7:46:39 PM}
// COMMENT: {7/13/2009 7:46:39 PM}			// disallow change
// COMMENT: {7/13/2009 7:46:39 PM}			//
// COMMENT: {7/13/2009 7:46:39 PM}			*pResult = TRUE;
// COMMENT: {7/13/2009 7:46:39 PM}			TRACE("Out %s Disallowed\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:46:39 PM}			return;
// COMMENT: {7/13/2009 7:46:39 PM}		}
// COMMENT: {7/13/2009 7:46:39 PM}	}
// COMMENT: {7/13/2009 7:46:39 PM}	*pResult = 0;
// COMMENT: {7/13/2009 7:46:39 PM}	TRACE("Out Allowed %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:46:39 PM}}

#define COMPARE_SET(S, R) \
do { \
	if (strItem.Compare(S) == 0) { \
		this->RichEditCtrl.SetWindowText(R.c_str()); \
	} \
} while (0)

// COMMENT: {7/13/2009 7:42:27 PM}void CHeadICPropsPage2::OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *pResult)
// COMMENT: {7/13/2009 7:42:27 PM}{
// COMMENT: {7/13/2009 7:42:27 PM}	TRACE("In %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:42:27 PM}	UNREFERENCED_PARAMETER(pResult);
// COMMENT: {7/13/2009 7:42:27 PM}	NMTREEVIEW *pTvn = reinterpret_cast<NMTREEVIEW*>(pNotifyStruct);
// COMMENT: {7/13/2009 7:42:27 PM}	this->ItemDDX = pTvn->itemNew.hItem;
// COMMENT: {7/13/2009 7:42:27 PM}	if (this->ItemDDX)
// COMMENT: {7/13/2009 7:42:27 PM}	{
// COMMENT: {7/13/2009 7:42:27 PM}		this->UpdateData(FALSE);
// COMMENT: {7/13/2009 7:42:27 PM}
// COMMENT: {7/13/2009 7:42:27 PM}		// update property description
// COMMENT: {7/13/2009 7:42:27 PM}		//
// COMMENT: {7/13/2009 7:42:27 PM}		if (this->TreeCtrl.GetSafeHwnd())
// COMMENT: {7/13/2009 7:42:27 PM}		{
// COMMENT: {7/13/2009 7:42:27 PM}			CString strItem = this->TreeCtrl.GetItemText(this->ItemDDX);
// COMMENT: {7/13/2009 7:42:27 PM}			COMPARE_SET(HEAD, this->m_sHeadRTF);
// COMMENT: {7/13/2009 7:42:27 PM}		}
// COMMENT: {7/13/2009 7:42:27 PM}	}
// COMMENT: {7/13/2009 7:42:27 PM}	if (this->TreeCtrl.GetSafeHwnd())
// COMMENT: {7/13/2009 7:42:27 PM}	{
// COMMENT: {7/13/2009 7:42:27 PM}		this->TreeCtrl.SetFocus();
// COMMENT: {7/13/2009 7:42:27 PM}	}
// COMMENT: {7/13/2009 7:42:27 PM}	TRACE("Out %s\n", __FUNCTION__);
// COMMENT: {7/13/2009 7:42:27 PM}}

void CHeadICPropsPage2::SetPropertyDescription()
{
	if (this->ItemDDX)
	{
		CString strItem = this->TreeCtrl.GetItemText(this->ItemDDX);

		COMPARE_SET(HEAD, this->m_sHeadRTF);
	}
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
