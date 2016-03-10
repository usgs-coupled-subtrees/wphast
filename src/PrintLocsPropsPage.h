#pragma once

#include "PropsPropertyPage.h"
// COMMENT: {2/11/2016 10:03:21 PM}#include "GridElt.h"
#include "PrintZone.h"
#include "afxwin.h"

#include "GridTimeSeries.h"

class CUnits;

// CPrintLocsPropsPage dialog

class CPrintLocsPropsPage : public CPropsPropertyPage
{
	DECLARE_DYNAMIC(CPrintLocsPropsPage)

public:
	CPrintLocsPropsPage();
	virtual ~CPrintLocsPropsPage();

	void SetProperties(const CPrintZone& rPrintZone);
	void GetProperties(CPrintZone& rPrintZone)const;

	void SetDesc(LPCTSTR desc)           { this->Description = desc; }
	LPCTSTR GetDesc()                    { return this->Description; }

	void SetUnits(const CUnits &u);

// Dialog Data
	enum { IDD = IDD_PROPS_PRINT_LOCS };

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

	// shell checkbox
	afx_msg void OnBnClickedCheckShell();

	// property controllers
	CGridTimeSeries PrintProperty;      // single

	// RTF strings
	std::string m_sDescriptionRTF;       // IDR_DESCRIPTION_RTF
	std::string m_sChemRTF;              // IDR_PR_LOC_CHEM_RTF
	std::string m_sXYZ_ChemRTF;          // IDR_PR_LOC_XYZ_CHEM_RTF
	std::string m_sPrintRTF;             // IDR_PR_LOC_PRINT_RTF

	// should be member of superclass
	CString Description;

protected:
	// data
	CPrintZone PrintZone;
};
