#pragma once
#include "afxcmn.h"


// CTempPropertyPage dialog

class CTempPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTempPropertyPage)

public:
	CTempPropertyPage();
	virtual ~CTempPropertyPage();

// Dialog Data
	enum { IDD = IDD_PROPS_CUBE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedApply();
	CString m_strHeightUnits;
};
