#pragma once


// CPropsPropertyPage dialog

class CPropsPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropsPropertyPage)

public:
// COMMENT: {7/10/2009 7:54:12 PM}	CPropsPropertyPage();
	explicit CPropsPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));

	virtual ~CPropsPropertyPage();

// Dialog Data
// COMMENT: {7/10/2009 7:54:18 PM}	enum { IDD = IDD_PROPS_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void DDV_SoftValidate();

	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();

	DECLARE_MESSAGE_MAP()
};
