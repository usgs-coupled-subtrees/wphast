#pragma once


// CRiverPointPropertyPage dialog

class CRiverPointPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CRiverPointPropertyPage)

public:
	CRiverPointPropertyPage();
	virtual ~CRiverPointPropertyPage();

// Dialog Data
	enum { IDD = IDD_RIVER_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
