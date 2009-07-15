#pragma once

#include <set>
#include "PropsPropertyPage.h"
#include "srcinput/Zone_budget.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"
#include "afxcmn.h"

#ifndef baseCZoneFlowRatePropertyPage
//#define baseCZoneFlowRatePropertyPage ETSLayoutPropertyPageXP
//#define baseCZoneFlowRatePropertyPage CPropertyPage
#define baseCZoneFlowRatePropertyPage CPropsPropertyPage
#endif

// CZoneFlowRatePropertyPage dialog

class CZoneFlowRatePropertyPage : public baseCZoneFlowRatePropertyPage
{
	DECLARE_DYNAMIC(CZoneFlowRatePropertyPage)

public:
	CZoneFlowRatePropertyPage();
	virtual ~CZoneFlowRatePropertyPage();

	void SetProperties(const Zone_budget& zone_budget);
	void GetProperties(Zone_budget& zone_budget)const;

	void SetUsedZoneFlowRates(const std::set<int>& used);

// Dialog Data
	enum { IDD = IDD_FLOW_RATE_PROPPAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void DDV_SoftValidate();

	Zone_budget  m_zone_budget;
	std::set<int> m_usedZoneFlowRatesNumbers;

	DECLARE_MESSAGE_MAP()

	CListCtrl CombinationList;

public:
	virtual BOOL OnInitDialog();
};
