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
#include "ChemIC.h"
#include "afxcmn.h"


// CICChemPropsPage dialog

class CICChemPropsPage // : public CPropertyPage
: public CResizablePage
, public TreePropSheet::CWhiteBackgroundProvider
{
	DECLARE_DYNAMIC(CICChemPropsPage)

public:
	CICChemPropsPage();
	virtual ~CICChemPropsPage();

	void CommonConstruct(void);

	void SetProperties(const CChemIC& rChemIC);
	void GetProperties(CChemIC& rChemIC)const;

	void SetFlowOnly(bool flowOnly)      { this->FlowOnly = flowOnly; }
	bool GetFlowOnly(void)const          { return this->FlowOnly; }

	void SetDefault(bool bDefault)       { this->Default = bDefault; }
	bool GetDefault(void)const           { return this->Default; }

	void SetDesc(LPCTSTR desc);
	LPCTSTR GetDesc()const;


// Dialog Data
	enum { IDD = IDD_PROPS_ICCHEM };

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

	CChemIC ChemIC;
	CChemIC ChemICConstant;
	CChemIC ChemICLinear;
	CChemIC ChemICPoints;
	CChemIC ChemICXYZ;
	
	CRichEditCtrl RichEditCtrl;

	HTREEITEM ItemDDX;

	bool FlowOnly;
	bool Default;

	std::string m_sDescriptionRTF;        // IDR_DESCRIPTION_RTF
	std::string m_sSolutionRTF;           // IDR_CHEM_IC_SOLUTION_RTF
	std::string m_sEquilibriumPhasesRTF;  // IDR_CHEM_IC_EQUILIBRIUM_PHASES_RTF
	std::string m_sSurfaceRTF;            // IDR_CHEM_IC_SURFACE_RTF
	std::string m_sExchangeRTF;           // IDR_CHEM_IC_EXCHANGE_RTF
	std::string m_sGasPhaseRTF;           // IDR_CHEM_IC_GAS_PHASE_RTF
	std::string m_sSolidSolutionsRTF;     // IDR_CHEM_IC_SOLID_SOLUTIONS_RTF
	std::string m_sKineticsRTF;           // IDR_CHEM_IC_KINETICS_RTF

	HTREEITEM htiSOLUTION;
	HTREEITEM htiEQUILIBRIUM_PHASES;
	HTREEITEM htiEXCHANGE;
	HTREEITEM htiSURFACE;
	HTREEITEM htiGAS_PHASE;
	HTREEITEM htiSOLID_SOLUTIONS;
	HTREEITEM htiKINETICS;

public:
	virtual BOOL OnInitDialog();

};
