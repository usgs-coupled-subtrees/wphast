#pragma once

#include "PropPage.h"

#include "afxwin.h"
#include "resource.h"
#include "property.h"


// CPropXYZ dialog

class CPropXYZ : public CPropPage
{
	DECLARE_DYNAMIC(CPropXYZ)

public:
	CPropXYZ();
	virtual ~CPropXYZ();

	Cproperty GetProperty();
	void SetProperty(Cproperty p);

// Dialog Data
	enum { IDD = IDD_PROP_XYZ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	Cproperty prop;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonXyz();
};
