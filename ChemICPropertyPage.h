#pragma once

#include "ChemIC.h"
#include "CheckTreeCtrl.h"
#include "resource.h"
#include <string>

// CChemICPropertyPage dialog

class CChemICPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CChemICPropertyPage)

public:
	CChemICPropertyPage();
	CChemICPropertyPage(UINT nIDTemplate, UINT nIDCaption = IDS_CHEM_IC_CAPTION161);
	CChemICPropertyPage(UINT nIDTemplate, UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

	virtual ~CChemICPropertyPage();

	void SetProperties(const CChemIC& chemIC);
	void GetProperties(CChemIC& chemIC)const;

	void SetNewModelWizard(bool bNewModelWizard)     { m_bNewModelWizard = bNewModelWizard; }
	bool GetNewModelWizard(void)const                { return m_bNewModelWizard; }

	void SetFlowOnlyWizard(bool bFlowOnlyWizard)     { m_bFlowOnlyWizard = bFlowOnlyWizard; }
	bool GetFlowOnlyWizard(void)const                { return m_bFlowOnlyWizard; }

	void SetShowFlowOnlyMessage(bool bShowFlowOnlyMessage)     { m_bShowFlowOnlyMessage = bShowFlowOnlyMessage; }
	bool GetShowFlowOnlyMessage(void)const                     { return m_bShowFlowOnlyMessage; }

	void SetFlowOnly(bool bFlowOnly)     { m_bFlowOnly = bFlowOnly; }
	bool GetFlowOnly(void)const          { return m_bFlowOnly; }


// Dialog Data
	enum { IDD = IDD_MEDIA_EXT_PROPPAGE };
	enum { IDD_INTERIOR = IDD_MEDIA_INT_PROPPAGE };

protected:
	void CommonConstruct(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	//{{USER
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam);
	//}}USER
	afx_msg void OnBnClickedSingleValueRadio();
	afx_msg void OnBnClickedLinearInterpolationRadio();

protected:
	CCheckTreeCtrl m_wndTreeCtrl;
	CRichEditCtrl m_wndRichEditCtrl;
	HTREEITEM m_htiDDX;

	CChemIC m_chem_ic;
	CChemIC m_chem_icFixed;
	CChemIC m_chem_icLinear;

	bool m_bFirstPaint;
	bool m_bNewModelWizard;
	bool m_bFlowOnlyWizard;
	bool m_bShowFlowOnlyMessage;
	bool m_bFlowOnly;

	std::string m_sSolutionRTF;           // IDR_CHEM_IC_SOLUTION_RTF
	std::string m_sEquilibriumPhasesRTF;  // IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF
	std::string m_sSurfaceRTF;            // IDR_CHEM_IC_SURFACE_RTF
	std::string m_sExchangeRTF;           // IDR_CHEM_IC_EXCHANGE_RTF
	std::string m_sGasPhaseRTF;           // IDR_CHEM_IC_GAS_PHASE_RTF
	std::string m_sSolidSolutionsRTF;     // IDR_CHEM_IC_SOLID_SOLUTIONS_RTF
	std::string m_sKineticsRTF;           // IDR_CHEM_IC_KINETICS_RTF

public:
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
