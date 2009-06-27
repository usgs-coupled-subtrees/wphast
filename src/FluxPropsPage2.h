#pragma once

#include "BC.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "afxwin.h"

// COMMENT: {6/25/2009 3:52:34 PM}struct GridGroup
// COMMENT: {6/25/2009 3:52:34 PM}{
// COMMENT: {6/25/2009 3:52:34 PM}	CModGridCtrlEx grid;
// COMMENT: {6/25/2009 3:52:34 PM}	int validation_row;
// COMMENT: {6/25/2009 3:52:34 PM}	std::vector<Ctime*> v_times;
// COMMENT: {6/25/2009 3:52:34 PM}	std::vector< std::vector<Cproperty*>* > vv_props;
// COMMENT: {6/25/2009 3:52:34 PM}};
#include "GridTimeSeries.h"


// CFluxPropsPage2 dialog

class CFluxPropsPage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CFluxPropsPage2)

public:
	CFluxPropsPage2();
	virtual ~CFluxPropsPage2();

	void SetProperties(const CBC& rBC);
	void GetProperties(CBC& rBC)const;

	void SetFlowOnly(bool flowOnly)      { this->FlowOnly = flowOnly; }
	bool GetFlowOnly(void)const          { return this->FlowOnly; }

	void SetDefault(bool bDefault)       { this->Default = Default; }
	bool GetDefault(void)const           { return Default; }

	void SetDesc(LPCTSTR desc)           { this->Description = desc; }
	LPCTSTR GetDesc()                    { return this->Description.c_str(); }

// Dialog Data
	enum { IDD = IDD_PROPS_FLUX_2 };

	// type enum
	enum ModeType
	{
		NONE     = 0, // also represents chosen property
		CONSTANT = 1,
		LINEAR   = 2,
		POINTS   = 3,
		XYZ      = 4,
	};

// COMMENT: {6/25/2009 8:37:07 PM}	void SetMode(CFluxPropsPage2::ModeType mode);
// COMMENT: {6/25/2009 8:37:07 PM}	void ShowConstant(bool show);
// COMMENT: {6/25/2009 8:37:07 PM}	void ShowLinear(bool show);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void DDX_Property(CDataExchange* pDX, Cproperty &p);
// COMMENT: {6/25/2009 2:27:51 PM}	void DDX_Series(CDataExchange* pDX, CModGridCtrlEx &grid, CTimeSeries<Cproperty> &props);
	void DDX_Series(CDataExchange* pDX, CModGridCtrlEx &grid, std::vector<Ctime*> &times, std::vector<Cproperty*> &props, bool bTimeZeroRequired = true);
// COMMENT: {6/25/2009 7:55:27 PM}	void DDV_SoftValidate(CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors, int row);	
	void DDX_Vectors(int row, CString &str, std::vector< std::vector<Cproperty*>* > &vectors);

// COMMENT: {6/25/2009 9:03:07 PM}	void InitializeGrid(CDataExchange* pDX, CModGridCtrlEx &grid/*, CTimeSeries<Cproperty> &series*/);
// COMMENT: {6/25/2009 9:03:07 PM}	void FreeVectors();
// COMMENT: {6/25/2009 9:03:07 PM}	void SetCurrentFocusCell(CModGridCtrlEx &grid, int row, int col);

	DECLARE_MESSAGE_MAP()

 	afx_msg void OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *plResult);
 	afx_msg void OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *plResult);

	void OnEndLabelEdit(NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors);
	afx_msg void OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result);

	void OnSelChanged(NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid);
	afx_msg void OnSelChangedFlux(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result);

	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);

	afx_msg void OnEnSetfocusDescEdit();

	CGridTimeSeries FluxSeries;
	CGridTimeSeries SolutionSeries;

// COMMENT: {6/25/2009 4:04:44 PM}	CModGridCtrlEx FluxGrid;         // IDC_TIMES_GRID
// COMMENT: {6/25/2009 4:10:05 PM}	CModGridCtrlEx SolutionGrid;     // IDC_GRID_SOLUTION
	CRichEditCtrl  RichEditCtrl;
	CTreeCtrl      TreeCtrl;
	CStatic        PropertyGroupBox;

	HTREEITEM ItemDDX;
// COMMENT: {6/25/2009 4:25:15 PM}	HTREEITEM htiFLUX;
// COMMENT: {6/25/2009 4:25:15 PM}	HTREEITEM htiSOLUTION;

	std::string m_sDescriptionRTF;   // IDR_DESCRIPTION_RTF
	std::string m_sAssocSolutionRTF; // IDR_BC_FLUX_ASSOC_SOL_RTF
	std::string m_sFluxRTF;          // IDR_BC_FLUX_FLUX_RTF
	std::string m_sFaceRTF;          // IDR_BC_FLUX_FACE_RTF

	std::string Description;

	CString DefaultUnits;

	bool FlowOnly;
	bool Default;

protected:
	CBC BC;

// COMMENT: {6/25/2009 8:02:04 PM}	int FluxValidationRow;
// COMMENT: {6/25/2009 8:02:04 PM}	std::vector<Ctime*> FluxTimes;
// COMMENT: {6/25/2009 8:02:04 PM}	std::vector< std::vector<Cproperty*>* > FluxVectors; // indexed by ModeType
// COMMENT: {6/25/2009 8:02:04 PM}	std::vector<Cproperty*> FluxVector;
// COMMENT: {6/25/2009 8:02:04 PM}	std::vector<Cproperty*> FluxVectorConstant;
// COMMENT: {6/25/2009 8:02:04 PM}	std::vector<Cproperty*> FluxVectorLinear;
// COMMENT: {6/25/2009 8:02:04 PM}
// COMMENT: {6/25/2009 8:02:04 PM}	int SolutionValidationRow;
// COMMENT: {6/25/2009 8:02:04 PM}	std::vector<Ctime*> SolutionTimes;
// COMMENT: {6/25/2009 8:02:04 PM}	std::vector< std::vector<Cproperty*>* > SolutionVectors; // indexed by ModeType
// COMMENT: {6/25/2009 8:02:04 PM}	std::vector<Cproperty*> SolutionVector;
// COMMENT: {6/25/2009 8:02:04 PM}	std::vector<Cproperty*> SolutionVectorConstant;
// COMMENT: {6/25/2009 8:02:04 PM}	std::vector<Cproperty*> SolutionVectorLinear;
};
