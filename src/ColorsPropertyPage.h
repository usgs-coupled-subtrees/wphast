#pragma once

#include "ColourPickerMultiMon.h"
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

	CColourPickerMultiMon m_mediaColorPicker;
	CColourPickerMultiMon m_headICColorPicker;
    CColourPickerMultiMon m_chemICColorPicker;
	CColourPickerMultiMon m_fluxColorPicker;
    CColourPickerMultiMon m_leakyColorPicker;
    CColourPickerMultiMon m_specHeadColorPicker;
    CColourPickerMultiMon m_riverColorPicker;
    CColourPickerMultiMon m_wellColorPicker;
    CColourPickerMultiMon m_bgColorPicker;
};
