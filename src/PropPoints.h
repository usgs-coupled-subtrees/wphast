#pragma once

#include "PropPage.h"

#include "gridctrl/ModGridCtrlEx.h"
#include "property.h"

#include "afxwin.h"
#include "resource.h"

// CPropPoints dialog

class CPropPoints : public CPropPage
{
	DECLARE_DYNAMIC(CPropPoints)

public:
	CPropPoints();
	virtual ~CPropPoints();

	Cproperty GetProperty();
	void SetProperty(Cproperty p);


// Dialog Data
	enum { IDD = IDD_PROP_POINTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CModGridCtrlEx   PointsGrid;

	BOOL SetupGrids(void);

	Cproperty prop;

public:
	virtual BOOL OnInitDialog();
};
