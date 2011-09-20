#pragma once

#include <string>
#include "HeadIC.h"
#include "Units.h"
#include "gridctrl/ModGridCtrlEx.h"

// CICHeadPropertyPage2 dialog

class CICHeadPropertyPage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CICHeadPropertyPage2)

public:
	CICHeadPropertyPage2();
	CICHeadPropertyPage2(UINT nIDCaption);
	CICHeadPropertyPage2(UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

	virtual ~CICHeadPropertyPage2();

	void SetProperties(const CHeadIC& r_headIC);
	void GetProperties(CHeadIC& r_headIC)const;

// Dialog Data
	enum { IDD = IDD_IC_HEAD_INT_PROPPAGE2 };

	// data
	CModGridCtrlEx Grid;
	CRichEditCtrl  RichEditCtrl;

protected:

	void CommonConstruct(void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnSelChanged(NMHDR *pNotifyStruct, LRESULT *result);

	CHeadIC      HeadIC;
	CUnits       Units;
	std::string  MaskRTF;    // IDR_IC_HEAD_MASK_RTF  -- not supported
	std::string  HeadRTF;    // IDR_IC_HEAD_HEAD_RTF
};
