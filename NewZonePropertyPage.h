#pragma once
#include "afxcmn.h"


// CNewZonePropertyPage dialog

class CNewZonePropertyPage : public CPropertyPage
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
	
	UINT m_type;
	CArray<CPropertyPage*, CPropertyPage*> m_PropPageArray;
public:
	virtual BOOL OnSetActive();
	UINT GetType(void);
	virtual LRESULT OnWizardNext();
	afx_msg void OnNMDblclkTreeZones(NMHDR *pNMHDR, LRESULT *pResult);
};
