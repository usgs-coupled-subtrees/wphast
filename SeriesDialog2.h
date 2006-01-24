#pragma once


// CSeriesDialog2 dialog

class CSeriesDialog2 : public CDialog
{
	DECLARE_DYNAMIC(CSeriesDialog2)

public:
	CSeriesDialog2(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSeriesDialog2();

// Dialog Data
	enum { IDD = IDD_SERIES_DIALOG2 };

	enum SeriesType { SD_LINEAR, SD_GROWTH } m_nType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_dStartValue;
	double m_dStartIncrement;
	double m_dFactor;
	double m_dEndValue;
	bool   m_bEndValueDefined;
	afx_msg void OnBnClickedRadioLinear();
	afx_msg void OnBnClickedRadioGrowth();
};
