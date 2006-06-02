#pragma once

#include "gridctrl/ModGridCtrlEx.h"
#include "gridctrl/DropDownButton.h"


// TestCModGridCtrlExPage dialog

class TestCModGridCtrlExPage : public CPropertyPage
{
	DECLARE_DYNAMIC(TestCModGridCtrlExPage)

public:
	TestCModGridCtrlExPage();
	virtual ~TestCModGridCtrlExPage();

// Dialog Data
	enum { IDD = IDD_TEST_CMODGRIDCTRLEX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CModGridCtrlEx   m_Grid;              // IDC_GRID
	CComboBox        m_wndComboBox;       // IDC_COMBO1
	CDropDownButton  m_wndDropDownButton; // IDC_BUTTON1

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton2();
};
