#pragma once

#include <set>
#include "srcinput/Zone_budget.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"
#include "afxcmn.h"

#ifndef baseCZoneFlowRatePropertyPage
//#define baseCZoneFlowRatePropertyPage ETSLayoutPropertyPageXP
#define baseCZoneFlowRatePropertyPage CPropertyPage
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

// COMMENT: {2/18/2009 9:16:28 PM}	void SetDesc(LPCTSTR desc) { m_desc = desc; }
// COMMENT: {2/18/2009 9:16:28 PM}	LPCTSTR GetDesc() { return m_desc.c_str(); }

// Dialog Data
	enum { IDD = IDD_FLOW_RATE_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// COMMENT: {2/18/2009 9:16:32 PM}	std::string  m_desc;

	Zone_budget  m_zone_budget;
	std::set<int> m_usedZoneFlowRatesNumbers;

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl CombinationList;

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
};
