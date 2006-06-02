#pragma once


// CCoorDialog dialog

class CCoorDialog : public CDialog
{
	DECLARE_DYNAMIC(CCoorDialog)

public:
	CCoorDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCoorDialog();

// Dialog Data
	enum { IDD = IDD_COOR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_x;
	double m_y;
};
