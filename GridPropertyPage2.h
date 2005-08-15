#pragma once
#include "afxcmn.h"

#include "GridKeyword.h"
#include "Grid.h"
#include "Units.h"
#include "Snap.h"
#include "gridctrl/ModGridCtrlEx.h"

class CGridSetAction;
class CWPhastDoc;
class CGridActor;
class CGridSetAction;


// CGridPropertyPage2 dialog

class CGridPropertyPage2 : public CPropertyPage
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
public:
// COMMENT: {7/20/2005 3:11:28 PM}	CGrid            m_grid[3];
// COMMENT: {7/20/2005 3:29:05 PM}	int              m_axes[3];
// COMMENT: {7/20/2005 3:34:22 PM}	CSnap            m_snap;
// COMMENT: {7/20/2005 3:34:22 PM}	bool             m_print_input_xy;

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
};
