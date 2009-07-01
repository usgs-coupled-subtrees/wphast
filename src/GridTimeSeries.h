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

class CGridTimeSeries
{
public:
	CGridTimeSeries(CWnd* pWnd);
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
	};

public:
	void InitializeGrid(CDataExchange* pDX);
	void ShowConstant(bool show);
	void ShowLinear(bool show);	
	void ShowPoints(bool show);
	void ShowXYZ(bool show);

	void SetSeries(const CTimeSeries<Cproperty> &series);
	void FreeVectors();
	void SetUnits(const CUnits &u);


	void DDX_Property(CDataExchange* pDX, Cproperty &p);
	void DDV_SoftValidate();
	void DDX_Vectors(int row, CString &str);
	void DDX_Series(CDataExchange* pDX, bool bTimeZeroRequired = true);
	void SetPointsGrid(CModGridCtrlEx *grid);

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
	std::vector<Cproperty*> v_xyz;

	CString DefaultTimeUnits;
};
