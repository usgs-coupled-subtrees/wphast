#pragma once



// CModelessPropertySheet

class CAction;
class CMacroAction;

class CModelessPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CModelessPropertySheet)

public:
	CModelessPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CModelessPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CModelessPropertySheet();

	void AddAction(CAction* pAction);

protected:
	DECLARE_MESSAGE_MAP()
	CMacroAction* m_pAction;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
};


