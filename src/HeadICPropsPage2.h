#pragma once

#include "PropsPropertyPage.h"
#include "HeadIC.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "afxwin.h"

#include "GridTimeSeries.h"

class CUnits;

// CHeadICPropsPage2 dialog

class CHeadICPropsPage2 : public CPropsPropertyPage
{
	DECLARE_DYNAMIC(CHeadICPropsPage2)

public:
	CHeadICPropsPage2();
	virtual ~CHeadICPropsPage2();

	void SetProperties(const CHeadIC& rHeadIC);
	void GetProperties(CHeadIC& rHeadIC)const;

	void SetDefault(bool bDefault)       { this->Default = bDefault; }
	bool GetDefault(void)const           { return this->Default; }

	void SetDesc(LPCTSTR desc)           { this->Description = desc; }
	LPCTSTR GetDesc()                    { return this->Description; }

	void SetUnits(const CUnits& u);

// Dialog Data
	enum { IDD = IDD_PROPS_HEADIC2 };

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
	afx_msg void OnCbnSelchangeComboProptype();\

	// mixture checkbox
	afx_msg void OnBnClickedCheckMixture();
	//}} COMMON

	// face checkbox
	afx_msg void OnBnClickedCheckFace();

	// property controllers
	CGridTimeSeries HeadProperty;          // single

	// RTF strings
	std::string m_sDescriptionRTF;  // IDR_DESCRIPTION_RTF
	std::string m_sHeadRTF;         // IDR_IC_HEAD_HEAD_RTF

	// should be member of superclass
	CString Description;

	// flags
	bool Default;

protected:
	// data
	CHeadIC HeadIC;
};
