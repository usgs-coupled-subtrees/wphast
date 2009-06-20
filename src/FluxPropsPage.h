#pragma once

#include <vector>

#include "TreePropSheetEx/ResizablePage.h"
#include "TreePropSheetEx/TreePropSheetUtil.hpp"
#include "TreePropSheetEx/TreePropSheetEx.h"

#include "PropDesc.h"
#include "PropNoneM.h"
#include "PropConstantM.h"
#include "PropLinearM.h"
#include "PropPoints.h"
#include "PropXYZ.h"

#include "TreePropSheetEx/TreePropSheetSplitter.h"
#include "TreePropSheetExSRC.h"

#include "PropStruct.h"
#include "BC.h"
#include "afxcmn.h"


// CFluxPropsPage dialog

class CFluxPropsPage // : public CPropertyPage
: public CResizablePage
, public TreePropSheet::CWhiteBackgroundProvider
{
	DECLARE_DYNAMIC(CFluxPropsPage)

public:
	CFluxPropsPage();
	virtual ~CFluxPropsPage();

	void CommonConstruct(void);

	void SetProperties(const CBC& rBC);
	void GetProperties(CBC& rBC)const;

	void SetFlowOnly(bool flowOnly)      { this->bFlowOnly = flowOnly; }
	bool GetFlowOnly(void)const          { return this->bFlowOnly; }

	void SetDefault(bool bDefault)       { this->bDefault = bDefault; }
	bool GetDefault(void)const           { return bDefault; }

	void SetDesc(LPCTSTR desc);
	LPCTSTR GetDesc()const;


// Dialog Data
	enum { IDD = IDD_PROPS_FLUX };

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

	CPropNoneM       PropNoneM;
	CPropConstantM   PropConstantM;
	CPropLinearM     PropLinearM;
// COMMENT: {6/1/2009 5:42:16 PM}	CPropPoints     PropPoints;
// COMMENT: {6/1/2009 5:42:16 PM}	CPropXYZ        PropXYZ;

	CBC BC;
	CBC BCConstant;
	CBC BCLinear;
	CBC BCPoints;
	CBC BCXYZ;

	CRichEditCtrl RichEditCtrl;

	HTREEITEM ItemDDX;

	bool bFlowOnly;
	bool bDefault;

	std::string m_sDescriptionRTF;        // IDR_DESCRIPTION_RTF
	std::string m_sSolutionRTF;           // IDR_CHEM_IC_SOLUTION_RTF
// COMMENT: {6/1/2009 5:41:57 PM}	std::string m_sEquilibriumPhasesRTF;  // IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF
// COMMENT: {6/1/2009 5:41:57 PM}	std::string m_sSurfaceRTF;            // IDR_CHEM_IC_SURFACE_RTF
// COMMENT: {6/1/2009 5:41:57 PM}	std::string m_sExchangeRTF;           // IDR_CHEM_IC_EXCHANGE_RTF
// COMMENT: {6/1/2009 5:41:57 PM}	std::string m_sGasPhaseRTF;           // IDR_CHEM_IC_GAS_PHASE_RTF
// COMMENT: {6/1/2009 5:41:57 PM}	std::string m_sSolidSolutionsRTF;     // IDR_CHEM_IC_SOLID_SOLUTIONS_RTF
// COMMENT: {6/1/2009 5:41:57 PM}	std::string m_sKineticsRTF;           // IDR_CHEM_IC_KINETICS_RTF

	HTREEITEM htiFLUX;
	HTREEITEM htiSOLUTION;
// COMMENT: {6/1/2009 5:41:51 PM}	HTREEITEM htiEXCHANGE;
// COMMENT: {6/1/2009 5:41:51 PM}	HTREEITEM htiSURFACE;
// COMMENT: {6/1/2009 5:41:51 PM}	HTREEITEM htiGAS_PHASE;
// COMMENT: {6/1/2009 5:41:51 PM}	HTREEITEM htiSOLID_SOLUTIONS;
// COMMENT: {6/1/2009 5:41:51 PM}	HTREEITEM htiKINETICS;

public:
	virtual BOOL OnInitDialog();

};
