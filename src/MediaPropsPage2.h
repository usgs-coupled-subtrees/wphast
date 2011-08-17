#pragma once

#include "PropsPropertyPage.h"
#include "GridElt.h"
#include "afxwin.h"

#include "GridTimeSeries.h"

class CUnits;

// CMediaPropsPage2 dialog

class CMediaPropsPage2 : public CPropsPropertyPage
{
	DECLARE_DYNAMIC(CMediaPropsPage2)

public:
	CMediaPropsPage2();
	virtual ~CMediaPropsPage2();

	void SetProperties(const CGridElt& rGridElt);
	void GetProperties(CGridElt& rGridElt)const;

	void SetFlowOnly(bool flowOnly)      { this->FlowOnly = flowOnly; }
	bool GetFlowOnly(void)const          { return this->FlowOnly; }

	void SetDefault(bool bDefault)       { this->Default = bDefault; }
	bool GetDefault(void)const           { return this->Default; }

	void SetDesc(LPCTSTR desc)           { this->Description = desc; }
	LPCTSTR GetDesc()                    { return this->Description; }

	void SetUnits(const CUnits &u);

// Dialog Data
	enum { IDD = IDD_PROPS_MEDIA2 };

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
	afx_msg void OnBnClickedShell();

	// property controllers
	CGridTimeSeries ActiveProperty;          // single
	CGridTimeSeries KxProperty;              // single
	CGridTimeSeries KyProperty;              // single
	CGridTimeSeries KzProperty;              // single
	CGridTimeSeries PorosityProperty;        // single
	CGridTimeSeries StorageProperty;         // single
	CGridTimeSeries AlphaLongProperty;       // single
	CGridTimeSeries AlphaHorizontalProperty; // single
	CGridTimeSeries AlphaVerticalProperty;   // single
	CGridTimeSeries TortuosityProperty;      // single

	// RTF strings
	std::string m_sDescriptionRTF;       // IDR_DESCRIPTION_RTF
	std::string m_sActiveRTF;            // IDR_MEDIA_ACTIVE_RTF
	std::string m_sKxRTF;                // IDR_MEDIA_KX_RTF
	std::string m_sKyRTF;                // IDR_MEDIA_KY_RTF
	std::string m_sKzRTF;                // IDR_MEDIA_KZ_RTF
	std::string m_sAlphaLongRTF;         // IDR_MEDIA_LONG_DISP_RTF
	std::string m_sPorosityRTF;          // IDR_MEDIA_POROSITY_RTF
	std::string m_sStorageRTF;           // IDR_MEDIA_SPEC_STORAGE_RTF
	std::string m_sAlphaHorizontalRTF;   // IDR_MEDIA_ALPHA_HORZ_RTF
	std::string m_sAlphaVerticalRTF;     // IDR_MEDIA_ALPHA_VERT_RTF
	std::string m_sTortuosityRTF;        // IDR_MEDIA_TORTUOSITY_RTF

	// should be member of superclass
	CString Description;

	// flags
	bool FlowOnly;
	bool Default;

protected:
	// data
	CGridElt GridElt;
};
