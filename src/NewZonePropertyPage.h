#pragma once
#include "afxcmn.h"

#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"

#ifndef baseCNewZonePropertyPage
#define baseCNewZonePropertyPage ETSLayoutPropertyPageXP
#endif

// CNewZonePropertyPage dialog

class CNewZonePropertyPage : public baseCNewZonePropertyPage
{
	DECLARE_DYNAMIC(CNewZonePropertyPage)

public:
	CNewZonePropertyPage();
	virtual ~CNewZonePropertyPage();

// Dialog Data
	enum { IDD = IDD_NEW_ZONE_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_wndTree;
	virtual BOOL OnInitDialog();
protected:
	HTREEITEM m_htiMedia;
	HTREEITEM m_htiBC;
	HTREEITEM m_htiIC;	
	HTREEITEM m_htiBCFlux;
	HTREEITEM m_htiBCLeaky;
	HTREEITEM m_htiBCSpec;
	HTREEITEM m_htiICHead;
	HTREEITEM m_htiChemIC;
	HTREEITEM m_htiFlowRate;
	
	UINT m_type;
	CArray<CPropertyPage*, CPropertyPage*> m_PropPageArray;
public:
	virtual BOOL OnSetActive();
	UINT GetType(void);
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	afx_msg void OnNMDblclkTreeZones(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTVNSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
};
