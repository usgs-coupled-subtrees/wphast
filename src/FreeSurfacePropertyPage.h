#pragma once


// CFreeSurfacePropertyPage dialog
#include "FreeSurface.h"
#include "afxcmn.h"

class CFreeSurfacePropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFreeSurfacePropertyPage)

public:
	CFreeSurfacePropertyPage();
	virtual ~CFreeSurfacePropertyPage();

	void SetFreeSurface(const CFreeSurface& freeSurface) { m_freeSurface = freeSurface; }
	CFreeSurface GetFreeSurface(void)const              { return m_freeSurface; }


// Dialog Data
	enum { IDD = IDD_FREE_SURFACE_INT_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CFreeSurface m_freeSurface;

	std::string m_sFreeSurfaceTrueRTF;  // IDR_FREE_SURFACE_TRUE_RTF
	std::string m_sFreeSurfaceFalseRTF; // IDR_FREE_SURFACE_FALSE_RTF

	CRichEditCtrl m_wndRichEditCtrl;
public:
	afx_msg void OnBnSetfocusFreeSurfaceRadio();
	afx_msg void OnBnSetfocusConfinedFlowRadio();
};
