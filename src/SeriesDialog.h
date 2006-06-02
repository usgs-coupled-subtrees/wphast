#pragma once


// CSeriesDialog dialog

class CSeriesDialog : public CDialog
{
	DECLARE_DYNAMIC(CSeriesDialog)

public:
	CSeriesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSeriesDialog();

// Dialog Data
	enum { IDD = IDD_SERIES_DIALOG };

	enum SeriesType { SD_LINEAR, SD_GROWTH } m_nType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_dStartValue;
	double m_dStepValue;
	double m_dStopValue;
	bool   m_bStopValueDefined;
};
