#pragma once

#include <string>
#include "GridElt.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"

// CMediaSpreadPropertyPage dialog

class CMediaSpreadPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMediaSpreadPropertyPage)

public:
	CMediaSpreadPropertyPage();
	virtual ~CMediaSpreadPropertyPage();

	void SetProperties(const CGridElt& r_gridElt);
	void GetProperties(CGridElt& r_gridElt);

	void SetFlowOnly(bool bFlowOnly)     { m_bFlowOnly = bFlowOnly; }
	bool GetFlowOnly(void)const          { return m_bFlowOnly; }

// Dialog Data
	enum { IDD = IDD_MEDIASPREAD_EXT_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL SetupGrids(void);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnItemChangedMedia(NMHDR *pNotifyStruct, LRESULT *result);

	CModGridCtrlEx   m_gridMedia;     // IDC_MEDIA_GRID

	CGridElt m_gridElt;

	bool m_bFlowOnly;

public:
	virtual BOOL OnInitDialog();
};
