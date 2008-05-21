#include "StdAfx.h"
#include "ColourPickerMultiMon.h"
#include "ColourPopupMultiMon.h"


CColourPickerMultiMon::CColourPickerMultiMon(void)
{
}

CColourPickerMultiMon::~CColourPickerMultiMon(void)
{
}

IMPLEMENT_DYNCREATE(CColourPickerMultiMon, CColourPicker)

BEGIN_MESSAGE_MAP(CColourPickerMultiMon, CColourPicker)
    //{{AFX_MSG_MAP(CColourPickerMultiMon)
    ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// On mouse click, create and show a CColourPopup window for colour selection
BOOL CColourPickerMultiMon::OnClicked()
{
    m_bActive = TRUE;
    CRect rect;
    GetWindowRect(rect);
    new CColourPopupMultiMon(CPoint(rect.left, rect.bottom),    // Point to display popup
							 GetColour(),                       // Selected colour
							 this,                              // parent
							 m_strDefaultText,                  // "Default" text area
							 m_strCustomText);                  // Custom Text

    CWnd *pParent = GetParent();
    if (pParent)
        pParent->SendMessage(CPN_DROPDOWN, (LPARAM)GetColour(), (WPARAM) GetDlgCtrlID());

    // Docs say I should return FALSE to stop the parent also getting the message.
    // HA! What a joke.

    return TRUE;
}
