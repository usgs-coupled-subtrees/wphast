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

	void SetSeries(const CTimeSeries<Cproperty> &series);
	void FreeVectors();

	void DDX_Property(CDataExchange* pDX, Cproperty &p);
	void DDV_SoftValidate(/*CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors, int row*/);
	void DDX_Vectors(int row, CString &str /*, std::vector< std::vector<Cproperty*>* > &vectors*/);
	void DDX_Series(CDataExchange* pDX, /*CModGridCtrlEx &this->grid, std::vector<Ctime*> &times, std::vector<Cproperty*> &props,*/ bool bTimeZeroRequired = true);

	void OnEndLabelEdit(int nRow, int nCol /*NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid, std::vector< std::vector<Cproperty*>* > &vectors*/);
	void OnSelChanged(int nRow, int nCol   /*NMHDR *pNotifyStruct, LRESULT *result, CModGridCtrlEx &grid*/);


	void SetMode(CGridTimeSeries::ModeType mode);

public:
	CModGridCtrlEx grid;
	int validation_row;
	std::vector<Ctime*> v_times;
	std::vector< std::vector<Cproperty*>* > vv_props;
	HTREEITEM treeitem;
	CString DefaultUnits;

protected:
	CWnd* DlgWnd;           // container usually a dialog

	std::vector<Cproperty*> v_selected;
	std::vector<Cproperty*> v_constant;
	std::vector<Cproperty*> v_linear;

};
