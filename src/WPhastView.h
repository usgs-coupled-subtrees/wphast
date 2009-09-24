// WPhastView.h : interface of the CWPhastView class
//

#pragma once

#include "IObserver.h"

class vtkObject;

#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
class vtkMFCWindow;
#else
class vtkWin32OpenGLRenderWindow;
#endif

class vtkRenderer;
class vtkWin32RenderWindowInteractor;
class vtkBoxWidget;
class vtkPointWidget;
class vtkPointWidget2;
class CPrismWidget;
class CRiverActor;

class CViewVTKCommand;

class vtkInteractorStyle;

class vtkCursor3D;
class vtkPolyDataMapper;
class vtkActor;

class vtkCubeSource;
class vtkCylinderSource;
class CWellActor;

class CModifyGridCommand;
class vtkImplicitPlaneWidget;

class CWPhastView : public CView, public IObserver
{
protected: // create from serialization only
	CWPhastView();
	DECLARE_DYNCREATE(CWPhastView)

// Attributes
public:
	CWPhastDoc* GetDocument() const;

// Operations
public:
	void HighlightProp(vtkProp *pProp);
	void HighlightProp3D(vtkProp3D *pProp3D);
	void ClearSelection(void);
	void Select(vtkProp *pProp);
protected:
	vtkProp *CurrentProp;   // currently selected prop

public:
	// IObserver
	virtual void Update(IObserver* pSender = 0, LPARAM lHint = 0L, CObject* pHint = 0, vtkObject* pObject = 0);

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view

// COMMENT: {9/8/2009 8:46:21 PM}	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
private:
// COMMENT: {9/8/2009 8:46:10 PM}	void Pipeline( void );

public:
	virtual ~CWPhastView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Renderer
#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
	vtkMFCWindow                   *MFCWindow;
#else
	vtkWin32OpenGLRenderWindow     *m_RenderWindow;
#endif
	vtkRenderer                    *Renderer;
#if !((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
	vtkWin32RenderWindowInteractor *m_RenderWindowInteractor;
#endif
	CViewVTKCommand                *ViewVTKCommand;
	vtkInteractorStyle             *InteractorStyle;

	// widgets
	//
	vtkBoxWidget                   *BoxWidget;
	vtkPointWidget2                *PointWidget;
	CPrismWidget                   *PrismWidget;
	vtkCallbackCommand             *PrismWidgetCallbackCommand;
	vtkCallbackCommand             *RiverCallbackCommand;

	double                          BackgroundColor[3];

// COMMENT: {9/8/2009 8:46:00 PM}	bool                            m_bResetCamera;
	bool                            bMovingGridLine;

	// 3D Cursor
	vtkCursor3D                    *Cursor3D;
	vtkPolyDataMapper              *Cursor3DMapper;
	vtkActor                       *Cursor3DActor;

	// New well
	CWellActor                     *WellActor;

	// New river
	CRiverActor                    *RiverActor;

	int                             ViewFromDirection;

	//
	// coordinate mode
	enum CoordinateState
	{
		GridMode    = 0,
		MapMode     = 1,
	} CoordinateMode;

	friend class CViewVTKCommand;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	vtkRenderer* GetRenderer(void) const {return this->Renderer;}
#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
	vtkRenderWindowInteractor* GetInteractor(void);
	vtkBoxWidget    *GetBoxWidget(void)const {return this->BoxWidget;}
	vtkPointWidget2 *GetPointWidget(void)const {return this->PointWidget;}
	CPrismWidget    *GetPrismWidget(void)const {return this->PrismWidget;}
#else
	vtkWin32RenderWindowInteractor* GetRenderWindowInteractor(void) const {return this->m_RenderWindowInteractor;}
#endif

protected:
// COMMENT: {9/8/2009 8:44:03 PM}	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
// COMMENT: {9/8/2009 8:44:03 PM}	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
	void ResetCamera(void);
	void ResetCamera(double bounds[6]);
	void ResetCamera(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);

	virtual void OnInitialUpdate(void);
	void DeleteContents(void);
	
	void SizeHandles(double size);

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
// COMMENT: {9/8/2009 8:44:35 PM}	afx_msg void OnUpdateToolsNewZone(CCmdUI *pCmdUI);
// COMMENT: {9/8/2009 8:44:35 PM}	afx_msg void OnToolsNewZone();

	// Moving/Copying grid lines
	//
	void CancelMoveGridLine(void);
	bool MovingGridLine(void)const;
	void StartMoveGridLine(void);
	void EndMoveGridLine(void);

	void CancelMode(void);

	void SetBackground(COLORREF cr);

public:
	afx_msg void OnToolsNewWell();
	afx_msg void OnUpdateToolsNewWell(CCmdUI *pCmdUI);

	// New Well
	//
	bool CreatingNewWell(void)const;
	void StartNewWell(void);
	void CancelNewWell(void);
	void EndNewWell(void);

	// Select Object
	//
	bool SelectingObject(void)const;

	// Coordinate mode
	// 
	void SetMapMode(void);
	void SetGridMode(void);
	void SetCoordinateMode(CoordinateState mode);
	CWPhastView::CoordinateState GetCoordinateMode(void)const;

public:

	afx_msg void OnUpdateViewFromNx(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFromNy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFromNz(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFromPx(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFromPy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFromPz(CCmdUI *pCmdUI);

	afx_msg void OnViewFromNx();
	afx_msg void OnViewFromNy();
	afx_msg void OnViewFromNz();
	afx_msg void OnViewFromPx();
	afx_msg void OnViewFromPy();
	afx_msg void OnViewFromPz();
	afx_msg void OnViewFromMapNz();
	afx_msg void OnViewFromMapPz();
	afx_msg void OnViewFromNextDirection();
	afx_msg void OnViewFromPrevDirection();

	void ParallelProjectionOff(void);
	void ParallelProjectionOn(void);

protected:
// COMMENT: {9/8/2009 8:45:00 PM}	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

public:
	afx_msg void OnUpdateToolsNewRiver(CCmdUI *pCmdUI);
	afx_msg void OnToolsNewRiver();

	// Rivers
	//
	static void RiverListener(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata);
	void OnEndNewRiver(bool bCancel = false);
	bool CreatingNewRiver(void)const;
	void StartNewRiver(void);
	void CancelNewRiver(void);
	void EndNewRiver(void);

	// Prisms
	//
	static void PrismWidgetListener(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);

	// Wells
	//
	void UpdateWellMode(void);


// COMMENT: {9/8/2009 8:45:16 PM}	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnToolsMoveVerLine();
	afx_msg void OnUpdateToolsMoveVerLine(CCmdUI *pCmdUI);
// COMMENT: {9/9/2005 6:14:50 PM}	afx_msg void OnUpdateToolsModifyGrid(CCmdUI *pCmdUI);
	afx_msg void OnToolsModifyGrid();
	afx_msg void OnUpdateToolsSelectObject(CCmdUI *pCmdUI);
	afx_msg void OnToolsSelectObject();
// COMMENT: {9/8/2009 8:45:33 PM}	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

#ifndef _DEBUG  // debug version in WPhastView.cpp
inline CWPhastDoc* CWPhastView::GetDocument() const
   { return reinterpret_cast<CWPhastDoc*>(m_pDocument); }
#endif

