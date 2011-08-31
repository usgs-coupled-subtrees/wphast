#pragma once

#include "PropsPropertyPage.h"
#include "BC.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "afxwin.h"

#include "GridTimeSeries.h"

class CUnits;

// CSpecifiedHeadPropsPage dialog

class CSpecifiedHeadPropsPage : public CPropsPropertyPage
{
	DECLARE_DYNAMIC(CSpecifiedHeadPropsPage)

public:
	CSpecifiedHeadPropsPage();
	virtual ~CSpecifiedHeadPropsPage();

	void SetProperties(const CBC& rBC);
	void GetProperties(CBC& rBC)const;

	void SetFlowOnly(bool flowOnly)      { this->FlowOnly = flowOnly; }
	bool GetFlowOnly(void)const          { return this->FlowOnly; }

	void SetDefault(bool bDefault)       { this->Default = bDefault; }
	bool GetDefault(void)const           { return this->Default; }

	void SetDesc(LPCTSTR desc)           { this->Description = desc; }
	LPCTSTR GetDesc()                    { return this->Description; }

	void SetUnits(const CUnits &u);

// Dialog Data
	enum { IDD = IDD_PROPS_SPECIFIED_HEAD2 };

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
	virtual void DDX_Series(CDataExchange* pDX);
	virtual void SetPropertyDescription();

	DECLARE_MESSAGE_MAP()

	//{{ COMMON
	// end edit time series grids
	afx_msg void OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result);

	// selection changed time series grids
	afx_msg void OnSelChangedFlux(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result);

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

	// exterior only checkbox
	afx_msg void OnBnClickedExteriorOnly();

	// solution type radios
	afx_msg void OnBnClickedSolutionRadios();

	// property controllers
	CGridTimeSeries HeadSeries;
	CGridTimeSeries SolutionSeries;

	// RTF strings
	std::string m_sDescriptionRTF;   // IDR_DESCRIPTION_RTF
	std::string m_sHeadRTF;          // IDR_BC_SPECIFIED_HEAD_RTF
	std::string m_sSolutionAssocRTF; // IDR_BC_SPECIFIED_SOL_ASSOC_RTF
	std::string m_sSolutionFixRTF;   // IDR_BC_SPECIFIED_SOL_FIX_RTF
	std::string m_sSolTypeRTF;       // IDR_BC_SOL_TYPE_RTF
	std::string m_sExtCellsRTF;      // IDR_BC_SPECIFIED_EXT_CELLS_RTF

	// should be member of superclass
	CString Description;

	// flags
	bool FlowOnly;
	bool Default;

protected:
	// data
	CBC BC;
public:
	afx_msg void OnBnSetfocusAssocRadio();
	afx_msg void OnBnSetfocusFixedRadio();
	afx_msg void OnBnClickedFixedRadio();
	afx_msg void OnBnSetfocusCheckExteriorOnly();
	afx_msg void OnBnSetfocusRadioEcAll();
	afx_msg void OnBnSetfocusRadioEcX();
	afx_msg void OnBnSetfocusRadioEcY();
	afx_msg void OnBnSetfocusRadioEcZ();
};
