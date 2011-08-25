#pragma once
#include "afxcmn.h"

#include "GridKeyword.h"
#include "Grid.h"
#include "Units.h"
#include "Snap.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayoutPropertyPageXP.h"

class CGridSetAction;
class CWPhastDoc;
class CGridActor;
class CGridSetAction;


#ifndef baseCGridPropertyPage2
///#define baseCGridPropertyPage2 ETSLayoutPropertyPageXP
#define baseCGridPropertyPage2 CPropertyPage
#endif

// CGridPropertyPage2 dialog

class CGridPropertyPage2 : public baseCGridPropertyPage2
{
	DECLARE_DYNAMIC(CGridPropertyPage2)

public:
	CGridPropertyPage2();
	virtual ~CGridPropertyPage2();

	void SetUnits(const CUnits& units);

	void SetProperties(const CGridKeyword& gridKeyword);
	void GetProperties(CGridKeyword& gridKeyword)const;

	void SetCurSel(int index);
	int GetCurSel(void)const;

// Dialog Data
	enum { IDD = IDD_GRID_PROPPAGE4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioUniform();
	afx_msg void OnBnClickedRadioNonuniform();
	afx_msg void OnTcnSelchangingTabCoor(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelChangedNonuniform(NMHDR *pNotifyStruct, LRESULT *result);

	void EnableUniform(BOOL bEnable = TRUE);
	void EnableNonuniform(BOOL bEnable = TRUE);

	CTabCtrl         m_tabCtrl;
	CSpinButtonCtrl  m_spinUnified;
	CSpinButtonCtrl  m_spinNonunified;

	CModGridCtrlEx   m_wndNonuniformGrid;      // IDC_GRID_NONUNIFORM
	CString          m_strHorizontalUnits;
	CString          m_strVerticalUnits;
	int              m_nIndex;
	bool             m_bNeedAction;
	CGridKeyword     m_gridKeyword;

	CGridSetAction  *m_pAction;

	CRichEditCtrl m_wndRichEditCtrl;

	std::string m_sGridUni;       // IDR_GRID_UNI_RTF
	std::string m_sGridNonUni;    // IDR_GRID_NONUNI_RTF
	std::string m_sGridMin;       // IDR_GRID_MIN_RTF
	std::string m_sGridMax;       // IDR_GRID_MAX_RTF
	std::string m_sGridNodes;     // IDR_GRID_NODES_RTF
	std::string m_sGridSnap;      // IDR_GRID_SNAP_RTF
	std::string m_sGridOrient;    // IDR_GRID_ORIENT_RTF
	std::string m_sGridChemDims;  // IDR_GRID_CHEM_DIMS_RTF
	std::string m_sGridOrigin;    // IDR_GRID_ORIGIN_RTF
	std::string m_sGridAngle;     // IDR_GRID_ANGLE_RTF

public:
	CWPhastDoc      *m_pDoc;
	CGridActor      *m_pActor;

	afx_msg void OnTcnSelchangeTabCoor(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnApply();
	virtual void OnCancel();

protected:
	virtual void PostNcDestroy();

public:
	afx_msg void OnEnChange();
	afx_msg void OnEnChangeEditNNodes();
	afx_msg void OnBnClickedButtonIns();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonSeries();
	afx_msg void OnEndLabelEditGrid(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnBnClickedButtonSubdivide();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnSetfocusRadioUniform();
	afx_msg void OnBnSetfocusRadioNonuniform();
	afx_msg void OnEnSetfocusEditMin();
	afx_msg void OnEnSetfocusEditMax();
	afx_msg void OnEnSetfocusEditUNodes();
	afx_msg void OnEnSetfocusEditSnap();
	afx_msg void OnEnSetfocusEditNNodes();
	afx_msg void OnBnSetfocusCheckX();
	afx_msg void OnBnSetfocusCheckY();
	afx_msg void OnBnSetfocusCheckZ();
	afx_msg void OnBnSetfocusRadioXy();
	afx_msg void OnBnSetfocusRadioXz();
	afx_msg void OnEnSetfocusEditOrigX();
	afx_msg void OnEnSetfocusEditOrigY();
	afx_msg void OnEnSetfocusEditOrigZ();
	afx_msg void OnEnSetfocusEditGridAngle();
};
