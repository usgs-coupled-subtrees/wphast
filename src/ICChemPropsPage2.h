#pragma once

#include "PropsPropertyPage.h"
#include "ChemIC.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "afxwin.h"

#include "GridTimeSeries.h"

class CUnits;

// CICChemPropsPage2 dialog

class CICChemPropsPage2 : public CPropsPropertyPage
{
	DECLARE_DYNAMIC(CICChemPropsPage2)

public:
	CICChemPropsPage2();
	virtual ~CICChemPropsPage2();

	void SetProperties(const CChemIC& rChemIC);
	void GetProperties(CChemIC& rChemIC)const;

	void SetFlowOnly(bool flowOnly)      { this->FlowOnly = flowOnly; }
	bool GetFlowOnly(void)const          { return this->FlowOnly; }

	void SetDefault(bool bDefault)       { this->Default = bDefault; }
	bool GetDefault(void)const           { return this->Default; }

	void SetDesc(LPCTSTR desc)           { this->Description = desc; }
	LPCTSTR GetDesc()                    { return this->Description; }

	void SetUnits(const CUnits& u);

// Dialog Data
	enum { IDD = IDD_PROPS_ICCHEM2 };

	// type enum
	enum ModeType
	{
		NONE     = 0, // also represents chosen property
		CONSTANT = 1,
		LINEAR   = 2,
		POINTS   = 3,
		XYZ      = 4,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void DDV_SoftValidate();
	virtual void DDX_Single(CDataExchange* pDX);
	virtual void SetPropertyDescription();

	DECLARE_MESSAGE_MAP()

	//{{ COMMON
	// browse for XYZ and XYZT
	afx_msg void OnBnClickedButtonXYZ();

	// DDV failures
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);

	// should be on superclass
	afx_msg void OnEnSetfocusDescEdit();

	// single property combo
	afx_msg void OnCbnSelchangeComboProptype();

	// mixture checkbox
	afx_msg void OnBnClickedCheckMixture();
	//}} COMMON

	// face checkbox
	afx_msg void OnBnClickedCheckFace();

	// property controllers
	CGridTimeSeries SolutionProperty;          // single
	CGridTimeSeries EquilibriumPhasesProperty; // single
	CGridTimeSeries SurfaceProperty;           // single
	CGridTimeSeries ExchangeProperty;          // single
	CGridTimeSeries GasPhaseProperty;          // single
	CGridTimeSeries SolidSolutionsProperty;    // single
	CGridTimeSeries KineticsProperty;          // single

	// RTF strings
	std::string m_sDescriptionRTF;        // IDR_DESCRIPTION_RTF
	std::string m_sSolutionRTF;           // IDR_CHEM_IC_SOLUTION_RTF
	std::string m_sEquilibriumPhasesRTF;  // IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF
	std::string m_sSurfaceRTF;            // IDR_CHEM_IC_SURFACE_RTF
	std::string m_sExchangeRTF;           // IDR_CHEM_IC_EXCHANGE_RTF
	std::string m_sGasPhaseRTF;           // IDR_CHEM_IC_GAS_PHASE_RTF
	std::string m_sSolidSolutionsRTF;     // IDR_CHEM_IC_SOLID_SOLUTIONS_RTF
	std::string m_sKineticsRTF;           // IDR_CHEM_IC_KINETICS_RTF

	// should be member of superclass
	CString Description;

	// flags
	bool FlowOnly;
	bool Default;

protected:
	// data
	CChemIC ChemIC;
};
