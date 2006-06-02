#pragma once

#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"

#include "BC.h"

#ifndef baseCBCLeakyPropertyPage2
#define baseCBCLeakyPropertyPage2 ETSLayoutPropertyPageXP
// COMMENT: {4/21/2006 6:48:17 PM}#define baseCBCLeakyPropertyPage2 CPropertyPage
#endif

// CBCLeakyPropertyPage2 dialog

class CBCLeakyPropertyPage2 : public baseCBCLeakyPropertyPage2
{
	DECLARE_DYNAMIC(CBCLeakyPropertyPage2)

public:
	CBCLeakyPropertyPage2();
	virtual ~CBCLeakyPropertyPage2();

	void SetProperties(const CBC& r_bc);
	void GetProperties(CBC& r_bc);

	void SetFlowOnly(bool bFlowOnly)     { m_bFlowOnly = bFlowOnly; }
	bool GetFlowOnly(void)const          { return m_bFlowOnly; }

	void SetDesc(LPCTSTR desc) { m_desc = desc; }
	LPCTSTR GetDesc() { return m_desc.c_str(); }

// Dialog Data
	enum { IDD = IDD_BC_LEAKY_PROPPAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL SetupGrids(void);

	CBC m_bc;
	CModGridCtrlEx   m_gridHead;     // IDC_HEAD_GRID
	CModGridCtrlEx   m_gridSolution; // IDC_SOLUTION_GRID
	CModGridCtrlEx   m_gridSingle;   // IDC_SINGLE_GRID
	bool             m_bFlowOnly;
	std::string      m_desc;

	CRichEditCtrl m_wndRichEditCtrl;

	std::string m_sDescriptionRTF;   // IDR_DESCRIPTION_RTF
	std::string m_sHeadRTF;          // IDR_BC_LEAKY_HEAD_RTF
	std::string m_sThicknessRTF;     // IDR_BC_LEAKY_THICKNESS_RTF
	std::string m_sHydCondRTF;       // IDR_BC_LEAKY_HYD_COND_RTF
	std::string m_sAssocSolutionRTF; // IDR_BC_LEAKY_ASSOC_SOL_RTF
	std::string m_sFaceRTF;          // IDR_BC_LEAKY_FACE_RTF

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCheckChangedHead(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnCheckChangedSolution(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnCheckChangedSingle(NMHDR *pNotifyStruct, LRESULT *result);

	afx_msg void OnBnClickedFace();

	afx_msg void OnSelChangedHead(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedSingle(NMHDR *pNotifyStruct, LRESULT *result);

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnEnSetfocusDescEdit();
	afx_msg void OnBnSetfocusFaceXRadio();
	afx_msg void OnBnSetfocusFaceYRadio();
	afx_msg void OnBnSetfocusFaceZRadio();
};
