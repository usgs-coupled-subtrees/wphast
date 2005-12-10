#pragma once

#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"
#include "PrintInput.h"

#ifndef baseCPrintInitialPropertyPage
#define baseCPrintInitialPropertyPage ETSLayoutPropertyPageXP
#endif

// CPrintInitialPropertyPage dialog

class CPrintInitialPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CPrintInitialPropertyPage)

public:
	CPrintInitialPropertyPage();
	virtual ~CPrintInitialPropertyPage();

public:
	CPrintInput GetData(void)const         { return this->m_PrintInput; }
	void SetData(CPrintInput printInput)   { this->m_PrintInput = printInput; }

public:
// Dialog Data
	enum { IDD = IDD_PRINT_INITIAL_EXT_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CModGridCtrlEx  m_GridLeft;   // IDC_GRID_LEFT
	CModGridCtrlEx  m_GridRight;  // IDC_GRID_RIGHT
	CPrintInput     m_PrintInput;
};
