#pragma once

#include "BC.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "afxwin.h"

// CFluxPropsPage2 dialog

class CFluxPropsPage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CFluxPropsPage2)

public:
	CFluxPropsPage2();
	virtual ~CFluxPropsPage2();

	void SetProperties(const CBC& rBC);
	void GetProperties(CBC& rBC)const;

	void SetFlowOnly(bool flowOnly)      { this->bFlowOnly = flowOnly; }
	bool GetFlowOnly(void)const          { return this->bFlowOnly; }

	void SetDefault(bool bDefault)       { this->bDefault = bDefault; }
	bool GetDefault(void)const           { return bDefault; }

	void SetDesc(LPCTSTR desc);
	LPCTSTR GetDesc()const;

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

	void SetMode(CFluxPropsPage2::ModeType mode);
	void ShowConstant(bool show);
	void ShowLinear(bool show);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void DDX_Property(CDataExchange* pDX, Cproperty &p);
	void DDX_Series(CDataExchange* pDX, CModGridCtrlEx &grid, CTimeSeries<Cproperty> &props);
	void DDX_Series(CDataExchange* pDX, CModGridCtrlEx &grid, std::vector<Ctime*> &times, std::vector<Cproperty*> &props);

	
	void DDX_Vectors(int row, CString &str, std::vector< std::vector<Cproperty*>* > &vectors);

	void InitializeGrid(CDataExchange* pDX, CModGridCtrlEx &grid, CTimeSeries<Cproperty> &series);


	DECLARE_MESSAGE_MAP()

 	afx_msg void OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *plResult);
 	afx_msg void OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *plResult);

	void OnEndLabelEdit(NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors);
	afx_msg void OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result);

	void OnSelChanged(NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid);
	afx_msg void OnSelChangedFlux(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result);

	CModGridCtrlEx   GridFlux;         // IDC_TIMES_GRID
	CModGridCtrlEx   GridSolution;     // IDC_GRID_SOLUTION
	CRichEditCtrl RichEditCtrl;
	CTreeCtrl     TreeCtrl;

	HTREEITEM ItemDDX;
	HTREEITEM htiFLUX;
	HTREEITEM htiSOLUTION;

	std::string m_sDescriptionRTF;   // IDR_DESCRIPTION_RTF
	std::string m_sAssocSolutionRTF; // IDR_BC_FLUX_ASSOC_SOL_RTF
	std::string m_sFluxRTF;          // IDR_BC_FLUX_FLUX_RTF
	std::string m_sFaceRTF;          // IDR_BC_FLUX_FACE_RTF

	CString DefaultUnits;

protected:
	CBC BC;
	//CBC BCConstant;
	//CBC BCLinear;
	//CBC BCPoints;
	//CBC BCXYZ;

	std::vector<Ctime*> FluxTimes;
	std::vector< std::vector<Cproperty*>* > FluxVectors; // indexed by ModeType
	std::vector<Cproperty*> FluxVector; // new
	std::vector<Cproperty*> FluxVectorConstant;
	std::vector<Cproperty*> FluxVectorLinear;

	std::vector<Ctime*> SolutionTimes;
	std::vector< std::vector<Cproperty*>* > SolutionVectors; // indexed by ModeType
	std::vector<Cproperty*> SolutionVector; // new
	std::vector<Cproperty*> SolutionVectorConstant;
	std::vector<Cproperty*> SolutionVectorLinear;


	bool bFlowOnly;
	bool bDefault;
public:
	afx_msg void OnEnSetfocusDescEdit();
	CStatic PropertyGroupBox;
};
