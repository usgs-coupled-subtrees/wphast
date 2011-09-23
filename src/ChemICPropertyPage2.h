#pragma once

#include <string>
#include "ChemIC.h"
#include "Units.h"
#include "gridctrl/ModGridCtrlEx.h"

// CChemICPropertyPage2 dialog

class CChemICPropertyPage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CChemICPropertyPage2)

public:
	CChemICPropertyPage2();
	CChemICPropertyPage2(UINT nIDTemplate, UINT nIDCaption = IDS_CHEM_IC_CAPTION161);
	CChemICPropertyPage2(UINT nIDTemplate, UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

	virtual ~CChemICPropertyPage2();

	void SetProperties(const CChemIC& chemIC);
	void GetProperties(CChemIC& chemIC)const;

	void SetNewModelWizard(bool newModelWizard)     { this->NewModelWizard = newModelWizard; }
	bool GetNewModelWizard(void)const               { return this->NewModelWizard; }

	void SetFlowOnlyWizard(bool flowOnlyWizard)     { this->FlowOnlyWizard = flowOnlyWizard; }
	bool GetFlowOnlyWizard(void)const               { return this->FlowOnlyWizard; }

	void SetShowFlowOnlyMessage(bool showFlowOnlyMessage)      { this->ShowFlowOnlyMessage = showFlowOnlyMessage; }
	bool GetShowFlowOnlyMessage(void)const                     { return this->ShowFlowOnlyMessage; }

	void SetFlowOnly(bool flowOnly)                 { this->FlowOnly = flowOnly; }
	bool GetFlowOnly(void)const                     { return this->FlowOnly; }


// Dialog Data
	enum { IDD = IDD_IC_CHEM_INT_PROPPAGE };

protected:
	void CommonConstruct(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CModGridCtrlEx Grid;
	CRichEditCtrl  RichEditCtrl;

	CChemIC        ChemIC;

	bool           NewModelWizard;
	bool           FlowOnlyWizard;
	bool           FlowOnly;
	bool           FirstPaint;
	bool           ShowFlowOnlyMessage;

	std::string    SolutionRTF;           // IDR_CHEM_IC_SOLUTION_RTF
	std::string    EquilibriumPhasesRTF;  // IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF
	std::string    SurfaceRTF;            // IDR_CHEM_IC_SURFACE_RTF
	std::string    ExchangeRTF;           // IDR_CHEM_IC_EXCHANGE_RTF
	std::string    GasPhaseRTF;           // IDR_CHEM_IC_GAS_PHASE_RTF
	std::string    SolidSolutionsRTF;     // IDR_CHEM_IC_SOLID_SOLUTIONS_RTF
	std::string    KineticsRTF;           // IDR_CHEM_IC_KINETICS_RTF

public:
	virtual BOOL OnSetActive();
	afx_msg void OnPaint();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardNext();
	afx_msg void OnSelChanged(NMHDR *pNotifyStruct, LRESULT *result);
};
