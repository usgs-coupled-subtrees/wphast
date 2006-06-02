#pragma once

class CWPhastDoc;
class CSetScaleAction;

// CScalePropertyPage dialog

class CScalePropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CScalePropertyPage)

public:
	CScalePropertyPage(void);
	virtual ~CScalePropertyPage();
	//CAction* GetAction(void);

// Dialog Data
	enum { IDD = IDD_SCALE_PROPPAGE };
	float	m_XScale;
	float	m_YScale;
	float	m_ZScale;
	CWPhastDoc* m_pDoc;
	bool    m_bNeedAction;
	CSetScaleAction* m_pAction;
	bool    m_bModifiedDoc;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnApply();
	afx_msg void OnEnChange();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnQueryCancel();
	virtual void OnReset();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void PostNcDestroy();
};
