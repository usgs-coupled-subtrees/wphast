#pragma once

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarCF

#ifndef baseCSizingDialogBarCFVS7
#define baseCSizingDialogBarCFVS7 CSizingControlBarCFVS7
#endif

#include "scbarcfvs7.h"

class CSizingDialogBarCFVS7 : public CSizingControlBarCFVS7
{
    DECLARE_DYNAMIC(CSizingDialogBarCFVS7)
public:
	CSizingDialogBarCFVS7();
	virtual ~CSizingDialogBarCFVS7();

	virtual BOOL Create(UINT nIDTemplate, LPCTSTR lpszWindowName,
		CWnd* pParentWnd, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNcPaint();
	//virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
protected:
#ifndef _AFX_NO_OCC_SUPPORT
	virtual BOOL SetOccDialogInfo(_AFX_OCC_DIALOG_INFO* pOccDialogInfo);
	// data and functions necessary for OLE control containment
	_AFX_OCC_DIALOG_INFO* m_pOccDialogInfo;
#endif
public:
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
};
