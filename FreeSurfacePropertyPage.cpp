// FreeSurfacePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "FreeSurfacePropertyPage.h"


// CFreeSurfacePropertyPage dialog

IMPLEMENT_DYNAMIC(CFreeSurfacePropertyPage, CPropertyPage)
CFreeSurfacePropertyPage::CFreeSurfacePropertyPage()
	: CPropertyPage(CFreeSurfacePropertyPage::IDD)
{
}

CFreeSurfacePropertyPage::~CFreeSurfacePropertyPage()
{
}

void CFreeSurfacePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	if (pDX->m_bSaveAndValidate)
	{
		// FREE_SURFACE_BC
		this->m_freeSurface = (IDC_FREE_SURFACE_RADIO == 
			this->GetCheckedRadioButton(IDC_CONFINED_FLOW_RADIO, IDC_FREE_SURFACE_RADIO));
	}
	else
	{
		// FREE_SURFACE_BC
		if (this->m_freeSurface) {
			this->CheckRadioButton(IDC_CONFINED_FLOW_RADIO, IDC_FREE_SURFACE_RADIO, IDC_FREE_SURFACE_RADIO);
		}
		else {
			this->CheckRadioButton(IDC_CONFINED_FLOW_RADIO, IDC_FREE_SURFACE_RADIO, IDC_CONFINED_FLOW_RADIO);
		}
	}
}


BEGIN_MESSAGE_MAP(CFreeSurfacePropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CFreeSurfacePropertyPage message handlers
