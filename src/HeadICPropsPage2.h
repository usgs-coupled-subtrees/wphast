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

// COMMENT: {7/10/2009 1:46:39 PM}	void SetFlowOnly(bool flowOnly)      { this->FlowOnly = flowOnly; }
// COMMENT: {7/10/2009 1:46:39 PM}	bool GetFlowOnly(void)const          { return this->FlowOnly; }

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
	virtual void SetPropertyDescription();

	virtual void DDX_Single(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	//{{ COMMON
 	afx_msg void OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *plResult);
 	afx_msg void OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *plResult);

	afx_msg void OnBnClickedButtonXYZ();

	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);

	afx_msg void OnEnSetfocusDescEdit();

	afx_msg void OnCbnSelchangeComboProptype();
	afx_msg void OnBnClickedCheckMixture();
	//}} COMMON

	afx_msg void OnBnClickedCheckFace();

	CGridTimeSeries HeadProperty;          // single

// COMMENT: {7/13/2009 7:18:32 PM}	CModGridCtrlEx PointsGrid;
// COMMENT: {7/13/2009 7:18:32 PM}
// COMMENT: {7/13/2009 7:18:32 PM}	CRichEditCtrl  RichEditCtrl;
// COMMENT: {7/13/2009 7:18:32 PM}	CTreeCtrl      TreeCtrl;
// COMMENT: {7/13/2009 7:18:32 PM}
// COMMENT: {7/13/2009 7:18:32 PM}	HTREEITEM ItemDDX;

	std::string m_sDescriptionRTF;  // IDR_DESCRIPTION_RTF
	std::string m_sHeadRTF;         // IDR_IC_HEAD_HEAD_RTF

	CString Description;

	bool Default;

protected:
	CHeadIC HeadIC;
};
