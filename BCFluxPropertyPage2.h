#pragma once

#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"

#include "BC.h"

#ifndef baseCBCFluxPropertyPage2
#define baseCBCFluxPropertyPage2 ETSLayoutPropertyPageXP
//#define baseCBCFluxPropertyPage2 CPropertyPage
#endif


// CBCFluxPropertyPage2 dialog

class CBCFluxPropertyPage2 : public baseCBCFluxPropertyPage2
{
	DECLARE_DYNAMIC(CBCFluxPropertyPage2)

public:
	CBCFluxPropertyPage2();
	virtual ~CBCFluxPropertyPage2();

	void SetProperties(const CBC& r_bc);
	void GetProperties(CBC& r_bc);

	void SetFlowOnly(bool bFlowOnly)     { m_bFlowOnly = bFlowOnly; }
	bool GetFlowOnly(void)const          { return m_bFlowOnly; }

	void SetDesc(LPCTSTR desc) { m_desc = desc; }
	LPCTSTR GetDesc() { return m_desc.c_str(); }

// Dialog Data
	enum { IDD = IDD_BC_FLUX_PROPPAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL SetupGrids(void);

	CBC m_bc;
	CModGridCtrlEx   m_gridFlux;     // IDC_FLUX_GRID
	CModGridCtrlEx   m_gridSolution; // IDC_SOLUTION_GRID
	bool             m_bFlowOnly;
	std::string      m_desc;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCheckChangedFlux(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnCheckChangedSolution(NMHDR *pNotifyStruct, LRESULT *result);

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedFace();
};
