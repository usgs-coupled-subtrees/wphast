#pragma once
#include "ETSLayout/ETSLayout.h"

class CAction;
class CMacroAction;

class ETSLayoutModelessPropertySheet : public ETSLayoutPropertySheet
{
	DECLARE_DYNAMIC(ETSLayoutModelessPropertySheet)

public:
	ETSLayoutModelessPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	ETSLayoutModelessPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~ETSLayoutModelessPropertySheet();

	void AddAction(CAction* pAction);

protected:
	DECLARE_MESSAGE_MAP()
	CMacroAction* m_pAction;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
protected:
	virtual void PostNcDestroy();
};
