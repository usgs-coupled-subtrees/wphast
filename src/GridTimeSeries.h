#pragma once

#include <vector> // std::vector
//#include <iosfwd> // std::ostream

#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
#include "enum_fix.h"

#include "property.h"
#include "TimeSeries.h"

#include "gridctrl/ModGridCtrlEx.h"

class CUnits;

#define NEW_SINGLE_PROPERTY

class CGridTimeSeries
{
public:
	CGridTimeSeries(CWnd* pWnd);
#if defined(NEW_SINGLE_PROPERTY)
	CGridTimeSeries(CWnd* pWnd, bool bSingle, bool bMixture, bool bRestart = false);
#endif

	~CGridTimeSeries(void);

public:
	// type enum
	enum ModeType
	{
		NONE     = 0, // also represents chosen property
		CONSTANT = 1,
		LINEAR   = 2,
		POINTS   = 3,
		XYZ      = 4,
		XYZT     = 5,
		RESTART  = 6
	};

public:
	void InitializeGrid(CDataExchange* pDX);
	void ShowConstant(bool show);
	void ShowLinear(bool show);	
	void ShowPoints(bool show);
	void ShowXYZ(bool show);
#if defined(NEW_SINGLE_PROPERTY)
	bool bEnableMixture;
	bool bSingleProperty;
	static void ShowSingleProperty(CWnd *pDlgWnd, int nShow);
	void ShowMixture(bool show);
	void EnableMixture(BOOL bEnable);
	void ShowRestart(bool show);
#endif

	void SetSeries(const CTimeSeries<Cproperty> &series);
	void GetSeries(CTimeSeries<Cproperty> &series)const;

#if defined(NEW_SINGLE_PROPERTY)
	void SetProperty(const property *prop);
	void GetProperty(property *&prop)const;
#endif

	void FreeVectors();
	void SetUnits(const CUnits &u);

	void DDX_Property(CDataExchange* pDX, Cproperty &p);
	void DDX_Mixture(CDataExchange* pDX, Cproperty &p);
	void DDV_SoftValidate();
	void DDX_Vectors(int row, CString &str);
	void DDX_Series(CDataExchange* pDX, bool bTimeZeroRequired = true);
	void SetPointsGrid(CModGridCtrlEx *grid);

#if defined(NEW_SINGLE_PROPERTY)
	void DDX_Single(CDataExchange* pDX, bool bRequired = true);
	void OnCbnSelchangeComboProptype();
	void OnBnClickedCheckMixture();
#endif

	void OnEndLabelEdit(int nRow, int nCol);
	void OnSelChanged(int nRow, int nCol);
	void OnBnClickedButtonXYZ();

	void SetMode(CGridTimeSeries::ModeType mode);
	void SetType(PROP_TYPE pt);

public:
	CModGridCtrlEx grid;

	int validation_row;
	std::vector<Ctime*> v_times;
	std::vector< std::vector<Cproperty*>* > vv_props;
	HTREEITEM treeitem;

protected:
	CWnd *DlgWnd;           // container usually a dialog
	CModGridCtrlEx *PointsGrid;

	std::vector<Cproperty*> v_selected;
	std::vector<Cproperty*> v_constant;
	std::vector<Cproperty*> v_linear;
	std::vector<Cproperty*> v_points;
	std::vector<Cproperty*> v_restart;
	std::vector<Cproperty*> v_xyz;	
	std::vector<Cproperty*> v_xyzt;

	const bool bEnableRestart;
	CString DefaultTimeUnits;
};
