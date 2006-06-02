#pragma once
#include "afxcmn.h"

#include "CheckListCtrl.h"
#include "property.h"

// CProto2PropertyPage dialog

class CProto2PropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CProto2PropertyPage)

public:
	CProto2PropertyPage();
	virtual ~CProto2PropertyPage();

// Dialog Data
	enum { IDD = IDD_PROTO2_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	int m_itemIndex;

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
	CCheckListCtrl m_wndCheckListCtrl;
	//CListCtrl m_wndCheckListCtrl;
	void SetProperties(const Cproperty& active, const Cproperty& kx, const Cproperty& ky, const Cproperty& porosity);
	// void GetProperties(property& active, property& kx, property& ky, property& porosity);

	afx_msg void OnLvnItemchangedPropList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangingPropList(NMHDR *pNMHDR, LRESULT *pResult);
};
