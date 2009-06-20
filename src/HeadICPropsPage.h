#pragma once

#include <vector>

#include "TreePropSheetEx/ResizablePage.h"
#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "TreePropSheetEx/TreePropSheetEx.h"

#include "PropDesc.h"
#include "PropNone.h"
#include "PropConstant.h"
#include "PropLinear.h"
#include "PropPoints.h"
#include "PropXYZ.h"

#include "TreePropSheetEx/TreePropSheetSplitter.h"
#include "TreePropSheetExSRC.h"

#include "PropStruct.h"
#include "HeadIC.h"
#include "afxcmn.h"


// CHeadICPropsPage dialog

class CHeadICPropsPage // : public CPropertyPage
: public CResizablePage
, public TreePropSheet::CWhiteBackgroundProvider
{
	DECLARE_DYNAMIC(CHeadICPropsPage)

public:
	CHeadICPropsPage();
	virtual ~CHeadICPropsPage();

	void CommonConstruct(void);

	void SetProperties(const CHeadIC& r_headIC);
	void GetProperties(CHeadIC& r_headIC)const;

// COMMENT: {5/27/2009 4:43:54 PM}	void SetFlowOnly(bool flowOnly)      { this->bFlowOnly = flowOnly; }
// COMMENT: {5/27/2009 4:43:54 PM}	bool GetFlowOnly(void)const          { return this->bFlowOnly; }

	void SetDefault(bool bDefault)       { this->bDefault = bDefault; }
	bool GetDefault(void)const           { return bDefault; }

	void SetDesc(LPCTSTR desc);
	LPCTSTR GetDesc()const;


// Dialog Data
	enum { IDD = IDD_PROPS_HEADIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

 	afx_msg void OnPageTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *plResult);
 	afx_msg void OnPageTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *plResult);
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);



protected:
	bool RegisterSheet(UINT nID, CPropertySheet& rSheet);
	CPropDesc                                PageDesc;
	CTreePropSheetExSRC                      SheetTop;
	TreePropSheet::CTreePropSheetSplitter    Splitter;

	CPropNone       PropNone;
	CPropConstant   PropConstant;
	CPropLinear     PropLinear;
	CPropPoints     PropPoints;
	CPropXYZ        PropXYZ;

	CHeadIC HeadIC;
	CHeadIC HeadICConstant;
	CHeadIC HeadICLinear;
	CHeadIC HeadICPoints;
	CHeadIC HeadICXYZ;
	
	CRichEditCtrl RichEditCtrl;

	HTREEITEM ItemDDX;

	bool bFlowOnly;
	bool bDefault;

	std::string m_sMaskRTF;    // IDR_IC_HEAD_MASK_RTF
	std::string m_sHeadRTF;    // IDR_IC_HEAD_HEAD_RTF

	HTREEITEM htiHEAD;

public:
	virtual BOOL OnInitDialog();

};
