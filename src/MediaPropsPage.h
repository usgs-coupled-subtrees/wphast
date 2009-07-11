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
#include "PropRich.h"

#include "TreePropSheetEx/TreePropSheetSplitter.h"
#include "TreePropSheetExSRC.h"

#include "PropStruct.h"
#include "GridElt.h"
#include "afxcmn.h"

#define MEDIA_SKIP

// CMediaPropsPage dialog

class CMediaPropsPage // : public CPropertyPage
: public CResizablePage
, public TreePropSheet::CWhiteBackgroundProvider
{
	DECLARE_DYNAMIC(CMediaPropsPage)

public:
	CMediaPropsPage();
	virtual ~CMediaPropsPage();

	void CommonConstruct(void);

	void SetProperties(const CGridElt& rGridElt);
	void GetProperties(CGridElt& rGridElt)const;

	void SetFlowOnly(bool flowOnly)      { this->FlowOnly = flowOnly; }
	bool GetFlowOnly(void)const          { return this->FlowOnly; }

	void SetDefault(bool bDefault)       { this->Default = bDefault; }
	bool GetDefault(void)const           { return this->Default; }

	void SetDesc(LPCTSTR desc);
	LPCTSTR GetDesc()const;


// Dialog Data
#if defined(MEDIA_SKIP)
	enum { IDD = IDD_PROPS_MEDIA1 };
#else
	enum { IDD = IDD_PROPS_MEDIA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

 	afx_msg void OnPageTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *plResult);
 	afx_msg void OnPageTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *plResult);
	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);



protected:
	bool RegisterSheet(UINT nID, CPropertySheet& rSheet);
// COMMENT: {4/1/2009 3:46:35 PM}	TreePropSheet::CTreePropSheetEx          SheetTop;
	//{{
	//CTreePropSheetExSRC                      SheetDesc;
	//CResizablePage                           PageDesc;
	CPropDesc                                PageDesc;
	CPropRich                                PageRich;
	//}}
	CTreePropSheetExSRC                      SheetTop;
// COMMENT: {5/28/2009 8:49:20 PM}	TreePropSheet::CTreePropSheetEx          SheetTop;
	TreePropSheet::CTreePropSheetSplitter    Splitter;

	CPropNone       PropNone;
	CPropConstant   PropConstant;
	CPropLinear     PropLinear;
	CPropPoints     PropPoints;
	CPropXYZ        PropXYZ;

	CGridElt GridElt;
	CGridElt GridEltConstant;
	CGridElt GridEltLinear;
	CGridElt GridEltPoints;
	CGridElt GridEltXYZ;
	
#if defined(MEDIA_SKIP)
	CRichEditCtrl RichEditCtrl;
#endif

	HTREEITEM ItemDDX;

	bool FlowOnly;
	bool Default;

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

	HTREEITEM htiACTIVE;
	HTREEITEM htiKX;
	HTREEITEM htiKY;
	HTREEITEM htiKZ;
	HTREEITEM htiPOROSITY;
	HTREEITEM htiSTORAGE;
	HTREEITEM htiALPHA_LONG;
	HTREEITEM htiALPHA_HORIZONTAL;
	HTREEITEM htiALPHA_VERTICAL; 

	//std::string sDesc;

public:
	virtual BOOL OnInitDialog();

};
