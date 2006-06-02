#pragma once


// CAxesPropertyPage dialog

class CAxesPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAxesPropertyPage)

public:
	CAxesPropertyPage();
	virtual ~CAxesPropertyPage();

// Dialog Data
	enum { IDD = IDD_AXES_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_XPos;
	float m_YPos;
	float m_ZPos;
};
