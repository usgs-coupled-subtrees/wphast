#pragma once

class CChemICPropertyPage;

// CNewModelIntro dialog
#include "FlowOnly.h"
#include "SteadyFlow.h"
#include "FreeSurface.h"

class CNewModelIntro : public CPropertyPage
{
	DECLARE_DYNAMIC(CNewModelIntro)

public:
	CNewModelIntro();
	virtual ~CNewModelIntro();

// COMMENT: {5/27/2004 3:30:01 PM}	void SetProperties(const CFlowOnly& r_flowOnly);
// COMMENT: {5/27/2004 3:30:01 PM}	void GetProperties(CFlowOnly& r_flowOnly)const;

	void SetFlowOnly(const CFlowOnly& flowOnly);
	CFlowOnly GetFlowOnly(void)const;

	void SetSteadyFlow(const CSteadyFlow& steadyFlow);
	CSteadyFlow GetSteadyFlow(void)const;

	void SetFreeSurface(const CFreeSurface& freeSurface);
	CFreeSurface GetFreeSurface(void)const;

// Dialog Data
	enum { IDD = IDD_NEW_MODEL_INTRO2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CFlowOnly    m_flowOnly;
	CSteadyFlow  m_steadyFlow;
	CFreeSurface m_freeSurface;

	CArray<CPropertyPage*, CPropertyPage*> m_PropPageArray;
	CChemICPropertyPage *m_pChemICPage;
	int m_idxChemICPage;

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedSoluteTransportRadio();
	afx_msg void OnBnClickedFlowOnlyRadio();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
};
