#pragma once
#include "afxcmn.h"

#include "TimeControl.h"
#include <list>
#include "gridctrl/ModGridCtrlEx.h"
// CTimeControlMultiPropertyPage dialog

class CTimeControlMultiPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTimeControlMultiPropertyPage)

public:
	CTimeControlMultiPropertyPage();
	virtual ~CTimeControlMultiPropertyPage();

// Dialog Data
	enum { IDD = IDD_TC_INT_M_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	std::list<CTimeControl> m_listTC;
	CModGridCtrlEx   m_Grid;              // IDC_GRID

	virtual BOOL OnInitDialog();
};
