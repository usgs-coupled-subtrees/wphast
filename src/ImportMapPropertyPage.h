#pragma once

#include "SiteMap2.h"

class CMapDialog;
class CGridPropertyPage;

// CImportMapPropertyPage dialog

class CImportMapPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CImportMapPropertyPage)

public:
	CImportMapPropertyPage();
	CImportMapPropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);
	void CommonConstruct(void);

	CSiteMap2 GetSiteMap2(void)const { return m_siteMap2; }
	void SetSiteMap2(const CSiteMap2& siteMap2) { m_siteMap2 = siteMap2; }

	bool m_bImport;
	bool m_bWorld;

	virtual ~CImportMapPropertyPage();

// Dialog Data
	enum { IDD = IDD_IMPORT_MAP_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void EnableImport(BOOL bEnable=TRUE);

	CArray<CPropertyPage*, CPropertyPage*> m_PropPageArray;
	CMapDialog *m_pDlg;
	CGridPropertyPage *m_pGridPage;
	int m_idxGridPage;

	CSiteMap2 m_siteMap2;
	CString m_sExt;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedImportYes();
	afx_msg void OnBnClickedImportNo();
	afx_msg void OnBnClickedUseWorldCheck();
	afx_msg void OnBnClickedBrowseMapButton();
	afx_msg void OnBnClickedBrowseWorldButton();
	virtual BOOL OnInitDialog();
	virtual LRESULT OnWizardNext();
	CString m_strMapFileName;
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnSetActive();
	CString m_strWorldFileName;
};
