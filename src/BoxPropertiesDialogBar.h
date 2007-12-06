#pragma once
#include "sizecbar/sdbarcfvs7.h"
#include "IObserver.h"
#include "gridctrl/ModGridCtrlEx.h"
#include <vector>

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
		BP_NONE,
		BP_POS_PLUS_LENGTH,
		BP_MIN_MAX,
		BP_WELL,
		BP_RIVER,
	};

	virtual BOOL Create(UINT nIDTemplate, LPCTSTR lpszWindowName,
		CWnd* pParentWnd, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP);

	void HideZoneControls(void);
	void ShowZoneControls(void);

	void HideWellControls(void);
	void ShowWellControls(void);

	void ShowRiverControls(void);
	void HideRiverControls(void);

	void ShowApply();
	void HideApply();

	void UpdateApply();

	void UpdateUnits(const CUnits& units);

protected:
	CWPhastView   *m_pView;
	vtkProp3D     *m_pProp3D;
	vtkCubeSource *m_CubeSource;
	CTabCtrl m_wndMethodTab;
	CModGridCtrlEx   m_wndRiverGrid;
	//CGridCtrl   m_wndRiverGrid;


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

	float m_XWell;
	float m_YWell;

	std::vector< std::pair<double, double> > m_vectorPoints;

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
public:
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnEndLabelEditGrid(NMHDR *pNotifyStruct, LRESULT *result);

	//
	///afx_msg void OnEnKillfocusYMAXEdit();
	afx_msg void OnEnKillfocusRange(UINT nID);
};
