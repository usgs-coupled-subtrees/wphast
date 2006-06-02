#pragma once

#include <string>
#include "HeadIC.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"
#include "afxcmn.h"

// CICHeadSpreadPropertyPage dialog

class CICHeadSpreadPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CICHeadSpreadPropertyPage)

public:
	CICHeadSpreadPropertyPage();
	virtual ~CICHeadSpreadPropertyPage();

	void SetProperties(const CHeadIC& headIC);
	void GetProperties(CHeadIC& headIC)const;

	void SetDesc(LPCTSTR desc) { m_desc = desc; }
	LPCTSTR GetDesc() { return m_desc.c_str(); }

// Dialog Data
	enum { IDD = IDD_IC_HEAD_SPREAD_PROPPAGE };

protected:
	void CommonConstruct(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL SetupGrids(void);

	CHeadIC      m_headIC;
	std::string  m_desc;

	CModGridCtrlEx m_gridHeadIC;  // IDC_HEADIC_GRID

	std::string m_sDescriptionRTF;  // IDR_DESCRIPTION_RTF
	std::string m_sHeadRTF;         // IDR_IC_HEAD_HEAD_RTF

	DECLARE_MESSAGE_MAP()

	afx_msg void OnCheckChangedHeadIC(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedHeadIC(NMHDR *pNotifyStruct, LRESULT *result);

public:
	virtual BOOL OnInitDialog();
	CRichEditCtrl m_wndRichEditCtrl;
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardNext();
	afx_msg void OnEnSetfocusDescEdit();
};
