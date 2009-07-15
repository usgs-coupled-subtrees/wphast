#pragma once

#include "PropPage.h"

#include "afxwin.h"
#include "resource.h"
#include "property.h"

// COMMENT: {5/27/2009 2:18:30 PM}#define __MULTIPLE__

#if defined(__MULTIPLE__)
#include "gridctrl/ModGridCtrlEx.h"
#endif



// CPropConstant dialog

class CPropConstant : public CPropPage
{
	DECLARE_DYNAMIC(CPropConstant)

public:
	CPropConstant();
	virtual ~CPropConstant();

	Cproperty GetProperty();
	void SetProperty(Cproperty p);

// Dialog Data
#if defined(__MULTIPLE__)
	enum { IDD = IDD_PROP_CONSTANT_M };
#else
	enum { IDD = IDD_PROP_CONSTANT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	Cproperty prop;

#if defined(__MULTIPLE__)
	CModGridCtrlEx   m_wndTimesGrid;      // IDC_TIMES_GRID
#endif

public:
	virtual BOOL OnInitDialog();
};
