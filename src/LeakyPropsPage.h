#pragma once

#include "PropsPropertyPage.h"
#include "BC.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "afxwin.h"

#include "GridTimeSeries.h"

class CUnits;

// CLeakyPropsPage dialog

class CLeakyPropsPage : public CPropsPropertyPage
{
	DECLARE_DYNAMIC(CLeakyPropsPage)

public:
	CLeakyPropsPage();
	virtual ~CLeakyPropsPage();

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
	enum { IDD = IDD_PROPS_LEAKY };

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

	DECLARE_MESSAGE_MAP()

	//{{ COMMON
 	afx_msg void OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *plResult);
 	afx_msg void OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *plResult);

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

	afx_msg void OnBnClickedCheckFace();

	CGridTimeSeries ThicknessSeries; // single
	CGridTimeSeries HydCondSeries;   // single
	CGridTimeSeries HeadSeries;
	CGridTimeSeries SolutionSeries;

	CModGridCtrlEx PointsGrid;

	CRichEditCtrl  RichEditCtrl;
	CTreeCtrl      TreeCtrl;

	HTREEITEM ItemDDX;

	std::string m_sDescriptionRTF;   // IDR_DESCRIPTION_RTF
	std::string m_sHeadRTF;          // IDR_DESCRIPTION_RTF
	std::string m_sThicknessRTF;     // IDR_BC_LEAKY_THICKNESS_RTF
	std::string m_sHydCondRTF;       // IDR_BC_LEAKY_HYD_COND_RTF
	std::string m_sAssocSolutionRTF; // IDR_BC_LEAKY_ASSOC_SOL_RTF
	std::string m_sFaceRTF;          // IDR_BC_LEAKY_FACE_RTF

	CString Description;
// COMMENT: {6/30/2009 11:16:25 PM}	CString DefaultUnits;

	bool FlowOnly;
	bool Default;

protected:
	CBC BC;
};
