#pragma once



// CStartup

class CStartup : public CPropertySheet
{
	DECLARE_DYNAMIC(CStartup)

public:
	CStartup(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CStartup(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CStartup();
	virtual ~CStartup();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};


