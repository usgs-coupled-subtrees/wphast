#pragma once

#include <string>
#include "ChemIC.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"
#include "afxcmn.h"


// CChemICSpreadPropertyPage dialog

class CChemICSpreadPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CChemICSpreadPropertyPage)

public:
	CChemICSpreadPropertyPage();
	virtual ~CChemICSpreadPropertyPage();

	void SetProperties(const CChemIC& chemIC);
	void GetProperties(CChemIC& chemIC)const;

	void SetFlowOnly(bool bFlowOnly)     { m_bFlowOnly = bFlowOnly; }
	bool GetFlowOnly(void)const          { return m_bFlowOnly; }

// Dialog Data
	enum { IDD = IDD_CHEMICSPREAD_PROPPAGE };

protected:
	void CommonConstruct(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL SetupGrids(void);

	CChemIC m_chemIC;

	CModGridCtrlEx m_gridChemIC;  // IDC_CHECMIC_GRID

	std::string m_sSolutionRTF;           // IDR_CHEM_IC_SOLUTION_RTF
	std::string m_sEquilibriumPhasesRTF;  // IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF
	std::string m_sSurfaceRTF;            // IDR_CHEM_IC_SURFACE_RTF
	std::string m_sExchangeRTF;           // IDR_CHEM_IC_EXCHANGE_RTF
	std::string m_sGasPhaseRTF;           // IDR_CHEM_IC_GAS_PHASE_RTF
	std::string m_sSolidSolutionsRTF;     // IDR_CHEM_IC_SOLID_SOLUTIONS_RTF
	std::string m_sKineticsRTF;           // IDR_CHEM_IC_KINETICS_RTF

	bool m_bFlowOnly;

	DECLARE_MESSAGE_MAP()

	afx_msg void OnCheckChangedChemIC(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedChemIC(NMHDR *pNotifyStruct, LRESULT *result);

public:
	virtual BOOL OnInitDialog();
	CRichEditCtrl m_wndRichEditCtrl;
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardNext();
};
