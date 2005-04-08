#pragma once
#include "ETSLayout/ETSLayout.h"

class ETSLayoutPropertyPageXP :	public ETSLayoutPropertyPage
{
	DECLARE_DYNCREATE(ETSLayoutPropertyPageXP)

public:
	ETSLayoutPropertyPageXP(void);
	ETSLayoutPropertyPageXP(UINT nIDTemplate, UINT nIDCaption = 0);
	//ETSLayoutPropertyPageXP( LPCTSTR lpszTemplateName, UINT nIDCaption = 0 );

	~ETSLayoutPropertyPageXP();

protected:
	// Generated message map functions
	//{{AFX_MSG(ETSLayoutPropertyPage)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
