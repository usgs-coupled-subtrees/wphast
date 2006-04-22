#pragma once

#include <string>
#include "GridElt.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"
#include "afxcmn.h"

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

	void SetDefault(bool bDefault)       { m_bDefault = bDefault; }
	bool GetDefault(void)const           { return m_bDefault; }

	void SetDesc(LPCTSTR desc) { m_desc = desc; }
	LPCTSTR GetDesc() { return m_desc.c_str(); }

// Dialog Data
	enum { IDD = IDD_MEDIASPREAD_EXT_PROPPAGE };

protected:
	void CommonConstruct(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL SetupGrids(void);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnCheckChangedMedia(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedMedia(NMHDR *pNotifyStruct, LRESULT *result);

	CModGridCtrlEx   m_gridMedia;     // IDC_MEDIA_GRID

	CGridElt m_gridElt;

	bool m_bFlowOnly;
	bool m_bDefault;

	std::string m_sDescriptionRTF;       // IDR_DESCRIPTION_RTF
	std::string m_sActiveRTF;            // IDR_MEDIA_ACTIVE_RTF
	std::string m_sKxRTF;                // IDR_MEDIA_KX_RTF
	std::string m_sKyRTF;                // IDR_MEDIA_KY_RTF
	std::string m_sKzRTF;                // IDR_MEDIA_KZ_RTF
	std::string m_sAlphaLongRTF;         // IDR_MEDIA_LONG_DISP_RTF
	std::string m_sPorosityRTF;          // IDR_MEDIA_POROSITY_RTF
	std::string m_sStorageRTF;           // IDR_MEDIA_SPEC_STORAGE_RTF
	std::string m_sAlphaHorizontalRTF;   // IDR_MEDIA_ALPHA_HORZ_RTF
	std::string m_sAlphaVerticalRTF;     // IDR_MEDIA_ALPHA_VERT_RTF

	std::string m_desc;

public:
	virtual BOOL OnInitDialog();
	CRichEditCtrl m_wndRichEditCtrl;
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardNext();
	afx_msg void OnEnSetfocusDescEdit();
};
