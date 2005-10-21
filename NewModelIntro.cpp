// NewModelIntro.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "NewModelIntro.h"
#include <float.h>

#include "ChemICPropertyPage.h"


// CNewModelIntro dialog

IMPLEMENT_DYNAMIC(CNewModelIntro, CPropertyPage)
CNewModelIntro::CNewModelIntro()
	: CPropertyPage(CNewModelIntro::IDD)
{
	this->m_psp.dwFlags |= PSP_HIDEHEADER;
}

CNewModelIntro::~CNewModelIntro()
{
}

void CNewModelIntro::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	if (pDX->m_bSaveAndValidate)
	{
		// FLOW_ONLY
		if (IDC_FLOW_ONLY_RADIO == this->GetCheckedRadioButton(IDC_FLOW_ONLY_RADIO, IDC_SOLUTE_TRANSPORT_RADIO)) {
			this->m_flowOnly.SetFlowOnly(true);
		}
		else {
			this->m_flowOnly.SetFlowOnly(false);

			double dDiffusivity;
			DDX_Text(pDX, IDC_MOL_DIFF_EDIT, dDiffusivity);
			DDV_MinMaxDouble(pDX, dDiffusivity, 0, DBL_MAX);
			this->m_flowOnly.SetDiffusivity(dDiffusivity);
		}

		// STEADY_FLOW
		this->m_steadyFlow = (IDC_STEADY_FLOW_RADIO == 
			this->GetCheckedRadioButton(IDC_STEADY_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO));

		// FREE_SURFACE_BC
		this->m_freeSurface = (IDC_FREE_SURFACE_RADIO == 
			this->GetCheckedRadioButton(IDC_CONFINED_FLOW_RADIO, IDC_FREE_SURFACE_RADIO));

	}
	else
	{
		// FLOW_ONLY
		if (this->m_flowOnly) {
			this->CheckRadioButton(IDC_FLOW_ONLY_RADIO, IDC_SOLUTE_TRANSPORT_RADIO, IDC_FLOW_ONLY_RADIO);
			this->OnBnClickedFlowOnlyRadio();
		}
		else {
			this->CheckRadioButton(IDC_FLOW_ONLY_RADIO, IDC_SOLUTE_TRANSPORT_RADIO, IDC_SOLUTE_TRANSPORT_RADIO);
			this->OnBnClickedSoluteTransportRadio();
		}
		double dDiffusivity = this->m_flowOnly.GetDiffusivity();
		DDX_Text(pDX, IDC_MOL_DIFF_EDIT, dDiffusivity);

		// STEADY_FLOW
		if (this->m_steadyFlow) {
			this->CheckRadioButton(IDC_STEADY_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO, IDC_STEADY_FLOW_RADIO);
		}
		else {
			this->CheckRadioButton(IDC_STEADY_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO, IDC_TRANSIENT_FLOW_RADIO);
		}

		// FREE_SURFACE_BC
		if (this->m_freeSurface) {
			this->CheckRadioButton(IDC_CONFINED_FLOW_RADIO, IDC_FREE_SURFACE_RADIO, IDC_FREE_SURFACE_RADIO);
		}
		else {
			this->CheckRadioButton(IDC_CONFINED_FLOW_RADIO, IDC_FREE_SURFACE_RADIO, IDC_CONFINED_FLOW_RADIO);
		}

	}
}

// COMMENT: {5/27/2004 3:29:23 PM}void CNewModelIntro::SetProperties(const CFlowOnly& r_flowOnly)
// COMMENT: {5/27/2004 3:29:23 PM}{
// COMMENT: {5/27/2004 3:29:23 PM}	this->m_flowOnly = r_flowOnly;
// COMMENT: {5/27/2004 3:29:23 PM}}

// COMMENT: {5/27/2004 3:29:32 PM}void CNewModelIntro::GetProperties(CFlowOnly& r_flowOnly)const
// COMMENT: {5/27/2004 3:29:32 PM}{
// COMMENT: {5/27/2004 3:29:32 PM}	r_flowOnly = this->m_flowOnly;
// COMMENT: {5/27/2004 3:29:32 PM}}

void CNewModelIntro::SetFlowOnly(const CFlowOnly& flowOnly)
{
	this->m_flowOnly = flowOnly;
}

CFlowOnly CNewModelIntro::GetFlowOnly(void)const
{
	return this->m_flowOnly;
}

void CNewModelIntro::SetSteadyFlow(const CSteadyFlow& steadyFlow)
{
	this->m_steadyFlow = steadyFlow;
}

CSteadyFlow CNewModelIntro::GetSteadyFlow(void)const
{
	return this->m_steadyFlow;
}

void CNewModelIntro::SetFreeSurface(const CFreeSurface& freeSurface)
{
	this->m_freeSurface = freeSurface;
}

CFreeSurface CNewModelIntro::GetFreeSurface(void)const
{
	return this->m_freeSurface;
}

BEGIN_MESSAGE_MAP(CNewModelIntro, CPropertyPage)
	ON_BN_CLICKED(IDC_SOLUTE_TRANSPORT_RADIO, OnBnClickedSoluteTransportRadio)
	ON_BN_CLICKED(IDC_FLOW_ONLY_RADIO, OnBnClickedFlowOnlyRadio)
END_MESSAGE_MAP()

// CNewModelIntro message handlers

BOOL CNewModelIntro::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	CWnd* pTitle = this->GetDlgItem(IDC_TITLE);

	CFont font;
	font.CreatePointFont(12, "Verdana Bold");
	pTitle->SetFont(&font);

// COMMENT: {6/2/2004 9:41:52 PM}	// Disable cancel if no apply button
// COMMENT: {6/2/2004 9:41:52 PM}	//
// COMMENT: {6/2/2004 9:41:52 PM}	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
// COMMENT: {6/2/2004 9:41:52 PM}	ASSERT_KINDOF(CPropertySheet, pSheet);
// COMMENT: {6/2/2004 9:41:52 PM}	if (pSheet->m_psh.dwFlags & PSH_NOAPPLYNOW) {
// COMMENT: {6/2/2004 9:41:52 PM}		pSheet->GetDlgItem(2)->EnableWindow(FALSE);
// COMMENT: {6/2/2004 9:41:52 PM}		pSheet->ModifyStyle(WS_SYSMENU, 0);
// COMMENT: {6/2/2004 9:41:52 PM}	}

	//{{
	// Store pointers to all the pages in m_PropPageArray.
	//
	CPropertySheet* sheet = static_cast<CPropertySheet*>(this->GetParent());
	int count = sheet->GetPageCount();
	for (int i = 0; i < count; ++i) {
		if ( CPropertyPage *pPage = sheet->GetPage(i) ) {
			this->m_PropPageArray.Add(pPage);
			if ( pPage->IsKindOf( RUNTIME_CLASS(CChemICPropertyPage) ) ) {
				this->m_pChemICPage = static_cast<CChemICPropertyPage*>(pPage);
				this->m_idxChemICPage = i;
			}
		}
	}
	//}}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNewModelIntro::OnBnClickedSoluteTransportRadio()
{
	this->GetDlgItem(IDC_MOL_DIFF_STATIC)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_MOL_DIFF_EDIT)->EnableWindow(TRUE);
}

void CNewModelIntro::OnBnClickedFlowOnlyRadio()
{
	this->GetDlgItem(IDC_MOL_DIFF_STATIC)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_MOL_DIFF_EDIT)->EnableWindow(FALSE);
}

BOOL CNewModelIntro::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();

	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard()) {
		pSheet->SetWizardButtons(PSWIZB_NEXT);
#if defined(_DEBUG) && !defined(__CPPUNIT__)
		pSheet->SetWizardButtons(PSWIZB_FINISH);
#endif
	}

	return bRet;
}

LRESULT CNewModelIntro::OnWizardNext()
{
	// Note: The page has NOT been validated.
	// Returning -1 also skips the validation
	//

// COMMENT: {6/3/2004 1:04:05 PM}	if (!this->UpdateData(TRUE)) {
// COMMENT: {6/3/2004 1:04:05 PM}		return -1;
// COMMENT: {6/3/2004 1:04:05 PM}	}
// COMMENT: {6/3/2004 1:04:05 PM}
// COMMENT: {6/3/2004 1:04:05 PM}	CPropertySheet* sheet = static_cast<CPropertySheet*>(this->GetParent());
// COMMENT: {6/3/2004 1:04:05 PM}	int count = sheet->GetPageCount();
// COMMENT: {6/3/2004 1:04:05 PM}	int arr_size = this->m_PropPageArray.GetSize();
// COMMENT: {6/3/2004 1:04:05 PM}
// COMMENT: {6/3/2004 1:04:05 PM}	ASSERT(this->m_pChemICPage);
// COMMENT: {6/3/2004 1:04:05 PM}	ASSERT(0 <= this->m_idxChemICPage);
// COMMENT: {6/3/2004 1:04:05 PM}	ASSERT(this->m_idxChemICPage < arr_size);
// COMMENT: {6/3/2004 1:04:05 PM}	ASSERT(this->m_pChemICPage == this->m_PropPageArray[this->m_idxChemICPage]);
// COMMENT: {6/3/2004 1:04:05 PM}
// COMMENT: {6/3/2004 1:04:05 PM}	if ((bool)this->m_flowOnly)
// COMMENT: {6/3/2004 1:04:05 PM}	{
// COMMENT: {6/3/2004 1:04:05 PM}		// Remove the ChemcIC page ONLY if it is found in the propertysheet.
// COMMENT: {6/3/2004 1:04:05 PM}		if (count == arr_size && this->m_pChemICPage) {
// COMMENT: {6/3/2004 1:04:05 PM}			ASSERT(sheet->GetPageIndex(this->m_pChemICPage) == this->m_idxChemICPage);
// COMMENT: {6/3/2004 1:04:05 PM}			sheet->RemovePage(this->m_pChemICPage);
// COMMENT: {6/3/2004 1:04:05 PM}		}
// COMMENT: {6/3/2004 1:04:05 PM}		ASSERT(sheet->GetPageIndex(this->m_pChemICPage) == -1);
// COMMENT: {6/3/2004 1:04:05 PM}	}
// COMMENT: {6/3/2004 1:04:05 PM}	else
// COMMENT: {6/3/2004 1:04:05 PM}	{
// COMMENT: {6/3/2004 1:04:05 PM}		// Add the ChemcIC page back to the propertysheet ONLY if it 
// COMMENT: {6/3/2004 1:04:05 PM}		// is NOT found from the propertysheet.
// COMMENT: {6/3/2004 1:04:05 PM}		if (count < arr_size) 
// COMMENT: {6/3/2004 1:04:05 PM}		{
// COMMENT: {6/3/2004 1:04:05 PM}			int i;
// COMMENT: {6/3/2004 1:04:05 PM}			ASSERT(sheet->GetPageIndex(this->m_pChemICPage) == -1);
// COMMENT: {6/3/2004 1:04:05 PM}			for (i = arr_size - 1; i > this->m_idxChemICPage; --i) {
// COMMENT: {6/3/2004 1:04:05 PM}				CPropertyPage *pPage = this->m_PropPageArray[i];
// COMMENT: {6/3/2004 1:04:05 PM}				ASSERT(sheet->GetPageIndex(this->m_PropPageArray[i]) != -1);
// COMMENT: {6/3/2004 1:04:05 PM}				sheet->RemovePage(this->m_PropPageArray[i]);
// COMMENT: {6/3/2004 1:04:05 PM}			}
// COMMENT: {6/3/2004 1:04:05 PM}
// COMMENT: {6/3/2004 1:04:05 PM}			ASSERT(sheet->GetPageCount() == this->m_idxChemICPage);
// COMMENT: {6/3/2004 1:04:05 PM}			for (; i < arr_size; ++i) {
// COMMENT: {6/3/2004 1:04:05 PM}				ASSERT(sheet->GetPageIndex(this->m_PropPageArray[i]) == -1);
// COMMENT: {6/3/2004 1:04:05 PM}				sheet->AddPage(this->m_PropPageArray[i]);
// COMMENT: {6/3/2004 1:04:05 PM}			}
// COMMENT: {6/3/2004 1:04:05 PM}			ASSERT(sheet->GetPageCount() == arr_size);
// COMMENT: {6/3/2004 1:04:05 PM}		}
// COMMENT: {6/3/2004 1:04:05 PM}		ASSERT(sheet->GetPageIndex(this->m_pChemICPage) == this->m_idxChemICPage);
// COMMENT: {6/3/2004 1:04:05 PM}	}
// COMMENT: {6/3/2004 1:04:05 PM}
// COMMENT: {6/3/2004 1:04:05 PM}	// Add your specialized code here and/or call the base class

	return CPropertyPage::OnWizardNext();
}
