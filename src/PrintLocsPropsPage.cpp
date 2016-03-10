// MediaPropsPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "PrintLocsPropsPage.h"

#include "Global.h"


// CPrintLocsPropsPage dialog

// COMMENT: {2/11/2016 9:50:59 PM}const TCHAR ACTIVE[]            = _T("Active");
// COMMENT: {2/11/2016 9:50:59 PM}const TCHAR KX[]                = _T("Kx");
// COMMENT: {2/11/2016 9:50:59 PM}const TCHAR KY[]                = _T("Ky");
// COMMENT: {2/11/2016 9:50:59 PM}const TCHAR KZ[]                = _T("Kz");
// COMMENT: {2/11/2016 9:50:59 PM}const TCHAR POROSITY[]          = _T("Porosity");
// COMMENT: {2/11/2016 9:50:59 PM}const TCHAR STORAGE[]           = _T("Specific storage");
// COMMENT: {2/11/2016 9:50:59 PM}const TCHAR ALPHA_LONG[]        = _T("Longitudinal dispersivity");
// COMMENT: {2/11/2016 9:50:59 PM}const TCHAR ALPHA_HORIZONTAL[]  = _T("Horizontal dispersivity");
// COMMENT: {2/11/2016 9:50:59 PM}const TCHAR ALPHA_VERTICAL[]    = _T("Vertical dispersivity");
// COMMENT: {2/11/2016 9:50:59 PM}const TCHAR TORTUOSITY[]        = _T("Tortuosity");
const TCHAR PRINT[]             = _T("Print");


const int SELECTED = 0;
const int SINGLE = 0;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CPrintLocsPropsPage, CPropsPropertyPage)

CPrintLocsPropsPage::CPrintLocsPropsPage()
	: CPropsPropertyPage(CPrintLocsPropsPage::IDD)
	, PrintProperty(this, true, false)
{
	TRACE("In %s\n", __FUNCTION__);

	// load property descriptions
	CGlobal::LoadRTFString(this->m_sDescriptionRTF,  IDR_DESCRIPTION_RTF);
	CGlobal::LoadRTFString(this->m_sChemRTF,         IDR_PR_LOC_CHEM_RTF);
	CGlobal::LoadRTFString(this->m_sXYZ_ChemRTF,     IDR_PR_LOC_XYZ_CHEM_RTF);
	CGlobal::LoadRTFString(this->m_sPrintRTF,        IDR_PR_LOC_PRINT_RTF);

	TRACE("Out %s\n", __FUNCTION__);
}

CPrintLocsPropsPage::~CPrintLocsPropsPage()
{
	TRACE("In %s\n", __FUNCTION__);
	TRACE("Out %s\n", __FUNCTION__);
}

void CPrintLocsPropsPage::DoDataExchange(CDataExchange* pDX)
{
	TRACE("In %s\n", __FUNCTION__);
	CPropsPropertyPage::DoDataExchange(pDX);

	if (this->TreeCtrl.GetCount() == 0)
	{
		// setup tree
		this->PrintProperty.treeitem           = this->TreeCtrl.InsertItem(PRINT, TVI_ROOT, TVI_LAST);

		// setup tree selection
		this->ItemDDX = this->PrintProperty.treeitem;
		this->TreeCtrl.SelectItem(this->ItemDDX);
		this->TreeCtrl.ModifyStyle(TVS_TRACKSELECT, TVS_FULLROWSELECT|TVS_SHOWSELALWAYS, 0);

		// set points grid for each propety
		this->PrintProperty.SetPointsGrid(&this->PointsGrid);
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

void CPrintLocsPropsPage::DDV_SoftValidate()
{
	// description
	CDataExchange dx(this, TRUE);
	::DDX_Text(&dx, IDC_DESC_EDIT, this->Description);

	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->PrintProperty.treeitem)
		{
			this->PrintProperty.DDV_SoftValidate();
		}
	}
}

void CPrintLocsPropsPage::DDX_Single(CDataExchange* pDX)
{
	if (this->ItemDDX)
	{
		if (!pDX->m_bSaveAndValidate)
		{
			// display single
			CGridTimeSeries::ShowSingleProperty(this, SW_SHOW);
		}

		// DDX_Single selected property
		if (this->ItemDDX == this->PrintProperty.treeitem)
		{
			this->PrintProperty.DDX_Single(pDX, false);
		}
	}
}

void CPrintLocsPropsPage::SetProperties(const CPrintZone& rPrintZone)
{
	TRACE("In %s\n", __FUNCTION__);

	// copy grid_elt
	this->PrintZone = rPrintZone;

	// single properties
	this->PrintProperty.SetProperty(rPrintZone.print);
	TRACE("Out %s\n", __FUNCTION__);
}

void CPrintLocsPropsPage::GetProperties(CPrintZone& rPrintZone)const
{
	TRACE("In %s\n", __FUNCTION__);

	rPrintZone = this->PrintZone;

	// single properties
	this->PrintProperty.GetProperty(rPrintZone.print);

	TRACE("Out %s\n", __FUNCTION__);
}

BEGIN_MESSAGE_MAP(CPrintLocsPropsPage, CPropsPropertyPage)
	// IDC_DESC_EDIT
	ON_EN_SETFOCUS(IDC_DESC_EDIT, OnEnSetfocusDescEdit)

	// IDC_BUTTON_XYZ
	ON_BN_CLICKED(IDC_BUTTON_XYZ, OnBnClickedButtonXYZ)

	// IDC_COMBO_PROPTYPE
	ON_CBN_SELCHANGE(IDC_COMBO_PROPTYPE, OnCbnSelchangeComboProptype)

	// IDC_CHECK_MIXTURE
	ON_BN_CLICKED(IDC_CHECK_MIXTURE, OnBnClickedCheckMixture)

// COMMENT: {3/9/2016 3:59:51 PM}	// IDC_CHECK_SHELL
// COMMENT: {3/9/2016 3:59:51 PM}	ON_BN_CLICKED(IDC_CHECK_SHELL, OnBnClickedCheckShell)

	// DDX failure
	ON_MESSAGE(UM_DDX_FAILURE, OnUM_DDXFailure)

// COMMENT: {3/9/2016 4:00:33 PM}	// shell
// COMMENT: {3/9/2016 4:00:33 PM}	ON_EN_SETFOCUS(IDC_EDIT_XW, &CPrintLocsPropsPage::OnEnSetfocusEditXw)
// COMMENT: {3/9/2016 4:00:33 PM}	ON_EN_SETFOCUS(IDC_EDIT_YW, &CPrintLocsPropsPage::OnEnSetfocusEditYw)
// COMMENT: {3/9/2016 4:00:33 PM}	ON_EN_SETFOCUS(IDC_EDIT_ZW, &CPrintLocsPropsPage::OnEnSetfocusEditZw)
// COMMENT: {3/9/2016 4:00:33 PM}	ON_BN_SETFOCUS(IDC_CHECK_SHELL, &CPrintLocsPropsPage::OnBnSetfocusCheckShell)
END_MESSAGE_MAP()


// CPrintLocsPropsPage message handlers

#define COMPARE_SET(S, R) \
do { \
	if (strItem.Compare(S) == 0) { \
		this->RichEditCtrl.SetWindowText(R.c_str()); \
	} \
} while (0)

void CPrintLocsPropsPage::SetPropertyDescription()
{
	CString strItem = this->TreeCtrl.GetItemText(this->ItemDDX);

	COMPARE_SET(PRINT,            this->m_sPrintRTF);
}

void CPrintLocsPropsPage::OnEnSetfocusDescEdit()
{
	TRACE("In %s\n", __FUNCTION__);
	this->RichEditCtrl.SetWindowText(this->m_sDescriptionRTF.c_str());
	TRACE("Out %s\n", __FUNCTION__);
}

LRESULT CPrintLocsPropsPage::OnUM_DDXFailure(WPARAM wParam, LPARAM lParam)
{
	CWnd* pFocus = (CWnd*)wParam;
	if (pFocus && pFocus->GetSafeHwnd())
	{
		ASSERT_KINDOF(CWnd, pFocus);
		pFocus->SetFocus();
	}
	return 0;
}

void CPrintLocsPropsPage::OnBnClickedButtonXYZ()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->PrintProperty.treeitem)
		{
			this->PrintProperty.OnBnClickedButtonXYZ();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

void CPrintLocsPropsPage::SetUnits(const CUnits &u)
{
	this->PrintProperty.SetUnits(u);
}

void CPrintLocsPropsPage::OnCbnSelchangeComboProptype()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->PrintProperty.treeitem)
		{
			this->PrintProperty.OnCbnSelchangeComboProptype();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

void CPrintLocsPropsPage::OnBnClickedCheckMixture()
{
	if (this->ItemDDX)
	{
		if (this->ItemDDX == this->PrintProperty.treeitem)
		{
			this->PrintProperty.OnBnClickedCheckMixture();
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

// COMMENT: {3/9/2016 4:01:25 PM}void CPrintLocsPropsPage::OnBnClickedCheckShell()
// COMMENT: {3/9/2016 4:01:25 PM}{
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}	static UINT IDs[] =
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}	{
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}		IDC_STATIC_XW,
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}		IDC_EDIT_XW,
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}		IDC_STATIC_YW,
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}		IDC_EDIT_YW,
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}		IDC_STATIC_ZW,
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}		IDC_EDIT_ZW,
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}	};
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}	size_t n = sizeof(IDs) / sizeof(IDs[0]);
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}	CWnd *pWnd;
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}	BOOL bEnable = this->IsDlgButtonChecked(IDC_CHECK_SHELL);
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}	for (size_t i = 0; i < n; ++i)
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}	{
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}		pWnd = this->GetDlgItem(IDs[i]);
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}		ASSERT(pWnd && pWnd->GetSafeHwnd());
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}		if (pWnd)
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}		{
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}			pWnd->EnableWindow(bEnable);
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}		}
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}	}
// COMMENT: {3/9/2016 4:01:25 PM}// COMMENT: {2/11/2016 10:13:27 PM}	this->RichEditCtrl.SetWindowText(this->m_sShellRTF.c_str());
// COMMENT: {3/9/2016 4:01:25 PM}}
// COMMENT: {3/9/2016 4:00:55 PM}void CPrintLocsPropsPage::OnEnSetfocusEditXw()
// COMMENT: {3/9/2016 4:00:55 PM}{
// COMMENT: {3/9/2016 4:00:55 PM}// COMMENT: {2/11/2016 10:13:46 PM}	this->RichEditCtrl.SetWindowText(this->m_sShellRTF.c_str());
// COMMENT: {3/9/2016 4:00:55 PM}}
// COMMENT: {3/9/2016 4:00:55 PM}
// COMMENT: {3/9/2016 4:00:55 PM}void CPrintLocsPropsPage::OnEnSetfocusEditYw()
// COMMENT: {3/9/2016 4:00:55 PM}{
// COMMENT: {3/9/2016 4:00:55 PM}// COMMENT: {2/11/2016 10:13:52 PM}	this->RichEditCtrl.SetWindowText(this->m_sShellRTF.c_str());
// COMMENT: {3/9/2016 4:00:55 PM}}
// COMMENT: {3/9/2016 4:00:55 PM}
// COMMENT: {3/9/2016 4:00:55 PM}void CPrintLocsPropsPage::OnEnSetfocusEditZw()
// COMMENT: {3/9/2016 4:00:55 PM}{
// COMMENT: {3/9/2016 4:00:55 PM}// COMMENT: {2/11/2016 10:13:57 PM}	this->RichEditCtrl.SetWindowText(this->m_sShellRTF.c_str());
// COMMENT: {3/9/2016 4:00:55 PM}}
// COMMENT: {3/9/2016 4:00:55 PM}
// COMMENT: {3/9/2016 4:00:55 PM}void CPrintLocsPropsPage::OnBnSetfocusCheckShell()
// COMMENT: {3/9/2016 4:00:55 PM}{
// COMMENT: {3/9/2016 4:00:55 PM}// COMMENT: {2/11/2016 10:14:02 PM}	this->RichEditCtrl.SetWindowText(this->m_sShellRTF.c_str());
// COMMENT: {3/9/2016 4:00:55 PM}}
