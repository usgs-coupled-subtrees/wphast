#pragma once


// CIntroPropertyPage dialog

class CIntroPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CIntroPropertyPage)

public:
	CIntroPropertyPage();
	virtual ~CIntroPropertyPage();

// Dialog Data
	enum { IDD = IDD_INTRO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
