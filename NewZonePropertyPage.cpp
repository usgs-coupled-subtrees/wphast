// NewZonePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "NewZonePropertyPage.h"


// CNewZonePropertyPage dialog

IMPLEMENT_DYNAMIC(CNewZonePropertyPage, CPropertyPage)
CNewZonePropertyPage::CNewZonePropertyPage()
: CPropertyPage(CNewZonePropertyPage::IDD)
, m_type(0)
, m_htiMedia(0)
, m_htiBC(0)
, m_htiBCFlux(0)
, m_htiBCLeaky(0)
, m_htiBCSpec(0)
, m_htiICHead(0)
, m_htiChemIC(0)
{
}

CNewZonePropertyPage::~CNewZonePropertyPage()
{
}

void CNewZonePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ZONES, m_wndTree);

	if (this->m_bFirstSetActive) {
		this->m_htiMedia   = this->m_wndTree.InsertItem(_T("MEDIA"));
		this->m_htiBC      = this->m_wndTree.InsertItem(_T("BOUNDARY_CONDITIONS"));
		this->m_htiIC      = this->m_wndTree.InsertItem(_T("INITIAL_CONDITIONS"));

		// BC
		this->m_htiBCFlux  = this->m_wndTree.InsertItem(_T("FLUX_BC"), this->m_htiBC);
		this->m_htiBCLeaky = this->m_wndTree.InsertItem(_T("LEAKY_BC"), this->m_htiBC);
		this->m_htiBCSpec  = this->m_wndTree.InsertItem(_T("SPECIFIED_VALUE_BC"), this->m_htiBC);

		// IC
		this->m_htiICHead  = this->m_wndTree.InsertItem(_T("HEAD_IC"), this->m_htiIC);
		this->m_htiChemIC  = this->m_wndTree.InsertItem(_T("CHEMISTRY_IC"), this->m_htiIC);		

		this->m_wndTree.EnsureVisible(this->m_htiBCFlux);
		this->m_wndTree.EnsureVisible(this->m_htiICHead);
		this->m_wndTree.EnsureVisible(this->m_htiMedia);
	}

	if (pDX->m_bSaveAndValidate) {
		HTREEITEM hti = this->m_wndTree.GetSelectedItem();
		if (hti == this->m_htiMedia) {
			this->m_type = ID_ZONE_TYPE_MEDIA;
		}
		else if (hti == this->m_htiBC) {
			::AfxMessageBox("Please choose the type of boundary condition.", MB_OK);
			pDX->Fail();
		}
		else if (hti == this->m_htiIC) {
			::AfxMessageBox("Please choose the type of initial condition.", MB_OK);
			pDX->Fail();
		}
		else if (hti == this->m_htiBCFlux) {
			this->m_type = ID_ZONE_TYPE_BC_FLUX;
		}
		else if (hti == this->m_htiBCLeaky) {
			this->m_type = ID_ZONE_TYPE_BC_LEAKY;
		}
		else if (hti == this->m_htiBCSpec) {
			this->m_type = ID_ZONE_TYPE_BC_SPECIFIED;
		}
		else if (hti == this->m_htiICHead) {
			this->m_type = ID_ZONE_TYPE_IC_HEAD;
		}
		else if (hti == this->m_htiChemIC) {
			this->m_type = ID_ZONE_TYPE_IC_CHEM;
		}
		else {
			::AfxMessageBox("Please choose the type of zone to define.", MB_OK);
			pDX->Fail();
		}
	}
	else {
		if (this->m_type == ID_ZONE_TYPE_MEDIA) {
			this->m_wndTree.SelectItem(this->m_htiMedia);
		}
		else if (this->m_type == ID_ZONE_TYPE_BC_FLUX) {
			this->m_wndTree.SelectItem(this->m_htiBCFlux);
		}
		else if (this->m_type == ID_ZONE_TYPE_BC_LEAKY) {
			this->m_wndTree.SelectItem(this->m_htiBCLeaky);
		}
		else if (this->m_type == ID_ZONE_TYPE_BC_SPECIFIED) {
			this->m_wndTree.SelectItem(this->m_htiBCSpec);
		}
		else if (this->m_type == ID_ZONE_TYPE_IC_HEAD) {
			this->m_wndTree.SelectItem(this->m_htiICHead);
		}
		else if (this->m_type == ID_ZONE_TYPE_IC_CHEM) {
			this->m_wndTree.SelectItem(this->m_htiChemIC);
		}		
		else {
			this->m_wndTree.SelectItem(this->m_htiMedia);
		}
	}
}

BEGIN_MESSAGE_MAP(CNewZonePropertyPage, CPropertyPage)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_ZONES, OnNMDblclkTreeZones)
END_MESSAGE_MAP()

// CNewZonePropertyPage message handlers

BOOL CNewZonePropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add extra initialization here

	// Store pointers to all the pages in m_PropPageArray.
	// 0 : this page        CNewZonePropertyPage
	// 1 : media page       CMediaPropertyPage
	// 2 : flux page        CBCFluxPropertyPage
	// 3 : leaky page       CBCLeakyPropertyPage
	// 4 : specified page   CBCSpecifiedPropertyPage
	// 5 : head_ic page     CICHeadPropertyPage
	// 6 : chem_ic page     CChemICPropertyPage
	
	CPropertySheet* pSheet = (CPropertySheet*) this->GetParent();   
	if (pSheet->IsWizard()) {
		int nCount = pSheet->GetPageCount();
		ASSERT(nCount == 7);
		for (int i = 0; i < nCount; ++i) {
			this->m_PropPageArray.Add(pSheet->GetPage(i));
		}

		// remove all but this page
		for (int i = 1; i < nCount; ++i) {
			pSheet->RemovePage(1);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CNewZonePropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard()) {
		pSheet->SetWizardButtons(PSWIZB_NEXT);
	}
	return bRet;
}

UINT CNewZonePropertyPage::GetType(void)
{
	return m_type;
}

LRESULT CNewZonePropertyPage::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*) this->GetParent();

	// 0 : this page        CNewZonePropertyPage
	// 1 : media page       CMediaPropertyPage
	// 2 : flux page        CBCFluxPropertyPage
	// 3 : leaky page       CBCLeakyPropertyPage
	// 4 : specified page   CBCSpecifiedPropertyPage
	// 5 : head_ic page     CICHeadPropertyPage
	// 6 : chem_ic page     CChemICPropertyPage

	// remove all but this page
	int nCount = pSheet->GetPageCount();
	for (int i = 1; i < nCount; ++i) {
		pSheet->RemovePage(1);
	}

	// add only the next page
	if (this->UpdateData(TRUE)) {
		switch (this->m_type) {
			case ID_ZONE_TYPE_MEDIA:
				pSheet->AddPage(this->m_PropPageArray[1]);
				break;
			case ID_ZONE_TYPE_BC_FLUX:
				pSheet->AddPage(this->m_PropPageArray[2]);
				break;
			case ID_ZONE_TYPE_BC_LEAKY:
				pSheet->AddPage(this->m_PropPageArray[3]);
				break;
			case ID_ZONE_TYPE_BC_SPECIFIED:
				pSheet->AddPage(this->m_PropPageArray[4]);
				break;
			case ID_ZONE_TYPE_IC_HEAD:
				pSheet->AddPage(this->m_PropPageArray[5]);
				break;
			case ID_ZONE_TYPE_IC_CHEM:
				pSheet->AddPage(this->m_PropPageArray[6]);
				break;
		}
	}

	return CPropertyPage::OnWizardNext();
}

void CNewZonePropertyPage::OnNMDblclkTreeZones(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	HTREEITEM hti = this->m_wndTree.GetSelectedItem();
	if ((hti != NULL) && (hti != this->m_htiBC) && (hti != this->m_htiIC))
	{	
		// simulate user pressing next
		CPropertySheet *pSheet = static_cast<CPropertySheet*>(this->GetParent());
		pSheet->PressButton(PSBTN_NEXT);
	}
	*pResult = 0;
}
