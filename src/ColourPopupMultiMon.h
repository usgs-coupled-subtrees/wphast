#pragma once
#include "colorpicker/ColourPopup.h"

class CColourPopupMultiMon : public CColourPopup
{
public:
    CColourPopupMultiMon(void);
    CColourPopupMultiMon(CPoint p, COLORREF crColour, CWnd* pParentWnd,
		LPCTSTR szDefaultText = NULL, LPCTSTR szCustomText = NULL);
	~CColourPopupMultiMon(void);

    BOOL Create(CPoint p, COLORREF crColour, CWnd* pParentWnd, 
                LPCTSTR szDefaultText = NULL, LPCTSTR szCustomText = NULL);

protected:
    virtual void SetWindowSize();
};
