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
}

CZoneFlowRatePropertyPage::~CZoneFlowRatePropertyPage()
{
}

void CZoneFlowRatePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	// skip CPropsPropertyPage::DoDataExchange
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
