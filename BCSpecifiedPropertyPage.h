#pragma once

#include "BCZone.h"
#include "CheckTreeCtrl.h"
#include <string>

// CBCSpecifiedPropertyPage dialog

class CBCSpecifiedPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CBCSpecifiedPropertyPage)

public:
	CBCSpecifiedPropertyPage();
	virtual ~CBCSpecifiedPropertyPage();

	void SetProperties(const CBCZone& r_bc);
	void GetProperties(CBCZone& r_bc);

	void SetStressPeriod(int nStressPeriod) { m_nStressPeriod = nStressPeriod;}
	int  GetStressPeriod(void)const         { return m_nStressPeriod; }

// Dialog Data
	enum { IDD = IDD_BC_SPECIFIED_PROPPAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void _SetProperties(const CBCZone& r_bc);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	//{{USER
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam);
	//}}USER
	afx_msg void OnBnClickedSingleValueRadio();
	afx_msg void OnBnClickedLinearInterpolationRadio();
protected:
	CCheckTreeCtrl m_wndTreeCtrl;
	CRichEditCtrl m_wndRichEditCtrl;
	HTREEITEM m_htiDDX;

	CBCZone m_bc;
	CBCZone m_bcFixed;
	CBCZone m_bcLinear;

	std::string m_sHeadRTF;      // IDR_BC_SPECIFIED_HEAD_RTF
	std::string m_sSolutionRTF;  // IDR_BC_SPECIFIED_SOL_RTF

	int m_nStressPeriod;
public:
	virtual BOOL OnSetActive();
};
