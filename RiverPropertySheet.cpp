// RiverPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "RiverPropertySheet.h"

#include "RiverPointPropertyPage.h"


// CRiverPropertySheet

IMPLEMENT_DYNAMIC(CRiverPropertySheet, CPropertySheet)
CRiverPropertySheet::CRiverPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	,m_pPages(0)
{
}

CRiverPropertySheet::CRiverPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	,m_pPages(0)
{
}

CRiverPropertySheet::~CRiverPropertySheet()
{
	delete[] this->m_pPages;
}


BEGIN_MESSAGE_MAP(CRiverPropertySheet, CPropertySheet)
END_MESSAGE_MAP()


// CRiverPropertySheet message handlers

void CRiverPropertySheet::SetRiver(const CRiver& r_river, const CUnits& r_units)
{
	while (this->GetPageCount())
	{
        this->RemovePage(0);
	}
	if (this->m_pPages) delete[] this->m_pPages;

	this->m_river = r_river;
	this->m_units = r_units;

	CString str;
	str.Format(_T("River %d"), this->m_river.n_user);
	this->SetTitle(str, PSH_PROPTITLE);

	ASSERT(this->m_river.m_listPoints.size() >= 2);
	this->m_pPages = new CRiverPointPropertyPage[this->m_river.m_listPoints.size()];

	std::list<CRiverPoint>::iterator iterRivPts = this->m_river.m_listPoints.begin();
	for (size_t i = 0; iterRivPts != this->m_river.m_listPoints.end(); ++iterRivPts, ++i)
	{
		this->m_pPages[i].SetProperties(*iterRivPts);
		this->m_pPages[i].SetUnits(this->m_units);
		if (i != 0 && i != this->m_river.m_listPoints.size() - 1)
		{
			this->m_pPages[i].SetFirstOrLastPoint(false);
		}

		this->m_pPages[i].SetFirstOrLastPoint(i == 0 || i == this->m_river.m_listPoints.size() - 1);
		this->AddPage(&this->m_pPages[i]);
	}
}

void CRiverPropertySheet::GetRiver(CRiver& r_river)const
{
	r_river = this->m_river;
}
