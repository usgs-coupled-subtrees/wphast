#pragma once

#include "HeadIC.h"
#include "CheckTreeCtrl.h"
#include <string>
#include "afxcmn.h"

// CICHeadPropertyPage dialog

class CICHeadPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CICHeadPropertyPage)

public:
	CICHeadPropertyPage();
	CICHeadPropertyPage(UINT nIDCaption);
	CICHeadPropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

	virtual ~CICHeadPropertyPage();

	void SetProperties(const CHeadIC& r_headIC);
	void GetProperties(CHeadIC& r_headIC)const;

// Dialog Data
	enum { IDD = IDD_IC_HEAD_EXT_PROPPAGE };
	enum { IDD_INTERIOR = IDD_IC_HEAD_INT_PROPPAGE };

protected:
	void CommonConstruct(void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	//{{USER
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam);
	//}}USER
	afx_msg void OnBnClickedSingleValueRadio();
	afx_msg void OnBnClickedLinearInterpolationRadio();


	CCheckTreeCtrl m_wndTreeCtrl;
	CRichEditCtrl m_wndRichEditCtrl;
	HTREEITEM m_htiDDX;

	CHeadIC m_headIC;
	CHeadIC m_headICFixed;
	CHeadIC m_headICLinear;

	std::string m_sMaskRTF;    // IDR_IC_HEAD_MASK_RTF
	std::string m_sHeadRTF;    // IDR_IC_HEAD_HEAD_RTF
public:
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
};
