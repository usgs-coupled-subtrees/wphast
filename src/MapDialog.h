#pragma once

#include <map>

class vtkWin32OpenGLRenderWindow;
class vtkRenderer;
class vtkWin32RenderWindowInteractor;

class vtkInteractorStyle;
class vtkPlaneWidget2;
class vtkCallbackCommand;
class vtkObject;

class vtkImageReader2;
class vtkDataSetMapper;
class vtkImageShiftScale;
class vtkImageActor;
class CMarker;
class CMapImageActor2;
class CMapActor;

#include "ETSLayout/ETSLayout.h"
#include "afxcmn.h"
#include "GridKeyword.h"
#include "WorldTransform.h"
#include "SiteMap3.h"

class CGridPoint : public CPoint
{
public:
	CGridPoint(void);
	double x_val;
	double y_val;

	double x_world;
	double y_world;

	bool   x_defined;
	bool   y_defined;
	bool   x_val_defined;
	bool   y_val_defined;

	int    x_pixel;
	int    y_pixel;
};


// COMMENT: {9/7/2010 3:48:29 PM}#define USE_MAP_ACTOR

#define WM_SHOWCOORDLG      WM_USER + 100
#define CMapDialogBase ETSLayoutDialog
// COMMENT: {4/17/2006 9:54:18 PM}#define CMapDialogBase CDialog

// CMapDialog dialog

class CMapDialog : public CMapDialogBase
{
// COMMENT: {5/3/2004 9:38:11 PM}	DECLARE_DYNAMIC(CMapDialog)

public:
	CMapDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMapDialog();

// Dialog Data
	enum { IDD = IDD_MAP_DIALOG };

	DECLARE_LAYOUT();
	virtual CRect GetRect();

	CSiteMap3 GetSiteMap3(void)const { return m_siteMap3; }

	// BUGBUG should these be public?
	CGridKeyword  GridKeyword;

	enum State {
		MDS_Point1,
		MDS_Point2,
		MDS_Grid,
	};

	// BUGBUG should these be public?
	void SetState(State state);
	enum CMapDialog::State GetState(void)const { return m_CurrentState; }

protected:

	enum State m_CurrentState;

	CString m_strHeaderTitle;
	CString m_strHeaderSubTitle;

	bool m_bFirstPaint;
	bool m_bHaveWorld;

	vtkWin32OpenGLRenderWindow     *m_RenderWindow;
	vtkRenderer                    *m_Renderer;
	vtkWin32RenderWindowInteractor *m_RenderWindowInteractor;
	vtkInteractorStyle             *m_Style;
	vtkPlaneWidget2                *m_Widget;
	vtkCallbackCommand             *m_CallbackCommand;
	CSize                           m_szMinimumRender;

	CStatic                         m_headerBorder;


	// the map	
	CMapImageActor2      *m_MapImageActor2;
#if defined(USE_MAP_ACTOR)
	CMapActor            *MapActor;
#endif
	vtkTransform         *Transform;

	//handles the events
	static void ProcessEvents(vtkObject* object, 
							unsigned long event,
							void* clientdata, 
							void* calldata);

	void UpdateModelOriginX(void)const;
	void UpdateModelOriginY(void)const;
	void UpdateModelOriginAngle(void)const;

	void UpdateGridLocationX(void)const;
	void UpdateGridLocationY(void)const;
	void UpdateGridLocationZ(void)const;

	void UpdateLength(void)const;
	void UpdateWidth(void)const;

	void UpdatePoint1(void);
	void UpdatePoint2(void);


// COMMENT: {5/4/2004 10:09:24 PM}	void UpdateXNodes(void)const;
// COMMENT: {5/4/2004 10:09:24 PM}	void UpdateYNodes(void)const;
// COMMENT: {5/4/2004 10:09:24 PM}	void UpdateZNodes(void)const;
	void UpdateNodes(int idx)const;


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void DDX_Point1(CDataExchange* pDX);	
	virtual void DDX_Point2(CDataExchange* pDX);	
	virtual void DDX_Grid(CDataExchange* pDX);

	// layout
	void LayoutPointsPage(void);
	void LayoutGridPage(void);
	void VisiblePointsPage(BOOL bEnable);
	void VisibleGridPage(BOOL bEnable);
	void EnablePoint1(BOOL bEnable);
	void EnablePoint2(BOOL bEnable);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int SetFileName(const char *filename);
	int SetWorldFileName(const char *filename);	
	afx_msg void OnPaint();
	afx_msg void OnEnKillfocusEditMoAngle();
	afx_msg void OnEnKillfocusEditMoY();
	afx_msg void OnEnKillfocusEditMoX();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnKillFocusMaximum(int idx);
	afx_msg void OnEnKillfocusEditLength();
	afx_msg void OnEnKillfocusEditWidth();
	//{{
	afx_msg void OnStaticMOGB();
	//}}
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnBnClickedButton1();

	CSpinButtonCtrl m_udXNodes;
	CSpinButtonCtrl m_udYNodes;
	CSpinButtonCtrl m_udZNodes;

	CSpinButtonCtrl m_udXP1; // IDC_SPIN_XP1
	CSpinButtonCtrl m_udYP1; // IDC_SPIN_YP1
	CSpinButtonCtrl m_udXP2; // IDC_SPIN_XP2
	CSpinButtonCtrl m_udYP2; // IDC_SPIN_YP2

	afx_msg void OnWizardBack();

	afx_msg void OnEnUpdateNodes(int idx);
	afx_msg void OnEnUpdateXNodesEdit();
	afx_msg void OnEnUpdateYNodesEdit();

	afx_msg void OnEnKillfocusNodesEdit(int idx);
	afx_msg void OnEnKillfocusXNodesEdit();
	afx_msg void OnEnKillfocusYNodesEdit();
	afx_msg void OnEnKillfocusZNodesEdit();

	afx_msg LRESULT OnShowCoorDlg(WPARAM wParam, LPARAM lParam);

	CSiteMap3 m_siteMap3;

	double m_xMin;
	double m_yMin;
	double m_zMin;
	double m_xLength;
	int    m_xNodes;

	CMarker            *m_Point1Actor;
	CMarker            *m_Point2Actor;
	CWorldTransform     m_worldTransform;

	CGridPoint m_point1;
	CGridPoint m_point2;
	CGridPoint m_mouseDownPoint;

	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
protected:
	virtual void OnOK();
public:
	afx_msg void OnEnUpdateEditXp1();
	afx_msg void OnEnUpdateEditYp1();
	afx_msg void OnEnUpdateEditXp2();
	afx_msg void OnEnUpdateEditYp2();
	afx_msg void OnDeltaposSpinXp1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditXp1();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	//afx_msg void OnEnChangeEditXc2();
	//afx_msg void OnEnChangeEditYc2();
	// ON_EN_CHANGE(IDC_EDIT_XC2, OnEnChangeEditXc2)
	// ON_EN_CHANGE(IDC_EDIT_YC2, OnEnChangeEditYc2)
	afx_msg void OnEnChangeRange(UINT nID);
	std::map<UINT, bool> m_needsExchange;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnKillfocusEditX();
	afx_msg void OnEnKillfocusEditY();
	afx_msg void OnEnKillfocusEditZ();
};
