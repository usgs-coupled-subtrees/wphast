#pragma once
#include "afxcmn.h"

//#include "GridLODActor.h"
#include "Grid.h"
#include "Units.h"

class CWPhastDoc;

// CGridPropertyPage dialog

class CGridPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGridPropertyPage)

public:
	CGridPropertyPage();
	CGridPropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);
	void CommonConstruct(void);

	virtual ~CGridPropertyPage();

	void SetApply(CWPhastDoc* pDoc);

// Dialog Data
	enum { IDD = IDD_GRID_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CWPhastDoc* m_pDoc;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	CSpinButtonCtrl m_spinX;
	CSpinButtonCtrl m_spinY;
	CSpinButtonCtrl m_spinZ;
	virtual BOOL OnInitDialog();
	CGrid m_grid[3];
	virtual BOOL OnApply();
	afx_msg void OnEnChange();
	////BOOL m_bFlowOnly;
	virtual BOOL OnSetActive();
	CString m_strHorizontalUnits;
	CString m_strVerticalUnits;
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardNext();
};
