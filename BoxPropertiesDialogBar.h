#pragma once
#include "sizecbar/sdbarcfvs7.h"
#include "IObserver.h"

class vtkObject;
class CWPhastView;
class vtkProp3D;
class vtkCubeSource;
class CUnits;

class CBoxPropertiesDialogBar :	public CSizingDialogBarCFVS7, public IObserver
{
    DECLARE_DYNAMIC(CBoxPropertiesDialogBar)
public:
	CBoxPropertiesDialogBar(void);
	virtual ~CBoxPropertiesDialogBar(void);

	virtual void Update(IObserver* pSender = 0, LPARAM lHint = 0L, CObject* pHint = 0, vtkObject* pObject = 0);

	void Set(CWPhastView* pView, vtkProp3D* pProp3D, const CUnits& units);
	vtkProp3D* GetProp3D(void);

	enum BP_TYPE {
		BP_POS_PLUS_LENGTH,
		BP_MIN_MAX,
	};

	virtual BOOL Create(UINT nIDTemplate, LPCTSTR lpszWindowName,
		CWnd* pParentWnd, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP);

protected:
	CWPhastView   *m_pView;
	vtkProp3D     *m_pProp3D;
	vtkCubeSource *m_CubeSource;
	CTabCtrl m_wndMethodTab;

	enum BP_TYPE m_nType;

	float m_X;
	float m_Y;
	float m_Z;

	float m_XLength;
	float m_YLength;
	float m_ZLength;

	float m_XMin;
	float m_XMax;
	float m_YMin;
	float m_YMax;
	float m_ZMin;
	float m_ZMax;

	CString m_strHorizontalUnits;
	CString m_strVerticalUnits;

	bool m_bNeedsUpdate;
	// float m_Position[3];
	virtual void DoDataExchange(CDataExchange* pDX);
public:
	void Enable(bool bEnable);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnEnChange(); 
	afx_msg void OnBnClickedApply();
	//afx_msg void OnEnKillfocusEditY();
	//afx_msg void OnEnKillfocusEditZ();
};
