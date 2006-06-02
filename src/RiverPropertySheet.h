#pragma once

#include "River.h"
#include "Units.h"

class CRiverPointPropertyPage;

// CRiverPropertySheet

class CRiverPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CRiverPropertySheet)

public:
	CRiverPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CRiverPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CRiverPropertySheet();

	void SetRiver(const CRiver& r_river, const CUnits& r_units);
	void GetRiver(CRiver& r_river)const;

protected:
	DECLARE_MESSAGE_MAP()
	CRiver m_river;
	CUnits m_units;
	CRiverPointPropertyPage* m_pPages;
};


