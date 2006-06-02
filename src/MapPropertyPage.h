#pragma once

#define __IS_RESIZABLE__

#if defined(__IS_RESIZABLE__)
#  include "ETSLayout/ETSLayout.h"
#  define CMapPropertyPageBase ETSLayoutPropertyPage
#else
#  define CMapPropertyPageBase CPropertyPage
#endif

// CMapPropertyPage dialog

class CMapPropertyPage : public CMapPropertyPageBase
{
	DECLARE_DYNAMIC(CMapPropertyPage)
	DECLARE_LAYOUT();

public:
	CMapPropertyPage();
	virtual ~CMapPropertyPage();

// Dialog Data
	enum { IDD = IDD_MAP_PROPPAGE };

	virtual CRect GetRect();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
