// WPhastDoc.h : interface of the CWPhastDoc class
//


#pragma once

#if defined(_MT)
#define _HDF5USEDLL_     /* reqd for Multithreaded run-time library (Win32) */
#endif
#include <hdf5.h>        /* HDF routines */

// forward declarations
#include <iosfwd> // std::ostream
#include "Subject.h"

class vtkProp;
class vtkPropCollection;
class CAction;

class vtkCubeSource;
class vtkPolyDataMapper;
class vtkLODActor;

class CZone;
class CZoneLODActor;
class CGridLODActor;

class CGrid;
class CUnits;
class CFlowOnly;
class CSteadyFlow;
class CFreeSurface;
class CGridElt;
class CHeadIC;
class CTimeControl;
class CPrintFreq;
class CNewModel;

class CModelessPropertySheet;
class CScalePropertyPage;

class vtkAxes;
class vtkTubeFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProp3D;
  class CAxesActor;
  class CZoneActor;
  class vtkAssembly;
  class vtkPropAssembly;
  class CWellActor;

// map
class CMapImageActor;
class CMapActor;
// COMMENT: {3/12/2004 4:47:12 PM}class vtkImageReader2;
// COMMENT: {3/12/2004 4:47:12 PM}class vtkImageShiftScale;
// COMMENT: {3/12/2004 4:47:12 PM}class vtkImageActor;

class CPropertyTreeControlBar;
class CBoxPropertiesDialogBar;

class CTreeCtrlNode;

enum ProjectionType {
	PT_PERSPECTIVE = 0,
	PT_PARALLEL    = 1
};

enum WPhastNotification {
	WPN_NONE          = 0,
	WPN_SELCHANGED    = 1,
	WPN_VISCHANGED    = 2,
	WPN_DELETE_WELL   = 3,
};

class CWPhastDoc : public CDocument, public CSubject
{
protected: // create from serialization only
	CWPhastDoc();
	DECLARE_DYNCREATE(CWPhastDoc)

// Attributes
public:
	CPropertyTreeControlBar* GetPropertyTreeControlBar() const;
	CBoxPropertiesDialogBar* GetBoxPropertiesDialogBar() const;

	vtkPropCollection *GetPropCollection() const;
	vtkPropCollection *GetRemovedPropCollection() const;

// Operations
public:
	void Execute(CAction* pAction);
	void New(const CNewModel& model);

public:
	void Add(CZoneActor *pZoneActor);
	void UnAdd(CZoneActor *pZoneActor);
	void Delete(CZoneActor *pZoneActor);
	void UnDelete(CZoneActor *pZoneActor);

	// Well actions
	void Add(CWellActor *pWellActor);
	void UnAdd(CWellActor *pWellActor);
	void Remove(CWellActor *pWellActor);
	void UnRemove(CWellActor *pWellActor);
	void Select(CWellActor *pWellActor);

protected:
	void InternalAdd(CZoneActor *pZoneActor, bool bAdd);
	void InternalDelete(CZoneActor *pZoneActor, bool bDelete);

public:
	CTreeCtrlNode AddStressPeriod(const CTimeControl& timeControl);
	void RemoveStressPeriod(int nStressPeriod);
	int GetStressPeriodCount(void)const;

	void ReleaseGraphicsResources(vtkProp* pProp);
	void ClearSelection(void);

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual CFile* GetFile(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError);

	// helper for standard commdlg dialogs
	virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle,
			DWORD lFlags, BOOL bOpenFileDialog);



// Implementation
public:
	virtual ~CWPhastDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:

protected:
	// pimpl
	struct WPhastDocImpl* m_pimpl;

	// prop collection
	vtkPropCollection *m_pPropCollection;
	vtkPropCollection *m_pRemovedPropCollection;

	vtkPropAssembly   *m_pPropAssemblyMedia;
	vtkPropAssembly   *m_pPropAssemblyBC;
	vtkPropAssembly   *m_pPropAssemblyIC;
	vtkPropAssembly   *m_pPropAssemblyWells;

	// grid
	CGridLODActor *m_pGridLODActor;

	// axes
	CAxesActor *m_pAxesActor;

	// map
	CMapActor *m_pMapActor;

	// Geometry property sheet
	CModelessPropertySheet *m_pGeometrySheet;
	CScalePropertyPage     *m_pScalePage;

	// properties
	CUnits       *m_pUnits;
	CTimeControl *m_pTimeControl;
	CPrintFreq   *m_pPrintFreq;

	CNewModel    *m_pModel;


	// properties
	int m_nNextZone;
	enum ProjectionType m_ProjectionMode;


protected:
	void InitDocument();
	void SerializeBC(bool bStoring, hid_t loc_id, int nStressPeriod = 1);
	void SerializeTimeControl(bool bStoring, hid_t loc_id, int nStressPeriod = 1);
	void SerializeIC(bool bStoring, hid_t loc_id);
	void SerializeMedia(bool bStoring, hid_t loc_id);
	void SerializeStressPeriods(bool bStoring, hid_t loc_id);
	void SerializeWells(bool bStoring, hid_t loc_id);


// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	BOOL DoImport(LPCTSTR lpszPathName);
	BOOL DoExport(LPCTSTR lpszPathName);
	BOOL WriteTransDat(std::ostream& os);

	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();

	void ResizeGrid(const CGrid&  x, const CGrid&  y, const CGrid&  z);

	void SetFlowOnly(const CFlowOnly& flowOnly);
	// const CFlowOnly& GetFlowOnly(void)const;
	CFlowOnly GetFlowOnly(void)const;

	void SetSteadyFlow(const CSteadyFlow &steadyFlow);
	CSteadyFlow GetSteadyFlow(void)const;

	void SetFreeSurface(const CFreeSurface &freeSurface);
	CFreeSurface GetFreeSurface(void)const;

	CNewModel* GetModel(void);
	void SetModel(const CNewModel &model);


	// CUnits GetUnits(void);
	const CUnits& GetUnits(void)const;
	void GetUnits(CUnits& units)const;
	void SetUnits(const CUnits& units);

	void SetTimeControl(const CTimeControl& timeControl, int nStressPeriod = 1);
	const CTimeControl& GetTimeControl(int nStressPeriod = 1)const;

	void SetPrintFrequency(const CPrintFreq& printFreq, int nStressPeriod = 1);
	const CPrintFreq& GetPrintFrequency(int nStressPeriod = 1)const;


	float* GetGridBounds();
	void SetScale(float x, float y, float z);
	float* GetScale();
	void GetScale(float data[3]);
	afx_msg void OnUpdateToolsGeometry(CCmdUI *pCmdUI);
	afx_msg void OnToolsGeometry();
	virtual void DeleteContents();
	void ResetCamera(void);
	CString GetNextZoneName(void);
	int GetNextWellNumber(void);
private:
	void AddDefaultZone(CZone* pZone);
public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	afx_msg void OnFileImport();
	afx_msg void OnFileExport();
	afx_msg void OnFileRun();
	afx_msg void OnToolsNewStressPeriod();
	afx_msg void OnUpdateViewAxes(CCmdUI *pCmdUI);
	afx_msg void OnViewAxes();
	afx_msg void OnUpdateSetprojectiontoParallel(CCmdUI *pCmdUI);
	afx_msg void OnSetprojectiontoParallel();
	afx_msg void OnUpdateSetprojectiontoPerspective(CCmdUI *pCmdUI);
	afx_msg void OnSetprojectiontoPerspective();
	afx_msg void OnUpdateViewSitemap(CCmdUI *pCmdUI);
	afx_msg void OnViewSitemap();
	afx_msg void OnViewGrid();
	afx_msg void OnUpdateViewGrid(CCmdUI *pCmdUI);

	// Media
	afx_msg void OnUpdateMediaZonesHideAll(CCmdUI *pCmdUI);
	afx_msg void OnMediaZonesHideAll();
	afx_msg void OnUpdateMediaZonesShowSelected(CCmdUI *pCmdUI);
	afx_msg void OnMediaZonesShowSelected();
	afx_msg void OnMediaZonesSelectAll();
	afx_msg void OnMediaZonesUnselectAll();

	// IC
	afx_msg void OnUpdateICZonesHideAll(CCmdUI *pCmdUI);
	afx_msg void OnICZonesHideAll();
	afx_msg void OnUpdateICZonesShowSelected(CCmdUI *pCmdUI);
	afx_msg void OnICZonesShowSelected();
	afx_msg void OnICZonesSelectAll();
	afx_msg void OnICZonesUnselectAll();

	// BC
	afx_msg void OnUpdateBCZonesHideAll(CCmdUI *pCmdUI);
	afx_msg void OnBCZonesHideAll();
	afx_msg void OnUpdateBCZonesShowSelected(CCmdUI *pCmdUI);
	afx_msg void OnBCZonesShowSelected();
	afx_msg void OnBCZonesSelectAll();
	afx_msg void OnBCZonesUnselectAll();

	vtkPropAssembly* GetPropAssemblyMedia(void)const;
	vtkPropAssembly* GetPropAssemblyIC(void)const;
	vtkPropAssembly* GetPropAssemblyBC(void)const;
	vtkPropAssembly* GetPropAssemblyWells(void)const;

public:
	void GetGrid(CGrid& x, CGrid& y, CGrid& z)const;

	// ID_WELLS_HIDEALL handlers
	afx_msg void OnUpdateWellsHideAll(CCmdUI *pCmdUI);
	afx_msg void OnWellsHideAll();

	// ID_WELLS_SHOWSELECTED handlers
	afx_msg void OnUpdateWellsShowSelected(CCmdUI *pCmdUI);
	afx_msg void OnWellsShowSelected();

	// ID_WELLS_SELECTALL handlers
	afx_msg void OnWellsSelectAll();

	// ID_WELLS_UNSELECTALL handlers
	afx_msg void OnWellsUnselectAll();

};

inline vtkPropAssembly* CWPhastDoc::GetPropAssemblyMedia() const
{ return this->m_pPropAssemblyMedia; }

inline vtkPropAssembly* CWPhastDoc::GetPropAssemblyIC() const
{ return this->m_pPropAssemblyIC; }

inline vtkPropAssembly* CWPhastDoc::GetPropAssemblyBC() const
{ return this->m_pPropAssemblyBC; }

inline vtkPropAssembly* CWPhastDoc::GetPropAssemblyWells() const
{ return this->m_pPropAssemblyWells; }
