#pragma once

#include "GridElt.h"
#include "CheckTreeCtrl.h"
#include <string>

// CMediaPropertyPage dialog

class CMediaPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMediaPropertyPage)

public:
	CMediaPropertyPage();
	CMediaPropertyPage(UINT nIDCaption);
	CMediaPropertyPage(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);
	CMediaPropertyPage(LPCTSTR lpszDummy, UINT nIDTemplate);

	virtual ~CMediaPropertyPage();

	void SetProperties(const CGridElt& r_grid_elt);
	void GetProperties(CGridElt& r_grid_elt)const;

	void SetNewModelWizard(bool bNewModelWizard)     { m_bNewModelWizard = bNewModelWizard; }
	bool GetNewModelWizard(void)const                { return m_bNewModelWizard; }

	void SetFlowOnlyWizard(bool bFlowOnlyWizard)     { m_bFlowOnlyWizard = bFlowOnlyWizard; }
	bool GetFlowOnlyWizard(void)const                { return m_bFlowOnlyWizard; }

	void SetShowFlowOnlyMessage(bool bShowFlowOnlyMessage)     { m_bShowFlowOnlyMessage = bShowFlowOnlyMessage; }
	bool GetShowFlowOnlyMessage(void)const                     { return m_bShowFlowOnlyMessage; }

	void SetFlowOnly(bool bFlowOnly)     { m_bFlowOnly = bFlowOnly; }
	bool GetFlowOnly(void)const          { return m_bFlowOnly; }

// Dialog Data
	enum { IDD = IDD_MEDIA_EXT_PROPPAGE };
	enum { IDD_INTERIOR = IDD_MEDIA_INT_PROPPAGE };

protected:
	void CommonConstruct(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTvnSelchangedPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingPropTree(NMHDR *pNMHDR, LRESULT *pResult);
	//{{USER
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUM_CheckStateChange(WPARAM wParam, LPARAM lParam);
	//}}USER
	afx_msg void OnBnClickedSingleValueRadio();
	afx_msg void OnBnClickedLinearInterpolationRadio();

protected:
	CCheckTreeCtrl m_wndTreeCtrl;
	CRichEditCtrl m_wndRichEditCtrl;
	HTREEITEM m_htiDDX;
	HTREEITEM m_htiAlpha_long;
	HTREEITEM m_htiAlpha_horizontal;
	HTREEITEM m_htiAlpha_vertical;

	CGridElt m_grid_elt;
	CGridElt m_grid_eltFixed;
	CGridElt m_grid_eltLinear;

	bool m_bFlowOnly;
	bool m_bShowFlowOnlyMessage;
	bool m_bFirstPaint;
	bool m_bNewModelWizard;
	bool m_bFlowOnlyWizard;


	std::string m_sActiveRTF;            // IDR_MEDIA_ACTIVE_RTF
	std::string m_sKxRTF;                // IDR_MEDIA_KX_RTF
	std::string m_sKyRTF;                // IDR_MEDIA_KY_RTF
	std::string m_sKzRTF;                // IDR_MEDIA_KZ_RTF
	std::string m_sLongDispRTF;          // IDR_MEDIA_LONG_DISP_RTF
	std::string m_sPorosityRTF;          // IDR_MEDIA_POROSITY_RTF
	std::string m_sSpecStorageRTF;       // IDR_MEDIA_SPEC_STORAGE_RTF
	std::string m_sTransDispRTF;         // IDR_MEDIA_TRANS_DISP_RTF

	std::string m_sAlphaHorizontalRTF;   // IDR_MEDIA_ALPHA_HORZ_RTF
	std::string m_sAlphaVericalRTF;      // IDR_MEDIA_ALPHA_VERT_RTF

public:
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
