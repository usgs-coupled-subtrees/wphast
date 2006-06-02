#pragma once
#include "afxcmn.h"

//// #include "structs.h"
#include "property.h"


// CProtoPropertyPage dialog

class CProtoPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CProtoPropertyPage)

public:
	CProtoPropertyPage();
	virtual ~CProtoPropertyPage();

// Dialog Data
	enum { IDD = IDD_PROTO_PROPPAGE };

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

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_wndTreeCtrl;
	HTREEITEM m_htiDDX;

	void SetProperties(const Cproperty& active, const Cproperty& kx, const Cproperty& ky, const Cproperty& porosity);
	void GetProperties(Cproperty& active, Cproperty& kx, Cproperty& ky, Cproperty& porosity);

	afx_msg void OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickPropTree(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);

	afx_msg void OnTvnKeydownPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSingleValueRadio();
	afx_msg void OnBnClickedLinearInterpolationRadio();
};
