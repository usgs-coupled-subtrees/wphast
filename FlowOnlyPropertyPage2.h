#pragma once


// CFlowOnlyPropertyPage2 dialog

class CFlowOnlyPropertyPage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CFlowOnlyPropertyPage2)

public:
	CFlowOnlyPropertyPage2();
	virtual ~CFlowOnlyPropertyPage2();

// Dialog Data
	enum { IDD = IDD_FLOW_ONLY_INT_PROPPAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
};
