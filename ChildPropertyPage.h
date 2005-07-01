#pragma once


// CChildPropertyPage dialog

class CChildPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CChildPropertyPage)

public:
// COMMENT: {6/29/2005 6:47:49 PM}	CChildPropertyPage();
	explicit CChildPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE), UINT nID = 0);
	explicit CChildPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE), UINT nID = 0);

	virtual ~CChildPropertyPage();

public:
	void AddPage(CPropertyPage* pPage);
	CPropertyPage* GetActivePage() const;
	int GetPageCount() const;

// Dialog Data
	enum { IDD = IDD_CHILDPROPERTYPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);

	CPropertySheet m_sheet;
	///CPtrArray m_pages;      // array of CPropertyPage pointers
	CArray<CPropertyPage*, CPropertyPage*> m_arrPages;

	UINT m_nID;
};
