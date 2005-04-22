#pragma once

#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"

////#include "BC.h"

#ifndef baseCTimeControlMultiPropertyPage2
#define baseCTimeControlMultiPropertyPage2 ETSLayoutPropertyPageXP
#endif

// CTimeControlMultiPropertyPage2 dialog

class CTimeControlMultiPropertyPage2 : public baseCTimeControlMultiPropertyPage2
{
	DECLARE_DYNAMIC(CTimeControlMultiPropertyPage2)

public:
	CTimeControlMultiPropertyPage2();
	virtual ~CTimeControlMultiPropertyPage2();

// Dialog Data
	enum { IDD = IDD_TC_INT_M_PROPPAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL SetupGrids(void);

	CModGridCtrlEx   m_gridTimeEnd;     // IDC_TIMEEND_GRID
	CModGridCtrlEx   m_gridTimeStep;    // IDC_TIMESTEP_GRID

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
