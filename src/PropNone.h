#pragma once

#include "PropPage.h"

#include "afxwin.h"
#include "resource.h"

// CPropNone dialog

class CPropNone : public CPropPage
{
	DECLARE_DYNAMIC(CPropNone)

public:
	CPropNone();
	virtual ~CPropNone();

// Dialog Data
	enum { IDD = IDD_PROP_NONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
};
