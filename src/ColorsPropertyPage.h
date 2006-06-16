#pragma once

#include "colorpicker/ColourPicker.h"
#include "DisplayColors.h"

// CColorsPropertyPage dialog

class CColorsPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CColorsPropertyPage)

public:
	CColorsPropertyPage();
	virtual ~CColorsPropertyPage();

// Dialog Data
	enum { IDD = IDD_COLORS };

	CDisplayColors m_colors;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg LRESULT OnSelChange(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	CColourPicker m_mediaColorPicker;
	CColourPicker m_headICColorPicker;
    CColourPicker m_chemICColorPicker;
	CColourPicker m_fluxColorPicker;
    CColourPicker m_leakyColorPicker;
    CColourPicker m_specHeadColorPicker;
    CColourPicker m_riverColorPicker;
    CColourPicker m_wellColorPicker;
};
