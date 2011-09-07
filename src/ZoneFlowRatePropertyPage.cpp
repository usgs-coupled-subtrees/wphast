// ZoneFlowRatePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ZoneFlowRatePropertyPage.h"

#include "Global.h"

// CZoneFlowRatePropertyPage dialog

IMPLEMENT_DYNAMIC(CZoneFlowRatePropertyPage, baseCZoneFlowRatePropertyPage)

CZoneFlowRatePropertyPage::CZoneFlowRatePropertyPage()
	: baseCZoneFlowRatePropertyPage(CZoneFlowRatePropertyPage::IDD)
{
	CGlobal::LoadRTFString(this->ZFNumberRTF,    IDR_ZF_NUMBER_RTF);
	CGlobal::LoadRTFString(this->ZFDescRTF,      IDR_ZF_DESC_RTF);
	CGlobal::LoadRTFString(this->ZFComboRTF,     IDR_ZF_COMBO_RTF);
	CGlobal::LoadRTFString(this->ZFHeadsXYZTRTF, IDR_ZF_HEADS_XYZT_RTF);
}

CZoneFlowRatePropertyPage::~CZoneFlowRatePropertyPage()
{
}

void CZoneFlowRatePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	// skip CPropsPropertyPage::DoDataExchange
	CPropertyPage::DoDataExchange(pDX);

	::DDX_Control(pDX, IDC_DESC_RICHEDIT, this->RichEditCtrl);
	this->RichEditCtrl.SetTargetDevice(NULL, 0);

	::DDX_Control(pDX, IDC_COMBINATION_LIST, this->CombinationList);
	this->CombinationList.SetExtendedStyle(LVS_EX_CHECKBOXES);
	if (!pDX->m_bSaveAndValidate && (this->CombinationList.GetItemCount() != this->m_usedZoneFlowRatesNumbers.size()))
	{
		// fill combo set
		std::set<int> s;
		std::vector<int>& c = this->m_zone_budget.Get_combo();
		std::vector<int>::const_iterator combo = c.begin();
		for (; combo != c.end(); ++combo)
		{
			s.insert(std::set<int>::value_type(*combo));
		}

		CString str;
		std::set<int>::const_iterator cit = this->m_usedZoneFlowRatesNumbers.begin();
		int n = 0;
		for (; cit != this->m_usedZoneFlowRatesNumbers.end(); ++cit, ++n)
		{
			ASSERT((*cit) != this->m_zone_budget.Get_n_user());
			str.Format("%d", (*cit));
			this->CombinationList.InsertItem(n, str);
			this->CombinationList.SetItemData(n, (*cit));
			if (s.find((*cit)) != s.end())
			{
				this->CombinationList.SetCheck(n);
			}
		}		
	}

	Zone_budget zone_budget(this->m_zone_budget);

	if (pDX->m_bSaveAndValidate)
	{
		// zone_budget.n_user
		int n_user;
		::DDX_Text(pDX, IDC_NUM_EDIT, n_user);

		std::set<int>::iterator iter = this->m_usedZoneFlowRatesNumbers.find(n_user);
		if (iter != this->m_usedZoneFlowRatesNumbers.end())
		{
			::AfxMessageBox("This zone flow rate number is already in use. Please choose a different number.");
			pDX->Fail();        // throws exception
		}
		zone_budget.Set_n_user(n_user);

		// zone_budget.description
		CString str;
		::DDX_Text(pDX, IDC_DESC_EDIT, str);
		zone_budget.Set_description((const char*)str);

		// zone_budget.write_heads
		if (this->IsDlgButtonChecked(IDC_CHECK_WRITE_HEADS))
		{
			zone_budget.Set_write_heads(true);

			// zone_budget.filename_heads
			CString filename;
			::DDX_Text(pDX, IDC_EDIT_WRITE_HEADS, filename);
			if (filename.IsEmpty())
			{
				::AfxMessageBox("No filename given.");
				pDX->Fail();
			}
			else
			{
				char buf[MAX_PATH+1];
				if (!::PathSearchAndQualify(filename, buf, MAX_PATH))  // uses cwd if filename is relative
				{
					TRACE("PathSearchAndQualify failed\n");
				}
				zone_budget.Get_filename_heads() = buf;
			}
		}
		else
		{
			zone_budget.Set_write_heads(false);
		}

		// zone_budget.combo
		zone_budget.Get_combo().clear();
		int n = this->CombinationList.GetItemCount();
		for (int i = 0; i < n; ++i)
		{
			if (this->CombinationList.GetCheck(i))
			{
				int j = (int)this->CombinationList.GetItemData(i);
				zone_budget.Get_combo().push_back(j);
			}
		}

		// all ok
		this->m_zone_budget = zone_budget;
	}
	else
	{
		// zone_budget.n_user
		int n_user = zone_budget.Get_n_user();
		::DDX_Text(pDX, IDC_NUM_EDIT, n_user);

		// zone_budget.description
		CString str(zone_budget.Get_description().c_str());
		::DDX_Text(pDX, IDC_DESC_EDIT, str);

		// zone_budget.write_heads
		if (zone_budget.Get_write_heads())
		{
			this->CheckDlgButton(IDC_CHECK_WRITE_HEADS, BST_CHECKED);
		}
		else
		{
			this->CheckDlgButton(IDC_CHECK_WRITE_HEADS, BST_UNCHECKED);
		}

		// zone_budget.filename_heads
		CString filename(zone_budget.Get_filename_heads().c_str());
		::DDX_Text(pDX, IDC_EDIT_WRITE_HEADS, filename);
		
		this->OnBnClickedCheckWriteHeads();

		// zone_budget.combo (see above)
	}
}

void CZoneFlowRatePropertyPage::DDV_SoftValidate()
{
	Zone_budget zone_budget(this->m_zone_budget);

	CDataExchange dx(this, TRUE);

	// zone_budget.n_user
	int n_user;
	if (CGlobal::DDX_Text_Safe(&dx, IDC_NUM_EDIT, n_user))
	{
		std::set<int>::iterator iter = this->m_usedZoneFlowRatesNumbers.find(n_user);
		if (iter == this->m_usedZoneFlowRatesNumbers.end())
		{
			zone_budget.Set_n_user(n_user);
		}
	}

	// zone_budget.description
	CString str;
	DDX_Text(&dx, IDC_DESC_EDIT, str);
	zone_budget.Set_description((const char*)str);

	// zone_budget.combo
	zone_budget.Get_combo().clear();
	int n = this->CombinationList.GetItemCount();
	for (int i = 0; i < n; ++i)
	{
		if (this->CombinationList.GetCheck(i))
		{
			int j = (int)this->CombinationList.GetItemData(i);
			zone_budget.Get_combo().push_back(j);
		}
	}

	// all ok
	this->m_zone_budget = zone_budget;
}


BEGIN_MESSAGE_MAP(CZoneFlowRatePropertyPage, baseCZoneFlowRatePropertyPage)
	ON_EN_SETFOCUS(IDC_NUM_EDIT, &CZoneFlowRatePropertyPage::OnEnSetfocusNumEdit)
	ON_EN_SETFOCUS(IDC_DESC_EDIT, &CZoneFlowRatePropertyPage::OnEnSetfocusDescEdit)
	ON_NOTIFY(NM_SETFOCUS, IDC_COMBINATION_LIST, &CZoneFlowRatePropertyPage::OnNMSetfocusCombinationList)
	ON_BN_CLICKED(IDC_CHECK_WRITE_HEADS, &CZoneFlowRatePropertyPage::OnBnClickedCheckWriteHeads)
	ON_BN_SETFOCUS(IDC_CHECK_WRITE_HEADS, &CZoneFlowRatePropertyPage::OnBnSetfocusCheckWriteHeads)
	ON_EN_SETFOCUS(IDC_EDIT_WRITE_HEADS, &CZoneFlowRatePropertyPage::OnEnSetfocusEditWriteHeads)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CZoneFlowRatePropertyPage::OnBnClickedButtonBrowse)
	ON_BN_SETFOCUS(IDC_BUTTON_BROWSE, &CZoneFlowRatePropertyPage::OnBnSetfocusButtonBrowse)
END_MESSAGE_MAP()

// CZoneFlowRatePropertyPage message handlers

 BOOL CZoneFlowRatePropertyPage::OnInitDialog()
{
	baseCZoneFlowRatePropertyPage::OnInitDialog();

	// Add extra initialization here
#ifdef SKIP
	// Layout controls
	this->CreateRoot(VERTICAL)
		<< itemFixed(VERTICAL, 5)
		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
			<< item(IDC_NUM_STATIC, NORESIZE | ALIGN_VCENTER, 0, 0, 0, 0)
			//<< itemFixed(HORIZONTAL, 8)
			<< item(IDC_NUM_EDIT, NORESIZE | ALIGN_VCENTER, 0, 0, 0, 0)
			<< itemFixed(HORIZONTAL, 12)		
			<< item(IDC_DESC_STATIC, NORESIZE | ALIGN_VCENTER, 0, 0, 0, 0)
			<< itemFixed(HORIZONTAL, 8)
			<< item(IDC_DESC_EDIT, ABSOLUTE_VERT | ALIGN_VCENTER, 0, 0, 0, 0)
			)
		<< itemFixed(VERTICAL, 5)
		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
			<< item(IDC_COMBINATION_LIST, NORESIZE, 0, 0, 0, 0)
			)
		//<< itemFixed(VERTICAL, 3)
		<< item(IDC_DESC_RICHEDIT, ABSOLUTE_VERT)		
		;
	this->UpdateLayout();
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CZoneFlowRatePropertyPage::GetProperties(Zone_budget& zone_budget)const
{
	zone_budget = this->m_zone_budget;
}

void CZoneFlowRatePropertyPage::SetProperties(const Zone_budget& zone_budget)
{
	this->m_zone_budget = zone_budget;
}

void CZoneFlowRatePropertyPage::SetUsedZoneFlowRates(const std::set<int>& used)
{
	this->m_usedZoneFlowRatesNumbers = used;
	if (used.rbegin() != used.rend())
	{
		this->m_zone_budget.Set_n_user((*used.rbegin()) + 1);
	}
	else
	{
		this->m_zone_budget.Set_n_user(1);
	}
}

void CZoneFlowRatePropertyPage::OnEnSetfocusNumEdit()
{
	this->RichEditCtrl.SetWindowText(this->ZFNumberRTF.c_str());
}

void CZoneFlowRatePropertyPage::OnEnSetfocusDescEdit()
{
	this->RichEditCtrl.SetWindowText(this->ZFDescRTF.c_str());
}

void CZoneFlowRatePropertyPage::OnNMSetfocusCombinationList(NMHDR *pNMHDR, LRESULT *pResult)
{
	this->RichEditCtrl.SetWindowText(this->ZFComboRTF.c_str());
	*pResult = 0;
}

void CZoneFlowRatePropertyPage::OnBnClickedCheckWriteHeads()
{
	static UINT IDs[] =
	{
		IDC_EDIT_WRITE_HEADS,
		IDC_BUTTON_BROWSE,
	};

	BOOL bOn = this->IsDlgButtonChecked(IDC_CHECK_WRITE_HEADS);

	size_t n = sizeof(IDs) / sizeof(IDs[0]);
	for (size_t i = 0; i < n; ++i)
	{
		if (CWnd *pWnd = this->GetDlgItem(IDs[i]))
		{
			pWnd->EnableWindow(bOn);
		}
	}
	TRACE("Out %s\n", __FUNCTION__);
}

void CZoneFlowRatePropertyPage::OnBnClickedButtonBrowse()
{
	CString filename;
	this->GetDlgItemTextA(IDC_EDIT_WRITE_HEADS, filename);

	CFileDialog fileDlg(FALSE, NULL, (filename.IsEmpty()) ? NULL : filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, this);

	if( fileDlg.DoModal() == IDOK )
	{
		this->GetDlgItem(IDC_EDIT_WRITE_HEADS)->SetWindowText(fileDlg.GetPathName());
	}
}

void CZoneFlowRatePropertyPage::OnBnSetfocusCheckWriteHeads()
{
	this->RichEditCtrl.SetWindowText(this->ZFHeadsXYZTRTF.c_str());
}

void CZoneFlowRatePropertyPage::OnEnSetfocusEditWriteHeads()
{
	this->RichEditCtrl.SetWindowText(this->ZFHeadsXYZTRTF.c_str());
}

void CZoneFlowRatePropertyPage::OnBnSetfocusButtonBrowse()
{
	this->RichEditCtrl.SetWindowText(this->ZFHeadsXYZTRTF.c_str());
}
