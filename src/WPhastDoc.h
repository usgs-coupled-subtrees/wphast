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
#include "IObserver.h"
#include "DisplayColors.h"

#include "PointConnectorMovePointAction.h"

class vtkProp;
class vtkPropCollection;
class CAction;

class vtkCubeSource;
class vtkPolyDataMapper;
class vtkLODActor;

struct zone;
class CZone;
class CZoneLODActor;
class CGridActor;

class CGrid;
class CGridKeyword;
class CUnits;
class CFlowOnly;
class CSteadyFlow;
class CFreeSurface;
class CSolutionMethod;
class CGridElt;
class CHeadIC;
class CTimeControl;
class CTimeControl2;
class CPrintInput;
class CPrintFreq;
class CNewModel;

class CModelessPropertySheet;
class CScalePropertyPage;
class CGridPropertyPage2;
class CGridRefinePage;
class CGridCoarsenPage;
class CGridElementsSelector;
class CNewZoneWidget;
class CNewWedgeWidget;
class CNewPrismWidget;

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
  class CRiverActor;
  class CDrainActor;

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
	WPN_SCALE_CHANGED = 4,
	WPN_SELCHANGING   = 5,
};

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CWPhastDoc : public CDocument, public CSubject, public IObserver
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

	// IObserver interface
	//
	virtual void Update(IObserver* pSender = 0, LPARAM lHint = 0L, CObject* pHint = 0, vtkObject* pObject = 0);

	void Add(CZoneActor *pZoneActor, HTREEITEM hInsertAfter = TVI_LAST);
	void UnAdd(CZoneActor *pZoneActor);
	void Delete(CZoneActor *pZoneActor);
	void UnDelete(CZoneActor *pZoneActor);
	void Select(CZoneActor *pZoneActor);

	void GetUsedZoneNumbers(std::set<int>& usedNums)const;
	void GetUsedWedgeNumbers(std::set<int>& usedNums)const;
	void GetUsedPrismNumbers(std::set<int>& usedNums)const;
	void GetUsedZoneFlowRates(std::set<int>& usedNums)const;

	int GetNextZoneNumber(void)const;
	int GetNextWedgeNumber(void)const;
	int GetNextPrismNumber(void)const;
	int GetNextZoneFlowRatesNumber(void)const;

	CString GetNextZoneName(void);
	CString GetNextWedgeName(void);
	CString GetNextPrismName(void);

	void Edit(CGridActor* pGridActor);
	void ModifyGrid(CGridActor* gridActor, CGridElementsSelector* gridElementsSelector);
	void SetGridKeyword(const CGridKeyword& gridKeyword);
	void GetGridKeyword(CGridKeyword& gridKeyword)const;
	CGridKeyword GetGridKeyword(void)const;

	vtkProp3D* GetGridActor(void);

	// Well actions
	void Add(CWellActor *pWellActor, HTREEITEM hInsertAfter = TVI_LAST);
	void UnAdd(CWellActor *pWellActor);
	void Remove(CWellActor *pWellActor);
	void UnRemove(CWellActor *pWellActor);
	void Select(CWellActor *pWellActor);
	int GetNextWellNumber(void);
	void GetUsedWellNumbers(std::set<int>& usedNums);


	// River actions
	//
	void Add(CRiverActor *pRiverActor, HTREEITEM hInsertAfter = TVI_LAST);
	void UnAdd(CRiverActor *pRiverActor);
	void Remove(CRiverActor *pRiverActor);
	void UnRemove(CRiverActor *pRiverActor);
	void Select(CRiverActor *pRiverActor);
	int GetNextRiverNumber(void);
	void GetUsedRiverNumbers(std::set<int>& usedNums);

	static void RiverListener(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata);

	// Drain actions
	//
	void Add(CDrainActor *pDrainActor, HTREEITEM hInsertAfter = TVI_LAST);
	void UnAdd(CDrainActor *pDrainActor);
	void Remove(CDrainActor *pDrainActor);
	void UnRemove(CDrainActor *pDrainActor);
	void Select(CDrainActor *pDrainActor);
	int GetNextDrainNumber(void);
	void GetUsedDrainNumbers(std::set<int>& usedNums);


	// Grid actions
	//
	static void GridListener(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata);	
	
	void UpdateGridDomain(void);
	void GetDefaultZone(zone &z)const;

	void SizeHandles(double size);

	void SetDisplayColors(const CDisplayColors& dc);
	CDisplayColors GetDisplayColors()const;

	void DataSourcePathsRelativeToAbsolute(LPCTSTR lpszPathName);
	void DataSourcePathsAbsoluteToRelative(LPCTSTR lpszPathName);
	std::string GetRelativePath(LPCTSTR lpszPathName, const std::string src_path)const;
	std::string GetAbsolutePath(LPCTSTR lpszPathName, const std::string relative_path)const;

protected:
	void InternalAdd(CZoneActor *pZoneActor, bool bAdd, HTREEITEM hInsertAfter = TVI_LAST);
	void InternalDelete(CZoneActor *pZoneActor, bool bDelete);

public:
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
	vtkPropAssembly   *m_pPropAssemblyRivers;
	vtkPropAssembly   *m_pPropAssemblyDrains;
	vtkPropAssembly   *m_pPropAssemblyZFR;

	// grid
	CGridActor *m_pGridActor;

	// axes
	CAxesActor *m_pAxesActor;

	// map
	CMapActor *m_pMapActor;

	// Geometry property sheet
	CModelessPropertySheet *m_pGeometrySheet;
	CScalePropertyPage     *m_pScalePage;

	// properties
	CUnits        *m_pUnits;

	CNewModel     *m_pModel;

	// callbacks
	vtkCallbackCommand    *RiverCallbackCommand;
	vtkCallbackCommand    *GridCallbackCommand;
	CPointConnectorMovePointAction<CRiverActor> *RiverMovePointAction;
	CPointConnectorMovePointAction<CDrainActor> *DrainMovePointAction;

	// properties
	enum ProjectionType m_ProjectionMode;

	// grid
	//
	CPropertySheet             *m_pGridSheet;
	CGridPropertyPage2         *m_pGridPage;

	CModelessPropertySheet     *ModifyGridSheet;
	CGridRefinePage            *GridRefinePage;
	CGridCoarsenPage           *GridCoarsenPage;
	CGridElementsSelector      *GridElementsSelector;

	// colors
	CDisplayColors DisplayColors;

protected:
	template<typename ACTOR>
	void SerializeActors(bool bStoring, hid_t loc_id, CTreeCtrlNode parentNode, const char* szNamesListHeading);

	void SerializeMedia(bool bStoring, hid_t loc_id);
	void SerializeIC(bool bStoring, hid_t loc_id);
	void SerializeBC(bool bStoring, hid_t loc_id);
	void SerializeWells(bool bStoring, hid_t loc_id);
	void SerializeRivers(bool bStoring, hid_t loc_id);
	void SerializeDrains(bool bStoring, hid_t loc_id);
	void SerializeZoneFlowRates(bool bStoring, hid_t loc_id);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
#if defined(WPHAST_AUTOMATION)
	// Generated OLE dispatch map functions

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
#endif
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	BOOL DoImport(LPCTSTR lpszPathName);
	BOOL DoExport(LPCTSTR lpszPathName);
	BOOL WriteTransDat(std::ostream& os);
	BOOL XMLExport(std::ostream& os, const char *prefix, LPCTSTR lpszPathName);
	friend class CXMLSerializer;

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

	void SetSolutionMethod(const CSolutionMethod &solutionMethod);
	CSolutionMethod GetSolutionMethod(void)const;

	CNewModel* GetModel(void);
	void SetModel(const CNewModel &model);

	// CUnits GetUnits(void);
	const CUnits& GetUnits(void)const;
	void GetUnits(CUnits& units)const;
	void SetUnits(const CUnits& units);
	void StandardizeUnits(CUnits& units)const;

	void SetTimeControl2(const CTimeControl2& timeControl2);
	const CTimeControl2& GetTimeControl2(void)const;

	void SetPrintFrequency(const CPrintFreq& printFreq);
	const CPrintFreq& GetPrintFrequency(void)const;

	void SetPrintInput(const CPrintInput& printInput);
	const CPrintInput& GetPrintInput(void)const;

	vtkFloatingPointType* GetGridBounds();
	void SetScale(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z);
	vtkFloatingPointType* GetScale();
	void GetScale(vtkFloatingPointType data[3]);
	afx_msg void OnUpdateToolsGeometry(CCmdUI *pCmdUI);
	afx_msg void OnToolsGeometry();
	virtual void DeleteContents();
	void ResetCamera(void);
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
	vtkPropAssembly* GetPropAssemblyRivers(void)const;
	vtkPropAssembly* GetPropAssemblyDrains(void)const;
	vtkPropAssembly* GetPropAssemblyZoneFlowRates(void)const;

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

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

protected:
	enum 
	{
		dispidRun = 4L,		dispidImport = 3L,		dispidSaveAs = 2L,		dispidVisible = 1
	};
	// Properties
	VARIANT GetVisible(void);
	void SetVisible(const VARIANT& newVal);
	// Methods
	VARIANT SaveAs(const VARIANT& FileName);
	VARIANT Import(const VARIANT& FileName);
	VARIANT Run(void);
public:
	afx_msg void OnViewHideAll();
	afx_msg void OnViewShowAll();

	// ID_RIVERS_HIDEALL handlers
	afx_msg void OnUpdateRiversHideAll(CCmdUI *pCmdUI);
	afx_msg void OnRiversHideAll();

	// ID_RIVERS_SHOWSELECTED handlers
	afx_msg void OnUpdateRiversShowSelected(CCmdUI *pCmdUI);
	afx_msg void OnRiversShowSelected();

	// ID_RIVERS_SELECTALL handlers
	afx_msg void OnRiversSelectAll();

	// ID_RIVERS_UNSELECTALL handlers
	afx_msg void OnRiversUnselectAll();

	// ID_TOOLS_MODIFYGRID
	afx_msg void OnUpdateToolsModifyGrid(CCmdUI *pCmdUI);
	afx_msg void OnToolsModifyGrid();
	void EndModifyGrid();

	// ID_TOOLS_NEWZONE
	afx_msg void OnUpdateToolsNewZone(CCmdUI *pCmdUI);
	afx_msg void OnToolsNewZone();
	void BeginNewZone();
	void EndNewZone();
	static void NewZoneListener(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
	vtkCallbackCommand *NewZoneCallbackCommand;
	CNewZoneWidget     *NewZoneWidget;

	// ID_TOOLS_NEWWEDGE
	afx_msg void OnUpdateToolsNewWedge(CCmdUI *pCmdUI);
	afx_msg void OnToolsNewWedge();
	void BeginNewWedge();
	void EndNewWedge();
	static void NewWedgeListener(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
	vtkCallbackCommand *NewWedgeCallbackCommand;
	CNewWedgeWidget    *NewWedgeWidget;

	// ID_TOOLS_NEWPRISM
	afx_msg void OnUpdateToolsNewPrism(CCmdUI *pCmdUI);
	afx_msg void OnToolsNewPrism();	
	void BeginNewPrism();
	void EndNewPrism();
	static void NewPrismListener(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
	vtkCallbackCommand *NewPrismCallbackCommand;
	CNewPrismWidget    *NewPrismWidget;

	// ID_TOOLS_NEWDRAIN
	afx_msg void OnUpdateToolsNewDrain(CCmdUI *pCmdUI);
	afx_msg void OnToolsNewDrain();	
	void BeginNewDrain();
	void EndNewDrain();
	static void NewDrainListener(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
	static void DrainListener(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
	void OnEndNewDrain(bool bCancel);
	vtkCallbackCommand *NewDrainCallbackCommand;
	CDrainActor        *NewDrainActor;
	vtkCallbackCommand *DrainCallbackCommand;


public:
	afx_msg void OnToolsColors();
};

inline vtkPropAssembly* CWPhastDoc::GetPropAssemblyMedia() const
{ return this->m_pPropAssemblyMedia; }

inline vtkPropAssembly* CWPhastDoc::GetPropAssemblyIC() const
{ return this->m_pPropAssemblyIC; }

inline vtkPropAssembly* CWPhastDoc::GetPropAssemblyBC() const
{ return this->m_pPropAssemblyBC; }

inline vtkPropAssembly* CWPhastDoc::GetPropAssemblyWells() const
{ return this->m_pPropAssemblyWells; }

inline vtkPropAssembly* CWPhastDoc::GetPropAssemblyRivers() const
{ return this->m_pPropAssemblyRivers; }

inline vtkPropAssembly* CWPhastDoc::GetPropAssemblyDrains() const
{ return this->m_pPropAssemblyDrains; }

inline vtkPropAssembly* CWPhastDoc::GetPropAssemblyZoneFlowRates() const
{ return this->m_pPropAssemblyZFR; }
