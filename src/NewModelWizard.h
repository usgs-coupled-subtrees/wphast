#pragma once

#include "NewModelIntro.h"
#include "Units1PropertyPage.h"
#include "Units2PropertyPage.h"
#include "Units3PropertyPage.h"
#include "Units4PropertyPage.h"
#include "ImportMapPropertyPage.h"
#include "GridPropertyPage.h"
#include "MediaPropertyPage.h"
#include "ICHeadPropertyPage.h"
#include "ChemICPropertyPage.h"
#include "TimeControlPropertyPage.h"
#include "NewModel.h"

// CNewModelWizard

class CNewModelWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CNewModelWizard)

public:
	CNewModelWizard(LPCTSTR pszCaption, CWnd* pParentWnd,
		UINT iSelectPage, HBITMAP hbmWatermark,
		HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);
	virtual ~CNewModelWizard();

	CFlowOnly     GetFlowOnly(void)const;
	CUnits        GetUnits(void)const;
	CGrid         GetGridX(void)const;
	CGrid         GetGridY(void)const;
	CGrid         GetGridZ(void)const;
	CGridElt      GetGridElt(void)const;
	CHeadIC       GetHeadIC(void)const;
	CTimeControl2 GetTimeControl2(void)const;

	CNewModel    GetModel(void)const;

	void SetGridElt(const CGridElt& rGridElt);
	void SetHeadIC(const CHeadIC& rHeadIC);
	void SetChemIC(const CChemIC& chemIC);

// COMMENT: {6/1/2004 1:49:02 PM}	void SetFlowOnly(const CFlowOnly &flowOnly);

// Attributes
public:
	CNewModelIntro           m_introPage;
	CUnits1PropertyPage      m_units1Page;
	CUnits2PropertyPage      m_units2Page;
	CUnits3PropertyPage      m_units3Page;
	CUnits4PropertyPage      m_units4Page;
	CImportMapPropertyPage   m_importPage;
	CGridPropertyPage        m_gridPage;
	CMediaPropertyPage       m_mediaPage;
	CICHeadPropertyPage      m_icHeadPage;
	CChemICPropertyPage      m_chemICPage;
	CTimeControlPropertyPage m_tcPage;

protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual INT_PTR DoModal();
};


