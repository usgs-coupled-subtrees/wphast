#pragma once

#include <string>
#include "GridElt.h"
#include "Units.h"
#include "gridctrl/ModGridCtrlEx.h"

// CMediaPropertyPage2 dialog

class CMediaPropertyPage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CMediaPropertyPage2)

public:
	CMediaPropertyPage2();	
	CMediaPropertyPage2(UINT nIDCaption);
	CMediaPropertyPage2(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);
	CMediaPropertyPage2(LPCTSTR lpszDummy, UINT nIDTemplate);

	virtual ~CMediaPropertyPage2();

	void SetProperties(const CGridElt& gridElt);
	void GetProperties(CGridElt& gridElt)const;

	void SetNewModelWizard(bool newModelWizard)                { this->NewModelWizard = newModelWizard; }
	bool GetNewModelWizard(void)const                          { return this->NewModelWizard; }

	void SetFlowOnlyWizard(bool flowOnlyWizard)                { this->FlowOnlyWizard = flowOnlyWizard; }
	bool GetFlowOnlyWizard(void)const                          { return this->FlowOnlyWizard; }

	void SetShowFlowOnlyMessage(bool showFlowOnlyMessage)      { this->ShowFlowOnlyMessage = showFlowOnlyMessage; }
	bool GetShowFlowOnlyMessage(void)const                     { return this->ShowFlowOnlyMessage; }

	void SetFlowOnly(bool flowOnly)                            { this->FlowOnly = flowOnly; }
	bool GetFlowOnly(void)const                                { return this->FlowOnly; }

// Dialog Data
	enum { IDD = IDD_MEDIA_INT_PROPPAGE2 };

protected:
	void CommonConstruct(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CRichEditCtrl  RichEditCtrl;
	CModGridCtrlEx Grid;

	CGridElt       GridElt;
	CUnits         Units;

	bool FlowOnly;
	bool ShowFlowOnlyMessage;
	bool FirstPaint;
	bool NewModelWizard;
	bool FlowOnlyWizard;

	std::string ActiveRTF;            // IDR_MEDIA_ACTIVE_RTF
	std::string KxRTF;                // IDR_MEDIA_KX_RTF
	std::string KyRTF;                // IDR_MEDIA_KY_RTF
	std::string KzRTF;                // IDR_MEDIA_KZ_RTF
	std::string LongDispRTF;          // IDR_MEDIA_LONG_DISP_RTF
	std::string PorosityRTF;          // IDR_MEDIA_POROSITY_RTF
	std::string SpecStorageRTF;       // IDR_MEDIA_SPEC_STORAGE_RTF
	std::string TransDispRTF;         // IDR_MEDIA_TRANS_DISP_RTF
	std::string AlphaHorizontalRTF;   // IDR_MEDIA_ALPHA_HORZ_RTF
	std::string AlphaVerticalRTF;     // IDR_MEDIA_ALPHA_VERT_RTF
	std::string TortuosityRTF;        // IDR_MEDIA_TORTUOSITY_RTF


public:
	virtual BOOL OnSetActive();
	afx_msg void OnPaint();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardNext();
	afx_msg void OnSelChanged(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnEndLabelEdit(NMHDR *pNotifyStruct, LRESULT *result);
};
