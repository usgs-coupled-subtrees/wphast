#pragma once


// CStartupDialog dialog

class CStartupDialog : public CDialog
{
	DECLARE_DYNAMIC(CStartupDialog)

public:
	CStartupDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStartupDialog();

// Dialog Data
	enum { IDD = IDD_STARTUP_INTRO };

	enum SDA_ACTION_TYPE { SDA_NEW_WIZARD, SDA_OPEN_FILE, SDA_CREATE_DEFAULT, SDA_IMPORT_FILE };

	SDA_ACTION_TYPE GetAction(void)          {  return m_nAction;    }
	void SetAction(SDA_ACTION_TYPE nAction)  {  m_nAction = nAction; }

	CString GetPathName(void)const           { return m_strPathName;     }
	void SetPathName(CString fileName)       { m_strPathName = fileName; }

protected:
	enum SDA_ACTION_TYPE m_nAction;
	CString m_strPathName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();
};
