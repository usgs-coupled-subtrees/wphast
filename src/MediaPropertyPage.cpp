// MediaPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "MediaPropertyPage.h"

#include "Global.h"
#include "property.h"
#include "NewModelWizard.h"


const TCHAR ALPHA_HORIZONTAL[] = _T("Horizonal trans dispersivity");
const TCHAR ALPHA_VERTICAL[]   = _T("Vertical trans dispersivity");

// CMediaPropertyPage dialog

IMPLEMENT_DYNAMIC(CMediaPropertyPage, CPropertyPage)

CMediaPropertyPage::CMediaPropertyPage()
	: CPropertyPage(CMediaPropertyPage::IDD)
{
	this->CommonConstruct();
}

CMediaPropertyPage::CMediaPropertyPage(UINT nIDCaption)
	: CPropertyPage(CMediaPropertyPage::IDD, nIDCaption)
{
	this->CommonConstruct();
}

CMediaPropertyPage::CMediaPropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
	: CPropertyPage(CMediaPropertyPage::IDD_INTERIOR, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle)
{
	this->CommonConstruct();
}

// HACK
CMediaPropertyPage::CMediaPropertyPage(LPCTSTR lpszDummy, UINT nIDTemplate)
	: CPropertyPage(nIDTemplate)
{
	this->CommonConstruct();
}
// HACK

void CMediaPropertyPage::CommonConstruct(void)
{
	this->m_htiDDX              = 0;
	this->m_htiAlpha_long       = 0;
	this->m_htiAlpha_horizontal = 0;
	this->m_htiAlpha_vertical   = 0;

	// this may be overridden in OnSetActive
	this->m_bFlowOnly            = false;
	this->m_bFirstPaint          = true;

	// 
	this->SetFlowOnlyWizard(false);
	this->SetNewModelWizard(false);
	this->SetShowFlowOnlyMessage(false);


	// init properties
	//
	this->SetProperties(this->m_grid_elt);

	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sActiveRTF,      IDR_MEDIA_ACTIVE_RTF);
	CGlobal::LoadRTFString(this->m_sKxRTF,          IDR_MEDIA_KX_RTF);
	CGlobal::LoadRTFString(this->m_sKyRTF,          IDR_MEDIA_KY_RTF);
	CGlobal::LoadRTFString(this->m_sKzRTF,          IDR_MEDIA_KZ_RTF);
	CGlobal::LoadRTFString(this->m_sLongDispRTF,    IDR_MEDIA_LONG_DISP_RTF);
	CGlobal::LoadRTFString(this->m_sPorosityRTF,    IDR_MEDIA_POROSITY_RTF);
	CGlobal::LoadRTFString(this->m_sSpecStorageRTF, IDR_MEDIA_SPEC_STORAGE_RTF);
	CGlobal::LoadRTFString(this->m_sTransDispRTF,         IDR_MEDIA_TRANS_DISP_RTF);
	CGlobal::LoadRTFString(this->m_sAlphaHorizontalRTF,   IDR_MEDIA_ALPHA_HORZ_RTF);
	CGlobal::LoadRTFString(this->m_sAlphaVericalRTF,      IDR_MEDIA_ALPHA_VERT_RTF);
}

CMediaPropertyPage::~CMediaPropertyPage()
{
}

void CMediaPropertyPage::GetProperties(CGridElt& r_grid_elt)const
{
	r_grid_elt = this->m_grid_elt;

	// delete inactive properties
	//
	if (r_grid_elt.active->type == PROP_UNDEFINED) {
		Cproperty::CopyProperty(&r_grid_elt.active, 0);
	}
	if (r_grid_elt.alpha_long->type == PROP_UNDEFINED) {
		Cproperty::CopyProperty(&r_grid_elt.alpha_long, 0);
	}
	if (r_grid_elt.alpha_horizontal->type == PROP_UNDEFINED) {
		Cproperty::CopyProperty(&r_grid_elt.alpha_horizontal, 0);
	}
	if (r_grid_elt.alpha_vertical->type == PROP_UNDEFINED) {
		Cproperty::CopyProperty(&r_grid_elt.alpha_vertical, 0);
	}
	if (r_grid_elt.kx->type == PROP_UNDEFINED) {
		Cproperty::CopyProperty(&r_grid_elt.kx, 0);
	}
	if (r_grid_elt.ky->type == PROP_UNDEFINED) {
		Cproperty::CopyProperty(&r_grid_elt.ky, 0);
	}
	if (r_grid_elt.kz->type == PROP_UNDEFINED) {
		Cproperty::CopyProperty(&r_grid_elt.kz, 0);
	}
	if (r_grid_elt.porosity->type == PROP_UNDEFINED) {
		Cproperty::CopyProperty(&r_grid_elt.porosity, 0);
	}
	if (r_grid_elt.storage->type == PROP_UNDEFINED) {
		Cproperty::CopyProperty(&r_grid_elt.storage, 0);
	}
}

void CMediaPropertyPage::SetProperties(const CGridElt& r_grid_elt)
{
	// copy grid_elt
	this->m_grid_elt = r_grid_elt;
	
	// fill empty properties
	//
	if (this->m_grid_elt.active == NULL) {
		this->m_grid_elt.active = new Cproperty();
	}
	if (this->m_grid_elt.alpha_long == NULL) {
		this->m_grid_elt.alpha_long = new Cproperty();
	}
	if (this->m_grid_elt.alpha_horizontal == NULL) {
		this->m_grid_elt.alpha_horizontal = new Cproperty();
	}
	if (this->m_grid_elt.alpha_vertical == NULL) {
		this->m_grid_elt.alpha_vertical = new Cproperty();
	}
	if (this->m_grid_elt.kx == NULL) {
		this->m_grid_elt.kx = new Cproperty();
	}
	if (this->m_grid_elt.ky == NULL) {
		this->m_grid_elt.ky = new Cproperty();
	}
	if (this->m_grid_elt.kz == NULL) {
		this->m_grid_elt.kz = new Cproperty();
	}
	if (this->m_grid_elt.porosity == NULL) {
		this->m_grid_elt.porosity = new Cproperty();
	}
	if (this->m_grid_elt.storage == NULL) {
		this->m_grid_elt.storage = new Cproperty();
	}

	// copy to fixed and linear
	//
	this->m_grid_eltFixed = this->m_grid_eltLinear = this->m_grid_elt;
}

void CMediaPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_TREE, m_wndTreeCtrl);
	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);

	if (this->m_wndTreeCtrl.GetCount() == 0) {
		HTREEITEM hti;

		// Active
		hti = this->m_wndTreeCtrl.InsertItem(_T("Active"), TVI_ROOT, TVI_LAST);
		if (this->m_grid_elt.active->type != PROP_UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// Kx
		hti = this->m_wndTreeCtrl.InsertItem(_T("Kx"), TVI_ROOT, TVI_LAST);
		if (this->m_grid_elt.kx->type != PROP_UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// Ky
		hti = this->m_wndTreeCtrl.InsertItem(_T("Ky"), TVI_ROOT, TVI_LAST);
		if (this->m_grid_elt.ky->type != PROP_UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// Kz
		hti = this->m_wndTreeCtrl.InsertItem(_T("Kz"), TVI_ROOT, TVI_LAST);
		if (this->m_grid_elt.kz->type != PROP_UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// Porosity
		hti = this->m_wndTreeCtrl.InsertItem(_T("Porosity"), TVI_ROOT, TVI_LAST);
		if (this->m_grid_elt.porosity->type != PROP_UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// Specific storage
		hti = this->m_wndTreeCtrl.InsertItem(_T("Specific storage"), TVI_ROOT, TVI_LAST);
		if (this->m_grid_elt.storage->type != PROP_UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// Long dispersivity
		hti = this->m_wndTreeCtrl.InsertItem(_T("Long dispersivity"), TVI_ROOT, TVI_LAST);
		this->m_htiAlpha_long = hti;
		if (this->m_grid_elt.alpha_long->type != PROP_UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		hti = this->m_wndTreeCtrl.InsertItem(ALPHA_HORIZONTAL, TVI_ROOT, TVI_LAST);
		this->m_htiAlpha_horizontal = hti;
		if (this->m_grid_elt.alpha_horizontal->type != PROP_UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		hti = this->m_wndTreeCtrl.InsertItem(ALPHA_VERTICAL, TVI_ROOT, TVI_LAST);
		this->m_htiAlpha_vertical   = hti;
		if (this->m_grid_elt.alpha_vertical->type != PROP_UNDEFINED) {
			this->m_wndTreeCtrl.SetCheck(hti, BST_CHECKED);
		}

		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sActiveRTF.c_str());

		// init
		this->m_htiDDX = this->m_wndTreeCtrl.GetFirstVisibleItem();
	}

	if (this->m_htiDDX) {
		CString strItem = this->m_wndTreeCtrl.GetItemText(this->m_htiDDX);

		// Active
		if (strItem.Compare(_T("Active")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_grid_elt.active),
				static_cast<Cproperty*>(this->m_grid_eltFixed.active),
				static_cast<Cproperty*>(this->m_grid_eltLinear.active)
				);
		}
		// Kx
		else if (strItem.Compare(_T("Kx")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_grid_elt.kx),
				static_cast<Cproperty*>(this->m_grid_eltFixed.kx),
				static_cast<Cproperty*>(this->m_grid_eltLinear.kx)
				);
		}
		// Ky
		else if (strItem.Compare(_T("Ky")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_grid_elt.ky),
				static_cast<Cproperty*>(this->m_grid_eltFixed.ky),
				static_cast<Cproperty*>(this->m_grid_eltLinear.ky)
				);
		}
		// Kz
		else if (strItem.Compare(_T("Kz")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_grid_elt.kz),
				static_cast<Cproperty*>(this->m_grid_eltFixed.kz),
				static_cast<Cproperty*>(this->m_grid_eltLinear.kz)
				);
		}
		// Long dispersivity
		else if (strItem.Compare(_T("Long dispersivity")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_grid_elt.alpha_long),
				static_cast<Cproperty*>(this->m_grid_eltFixed.alpha_long),
				static_cast<Cproperty*>(this->m_grid_eltLinear.alpha_long)
				);
			if (!this->m_bFlowOnly) {
				if (pDX->m_bSaveAndValidate && this->m_grid_elt.alpha_long->type == PROP_UNDEFINED) {
					CString str;
					str.Format(_T("A default \"%s\" must be defined."), _T("Long dispersivity"));
					::AfxMessageBox(str);
					pDX->Fail();
				}
			}
		}
		// Porosity
		else if (strItem.Compare(_T("Porosity")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_grid_elt.porosity),
				static_cast<Cproperty*>(this->m_grid_eltFixed.porosity),
				static_cast<Cproperty*>(this->m_grid_eltLinear.porosity)
				);
		}
		// Specific storage
		else if (strItem.Compare(_T("Specific storage")) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_grid_elt.storage),
				static_cast<Cproperty*>(this->m_grid_eltFixed.storage),
				static_cast<Cproperty*>(this->m_grid_eltLinear.storage)
				);
		}
		// ALPHA_HORIZONTAL
		else if (strItem.Compare(ALPHA_HORIZONTAL) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_grid_elt.alpha_horizontal),
				static_cast<Cproperty*>(this->m_grid_eltFixed.alpha_horizontal),
				static_cast<Cproperty*>(this->m_grid_eltLinear.alpha_horizontal)
				);
			if (!this->m_bFlowOnly) {
				if (pDX->m_bSaveAndValidate && this->m_grid_elt.alpha_horizontal->type == PROP_UNDEFINED) {
					CString str;
					str.Format("A default \"%s\" must be defined.", ALPHA_HORIZONTAL);
					::AfxMessageBox(str);
					pDX->Fail();
				}
			}
		}
		// ALPHA_VERTICAL
		else if (strItem.Compare(ALPHA_VERTICAL) == 0) {
			CGlobal::DDX_Property(pDX, &this->m_wndTreeCtrl, this->m_htiDDX,
				static_cast<Cproperty*>(this->m_grid_elt.alpha_vertical),
				static_cast<Cproperty*>(this->m_grid_eltFixed.alpha_vertical),
				static_cast<Cproperty*>(this->m_grid_eltLinear.alpha_vertical)
				);
			if (!this->m_bFlowOnly) {
				if (pDX->m_bSaveAndValidate && this->m_grid_elt.alpha_vertical->type == PROP_UNDEFINED) {
					CString str;
					str.Format("A default \"%s\" must be defined.", ALPHA_VERTICAL);
					::AfxMessageBox(str);
					pDX->Fail();
				}
			}
		}

		// ERROR
		else {
			ASSERT(FALSE);
		}
	}
}

BEGIN_MESSAGE_MAP(CMediaPropertyPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROP_TREE, OnTvnSelchangedPropTree)
	ON_NOTIFY(TVN_SELCHANGING, IDC_PROP_TREE, OnTvnSelchangingPropTree)
	//{{USER
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)
	ON_MESSAGE(UM_CHECKSTATECHANGE, OnUM_CheckStateChange)
	//}}USER
	ON_BN_CLICKED(IDC_SINGLE_VALUE_RADIO, OnBnClickedSingleValueRadio)
	ON_BN_CLICKED(IDC_LINEAR_INTERPOLATION_RADIO, OnBnClickedLinearInterpolationRadio)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CMediaPropertyPage message handlers

void CMediaPropertyPage::OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// Add your control notification handler code here

	// exchange property
	//
	this->m_htiDDX = pNMTreeView->itemNew.hItem;
	this->UpdateData(FALSE);

	// update property description
	//
	if (this->m_htiDDX) {
		CString strItem = this->m_wndTreeCtrl.GetItemText(this->m_htiDDX);

		// Active
		if (strItem.Compare(_T("Active")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sActiveRTF.c_str());
		}
		// Kx
		else if (strItem.Compare(_T("Kx")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sKxRTF.c_str());
		}
		// Ky
		else if (strItem.Compare(_T("Ky")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sKyRTF.c_str());
		}
		// Kz
		else if (strItem.Compare(_T("Kz")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sKzRTF.c_str());
		}
		// Long dispersivity
		else if (strItem.Compare(_T("Long dispersivity")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sLongDispRTF.c_str());
		}
		// Porosity
		else if (strItem.Compare(_T("Porosity")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sPorosityRTF.c_str());
		}
		// Specific storage
		else if (strItem.Compare(_T("Specific storage")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sSpecStorageRTF.c_str());
		}
		// Trans dispersivity
		else if (strItem.Compare(_T("Trans dispersivity")) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sTransDispRTF.c_str());
		}
		//{{
		// ALPHA_HORIZONTAL
		else if (strItem.Compare(ALPHA_HORIZONTAL) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sAlphaHorizontalRTF.c_str());
		}
		// ALPHA_VERTICAL
		else if (strItem.Compare(ALPHA_VERTICAL) == 0) {
			this->m_wndRichEditCtrl.SetWindowText(this->m_sAlphaVericalRTF.c_str());
		}
		//}}
		// ERROR
		else {
			ASSERT(FALSE);
			this->m_wndRichEditCtrl.SetWindowText("");
		}
	}

	*pResult = 0;
}

void CMediaPropertyPage::OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// Add your control notification handler code here

	// validate itemOld
	//
	this->m_htiDDX = pNMTreeView->itemOld.hItem;
	if (!this->UpdateData(TRUE)) {
		// notify which control caused failure
		//
		CWnd* pFocus = CWnd::GetFocus();
		::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

		// disallow change
		//
		*pResult = TRUE;
		return;
	}

	*pResult = 0;
}

LRESULT CMediaPropertyPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	// DDX/DDV failure occured
	//
	CWnd* pFocus = (CWnd*)wParam;
	ASSERT_KINDOF(CWnd, pFocus);
	pFocus->SetFocus();
	return 0;
}

LRESULT CMediaPropertyPage::OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hSelectedItem = this->m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hItemChanged = (HTREEITEM)lParam;

	// handle special case when hSelectedItem is the same
	// as hItemChanged
	if (hSelectedItem == hItemChanged) {
		// update current item
		//
		this->m_htiDDX = hSelectedItem;
		this->UpdateData(FALSE);
		return 0;
	}

	this->m_htiDDX = hSelectedItem;
	if (this->UpdateData(TRUE)) {
		// set selection on item that was checked
		// causes TVN_SELCHANGING and TVN_SELCHANGED
		//
		this->m_wndTreeCtrl.SelectItem(hItemChanged);
	}
	else {
		// UpdateData failed
		// notify which control caused the failure
		//
		return (LRESULT)CWnd::GetFocus();
	}

	return 0;
}

void CMediaPropertyPage::OnBnClickedSingleValueRadio()
{
	CGlobal::EnableLinearInterpolation(this, FALSE);
	CGlobal::EnableFixed(this, TRUE);
}

void CMediaPropertyPage::OnBnClickedLinearInterpolationRadio()
{
	CGlobal::EnableLinearInterpolation(this, TRUE);
	CGlobal::EnableFixed(this, FALSE);
}

BOOL CMediaPropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	if (this->m_bNewModelWizard)
	{
		CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
		ASSERT_KINDOF(CPropertySheet, pSheet);
		if (pSheet->IsWizard())
		{
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);

			// add vertical and horiz transverse dipersivity if missing
			CNewModelWizard* pWizard = static_cast<CNewModelWizard*>(pSheet);
			CFlowOnly flowOnly = pWizard->GetFlowOnly();

			this->SetFlowOnly((bool)flowOnly);

			if (this->GetFlowOnly()) {
				// TODO check for defaults and maybe delete
			}
			else 
			{
				CGridElt gridElt = CGridElt::NewDefaults(this->m_bFlowOnly);
				if (this->m_grid_elt.alpha_long->type == PROP_UNDEFINED) {
					Cproperty::CopyProperty(&this->m_grid_elt.alpha_long, gridElt.alpha_long);
					this->m_wndTreeCtrl.SetCheck(this->m_htiAlpha_long, BST_CHECKED);
				}
				if (this->m_grid_elt.alpha_horizontal->type == PROP_UNDEFINED) {
					Cproperty::CopyProperty(&this->m_grid_elt.alpha_horizontal, gridElt.alpha_horizontal);
					this->m_wndTreeCtrl.SetCheck(this->m_htiAlpha_horizontal, BST_CHECKED);
				}
				if (this->m_grid_elt.alpha_vertical->type == PROP_UNDEFINED) {
					Cproperty::CopyProperty(&this->m_grid_elt.alpha_vertical, gridElt.alpha_vertical);
					this->m_wndTreeCtrl.SetCheck(this->m_htiAlpha_vertical, BST_CHECKED);
				}
				this->m_grid_eltFixed = this->m_grid_eltLinear = this->m_grid_elt;
			}
		}
	}
	else if (this->m_bFlowOnlyWizard)
	{
		CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
		ASSERT_KINDOF(CPropertySheet, pSheet);
		if (pSheet->IsWizard()) {
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
			this->SetFlowOnly(false);
		}
	}
	else
	{
		CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
		ASSERT_KINDOF(CPropertySheet, pSheet);
		if (pSheet->IsWizard()) {
			pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
		}
	}
	return bRet;
}

BOOL CMediaPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

// COMMENT: {6/2/2004 9:41:47 PM}	// Disable cancel if no apply button
// COMMENT: {6/2/2004 9:41:47 PM}	//
// COMMENT: {6/2/2004 9:41:47 PM}	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
// COMMENT: {6/2/2004 9:41:47 PM}	ASSERT_KINDOF(CPropertySheet, pSheet);
// COMMENT: {6/2/2004 9:41:47 PM}	if (pSheet->m_psh.dwFlags & PSH_NOAPPLYNOW) {
// COMMENT: {6/2/2004 9:41:47 PM}		pSheet->GetDlgItem(2)->EnableWindow(FALSE);
// COMMENT: {6/2/2004 9:41:47 PM}		pSheet->ModifyStyle(WS_SYSMENU, 0);
// COMMENT: {6/2/2004 9:41:47 PM}	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMediaPropertyPage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// Add your message handler code here
	// Do not call CPropertyPage::OnPaint() for painting messages

	if (this->m_bFirstPaint)
	{
		this->m_bFirstPaint = false;

		if (this->m_bShowFlowOnlyMessage) {
			this->m_wndTreeCtrl.SelectItem(this->m_htiAlpha_long);
			::AfxMessageBox("Since you have turned on solute transport you may want to review the default dispersivities.");
			this->m_bShowFlowOnlyMessage = false;
		}
	}
}

BOOL CMediaPropertyPage::OnKillActive()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
}

LRESULT CMediaPropertyPage::OnWizardNext()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		if (!this->UpdateData(TRUE))
		{
			return -1; // return –1 to prevent the page from changing 
		}
	}
	return CPropertyPage::OnWizardNext();
}
