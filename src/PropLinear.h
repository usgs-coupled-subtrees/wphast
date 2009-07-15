#pragma once

#include "PropPage.h"

#include "afxwin.h"
#include "resource.h"

#include "property.h"

// CPropLinear dialog

class CPropLinear : public CPropPage
{
	DECLARE_DYNAMIC(CPropLinear)

public:
	CPropLinear();
	virtual ~CPropLinear();

	Cproperty GetProperty();
	void SetProperty(Cproperty p);

// Dialog Data
	enum { IDD = IDD_PROP_LINEAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	Cproperty prop;

public:
	virtual BOOL OnInitDialog();
};
