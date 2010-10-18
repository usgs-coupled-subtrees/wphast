// WPhastDoc.cpp : implementation of the CWPhastDoc class
//
#include "stdafx.h"
#include "WPhast.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

#include "srcimpl.h"       // SrcFullPath SrcResolveShortcut
#include "HDFMirrorFile.h"
#include "PhastInput.h"
#include "PrintFreqPropertyPage.h"

// #include <vtkPropCollection.h>
#include <vtkCubeSource.h>
#include "srcWedgeSource.h"
#include <vtkPolyDataMapper.h>
#include <vtkLODActor.h>
#include <vtkProperty.h>

#include <vtkAxes.h>
#include <vtkTubeFilter.h>
#include <vtkAssembly.h>
#include <vtkPropAssembly.h>
#include <vtkProp3DCollection.h>
#include <vtkOutlineFilter.h>

#include <vector>
#include <list>
#include <fstream>  // std::ifstream
#include <sstream>  // std::ostringstream std::istringstream

#include "srcinput/Filedata.h"
#include "srcinput/Domain.h"

#include "Action.h"
#include "SetMediaAction.h"
#include "SetBCAction.h"
#include "SetChemICAction.h"
#include "SetHeadICAction.h"

#include "SetDisplayColorsAction.h"
#include "ZoneActor.h"
#include "DrainActor.h"
#include "PointConnectorCreateAction.h"
#include "RiverPropertyPage2.h"
#include "DrainPropertyPage.h"
#include "MediaZoneActor.h"
#include "BCZoneActor.h"
#include "ICZoneActor.h"
#include "ICHeadZoneActor.h"
#include "ICChemZoneActor.h"
#include "ZoneFlowRateZoneActor.h"

#include "MediaPropsPage2.h"
#include "FluxPropsPage2.h"
#include "LeakyPropsPage.h"
#include "HeadICPropsPage2.h"
#include "SpecifiedHeadPropsPage.h"
#include "ICChemPropsPage2.h"

#include "SeException.h"

#include "ISerial.h"
#include "DelayRedraw.h"

////#include "ZoneLODActor.h"
#include "GridActor.h"
#include "AxesActor.h"
#include "Global.h"

#include "ImportErrorDialog.h"
#include "RunDialog.h"
#include "PerimeterDlg.h"

#include "DisplayColors.h"
#include "PropertyTreeControlBar.h"
#include "BoxPropertiesDialogBar.h"
#include "TimeControlPropertyPage.h"
#include "ColorsPropertyPage.h"

#include "ModelessPropertySheet.h"
#include "ETSLayoutModelessPropertySheet.h"
#include "ScalePropertyPage.h"

#include "GridPropertyPage2.h"
#include "GridRefinePage.h"
#include "GridCoarsenPage.h"
#include "GridElementsSelector.h"

#include "NewZoneWidget.h"
#include "NewWedgeWidget.h"
#include "NewPrismWidget.h"

#include <vtkBoxWidget.h>

#include <vtkAbstractPropPicker.h>
#include <vtkWin32RenderWindowInteractor.h>
#include <vtkPolygon.h>
#include <vtkTriangleFilter.h>

#if defined(__CPPUNIT__)
#include <Pdh.h>
#endif

// #include "CreateZoneAction.h"
#include "ZoneCreateAction.h"
#include "AddStressPeriodAction.h"
#include "WellCreateAction.h"
#include "PointConnectorCreateAction.h"
#include "PointConnectorMovePointAction.h"
#include "PointConnectorInsertPointAction.h"
#include "GridDeleteLineAction.h"
#include "GridInsertLineAction.h"
#include "GridMoveLineAction.h"
#include "SetChemICAction.h"


// zone property pages
#include "NewZonePropertyPage.h"
#include "MediaSpreadPropertyPage.h"
#include "BCFluxPropertyPage2.h"
#include "BCLeakyPropertyPage2.h"
#include "BCSpecifiedHeadPropertyPage.h"
#include "ICHeadSpreadPropertyPage.h"
#include "ChemICSpreadPropertyPage.h"
#include "ZoneFlowRatePropertyPage.h"

#include "Unit.h"
#include "Units.h"
#include "NewModel.h"
#include "FlowOnly.h"
#include "SteadyFlow.h"
#include "FreeSurface.h"
#include "RedrawOnDtor.h"

#include "HeadIC.h"
#include "TimeControl2.h"
#include "MapActor.h"
// COMMENT: {8/19/2009 6:08:40 PM}#include "MapImageActor3.h"
#include "WorldTransform.h"
#include "SiteMap.h"
#include "NewModel.h"

#include "WellActor.h"
#include "RiverActor.h"
#include "Drain.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

#include "xml/StrX.h"
#include "xml/XStr.h"
#include "SaveCurrentDirectory.h"
#include "XMLSerializer.h"

extern void GetDefaultMedia(struct grid_elt* p_grid_elt);
extern void GetDefaultHeadIC(struct Head_ic* p_head_ic);
extern void GetDefaultChemIC(struct chem_ic* p_chem_ic);

static const TCHAR szZoneFormat[]    = _T("Box %d");
static const TCHAR szWedgeFormat[]   = _T("Wedge %d");
static const TCHAR szPrismFormat[]   = _T("Prism %d");

static const TCHAR szZoneFind[]      = _T("Box ");
static const TCHAR szWedgeFind[]     = _T("Wedge ");
static const TCHAR szPrismFind[]     = _T("Prism ");


int error_msg (const char *err_str, const int stop);

#include "FakeFiledata.h"
void Clear_NNInterpolatorList(void);
void Clear_file_data_map(void);
void Clear_KDtreeList(void);

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


struct WPhastDocImpl
{
	// Note: see http://www.gotw.ca/publications/mill04.htm
	std::vector<CAction*>            m_vectorActions;
	std::vector<CAction*>::size_type m_vectorActionsIndex;
	std::vector<CAction*>::size_type m_lastSaveIndex;
	std::vector<CZone*>              m_vectorDefaultZones;
};



// CWPhastDoc

IMPLEMENT_DYNCREATE(CWPhastDoc, CDocument)

BEGIN_MESSAGE_MAP(CWPhastDoc, CDocument)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_GEOMETRY, OnUpdateToolsGeometry)
	ON_COMMAND(ID_TOOLS_GEOMETRY, OnToolsGeometry)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_FILE_RUN, OnFileRun)
	ON_COMMAND(ID_TOOLS_NEWSTRESSPERIOD, OnToolsNewStressPeriod)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AXES, OnUpdateViewAxes)
	ON_COMMAND(ID_VIEW_AXES, OnViewAxes)
	ON_UPDATE_COMMAND_UI(ID_SETPROJECTIONTO_PARALLEL, OnUpdateSetprojectiontoParallel)
	ON_COMMAND(ID_SETPROJECTIONTO_PARALLEL, OnSetprojectiontoParallel)
	ON_UPDATE_COMMAND_UI(ID_SETPROJECTIONTO_PERSPECTIVE, OnUpdateSetprojectiontoPerspective)
	ON_COMMAND(ID_SETPROJECTIONTO_PERSPECTIVE, OnSetprojectiontoPerspective)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SITEMAP, OnUpdateViewSitemap)
	ON_COMMAND(ID_VIEW_SITEMAP, OnViewSitemap)
	ON_COMMAND(ID_VIEW_GRID, OnViewGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, OnUpdateViewGrid)

	ON_UPDATE_COMMAND_UI(ID_MEDIAZONES_HIDEALL, OnUpdateMediaZonesHideAll)
	ON_COMMAND(ID_MEDIAZONES_HIDEALL, OnMediaZonesHideAll)
	ON_UPDATE_COMMAND_UI(ID_MEDIAZONES_SHOWSELECTED, OnUpdateMediaZonesShowSelected)
	ON_COMMAND(ID_MEDIAZONES_SHOWSELECTED, OnMediaZonesShowSelected)
	ON_COMMAND(ID_MEDIAZONES_SELECTALL, OnMediaZonesSelectAll)
	ON_COMMAND(ID_MEDIAZONES_UNSELECTALL, OnMediaZonesUnselectAll)

	ON_UPDATE_COMMAND_UI(ID_ICZONES_HIDEALL, OnUpdateICZonesHideAll)
	ON_COMMAND(ID_ICZONES_HIDEALL, OnICZonesHideAll)
	ON_UPDATE_COMMAND_UI(ID_ICZONES_SHOWSELECTED, OnUpdateICZonesShowSelected)
	ON_COMMAND(ID_ICZONES_SHOWSELECTED, OnICZonesShowSelected)
	ON_COMMAND(ID_ICZONES_SELECTALL, OnICZonesSelectAll)
	ON_COMMAND(ID_ICZONES_UNSELECTALL, OnICZonesUnselectAll)

	ON_UPDATE_COMMAND_UI(ID_BCZONES_HIDEALL, OnUpdateBCZonesHideAll)
	ON_COMMAND(ID_BCZONES_HIDEALL, OnBCZonesHideAll)
	ON_UPDATE_COMMAND_UI(ID_BCZONES_SHOWSELECTED, OnUpdateBCZonesShowSelected)
	ON_COMMAND(ID_BCZONES_SHOWSELECTED, OnBCZonesShowSelected)
	ON_COMMAND(ID_BCZONES_SELECTALL, OnBCZonesSelectAll)
	ON_COMMAND(ID_BCZONES_UNSELECTALL, OnBCZonesUnselectAll)

	// ID_WELLS_HIDEALL
	ON_UPDATE_COMMAND_UI(ID_WELLS_HIDEALL, OnUpdateWellsHideAll)
	ON_COMMAND(ID_WELLS_HIDEALL, OnWellsHideAll)

	// ID_WELLS_SHOWSELECTED
	ON_UPDATE_COMMAND_UI(ID_WELLS_SHOWSELECTED, OnUpdateWellsShowSelected)
	ON_COMMAND(ID_WELLS_SHOWSELECTED, OnWellsShowSelected)

	// ID_WELLS_SELECTALL
	ON_COMMAND(ID_WELLS_SELECTALL, OnWellsSelectAll)

	// ID_WELLS_UNSELECTALL
	ON_COMMAND(ID_WELLS_UNSELECTALL, OnWellsUnselectAll)

	// ID_RIVERS_HIDEALL
	ON_UPDATE_COMMAND_UI(ID_RIVERS_HIDEALL, OnUpdateRiversHideAll)
	ON_COMMAND(ID_RIVERS_HIDEALL, OnRiversHideAll)

	// ID_RIVERS_SHOWSELECTED
	ON_UPDATE_COMMAND_UI(ID_RIVERS_SHOWSELECTED, OnUpdateRiversShowSelected)
	ON_COMMAND(ID_RIVERS_SHOWSELECTED, OnRiversShowSelected)

	// ID_RIVERS_SELECTALL
	ON_COMMAND(ID_RIVERS_SELECTALL, OnRiversSelectAll)

	// ID_RIVERS_UNSELECTALL
	ON_COMMAND(ID_RIVERS_UNSELECTALL, OnRiversUnselectAll)

	ON_COMMAND(ID_VIEW_HIDEALL, OnViewHideAll)
	ON_COMMAND(ID_VIEW_SHOWALL, OnViewShowAll)

	// ID_TOOLS_MODIFYGRID
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MODIFYGRID, OnUpdateToolsModifyGrid)
	ON_COMMAND(ID_TOOLS_MODIFYGRID, OnToolsModifyGrid)

	// ID_TOOLS_NEWZONE
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWZONE, OnUpdateToolsNewZone)
	ON_COMMAND(ID_TOOLS_NEWZONE, OnToolsNewZone)

	// ID_TOOLS_NEWWEDGE
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWWEDGE, OnUpdateToolsNewWedge)
	ON_COMMAND(ID_TOOLS_NEWWEDGE, OnToolsNewWedge)

	// ID_TOOLS_NEWPRISM
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWPRISM, OnUpdateToolsNewPrism)
	ON_COMMAND(ID_TOOLS_NEWPRISM, OnToolsNewPrism)

	// ID_TOOLS_NEWDRAIN
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWDRAIN, OnUpdateToolsNewDrain)
	ON_COMMAND(ID_TOOLS_NEWDRAIN, OnToolsNewDrain)

	// ID_TOOLS_COLORS
	ON_COMMAND(ID_TOOLS_COLORS, &CWPhastDoc::OnToolsColors)
	ON_COMMAND(ID_ACCELERATOR32862, &CWPhastDoc::OnAccelerator32862)
END_MESSAGE_MAP()

#if defined(WPHAST_AUTOMATION)
BEGIN_DISPATCH_MAP(CWPhastDoc, CDocument)
	DISP_PROPERTY_EX_ID(CWPhastDoc, "Visible", dispidVisible, GetVisible, SetVisible, VT_VARIANT)
	DISP_FUNCTION_ID(CWPhastDoc, "SaveAs", dispidSaveAs, SaveAs, VT_VARIANT, VTS_VARIANT)
	DISP_FUNCTION_ID(CWPhastDoc, "Import", dispidImport, Import, VT_VARIANT, VTS_VARIANT)
	DISP_FUNCTION_ID(CWPhastDoc, "Run", dispidRun, Run, VT_VARIANT, VTS_NONE)
END_DISPATCH_MAP()

// Note: we add support for IID_IWPhast to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the
//  dispinterface in the .IDL file.

// {7AF963C5-1815-4D7A-81DC-1987723AB9CE}
static const IID IID_IWPhast =
{ 0x7AF963C5, 0x1815, 0x4D7A, { 0x81, 0xDC, 0x19, 0x87, 0x72, 0x3A, 0xB9, 0xCE } };

BEGIN_INTERFACE_MAP(CWPhastDoc, CDocument)
	INTERFACE_PART(CWPhastDoc, IID_IWPhast, Dispatch)
END_INTERFACE_MAP()
#endif

CWPhastDoc::CWPhastDoc()
: m_pimpl(0)
, m_pPropCollection(0)
, m_pRemovedPropCollection(0)
, m_pGridActor(0)
, AxesActor(0)
, m_pGeometrySheet(0)
, m_pScalePage(0)
, m_pUnits(0)
, m_pModel(0) // , m_pFlowOnly(0)
, ProjectionMode(PT_PERSPECTIVE)
, m_pMapActor(0)
// COMMENT: {8/19/2009 6:11:30 PM}//{{
// COMMENT: {8/19/2009 6:11:30 PM}, MapImageActor3(0)
// COMMENT: {8/19/2009 6:11:30 PM}//}}
, m_pPropAssemblyMedia(0)
, m_pPropAssemblyBC(0)
, m_pPropAssemblyIC(0)
, m_pPropAssemblyWells(0)
, m_pPropAssemblyRivers(0)
, m_pPropAssemblyDrains(0)
, m_pPropAssemblyZFR(0)
, RiverCallbackCommand(0)
, GridCallbackCommand(0)
, RiverMovePointAction(0)
, DrainCallbackCommand(0)
, DrainMovePointAction(0)
, m_pGridSheet(0)
, m_pGridPage(0)
, ModifyGridSheet(0)
, GridRefinePage(0)
, GridCoarsenPage(0)
, GridElementsSelector(0)
, NewZoneWidget(0)
, NewZoneCallbackCommand(0)
, NewWedgeWidget(0)
, NewWedgeCallbackCommand(0)
, NewPrismWidget(0)
, NewPrismCallbackCommand(0)
, NewDrainActor(0)
, NewDrainCallbackCommand(0)
, hGridAccel(0)
{
#if defined(WPHAST_AUTOMATION)
	EnableAutomation();

	AfxOleLockApp();
#endif

	VERIFY(this->hGridAccel = ::LoadAccelerators(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_GRID_ACCEL)));

	ASSERT(this->m_pimpl == 0);
	this->m_pimpl = new WPhastDocImpl();
	this->m_pimpl->m_vectorActionsIndex = 0;
	this->m_pimpl->m_lastSaveIndex = -1;

	// create the prop list
	//
	ASSERT(this->m_pPropCollection == 0);
	this->m_pPropCollection = vtkPropCollection::New();

	// create the removed prop list
	//
	ASSERT(this->m_pRemovedPropCollection == 0);
	this->m_pRemovedPropCollection = vtkPropCollection::New();

	// create the prop-assemblies
	//
	ASSERT(this->m_pPropAssemblyMedia  == 0);
	ASSERT(this->m_pPropAssemblyIC     == 0);
	ASSERT(this->m_pPropAssemblyBC     == 0);
	ASSERT(this->m_pPropAssemblyWells  == 0);
	ASSERT(this->m_pPropAssemblyRivers == 0);
	ASSERT(this->m_pPropAssemblyDrains == 0);
	ASSERT(this->m_pPropAssemblyZFR    == 0);

	this->m_pPropAssemblyMedia  = vtkPropAssembly::New();
	this->m_pPropAssemblyIC     = vtkPropAssembly::New();
	this->m_pPropAssemblyBC     = vtkPropAssembly::New();
	this->m_pPropAssemblyWells  = vtkPropAssembly::New();
	this->m_pPropAssemblyRivers = vtkPropAssembly::New();
	this->m_pPropAssemblyDrains = vtkPropAssembly::New();	
	this->m_pPropAssemblyZFR    = vtkPropAssembly::New();

	// create the axes
	//
	this->AxesActor = CAxesActor::New();

	// create Geometry property sheet
	//
	this->m_pGeometrySheet = new CModelessPropertySheet("Geometry");
	this->m_pScalePage = new CScalePropertyPage();
	this->m_pScalePage->m_pDoc = this;
	this->m_pGeometrySheet->AddPage(this->m_pScalePage);

	// create units
	//
	this->m_pUnits = new CUnits;

	// create model
	//
	this->m_pModel = new CNewModel;
	this->m_pModel->m_printFreq = CPrintFreq::NewDefaults();

	// river event listener
	//
	this->RiverCallbackCommand = vtkCallbackCommand::New();
	this->RiverCallbackCommand->SetClientData(this);
	this->RiverCallbackCommand->SetCallback(CWPhastDoc::RiverListener);

	// drain event listener
	//
	this->DrainCallbackCommand = vtkCallbackCommand::New();
	this->DrainCallbackCommand->SetClientData(this);
	this->DrainCallbackCommand->SetCallback(CWPhastDoc::DrainListener);

	// grid event listener
	//
	this->GridCallbackCommand = vtkCallbackCommand::New();
	this->GridCallbackCommand->SetClientData(this);
	this->GridCallbackCommand->SetCallback(CWPhastDoc::GridListener);

	//{{
	// grid actor
	//
	this->m_pGridActor = CGridActor::New();
	ASSERT(this->m_pGridActor);
	this->m_pGridActor->AddObserver(CGridActor::DeleteGridLineEvent, this->GridCallbackCommand);
	this->m_pGridActor->AddObserver(CGridActor::InsertGridLineEvent, this->GridCallbackCommand);
	this->m_pGridActor->AddObserver(CGridActor::MoveGridLineEvent,   this->GridCallbackCommand);
	this->m_pGridActor->SetScale(1, 1, 1);
	this->m_pGridActor->SetPickable(0);
	//}}

	// colors
	//
	this->SetDisplayColors(this->DisplayColors);
}

#define CLEANUP_ASSEMBLY_MACRO(PTR_ASSEMBLY) \
	ASSERT(PTR_ASSEMBLY); \
	if (PTR_ASSEMBLY) { \
		if (vtkPropCollection *pPropCollection = PTR_ASSEMBLY->GetParts()) { \
			ASSERT(pPropCollection->GetNumberOfItems() == 0); \
			pPropCollection->RemoveAllItems(); \
		} \
		PTR_ASSEMBLY->Delete(); \
		PTR_ASSEMBLY = 0; \
	}
#define ASSERT_DELETE_SET_NULL_MACRO(POINTER) \
	ASSERT(POINTER); \
	if (POINTER) { \
		delete POINTER; \
		POINTER = 0; \
	}

CWPhastDoc::~CWPhastDoc()
{
#if defined(WPHAST_AUTOMATION)
	AfxOleUnlockApp();
#endif

	ASSERT(this->hGridAccel);
	if (this->hGridAccel)
	{
		VERIFY(::DestroyAcceleratorTable(this->hGridAccel));
		this->hGridAccel = 0;
	}

	ASSERT(this->m_pimpl);
	if (this->m_pimpl)
	{
		ASSERT(this->m_pimpl->m_vectorActions.empty()); // should be emptied in DeleteContents
		delete this->m_pimpl;
		this->m_pimpl = 0;
	}

	ASSERT(this->m_pPropCollection);
	if (this->m_pPropCollection)
	{
		ASSERT(this->m_pPropCollection->GetNumberOfItems() == 0); // should be emptied in DeleteContents
		this->m_pPropCollection->Delete();
		this->m_pPropCollection = 0;
	}

	ASSERT(this->m_pRemovedPropCollection);
	if (this->m_pRemovedPropCollection)
	{
		ASSERT(this->m_pRemovedPropCollection->GetNumberOfItems() == 0); // should be emptied in DeleteContents
		this->m_pRemovedPropCollection->Delete();
		this->m_pRemovedPropCollection = 0;
	}

 	ASSERT(this->m_pGridActor);
	if (this->m_pGridActor)
	{
		this->m_pGridActor->Delete();
		this->m_pGridActor = 0;
	}

	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyMedia);
	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyIC);
	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyBC);
	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyWells);
	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyRivers);
	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyDrains);
	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyZFR);
	

	ASSERT(this->AxesActor);
	if (this->AxesActor)
	{
		this->AxesActor->Delete();
		this->AxesActor = 0;
	}

	ASSERT(this->m_pScalePage == 0);     // should be deleted in CWPhastDoc::OnCloseDocument()
	ASSERT(this->m_pGeometrySheet == 0); // should be deleted in CWPhastDoc::OnCloseDocument()

	ASSERT_DELETE_SET_NULL_MACRO(this->m_pUnits);
	ASSERT_DELETE_SET_NULL_MACRO(this->m_pModel);

	if (this->m_pMapActor)
	{
		this->m_pMapActor->Delete();
		this->m_pMapActor = 0;
	}

	// callbacks
	//
	if (this->RiverCallbackCommand)
	{
		this->RiverCallbackCommand->Delete();
	}
	if (this->DrainCallbackCommand)
	{
		this->DrainCallbackCommand->Delete();
	}
	if (this->GridCallbackCommand)
	{
		this->GridCallbackCommand->Delete();
	}

	// Grid
	//
	if (this->m_pGridPage)
	{
		this->m_pGridPage->DestroyWindow();
		delete this->m_pGridPage;
		this->m_pGridPage = 0;
	}
	if (this->m_pGridSheet)
	{
		this->m_pGridSheet->DestroyWindow();
		delete this->m_pGridSheet;
		this->m_pGridSheet = 0;
	}

	// Grid modifiers
	//
	if (this->ModifyGridSheet)
	{
		this->ModifyGridSheet->DestroyWindow();
		delete this->ModifyGridSheet;
		this->ModifyGridSheet = 0;
	}
	if (this->GridRefinePage)
	{
		this->GridRefinePage->DestroyWindow();
		delete this->GridRefinePage;
		this->GridRefinePage = 0;
	}
	if (this->GridCoarsenPage)
	{
		this->GridCoarsenPage->DestroyWindow();
		delete this->GridCoarsenPage;
		this->GridCoarsenPage = 0;
	}
	ASSERT(this->NewZoneWidget == 0);           // should be deleted in pView->CancelMode
	ASSERT(this->NewZoneCallbackCommand == 0);  // should be deleted in pView->CancelMode
	ASSERT(this->GridElementsSelector == 0);    // should be deleted in pView->CancelMode
}

BOOL CWPhastDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// add reinitialization code here
	// (SDI documents will reuse this document)
	TRACE("CWPhastDoc::OnNewDocument()\n");
	this->SetModifiedFlag(TRUE);

#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
	if (vtkPropCollection *props = this->GetPropCollection())
	{
		// should be removed in DeleteContents()
		ASSERT(props->GetNumberOfItems() == 0);
	}

	// execute object pipeline
	this->ExecutePipeline();
#endif

	return TRUE;
}

// CWPhastDoc serialization

void CWPhastDoc::Serialize(CArchive& ar)
{
	static const char szWPhast[] = "WPhast";
	herr_t status;

#if defined(__CPPUNIT__)
	HQUERY hquery;
	PDH_STATUS pdh_status = PdhOpenQuery(NULL, 0, &hquery);
	VERIFY(pdh_status == ERROR_SUCCESS);

	PDH_HCOUNTER hcounter;
	pdh_status = PdhAddCounter(hquery, _T("\\Process(WPhast)\\Private Bytes"), 0, &hcounter);
	if (pdh_status != ERROR_SUCCESS)
	{
		PdhCloseQuery(hquery);
	}

	pdh_status = PdhCollectQueryData(hquery);
	if (pdh_status == ERROR_SUCCESS)
	{
		PDH_RAW_COUNTER value;
		DWORD dwType;
		pdh_status = PdhGetRawCounterValue(hcounter, &dwType, &value);
		if (pdh_status == ERROR_SUCCESS)
		{
			TRACE("Before serialize %lld %lld\n", value.TimeStamp, value.FirstValue);
		}
	}
#endif

	CWaitCursor wait;

	bool bStoring = (ar.IsStoring() == TRUE);

	if (ar.IsStoring())
	{
		// Update StatusBar
		//
		if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
		{
			CString status(_T("Saving..."));
			pWnd->SetWindowText(status);
		}

		// get hdf file handle
		//
		CHDFMirrorFile* pFile = (CHDFMirrorFile*)ar.GetFile();
		ASSERT(pFile->GetHID() > 0);

		// Create the "/WPhast" group
		//
		hid_t wphast_id = ::H5Gcreate(pFile->GetHID(), szWPhast, 0);
		ASSERT(wphast_id > 0);
		if (wphast_id > 0)
		{
			// store flowonly
			ASSERT(this->m_pModel);
			this->m_pModel->m_flowOnly.Serialize(bStoring, wphast_id);
			this->m_pModel->m_freeSurface.Serialize(bStoring, wphast_id);
			this->m_pModel->m_steadyFlow.Serialize(bStoring, wphast_id);
			this->m_pModel->m_solutionMethod.Serialize(bStoring, wphast_id);
			this->m_pModel->m_printInput.Serialize(bStoring, wphast_id);
			this->m_pModel->m_printFreq.Serialize(bStoring, wphast_id);
			this->m_pModel->m_timeControl2.Serialize(bStoring, wphast_id);

			// store units
			ASSERT(this->m_pUnits);
			this->m_pUnits->Serialize(bStoring, wphast_id);

			// store grid
			this->m_pGridActor->Serialize(bStoring, wphast_id);

			// store axes
			//this->m_pAxesActor->Serialize(bStoring, wphast_id);  // not implemented

			// store site map
			if (this->m_pMapActor) this->m_pMapActor->Serialize(bStoring, wphast_id);

			// store display colors
			this->DisplayColors.Serialize(bStoring, wphast_id);

			// store media
			this->SerializeMedia(bStoring, wphast_id);

			// store ic
			this->SerializeIC(bStoring, wphast_id);

			// store bc
			this->SerializeBC(bStoring, wphast_id);

			// store wells
			this->SerializeWells(bStoring, wphast_id);

			// store rivers
			this->SerializeRivers(bStoring, wphast_id);

			// store drains
			this->SerializeDrains(bStoring, wphast_id);

			// store zone flow rates
			this->SerializeZoneFlowRates(bStoring, wphast_id);

			// close WPhast group
			status = ::H5Gclose(wphast_id);
			ASSERT(status >= 0);
		}
	}
	else
	{
		// add loading code here

		// Update StatusBar
		//
		if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
		{
			CString status(_T("Loading..."));
			pWnd->SetWindowText(status);
		}

		// delay redrawing treectrl
		//
		CWnd *pWndTreeCtrl = 0;
		if (CPropertyTreeControlBar *pPropertyTreeControlBar = this->GetPropertyTreeControlBar())
		{
			pWndTreeCtrl = pPropertyTreeControlBar->GetTreeCtrl();
		}
		CDelayRedraw delayTree(pWndTreeCtrl);

		// delay redrawing render window
		//
		CDelayRedraw delayRender(::AfxGetMainWnd()->GetActiveWindow(), this);

		// This fixes the BUG that occured when creating a new document
		// and then opening an existing document causing a crash in 
		// CWPhastView::Select() called from CWPhastView::Update the
		// CWPhastView::CurrentProp had probably been deleted
		this->ClearSelection();

		CHDFMirrorFile* pFile = (CHDFMirrorFile*)ar.GetFile();
		ASSERT(pFile->GetHID() > 0);
		if (pFile->GetHID() < 0) return;

		// Open the "/WPhast" group
		//
		hid_t wphast_id = ::H5Gopen(pFile->GetHID(), szWPhast);
		ASSERT(wphast_id > 0);
		if (wphast_id > 0)
		{
			// Note: can't call this->New(...) here since the defaults are unknown
			// until this->SerializeMedia and this->SerializeIC are called

			// load model (flowonly freeSurface steadyFlow)
			ASSERT(this->m_pModel);
			this->m_pModel->m_flowOnly.Serialize(bStoring, wphast_id);
			this->m_pModel->m_freeSurface.Serialize(bStoring, wphast_id);
			this->m_pModel->m_steadyFlow.Serialize(bStoring, wphast_id);
			this->m_pModel->m_solutionMethod.Serialize(bStoring, wphast_id);
			this->m_pModel->m_printInput.Serialize(bStoring, wphast_id);
			this->m_pModel->m_printFreq.Serialize(bStoring, wphast_id);
			this->m_pModel->m_timeControl2.Serialize(bStoring, wphast_id);

			// update properties bar
			this->SetModel(*this->m_pModel);

			// load units
			ASSERT(this->m_pUnits);
			this->m_pUnits->Serialize(bStoring, wphast_id);
			// update properties bar
			if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
			{
				pTree->SetUnits(this->m_pUnits);
			}

			// load grid
			this->m_pGridActor->Serialize(bStoring, wphast_id);

			// set grid
			CGrid x, y, z;
			this->m_pGridActor->GetGrid(x, y, z);
			this->ResizeGrid(x, y, z);

			// load axes
			//this->m_pAxesActor->Serialize(bStoring, wphast_id); // not implemented
			this->GetPropCollection()->AddItem(this->AxesActor);

			// load site map
			ASSERT(this->m_pMapActor == NULL);
			this->m_pMapActor = CMapActor::New();
			this->m_pMapActor->Serialize(bStoring, wphast_id);
			CSiteMap2 siteMap2 = this->m_pMapActor->GetSiteMap2();
			if (siteMap2.FileName.empty())
			{
				this->m_pMapActor->Delete();
				this->m_pMapActor = 0;
			}
			else
			{
				this->m_pMapActor->SetPickable(0);
				this->GetPropCollection()->AddItem(this->m_pMapActor);
			}

			// load display colors
			this->DisplayColors.Serialize(bStoring, wphast_id);
			this->SetDisplayColors(this->DisplayColors);

			// load media
			this->SerializeMedia(bStoring, wphast_id);

			// load ICs
			this->SerializeIC(bStoring, wphast_id);

			// load BCs
			this->SerializeBC(bStoring, wphast_id);

			// load wells
			this->SerializeWells(bStoring, wphast_id);

			// load rivers
			this->SerializeRivers(bStoring, wphast_id);

			// load drains
			this->SerializeDrains(bStoring, wphast_id);

			// load zone flow rates
			this->SerializeZoneFlowRates(bStoring, wphast_id);

			// update properties bar
			if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
			{
				pTree->SetPrintFrequency(&this->m_pModel->m_printFreq);
			}

			// update properties bar
			if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
			{
				pTree->SetTimeControl2(&this->m_pModel->m_timeControl2);
			}

			// close WPhast group
			status = ::H5Gclose(wphast_id);
			ASSERT(status >= 0);
		}

		// set scale for all zones, wells ...
		double* scale = this->m_pGridActor->GetScale();
		this->SetScale(scale[0], scale[1], scale[2]);

		this->ResetCamera();

		// update properties bar
		if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
		{
			pTree->GetGridNode().Expand(TVE_COLLAPSE);
			pTree->GetMediaNode().Expand(TVE_COLLAPSE);
			pTree->GetICNode().Expand(TVE_COLLAPSE);
			pTree->GetBCNode().Expand(TVE_COLLAPSE);
			pTree->GetWellsNode().Expand(TVE_COLLAPSE);
			pTree->GetRiversNode().Expand(TVE_COLLAPSE);
			pTree->GetDrainsNode().Expand(TVE_COLLAPSE);
			pTree->GetZoneFlowRatesNode().Expand(TVE_COLLAPSE);
			this->ClearSelection();
		}
	}

#if defined(__CPPUNIT__)
	pdh_status = PdhCollectQueryData(hquery);
	if (pdh_status == ERROR_SUCCESS)
	{
		PDH_RAW_COUNTER value;
		DWORD dwType;
		pdh_status = PdhGetRawCounterValue(hcounter, &dwType, &value);
		if (pdh_status == ERROR_SUCCESS)
		{
			TRACE("After serialize %lld %lld\n", value.TimeStamp, value.FirstValue);
		}
	}
#endif

	// Update StatusBar
	//
	if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
	{
		CString status;
		status.LoadString(AFX_IDS_IDLEMESSAGE);
		pWnd->SetWindowText(status);
	}
}

void CWPhastDoc::SerializeMedia(bool bStoring, hid_t loc_id)
{
	if (CPropertyTreeControlBar *pBar = this->GetPropertyTreeControlBar())
	{
		this->SerializeActors<CMediaZoneActor>(bStoring, loc_id, pBar->GetMediaNode(), "Zones");
	}
}

void CWPhastDoc::SerializeBC(bool bStoring, hid_t loc_id)
{
	if (CPropertyTreeControlBar *pBar = this->GetPropertyTreeControlBar())
	{
		this->SerializeActors<CBCZoneActor>(bStoring, loc_id, pBar->GetBCNode(), "Zones");
	}
}

void CWPhastDoc::SerializeWells(bool bStoring, hid_t loc_id)
{
	if (CPropertyTreeControlBar *pBar = this->GetPropertyTreeControlBar())
	{
		this->SerializeActors<CWellActor>(bStoring, loc_id, pBar->GetWellsNode(), "Wells");
	}
}

void CWPhastDoc::SerializeRivers(bool bStoring, hid_t loc_id)
{
	if (CPropertyTreeControlBar *pBar = this->GetPropertyTreeControlBar())
	{
		this->SerializeActors<CRiverActor>(bStoring, loc_id, pBar->GetRiversNode(), "Rivers");
	}
}

void CWPhastDoc::SerializeDrains(bool bStoring, hid_t loc_id)
{
	if (CPropertyTreeControlBar *pBar = this->GetPropertyTreeControlBar())
	{
		this->SerializeActors<CDrainActor>(bStoring, loc_id, pBar->GetDrainsNode(), "Drains");
	}
}

void CWPhastDoc::SerializeZoneFlowRates(bool bStoring, hid_t loc_id)
{
	if (CPropertyTreeControlBar *pBar = this->GetPropertyTreeControlBar())
	{
		this->SerializeActors<CZoneFlowRateZoneActor>(bStoring, loc_id, pBar->GetZoneFlowRatesNode(), "Zones");
	}
}

void CWPhastDoc::SerializeIC(bool bStoring, hid_t loc_id)
{
	// Revisions prior to and including r669 stored ALL the IC zones
	// in the IC group.  Now each IC zone type has its own group heading.
	// This code should be backwards compatible for reading of .wphast
	// files.
	//
	// through r669:
	//
	//      GROUP "IC" {
	//         GROUP "Default:IC_CHEM" {
	//            ...
	//         }
	//         GROUP "Default:IC_HEAD" {
	//            ...
	//         }
	//         GROUP "Zone 10:IC_CHEM" {
	//            ...
	//         }
	//         GROUP "Zone 9:IC_HEAD" {
	//            ...
	//         }
	//         DATASET "Zones" {
	//            ...
	//            DATA {
	//               "Default:IC_HEAD", "Default:IC_CHEM", "Zone 9:IC_HEAD",
	//               "Zone 10:IC_CHEM"
	//            }
	//         }
	//      }
	//
	// from now on this is the layout:
	//
	//      GROUP "IC" {
	//         GROUP "ICChem" {
	//            GROUP "Default" {
	//               ...
	//            }
	//            GROUP "Zone 10" {
	//               ...
	//            }
	//            DATASET "Zones" {
	//               ...
	//               DATA {
	//                  "Default", "Zone 10"
	//               }
	//            }
	//         }
	//         GROUP "ICHead" {
	//            GROUP "Default" {
	//               ...
	//            }
	//            GROUP "Zone 9" {
	//               ...
	//            }
	//            DATASET "Zones" {
	//               ...
	//               DATA {
	//                  "Default", "Zone 9"
	//               }
	//            }
	//         }
	//      }
	//

	static const char szIC[]    = "IC";
	static const char szZones[] = "Zones";

	hid_t ic_id;
	hid_t zone_id;
	herr_t status;

	if (bStoring)
	{
		// Create the szIC group
		ic_id = ::H5Gcreate(loc_id, szIC, 0); // always created even if empty
		ASSERT(ic_id > 0);
		if (ic_id > 0)
		{
			// write new formats
			//
			if (CPropertyTreeControlBar *pBar = this->GetPropertyTreeControlBar())
			{
				this->SerializeActors<CICHeadZoneActor>(bStoring, ic_id, pBar->GetICHeadNode(), szZones);
				this->SerializeActors<CICChemZoneActor>(bStoring, ic_id, pBar->GetICChemNode(), szZones);
			}

			// close the szIC group
			status = ::H5Gclose(ic_id);
			ASSERT(status >= 0);
		}
	}
	else
	{
		// add loading code here

		// Open the szIC group
		ic_id = ::H5Gopen(loc_id, szIC);
		ASSERT(ic_id > 0);
		if (ic_id > 0)
		{
			hid_t old_id = ::H5Dopen(ic_id, szZones);
			if (old_id <= 0)
			{
				// read new format
				//
				if (CPropertyTreeControlBar *pBar = this->GetPropertyTreeControlBar())
				{
					this->SerializeActors<CICHeadZoneActor>(bStoring, ic_id, pBar->GetICHeadNode(), szZones);
					this->SerializeActors<CICChemZoneActor>(bStoring, ic_id, pBar->GetICChemNode(), szZones);
				}
			}
			else
			{
				// read old format (this .wphast file was written by a version older than
				// or equal to 669)
				//
				status = ::H5Dclose(old_id);
				ASSERT(status >= 0);

				std::list<std::string> listNames;
				CGlobal::ReadList(ic_id, szZones, listNames);
				std::list<std::string>::iterator iter = listNames.begin();
				for (; iter != listNames.end(); ++iter)
				{
					// open zone group
					zone_id = ::H5Gopen(ic_id, (*iter).c_str());
					ASSERT(zone_id > 0);
					if (zone_id > 0)
					{
						std::string name = (*iter);
						std::string::size_type pos;

						if ((pos = name.find(":IC_HEAD")) != std::string::npos)
						{
							CICHeadZoneActor* pZone = CICHeadZoneActor::New();
							name.resize(pos);
							pZone->Serialize(false, zone_id, this);
							pZone->SetName(name.c_str());
							this->Add(pZone);
							pZone->Delete(); // ok ref counted
						}
						else if ((pos = name.find(":IC_CHEM")) != std::string::npos)
						{
							CICChemZoneActor* pZone = CICChemZoneActor::New();
							name.resize(pos);
							pZone->Serialize(false, zone_id, this);
							pZone->SetName(name.c_str());
							this->Add(pZone);
							pZone->Delete(); // ok ref counted
						}
						else
						{
							ASSERT(FALSE);
						}

						// close the szZones group
						status = ::H5Gclose(zone_id);
						ASSERT(status >= 0);
					}
				}

			}
			// close the szIC group
			status = ::H5Gclose(ic_id);
			ASSERT(status >= 0);
		}
	}
}

template<typename ACTOR>
void CWPhastDoc::SerializeActors(bool bStoring, hid_t loc_id, CTreeCtrlNode parentNode, const char* szNamesListHeading)
{
	hid_t group_id;
	hid_t actor_id;
	herr_t status;

	if (bStoring)
	{
		// STORING

		// create the ACTOR::szHeading group
		group_id = ::H5Gcreate(loc_id, ACTOR::szHeading, 0); // always created even if empty
		ASSERT(group_id > 0);
		if (group_id > 0)
		{
			int nCount;
			std::list<LPCTSTR> listNames;
			std::list<ACTOR*> listActors;

			// get actors and actors names
			//
			nCount = parentNode.GetChildCount();
			for (int i = 0; i < nCount; ++i)
			{
				if (ACTOR *pActor = ACTOR::SafeDownCast((vtkObject*)parentNode.GetChildAt(i).GetData()))
				{
					listActors.push_back(pActor);
					ASSERT(pActor->GetName() && ::strlen(pActor->GetName()));
					listNames.push_back(pActor->GetName());
				}
				else ASSERT(FALSE);
			}

			if (listNames.size() > 0)
			{
				// store actor names
				//
				CGlobal::WriteList(group_id, szNamesListHeading, listNames);

				// store each actor
				//
				std::list<ACTOR*>::iterator iter = listActors.begin();
				for (; iter != listActors.end(); ++iter)
				{
					// create actor group
					//
					actor_id = ::H5Gcreate(group_id, (*iter)->GetName(), 0);
					ASSERT(actor_id > 0);
					if (actor_id > 0)
					{
						// store actor
						//
						(*iter)->Serialize(true, actor_id, this);

						// close the actor group
						//
						status = ::H5Gclose(actor_id);
						ASSERT(status >= 0);
					}
				}
			}

			// close the ACTOR::szHeading group
			//
			status = ::H5Gclose(group_id);
			ASSERT(status >= 0);
		}
	}
	else
	{
		// LOADING

		// open the ACTOR::szHeading group
		//
		group_id = ::H5Gopen(loc_id, ACTOR::szHeading);
		ASSERT(group_id > 0);
		if (group_id > 0)
		{
			std::list<std::string> listNames;
			CGlobal::ReadList(group_id, szNamesListHeading, listNames);
			std::list<std::string>::iterator iter = listNames.begin();
			for (; iter != listNames.end(); ++iter)
			{
				// open actor group
				//
				actor_id = ::H5Gopen(group_id, (*iter).c_str());
				ASSERT(actor_id > 0);
				if (actor_id > 0)
				{
					ACTOR* pActor = ACTOR::New();
					if (pActor)
					{
						// load actor
						//
						pActor->Serialize(false, actor_id, this);
						pActor->SetName((*iter).c_str());
						this->Add(pActor);
						pActor->Delete(); // ok ref counted
					}

					// close the actor group
					//
					status = ::H5Gclose(actor_id);
					ASSERT(status >= 0);
				}
			}

			// close the ACTOR::szHeading group
			//
			status = ::H5Gclose(group_id);
			ASSERT(status >= 0);
		}
	}
}

// CWPhastDoc diagnostics

#ifdef _DEBUG
void CWPhastDoc::AssertValid() const
{
	CDocument::AssertValid();
	ASSERT(this->AxesActor != 0);
	ASSERT(this->m_pimpl != 0);
	ASSERT(this->m_pPropCollection != 0);
	ASSERT(this->m_pRemovedPropCollection != 0);
}

void CWPhastDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

CPropertyTreeControlBar* CWPhastDoc::GetPropertyTreeControlBar() const
{
	CFrameWnd* pFrame = (CFrameWnd*)::AfxGetMainWnd();
	if (!pFrame) return 0;

	if (CPropertyTreeControlBar* pTree = static_cast<CPropertyTreeControlBar*>(pFrame->GetControlBar(IDW_CONTROLBAR_TREE))) {
		ASSERT_KINDOF(CPropertyTreeControlBar, pTree);
		ASSERT_VALID(pTree);
		return pTree;
	}
	///ASSERT(FALSE);
	return 0;
}

CBoxPropertiesDialogBar* CWPhastDoc::GetBoxPropertiesDialogBar() const
{
	CFrameWnd* pFrame = (CFrameWnd*)::AfxGetMainWnd();
	if (!pFrame) return 0;

	if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(pFrame->GetControlBar(IDW_CONTROLBAR_BOXPROPS))) {
		ASSERT_KINDOF(CBoxPropertiesDialogBar, pBar);
		ASSERT_VALID(pBar);
		return pBar;
	}
	ASSERT(FALSE);
	return 0;
}

vtkPropCollection* CWPhastDoc::GetPropCollection() const
{
	return this->m_pPropCollection;
}

vtkPropCollection* CWPhastDoc::GetRemovedPropCollection() const
{
	return this->m_pRemovedPropCollection;
}

// CWPhastDoc commands

BOOL CWPhastDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	// save and restore current directory
	CSaveCurrentDirectory save(lpszPathName);

	char curdir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, curdir);
	TRACE("CWPhastDoc::OnOpenDocument(%s) GetCurrentDirectory = %s\n", lpszPathName, curdir);

	BOOL bOk = CDocument::OnOpenDocument(lpszPathName);
	this->DataSourcePathsRelativeToAbsolute(lpszPathName);

#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
	this->ExecutePipeline();
#endif

	return bOk;
}

CFile* CWPhastDoc::GetFile(LPCTSTR lpszFileName, UINT nOpenFlags,
	CFileException* pError)
{
	/// return CDocument::GetFile(lpszFileName, nOpenFlags, pError);

	CHDFMirrorFile* pFile = new CHDFMirrorFile;
	ASSERT(pFile != NULL);
	if (!pFile->Open(lpszFileName, nOpenFlags, pError))
	{
		delete pFile;
		pFile = NULL;
	}
	return pFile;
}

///////////////////////////////////////////////////////////////////////////////////////
#define CLEAR_PROP_ASSEMBLY_MACRO(PROP_ASSEMBLY_PTR) \
do { \
	ASSERT(PROP_ASSEMBLY_PTR); \
	if (PROP_ASSEMBLY_PTR) { \
		if (vtkPropCollection *pPropCollection = PROP_ASSEMBLY_PTR->GetParts()) { \
			pPropCollection->RemoveAllItems(); \
			ASSERT(pPropCollection->GetNumberOfItems() == 0); \
			PROP_ASSEMBLY_PTR->Modified(); \
		} \
		ASSERT(PROP_ASSEMBLY_PTR->GetNumberOfPaths() == 0); \
	} \
} while(0)
///////////////////////////////////////////////////////////////////////////////////////

void CWPhastDoc::DeleteContents()
{
	// Add your specialized code here and/or call the base class
	TRACE("CWPhastDoc::DeleteContents()\n");

	//{{
	// remove old actors
	if ( vtkPropCollection *props = this->GetPropCollection() )
	{
		POSITION pos =  this->GetFirstViewPosition();
		CWPhastView *pView = (CWPhastView*)this->GetNextView(pos);
		if (pView)
		{
			vtkCollectionSimpleIterator csi;
			props->InitTraversal(csi);
			for (int i = 0; i < props->GetNumberOfItems(); ++i)
			{
				vtkProp* prop = props->GetNextProp(csi);
				if (prop)
				{
					pView->GetRenderer()->RemoveViewProp(prop);
				}
			}
			pView->DeleteContents();
// COMMENT: {9/17/2009 4:12:56 PM}			ASSERT(pView->GetRenderer()->GetViewProps()->GetNumberOfItems() == 0);
			pView->UpdateWindow();
			this->UpdateAllViews(0);
		}
	}
	//}}

	ASSERT(this->AxesActor);

	// reset model
	ASSERT(this->m_pModel);
	delete this->m_pModel;
	this->m_pModel = new CNewModel();
	this->m_pModel->m_printFreq = CPrintFreq::NewDefaults();

	// reset units
	ASSERT(this->m_pUnits);
	delete this->m_pUnits;
	this->m_pUnits = new CUnits();

	// reset colors
	CDisplayColors colors;
	this->SetDisplayColors(colors);

	// clear undo/redo
	//
	ASSERT(this->m_pimpl);
	if (this->m_pimpl)
	{
		// delete all actions
		std::vector<CAction*>::iterator iter = this->m_pimpl->m_vectorActions.begin();
		for (; iter != this->m_pimpl->m_vectorActions.end(); ++iter) {
			delete (*iter);
		}
		this->m_pimpl->m_vectorActions.clear();
		this->m_pimpl->m_vectorActionsIndex = 0;
		this->m_pimpl->m_lastSaveIndex = -1;
	}

	// create the grid
	//
	if (this->m_pGridActor)
	{
		if (this->m_pGridActor->GetEnabled())
		{
			this->m_pGridActor->SetEnabled(0);
		}
		this->m_pGridActor->Delete();
	}
	this->m_pGridActor = CGridActor::New();
	ASSERT(this->m_pGridActor);
	this->m_pGridActor->AddObserver(CGridActor::DeleteGridLineEvent, this->GridCallbackCommand);
	this->m_pGridActor->AddObserver(CGridActor::InsertGridLineEvent, this->GridCallbackCommand);
	this->m_pGridActor->AddObserver(CGridActor::MoveGridLineEvent,   this->GridCallbackCommand);
// COMMENT: {8/9/2005 7:57:14 PM}	this->m_pGridActor->GetProperty()->SetColor(1.0, 0.8, 0.6);
	this->m_pGridActor->SetScale(1, 1, 1);
	this->m_pGridActor->SetPickable(0);

// COMMENT: {9/10/2009 11:22:38 PM}	// update views
// COMMENT: {9/10/2009 11:22:38 PM}	//
// COMMENT: {9/10/2009 11:22:38 PM}	POSITION pos = this->GetFirstViewPosition();
// COMMENT: {9/10/2009 11:22:38 PM}	while (pos != NULL)
// COMMENT: {9/10/2009 11:22:38 PM}	{
// COMMENT: {9/10/2009 11:22:38 PM}		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
// COMMENT: {9/10/2009 11:22:38 PM}		ASSERT_VALID(pView);
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}		ASSERT(this->m_pPropCollection);
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}		if (this->m_pPropCollection)
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}		{
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}			// remove all props
																				///vtkCollectionSimpleIterator csi;
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}			this->m_pPropCollection->InitTraversal(csi);
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}			for (int i = 0; i < this->m_pPropCollection->GetNumberOfItems(); ++i)
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}			{
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}				vtkProp* prop = this->m_pPropCollection->GetNextProp(csi);
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}				pView->GetRenderer()->GetViewProps()->RemoveItem(prop);
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}			}
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}		}
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}		// all vtkProps must release before the Renderer is destroyed
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}		// ie when an import fails
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}		this->m_pGridActor->ReleaseGraphicsResources(pView->GetRenderer()->GetVTKWindow());
// COMMENT: {9/10/2009 11:22:38 PM}// COMMENT: {9/4/2009 7:49:01 PM}		this->m_pAxesActor->ReleaseGraphicsResources(pView->GetRenderer()->GetVTKWindow());
// COMMENT: {9/10/2009 11:22:38 PM}
// COMMENT: {9/10/2009 11:22:38 PM}		pView->DeleteContents();
// COMMENT: {9/10/2009 11:22:38 PM}	}

	// clear prop collection
	//
	ASSERT(this->m_pPropCollection);
	if (this->m_pPropCollection)
	{
		// Note: all CZone are deleted when the CZoneCreateAction<CZone> is deleted
		this->m_pPropCollection->RemoveAllItems();
	}

	// clear removed prop collection
	//
	ASSERT(this->m_pRemovedPropCollection);
	if (this->m_pRemovedPropCollection)
	{
		this->m_pRemovedPropCollection->RemoveAllItems();
	}

	// update tree
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		ASSERT_VALID(pTree);
		pTree->DeleteContents();
	}

	// clear the zone prop-assemblies
	//
	CLEAR_PROP_ASSEMBLY_MACRO(this->m_pPropAssemblyMedia);
	CLEAR_PROP_ASSEMBLY_MACRO(this->m_pPropAssemblyIC);
	CLEAR_PROP_ASSEMBLY_MACRO(this->m_pPropAssemblyBC);
	CLEAR_PROP_ASSEMBLY_MACRO(this->m_pPropAssemblyWells);
	CLEAR_PROP_ASSEMBLY_MACRO(this->m_pPropAssemblyRivers);
	CLEAR_PROP_ASSEMBLY_MACRO(this->m_pPropAssemblyDrains);
	CLEAR_PROP_ASSEMBLY_MACRO(this->m_pPropAssemblyZFR);

	// Turn-all on by default
	//
	this->m_pPropAssemblyMedia->SetVisibility(1);
	this->m_pPropAssemblyIC->SetVisibility(1);
	this->m_pPropAssemblyBC->SetVisibility(1);
	this->m_pPropAssemblyWells->SetVisibility(1);
	this->m_pPropAssemblyRivers->SetVisibility(1);
	this->m_pPropAssemblyDrains->SetVisibility(1);
	this->m_pPropAssemblyZFR->SetVisibility(1);

	// reset scale
	//

// COMMENT: {7/28/2005 4:38:34 PM}	//{{
// COMMENT: {7/28/2005 4:38:34 PM}	// create the grid
// COMMENT: {7/28/2005 4:38:34 PM}	//
// COMMENT: {7/28/2005 4:38:34 PM}	if (this->m_pGridActor)
// COMMENT: {7/28/2005 4:38:34 PM}	{
// COMMENT: {7/28/2005 4:38:34 PM}		this->m_pGridActor->Delete();
// COMMENT: {7/28/2005 4:38:34 PM}	}
// COMMENT: {7/28/2005 4:38:34 PM}	this->m_pGridActor = CGridActor::New();
// COMMENT: {7/28/2005 4:38:34 PM}	this->m_pGridActor->GetProperty()->SetColor(1.0, 0.8, 0.6);
// COMMENT: {7/28/2005 4:38:34 PM}	//}}
// COMMENT: {7/28/2005 4:38:34 PM}	ASSERT(this->m_pGridActor);
// COMMENT: {7/28/2005 4:38:34 PM}	this->m_pGridActor->SetScale(1, 1, 1);

	if (this->m_pMapActor)
	{
		this->m_pMapActor->Delete();
		this->m_pMapActor = 0;
	}
// COMMENT: {8/19/2009 6:09:33 PM}	if (this->MapImageActor3)
// COMMENT: {8/19/2009 6:09:33 PM}	{
// COMMENT: {8/19/2009 6:09:33 PM}		this->MapImageActor3->Delete();
// COMMENT: {8/19/2009 6:09:33 PM}		this->MapImageActor3 = 0;
// COMMENT: {8/19/2009 6:09:33 PM}	}

	// update geometry property sheet
	// Note: can't call this->SetScale(1.0f, 1.0f, 1.0f);
	// Since the this->m_pGridActor contains no data for
	// this->m_pGridActor->GetBounds
	if (this->m_pScalePage)
	{
		this->m_pScalePage->m_XScale = 1;
		this->m_pScalePage->m_YScale = 1;
		this->m_pScalePage->m_ZScale = 1;
		if (this->m_pScalePage->GetSafeHwnd())
		{
			this->m_pScalePage->UpdateData(FALSE);
		}
	}

	// Update BoxPropertiesDialogBar
	//
	if (CBoxPropertiesDialogBar* pBar = this->GetBoxPropertiesDialogBar())
	{
		pBar->Set(0, 0, this->GetUnits());
	}

// COMMENT: {9/8/2009 6:10:37 PM}	this->UpdateAllViews(0);

	// cleanup phastinput data
	//
	Clear_NNInterpolatorList();
	FakeFiledata::Clear_fake_file_data_list();
	Clear_file_data_map();
	Clear_KDtreeList();

	CDocument::DeleteContents();
}

void CWPhastDoc::Execute(CAction* pAction)
{
	pAction->Execute();

	while (this->m_pimpl->m_vectorActionsIndex < this->m_pimpl->m_vectorActions.size()) {
		delete this->m_pimpl->m_vectorActions.back();
		this->m_pimpl->m_vectorActions.pop_back();
	}
	this->m_pimpl->m_vectorActions.push_back(pAction);
	this->m_pimpl->m_vectorActionsIndex = this->m_pimpl->m_vectorActions.size();
	this->SetModifiedFlag(TRUE);
}

void CWPhastDoc::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
// COMMENT: {8/1/2007 1:51:50 PM}	if (CBoxPropertiesDialogBar *pBar = this->GetBoxPropertiesDialogBar())
// COMMENT: {8/1/2007 1:51:50 PM}	{
// COMMENT: {8/1/2007 1:51:50 PM}		if (pBar->IsChild(CWnd::GetFocus()))
// COMMENT: {8/1/2007 1:51:50 PM}		{
// COMMENT: {8/1/2007 1:51:50 PM}			pBar->OnUpdateEditUndo(pCmdUI);
// COMMENT: {8/1/2007 1:51:50 PM}			return;
// COMMENT: {8/1/2007 1:51:50 PM}		}
// COMMENT: {8/1/2007 1:51:50 PM}	}

	BOOL b = (this->m_pimpl->m_vectorActionsIndex > 0);
	b = b && (this->m_pGridSheet->GetSafeHwnd() == 0);
	b = b && (this->m_pGeometrySheet->GetSafeHwnd() == 0);
	pCmdUI->Enable(b);
}

void CWPhastDoc::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
// COMMENT: {8/1/2007 1:51:59 PM}	if (CBoxPropertiesDialogBar *pBar = this->GetBoxPropertiesDialogBar())
// COMMENT: {8/1/2007 1:51:59 PM}	{
// COMMENT: {8/1/2007 1:51:59 PM}		if (pBar->IsChild(CWnd::GetFocus()))
// COMMENT: {8/1/2007 1:51:59 PM}		{
// COMMENT: {8/1/2007 1:51:59 PM}			pBar->OnUpdateEditRedo(pCmdUI);
// COMMENT: {8/1/2007 1:51:59 PM}			return;
// COMMENT: {8/1/2007 1:51:59 PM}		}
// COMMENT: {8/1/2007 1:51:59 PM}	}

	BOOL b = (this->m_pimpl->m_vectorActionsIndex < this->m_pimpl->m_vectorActions.size());
	b = b && (this->m_pGridSheet->GetSafeHwnd() == 0);
	b = b && (this->m_pGeometrySheet->GetSafeHwnd() == 0);
	pCmdUI->Enable(b);
}

void CWPhastDoc::OnEditUndo()
{
// COMMENT: {8/1/2007 1:52:07 PM}	if (CBoxPropertiesDialogBar *pBar = this->GetBoxPropertiesDialogBar())
// COMMENT: {8/1/2007 1:52:07 PM}	{
// COMMENT: {8/1/2007 1:52:07 PM}		if (pBar->IsChild(CWnd::GetFocus()))
// COMMENT: {8/1/2007 1:52:07 PM}		{
// COMMENT: {8/1/2007 1:52:07 PM}			pBar->OnEditUndo();
// COMMENT: {8/1/2007 1:52:07 PM}			return;
// COMMENT: {8/1/2007 1:52:07 PM}		}
// COMMENT: {8/1/2007 1:52:07 PM}	}

	CWaitCursor wait;
	--(this->m_pimpl->m_vectorActionsIndex);
	ASSERT(this->m_pimpl->m_vectorActionsIndex >= 0);
	this->m_pimpl->m_vectorActions.at(this->m_pimpl->m_vectorActionsIndex)->UnExecute();
	this->SetModifiedFlag(this->m_pimpl->m_vectorActionsIndex != this->m_pimpl->m_lastSaveIndex);
}

void CWPhastDoc::OnEditRedo() // 57644
{
// COMMENT: {8/1/2007 1:52:19 PM}	if (CBoxPropertiesDialogBar *pBar = this->GetBoxPropertiesDialogBar())
// COMMENT: {8/1/2007 1:52:19 PM}	{
// COMMENT: {8/1/2007 1:52:19 PM}		if (pBar->IsChild(CWnd::GetFocus()))
// COMMENT: {8/1/2007 1:52:19 PM}		{
// COMMENT: {8/1/2007 1:52:19 PM}			pBar->OnEditRedo();
// COMMENT: {8/1/2007 1:52:19 PM}			return;
// COMMENT: {8/1/2007 1:52:19 PM}		}
// COMMENT: {8/1/2007 1:52:19 PM}	}
	CWaitCursor wait;
	ASSERT(this->m_pimpl->m_vectorActionsIndex < this->m_pimpl->m_vectorActions.size());
	this->m_pimpl->m_vectorActions.at(this->m_pimpl->m_vectorActionsIndex)->Execute();
	++(this->m_pimpl->m_vectorActionsIndex);
	ASSERT(this->m_pimpl->m_vectorActionsIndex != 0);
	this->SetModifiedFlag(this->m_pimpl->m_vectorActionsIndex != this->m_pimpl->m_lastSaveIndex);
}

void CWPhastDoc::SetScale(double x, double y, double z)
{
	ASSERT(x != 0);
	ASSERT(y != 0);
	ASSERT(z != 0);

	double scale[3];
	scale[0] = x;
	scale[1] = y;
	scale[2] = z;

	// Update scale page
	//
	ASSERT(this->m_pScalePage);
	if (this->m_pScalePage)
	{
		this->m_pScalePage->m_XScale = scale[0];
		this->m_pScalePage->m_YScale = scale[1];
		this->m_pScalePage->m_ZScale = scale[2];
		if (this->m_pScalePage->GetSafeHwnd())
		{
			this->m_pScalePage->UpdateData(FALSE);
		}
	}

	// set scale for the grid
	//
	this->m_pGridActor->SetScale(scale);

	// reset the axes
	//
	double bounds[6];
	this->m_pGridActor->GetBounds(bounds);
	float defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
	this->AxesActor->SetDefaultPositions(bounds);
	this->AxesActor->SetDefaultSize(defaultAxesSize);
	this->AxesActor->SetDefaultTubeDiameter(defaultAxesSize * 0.1);

	// set scale for the map
	//
	if (this->m_pMapActor)
	{
		this->m_pMapActor->SetScale(x, y, z);
	}
// COMMENT: {8/19/2009 6:09:47 PM}	//{{
// COMMENT: {8/19/2009 6:09:47 PM}	if (this->MapImageActor3)
// COMMENT: {8/19/2009 6:09:47 PM}	{
// COMMENT: {8/19/2009 6:09:47 PM}		this->MapImageActor3->SetScale(x, y, z);
// COMMENT: {8/19/2009 6:09:47 PM}	}
// COMMENT: {8/19/2009 6:09:47 PM}	//}}

	// set scale for all zones
	//
	if (vtkPropCollection* pCollection = this->GetPropCollection())
	{
		vtkCollectionSimpleIterator csi;
		pCollection->InitTraversal(csi);
		for (int i = 0; i < pCollection->GetNumberOfItems(); ++i)
		{
			vtkProp* prop = pCollection->GetNextProp(csi);
			if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop))
			{
				pZone->SetScale(scale);
			}
			if (vtkPropAssembly *pPropAssembly = vtkPropAssembly::SafeDownCast(prop))
			{
				if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
				{
					vtkProp *pProp;
					vtkCollectionSimpleIterator csi2;
					pPropCollection->InitTraversal(csi2);
					for (; (pProp = pPropCollection->GetNextProp(csi2)); )
					{
						if (vtkProp3D *prop3D = vtkProp3D::SafeDownCast(pProp))
						{
							prop3D->SetScale(scale);
						}
					}
				}
			}
		}
	}

	// if modifying grid update GridElementsSelector
	//
	if (this->GridElementsSelector)
	{
		int ibounds[6];
		this->GridElementsSelector->GetIBounds(ibounds);
		this->GridElementsSelector->SetIBounds(ibounds);
	}

	this->Notify(0, WPN_SCALE_CHANGED, 0, 0);
// COMMENT: {5/8/2006 4:32:55 PM}	this->UpdateAllViews(0);
}

void CWPhastDoc::SetFlowOnly(const CFlowOnly& flowOnly)
{
	this->m_pModel->m_flowOnly = flowOnly;
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetFlowOnly(&this->m_pModel->m_flowOnly);
	}
}

CFlowOnly CWPhastDoc::GetFlowOnly(void)const
{
	return this->m_pModel->m_flowOnly;
}

void CWPhastDoc::SetSteadyFlow(const CSteadyFlow& steadyFlow)
{
	this->m_pModel->m_steadyFlow = steadyFlow;
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetSteadyFlow(&this->m_pModel->m_steadyFlow);
	}
}

CSteadyFlow CWPhastDoc::GetSteadyFlow(void)const
{
	return this->m_pModel->m_steadyFlow;
}

void CWPhastDoc::SetFreeSurface(const CFreeSurface &freeSurface)
{
	this->m_pModel->m_freeSurface = freeSurface;
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetFreeSurface(&this->m_pModel->m_freeSurface);
	}
}

CFreeSurface CWPhastDoc::GetFreeSurface(void)const
{
	return this->m_pModel->m_freeSurface;
}

CNewModel* CWPhastDoc::GetModel(void)
{
	return this->m_pModel;
}

void CWPhastDoc::SetModel(const CNewModel& model)
{
	ASSERT(this->m_pModel);
	(*this->m_pModel) = model;
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetModel(this->m_pModel);
	}
}

void CWPhastDoc::ResizeGrid(const CGrid& x, const CGrid&  y, const CGrid&  z)
{
	ASSERT(this->m_pGridActor);
	ASSERT(this->AxesActor);
	ASSERT(this->m_pPropCollection);
	ASSERT(this->m_pimpl);
	ASSERT(this->m_pUnits);
	ASSERT(this->m_pModel);

	// reset the grid
	//
	this->m_pGridActor->SetGrid(x, y, z, *this->m_pUnits);
	this->m_pGridActor->SetPickable(0);
	this->m_pPropCollection->AddItem(this->m_pGridActor);
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetGridActor(this->m_pGridActor);
	}

	// Update default zones etc.
	//
	this->UpdateGridDomain();
}

void CWPhastDoc::ResizeGrid(const CGridKeyword& keyword)
{
	ASSERT(this->m_pGridActor);
	ASSERT(this->AxesActor);
	ASSERT(this->m_pPropCollection);
	ASSERT(this->m_pimpl);
	ASSERT(this->m_pUnits);
	ASSERT(this->m_pModel);

	if (this->m_pMapActor)
	{
		CSiteMap2 siteMap2 = this->m_pMapActor->GetSiteMap2();
		siteMap2.Angle = keyword.m_grid_angle;
		this->m_pMapActor->SetSiteMap2(siteMap2);
	}

	// reset the grid
	//
	this->m_pGridActor->SetGridKeyword(keyword, this->GetUnits());
	this->m_pGridActor->SetPickable(0);
	this->m_pPropCollection->AddItem(this->m_pGridActor);
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetGridActor(this->m_pGridActor);
	}

// COMMENT: {9/7/2010 10:18:15 PM}	// Update default zones etc.
// COMMENT: {9/7/2010 10:18:15 PM}	//
// COMMENT: {9/7/2010 10:18:15 PM}	this->UpdateGridDomain();

	//{{
	// update possible selection
	//
	this->Notify(this, WPN_DOMAIN_CHANGED, 0, 0);

// COMMENT: {9/7/2010 10:19:23 PM}	// refresh screen
// COMMENT: {9/7/2010 10:19:23 PM}	//
// COMMENT: {9/7/2010 10:19:23 PM}	this->UpdateAllViews(0);
	//}}
}

void CWPhastDoc::AddDefaultZone(CZone* pZone)
{
	// TODO : Is this still used?
	if (pZone)
	{
		this->m_pimpl->m_vectorDefaultZones.push_back(pZone);
	}
}

double* CWPhastDoc::GetScale()
{
	return this->m_pGridActor->GetScale();
}

void CWPhastDoc::GetScale(double data[3])
{
	this->m_pGridActor->GetScale(data);
}

double* CWPhastDoc::GetGridBounds()
{
	if (this->m_pGridActor)
	{
		return this->m_pGridActor->GetBounds();
	}
	ASSERT(FALSE);
	return 0;
}

void CWPhastDoc::OnUpdateToolsGeometry(CCmdUI *pCmdUI)
{
	if (this->m_pGeometrySheet->GetSafeHwnd())
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CWPhastDoc::OnToolsGeometry()
{
	if (this->m_pGeometrySheet->GetSafeHwnd())
	{
		this->m_pGeometrySheet->DestroyWindow();
	}
	else
	{
		// this allows main window to cover the propertysheet but
		// also causes the propertysheet to display on the taskbar
		///this->m_pGeometrySheet->Create();

		// this forces the propertysheet to always be on top of
		// the main window but the propertysheet doesn't show up
		// on the taskbar
		this->m_pGeometrySheet->Create(::AfxGetApp()->m_pMainWnd);
	}
}

void CWPhastDoc::ResetCamera(void)
{
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) this->GetNextView(pos);
		ASSERT_VALID(pView);
		if (pView->GetRenderer())
		{
			pView->GetRenderer()->ResetCamera();
			pView->GetRenderer()->ResetCameraClippingRange();
		}
	}
}

void CWPhastDoc::ResetCamera(double bounds[6])
{
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) this->GetNextView(pos);
		ASSERT_VALID(pView);
		if (pView->GetRenderer())
		{
			pView->GetRenderer()->ResetCamera(bounds);
			pView->GetRenderer()->ResetCameraClippingRange();
		}
	}
}

void CWPhastDoc::ResetCamera(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
	double bounds[6];

	bounds[0] = xmin;
	bounds[1] = xmax;
	bounds[2] = ymin;
	bounds[3] = ymax;
	bounds[4] = zmin;
	bounds[5] = zmax;

	this->ResetCamera(bounds);
}

CString CWPhastDoc::GetNextZoneName(void)
{
	/*static*/ CString str;
	str.Format(szZoneFormat, this->GetNextZoneNumber());
	return str;
}

CString CWPhastDoc::GetNextWedgeName(void)
{
	/*static*/ CString str;
	str.Format(szWedgeFormat, this->GetNextWedgeNumber());
	return str;
}

CString CWPhastDoc::GetNextPrismName(void)
{
	/*static*/ CString str;
	str.Format(szPrismFormat, this->GetNextPrismNumber());
	return str;
}

int CWPhastDoc::GetNextWellNumber(void)
{
	std::set<int> wellNums;
	this->GetUsedWellNumbers(wellNums);
	if (wellNums.rbegin() != wellNums.rend())
	{
		return (*wellNums.rbegin()) + 1;
	}
	else
	{
		return 1;
	}
}

void CWPhastDoc::GetUsedWellNumbers(std::set<int>& usedNums)
{
	usedNums.clear();
	if (vtkPropCollection *pPropCollection = this->GetPropAssemblyWells()->GetParts())
	{
		vtkProp *pProp = 0;
		vtkCollectionSimpleIterator csi;
		pPropCollection->InitTraversal(csi);
		for (; (pProp = pPropCollection->GetNextProp(csi)); )
		{
			if (CWellActor *pWellActor = CWellActor::SafeDownCast(pProp))
			{
				// store used n_user numbers
				//
				std::pair< std::set<int>::iterator, bool > pr;
				pr = usedNums.insert( pWellActor->GetWell().n_user );
				ASSERT(pr.second); // duplicate?
			}
		}
	}
}

int CWPhastDoc::GetNextRiverNumber(void)
{
	std::set<int> riverNums;
	this->GetUsedRiverNumbers(riverNums);
	if (riverNums.rbegin() != riverNums.rend())
	{
		return (*riverNums.rbegin()) + 1;
	}
	else
	{
		return 1;
	}
}

void CWPhastDoc::GetUsedRiverNumbers(std::set<int>& usedNums)
{
	usedNums.clear();
	if (vtkPropCollection *pPropCollection = this->GetPropAssemblyRivers()->GetParts())
	{
		vtkProp *pProp = 0;
		vtkCollectionSimpleIterator csi;
		pPropCollection->InitTraversal(csi);
		for (; (pProp = pPropCollection->GetNextProp(csi)); )
		{
			if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(pProp))
			{
				// store used n_user numbers
				//
				std::pair< std::set<int>::iterator, bool > pr;
				pr = usedNums.insert( pRiverActor->GetRiver().n_user );
				ASSERT(pr.second); // duplicate?
			}
		}
	}
}

int CWPhastDoc::GetNextZoneNumber(void)const
{
	std::set<int> zoneNums;
	this->GetUsedZoneNumbers(zoneNums);
	if (zoneNums.rbegin() != zoneNums.rend())
	{
		return (*zoneNums.rbegin()) + 1;
	}
	else
	{
		return 1;
	}
}

int CWPhastDoc::GetNextWedgeNumber(void)const
{
	std::set<int> wedgeNums;
	this->GetUsedWedgeNumbers(wedgeNums);
	if (wedgeNums.rbegin() != wedgeNums.rend())
	{
		return (*wedgeNums.rbegin()) + 1;
	}
	else
	{
		return 1;
	}
}

int CWPhastDoc::GetNextPrismNumber(void)const
{
	std::set<int> prismNums;
	this->GetUsedPrismNumbers(prismNums);
	if (prismNums.rbegin() != prismNums.rend())
	{
		return (*prismNums.rbegin()) + 1;
	}
	else
	{
		return 1;
	}
}

int CWPhastDoc::GetNextZoneFlowRatesNumber(void)const
{
	std::set<int> nums;
	this->GetUsedZoneFlowRates(nums);
	if (nums.rbegin() != nums.rend())
	{
		return (*nums.rbegin()) + 1;
	}
	else
	{
		return 1;
	}
}


void CWPhastDoc::GetUsedZoneNumbers(std::set<int>& usedNums)const
{
	char *ptr;
	CZoneActor *pZoneActor;
	usedNums.clear();

	std::list<vtkPropCollection*> collections;
	collections.push_back(this->GetPropAssemblyMedia()->GetParts());
	collections.push_back(this->GetPropAssemblyBC()->GetParts());
	collections.push_back(this->GetPropAssemblyIC()->GetParts());
	collections.push_back(this->GetPropAssemblyZoneFlowRates()->GetParts());

	std::list<vtkPropCollection*>::iterator iter = collections.begin();
	for (; iter != collections.end(); ++iter)
	{
		if (vtkPropCollection *pPropCollection = (*iter))
		{
			vtkProp *pProp = 0;
			vtkCollectionSimpleIterator csi;
			pPropCollection->InitTraversal(csi);
			for (; (pProp = pPropCollection->GetNextProp(csi)); )
			{
				if ((pZoneActor = CZoneActor::SafeDownCast(pProp)) && (pZoneActor->GetChopType() == srcWedgeSource::CHOP_NONE))
				{
					// store used n_user numbers
					//
					std::pair< std::set<int>::iterator, bool > pr;
					CString str = pZoneActor->GetName();
					if (str.Find(szZoneFind) == 0)
					{
						int n = ::strtol((const char*)str + ::strlen(szZoneFind), &ptr, 10);
						pr = usedNums.insert(n);
						ASSERT(pr.second); // duplicate?
					}
				}
			}
		}
	}
}

void CWPhastDoc::GetUsedWedgeNumbers(std::set<int>& usedNums)const
{
	char *ptr;
	CZoneActor *pZoneActor;
	usedNums.clear();

	std::list<vtkPropCollection*> collections;
	collections.push_back(this->GetPropAssemblyMedia()->GetParts());
	collections.push_back(this->GetPropAssemblyBC()->GetParts());
	collections.push_back(this->GetPropAssemblyIC()->GetParts());
	collections.push_back(this->GetPropAssemblyZoneFlowRates()->GetParts());

	std::list<vtkPropCollection*>::iterator iter = collections.begin();
	for (; iter != collections.end(); ++iter)
	{
		if (vtkPropCollection *pPropCollection = (*iter))
		{
			vtkProp *pProp = 0;
			vtkCollectionSimpleIterator csi;
			pPropCollection->InitTraversal(csi);
			for (; (pProp = pPropCollection->GetNextProp(csi)); )
			{
				if ((pZoneActor = CZoneActor::SafeDownCast(pProp)) && (pZoneActor->GetChopType() != srcWedgeSource::CHOP_NONE))
				{
					// store used n_user numbers
					//
					std::pair< std::set<int>::iterator, bool > pr;
					CString str = pZoneActor->GetName();
					if (str.Find(szWedgeFind) == 0) {
						int n = ::strtol((const char*)str + 5, &ptr, 10);
						pr = usedNums.insert(n);
						ASSERT(pr.second); // duplicate?
					}
				}
			}
		}
	}
}

void CWPhastDoc::GetUsedPrismNumbers(std::set<int>& usedNums)const
{
	char *ptr;
	CZoneActor *pZoneActor;
	usedNums.clear();

	std::list<vtkPropCollection*> collections;
	collections.push_back(this->GetPropAssemblyMedia()->GetParts());
	collections.push_back(this->GetPropAssemblyBC()->GetParts());
	collections.push_back(this->GetPropAssemblyIC()->GetParts());
	collections.push_back(this->GetPropAssemblyZoneFlowRates()->GetParts());

	std::list<vtkPropCollection*>::iterator iter = collections.begin();
	for (; iter != collections.end(); ++iter)
	{
		if (vtkPropCollection *pPropCollection = (*iter))
		{
			vtkProp *pProp = 0;
			vtkCollectionSimpleIterator csi;
			pPropCollection->InitTraversal(csi);
			for (; (pProp = pPropCollection->GetNextProp(csi)); )
			{
				if ((pZoneActor = CZoneActor::SafeDownCast(pProp)))
				{
					if (pZoneActor->GetPolyhedronType() == Polyhedron::PRISM)
					{
						// store used n_user numbers
						//
						std::pair< std::set<int>::iterator, bool > pr;
						CString str = pZoneActor->GetName();
						if (str.Find(szPrismFind) == 0)
						{
							int n = ::strtol((const char*)str + 5, &ptr, 10);
							pr = usedNums.insert(n);
							ASSERT(pr.second); // duplicate?
						}
					}
				}
			}
		}
	}
}

void CWPhastDoc::GetUsedZoneFlowRates(std::set<int>& usedNums)const
{
	usedNums.clear();
	if (vtkPropCollection *pPropCollection = this->GetPropAssemblyZoneFlowRates()->GetParts())
	{
		vtkProp *pProp = 0;
		vtkCollectionSimpleIterator csi;
		pPropCollection->InitTraversal(csi);
		for (; (pProp = pPropCollection->GetNextProp(csi)); )
		{
			if (CZoneFlowRateZoneActor *pActor = CZoneFlowRateZoneActor::SafeDownCast(pProp))
			{
				// store used n_user numbers
				//
				std::pair< std::set<int>::iterator, bool > pr;
				pr = usedNums.insert( pActor->GetData().Get_n_user() );
				ASSERT(pr.second); // duplicate?
			}
		}
	}
}

void CWPhastDoc::DataSourcePathsRelativeToAbsolute(LPCTSTR lpszPathName)
{
	CZoneActor *pZoneActor;

	std::list<vtkPropCollection*> collections;
	collections.push_back(this->GetPropAssemblyMedia()->GetParts());
	collections.push_back(this->GetPropAssemblyBC()->GetParts());
	collections.push_back(this->GetPropAssemblyIC()->GetParts());
	collections.push_back(this->GetPropAssemblyZoneFlowRates()->GetParts());

	std::list<vtkPropCollection*>::iterator iter = collections.begin();
	for (; iter != collections.end(); ++iter)
	{
		if (vtkPropCollection *pPropCollection = (*iter))
		{
			vtkProp *pProp = 0;
			vtkCollectionSimpleIterator csi;
			pPropCollection->InitTraversal(csi);
			for (; (pProp = pPropCollection->GetNextProp(csi)); )
			{
				if ((pZoneActor = CZoneActor::SafeDownCast(pProp)))
				{
					if (CMediaZoneActor *pMediaZoneActor = CMediaZoneActor::SafeDownCast(pZoneActor))
					{
						CGridElt elt = pMediaZoneActor->GetData();
						CGlobal::PathsRelativeToAbsolute(lpszPathName, this, elt);
						pMediaZoneActor->SetData(elt);
					}
					if (CBCZoneActor *pBCZoneActor = CBCZoneActor::SafeDownCast(pZoneActor))
					{
						CBC bc = pBCZoneActor->GetData();
						CGlobal::PathsRelativeToAbsolute(lpszPathName, this, bc);
						pBCZoneActor->SetData(bc);
					}
					if (CICChemZoneActor *pICChemZoneActor = CICChemZoneActor::SafeDownCast(pZoneActor))
					{
						CChemIC chemIC = pICChemZoneActor->GetData();
						CGlobal::PathsRelativeToAbsolute(lpszPathName, this, chemIC);
						pICChemZoneActor->SetData(chemIC);
					}
					if (CICHeadZoneActor *pICHeadZoneActor = CICHeadZoneActor::SafeDownCast(pZoneActor))
					{
						CHeadIC headIC = pICHeadZoneActor->GetData();
						CGlobal::PathsAbsoluteToRelative(lpszPathName, this, headIC);
						pICHeadZoneActor->SetData(headIC);
					}
					if (pZoneActor->GetPolyhedronType() == Polyhedron::PRISM)
					{
						if (Prism *p = dynamic_cast<Prism*>(pZoneActor->GetPolyhedron()))
						{
							std::string filename;
							filename = p->perimeter.Get_file_name();
							if (filename.length() > 0)
							{
								p->perimeter.Set_file_name(this->GetAbsolutePath(lpszPathName, filename));
							}
							filename = p->top.Get_file_name();
							if (filename.length() > 0)
							{
								p->top.Set_file_name(this->GetAbsolutePath(lpszPathName, filename));
							}
							filename = p->bottom.Get_file_name();
							if (filename.length() > 0)
							{
								p->bottom.Set_file_name(this->GetAbsolutePath(lpszPathName, filename));
							}
						}
					}
				}
			}
		}
	}
}

void CWPhastDoc::DataSourcePathsAbsoluteToRelative(LPCTSTR lpszPathName)
{
	CZoneActor *pZoneActor;

	std::list<vtkPropCollection*> collections;
	collections.push_back(this->GetPropAssemblyMedia()->GetParts());
	collections.push_back(this->GetPropAssemblyBC()->GetParts());
	collections.push_back(this->GetPropAssemblyIC()->GetParts());
	collections.push_back(this->GetPropAssemblyZoneFlowRates()->GetParts());

	std::list<vtkPropCollection*>::iterator iter = collections.begin();
	for (; iter != collections.end(); ++iter)
	{
		if (vtkPropCollection *pPropCollection = (*iter))
		{
			vtkProp *pProp = 0;
			vtkCollectionSimpleIterator csi;
			pPropCollection->InitTraversal(csi);
			for (; (pProp = pPropCollection->GetNextProp(csi)); )
			{
				if ((pZoneActor = CZoneActor::SafeDownCast(pProp)))
				{
					if (CMediaZoneActor *pMediaZoneActor = CMediaZoneActor::SafeDownCast(pZoneActor))
					{
						CGridElt elt = pMediaZoneActor->GetData();
						CGlobal::PathsAbsoluteToRelative(lpszPathName, this, elt);

						CTreeCtrl *pTreeCtrl = this->GetPropertyTreeControlBar()->GetTreeCtrl();
						CAction *pAction = new CSetMediaAction(pMediaZoneActor, pTreeCtrl, elt, pMediaZoneActor->GetDesc());
						pAction->Execute();
						delete pAction;
					}
					if (CBCZoneActor *pBCZoneActor = CBCZoneActor::SafeDownCast(pZoneActor))
					{
						CBC bc = pBCZoneActor->GetData();
						CGlobal::PathsAbsoluteToRelative(lpszPathName, this, bc);

						CTreeCtrl *pTreeCtrl = this->GetPropertyTreeControlBar()->GetTreeCtrl();
						CAction *pAction = new CSetBCAction(pBCZoneActor, pTreeCtrl, bc, pBCZoneActor->GetDesc());
						pAction->Execute();
						delete pAction;
					}
					if (CICChemZoneActor *pICChemZoneActor = CICChemZoneActor::SafeDownCast(pZoneActor))
					{
						CChemIC chemIC = pICChemZoneActor->GetData();
						CGlobal::PathsAbsoluteToRelative(lpszPathName, this, chemIC);

						CTreeCtrl *pTreeCtrl = this->GetPropertyTreeControlBar()->GetTreeCtrl();
						CAction *pAction = new CSetChemICAction(pICChemZoneActor, pTreeCtrl, chemIC, pICChemZoneActor->GetDesc());
						pAction->Execute();
						delete pAction;
					}
					if (CICHeadZoneActor *pICHeadZoneActor = CICHeadZoneActor::SafeDownCast(pZoneActor))
					{
						CHeadIC headIC = pICHeadZoneActor->GetData();
						CGlobal::PathsAbsoluteToRelative(lpszPathName, this, headIC);

						CTreeCtrl *pTreeCtrl = this->GetPropertyTreeControlBar()->GetTreeCtrl();
						CAction *pAction = new CSetHeadICAction(pICHeadZoneActor, pTreeCtrl, headIC, pICHeadZoneActor->GetDesc());
						pAction->Execute();
						delete pAction;
					}
					if (pZoneActor->GetPolyhedronType() == Polyhedron::PRISM)
					{
						if (Prism *p = dynamic_cast<Prism*>(pZoneActor->GetPolyhedron()))
						{
							std::string filename;
							filename = p->perimeter.Get_file_name();
							if (filename.length() > 0)
							{
								p->perimeter.Set_file_name(this->GetRelativePath(lpszPathName, filename));
							}
							filename = p->top.Get_file_name();
							if (filename.length() > 0)
							{
								p->top.Set_file_name(this->GetRelativePath(lpszPathName, filename));
							}
							filename = p->bottom.Get_file_name();
							if (filename.length() > 0)
							{
								p->bottom.Set_file_name(this->GetRelativePath(lpszPathName, filename));
							}
						}
					}
				}
			}
		}
	}
}

std::string CWPhastDoc::GetRelativePath(LPCTSTR lpszPathName, const std::string src_path)const
{
	TCHAR szOut[MAX_PATH] = "";
	if (::PathIsSameRoot(lpszPathName, src_path.c_str()))
	{
		VERIFY(::PathRelativePathTo(szOut, lpszPathName, FILE_ATTRIBUTE_NORMAL, src_path.c_str(), FILE_ATTRIBUTE_NORMAL));
		return std::string(szOut);
	}
	return src_path;
}

std::string CWPhastDoc::GetAbsolutePath(LPCTSTR lpszPathName, const std::string relative_path)const
{
	if (::PathIsRelative(relative_path.c_str()))
	{
		TCHAR szPath[MAX_PATH];
		TCHAR szOut[MAX_PATH];
		if (::PathIsFileSpec(lpszPathName))
		{
			VERIFY(::GetCurrentDirectory(MAX_PATH, szPath));
			VERIFY(::PathAddBackslash(szPath));
		}
		else
		{
			TCHAR szDrive[_MAX_DRIVE];
			TCHAR szDir[_MAX_DIR];
			TCHAR szFName[_MAX_FNAME];
			TCHAR szExt[_MAX_EXT];
			VERIFY(::_tsplitpath_s(lpszPathName, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);
			VERIFY(::_tmakepath_s(szPath, _MAX_DIR, szDrive, szDir, NULL, NULL) == 0);
			ASSERT(::strlen(szPath));
		}
		VERIFY(::PathAppend(szPath, relative_path.c_str()));
		VERIFY(::PathCanonicalize(szOut, szPath));
		return std::string(szOut);
	}
	return relative_path;
}

BOOL CWPhastDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// Add your specialized code here and/or call the base class
	this->m_pimpl->m_lastSaveIndex = this->m_pimpl->m_vectorActionsIndex;

	this->DataSourcePathsAbsoluteToRelative(lpszPathName);
	BOOL bOk = CDocument::OnSaveDocument(lpszPathName);
	this->DataSourcePathsRelativeToAbsolute(lpszPathName);
	return bOk;
}

void CWPhastDoc::OnCloseDocument()
{
	// Add your specialized code here and/or call the base class

	// must destroy all modeless dialogs here
	// otherwise GetPropertyTreeControlBar will cause
	// an ASSERT when called from CWPhastDoc::DeleteContents
	// which is called by CDocument::OnCloseDocument

	ASSERT(this->m_pScalePage);
	this->m_pScalePage->DestroyWindow();
	delete this->m_pScalePage;
	this->m_pScalePage = 0;

	ASSERT(this->m_pGeometrySheet);
	this->m_pGeometrySheet->DestroyWindow();
	delete this->m_pGeometrySheet;
	this->m_pGeometrySheet = 0;

	//{{HACK
	// this is used to avoid a bug that occurs when a box zone is converted
	// to a prism and the app is closed causing a vtk referrence crash
	if (this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode parentNode = this->GetPropertyTreeControlBar()->GetMediaNode();
		if (parentNode)
		{
			int nCount = parentNode.GetChildCount();
			for (int i = 0; i < nCount; ++i)
			{
				if (CZoneActor *pActor = CZoneActor::SafeDownCast((vtkObject*)parentNode.GetChildAt(i).GetData()))
				{
					if (pActor->GetPolyhedronType() == Polyhedron::PRISM)
					{
						struct zone z;
						Cube c(&z);
						CUnits u;
						pActor->SetPolyhedron(&c, u);
					}
				}
			}
		}
	}
	//}}HACK

	CDocument::OnCloseDocument();
}

void CWPhastDoc::OnFileImport()
{
	CString newName;
	if (!DoPromptFileName(newName, IDS_IMPORT_PHAST_TRANS_133, //AFX_IDS_OPENFILE,
	  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE))
		return; // open cancelled

	// modified from: CDocument* CDocManager::OpenDocumentFile
	//
	TCHAR szPath[_MAX_PATH];
	ASSERT(lstrlen(newName) < sizeof(szPath)/sizeof(szPath[0]));
	TCHAR szTemp[_MAX_PATH];
	if (newName[0] == '\"')
		newName = newName.Mid(1);
	lstrcpyn(szTemp, newName, _MAX_PATH);
	LPTSTR lpszLast = _tcsrchr(szTemp, '\"');
	if (lpszLast != NULL)
		*lpszLast = 0;
	SrcFullPath(szPath, szTemp);
	TCHAR szLinkName[_MAX_PATH];
	if (SrcResolveShortcut(AfxGetMainWnd(), szPath, szLinkName, _MAX_PATH))
		lstrcpy(szPath, szLinkName);

#if defined(__CPPUNIT__)
	HQUERY hquery;
	PDH_STATUS status = PdhOpenQuery(NULL, 0, &hquery);
	VERIFY(status == ERROR_SUCCESS);

	PDH_HCOUNTER hcounter;
	///status = PdhAddCounter(hquery, _T("\\\\.\\Process(WPhast)\\Private Bytes"), 0, &hcounter);
	status = PdhAddCounter(hquery, _T("\\Process(WPhast)\\Private Bytes"), 0, &hcounter);
	if (status != ERROR_SUCCESS)
	{
		PdhCloseQuery(hquery);
	}

	status = PdhCollectQueryData(hquery);
	if (status == ERROR_SUCCESS)
	{
		PDH_RAW_COUNTER value;
		DWORD dwType;
		status = PdhGetRawCounterValue(hcounter, &dwType, &value);
		if (status == ERROR_SUCCESS)
		{
			TRACE("Before import %lld %lld\n", value.TimeStamp, value.FirstValue);
		}
	}
#endif

	CDocument* pDoc = this->GetDocTemplate()->OpenDocumentFile(NULL);
	if (pDoc)
	{
		ASSERT(pDoc == this);
// COMMENT: {6/2/2008 5:43:39 PM}		CDelayRedraw delay(::AfxGetMainWnd(), pDoc); // this breaks CWaitCursor
		CDelayRedraw del1(this->GetPropertyTreeControlBar(), pDoc);
		POSITION pos = this->GetFirstViewPosition();
		CDelayRedraw del2(GetNextView(pos));
		CDelayRedraw del3(this->GetBoxPropertiesDialogBar());
		if (!this->DoImport(szPath))
		{
			::AfxMessageBox("An error occured during the import", MB_OK);
			this->SetModifiedFlag(FALSE);
		}
// COMMENT: {9/10/2009 10:39:36 PM}		this->ResetCamera();
		this->ExecutePipeline();
	}
	if (CPropertyTreeControlBar *pPropertyTreeControlBar = this->GetPropertyTreeControlBar())
	{
		if (CWnd *pTree = pPropertyTreeControlBar->GetTreeCtrl())
		{
			pTree->RedrawWindow();
		}
	}
#if defined(__CPPUNIT__)
	status = PdhCollectQueryData(hquery);
	if (status == ERROR_SUCCESS)
	{
		PDH_RAW_COUNTER value;
		DWORD dwType;
		status = PdhGetRawCounterValue(hcounter, &dwType, &value);
		if (status == ERROR_SUCCESS)
		{
			TRACE("After import %lld %lld\n", value.TimeStamp, value.FirstValue);
		}
	}
#endif
}

BOOL CWPhastDoc::DoImport(LPCTSTR lpszPathName)
{
	// Return Value
	// Nonzero if the file was successfully loaded; otherwise 0.
	CWaitCursor wait;
	CSaveCurrentDirectory save;
	// Update StatusBar
	//
	this->UpdateAllViews(NULL);
	if (::AfxGetMainWnd())
	{
		if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
		{
			CString status(_T("Importing..."));
			pWnd->SetWindowText(status);
		}
	}
	//::Sleep(100);
	this->UpdateAllViews(NULL);

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];

	::_tsplitpath(lpszPathName, szDrive, szDir, szFName, szExt);

	// change directory in case there's a sitemap
	//
	TCHAR szNewDir[MAX_PATH];
	::_tmakepath(szNewDir, szDrive, szDir, NULL, NULL);
	BOOL b = save.SetCurrentDirectory(szNewDir);

	// remove .trans if it exists
	//
	CString strPrefix(szFName);
	strPrefix.MakeLower();
	strPrefix.Replace(_T(".trans"), _T(""));

	// get title
	//
	TCHAR szTitle[MAX_PATH];
	::_tmakepath(szTitle, NULL, NULL, szFName, szExt);

	// get fullpath
	//
	TCHAR szFullpath[MAX_PATH];
	::_tfullpath(szFullpath, lpszPathName, MAX_PATH);

	std::ifstream ifs;
	ifs.open(szTitle);
	if (!ifs.is_open())
	{
		return FALSE;
	}

	// xml
	//
	std::stringbuf buf;
	std::iostream xml(&buf);
	CGlobal::ExtractXMLStream(ifs, xml);
	ifs.seekg(0, std::ios_base::beg);
	xml.seekg(0, std::ios_base::beg);

	ASSERT(Filedata::file_data_map.size() == 0);
	CPhastInput* pInput = CPhastInput::New(ifs, strPrefix, false);
	if (!pInput) return FALSE;

	// save
	std::map<const struct grid_elt*, Polyhedron*> grid_elt_map;
	std::map<const struct BC*,       Polyhedron*> bc_map;
	std::map<const Zone_budget*,     Polyhedron*> zb_map;
	std::map<const struct Head_ic*,  Polyhedron*> head_ic_map;
	std::map<const struct chem_ic*,  Polyhedron*> chem_ic_map;

	BOOL bReturnValue = TRUE;
	try
	{
		CPrintFreq printFreq;
		printFreq.InitSync(pInput); // must be called before Load()

		::OutputDebugString("Begin loading trans.dat file\n");

		pInput->Read();

		// store original polyhs

		// MEDIA zones
		for (int i = 0; i < ::count_grid_elt_zones; ++i)
		{
			const struct grid_elt* grid_elt_ptr = ::grid_elt_zones[i];
			grid_elt_map[grid_elt_ptr] = grid_elt_ptr->polyh ? grid_elt_ptr->polyh->clone() : 0;
		}
		// BC zones
		for (int i = 0; i < ::count_bc; ++i)
		{
			const struct BC* bc_ptr = ::bc[i];
			bc_map[bc_ptr] = bc_ptr->polyh ? bc_ptr->polyh->clone() : 0;
		}
		// Zone_budget zones
		std::map<int, Zone_budget*>::iterator zit = Zone_budget::zone_budget_map.begin();
		for (; zit != Zone_budget::zone_budget_map.end(); ++zit)
		{
			zb_map[zit->second] = zit->second->Get_polyh() ? zit->second->Get_polyh()->clone() : 0;
		}
		// IC zones
		for (int i = 0; i < ::count_head_ic; ++i)
		{
			const struct Head_ic* head_ic_ptr = ::head_ic[i];
			head_ic_map[head_ic_ptr] = head_ic_ptr->polyh ? head_ic_ptr->polyh->clone() : 0;
		}
		// CHEMISTRY_IC
		for (int i = 0; i < ::count_chem_ic; ++i)
		{
			const struct chem_ic* chem_ic_ptr = ::chem_ic[i];
			chem_ic_map[chem_ic_ptr] = chem_ic_ptr->polyh ? chem_ic_ptr->polyh->clone() : 0;
		}
		pInput->Accumulate(false);
		///pInput->Load();

		::OutputDebugString("Finished loading trans.dat file\n");
		if (pInput->GetErrorCount() != 0)
		{
			// goto ImportError;
			// throw "Input file contains errors.";
			throw pInput->GetErrorCount();
		}

		// start loading data

		// FLOW_ONLY
		//
		CFlowOnly flowOnly(::flow_only != 0);
		if (!flowOnly)
		{
			flowOnly.SetDiffusivity(::fluid_diffusivity);
		}

		// STEADY_FLOW
		//
		CSteadyFlow steadyFlow;
		steadyFlow.SyncWithSrcInput();

		// MEDIA
		//
		CGridElt gridElt = CGridElt::Full();
		size_t n = sizeof(gridElt);
		ASSERT(n == 80);
		::GetDefaultMedia(&gridElt);

		// HEAD_IC
		//
		CHeadIC headIC = CHeadIC::Full();
		::GetDefaultHeadIC(&headIC);

		// CHEMISTRY_IC
		//
		CChemIC chemIC = CChemIC::Full();
		::GetDefaultChemIC(&chemIC);

		// PRINT_INITIAL
		//
		printFreq.SyncWithSrcInput();

		// PRINT_FREQUENCY
		//
		CPrintInput printInput;
		printInput.SyncWithSrcInput();

		// SOLUTION_METHOD
		//
		CSolutionMethod solutionMethod;
		solutionMethod.SyncWithSrcInput();

		// TIME_CONTROL
		//
		CTimeControl2 timeControl2(::time_step, ::time_end, ::count_time_end, ::time_start);

		// create new document
		//
		ASSERT(::map_to_grid);
		CNewModel model;
		model.m_flowOnly       = flowOnly;
		model.m_freeSurface    = (::free_surface != 0);
		model.m_steadyFlow     = steadyFlow;
		model.m_units          = CUnits(::units, *::map_to_grid);
		model.m_gridKeyword    = CGridKeyword(::grid, ::snap, ::axes, ::print_input_xy, ::grid_origin, ::grid_angle);
		model.m_media          = gridElt;
		model.m_headIC         = headIC;
		model.m_chemIC         = chemIC;
		model.m_printInput     = printInput;
		model.m_printFreq      = printFreq;
		model.m_timeControl2   = timeControl2;
		model.m_solutionMethod = solutionMethod;

		// load xml data
		//
		CXMLSerializer xmls;
		xmls.load(xml, model);

		this->New(model);

		// default zone
		CZone defaultZone;
		this->m_pGridActor->GetDefaultZone(defaultZone);

		// MEDIA zones
		//
		for (int i = 0; i < ::count_grid_elt_zones; ++i)
		{
			// not undoable
			const struct grid_elt* grid_elt_ptr = ::grid_elt_zones[i];
			ASSERT(grid_elt_ptr->polyh && ::AfxIsValidAddress(grid_elt_ptr->polyh, sizeof(Polyhedron)));
			if (i == 0 && dynamic_cast<Cube*>(grid_elt_ptr->polyh) && defaultZone == *grid_elt_ptr->polyh->Get_bounding_box())
			{
				// if the first zone is equivalent to the default gridElt
				// don't add it
				if (model.m_media == *grid_elt_ptr)
				{
					continue;
				}
				// special case for active property
				// it's assumed that the default media zone
				// is always active
				CGridElt alt(*grid_elt_ptr);
				if (alt.active) delete static_cast<Cproperty*>(alt.active);
				alt.active = new Cproperty(1);
				if (model.m_media == alt)
				{
					continue;
				}
			}

			// store pre-translated polyh
			CGridElt data(*grid_elt_ptr);
			std::auto_ptr<Polyhedron> ap(data.polyh);
			ASSERT(grid_elt_map.find(grid_elt_ptr) != grid_elt_map.end());
			data.polyh = grid_elt_map[grid_elt_ptr] ? grid_elt_map[grid_elt_ptr]->clone() : grid_elt_ptr->polyh->clone();

			// not undoable
			std::auto_ptr< CZoneCreateAction<CMediaZoneActor> > pAction(
				new CZoneCreateAction<CMediaZoneActor>(
					this,
					data.polyh,
					::grid_origin,
					::grid_angle,
					data.polyh->Get_description()->c_str()
					)
				);
			pAction->GetZoneActor()->SetData(data);
			pAction->Execute();
		}

		// BC Zones
		//
		//std::list<CBCZoneActor*> listBCZoneActors;
		for (int i = 0; i < ::count_bc; ++i)
		{
			const struct BC* bc_ptr = ::bc[i];
			ASSERT(bc_ptr->polyh && ::AfxIsValidAddress(bc_ptr->polyh, sizeof(Polyhedron)));

			// store pre-translated polyh
			CBC data(*bc_ptr);
			std::auto_ptr<Polyhedron> ap(data.polyh);
			ASSERT(bc_map.find(bc_ptr) != bc_map.end());
			data.polyh = bc_map[bc_ptr] ? bc_map[bc_ptr]->clone() : bc_ptr->polyh->clone();

			// not undoable
			std::auto_ptr< CZoneCreateAction<CBCZoneActor> > pAction(
				new CZoneCreateAction<CBCZoneActor>(
					this,
					data.polyh,
					::grid_origin,
					::grid_angle,
					data.polyh->Get_description()->c_str()
					)
				);
			pAction->GetZoneActor()->SetData(data);
			pAction->Execute();
		}

		// Wells
		//
		for (int i = 0; i < ::count_wells; ++i)
		{
			const Well *well_ptr = &::wells[i];
			CWellSchedule well(*well_ptr);

			// not undoable
			CWellCreateAction *pAction = new CWellCreateAction(this, well);
			pAction->Execute();
			delete pAction;
		}

		// RIVER
		//
		for (int i = 0; i < ::count_rivers; ++i)
		{
			const River* river_ptr = &::rivers[i];
			CRiver river(*river_ptr);

			// not undoable
			CPointConnectorCreateAction<CRiverActor, CRiver> *pAction = new CPointConnectorCreateAction<CRiverActor, CRiver>(this, river);
			pAction->Execute();
			delete pAction;
		}

		// DRAIN
		//
		for (size_t i = 0; i < ::drains.size(); ++i)
		{
			const Drain* drain_ptr = ::drains[i];
			CDrain drain(*drain_ptr);

			// not undoable
			CPointConnectorCreateAction<CDrainActor, CDrain> *pAction = new CPointConnectorCreateAction<CDrainActor, CDrain>(this, drain);
			pAction->Execute();
			delete pAction;
		}

		// ZONE_FLOW
		//
		std::map<int, Zone_budget*>::iterator it = Zone_budget::zone_budget_map.begin();
		for (; it != Zone_budget::zone_budget_map.end(); ++it)
		{
			// store pre-translated polyh
			Zone_budget data(*it->second);
			ASSERT(zb_map.find(it->second) != zb_map.end());
			ASSERT(zb_map[it->second] || it->second->Get_polyh());
			if (zb_map[it->second] == 0 && it->second->Get_polyh() == 0)
			{
				::AfxMessageBox("Warning: Empty ZONE_FLOW Not Implemented");
			}
			else
			{
				data.Set_polyh(zb_map[it->second] ? zb_map[it->second]->clone() : it->second->Get_polyh()->clone());

				// not undoable
				std::auto_ptr< CZoneCreateAction<CZoneFlowRateZoneActor> > pAction(
					new CZoneCreateAction<CZoneFlowRateZoneActor>(
						this,
						it->second->Get_polyh(),
						::grid_origin,
						::grid_angle,
						it->second->Get_polyh()->Get_description()->c_str()
						)
					);
				pAction->GetZoneActor()->SetData(data);
				pAction->Execute();
			}
		}

		// IC
		//
		for (int i = 0; i < ::count_head_ic; ++i)
		{
			const struct Head_ic* head_ic_ptr = ::head_ic[i];
			ASSERT(head_ic_ptr->polyh && ::AfxIsValidAddress(head_ic_ptr->polyh, sizeof(Polyhedron)));
			if (i == 0 && dynamic_cast<Cube*>(head_ic_ptr->polyh) && defaultZone == *head_ic_ptr->polyh->Get_bounding_box())
			{
				// if the first zone is equivalent to the default headIC
				// don't add it
				if (model.m_headIC == *head_ic_ptr)
				{
					continue;
				}
			}

			// store pre-translated polyh
			CHeadIC data(*head_ic_ptr);
			std::auto_ptr<Polyhedron> ap(data.polyh);
			ASSERT(head_ic_map.find(head_ic_ptr) != head_ic_map.end());
			data.polyh = head_ic_map[head_ic_ptr] ? head_ic_map[head_ic_ptr]->clone() : head_ic_ptr->polyh->clone();

			// not undoable
			std::auto_ptr< CZoneCreateAction<CICHeadZoneActor> > pAction(
				new CZoneCreateAction<CICHeadZoneActor>(
					this,
					data.polyh,
					::grid_origin,
					::grid_angle,
					data.polyh->Get_description()->c_str()
					)
				);
			pAction->GetZoneActor()->SetData(data);
			pAction->Execute();
		}

		// CHEMISTRY_IC
		//
		for (int i = 0; i < ::count_chem_ic; ++i)
		{
			const struct chem_ic* chem_ic_ptr = ::chem_ic[i];
			ASSERT(chem_ic_ptr->polyh && ::AfxIsValidAddress(chem_ic_ptr->polyh, sizeof(Polyhedron)));
			if (i == 0 && dynamic_cast<Cube*>(chem_ic_ptr->polyh) && defaultZone == *chem_ic_ptr->polyh->Get_bounding_box())
			{
				// if the first zone is equivalent to the default chemIC
				// don't add it
				if (model.m_chemIC == *chem_ic_ptr)
				{
					continue;
				}
				// check special case
				//
				CChemIC ic;
				Cproperty::CopyProperty(&ic.solution, chem_ic_ptr->solution);
				if (model.m_chemIC == ic)
				{
					if (this->GetPropertyTreeControlBar())
					{
						CTreeCtrlNode nodeICChem = this->GetPropertyTreeControlBar()->GetICChemNode();
						if (CICChemZoneActor *pZone = CICChemZoneActor::SafeDownCast((vtkObject*)nodeICChem.GetChildAt(0).GetData()))
						{
							if (CTreeCtrl *pTreeCtrl = this->GetPropertyTreeControlBar()->GetTreeCtrl())
							{
								CChemIC chemIC(*chem_ic_ptr);
								CSetChemICAction* a = new CSetChemICAction(pZone, pTreeCtrl, chemIC, pZone->GetDesc());
								a->Execute();
								delete a;
								continue;
							}
						}
					}
				}
			}

			// store pre-translated polyh
			CChemIC data(*chem_ic_ptr);
			std::auto_ptr<Polyhedron> ap(data.polyh);
			ASSERT(chem_ic_map.find(chem_ic_ptr) != chem_ic_map.end());
			data.polyh = chem_ic_map[chem_ic_ptr] ? chem_ic_map[chem_ic_ptr]->clone() : chem_ic_ptr->polyh->clone();

			// not undoable
			std::auto_ptr< CZoneCreateAction<CICChemZoneActor> > pAction(
				new CZoneCreateAction<CICChemZoneActor>(
					this,
					data.polyh,
					::grid_origin,
					::grid_angle,
					data.polyh->Get_description()->c_str()
					)
				);
			pAction->GetZoneActor()->SetData(data);
			pAction->Execute();
		}
		// convert all relative paths to absolute paths
		this->DataSourcePathsRelativeToAbsolute(lpszPathName);
	}
	catch (int)
	{
		CImportErrorDialog dlg;
		dlg.m_lpszErrorMessages = pInput->GetErrorMsg();
		dlg.DoModal();
		bReturnValue = FALSE;
	}
	catch (const char * error)
	{
		::AfxMessageBox(error, MB_OK|MB_ICONEXCLAMATION);
		bReturnValue = FALSE;
	}
#if !defined(_DEBUG)
	catch (...)
	{
		::AfxMessageBox("An unknown error occured during import", MB_OK|MB_ICONEXCLAMATION);
		bReturnValue = FALSE;
	}
#endif

	std::map<const struct grid_elt*, Polyhedron*>::iterator gi = grid_elt_map.begin();
	for(; gi != grid_elt_map.end(); ++gi)
	{
		delete gi->second;
	}
	std::map<const struct BC*, Polyhedron*>::iterator bi = bc_map.begin();
	for(; bi != bc_map.end(); ++bi)
	{
		delete bi->second;
	}
	std::map<const Zone_budget*, Polyhedron*>::iterator zi = zb_map.begin();
	for(; zi != zb_map.end(); ++zi)
	{
		delete zi->second;
	}
	std::map<const struct Head_ic*, Polyhedron*>::iterator hi = head_ic_map.begin();
	for(; hi != head_ic_map.end(); ++hi)
	{
		delete hi->second;
	}
	std::map<const struct chem_ic*, Polyhedron*>::iterator ci = chem_ic_map.begin();
	for(; ci != chem_ic_map.end(); ++ci)
	{
		delete ci->second;
	}

	// reset tree control
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->GetGridNode().Expand(TVE_COLLAPSE);
		pTree->GetMediaNode().Expand(TVE_COLLAPSE);
		pTree->GetICNode().Expand(TVE_COLLAPSE);
		pTree->GetBCNode().Expand(TVE_COLLAPSE);
		pTree->GetWellsNode().Expand(TVE_COLLAPSE);
		pTree->GetRiversNode().Expand(TVE_COLLAPSE);
		pTree->GetDrainsNode().Expand(TVE_COLLAPSE);
		pTree->GetZoneFlowRatesNode().Expand(TVE_COLLAPSE);
		this->ClearSelection();
	}

	pInput->Delete();
	this->SetTitle(strPrefix);
	this->SetModifiedFlag(TRUE);

	if (::AfxGetMainWnd())
	{
		if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
		{
			CString status(_T("Ready"));
			pWnd->SetWindowText(status);
		}
	}

	return bReturnValue;

//ImportError:
//	CImportErrorDialog dlg;
//	dlg.m_lpszErrorMessages = pInput->GetErrorMsg();
//	dlg.DoModal();
//	pInput->Delete();
//	return FALSE;
}

void CWPhastDoc::OnFileExport()
{
	CString newName;
	newName = this->GetTitle();

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];

	::_tsplitpath_s(newName.GetBuffer(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);
	newName = szFName;
	newName += ".trans.dat";

	if (!DoPromptFileName(newName, IDS_EXPORT_PHAST_TRANS_136,
	  OFN_HIDEREADONLY, FALSE))
		return; // save cancelled

	// modified from: CDocument* CDocManager::OpenDocumentFile
	TCHAR szPath[_MAX_PATH];
	ASSERT(lstrlen(newName) < sizeof(szPath)/sizeof(szPath[0]));
	TCHAR szTemp[_MAX_PATH];
	if (newName[0] == '\"')
		newName = newName.Mid(1);
	lstrcpyn(szTemp, newName, _MAX_PATH);
	LPTSTR lpszLast = _tcsrchr(szTemp, '\"');
	if (lpszLast != NULL)
		*lpszLast = 0;
	SrcFullPath(szPath, szTemp);
	TCHAR szLinkName[_MAX_PATH];
	if (SrcResolveShortcut(AfxGetMainWnd(), szPath, szLinkName, _MAX_PATH))
		lstrcpy(szPath, szLinkName);

	CString strPath(szPath);
	if (strPath.Right(10).CompareNoCase(".trans.dat") != 0)
	{
		strPath += ".trans.dat";
	}

	this->DataSourcePathsAbsoluteToRelative(strPath);
	if (!this->DoExport(strPath))
	{
		::AfxMessageBox("An error occured during the export", MB_OK);
// COMMENT: {8/7/2008 4:03:09 PM}		this->SetModifiedFlag(FALSE);
	}
	this->DataSourcePathsRelativeToAbsolute(strPath);
}

BOOL CWPhastDoc::DoExport(LPCTSTR lpszPathName)
{
	std::ofstream ofs;
	ofs.open(lpszPathName);
	if (!ofs.is_open())
	{
		CString str("Unable to open \"");
		str += lpszPathName;
		str += "\" for writing.";
		::AfxMessageBox(str, MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	// preserve cd
	CSaveCurrentDirectory save;

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	::_tsplitpath_s(lpszPathName, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);

	TCHAR szDirectory[_MAX_DIR];
	::_tmakepath_s(szDirectory, _MAX_DIR, szDrive, szDir, NULL, NULL);
	save.SetCurrentDirectory(szDirectory);

	CString strPrefix(szFName);
	if (strPrefix.Right(6).CompareNoCase(".trans") == 0)
	{
		strPrefix = strPrefix.Left(strPrefix.GetLength() - 6);
	}

	this->XMLExport(ofs, strPrefix, lpszPathName);
	return this->WriteTransDat(ofs);
}

BOOL CWPhastDoc::XMLExport(std::ostream& os, const char *prefix, LPCTSTR lpszPathName)
{
	CXMLSerializer x;

	const char* xml = x.writeDOM(this, prefix, lpszPathName);
	std::string str(xml);
	std::istringstream iss(str);

	while(iss)
	{
		std::string s;
		std::getline(iss, s);

		// skip over empty lines
		//
		if (!s.empty())
		{
			os << "# " << s << std::endl;
		}
	}

	// add empty comment line
	os << "# " << std::endl;

	return TRUE;
}

BOOL CWPhastDoc::WriteTransDat(std::ostream& os)
{
	int nCount;

	os.precision(DBL_DIG);

	// FLOW_ONLY
	os << this->m_pModel->m_flowOnly;

	// STEADY_FLOW
	os << this->m_pModel->m_steadyFlow;

	// FREE_SURFACE
	os << this->m_pModel->m_freeSurface;

	// SOLUTION_METHOD
	os << this->m_pModel->m_solutionMethod;

	// UNITS
	os << this->GetUnits();

	// GRID
	CGridKeyword gridKeyword;
	this->m_pGridActor->GetGridKeyword(gridKeyword);
	os << gridKeyword;

	// MEDIA
	CTreeCtrlNode nodeMedia = this->GetPropertyTreeControlBar()->GetMediaNode();
	nCount = nodeMedia.GetChildCount();
	if (nCount > 0)
	{
		os << "MEDIA\n";
	}
	for (int i = 0; i < nCount; ++i)
	{
		if (CMediaZoneActor *pZone = CMediaZoneActor::SafeDownCast((vtkObject*)nodeMedia.GetChildAt(i).GetData()))
		{
			CGridElt elt = pZone->GetGridElt();
			os << elt;
		}
	}

	// BCs SPECIFIED_VALUE_BC FLUX_BC LEAKY_BC
	CTreeCtrlNode nodeBC = this->GetPropertyTreeControlBar()->GetBCNode();
	nCount = nodeBC.GetChildCount();
	for (int i = 0; i < nCount; ++i) {
		if (CBCZoneActor *pZone = CBCZoneActor::SafeDownCast((vtkObject*)nodeBC.GetChildAt(i).GetData())) {
			CBC b = pZone->GetBC();
			os << b;
		}
	}

	// HEAD_IC
	CTreeCtrlNode nodeICHead = this->GetPropertyTreeControlBar()->GetICHeadNode();
	nCount = nodeICHead.GetChildCount();
	for (int i = 0; i < nCount; ++i)
	{
		if (CICHeadZoneActor *pZone = CICHeadZoneActor::SafeDownCast((vtkObject*)nodeICHead.GetChildAt(i).GetData()))
		{
			os << pZone->GetData();
		}
	}

	// CHEMISTRY_IC
	CTreeCtrlNode nodeICChem = this->GetPropertyTreeControlBar()->GetICChemNode();
	nCount = nodeICChem.GetChildCount();
	for (int i = 0; i < nCount; ++i)
	{
		if (CICChemZoneActor *pZone = CICChemZoneActor::SafeDownCast((vtkObject*)nodeICChem.GetChildAt(i).GetData()))
		{
			os << pZone->GetData();
		}
	}

	// Wells
	CTreeCtrlNode nodeWells = this->GetPropertyTreeControlBar()->GetWellsNode();
	nCount = nodeWells.GetChildCount();
	for (int i = 0; i < nCount; ++i)
	{
		if (CWellActor *pWellActor = CWellActor::SafeDownCast((vtkObject*)nodeWells.GetChildAt(i).GetData()))
		{
			os << pWellActor->GetWell();
		}
	}

	// RIVERS
	CTreeCtrlNode nodeRivers = this->GetPropertyTreeControlBar()->GetRiversNode();
	nCount = nodeRivers.GetChildCount();
	for (int i = 0; i < nCount; ++i)
	{
		if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast((vtkObject*)nodeRivers.GetChildAt(i).GetData()))
		{
			os << *pRiverActor;
		}
	}

	// DRAINS
	CTreeCtrlNode nodeDrains = this->GetPropertyTreeControlBar()->GetDrainsNode();
	nCount = nodeDrains.GetChildCount();
	for (int i = 0; i < nCount; ++i)
	{
		if (CDrainActor *pDrainActor = CDrainActor::SafeDownCast((vtkObject*)nodeDrains.GetChildAt(i).GetData()))
		{
			os << *pDrainActor;
		}
	}

	// ZONE_FLOW
	CTreeCtrlNode nodeZoneFlowRates = this->GetPropertyTreeControlBar()->GetZoneFlowRatesNode();
	nCount = nodeZoneFlowRates.GetChildCount();
	for (int i = 0; i < nCount; ++i)
	{
		if (CZoneFlowRateZoneActor *pZone = CZoneFlowRateZoneActor::SafeDownCast((vtkObject*)nodeZoneFlowRates.GetChildAt(i).GetData()))
		{
			os << pZone->GetData();
		}
	}

	os << this->GetPrintInput();

	os << this->GetPrintFrequency();

	os << this->GetTimeControl2();

	return TRUE;
}

BOOL CWPhastDoc::DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog)
{
	// modified from: BOOL CDocManager::DoPromptFileName
	CFileDialog dlgFile(bOpenFileDialog, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0);

	CString title;
	VERIFY(title.LoadString(nIDSTitle));

	dlgFile.m_ofn.Flags |= lFlags;

	CString strFilter;

	// add *.trans.dat filter
	// modified from: _AfxAppendFilterSuffix
	strFilter += _T("Phast Transport Files (*.trans.dat)");
	strFilter += (TCHAR)_T('\0');
	strFilter += _T("*.trans.dat");
	strFilter += (TCHAR)_T(';');
	strFilter += (TCHAR)_T('\0');
	dlgFile.m_ofn.nFilterIndex = dlgFile.m_ofn.nMaxCustFilter + 1;  // 1 based number
	dlgFile.m_ofn.nMaxCustFilter++;

	// append the "*.*" all files filter
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);

	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();
	return nResult == IDOK;
}

const CUnits& CWPhastDoc::GetUnits(void)const
{
	return *this->m_pUnits;
}

void CWPhastDoc::GetUnits(CUnits& units)const
{
	units = (*this->m_pUnits);
}

void CWPhastDoc::StandardizeUnits(CUnits& units)const
{
	// minimize length units for display purposes
	//
	if (units.horizontal.defined == TRUE)
	{
		CString strLength(units.horizontal.input);
		CGlobal::MinimizeLengthUnits(strLength);
		units.horizontal.set_input(strLength);
	}
	if (units.vertical.defined == TRUE)
	{
		CString strLength(units.vertical.input);
		CGlobal::MinimizeLengthUnits(strLength);
		units.vertical.set_input(strLength);
	}

	if (units.map_horizontal.defined == TRUE)
	{
		CString strLength(units.map_horizontal.input);
		CGlobal::MinimizeLengthUnits(strLength);
		units.map_horizontal.set_input(strLength);
	}
	if (units.map_vertical.defined == TRUE)
	{
		CString strLength(units.map_vertical.input);
		CGlobal::MinimizeLengthUnits(strLength);
		units.map_vertical.set_input(strLength);
	}
}


void CWPhastDoc::SetUnits(const CUnits& units)
{
	ASSERT(this->m_pUnits);
	(*this->m_pUnits) = units;

	// minimize length units for display purposes
	//
	this->StandardizeUnits(*this->m_pUnits);

	// update properties bar
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetUnits(this->m_pUnits);
	}

	// update grid
	//
	CGrid x, y, z;
	this->m_pGridActor->GetGrid(x, y, z);
	this->ResizeGrid(x, y, z);

	// set scale for all zones
	//
	if (vtkPropCollection* pCollection = this->GetPropCollection())
	{
		vtkCollectionSimpleIterator csi;
		pCollection->InitTraversal(csi);
		for (int i = 0; i < pCollection->GetNumberOfItems(); ++i)
		{
			vtkProp* prop = pCollection->GetNextProp(csi);
			if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop))
			{
				ASSERT(FALSE);
				pZone->SetUnits(units);
			}
			if (vtkPropAssembly *pPropAssembly = vtkPropAssembly::SafeDownCast(prop))
			{
				if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
				{
					vtkProp *pProp;
					vtkCollectionSimpleIterator c2;
					pPropCollection->InitTraversal(c2);
					for (; (pProp = pPropCollection->GetNextProp(c2)); )
					{
						if (vtkProp3D *prop3D = vtkProp3D::SafeDownCast(pProp))
						{
							if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop3D))
							{
								pZone->SetUnits(units);
							}
							if (CWellActor *pWellActor = CWellActor::SafeDownCast(prop3D))
							{
								pWellActor->SetUnits(units);
							}
							if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(prop3D))
							{
								pRiverActor->SetUnits(units);
							}
						}
					}
				}
			}
		}
	}

	// for all views
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);

		// resize the widgets
		//
		if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( pView->GetInteractor()->GetPicker() ))
		{
			if (vtkProp3D* prop = picker->GetProp3D())
			{
				pView->Select(prop);
			}
		}
	}
	this->UpdateAllViews(0);


	// Update BoxPropertiesDialogBar
	//
	if (CBoxPropertiesDialogBar* pBar = this->GetBoxPropertiesDialogBar())
	{
		POSITION pos = this->GetFirstViewPosition();
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		pBar->Set(pView, pBar->GetProp3D(), this->GetUnits());
	}
}

void CWPhastDoc::New(const CNewModel& model)
{
	ASSERT(this->m_pGridActor);
	ASSERT(this->AxesActor);
	ASSERT(this->m_pPropCollection);
	ASSERT(this->m_pimpl);
	ASSERT(this->m_pUnits);

	if (vtkPropCollection *props = this->GetPropCollection())
	{
		ASSERT(props->GetNumberOfItems() == 0);
	}

	// set FlowOnly
	// set SteadyFlow
	// set FreeSurface
	// set PrintInput
	// set PrintFreq
	this->SetModel(model);

	// set the units
	//
	ASSERT(this->m_pUnits);
	*this->m_pUnits = model.m_units;
	this->StandardizeUnits(*this->m_pUnits);

	// update properties bar
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetUnits(this->m_pUnits);
	}

	// set the grid
	//
	this->m_pGridActor->SetGridKeyword(model.m_gridKeyword, this->GetUnits());
	this->m_pGridActor->SetPickable(0);
	this->m_pPropCollection->AddItem(this->m_pGridActor);
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar()) {
		pTree->SetGridActor(this->m_pGridActor);
	}

	// set the axes
	//
	double *bounds = this->GetGridBounds();
	double defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
	this->AxesActor->SetDefaultPositions(bounds);
	this->AxesActor->SetDefaultSize(defaultAxesSize);
	this->AxesActor->SetDefaultTubeDiameter(defaultAxesSize * 0.1);
	this->m_pPropCollection->AddItem(this->AxesActor);

	// create the default zones
	//
	CZone zone;
	this->m_pGridActor->GetDefaultZone(zone);
	Domain domain(&zone, PHAST_Transform::GRID);

	// default media
	//
	CZoneCreateAction<CMediaZoneActor>* pMediaAction = new CZoneCreateAction<CMediaZoneActor>(
		this,
		"Default",
		&domain,
		NULL
		);
	CGridElt media(model.m_media);
	ASSERT(media.polyh == NULL);
	media.polyh = domain.clone();
	pMediaAction->GetZoneActor()->SetData(media);
	pMediaAction->GetZoneActor()->SetDefault(true);
	pMediaAction->Execute();
	delete pMediaAction;


	// default head_ic
	CZoneCreateAction<CICHeadZoneActor>* pICHeadAction = new CZoneCreateAction<CICHeadZoneActor>(
		this,
		"Default",
		&domain,
		NULL
		);
	pICHeadAction->GetZoneActor()->SetData(model.m_headIC);
	pICHeadAction->GetZoneActor()->SetDefault(true);
	pICHeadAction->Execute();
	delete pICHeadAction;

	// CHEMISTRY_IC (default)
	CZoneCreateAction<CICChemZoneActor>* pChemICAction = new CZoneCreateAction<CICChemZoneActor>(
		this,
		"Default",
		&domain,
		NULL
		);
	pChemICAction->GetZoneActor()->SetData(model.m_chemIC);
	pChemICAction->GetZoneActor()->SetDefault(true);
	pChemICAction->Execute();
	delete pChemICAction;

	// update properties bar
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetTimeControl2(&this->m_pModel->m_timeControl2);
	}

	if (model.HasSiteMap2())
	{
		ASSERT(this->m_pMapActor == NULL);
		this->m_pMapActor = CMapActor::New();   // Note pixel(0,0) is the same size as all other pixels
// COMMENT: {8/11/2009 3:07:31 PM}		//{{
// COMMENT: {8/11/2009 3:07:31 PM}		this->m_pMapActor->GetProperty()->SetOpacity(0.3);
// COMMENT: {8/11/2009 3:07:31 PM}		//}}
		this->m_pMapActor->SetSiteMap2(model.GetSiteMap2());
		this->m_pMapActor->SetPickable(0);
		this->GetPropCollection()->AddItem(this->m_pMapActor);
// COMMENT: {8/10/2009 6:02:25 PM}		//{{
// COMMENT: {8/10/2009 6:02:25 PM}		this->MapImageActor3 = CMapImageActor3::New();
// COMMENT: {8/10/2009 6:02:25 PM}		this->MapImageActor3->SetFileName(model.GetSiteMap().m_fileName.c_str());
// COMMENT: {8/10/2009 6:02:25 PM}		this->MapImageActor3->SetWorldTransform(model.GetSiteMap().GetWorldTransform());
// COMMENT: {8/10/2009 6:02:25 PM}		this->MapImageActor3->SetPickable(1);
// COMMENT: {8/10/2009 6:02:25 PM}		this->GetPropCollection()->AddItem(this->MapImageActor3);
// COMMENT: {8/10/2009 6:02:25 PM}		//}}
	}

	// reset tree control
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->GetGridNode().Expand(TVE_COLLAPSE);
		pTree->GetMediaNode().Expand(TVE_COLLAPSE);
		pTree->GetICNode().Expand(TVE_COLLAPSE);
		pTree->GetBCNode().Expand(TVE_COLLAPSE);
		pTree->GetWellsNode().Expand(TVE_COLLAPSE);
		pTree->GetRiversNode().Expand(TVE_COLLAPSE);
		pTree->ClearSelection();
	}

	// Update BoxPropertiesDialogBar
	//
	if (CBoxPropertiesDialogBar* pBar = this->GetBoxPropertiesDialogBar())
	{
		POSITION pos = this->GetFirstViewPosition();
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		pBar->Set(pView, 0, this->GetUnits());
	}

// COMMENT: {9/4/2009 9:16:25 PM}	// refresh screen
// COMMENT: {9/4/2009 9:16:25 PM}	//
// COMMENT: {9/4/2009 9:16:25 PM}	this->ResetCamera();
// COMMENT: {9/4/2009 9:16:25 PM}	this->UpdateAllViews(0);
// COMMENT: {9/9/2009 9:26:21 PM}	this->ExecutePipeline();
}

void CWPhastDoc::OnFileRun()
{
	CWaitCursor wait;
	// this is required to get the prefix
	//
	if (this->GetPathName().IsEmpty())
	{
		if (!this->DoSave(NULL))
		{
			return;
		}
	}

	CSaveCurrentDirectory scd;
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];

	ASSERT(!this->GetPathName().IsEmpty());
	::_tsplitpath(this->GetPathName(), szDrive, szDir, szFName, szExt);

	TCHAR szOutDir[_MAX_PATH];
	::_tmakepath(szOutDir, szDrive, szDir, NULL, NULL);
	scd.SetCurrentDirectoryA(szOutDir);

	TCHAR szPhastTmp[_MAX_PATH];
	::_tmakepath(szPhastTmp, szDrive, szDir, "Phast", ".tmp");

	CString strPrefix(szFName);

	std::ostringstream oss;
	this->DataSourcePathsAbsoluteToRelative(szPhastTmp);
	this->WriteTransDat(oss);
	this->DataSourcePathsRelativeToAbsolute(szPhastTmp);

	std::string str = oss.str();
	std::istringstream iss(str);

	CPhastInput* pInput = CPhastInput::New(iss, strPrefix);
	if (!pInput) return;

	// Update StatusBar
	//
	if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
	{
		CString status(_T("Writing Phast.tmp..."));
		pWnd->SetWindowText(status);
	}

	bool bWritePhastTmpOK = false;
	try
	{
		pInput->WritePhastTmp(szPhastTmp);
		if (pInput->GetErrorCount() != 0)
		{
			// goto ImportError;
			// throw "Input file contains errors.";
			throw pInput->GetErrorCount();
		}
		bWritePhastTmpOK = true;
	}
	catch (int)
	{
		// Update StatusBar
		//
		if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
		{
			CString status;
			status.LoadStringA(AFX_IDS_IDLEMESSAGE);
			pWnd->SetWindowText(status);
		}

		// display errors
		CImportErrorDialog dlg;
		dlg.m_lpszErrorMessages = pInput->GetErrorMsg();
		dlg.DoModal();
	}
	catch (const char * error)
	{
		::AfxMessageBox(error, MB_OK|MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		::AfxMessageBox("An unknown error occured during the run.", MB_OK|MB_ICONEXCLAMATION);
	}
	pInput->Delete();

	if (bWritePhastTmpOK)
	{
		wait.Restore();

		TCHAR szPhastTmpDir[_MAX_PATH];
		::_tmakepath(szPhastTmpDir, szDrive, szDir, "", "");

		// Update StatusBar
		//
		if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
		{
			CString status(_T("Running..."));
			pWnd->SetWindowText(status);
		}

		CRunDialog run;
		run.SetWorkingDirectory(szPhastTmpDir);
		run.DoModal();
	}
}

void CWPhastDoc::SetTimeControl2(const CTimeControl2& timeControl2)
{
	ASSERT(this->m_pModel);
	this->m_pModel->m_timeControl2 = timeControl2;

	// update properties bar
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetTimeControl2(&this->m_pModel->m_timeControl2);
	}
}

const CTimeControl2& CWPhastDoc::GetTimeControl2(void)const
{
	return this->m_pModel->m_timeControl2;
}

void CWPhastDoc::SetPrintFrequency(const CPrintFreq& printFreq)
{
	this->m_pModel->m_printFreq = printFreq;

	// update properties bar
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetPrintFrequency(&this->m_pModel->m_printFreq);
	}
}

const CPrintFreq& CWPhastDoc::GetPrintFrequency(void)const
{
	return this->m_pModel->m_printFreq;
}

void CWPhastDoc::SetPrintInput(const CPrintInput& printInput)
{
	this->m_pModel->m_printInput = printInput;

	// update properties bar
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetPrintInput(&this->m_pModel->m_printInput);
	}
}

const CPrintInput& CWPhastDoc::GetPrintInput(void)const
{
	return this->m_pModel->m_printInput;
}

void CWPhastDoc::OnToolsNewStressPeriod(void)
{
}

void CWPhastDoc::ReleaseGraphicsResources(vtkProp* pProp)
{
	// foreach view
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView *pView = this->GetNextView(pos);
		if (CWPhastView *pWPhastView = static_cast<CWPhastView*>(pView))
		{
			ASSERT_KINDOF(CWPhastView, pWPhastView);
			ASSERT_VALID(pWPhastView);
			pProp->ReleaseGraphicsResources(pWPhastView->GetRenderer()->GetVTKWindow());
		}
	}
}

void CWPhastDoc::OnUpdateViewAxes(CCmdUI *pCmdUI)
{
	if (this->AxesActor->GetVisibility())
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CWPhastDoc::OnViewAxes()
{
	if (this->AxesActor->GetVisibility())
	{
		this->AxesActor->SetVisibility(0);
	}
	else
	{
		this->AxesActor->SetVisibility(1);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateSetprojectiontoParallel(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(this->ProjectionMode == PT_PARALLEL);
}

void CWPhastDoc::OnSetprojectiontoParallel()
{
	this->ProjectionMode = PT_PARALLEL;
	// update views
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		pView->ParallelProjectionOn();
	}
	UpdateAllViews(NULL);
}

void CWPhastDoc::OnUpdateSetprojectiontoPerspective(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(this->ProjectionMode == PT_PERSPECTIVE);
}

void CWPhastDoc::OnSetprojectiontoPerspective()
{
	this->ProjectionMode = PT_PERSPECTIVE;

	// update views
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		pView->ParallelProjectionOff();
	}
	UpdateAllViews(NULL);
}

void CWPhastDoc::OnUpdateViewSitemap(CCmdUI *pCmdUI)
{
	if (this->m_pMapActor == NULL)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	if (this->m_pMapActor->GetVisibility())
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CWPhastDoc::OnViewSitemap()
{
	if (this->m_pMapActor->GetVisibility())
	{
		this->m_pMapActor->SetVisibility(0);
	}
	else
	{
		this->m_pMapActor->SetVisibility(1);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateViewGrid(CCmdUI *pCmdUI)
{
	if (this->m_pGridActor->GetVisibility())
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CWPhastDoc::OnViewGrid()
{
	CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar();

	if (this->m_pGridActor->GetVisibility())
	{
		this->m_pGridActor->SetVisibility(0);
		// if (pTree) pTree->SetNodeCheck(pTree->GetGridNode(), BST_UNCHECKED);
		if (pTree) pTree->GetGridNode().SetState(INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1), TVIS_STATEIMAGEMASK);

	}
	else
	{
		this->m_pGridActor->SetVisibility(1);
		// if (pTree) pTree->SetNodeCheck(pTree->GetGridNode(), BST_CHECKED);
		if (pTree) pTree->GetGridNode().SetState(INDEXTOSTATEIMAGEMASK(BST_CHECKED + 1), TVIS_STATEIMAGEMASK);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateMediaZonesHideAll(CCmdUI *pCmdUI)
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		switch (pTree->GetMediaCheck())
		{
		case BST_UNCHECKED:
			// currently unchecked
			pCmdUI->SetRadio(TRUE);
			break;
		case BST_CHECKED:
			// currently checked
			pCmdUI->SetRadio(FALSE);
			break;
		default:
			ASSERT(FALSE);
			pCmdUI->Enable(FALSE);
			break;
		}
	}
}

void CWPhastDoc::OnMediaZonesHideAll()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetMediaCheck(BST_UNCHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateMediaZonesShowSelected(CCmdUI *pCmdUI)
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		switch (pTree->GetMediaCheck())
		{
		case BST_UNCHECKED:
			// currently unchecked
			pCmdUI->SetRadio(FALSE);
			break;
		case BST_CHECKED:
			// currently checked
			pCmdUI->SetRadio(TRUE);
			break;
		default:
			ASSERT(FALSE);
			pCmdUI->Enable(FALSE);
			break;
		}
	}
}

void CWPhastDoc::OnMediaZonesShowSelected()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetMediaCheck(BST_CHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnMediaZonesSelectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode nodeMedia = pTree->GetMediaNode();
		int nCount = nodeMedia.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(nodeMedia.GetChildAt(i), BST_CHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnMediaZonesUnselectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode nodeMedia = pTree->GetMediaNode();
		int nCount = nodeMedia.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(nodeMedia.GetChildAt(i), BST_UNCHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateICZonesHideAll(CCmdUI *pCmdUI)
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		switch (pTree->GetICCheck())
		{
		case BST_UNCHECKED:
			// currently unchecked
			pCmdUI->SetRadio(TRUE);
			break;
		case BST_CHECKED:
			// currently checked
			pCmdUI->SetRadio(FALSE);
			break;
		default:
			ASSERT(FALSE);
			pCmdUI->Enable(FALSE);
			break;
		}
	}
}

void CWPhastDoc::OnICZonesHideAll()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetICCheck(BST_UNCHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateBCZonesHideAll(CCmdUI *pCmdUI)
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		switch (pTree->GetBCCheck())
		{
		case BST_UNCHECKED:
			// currently unchecked
			pCmdUI->SetRadio(TRUE);
			break;
		case BST_CHECKED:
			// currently checked
			pCmdUI->SetRadio(FALSE);
			break;
		default:
			ASSERT(FALSE);
			pCmdUI->Enable(FALSE);
			break;
		}
	}
}

void CWPhastDoc::OnBCZonesHideAll()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetBCCheck(BST_UNCHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateICZonesShowSelected(CCmdUI *pCmdUI)
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		switch (pTree->GetICCheck())
		{
		case BST_UNCHECKED:
			// currently unchecked
			pCmdUI->SetRadio(FALSE);
			break;
		case BST_CHECKED:
			// currently checked
			pCmdUI->SetRadio(TRUE);
			break;
		default:
			ASSERT(FALSE);
			pCmdUI->Enable(FALSE);
			break;
		}
	}
}

void CWPhastDoc::OnICZonesShowSelected()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetICCheck(BST_CHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateBCZonesShowSelected(CCmdUI *pCmdUI)
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		switch (pTree->GetBCCheck())
		{
		case BST_UNCHECKED:
			// currently unchecked
			pCmdUI->SetRadio(FALSE);
			break;
		case BST_CHECKED:
			// currently checked
			pCmdUI->SetRadio(TRUE);
			break;
		default:
			ASSERT(FALSE);
			pCmdUI->Enable(FALSE);
			break;
		}
	}
}

void CWPhastDoc::OnBCZonesShowSelected()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetBCCheck(BST_CHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnICZonesSelectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		int nCount;

		// head
		CTreeCtrlNode nodeICHead = pTree->GetICHeadNode();
		nCount = nodeICHead.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(nodeICHead.GetChildAt(i), BST_CHECKED);
		}

		// chem
		CTreeCtrlNode nodeICChem = pTree->GetICChemNode();
		nCount = nodeICChem.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(nodeICChem.GetChildAt(i), BST_CHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnICZonesUnselectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		int nCount;

		// head
		CTreeCtrlNode nodeICHead = pTree->GetICHeadNode();
		nCount = nodeICHead.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(nodeICHead.GetChildAt(i), BST_UNCHECKED);
		}

		// chem
		CTreeCtrlNode nodeICChem = pTree->GetICChemNode();
		nCount = nodeICChem.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(nodeICChem.GetChildAt(i), BST_UNCHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnBCZonesSelectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode node = pTree->GetBCNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_CHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnBCZonesUnselectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode node = pTree->GetBCNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_UNCHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::ClearSelection(void)
{
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) this->GetNextView(pos);
		ASSERT_VALID(pView);
		vtkRenderWindowInteractor *iren = pView->GetInteractor();

		// clear selected river points
		//
		vtkPropCollection *pRiverPropCollection = this->GetPropAssemblyRivers()->GetParts();
		vtkCollectionSimpleIterator rcsi;
		pRiverPropCollection->InitTraversal(rcsi);
		vtkProp* pRiverProp = pRiverPropCollection->GetNextProp(rcsi);
		for (; pRiverProp; pRiverProp = pRiverPropCollection->GetNextProp(rcsi))
		{
			if (CRiverActor *pActor = CRiverActor::SafeDownCast(pRiverProp))
			{
				pActor->ClearSelection();
				if (pActor->GetInteractor())
				{
					pActor->SetInteractor(iren);
					pActor->SetEnabled(0);
				}
			}
		}

		// clear selected drain points  -- TODO river and drain move to Notify()
		//
		vtkPropCollection *pDrainPropCollection = this->GetPropAssemblyDrains()->GetParts();
		vtkCollectionSimpleIterator dcsi;
		pDrainPropCollection->InitTraversal(dcsi);
		vtkProp* pDrainProp = pDrainPropCollection->GetNextProp(dcsi);
		for (; pDrainProp; pDrainProp = pDrainPropCollection->GetNextProp(dcsi))
		{
			if (CDrainActor *pActor = CDrainActor::SafeDownCast(pDrainProp))
			{
				pActor->ClearSelection();
				if (pActor->GetInteractor())
				{
					pActor->SetInteractor(iren);
					pActor->SetEnabled(0);
				}
			}
		}
	}

	// Notify
	//
	this->Notify(0, WPN_SELCHANGED, 0, 0);
}

void CWPhastDoc::Add(CZoneActor *pZoneActor, HTREEITEM hInsertAfter)
{
	this->InternalAdd(pZoneActor, true, hInsertAfter);
}

void CWPhastDoc::UnAdd(CZoneActor *pZoneActor)
{
	this->InternalDelete(pZoneActor, false);
}

void CWPhastDoc::Delete(CZoneActor *pZoneActor)
{
	this->InternalDelete(pZoneActor, true);
}

void CWPhastDoc::UnDelete(CZoneActor *pZoneActor)
{
	this->InternalAdd(pZoneActor, false);
}

void CWPhastDoc::InternalAdd(CZoneActor *pZoneActor, bool bAdd, HTREEITEM hInsertAfter)
{
	ASSERT(pZoneActor);
	if (pZoneActor == NULL) return;

	// set scale
	//
	double *scale = this->GetScale();
	pZoneActor->SetScale(scale[0], scale[1], scale[2]);

	// add to document
	//
	pZoneActor->Add(this);

	if (!bAdd)
	{
		// see InternalDelete
		//
		ASSERT(this->m_pRemovedPropCollection);
		ASSERT(this->m_pRemovedPropCollection->IsItemPresent(pZoneActor));
		this->m_pRemovedPropCollection->RemoveItem(pZoneActor);
	}

	// add to property tree
	//
	if (CPropertyTreeControlBar *pPropertyTreeControlBar = this->GetPropertyTreeControlBar())
	{
		if (bAdd)
		{
			pPropertyTreeControlBar->AddZone(pZoneActor, hInsertAfter);
		}
		else
		{
			pZoneActor->UnRemove(pPropertyTreeControlBar);
		}
	}

	if (bAdd)
	{
		this->Notify(0, WPN_SELCHANGED, 0, pZoneActor);
	}

	// render
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::InternalDelete(CZoneActor *pZoneActor, bool bDelete)
{
	ASSERT(pZoneActor);
	if (pZoneActor == NULL) return;

	if (bDelete)
	{
		// make sure pZoneActor ref count doesn't go to zero
		//
		ASSERT(this->m_pRemovedPropCollection);
		ASSERT(!this->m_pRemovedPropCollection->IsItemPresent(pZoneActor));
		this->m_pRemovedPropCollection->AddItem(pZoneActor);
	}

	// remove from document
	//
	pZoneActor->Remove(this);

	// foreach view
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView *pView = this->GetNextView(pos);
		if (CWPhastView *pWPhastView = static_cast<CWPhastView*>(pView))
		{
			ASSERT_KINDOF(CWPhastView, pWPhastView);
			ASSERT_VALID(pWPhastView);

			// clear selection bounding box
			//
			pWPhastView->ClearSelection();

			// Update BoxPropertiesDialogBar
			//
			if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) )
			{
				pBar->Set(pWPhastView, NULL, this->GetUnits());
			}
		}
	}

	// remove from property tree
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->RemoveZone(pZoneActor);
	}

	// render
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::Add(CWellActor *pWellActor, HTREEITEM hInsertAfter)
{
	ASSERT(pWellActor);
	if (!pWellActor) return;
	ASSERT(pWellActor->GetPickable());

	// set scale
	//
	double *scale = this->GetScale();
	pWellActor->SetScale(scale[0], scale[1], scale[2]);

	// set height
	//
	CGrid xyz[3];
	this->GetGrid(xyz[0], xyz[1], xyz[2]);
	pWellActor->SetZAxis(xyz[2], this->GetUnits());

	// set radius
	//
	double *bounds = this->GetGridBounds();
	float defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
	pWellActor->SetDefaultTubeDiameter(defaultAxesSize * 0.17 / sqrt(scale[0] * scale[1]));

	// add to well assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyWells())
	{
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(pWellActor));
		pPropAssembly->AddPart(pWellActor);
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(pWellActor));
		this->AddPropAssembly(pPropAssembly);
	}

	// add to property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pWellActor->Add(pTree, hInsertAfter);
	}

	// render
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::UnAdd(CWellActor *pWellActor)
{
	ASSERT(pWellActor);
	if (!pWellActor) return;

	// remove from wells assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyWells())
	{
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(pWellActor));
		pPropAssembly->RemovePart(pWellActor);
		// VTK HACK
		// This is req'd because ReleaseGraphicsResources is not called when
		// vtkPropAssembly::RemovePart(vtkProp *prop) is called
		this->ReleaseGraphicsResources(pWellActor);
	}

	// remove from property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pWellActor->UnAdd(pTree);
	}

	// render
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::Remove(CWellActor *pWellActor)
{
	ASSERT(pWellActor && pWellActor->IsA("CWellActor"));
	if (pWellActor == 0) return;

	// validate actor
	//
	double *scale = this->GetScale();
	double *wellscale = pWellActor->GetScale();
	ASSERT(wellscale[0] == scale[0]);
	ASSERT(wellscale[1] == scale[1]);
	ASSERT(wellscale[2] == scale[2]);

	// make sure pWellActor ref count doesn't go to zero
	//
	ASSERT(this->m_pRemovedPropCollection);
	ASSERT(!this->m_pRemovedPropCollection->IsItemPresent(pWellActor));
	this->m_pRemovedPropCollection->AddItem(pWellActor);

	// remove from wells assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyWells())
	{
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(pWellActor));
		pPropAssembly->RemovePart(pWellActor);
		// VTK HACK
		// This is req'd because ReleaseGraphicsResources is not called when
		// vtkPropAssembly::RemovePart(vtkProp *prop) is called
		this->ReleaseGraphicsResources(pWellActor);
	}

	// remove from property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pWellActor->Remove(pTree);
	}

	// render scene
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::UnRemove(CWellActor *pWellActor)
{
	ASSERT(pWellActor && pWellActor->IsA("CWellActor"));
	if (pWellActor == 0) return;

	// validate pWellActor
	//
	ASSERT(pWellActor->GetReferenceCount() > 0);

	// verify scale
	//
	double *scale = this->GetScale();
	double *wellscale = pWellActor->GetScale();
	ASSERT(wellscale[0] == scale[0]);
	ASSERT(wellscale[1] == scale[1]);
	ASSERT(wellscale[2] == scale[2]);

	// add to wells assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyWells())
	{
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(pWellActor));
		pPropAssembly->AddPart(pWellActor);
		if (!this->GetPropCollection()->IsItemPresent(pPropAssembly))
		{
			this->GetPropCollection()->AddItem(pPropAssembly);
			ASSERT(pWellActor->GetReferenceCount() > 1);
		}
	}

	// see CWPhastDoc::Delete
	//
	ASSERT(this->m_pRemovedPropCollection);
	ASSERT(this->m_pRemovedPropCollection->IsItemPresent(pWellActor));
	this->m_pRemovedPropCollection->RemoveItem(pWellActor);
	ASSERT(pWellActor->GetReferenceCount() > 0);


	// add to property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pWellActor->UnRemove(pTree);
	}

	// render scene
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::Select(CZoneActor *pZoneActor)
{
	TRACE("%s, in\n", __FUNCTION__);
	ASSERT(pZoneActor && pZoneActor->IsA("CZoneActor"));
	this->Notify(0, WPN_SELCHANGED, 0, pZoneActor);
	TRACE("%s, out\n", __FUNCTION__);
}

void CWPhastDoc::Select(CWellActor *pWellActor)
{
	ASSERT(pWellActor && pWellActor->IsA("CWellActor"));
	this->Notify(0, WPN_SELCHANGED, 0, pWellActor);
}

void CWPhastDoc::Select(CRiverActor *pRiverActor)
{
	ASSERT(pRiverActor && pRiverActor->IsA("CRiverActor"));
	this->Notify(0, WPN_SELCHANGED, 0, pRiverActor);
}

void CWPhastDoc::Select(CDrainActor *pDrainActor)
{
	ASSERT(pDrainActor && pDrainActor->IsA("CDrainActor"));
	this->Notify(0, WPN_SELCHANGED, 0, pDrainActor);
}

void CWPhastDoc::GetGrid(CGrid& x, CGrid& y, CGrid& z)const
{
	this->m_pGridActor->GetGrid(x, y, z);
}

void CWPhastDoc::OnUpdateWellsHideAll(CCmdUI *pCmdUI)
{
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyWells())
	{
		if (pPropAssembly->GetVisibility())
		{
			pCmdUI->SetRadio(FALSE);
		}
		else
		{
			pCmdUI->SetRadio(TRUE);
		}
	}
}

void CWPhastDoc::OnWellsHideAll()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetNodeCheck(pTree->GetWellsNode(), BST_UNCHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateWellsShowSelected(CCmdUI *pCmdUI)
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		switch (pTree->GetNodeCheck(pTree->GetWellsNode()))
		{
		case BST_UNCHECKED:
			// currently unchecked
			pCmdUI->SetRadio(FALSE);
			break;
		case BST_CHECKED:
			// currently checked
			pCmdUI->SetRadio(TRUE);
			break;
		default:
			ASSERT(FALSE);
			pCmdUI->Enable(FALSE);
			break;
		}
	}
}

void CWPhastDoc::OnWellsShowSelected()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetNodeCheck(pTree->GetWellsNode(), BST_CHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnWellsSelectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode node = pTree->GetWellsNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_CHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnWellsUnselectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode node = pTree->GetWellsNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_UNCHECKED);
		}
	}
	this->UpdateAllViews(0);
}

VARIANT CWPhastDoc::GetVisible(void)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	VARIANT vaResult;
	VariantInit(&vaResult);
	vaResult.vt = VT_BOOL;

	if (::IsWindowVisible(::AfxGetApp()->m_pMainWnd->GetSafeHwnd()))
	{
		vaResult.boolVal = VARIANT_TRUE;
	}
	else
	{
		vaResult.boolVal = VARIANT_FALSE;
	}
	return vaResult;
}

//void CWPhastDoc::SetVis(VARIANT newVal)
void CWPhastDoc::SetVisible(const VARIANT& newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// Add your property handler code here
#if defined(_DEBUG)
	afxDump << (COleVariant)newVal;
	ASSERT(newVal.vt == VT_BOOL);
#endif

	if (newVal.boolVal)
	{
		::AfxGetMainWnd()->ShowWindow(SW_SHOW);
	}
	else
	{
		::AfxGetMainWnd()->ShowWindow(SW_HIDE);
	}
}

VARIANT CWPhastDoc::SaveAs(const VARIANT& FileName)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	VARIANT vaResult;
	VariantInit(&vaResult);
	vaResult.vt      = VT_BOOL;
	vaResult.boolVal = VARIANT_FALSE;

#if defined(_DEBUG)
	afxDump << (COleVariant)FileName;
	ASSERT(FileName.vt == VT_BSTR);
#endif

	CString name(FileName.bstrVal);
	if (this->OnSaveDocument(name))
	{
		vaResult.boolVal = VARIANT_TRUE;
	}

	// Add your dispatch handler code here

	return vaResult;
}

VARIANT CWPhastDoc::Import(const VARIANT& FileName)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	VARIANT vaResult;
	VariantInit(&vaResult);
	vaResult.vt      = VT_BOOL;
	vaResult.boolVal = VARIANT_FALSE;

#if defined(_DEBUG)
	afxDump << (COleVariant)FileName;
	ASSERT(FileName.vt == VT_BSTR);
#endif

	CString newName(FileName.bstrVal);

	// modified from: CDocument* CDocManager::OpenDocumentFile
	//
	TCHAR szPath[_MAX_PATH];
	ASSERT(lstrlen(newName) < sizeof(szPath)/sizeof(szPath[0]));
	TCHAR szTemp[_MAX_PATH];
	if (newName[0] == '\"')
		newName = newName.Mid(1);
	lstrcpyn(szTemp, newName, _MAX_PATH);
	LPTSTR lpszLast = _tcsrchr(szTemp, '\"');
	if (lpszLast != NULL)
		*lpszLast = 0;
	SrcFullPath(szPath, szTemp);
	TCHAR szLinkName[_MAX_PATH];
	if (SrcResolveShortcut(AfxGetMainWnd(), szPath, szLinkName, _MAX_PATH))
		lstrcpy(szPath, szLinkName);

	//
	// avoid SaveAs Dialog
	this->SetModifiedFlag(FALSE);

	CDocument* pDoc = this->GetDocTemplate()->OpenDocumentFile(NULL, FALSE);
	if (pDoc)
	{
		ASSERT(pDoc == this);
		if (this->DoImport(szPath))
		{
			vaResult.boolVal = VARIANT_TRUE;
			this->SetModifiedFlag(FALSE);
		}
		if (::AfxGetMainWnd()->IsWindowVisible())
		{
			this->ExecutePipeline();
		}
	}

	return vaResult;
}

VARIANT CWPhastDoc::Run(void)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	VARIANT vaResult;
	VariantInit(&vaResult);

	///OnFileRun();
	::AfxMessageBox("This method has not been implemented yet");

	return vaResult;
}

void CWPhastDoc::Add(CRiverActor *pRiverActor, HTREEITEM hInsertAfter)
{
	ASSERT(pRiverActor);
	if (!pRiverActor) return;

	// set scale
	//
	double *scale = this->GetScale();
	pRiverActor->SetScale(scale[0], scale[1], scale[2]);

	// set radius
	//
	pRiverActor->ScaleFromBounds(this->GetGridBounds());

	// set z
	//
	CGrid x, y, z;
	this->GetGrid(x, y, z);
	z.Setup();
	pRiverActor->SetZ(z.coord[z.count_coord - 1]);

	// for all views disable interaction
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		pRiverActor->SetInteractor(pView->GetInteractor());
		pRiverActor->SetEnabled(0);
	}

	// add to river assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyRivers())
	{
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(pRiverActor));
		pPropAssembly->AddPart(pRiverActor);
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(pRiverActor));
		this->AddPropAssembly(pPropAssembly);
	}

	// add to property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pRiverActor->Add(pTree, hInsertAfter);
	}

	// add listeners
	//
	ASSERT(this->RiverCallbackCommand);
	pRiverActor->AddObserver(CRiverActor::StartMovePointEvent, this->RiverCallbackCommand);
	pRiverActor->AddObserver(CRiverActor::MovingPointEvent,    this->RiverCallbackCommand);
	pRiverActor->AddObserver(CRiverActor::EndMovePointEvent,   this->RiverCallbackCommand);
	pRiverActor->AddObserver(CRiverActor::InsertPointEvent,    this->RiverCallbackCommand);

	// render
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::UnAdd(CRiverActor *pRiverActor)
{
	ASSERT(pRiverActor);
	if (!pRiverActor) return;

	// remove from rivers assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyRivers())
	{
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(pRiverActor));
		pPropAssembly->RemovePart(pRiverActor);
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(pRiverActor));
// COMMENT: {9/23/2009 8:24:23 PM}		// VTK HACK
// COMMENT: {9/23/2009 8:24:23 PM}		// This is req'd because ReleaseGraphicsResources is not called when
// COMMENT: {9/23/2009 8:24:23 PM}		// vtkPropAssembly::RemovePart(vtkProp *prop) is called
// COMMENT: {9/23/2009 8:24:23 PM}		this->ReleaseGraphicsResources(pRiverActor);
		//{{
		pRiverActor->SetInteractor(0);
		//}}
	}

	// remove from property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pRiverActor->UnAdd(pTree);
	}

	// remove listeners
	//
	ASSERT(this->RiverCallbackCommand);
	pRiverActor->RemoveObserver(this->RiverCallbackCommand);

	// render
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::Remove(CRiverActor *pRiverActor)
{
	ASSERT(pRiverActor && pRiverActor->IsA("CRiverActor"));
	if (pRiverActor == 0) return;

	// validate actor
	//
	double *scale = this->GetScale();
	double *riverscale = pRiverActor->GetScale();
// COMMENT: {7/20/2009 8:33:12 PM}	ASSERT(riverscale[0] == scale[0]);
// COMMENT: {7/20/2009 8:33:12 PM}	ASSERT(riverscale[1] == scale[1]);
// COMMENT: {7/20/2009 8:33:12 PM}	ASSERT(riverscale[2] == scale[2]);

	// make sure pRiverActor ref count doesn't go to zero
	//
	ASSERT(this->m_pRemovedPropCollection);
	ASSERT(!this->m_pRemovedPropCollection->IsItemPresent(pRiverActor));
	this->m_pRemovedPropCollection->AddItem(pRiverActor);

	// remove from rivers assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyRivers())
	{
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(pRiverActor));
		pPropAssembly->RemovePart(pRiverActor);
		// VTK HACK
		// This is req'd because ReleaseGraphicsResources is not called when
		// vtkPropAssembly::RemovePart(vtkProp *prop) is called
		this->ReleaseGraphicsResources(pRiverActor);
	}

	// remove from property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pRiverActor->Remove(pTree);
	}

	// render scene
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::UnRemove(CRiverActor *pRiverActor)
{
	ASSERT(pRiverActor && pRiverActor->IsA("CRiverActor"));
	if (pRiverActor == 0) return;

	// validate pWellActor
	//
	ASSERT(pRiverActor->GetReferenceCount() > 0);

	// verify scale
	//
	double *scale = this->GetScale();
	double *riverscale = pRiverActor->GetScale();
// COMMENT: {7/20/2009 8:33:36 PM}	ASSERT(riverscale[0] == scale[0]);
// COMMENT: {7/20/2009 8:33:36 PM}	ASSERT(riverscale[1] == scale[1]);
// COMMENT: {7/20/2009 8:33:36 PM}	ASSERT(riverscale[2] == scale[2]);

	// add to rivers assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyRivers())
	{
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(pRiverActor));
		pPropAssembly->AddPart(pRiverActor);
		if (!this->GetPropCollection()->IsItemPresent(pPropAssembly))
		{
			this->GetPropCollection()->AddItem(pPropAssembly);
			ASSERT(pRiverActor->GetReferenceCount() > 1);
		}
	}

	// see CWPhastDoc::Delete
	//
	ASSERT(this->m_pRemovedPropCollection);
	ASSERT(this->m_pRemovedPropCollection->IsItemPresent(pRiverActor));
	this->m_pRemovedPropCollection->RemoveItem(pRiverActor);
	ASSERT(pRiverActor->GetReferenceCount() > 0);


	// add to property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pRiverActor->UnRemove(pTree);
	}

	// render scene
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::Remove(CDrainActor *pDrainActor)
{
	ASSERT(pDrainActor && pDrainActor->IsA("CDrainActor"));
	if (pDrainActor == 0) return;

	// validate actor
	//
	double *scale = this->GetScale();
	double *riverscale = pDrainActor->GetScale();
// COMMENT: {7/20/2009 8:34:20 PM}	ASSERT(riverscale[0] == scale[0]);
// COMMENT: {7/20/2009 8:34:20 PM}	ASSERT(riverscale[1] == scale[1]);
// COMMENT: {7/20/2009 8:34:20 PM}	ASSERT(riverscale[2] == scale[2]);

	// make sure pDrainActor ref count doesn't go to zero
	//
	ASSERT(this->m_pRemovedPropCollection);
	ASSERT(!this->m_pRemovedPropCollection->IsItemPresent(pDrainActor));
	this->m_pRemovedPropCollection->AddItem(pDrainActor);

	// remove from rivers assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyDrains())
	{
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(pDrainActor));
		pPropAssembly->RemovePart(pDrainActor);
		// VTK HACK
		// This is req'd because ReleaseGraphicsResources is not called when
		// vtkPropAssembly::RemovePart(vtkProp *prop) is called
		this->ReleaseGraphicsResources(pDrainActor);
	}

	// remove from property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pDrainActor->Remove(pTree);
	}

	// render scene
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::UnRemove(CDrainActor *pDrainActor)
{
	ASSERT(pDrainActor && pDrainActor->IsA("CDrainActor"));
	if (pDrainActor == 0) return;

	// validate pDrainActor
	//
	ASSERT(pDrainActor->GetReferenceCount() > 0);

	// verify scale
	//
	double *scale = this->GetScale();
	double *riverscale = pDrainActor->GetScale();
// COMMENT: {7/20/2009 8:34:29 PM}	ASSERT(riverscale[0] == scale[0]);
// COMMENT: {7/20/2009 8:34:29 PM}	ASSERT(riverscale[1] == scale[1]);
// COMMENT: {7/20/2009 8:34:29 PM}	ASSERT(riverscale[2] == scale[2]);

	// add to drains assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyDrains())
	{
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(pDrainActor));
		pPropAssembly->AddPart(pDrainActor);
		if (!this->GetPropCollection()->IsItemPresent(pPropAssembly))
		{
			this->GetPropCollection()->AddItem(pPropAssembly);
			ASSERT(pDrainActor->GetReferenceCount() > 1);
		}
	}

	// see CWPhastDoc::Delete
	//
	ASSERT(this->m_pRemovedPropCollection);
	ASSERT(this->m_pRemovedPropCollection->IsItemPresent(pDrainActor));
	this->m_pRemovedPropCollection->RemoveItem(pDrainActor);
	ASSERT(pDrainActor->GetReferenceCount() > 0);


	// add to property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pDrainActor->UnRemove(pTree);
	}

	// render scene
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::RiverListener(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
	ASSERT(caller->IsA("CRiverActor"));
	ASSERT(clientdata);

	if (CRiverActor* river = CRiverActor::SafeDownCast(caller))
	{
		CWPhastDoc* self = reinterpret_cast<CWPhastDoc*>(clientdata);

		switch (eid)
		{
		case CRiverActor::StartMovePointEvent:
			ASSERT(self->RiverMovePointAction == 0);
			self->RiverMovePointAction = new CPointConnectorMovePointAction<CRiverActor>(river, self, river->GetCurrentPointId(), river->GetCurrentPointPosition()[0], river->GetCurrentPointPosition()[1]);
			break;

		case CRiverActor::MovingPointEvent:
			ASSERT(self->RiverMovePointAction != 0);
			if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
			{
				// update message bar
				//
				static TCHAR buffer[80];
				const CUnits& units = self->GetUnits();
				::_sntprintf(buffer, 80, "%g[%s] x %g[%s]",
					self->RiverMovePointAction->GetActor()->GetCurrentPointPosition()[0] / units.horizontal.input_to_si,
					units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
					self->RiverMovePointAction->GetActor()->GetCurrentPointPosition()[1] / units.horizontal.input_to_si,
					units.horizontal.defined ? units.horizontal.input : units.horizontal.si
					);
				pWnd->SetWindowText(buffer);
			}
			break;

		case CRiverActor::EndMovePointEvent:
			ASSERT(self->RiverMovePointAction != 0);
			self->RiverMovePointAction->SetPoint(river->GetCurrentPointPosition()[0], river->GetCurrentPointPosition()[1]);
			self->Execute(self->RiverMovePointAction);
			self->RiverMovePointAction = 0;
			break;

		case CRiverActor::InsertPointEvent:
			{
				double* pos = river->GetCurrentPointPosition();
				vtkIdType id = river->GetCurrentPointId() - 1;
				CPointConnectorInsertPointAction<CRiverActor, CRiverPoint>* pRiverInsertPointAction =
					new CPointConnectorInsertPointAction<CRiverActor, CRiverPoint>(river, self, id, pos[0], pos[1], pos[2], true);
				self->Execute(pRiverInsertPointAction);
			}
			break;

		default:
			ASSERT(FALSE);
			break;
		}
	}
}

void CWPhastDoc::GetGridKeyword(CGridKeyword& gridKeyword)const
{
	this->m_pGridActor->GetGridKeyword(gridKeyword);
}

CGridKeyword CWPhastDoc::GetGridKeyword(void)const
{
	CGridKeyword gridKeyword;
	this->m_pGridActor->GetGridKeyword(gridKeyword);
	return gridKeyword;
}

void CWPhastDoc::SetGridKeyword(const CGridKeyword& gridKeyword)
{
	this->m_pGridActor->SetGridKeyword(gridKeyword, this->GetUnits());
// COMMENT: {8/19/2009 4:35:52 PM}	this->ResizeGrid(gridKeyword.m_grid[0], gridKeyword.m_grid[1], gridKeyword.m_grid[2]);
	this->ResizeGrid(gridKeyword);

	// set grid for all zones
	//
	if (vtkPropCollection* pCollection = this->GetPropCollection())
	{
		vtkCollectionSimpleIterator csi;
		pCollection->InitTraversal(csi);
		for (int i = 0; i < pCollection->GetNumberOfItems(); ++i)
		{
			vtkProp* prop = pCollection->GetNextProp(csi);
			if (vtkPropAssembly *pPropAssembly = vtkPropAssembly::SafeDownCast(prop))
			{
				if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
				{
					vtkProp *pProp;
					vtkCollectionSimpleIterator pcsi;
					pPropCollection->InitTraversal(pcsi);
					for (; (pProp = pPropCollection->GetNextProp(pcsi)); )
					{
						if (vtkProp3D *prop3D = vtkProp3D::SafeDownCast(pProp))
						{
							if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop3D))
							{
// COMMENT: {12/30/2008 5:39:12 PM}								pZone->SetUnits(units);
							}
							if (CWellActor *pWellActor = CWellActor::SafeDownCast(prop3D))
							{
								pWellActor->SetGridKeyword(gridKeyword);
							}
							if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(prop3D))
							{
// COMMENT: {12/30/2008 5:39:17 PM}								pRiverActor->SetUnits(units);
							}
						}
					}
				}
			}
		}
	}
}

void CWPhastDoc::Edit(CGridActor* pGridActor)
{
	ASSERT(pGridActor == this->m_pGridActor);
	if (!this->m_pGridSheet)
	{
		this->m_pGridSheet = new CModelessPropertySheet("Grid");
		///this->m_pGridSheet = new ETSLayoutModelessPropertySheet("Time Control", ::AfxGetMainWnd());
		///((ETSLayoutPropertySheet*)this->m_pGridSheet)->ModelessWithButtons();

	}
	if (!this->m_pGridPage)
	{
		this->m_pGridPage  = new CGridPropertyPage2();
		this->m_pGridSheet->AddPage(this->m_pGridPage);
	}

	CGridKeyword gridKeyword;
	pGridActor->GetGridKeyword(gridKeyword);
	this->m_pGridPage->SetProperties(gridKeyword);
	this->m_pGridPage->SetUnits(this->GetUnits());

	this->m_pGridPage->m_pDoc = this;
	this->m_pGridPage->m_pActor = pGridActor;

	this->m_pGridSheet->Create(::AfxGetApp()->m_pMainWnd);
}

void CWPhastDoc::ModifyGrid(CGridActor* gridActor, CGridElementsSelector* gridElementsSelector)
{
	ASSERT(gridActor == this->m_pGridActor);

	if (this->ModifyGridSheet)
	{
		this->ModifyGridSheet->DestroyWindow();
		delete this->ModifyGridSheet;
		this->ModifyGridSheet = 0;
	}
	if (this->GridRefinePage)
	{
		this->GridRefinePage->DestroyWindow();
		delete this->GridRefinePage;
		this->GridRefinePage = 0;
	}
	if (this->GridCoarsenPage)
	{
		this->GridCoarsenPage->DestroyWindow();
		delete this->GridCoarsenPage;
		this->GridCoarsenPage = 0;
	}

	this->ModifyGridSheet = new CModelessPropertySheet("");
	this->GridRefinePage  = new CGridRefinePage();
	this->GridCoarsenPage = new CGridCoarsenPage();
	this->ModifyGridSheet->AddPage(this->GridRefinePage);
	this->ModifyGridSheet->AddPage(this->GridCoarsenPage);

	CGridKeyword gridKeyword;
	gridActor->GetGridKeyword(gridKeyword);

	this->GridRefinePage->SetProperties(gridKeyword);
	this->GridRefinePage->SetUnits(this->GetUnits());
	this->GridRefinePage->SetDocument(this);
	this->GridRefinePage->SetActor(gridActor);
	this->GridRefinePage->SetWidget(gridElementsSelector);

	this->GridCoarsenPage->SetProperties(gridKeyword);
	this->GridCoarsenPage->SetUnits(this->GetUnits());
	this->GridCoarsenPage->SetDocument(this);
	this->GridCoarsenPage->SetActor(gridActor);
	this->GridCoarsenPage->SetWidget(gridElementsSelector);

	int min[3];
	gridElementsSelector->GetMin(min);
	this->GridRefinePage->SetMin(min);
	this->GridCoarsenPage->SetMin(min);

	int max[3];
	gridElementsSelector->GetMax(max);
	this->GridRefinePage->SetMax(max);
	this->GridCoarsenPage->SetMax(max);

	this->ModifyGridSheet->Create(::AfxGetApp()->m_pMainWnd);
}

void CWPhastDoc::Update(IObserver* pSender, LPARAM lHint, CObject* pHint, vtkObject* pObject)
{
}

vtkProp3D* CWPhastDoc::GetGridActor(void)
{
	return this->m_pGridActor;
}

void CWPhastDoc::GridListener(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
	ASSERT(caller->IsA("CGridActor"));
	ASSERT(clientdata);

	if (CGridActor* grid = CGridActor::SafeDownCast(caller))
	{
		CWPhastDoc* self = reinterpret_cast<CWPhastDoc*>(clientdata);
		switch (eid)
		{
		case CGridActor::DeleteGridLineEvent:
			{
				int axis =  grid->GetAxisIndex();
				int index =  grid->GetPlaneIndex();
				double value = *(double*)calldata;
				CGridDeleteLineAction* pGridDeleteLineAction = new CGridDeleteLineAction(grid, self, axis, index, value, true);
				self->Execute(pGridDeleteLineAction);
			}
			break;
		case CGridActor::InsertGridLineEvent:
			{
				int axis =  grid->GetAxisIndex();
				int index =  grid->GetPlaneIndex();
				double value = *(double*)calldata;
				CGridInsertLineAction* pGridInsertLineAction = new CGridInsertLineAction(grid, self, axis, index, value, true);
				self->Execute(pGridInsertLineAction);
			}
			break;
		case CGridActor::MoveGridLineEvent:
			{
				GridLineMoveMemento memento = *(GridLineMoveMemento*)calldata;
				CGridMoveLineAction* pGridMoveLineAction = new CGridMoveLineAction(grid, self, memento, true);
				self->Execute(pGridMoveLineAction);
			}
			break;
		}
	}
}

void CWPhastDoc::UpdateGridDomain(void)
{
	// get scale
	//
	double *scale = this->GetScale();

	// get bounds
	//
	double bounds[6];
	this->m_pGridActor->GetBounds(bounds);
	float defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;

	// reset the axes
	//
	this->AxesActor->SetDefaultPositions(bounds);
	this->AxesActor->SetDefaultSize(defaultAxesSize);
	this->AxesActor->SetDefaultTubeDiameter(defaultAxesSize * 0.1);

	// update default zones
	//
	if (vtkPropCollection* pCollection = this->GetPropCollection())
	{
		CZone bounds;
		this->m_pGridActor->GetDefaultZone(bounds);
		vtkCollectionSimpleIterator csi;
		pCollection->InitTraversal(csi);
		for (int i = 0; i < pCollection->GetNumberOfItems(); ++i)
		{
			vtkProp* prop = pCollection->GetNextProp(csi);
			if (vtkPropAssembly *pPropAssembly = vtkPropAssembly::SafeDownCast(prop))
			{
				if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
				{
					vtkCollectionSimpleIterator pcsi;
					pPropCollection->InitTraversal(pcsi);
					for (int i = 0; i < pPropCollection->GetNumberOfItems(); ++i)
					{
						vtkProp* prop = pPropCollection->GetNextProp(pcsi);
						if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop))
						{
							if (dynamic_cast<Domain*>(pZone->GetPolyhedron()) || pZone->GetDefault())
							{
								pZone->SetBounds(bounds, this->GetUnits());
							}
						}
					}
				}
			}
			if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop))
			{
				if (dynamic_cast<Domain*>(pZone->GetPolyhedron()) || pZone->GetDefault())
				{
					pZone->SetBounds(bounds, this->GetUnits());
				}
			}
		}
	}

#ifdef SKIP
	// update rivers
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyRivers())
	{
		if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
		{
			vtkProp *pProp;
			vtkCollectionSimpleIterator csi;
			pPropCollection->InitTraversal(csi);
			for (; (pProp = pPropCollection->GetNextProp(csi)); )
			{
				if (CRiverActor* pRiverActor = CRiverActor::SafeDownCast(pProp))
				{
					// update radius
					//
					pRiverActor->ScaleFromBounds(this->GetGridBounds());
					TRACE("pRiverActor->ScaleFromBounds\n");

					//
					//
					CGrid x, y, z;
					this->GetGrid(x, y, z);
					z.Setup();
					pRiverActor->SetZ(z.coord[z.count_coord - 1]);
				}
			}
		}
	}

	// update wells
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyWells())
	{
		CGrid xyz[3];
		this->GetGrid(xyz[0], xyz[1], xyz[2]);
		if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
		{
			vtkProp *pProp;
			vtkCollectionSimpleIterator csi;
			pPropCollection->InitTraversal(csi);
			for (; (pProp = pPropCollection->GetNextProp(csi)); )
			{
				if (CWellActor* pWellActor = CWellActor::SafeDownCast(pProp))
				{
					// update height
					//
					pWellActor->SetZAxis(xyz[2], this->GetUnits());

					// update radius
					//
// COMMENT: {8/16/2005 6:59:13 PM}					pWellActor->SetDefaultTubeDiameter(defaultAxesSize * 0.17 / sqrt(scale[0] * scale[1]));
					pWellActor->SetDefaultTubeDiameter(defaultAxesSize * 0.17);
					TRACE("pWellActor->SetDefaultTubeDiameter = %g\n", defaultAxesSize * 0.17);
				}
			}
		}
	}
#endif

	// update possible selection
	//
	this->Notify(this, WPN_DOMAIN_CHANGED, 0, 0);

	// refresh screen
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnViewHideAll()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetMediaCheck(BST_UNCHECKED);
		pTree->SetBCCheck(BST_UNCHECKED);
		pTree->SetICCheck(BST_UNCHECKED);
		pTree->SetNodeCheck(pTree->GetWellsNode(), BST_UNCHECKED);
		pTree->SetNodeCheck(pTree->GetRiversNode(), BST_UNCHECKED);
		pTree->SetNodeCheck(pTree->GetDrainsNode(), BST_UNCHECKED);
		pTree->SetNodeCheck(pTree->GetZoneFlowRatesNode(), BST_UNCHECKED);
		pTree->GetGridNode().Select();
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateRiversHideAll(CCmdUI *pCmdUI)
{
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyRivers())
	{
		if (pPropAssembly->GetVisibility())
		{
			pCmdUI->SetRadio(FALSE);
		}
		else
		{
			pCmdUI->SetRadio(TRUE);
		}
	}
}

void CWPhastDoc::OnRiversHideAll()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetNodeCheck(pTree->GetRiversNode(), BST_UNCHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateRiversShowSelected(CCmdUI *pCmdUI)
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		switch (pTree->GetNodeCheck(pTree->GetRiversNode()))
		{
		case BST_UNCHECKED:
			// currently unchecked
			pCmdUI->SetRadio(FALSE);
			break;
		case BST_CHECKED:
			// currently checked
			pCmdUI->SetRadio(TRUE);
			break;
		default:
			ASSERT(FALSE);
			pCmdUI->Enable(FALSE);
			break;
		}
	}
}

void CWPhastDoc::OnRiversShowSelected()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetNodeCheck(pTree->GetRiversNode(), BST_CHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnRiversSelectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode node = pTree->GetRiversNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_CHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnRiversUnselectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode node = pTree->GetRiversNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_UNCHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateToolsModifyGrid(CCmdUI *pCmdUI)
{
	if (CGridActor* pGridActor = CGridActor::SafeDownCast(this->GetGridActor()))
	{
		if (pGridActor->GetVisibility())
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}

	if (this->GridElementsSelector)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CWPhastDoc::OnToolsModifyGrid()
{
	if (this->GridElementsSelector)
	{
		this->EndModifyGrid();
	}
	else
	{
		POSITION pos = this->GetFirstViewPosition();
		if (pos != NULL)
		{
			CWPhastView *pView = (CWPhastView*) GetNextView(pos);
			ASSERT_VALID(pView);
			ASSERT(pView->GetInteractor());
			if (pView->GetInteractor())
			{
				pView->CancelMode();

				this->GridElementsSelector = CGridElementsSelector::New();
				this->GridElementsSelector->SetInteractor(pView->GetInteractor());
				this->GridElementsSelector->SetGridActor(reinterpret_cast<CGridActor *>(this->GetGridActor()));
				this->GridElementsSelector->SetDocument(this);
				this->GridElementsSelector->SetEnabled(1);
			}
		}
	}
}

void CWPhastDoc::EndModifyGrid()
{
	if (this->GridElementsSelector)
	{
		this->GridElementsSelector->SetInteractor(0);
		this->GridElementsSelector->Delete();
		this->GridElementsSelector = 0;
	}
	if (this->ModifyGridSheet && this->ModifyGridSheet->GetSafeHwnd())
	{
		this->ModifyGridSheet->DestroyWindow();
	}
}

CSolutionMethod CWPhastDoc::GetSolutionMethod(void)const
{
	return this->m_pModel->m_solutionMethod;
}

void CWPhastDoc::SetSolutionMethod(const CSolutionMethod &solutionMethod)
{
	this->m_pModel->m_solutionMethod = solutionMethod;
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetSolutionMethod(&this->m_pModel->m_solutionMethod);
	}
}

void CWPhastDoc::OnViewShowAll()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetMediaCheck(BST_CHECKED);
		pTree->SetBCCheck(BST_CHECKED);
		pTree->SetICCheck(BST_CHECKED);
		pTree->SetNodeCheck(pTree->GetWellsNode(), BST_CHECKED);
		pTree->SetNodeCheck(pTree->GetRiversNode(), BST_CHECKED);
		pTree->SetNodeCheck(pTree->GetDrainsNode(), BST_CHECKED);
		pTree->SetNodeCheck(pTree->GetZoneFlowRatesNode(), BST_CHECKED);
		pTree->GetGridNode().Select();
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::SizeHandles(double size)
{
	if (size != size) return;
	TRACE("SizeHandles = %g\n", size);

	// size handles for wells/rivers/drains
	//
	if (vtkPropCollection *pCollection = this->GetPropCollection())
	{
		vtkProp *prop;
		vtkCollectionSimpleIterator c;
		pCollection->InitTraversal(c);
		for (; (prop = pCollection->GetNextProp(c)); )
		{
			if (vtkPropAssembly *pPropAssembly = vtkPropAssembly::SafeDownCast(prop))
			{
				if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
				{
					vtkProp *pProp;
					vtkCollectionSimpleIterator c2;
					pPropCollection->InitTraversal(c2);
					for (; (pProp = pPropCollection->GetNextProp(c2)); )
					{
						if (vtkProp3D *prop3D = vtkProp3D::SafeDownCast(pProp))
						{
							if (CWellActor *pWellActor = CWellActor::SafeDownCast(prop3D))
							{
								pWellActor->SetRadius(0.008 * size);
							}
							if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(prop3D))
							{
								pRiverActor->SetRadius(0.008 * size);
							}
							if (CDrainActor *pDrainActor = CDrainActor::SafeDownCast(prop3D))
							{
								pDrainActor->SetRadius(0.008 * size);
							}
						}
					}
				}
			}
		}
	}
}

void CWPhastDoc::OnUpdateToolsNewZone(CCmdUI *pCmdUI)
{
	if (this->NewZoneWidget && this->NewZoneWidget->GetEnabled())
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CWPhastDoc::OnToolsNewZone()
{
	if (this->NewZoneWidget && this->NewZoneWidget->GetEnabled())
	{
		this->EndNewZone();
	}
	else
	{
		if (this->NewZoneWidget) this->EndNewZone();
		this->BeginNewZone();
	}
}

void CWPhastDoc::BeginNewZone()
{
	ASSERT(this->NewZoneWidget == 0);
	ASSERT(this->NewZoneCallbackCommand == 0);

	POSITION pos = this->GetFirstViewPosition();
	if (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		ASSERT(pView->GetInteractor());
		if (pView->GetInteractor())
		{
			pView->CancelMode();

			// create widget
			this->NewZoneWidget = CNewZoneWidget::New();
			this->NewZoneWidget->SetInteractor(pView->GetInteractor());
			this->NewZoneWidget->SetProp3D(this->GetGridActor());

			// add listener callback
			this->NewZoneCallbackCommand = vtkCallbackCommand::New();
			this->NewZoneCallbackCommand->SetClientData(this);
			this->NewZoneCallbackCommand->SetCallback(CWPhastDoc::NewZoneListener);
			this->NewZoneWidget->AddObserver(vtkCommand::EndInteractionEvent, this->NewZoneCallbackCommand);

			// enable widget
			this->NewZoneWidget->SetEnabled(1);
		}
	}
}

void CWPhastDoc::EndNewZone()
{
	if (this->NewZoneCallbackCommand)
	{
		ASSERT(this->NewZoneCallbackCommand->IsA("vtkObjectBase"));
		this->NewZoneCallbackCommand->Delete();
		this->NewZoneCallbackCommand = 0;
	}
	if (this->NewZoneWidget)
	{
		ASSERT(this->NewZoneWidget->IsA("CNewZoneWidget"));
		this->NewZoneWidget->SetInteractor(0);
		this->NewZoneWidget->Delete();
		this->NewZoneWidget = 0;
	}
}

void CWPhastDoc::NewZoneListener(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
	ASSERT(caller->IsA("CNewZoneWidget"));
	ASSERT(clientdata);

	if (clientdata)
	{
		CWPhastDoc* self = reinterpret_cast<CWPhastDoc*>(clientdata);
		if (eid == vtkCommand::EndInteractionEvent)
		{
			double scaled_meters[6];
			self->NewZoneWidget->GetBounds(scaled_meters);

			// calc zone
			CZone zone;
			double* scale = self->GetScale();
			const CUnits& units = self->GetUnits();
			zone.x1 = scaled_meters[0] / scale[0] / units.horizontal.input_to_si;
			zone.x2 = scaled_meters[1] / scale[0] / units.horizontal.input_to_si;
			zone.y1 = scaled_meters[2] / scale[1] / units.horizontal.input_to_si;
			zone.y2 = scaled_meters[3] / scale[1] / units.horizontal.input_to_si;
			zone.z1 = scaled_meters[4] / scale[2] / units.vertical.input_to_si;
			zone.z2 = scaled_meters[5] / scale[2] / units.vertical.input_to_si;

			TRACE("x(%g-%g) y(%g-%g) z(%g-%g)\n", zone.x1, zone.x2, zone.y1, zone.y2, zone.z1, zone.z2);

			// get type of zone
			//
// COMMENT: {7/13/2009 2:44:44 PM}			ETSLayoutPropertySheet        sheet("Zone Wizard", NULL, 0, NULL, false);
			CPropertySheet                sheet("Zone Wizard", NULL, 0, NULL, false);

			CNewZonePropertyPage          newZone;
			CMediaPropsPage2              mediaProps;
			CFluxPropsPage2               fluxProps;
			CLeakyPropsPage               leakyProps;
			CSpecifiedHeadPropsPage       specifiedProps;
			CHeadICPropsPage2             headProps;
			CICChemPropsPage2             chemICProps;
			CZoneFlowRatePropertyPage     zoneFlowRateProps;

			// CChemICSpreadPropertyPage only needs the flowonly flag when the zone is a
			// default zone
			//
			bool bFlowOnly = self->GetFlowOnly();

			fluxProps.SetFlowOnly(bFlowOnly);
			leakyProps.SetFlowOnly(bFlowOnly);
			specifiedProps.SetFlowOnly(bFlowOnly);

			// set used zone flow rate numbers
			std::set<int> usedZoneFlowRatesNumbers;
			self->GetUsedZoneFlowRates(usedZoneFlowRatesNumbers);
			zoneFlowRateProps.SetUsedZoneFlowRates(usedZoneFlowRatesNumbers);

			sheet.AddPage(&newZone);
			sheet.AddPage(&mediaProps);
			sheet.AddPage(&fluxProps);
			sheet.AddPage(&leakyProps);
			sheet.AddPage(&specifiedProps);
			sheet.AddPage(&headProps);
			sheet.AddPage(&chemICProps);
			sheet.AddPage(&zoneFlowRateProps);

			sheet.SetWizardMode();

			if (sheet.DoModal() == ID_WIZFINISH)
			{
				if (newZone.GetType() == ID_ZONE_TYPE_MEDIA)
				{
					CGridElt elt;
					mediaProps.GetProperties(elt);
					elt.polyh = new Cube(&zone);
					CMediaZoneActor::Create(self, elt, mediaProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_FLUX)
				{
					CBC bc;
					fluxProps.GetProperties(bc);
					bc.polyh = new Cube(&zone);
					CBCZoneActor::Create(self, bc, fluxProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_LEAKY)
				{
					CBC bc;
					leakyProps.GetProperties(bc);
					bc.polyh = new Cube(&zone);
					CBCZoneActor::Create(self, bc, leakyProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_SPECIFIED)
				{
					CBC bc;
					specifiedProps.GetProperties(bc);
					bc.polyh = new Cube(&zone);
					CBCZoneActor::Create(self, bc, specifiedProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_IC_HEAD)
				{
					CHeadIC headic;
					headProps.GetProperties(headic);
					headic.polyh = new Cube(&zone);
					CICHeadZoneActor::Create(self, headic, headProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_IC_CHEM)
				{
					CChemIC chemIC;
					chemICProps.GetProperties(chemIC);
					chemIC.polyh = new Cube(&zone);
					CICChemZoneActor::Create(self, chemIC, chemICProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_FLOW_RATE)
				{
					Zone_budget zone_budget;
					zoneFlowRateProps.GetProperties(zone_budget);
					zone_budget.Set_polyh(new Cube(&zone));
					CZoneFlowRateZoneActor::Create(self, zone_budget);
				}
			}

			// Note: cannot call EndNewZone here
			self->NewZoneWidget->SetInteractor(0);
		}
	}

}

BOOL CWPhastDoc::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (this->NewZoneWidget && this->NewZoneWidget->GetEnabled())
	{
		return this->NewZoneWidget->OnSetCursor(pWnd, nHitTest, message);
	}
	if (this->NewWedgeWidget && this->NewWedgeWidget->GetEnabled())
	{
		return this->NewWedgeWidget->OnSetCursor(pWnd, nHitTest, message);
	}
	if (this->NewPrismWidget && this->NewPrismWidget->GetEnabled())
	{
		return this->NewPrismWidget->OnSetCursor(pWnd, nHitTest, message);
	}
	if (this->NewDrainActor && this->NewDrainActor->GetEnabled())
	{
		return this->NewDrainActor->OnSetCursor(pWnd, nHitTest, message);
	}
	return FALSE;
}

void CWPhastDoc::OnToolsColors()
{
	CPropertySheet sheet;
	CColorsPropertyPage page;
	sheet.AddPage(&page);
	page.m_colors = this->DisplayColors;

	if (sheet.DoModal() == IDOK)
	{
		CAction *pAction = new CSetDisplayColorsAction(this, page.m_colors);
		this->Execute(pAction);
	}
}

void CWPhastDoc::SetDisplayColors(const CDisplayColors& dc)
{
	this->DisplayColors = dc;

	CICChemZoneActor::SetStaticColor(this->DisplayColors.crICChem);
	CBCZoneActor::SetStaticColor(BC_info::BC_FLUX, this->DisplayColors.crFlux);
	CICHeadZoneActor::SetStaticColor(this->DisplayColors.crICHead);
	CBCZoneActor::SetStaticColor(BC_info::BC_LEAKY, this->DisplayColors.crLeaky);
	CMediaZoneActor::SetStaticColor(this->DisplayColors.crMedia);
	CRiverActor::SetStaticColor(this->DisplayColors.crRiver);
	CDrainActor::SetStaticColor(this->DisplayColors.crDrain);
	CBCZoneActor::SetStaticColor(BC_info::BC_SPECIFIED, this->DisplayColors.crSpecHead);
	CWellActor::SetStaticColor(this->DisplayColors.crWell);
	CZoneFlowRateZoneActor::SetStaticColor(this->DisplayColors.crZoneFlowRate);

	POSITION pos = this->GetFirstViewPosition();
	if (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		pView->SetBackground(this->DisplayColors.crBackground);
	}

	// refresh screen
	//
	this->UpdateAllViews(0);
}

CDisplayColors CWPhastDoc::GetDisplayColors()const
{
	return this->DisplayColors;
}

void CWPhastDoc::OnUpdateToolsNewWedge(CCmdUI *pCmdUI)
{
	if (this->NewWedgeWidget && this->NewWedgeWidget->GetEnabled())
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CWPhastDoc::OnToolsNewWedge()
{
	if (this->NewWedgeWidget && this->NewWedgeWidget->GetEnabled())
	{
		this->EndNewWedge();
	}
	else
	{
		if (this->NewWedgeWidget) this->EndNewWedge();
		this->BeginNewWedge();
	}
}

void CWPhastDoc::BeginNewWedge()
{
	ASSERT(this->NewWedgeWidget == 0);
	ASSERT(this->NewWedgeCallbackCommand == 0);

	POSITION pos = this->GetFirstViewPosition();
	if (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		ASSERT(pView->GetInteractor());
		if (pView->GetInteractor())
		{
			pView->CancelMode();

			// create widget
			this->NewWedgeWidget = CNewWedgeWidget::New();
			this->NewWedgeWidget->SetInteractor(pView->GetInteractor());
			this->NewWedgeWidget->SetProp3D(this->GetGridActor());

			// add listener callback
			this->NewWedgeCallbackCommand = vtkCallbackCommand::New();
			this->NewWedgeCallbackCommand->SetClientData(this);
			this->NewWedgeCallbackCommand->SetCallback(CWPhastDoc::NewWedgeListener);
			this->NewWedgeWidget->AddObserver(vtkCommand::EndInteractionEvent, this->NewWedgeCallbackCommand);

			// enable widget
			this->NewWedgeWidget->SetEnabled(1);
		}
	}
}

void CWPhastDoc::EndNewWedge()
{
	if (this->NewWedgeCallbackCommand)
	{
		ASSERT(this->NewWedgeCallbackCommand->IsA("vtkObjectBase"));
		this->NewWedgeCallbackCommand->Delete();
		this->NewWedgeCallbackCommand = 0;
	}
	if (this->NewWedgeWidget)
	{
		ASSERT(this->NewWedgeWidget->IsA("CNewWedgeWidget"));
		this->NewWedgeWidget->SetInteractor(0);
		this->NewWedgeWidget->Delete();
		this->NewWedgeWidget = 0;
	}
}

void CWPhastDoc::NewWedgeListener(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
	ASSERT(caller->IsA("CNewWedgeWidget"));
	ASSERT(clientdata);

	if (clientdata)
	{
		CWPhastDoc* self = reinterpret_cast<CWPhastDoc*>(clientdata);
		if (eid == vtkCommand::EndInteractionEvent)
		{
			double scaled_meters[6];
			self->NewWedgeWidget->GetBounds(scaled_meters);

			// calc zone
			CZone zone;
			double* scale = self->GetScale();
			const CUnits& units = self->GetUnits();
			zone.x1 = scaled_meters[0] / scale[0] / units.horizontal.input_to_si;
			zone.x2 = scaled_meters[1] / scale[0] / units.horizontal.input_to_si;
			zone.y1 = scaled_meters[2] / scale[1] / units.horizontal.input_to_si;
			zone.y2 = scaled_meters[3] / scale[1] / units.horizontal.input_to_si;
			zone.z1 = scaled_meters[4] / scale[2] / units.vertical.input_to_si;
			zone.z2 = scaled_meters[5] / scale[2] / units.vertical.input_to_si;

			TRACE("x(%g-%g) y(%g-%g) z(%g-%g)\n", zone.x1, zone.x2, zone.y1, zone.y2, zone.z1, zone.z2);

			srcWedgeSource::ChopType ct = self->NewWedgeWidget->GetChopType();

			// get type of zone
			//
			ETSLayoutPropertySheet        sheet("Wedge Wizard", NULL, 0, NULL, false);

			CNewZonePropertyPage          newZone;
			CMediaPropsPage2              mediaProps;
			CFluxPropsPage2               fluxProps;
			CLeakyPropsPage               leakyProps;
			CSpecifiedHeadPropsPage       specifiedProps;
			CHeadICPropsPage2             headProps;
			CICChemPropsPage2             chemICProps;
			CZoneFlowRatePropertyPage     zoneFlowRateProps;

			// CChemICSpreadPropertyPage only needs the flowonly flag when the zone is a
			// default zone
			//
			bool bFlowOnly = self->GetFlowOnly();

			fluxProps.SetFlowOnly(bFlowOnly);
			leakyProps.SetFlowOnly(bFlowOnly);
			specifiedProps.SetFlowOnly(bFlowOnly);

			// set used zone flow rate numbers
			std::set<int> usedZoneFlowRatesNumbers;
			self->GetUsedZoneFlowRates(usedZoneFlowRatesNumbers);
			zoneFlowRateProps.SetUsedZoneFlowRates(usedZoneFlowRatesNumbers);

			sheet.AddPage(&newZone);
			sheet.AddPage(&mediaProps);
			sheet.AddPage(&fluxProps);
			sheet.AddPage(&leakyProps);
			sheet.AddPage(&specifiedProps);
			sheet.AddPage(&headProps);
			sheet.AddPage(&chemICProps);
			sheet.AddPage(&zoneFlowRateProps);

			sheet.SetWizardMode();

			if (sheet.DoModal() == ID_WIZFINISH)
			{
				if (newZone.GetType() == ID_ZONE_TYPE_MEDIA)
				{
					CGridElt elt;
					mediaProps.GetProperties(elt);
					elt.polyh = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(ct));
					CMediaZoneActor::Create(self, elt, mediaProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_FLUX)
				{
					CBC bc;
					fluxProps.GetProperties(bc);
					bc.polyh = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(ct));
					CBCZoneActor::Create(self, bc, fluxProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_LEAKY)
				{
					CBC bc;
					leakyProps.GetProperties(bc);
					bc.polyh = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(ct));
					CBCZoneActor::Create(self, bc, leakyProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_SPECIFIED)
				{
					CBC bc;
					specifiedProps.GetProperties(bc);
					bc.polyh = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(ct));
					CBCZoneActor::Create(self, bc, specifiedProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_IC_HEAD)
				{
					CHeadIC headic;
					headProps.GetProperties(headic);
					headic.polyh = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(ct));
					CICHeadZoneActor::Create(self, headic, headProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_IC_CHEM)
				{
					CChemIC chemIC;
					chemICProps.GetProperties(chemIC);
					chemIC.polyh = new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(ct));
					CICChemZoneActor::Create(self, chemIC, chemICProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_FLOW_RATE)
				{
					Zone_budget zone_budget;
					zoneFlowRateProps.GetProperties(zone_budget);
					zone_budget.Set_polyh(new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(ct)));
					CZoneFlowRateZoneActor::Create(self, zone_budget);
				}
			}

			// Note: cannot call EndNewWedge here
			self->NewWedgeWidget->SetInteractor(0);
		}
	}

}

void CWPhastDoc::OnUpdateToolsNewPrism(CCmdUI *pCmdUI)
{
	if (this->NewPrismWidget && this->NewPrismWidget->GetEnabled())
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CWPhastDoc::OnToolsNewPrism()
{
	if (this->NewPrismWidget && this->NewPrismWidget->GetEnabled())
	{
		this->EndNewPrism();
	}
	else
	{
		// if (this->NewPrismWidget) this->EndNewPrism();
		// Cancel Mode calls EndNewPrism

		// CancelMode
		//
		POSITION pos = this->GetFirstViewPosition();
		if (pos != NULL)
		{
			CWPhastView *pView = (CWPhastView*) GetNextView(pos);
			ASSERT_VALID(pView);
			if (pView)
			{
				pView->CancelMode();
			}
			this->UpdateAllViews(0);
		}

		CPerimeterDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			CWaitCursor wait;
			if (dlg.type == CPerimeterDlg::PT_DRAW)
			{
				this->BeginNewPrism();
			}
			else
			{
				// use an empty phastinput in case of errors
				//

				std::istringstream emptyiss;
				CPhastInput* pInput = CPhastInput::New(emptyiss, "OnToolsNewPrism", false);

				std::ostringstream oss;
				oss << "-perimeter SHAPE map " << dlg.m_strShapefile;
				std::istringstream iss(oss.str());

				Prism *p = 0;
				bool bOk = false;
				try
				{
					p = new Prism();
					if (p && p->Read(iss))
					{
						this->GetDefaultZone(::domain);

						p->Tidy();

						// this may need to be updated when grid rotation is enabled
						//
						const CUnits &units = this->GetUnits();
						CGridKeyword gridKeyword = this->GetGridKeyword();
						double scale_h = units.map_horizontal.input_to_si / units.horizontal.input_to_si;
						double scale_v = units.map_vertical.input_to_si / units.vertical.input_to_si;
						PHAST_Transform t = PHAST_Transform(
							gridKeyword.m_grid_origin[0],
							gridKeyword.m_grid_origin[1],
							gridKeyword.m_grid_origin[2],
							gridKeyword.m_grid_angle,
							scale_h,
							scale_h,
							scale_v
							);
						p->Convert_coordinates(PHAST_Transform::GRID, &t);
					}
					if (pInput->GetErrorCount() != 0)
					{
						throw pInput->GetErrorCount();
					}
					bOk = true;
				}
				catch (int)
				{
					CImportErrorDialog dlg;
					dlg.m_lpszErrorMessages = pInput->GetErrorMsg();
					dlg.DoModal();
					delete p;
				}
				catch(CSeException *e)
				{
					TCHAR trcMsg[1024];
					e->GetErrorMessage(trcMsg, 1024);
					TRACE(trcMsg);
					TRACE(_T("\n"));
					e->ReportError(MB_OK | MB_ICONSTOP);
					e->Delete();
					delete p;
				}
				catch (...)
				{
					::AfxMessageBox("An unknown error occured while loading perimeter file.", MB_OK|MB_ICONEXCLAMATION);
					delete p;
				}

				pInput->Delete();

				if (bOk)
				{
					// get type of zone
					//
					ETSLayoutPropertySheet        sheet("Prism Wizard", NULL, 0, NULL, false);

					CNewZonePropertyPage          newZone;
					CMediaPropsPage2              mediaProps;
					CFluxPropsPage2               fluxProps;
					CLeakyPropsPage               leakyProps;
					CSpecifiedHeadPropsPage       specifiedProps;
					CHeadICPropsPage2             headProps;
					CICChemPropsPage2             chemICProps;
					CZoneFlowRatePropertyPage     zoneFlowRateProps;

					// CChemICSpreadPropertyPage only needs the flowonly flag when the zone is a
					// default zone
					//
					bool bFlowOnly = this->GetFlowOnly();

					fluxProps.SetFlowOnly(bFlowOnly);
					leakyProps.SetFlowOnly(bFlowOnly);
					specifiedProps.SetFlowOnly(bFlowOnly);

					// set used zone flow rate numbers
					std::set<int> usedZoneFlowRatesNumbers;
					this->GetUsedZoneFlowRates(usedZoneFlowRatesNumbers);
					zoneFlowRateProps.SetUsedZoneFlowRates(usedZoneFlowRatesNumbers);

					sheet.AddPage(&newZone);
					sheet.AddPage(&mediaProps);
					sheet.AddPage(&fluxProps);
					sheet.AddPage(&leakyProps);
					sheet.AddPage(&specifiedProps);
					sheet.AddPage(&headProps);
					sheet.AddPage(&chemICProps);
					sheet.AddPage(&zoneFlowRateProps);

					sheet.SetWizardMode();

					if (sheet.DoModal() == ID_WIZFINISH)
					{
						this->UpdateAllViews(0);
						CWaitCursor wait;
						if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
						{
							CString status(_T("Creating new prism..."));
							pWnd->SetWindowText(status);
						}
						::Sleep(100);

						if (newZone.GetType() == ID_ZONE_TYPE_MEDIA)
						{
							CGridElt elt;
							mediaProps.GetProperties(elt);
							elt.polyh = p;
							CMediaZoneActor::Create(this, elt, mediaProps.GetDesc());
						}
						else if (newZone.GetType() == ID_ZONE_TYPE_BC_FLUX)
						{
							CBC bc;
							fluxProps.GetProperties(bc);
							bc.polyh = p;
							CBCZoneActor::Create(this, bc, fluxProps.GetDesc());
						}
						else if (newZone.GetType() == ID_ZONE_TYPE_BC_LEAKY)
						{
							CBC bc;
							leakyProps.GetProperties(bc);
							bc.polyh = p;
							CBCZoneActor::Create(this, bc, leakyProps.GetDesc());
						}
						else if (newZone.GetType() == ID_ZONE_TYPE_BC_SPECIFIED)
						{
							CBC bc;
							specifiedProps.GetProperties(bc);
							bc.polyh = p;
							CBCZoneActor::Create(this, bc, specifiedProps.GetDesc());
						}
						else if (newZone.GetType() == ID_ZONE_TYPE_IC_HEAD)
						{
							CHeadIC headic;
							headProps.GetProperties(headic);
							headic.polyh = p;
							CICHeadZoneActor::Create(this, headic, headProps.GetDesc());
						}
						else if (newZone.GetType() == ID_ZONE_TYPE_IC_CHEM)
						{
							CChemIC chemIC;
							chemICProps.GetProperties(chemIC);
							chemIC.polyh = p;
							CICChemZoneActor::Create(this, chemIC, chemICProps.GetDesc());
						}
						else if (newZone.GetType() == ID_ZONE_TYPE_FLOW_RATE)
						{
							Zone_budget zone_budget;
							zoneFlowRateProps.GetProperties(zone_budget);
							zone_budget.Set_polyh(p);
							CZoneFlowRateZoneActor::Create(this, zone_budget);
						}
					}
					else
					{
						delete p;
					}

					// refresh screen
					//
					this->ResetCamera();
					this->UpdateAllViews(0);
				}
				else
				{
					delete p;
				}
			}
		}
	}
}

void CWPhastDoc::BeginNewPrism()
{
	ASSERT(this->NewPrismWidget == 0);
	ASSERT(this->NewPrismCallbackCommand == 0);

	POSITION pos = this->GetFirstViewPosition();
	if (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		ASSERT(pView->GetInteractor());
		if (pView->GetInteractor())
		{
			pView->CancelMode();

			// create callback
			this->NewPrismCallbackCommand = vtkCallbackCommand::New();
			this->NewPrismCallbackCommand->SetClientData(this);
			this->NewPrismCallbackCommand->SetCallback(CWPhastDoc::NewPrismListener);

			// create widget
			this->NewPrismWidget = CNewPrismWidget::New();
			this->NewPrismWidget->SetInteractor(pView->GetInteractor());
			this->NewPrismWidget->SetProp3D(this->GetGridActor());
			this->NewPrismWidget->AddObserver(vtkCommand::EndInteractionEvent, this->NewPrismCallbackCommand);

			// enable widget
			this->NewPrismWidget->SetEnabled(1);
		}
	}
}

void CWPhastDoc::EndNewPrism()
{
	if (this->NewPrismCallbackCommand)
	{
		ASSERT(this->NewPrismCallbackCommand->IsA("vtkObjectBase"));
		this->NewPrismCallbackCommand->Delete();
		this->NewPrismCallbackCommand = 0;
	}
	if (this->NewPrismWidget)
	{
		ASSERT(this->NewPrismWidget->IsA("CNewPrismWidget"));
		this->NewPrismWidget->SetInteractor(0);
		this->NewPrismWidget->Delete();
		this->NewPrismWidget = 0;
	}
}

void CWPhastDoc::NewPrismListener(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
	ASSERT(caller->IsA("CNewPrismWidget"));
	ASSERT(clientdata);

	if (clientdata)
	{
		CWPhastDoc* self = reinterpret_cast<CWPhastDoc*>(clientdata);

		if (eid == vtkCommand::EndInteractionEvent)
		{
			double* scale = self->GetScale();
			const CUnits& units = self->GetUnits();

			vtkPoints *points = self->NewPrismWidget->GetPoints();

			if (points->GetNumberOfPoints() < 6)
			{
				::AfxMessageBox("Perimeter must be defined by at least 3 points.");

				// Note: cannot call EndNewPrism here
				self->NewPrismWidget->SetInteractor(0);
				return;
			}

			std::ostringstream oss;
			oss.precision(DBL_DIG);
			oss << "-perimeter POINTS grid" << std::endl;
			double pt[3];
			for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i+=2)
			{
				//double *pt = points->GetPoint(i);
				points->GetPoint(i, pt);
				oss << pt[0] / scale[0] / units.horizontal.input_to_si << " ";
				oss << pt[1] / scale[1] / units.horizontal.input_to_si << " ";
				oss << "0.0" << std::endl;
			}
			TRACE(oss.str().c_str());
			std::istringstream iss(oss.str());

			// setup domain
			self->GetDefaultZone(::domain);

			Prism *p = new Prism();
			if (p->Read(iss))
			{
				p->Tidy();

				// get type of zone
				//
				ETSLayoutPropertySheet        sheet("Prism Wizard", NULL, 0, NULL, false);

				CNewZonePropertyPage          newZone;
				CMediaPropsPage2              mediaProps;
				CFluxPropsPage2               fluxProps;
				CLeakyPropsPage               leakyProps;
				CSpecifiedHeadPropsPage       specifiedProps;
				CHeadICPropsPage2             headProps;
				CICChemPropsPage2             chemICProps;
				CZoneFlowRatePropertyPage     zoneFlowRateProps;

				// CChemICSpreadPropertyPage only needs the flowonly flag when the zone is a
				// default zone
				//
				bool bFlowOnly = self->GetFlowOnly();

				fluxProps.SetFlowOnly(bFlowOnly);
				leakyProps.SetFlowOnly(bFlowOnly);
				specifiedProps.SetFlowOnly(bFlowOnly);

				// set used zone flow rate numbers
				std::set<int> usedZoneFlowRatesNumbers;
				self->GetUsedZoneFlowRates(usedZoneFlowRatesNumbers);
				zoneFlowRateProps.SetUsedZoneFlowRates(usedZoneFlowRatesNumbers);

				sheet.AddPage(&newZone);
				sheet.AddPage(&mediaProps);
				sheet.AddPage(&fluxProps);
				sheet.AddPage(&leakyProps);
				sheet.AddPage(&specifiedProps);
				sheet.AddPage(&headProps);
				sheet.AddPage(&chemICProps);
				sheet.AddPage(&zoneFlowRateProps);

				sheet.SetWizardMode();

				if (sheet.DoModal() == ID_WIZFINISH)
				{
					if (newZone.GetType() == ID_ZONE_TYPE_MEDIA)
					{
						CGridElt elt;
						mediaProps.GetProperties(elt);
						elt.polyh = p;
						CMediaZoneActor::Create(self, elt, mediaProps.GetDesc());
					}
					else if (newZone.GetType() == ID_ZONE_TYPE_BC_FLUX)
					{
						CBC bc;
						fluxProps.GetProperties(bc);
						bc.polyh = p;
						CBCZoneActor::Create(self, bc, fluxProps.GetDesc());
					}
					else if (newZone.GetType() == ID_ZONE_TYPE_BC_LEAKY)
					{
						CBC bc;
						leakyProps.GetProperties(bc);
						bc.polyh = p;
						CBCZoneActor::Create(self, bc, leakyProps.GetDesc());
					}
					else if (newZone.GetType() == ID_ZONE_TYPE_BC_SPECIFIED)
					{
						CBC bc;
						specifiedProps.GetProperties(bc);
						bc.polyh = p;
						CBCZoneActor::Create(self, bc, specifiedProps.GetDesc());
					}
					else if (newZone.GetType() == ID_ZONE_TYPE_IC_HEAD)
					{
						CHeadIC headic;
						headProps.GetProperties(headic);
						headic.polyh = p;
						CICHeadZoneActor::Create(self, headic, headProps.GetDesc());
					}
					else if (newZone.GetType() == ID_ZONE_TYPE_IC_CHEM)
					{
						CChemIC chemIC;
						chemICProps.GetProperties(chemIC);
						chemIC.polyh = p;
						CICChemZoneActor::Create(self, chemIC, chemICProps.GetDesc());
					}
					else if (newZone.GetType() == ID_ZONE_TYPE_FLOW_RATE)
					{
						Zone_budget zone_budget;
						zoneFlowRateProps.GetProperties(zone_budget);
						zone_budget.Set_polyh(p);
						CZoneFlowRateZoneActor::Create(self, zone_budget);
					}
				}
				else
				{
					delete p;
				}
			}

			// Note: cannot call EndNewPrism here
			self->NewPrismWidget->SetInteractor(0);
		}
	}
}

void CWPhastDoc::GetDefaultZone(zone &z)const
{
	if (this->m_pGridActor)
	{
		this->m_pGridActor->GetDefaultZone(z);
	}
}

void CWPhastDoc::OnUpdateToolsNewDrain(CCmdUI *pCmdUI)
{
	if (this->NewDrainCallbackCommand)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CWPhastDoc::OnToolsNewDrain()
{
	TRACE("%s, in\n", __FUNCTION__);

	if (this->NewDrainActor && this->NewDrainActor->GetEnabled())
	{
		this->EndNewDrain();
	}
	else
	{
		// CancelMode
		//
		POSITION pos = this->GetFirstViewPosition();
		if (pos != NULL)
		{
			CWPhastView *pView = (CWPhastView*) GetNextView(pos);
			ASSERT_VALID(pView);
			if (pView)
			{
				pView->CancelMode();    // calls EndNewDrain
			}
			this->UpdateAllViews(0);
		}

		// Begin
		//
		this->BeginNewDrain();
	}

	TRACE("%s, out\n", __FUNCTION__);
}

void CWPhastDoc::BeginNewDrain()
{
	TRACE("%s, in\n", __FUNCTION__);

	ASSERT(this->NewDrainActor == 0);
	ASSERT(this->NewDrainCallbackCommand == 0);

	POSITION pos = this->GetFirstViewPosition();
	if (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		ASSERT(pView->GetInteractor());
		if (pView->GetInteractor())
		{
			pView->CancelMode();

			// create callback
			this->NewDrainCallbackCommand = vtkCallbackCommand::New();
			this->NewDrainCallbackCommand->SetClientData(this);
			this->NewDrainCallbackCommand->SetCallback(CWPhastDoc::NewDrainListener);

			// create drain
			CDrain drain;
			drain.coordinate_system = PHAST_Transform::GRID;
			drain.z_coordinate_system = PHAST_Transform::GRID;
			drain.n_user = this->GetNextDrainNumber();

			// create widget
			this->NewDrainActor = CDrainActor::StartNewDrain(pView->GetInteractor());
			this->NewDrainActor->SetDrain(drain, this->GetUnits(), this->GetGridKeyword());
			this->NewDrainActor->AddObserver(CRiverActor::EndNewEvent, this->NewDrainCallbackCommand);
			this->NewDrainActor->AddObserver(CRiverActor::CancelNewEvent, this->NewDrainCallbackCommand);

			// scale
			double* scale = this->GetScale();
			this->NewDrainActor->SetScale(scale[0], scale[1], scale[2]);

			this->NewDrainActor->ScaleFromBounds(this->GetGridBounds());

			// TODO scale by window size
			CGrid x, y, z;
			this->GetGrid(x, y, z);
			z.Setup();
			this->NewDrainActor->SetZ(z.coord[z.count_coord - 1]);

			// add prop
			POSITION pos = this->GetFirstViewPosition();
			if (pos != NULL)
			{
				CWPhastView *pView = (CWPhastView*) GetNextView(pos);
				ASSERT_VALID(pView);
				if (pView)
				{
					pView->GetRenderer()->AddViewProp(this->NewDrainActor);
				}
				this->UpdateAllViews(0);
			}
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CWPhastDoc::EndNewDrain()
{
	TRACE("%s, in\n", __FUNCTION__);

	// remove prop
	POSITION pos = this->GetFirstViewPosition();
	if (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		ASSERT(pView->GetRenderer());
		if (pView->GetRenderer())
		{
			pView->GetRenderer()->RemoveViewProp(this->NewDrainActor);
		}
	}

	if (this->NewDrainCallbackCommand)
	{
		ASSERT(this->NewDrainCallbackCommand->IsA("vtkObjectBase"));
		this->NewDrainCallbackCommand->Delete();
		this->NewDrainCallbackCommand = 0;
	}
	if (this->NewDrainActor)
	{
		ASSERT(this->NewDrainActor->IsA("CDrainActor"));
		this->NewDrainActor->SetEnabled(0);
		this->NewDrainActor->SetInteractor(0);

		this->NewDrainActor->Delete();
		this->NewDrainActor = 0;
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CWPhastDoc::NewDrainListener(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
	TRACE("%s, in\n", __FUNCTION__);
	ASSERT(caller->IsA("CDrainActor"));
	ASSERT(clientdata);

	if (CDrainActor* pDrainActor = CDrainActor::SafeDownCast(caller))
	{
		CWPhastDoc* self = reinterpret_cast<CWPhastDoc*>(clientdata);
		switch (eid)
		{
		case CDrainActor::CancelNewEvent:
			self->OnEndNewDrain(true);
			break;
		case CDrainActor::EndNewEvent:
			self->OnEndNewDrain(false);
			break;
		}
	}
	TRACE("%s, out\n", __FUNCTION__);
}

void CWPhastDoc::OnEndNewDrain(bool bCancel)
{
	//
	// NOTE: this->m_pRiverActor->Delete() can't be called within
	// here because the list of listeners will also be deleted
	// *** this may be fixed in 4.4 ***
	//
	ASSERT(this->NewDrainActor != 0);
	if (this->NewDrainActor)
	{
		if (!bCancel)
		{
			if (this->NewDrainActor->GetPointCount() > 1)
			{
				CDrain drain = this->NewDrainActor->GetDrain();
				CString title;
				title.Format("Drain %d Properties", drain.n_user);

				CPropertySheet sheet(title);
				CDrainPropertyPage page;
				page.SetProperties(drain);
				page.SetUnits(this->GetUnits());
				page.SetGridKeyword(this->GetGridKeyword());
				page.SetFlowOnly(bool(this->GetFlowOnly()));
				std::set<int> drainNums;
				this->GetUsedDrainNumbers(drainNums);
				page.SetUsedDrainNumbers(drainNums);
				sheet.AddPage(&page);
				if (sheet.DoModal() == IDOK)
				{
					CDrain drain;
					page.GetProperties(drain);
					this->Execute(new CPointConnectorCreateAction<CDrainActor, CDrain>(this, drain));
				}
			}
			else
			{
				::AfxMessageBox("Drains must contain at least two points");
			}
		}

		POSITION pos = this->GetFirstViewPosition();
		if (pos != NULL)
		{
			CWPhastView *pView = (CWPhastView*) this->GetNextView(pos);
			ASSERT_VALID(pView);
			if (pView)
			{
				pView->GetRenderer()->RemoveViewProp(this->NewDrainActor);
			}
		}
	}

	if (this->NewDrainCallbackCommand)
	{
		this->NewDrainCallbackCommand->Delete();
		this->NewDrainCallbackCommand = 0;
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::GetUsedDrainNumbers(std::set<int>& usedNums)
{
	usedNums.clear();
	if (vtkPropCollection *pPropCollection = this->GetPropAssemblyDrains()->GetParts())
	{
		vtkProp *pProp = 0;
		vtkCollectionSimpleIterator csi;
		pPropCollection->InitTraversal(csi);
		for (; (pProp = pPropCollection->GetNextProp(csi)); )
		{
			if (CDrainActor *pDrainActor = CDrainActor::SafeDownCast(pProp))
			{
				// store used n_user numbers
				//
				std::pair< std::set<int>::iterator, bool > pr;
				pr = usedNums.insert( pDrainActor->GetDrain().n_user );
				ASSERT(pr.second); // duplicate?
			}
		}
	}
}

int CWPhastDoc::GetNextDrainNumber(void)
{
	std::set<int> drainNums;
	this->GetUsedDrainNumbers(drainNums);
	if (drainNums.rbegin() != drainNums.rend())
	{
		return (*drainNums.rbegin()) + 1;
	}
	else
	{
		return 1;
	}
}

void CWPhastDoc::Add(CDrainActor *pDrainActor, HTREEITEM hInsertAfter)
{
	ASSERT(pDrainActor);
	if (!pDrainActor) return;

	// set scale
	//
	double *scale = this->GetScale();
	pDrainActor->SetScale(scale[0], scale[1], scale[2]);

	// set radius
	//
	pDrainActor->ScaleFromBounds(this->GetGridBounds());

	// set z
	//
	CGrid x, y, z;
	this->GetGrid(x, y, z);
	z.Setup();
	pDrainActor->SetZ(z.coord[z.count_coord - 1]);

	// for all views disable interaction
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		pDrainActor->SetInteractor(pView->GetInteractor());
		pDrainActor->SetEnabled(0);
	}

	// add to river assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyDrains())
	{
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(pDrainActor));
		pPropAssembly->AddPart(pDrainActor);
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(pDrainActor));
		this->AddPropAssembly(pPropAssembly);
	}

	// add to property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pDrainActor->Add(pTree, hInsertAfter);
	}

	// add listeners
	//
	ASSERT(this->DrainCallbackCommand);
	pDrainActor->AddObserver(CDrainActor::StartMovePointEvent, this->DrainCallbackCommand);
	pDrainActor->AddObserver(CDrainActor::MovingPointEvent,    this->DrainCallbackCommand);
	pDrainActor->AddObserver(CDrainActor::EndMovePointEvent,   this->DrainCallbackCommand);
	pDrainActor->AddObserver(CDrainActor::InsertPointEvent,    this->DrainCallbackCommand);

	// render
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::UnAdd(CDrainActor *pDrainActor)
{
	ASSERT(pDrainActor);
	if (!pDrainActor) return;

	// remove from rivers assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyDrains())
	{
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(pDrainActor));
		pPropAssembly->RemovePart(pDrainActor);
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(pDrainActor));
// COMMENT: {9/23/2009 8:52:37 PM}		// VTK HACK
// COMMENT: {9/23/2009 8:52:37 PM}		// This is req'd because ReleaseGraphicsResources is not called when
// COMMENT: {9/23/2009 8:52:37 PM}		// vtkPropAssembly::RemovePart(vtkProp *prop) is called
// COMMENT: {9/23/2009 8:52:37 PM}		this->ReleaseGraphicsResources(pDrainActor);
		//{{
		pDrainActor->SetInteractor(0);
		//}}
	}

	// remove from property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pDrainActor->UnAdd(pTree);
	}

	// remove listeners
	//
	ASSERT(this->DrainCallbackCommand);
	pDrainActor->RemoveObserver(this->DrainCallbackCommand);

	// render
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::DrainListener(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
	ASSERT(caller->IsA("CDrainActor"));
	ASSERT(clientdata);

	if (CDrainActor* drainActor = CDrainActor::SafeDownCast(caller))
	{
		CWPhastDoc* self = reinterpret_cast<CWPhastDoc*>(clientdata);

		switch (eid)
		{
		case CDrainActor::StartMovePointEvent:
			ASSERT(self->DrainMovePointAction == 0);
			self->DrainMovePointAction = new CPointConnectorMovePointAction<CDrainActor>(drainActor, self, drainActor->GetCurrentPointId(), drainActor->GetCurrentPointPosition()[0], drainActor->GetCurrentPointPosition()[1]);
			break;

		case CDrainActor::MovingPointEvent:
			ASSERT(self->DrainMovePointAction != 0);
			if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
			{
				// update message bar
				//
				static TCHAR buffer[80];
				const CUnits& units = self->GetUnits();
				::_sntprintf(buffer, 80, "%g[%s] x %g[%s]",
					self->DrainMovePointAction->GetActor()->GetCurrentPointPosition()[0] / units.horizontal.input_to_si,
					units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
					self->DrainMovePointAction->GetActor()->GetCurrentPointPosition()[1] / units.horizontal.input_to_si,
					units.horizontal.defined ? units.horizontal.input : units.horizontal.si
					);
				pWnd->SetWindowText(buffer);
			}
			break;

		case CDrainActor::EndMovePointEvent:
			ASSERT(self->DrainMovePointAction != 0);
			if (self->DrainMovePointAction)
			{
				self->DrainMovePointAction->SetPoint(drainActor->GetCurrentPointPosition()[0], drainActor->GetCurrentPointPosition()[1]);
				self->Execute(self->DrainMovePointAction);
				self->DrainMovePointAction = 0;
			}
			break;

		case CDrainActor::InsertPointEvent:
			{
				double* pos = drainActor->GetCurrentPointPosition();
				vtkIdType id = drainActor->GetCurrentPointId() - 1;
				CPointConnectorInsertPointAction<CDrainActor, CRiverPoint>* pDrainInsertPointAction =
					new CPointConnectorInsertPointAction<CDrainActor, CRiverPoint>(drainActor, self, id, pos[0], pos[1], pos[2], true);
				self->Execute(pDrainInsertPointAction);
			}
			break;

		default:
			ASSERT(FALSE);
			break;
		}
	}
}

HACCEL CWPhastDoc::GetDefaultAccelerator()
{
	// Need to override in order to allow GridActor to 
	// handle WM_KEYPRESS for delete
	//
	POSITION pos =  this->GetFirstViewPosition();
	CWPhastView *pView = (CWPhastView*)this->GetNextView(pos);
	ASSERT_VALID(pView);
	if (pView && pView->MovingGridLine())
	{
		// maybe use GetForegroundWindow
		if (::GetFocus() == pView->GetSafeHwnd())
		{
			return this->hGridAccel;
		}
	}
	return CDocument::GetDefaultAccelerator();
}

#if ((VTK_MAJOR_VERSION >= 5) && (VTK_MINOR_VERSION >= 4))
void CWPhastDoc::ExecutePipeline()
{
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) this->GetNextView(pos);
		ASSERT_VALID(pView);

		// add props to renderer
		if (vtkPropCollection *props = this->GetPropCollection())
		{
			vtkCollectionSimpleIterator csi;
			props->InitTraversal(csi);
			for (int i = 0; i < props->GetNumberOfItems(); ++i)
			{
				vtkProp* prop = props->GetNextProp(csi);
				if (prop)
				{
					if (vtkPropAssembly *pPropAssembly = vtkPropAssembly::SafeDownCast(prop))
					{
						if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
						{
							if (pPropCollection->GetNumberOfItems() == 0)
							{
								ASSERT(FALSE);
								continue;
							}
						}
						TRACE("Adding vtkPropAssembly to Renderer\n");
					}
					pView->GetRenderer()->AddViewProp(prop);
					if (i == 0)
					{
						// Note: pView->ResetCamera(xmin, xmax, ymin, ymax, zmin, zmax)
						// doesn't seem to work for example ex1
						ASSERT(prop == this->m_pGridActor);
						pView->ResetCamera();
					}
				}
			}
		}
		pView->GetRenderer()->ResetCameraClippingRange();
	}
}
#endif

void CWPhastDoc::OnAccelerator32862()
{
#if defined(__CPPUNIT__)
	if (this->m_pMapActor)
	{
		//{{
		CSiteMap2 siteMap2 = this->m_pMapActor->GetSiteMap2();
		double angle_save = siteMap2.Angle;
		for (double a = 0; a < 360.0; a+=1.0)
		{
			siteMap2.Angle = angle_save + a;
			this->m_pMapActor->SetSiteMap2(siteMap2);
			this->Notify(this, WPN_DOMAIN_CHANGED, 0, 0);
			
		}
		siteMap2.Angle = angle_save;
		this->m_pMapActor->SetSiteMap2(siteMap2);
		this->Notify(this, WPN_DOMAIN_CHANGED, 0, 0);
		//}}

// COMMENT: {9/7/2010 10:21:15 PM}		//{{
// COMMENT: {9/7/2010 10:21:15 PM}		CGridKeyword keyword = this->GetGridKeyword();
// COMMENT: {9/7/2010 10:21:15 PM}		double angle_save = keyword.m_grid_angle;
// COMMENT: {9/7/2010 10:21:15 PM}		for (double a = 0; a < 360.0; a+=10.0)
// COMMENT: {9/7/2010 10:21:15 PM}		{
// COMMENT: {9/7/2010 10:21:15 PM}			keyword.m_grid_angle = angle_save + a;
// COMMENT: {9/7/2010 10:21:15 PM}
// COMMENT: {9/7/2010 10:21:15 PM}			this->ResizeGrid(keyword);
// COMMENT: {9/7/2010 10:21:15 PM}		}
// COMMENT: {9/7/2010 10:21:15 PM}		keyword.m_grid_angle = angle_save;
// COMMENT: {9/7/2010 10:21:15 PM}		this->ResizeGrid(keyword);
// COMMENT: {9/7/2010 10:21:15 PM}		//}}
	}
#endif
}

void CWPhastDoc::AddPropAssembly(vtkPropAssembly *pPropAssembly)
{
	if (!this->GetPropCollection()->IsItemPresent(pPropAssembly))
	{
		this->GetPropCollection()->AddItem(pPropAssembly);
		POSITION pos = this->GetFirstViewPosition();
		while (pos != NULL)
		{
			CWPhastView *pView = (CWPhastView*) GetNextView(pos);
			ASSERT_VALID(pView);
			if (pView->GetRenderer()->GetViewProps()->IsItemPresent(this->GetPropAssemblyMedia()))
			{
				// This is a hack.  Since the default media must exist
				// the list of props must contain the media assembly
				// and therefore ExecutePipeline has been called for this
				// document.  If ExecutePipeline has not been executed
				// the river assembly will flash
				pView->GetRenderer()->AddViewProp(pPropAssembly);
			}
		}
	}
}
