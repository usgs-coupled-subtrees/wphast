#pragma once

#include "NewModelIntro.h"
#include "Units1PropertyPage.h"
#include "Units2PropertyPage.h"
#include "GridPropertyPage.h"
#include "MediaPropertyPage.h"
#include "ICHeadPropertyPage.h"
#include "TimeControlPropertyPage.h"

// CNewModelWizard2

class CNewModelWizard2 : public CPropertySheet
{
	DECLARE_DYNAMIC(CNewModelWizard2)

public:
	CNewModelWizard2(LPCTSTR pszCaption, CWnd* pParentWnd,
		UINT iSelectPage, HBITMAP hbmWatermark,
		HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);
	virtual ~CNewModelWizard2();

	CFlowOnly     GetFlowOnly(void)const;
	CUnits        GetUnits(void)const;
	CGrid         GetGridX(void)const;
	CGrid         GetGridY(void)const;
	CGrid         GetGridZ(void)const;
	CGridElt      GetGridElt(void)const;
	CHeadIC       GetHeadIC(void)const;
	CTimeControl2 GetTimeControl2(void)const;

	void SetGridElt(const CGridElt& rGridElt);
	void SetHeadIC(const CHeadIC& rHeadIC);

// Attributes
public:
	CNewModelIntro           m_introPage;
	CUnits1PropertyPage      m_units1Page;
	CUnits2PropertyPage      m_units2Page;
	CGridPropertyPage        m_gridPage;
	CMediaPropertyPage       m_mediaPage;
	CICHeadPropertyPage      m_icHeadPage;
	CTimeControlPropertyPage m_tcPage;

protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual INT_PTR DoModal();
};


