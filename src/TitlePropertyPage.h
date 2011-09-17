#pragma once


// CTitlePropertyPage dialog
#include "Title.h"
#include "afxwin.h"

class CTitlePropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTitlePropertyPage)

public:
	CTitlePropertyPage();
	virtual ~CTitlePropertyPage();

// Dialog Data
	enum { IDD = IDD_TITLE_PROPPAGE };

	void SetTitle(const CTitle& t)      { Title = t; }
	CTitle GetTitle(void)const          { return Title; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CTitle         Title;
	CRichEditCtrl  RichEditCtrl;
	std::string    TitleRTF;
public:
	afx_msg void OnEnSetfocusEditTitle();
	CEdit Edit;
};
