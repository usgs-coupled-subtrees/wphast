#pragma once


class vtkWin32OpenGLRenderWindow;
class vtkRenderer;
class vtkWin32RenderWindowInteractor;

#include "ETSLayout/ETSLayout.h"

// CTestVTKDialog dialog

class CTestVTKDialog : public ETSLayoutDialog
{
	DECLARE_DYNAMIC(CTestVTKDialog)

public:
	CTestVTKDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestVTKDialog();

	DECLARE_LAYOUT();
	virtual CRect GetRect();

// Dialog Data
	enum { IDD = IDD_TEST_VTK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	vtkWin32OpenGLRenderWindow *m_RenderWindow;
	vtkRenderer *m_Renderer;
	vtkWin32RenderWindowInteractor *m_RenderWindowInteractor;

	HWND m_hWndStatusBar;
	CStatusBar statusBar;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
};
