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
	virtual void SetPropertyDescription();

	virtual void DDX_Series(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	//{{ COMMON
	afx_msg void OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result);

	afx_msg void OnSelChangedFlux(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result);

	afx_msg void OnBnClickedButtonXYZ();

	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);

	afx_msg void OnEnSetfocusDescEdit();

	afx_msg void OnCbnSelchangeComboProptype();
	afx_msg void OnBnClickedCheckMixture();
	//}} COMMON

	// exterior only checkbox
	afx_msg void OnBnClickedExteriorOnly();

// COMMENT: {7/14/2009 2:32:47 PM}	// exterior only radios
// COMMENT: {7/14/2009 2:32:47 PM}	void OnBnClickedExteriorRadios();
// COMMENT: {7/14/2009 2:32:47 PM}
// COMMENT: {7/14/2009 2:32:47 PM}	// solution type radios
// COMMENT: {7/14/2009 2:32:47 PM}	afx_msg void OnBnClickedSolutionRadios();

	CGridTimeSeries HeadSeries;
	CGridTimeSeries SolutionSeries;

// COMMENT: {7/13/2009 7:19:03 PM}	CModGridCtrlEx PointsGrid;
// COMMENT: {7/13/2009 7:19:03 PM}
// COMMENT: {7/13/2009 7:19:03 PM}	CRichEditCtrl  RichEditCtrl;
// COMMENT: {7/13/2009 7:19:03 PM}	CTreeCtrl      TreeCtrl;
// COMMENT: {7/13/2009 7:19:03 PM}
// COMMENT: {7/13/2009 7:19:03 PM}	HTREEITEM ItemDDX;

	std::string m_sDescriptionRTF;   // IDR_DESCRIPTION_RTF
	std::string m_sHeadRTF;          // IDR_BC_SPECIFIED_HEAD_RTF
	std::string m_sSolutionAssocRTF; // IDR_BC_SPECIFIED_SOL_ASSOC_RTF
	std::string m_sSolutionFixRTF;   // IDR_BC_SPECIFIED_SOL_FIX_RTF
	std::string m_sSolTypeRTF;       // IDR_BC_SOL_TYPE_RTF

	CString Description;

	bool FlowOnly;
	bool Default;

protected:
	CBC BC;
public:
};
