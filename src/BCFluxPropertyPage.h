#pragma once

#include "BC.h"
#include "CheckTreeCtrl.h"
#include <string>

// CBCFluxPropertyPage dialog

class CBCFluxPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CBCFluxPropertyPage)

public:
	CBCFluxPropertyPage();
	virtual ~CBCFluxPropertyPage();

	void SetProperties(const CBC& r_bc);
	void GetProperties(CBC& r_bc);

	void SetStressPeriod(int nStressPeriod) { m_nStressPeriod = nStressPeriod;}
	int  GetStressPeriod(void)const         { return m_nStressPeriod; }

// Dialog Data
	enum { IDD = IDD_BC_FLUX_PROPPAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void _SetProperties(const CBC& r_bc);


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

	CBC m_bc;
	CBC m_bcFixed;
	CBC m_bcLinear;

	std::string m_sAssocSolutionRTF; // IDR_BC_FLUX_ASSOC_SOL_RTF
	std::string m_sFluxRTF;          // IDR_BC_FLUX_FLUX_RTF

	int m_nStressPeriod;
public:
	virtual BOOL OnSetActive();
};
