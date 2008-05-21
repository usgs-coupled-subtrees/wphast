#pragma once
#include "colorpicker/ColourPicker.h"

class CColourPickerMultiMon : public CColourPicker
{
public:
	CColourPickerMultiMon(void);
    DECLARE_DYNCREATE(CColourPickerMultiMon);

	~CColourPickerMultiMon(void);

    // Generated message map functions
protected:
    //{{AFX_MSG(CColourPicker)
    afx_msg BOOL OnClicked();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};
