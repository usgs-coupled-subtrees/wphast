// ZoneFlowRatePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "ZoneFlowRatePropertyPage.h"


// CZoneFlowRatePropertyPage dialog

IMPLEMENT_DYNAMIC(CZoneFlowRatePropertyPage, baseCZoneFlowRatePropertyPage)

CZoneFlowRatePropertyPage::CZoneFlowRatePropertyPage()
	: baseCZoneFlowRatePropertyPage(CZoneFlowRatePropertyPage::IDD)
{
}

CZoneFlowRatePropertyPage::~CZoneFlowRatePropertyPage()
{
}

void CZoneFlowRatePropertyPage::DoDataExchange(CDataExchange* pDX)
{
// COMMENT: {7/13/2009 7:20:16 PM}	baseCZoneFlowRatePropertyPage::DoDataExchange(pDX);
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBINATION_LIST, this->CombinationList);
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
		DDX_Text(pDX, IDC_NUM_EDIT, n_user);

		std::set<int>::iterator iter = this->m_usedZoneFlowRatesNumbers.find(n_user);
		if (iter != this->m_usedZoneFlowRatesNumbers.end())
		{
			::AfxMessageBox("This zone flow rate number is already in use. Please choose a different number.");
			pDX->Fail();        // throws exception
		}
		zone_budget.Set_n_user(n_user);

		// zone_budget.description
		CString str;
		DDX_Text(pDX, IDC_DESC_EDIT, str);
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
	else
	{
		// zone_budget.n_user
		int n_user = zone_budget.Get_n_user();
		DDX_Text(pDX, IDC_NUM_EDIT, n_user);

		// zone_budget.description
		CString str(zone_budget.Get_description().c_str());
		DDX_Text(pDX, IDC_DESC_EDIT, str);

		// zone_budget.combo (see above)
	}
}


BEGIN_MESSAGE_MAP(CZoneFlowRatePropertyPage, baseCZoneFlowRatePropertyPage)
END_MESSAGE_MAP()

// CZoneFlowRatePropertyPage message handlers

 BOOL CZoneFlowRatePropertyPage::OnInitDialog()
{
	baseCZoneFlowRatePropertyPage::OnInitDialog();

	// Add extra initialization here

// COMMENT: {2/18/2009 4:18:25 PM}	// Layout controls
// COMMENT: {2/18/2009 4:18:25 PM}	this->CreateRoot(VERTICAL)
// COMMENT: {2/18/2009 4:18:25 PM}		<< itemFixed(VERTICAL, 5)
// COMMENT: {2/18/2009 4:18:25 PM}		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
// COMMENT: {2/18/2009 4:18:25 PM}			<< item(IDC_NUM_STATIC, NORESIZE | ALIGN_VCENTER, 0, 0, 0, 0)
// COMMENT: {2/18/2009 4:18:25 PM}			//<< itemFixed(HORIZONTAL, 8)
// COMMENT: {2/18/2009 4:18:25 PM}			<< item(IDC_NUM_EDIT, NORESIZE | ALIGN_VCENTER, 0, 0, 0, 0)
// COMMENT: {2/18/2009 4:18:25 PM}			<< itemFixed(HORIZONTAL, 12)		
// COMMENT: {2/18/2009 4:18:25 PM}			<< item(IDC_DESC_STATIC, NORESIZE | ALIGN_VCENTER, 0, 0, 0, 0)
// COMMENT: {2/18/2009 4:18:25 PM}			<< itemFixed(HORIZONTAL, 8)
// COMMENT: {2/18/2009 4:18:25 PM}			<< item(IDC_DESC_EDIT, ABSOLUTE_VERT | ALIGN_VCENTER, 0, 0, 0, 0)
// COMMENT: {2/18/2009 4:18:25 PM}			)
// COMMENT: {2/18/2009 4:18:25 PM}		<< itemFixed(VERTICAL, 5)
// COMMENT: {2/18/2009 4:18:25 PM}		<< 	( pane(HORIZONTAL, ABSOLUTE_VERT, 0, 0, 0 )
// COMMENT: {2/18/2009 4:18:25 PM}			<< item(IDC_COMBINATION_LIST, NORESIZE, 0, 0, 0, 0)
// COMMENT: {2/18/2009 4:18:25 PM}			)
// COMMENT: {2/18/2009 4:18:25 PM}// COMMENT: {2/18/2009 4:06:26 PM}		<< item(IDC_SINGLE_GRID, ABSOLUTE_VERT)
// COMMENT: {2/18/2009 4:18:25 PM}// COMMENT: {2/18/2009 4:06:26 PM}		<< item(IDC_HEAD_GRID, GREEDY)
// COMMENT: {2/18/2009 4:18:25 PM}// COMMENT: {2/18/2009 4:06:26 PM}		<< item(IDC_SOLUTION_GRID, GREEDY)
// COMMENT: {2/18/2009 4:18:25 PM}		//<< itemFixed(VERTICAL, 3)
// COMMENT: {2/18/2009 4:18:25 PM}		<< item(IDC_DESC_RICHEDIT, ABSOLUTE_VERT)		
// COMMENT: {2/18/2009 4:18:25 PM}		;
// COMMENT: {2/18/2009 4:18:25 PM}	this->UpdateLayout();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CZoneFlowRatePropertyPage::OnSetActive()
{
	BOOL bRet = CPropertyPage::OnSetActive();

	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	}
	return bRet;
}

BOOL CZoneFlowRatePropertyPage::OnKillActive()
{
	CPropertySheet* pSheet = static_cast<CPropertySheet*>(this->GetParent());
	ASSERT_KINDOF(CPropertySheet, pSheet);
	if (pSheet->IsWizard())
	{
		return TRUE;
	}
	return CPropertyPage::OnKillActive();
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
