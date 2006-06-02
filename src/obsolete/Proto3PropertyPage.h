#pragma once

#include "property.h"
#include "CheckTreeCtrl.h"
#include <string>

// CProto3PropertyPage dialog

class CProto3PropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CProto3PropertyPage)

public:
	CProto3PropertyPage();
	virtual ~CProto3PropertyPage();

	void SetProperties(const Cproperty& active, const Cproperty& kx, const Cproperty& ky, const Cproperty& porosity);
	void GetProperties(Cproperty& active, Cproperty& kx, Cproperty& ky, Cproperty& porosity)const;


// Dialog Data
	enum { IDD = IDD_PROTO3_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	Cproperty m_propActive;
	Cproperty m_propKx;
	Cproperty m_propKy;
	Cproperty m_propPorosity;

	Cproperty m_propActiveFixed;
	Cproperty m_propKxFixed;
	Cproperty m_propKyFixed;
	Cproperty m_propPorosityFixed;

	Cproperty m_propActiveLinear;
	Cproperty m_propKxLinear;
	Cproperty m_propKyLinear;
	Cproperty m_propPorosityLinear;

	std::string m_sActiveRTF;
	std::string m_sKxRTF;
	std::string m_sKyRTF;
	std::string m_sPorosityRTF;

	DECLARE_MESSAGE_MAP()
public:
	CCheckTreeCtrl m_wndTreeCtrl;
	CRichEditCtrl m_wndRichEditCtrl;
	HTREEITEM m_htiDDX;
	afx_msg void OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	//{{USER
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam);
	//}}USER
	afx_msg void OnBnClickedSingleValueRadio();
	afx_msg void OnBnClickedLinearInterpolationRadio();
};
