#pragma once

#include "TreePropSheetEx/TreePropSheetEx.h"
#include "PropNone.h"
#include "PropConstant.h"


class CTreePropSheetExSRC : public TreePropSheet::CTreePropSheetEx
{
	DECLARE_DYNAMIC(CTreePropSheetExSRC)

public:
	CTreePropSheetExSRC(void);
	CTreePropSheetExSRC(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CTreePropSheetExSRC(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CTreePropSheetExSRC(void);

// Message handlers
protected:
	//{{AFX_MSG(CTreePropSheetExSRC)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

 	afx_msg void OnPageTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *plResult);
 	afx_msg void OnPageTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *plResult);

	afx_msg LRESULT OnSetCurSel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetCurSelId(WPARAM wParam, LPARAM lParam);

	CPropNone      PropNone;
	CPropConstant  PropConstant;
};
