#pragma once


// CFreeSurfacePropertyPage dialog
#include "FreeSurface.h"

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
};
