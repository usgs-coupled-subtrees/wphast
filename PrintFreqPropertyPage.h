#pragma once


#include "PrintFreq.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "Units.h"
// CPrintFreqPropertyPage dialog

class CPrintFreqPropertyPage : public CPropertyPage
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
	void DDX_PRINT(CDataExchange* pDX, int row, Ctime& time);

	CModGridCtrlEx   m_Grid;              // IDC_GRID

	DECLARE_MESSAGE_MAP()
};
