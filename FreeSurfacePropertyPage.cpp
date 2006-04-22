// FreeSurfacePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "FreeSurfacePropertyPage.h"

#include "Global.h"

// CFreeSurfacePropertyPage dialog

IMPLEMENT_DYNAMIC(CFreeSurfacePropertyPage, CPropertyPage)
CFreeSurfacePropertyPage::CFreeSurfacePropertyPage()
	: CPropertyPage(CFreeSurfacePropertyPage::IDD)
{
	// load property descriptions
	//
	CGlobal::LoadRTFString(this->m_sFreeSurfaceTrueRTF,  IDR_FREE_SURFACE_TRUE_RTF);
	CGlobal::LoadRTFString(this->m_sFreeSurfaceFalseRTF, IDR_FREE_SURFACE_FALSE_RTF);
}

CFreeSurfacePropertyPage::~CFreeSurfacePropertyPage()
{
}

void CFreeSurfacePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DESC_RICHEDIT, m_wndRichEditCtrl);
	if (this->m_bFirstSetActive)
	{
		// wrap richedit to window
		this->m_wndRichEditCtrl.SetTargetDevice(NULL, 0);
		this->m_wndRichEditCtrl.SetWindowText(this->m_sFreeSurfaceTrueRTF.c_str());
	}

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
	ON_BN_SETFOCUS(IDC_FREE_SURFACE_RADIO, OnBnSetfocusFreeSurfaceRadio)
	ON_BN_SETFOCUS(IDC_CONFINED_FLOW_RADIO, OnBnSetfocusConfinedFlowRadio)
END_MESSAGE_MAP()


// CFreeSurfacePropertyPage message handlers

void CFreeSurfacePropertyPage::OnBnSetfocusFreeSurfaceRadio()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sFreeSurfaceTrueRTF.c_str());
}

void CFreeSurfacePropertyPage::OnBnSetfocusConfinedFlowRadio()
{
	this->m_wndRichEditCtrl.SetWindowText(this->m_sFreeSurfaceFalseRTF.c_str());
}
