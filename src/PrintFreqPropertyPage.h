#pragma once


#include "PrintFreq.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "Units.h"
#include "ETSLayout/ETSLayout.h"
#include "ETSLayoutPropertyPageXP.h"

#ifndef baseCPrintFreqPropertyPage
#define baseCPrintFreqPropertyPage ETSLayoutPropertyPageXP
//#define baseCPrintFreqPropertyPage CPropertyPage
#endif

// CPrintFreqPropertyPage dialog

class CPrintFreqPropertyPage : public baseCPrintFreqPropertyPage
{
	DECLARE_DYNAMIC(CPrintFreqPropertyPage)

public:
	CPrintFreqPropertyPage();
	virtual ~CPrintFreqPropertyPage();

	// Ctime  m_print_hdf_chem;
	CPrintFreq m_printFreq;
	CUnits     m_units;

// Dialog Data
	enum { IDD = IDD_PRINT_FREQUENCY_EXT_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void DDX_PRINT(CDataExchange* pDX, int nRow, int nCol, Ctime& time);
	void DDX_PRINT(CDataExchange* pDX, int nRow, int nCol, int& value);
	void DDX_PRINT(CDataExchange* pDX, int nRow, int nCol, CTimeSeries<Ctime>& timeSeries, const Ctime& time);
	void DDX_PRINT(CDataExchange* pDX, int nRow, int nCol, CTimeSeries<int>& timeSeries, const Ctime& time);


	CModGridCtrlEx   m_Grid;              // IDC_GRID

	CRichEditCtrl m_wndRichEditCtrl;

	afx_msg void OnSelChangedGrid(NMHDR *pNotifyStruct, LRESULT *result);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonDelete();

	afx_msg void OnEndLabelEditGrid(NMHDR *pNotifyStruct, LRESULT *result);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnSetfocusCheckSaveHeads();
};
