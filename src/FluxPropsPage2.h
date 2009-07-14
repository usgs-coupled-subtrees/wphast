#pragma once

#include "PropsPropertyPage.h"
#include "BC.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "afxwin.h"

#include "GridTimeSeries.h"

class CUnits;

// CFluxPropsPage2 dialog

class CFluxPropsPage2 : public CPropsPropertyPage
{
	DECLARE_DYNAMIC(CFluxPropsPage2)

public:
	CFluxPropsPage2();
	virtual ~CFluxPropsPage2();

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
	enum { IDD = IDD_PROPS_FLUX_3 };

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

	// face checkbox
	afx_msg void OnBnClickedCheckFace();
	afx_msg void OnBnSetfocusCheckFace();

	// property controllers
	CGridTimeSeries FluxSeries;
	CGridTimeSeries SolutionSeries;

	// RTF strings
	std::string m_sDescriptionRTF;   // IDR_DESCRIPTION_RTF
	std::string m_sAssocSolutionRTF; // IDR_BC_FLUX_ASSOC_SOL_RTF
	std::string m_sFluxRTF;          // IDR_BC_FLUX_FLUX_RTF
	std::string m_sFaceRTF;          // IDR_BC_FLUX_FACE_RTF

	// should be member of superclass
	CString Description;

	// flags
	bool FlowOnly;
	bool Default;

protected:
	// data
	CBC BC;
};
