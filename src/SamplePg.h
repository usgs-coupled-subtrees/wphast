#pragma once

#include "resource.h"

// CSamplePg dialog

class CSamplePg : public CPropertyPage
{
	DECLARE_DYNAMIC(CSamplePg)

public:
	CSamplePg();
	virtual ~CSamplePg();

// Dialog Data
	enum { IDD = IDD_PL_SAMPLE };

	int thin_grid[3];

	int spinMax[3];


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void UpdateSpinners();

	DECLARE_MESSAGE_MAP()

	BOOL AxisEnabled[3];

	CSpinButtonCtrl spinFreq[3];

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckX2();
	afx_msg void OnBnClickedCheckY2();
	afx_msg void OnBnClickedCheckZ2();
};
