// WPhastDoc.cpp : implementation of the CWPhastDoc class
//
#include "stdafx.h"
#include "WPhast.h"

#include <map>      // std::map
#include <vector>   // std::vector
#include <list>     // std::list
#include <fstream>  // std::ifstream
#include <sstream>  // std::ostringstream std::istringstream

#include "WPhastDoc.h"
#include "WPhastView.h"

#include "srcimpl.h"       // SrcFullPath SrcResolveShortcut
#include "HDFMirrorFile.h"
#include "PhastInput.h"
#include "PrintFreqPropertyPage.h"

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


#include "Version.h"

#include "srcinput/Filedata.h"
#include "srcinput/Domain.h"
#include "srcinput/Polyhedron.h"
#include "NullPolyhedron.h"

#include "Action.h"
#include "SetMediaAction.h"
#include "SetBCAction.h"
#include "SetChemICAction.h"
#include "SetHeadICAction.h"
#include "GridRotateAction.h"
#include "SetZoneBudgetAction.h"

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

#include "GridActor.h"
#include "AxesActor.h"
#include "Global.h"

#include "ImportErrorDialog.h"
#include "ImportWarningDialog.h"
#include "RunDialog.h"
#include "PerimeterDlg.h"
#include "FileRecreateDialog.h"

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

#include "ZoneCreateAction.h"
#include "WellCreateAction.h"
#include "PointConnectorCreateAction.h"
#include "PointConnectorMovePointAction.h"
#include "PointConnectorInsertPointAction.h"
#include "PointConnectorDeletePointAction.h"
#include "GridDeleteLineAction.h"
#include "GridInsertLineAction.h"
#include "GridMoveLineAction.h"
#include "SetChemICAction.h"


// zone property pages
#include "NewZonePropertyPage.h"
#include "MediaSpreadPropertyPage.h"
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
#include "WorldTransform.h"
#include "SiteMap.h"
#include "SiteMap2.h"
#include "SiteMap3.h"
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

#include <atlpath.h> // ATL::ATLPath::FileExists


extern void GetDefaultMedia(struct grid_elt* p_grid_elt);
extern void GetDefaultHeadIC(struct Head_ic* p_head_ic);
extern void GetDefaultChemIC(struct chem_ic* p_chem_ic);

static const char szWPhast[] = "WPhast";
static const char szFiles[]  = "Files";

static const TCHAR szZoneFormat[]    = _T("Box %d");
static const TCHAR szWedgeFormat[]   = _T("Wedge %d");
static const TCHAR szPrismFormat[]   = _T("Prism %d");
static const TCHAR szDomainFormat[]  = _T("Domain %d");
static const TCHAR szNullFormat[]    = _T("Null %d");

static const TCHAR szZoneFind[]      = _T("Box ");
static const TCHAR szWedgeFind[]     = _T("Wedge ");
static const TCHAR szPrismFind[]     = _T("Prism ");
static const TCHAR szDomainFind[]    = _T("Domain ");
static const TCHAR szNullFind[]      = _T("Null ");


int error_msg (const char *err_str, const int stop);

#include "FakeFiledata.h"
void Clear_NNInterpolatorList(void);
void Clear_file_data_map(void);
void Clear_KDtreeList(void);

#define CANCEL_WHEN_GRID_UNCHECKED

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

	// MODE
	ON_UPDATE_COMMAND_UI(ID_SETGRID_MODE, OnUpdateGridMode)
	ON_COMMAND(ID_SETGRID_MODE, OnSetGridMode)

	ON_UPDATE_COMMAND_UI(ID_SETMAP_MODE, OnUpdateMapMode)
	ON_COMMAND(ID_SETMAP_MODE, OnSetMapMode)

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

	// ID_DRAINS_HIDEALL
	ON_UPDATE_COMMAND_UI(ID_DRAINS_HIDEALL, OnUpdateDrainsHideAll)
	ON_COMMAND(ID_DRAINS_HIDEALL, OnDrainsHideAll)

	// ID_DRAINS_SHOWSELECTED
	ON_UPDATE_COMMAND_UI(ID_DRAINS_SHOWSELECTED, OnUpdateDrainsShowSelected)
	ON_COMMAND(ID_DRAINS_SHOWSELECTED, OnDrainsShowSelected)

	// ID_DRAINS_SELECTALL
	ON_COMMAND(ID_DRAINS_SELECTALL, OnDrainsSelectAll)

	// ID_DRAINS_UNSELECTALL
	ON_COMMAND(ID_DRAINS_UNSELECTALL, OnDrainsUnselectAll)

	// ID_ZONE_FLOW_HIDEALL
	ON_UPDATE_COMMAND_UI(ID_ZONE_FLOW_HIDEALL, OnUpdateZoneFlowHideAll)
	ON_COMMAND(ID_ZONE_FLOW_HIDEALL, OnZoneFlowHideAll)

	// ID_ZONE_FLOW_SHOWSELECTED
	ON_UPDATE_COMMAND_UI(ID_ZONE_FLOW_SHOWSELECTED, OnUpdateZoneFlowShowSelected)
	ON_COMMAND(ID_ZONE_FLOW_SHOWSELECTED, OnZoneFlowShowSelected)

	// ID_ZONE_FLOW_SELECTALL
	ON_COMMAND(ID_ZONE_FLOW_SELECTALL, OnZoneFlowSelectAll)

	// ID_ZONE_FLOW_UNSELECTALL
	ON_COMMAND(ID_ZONE_FLOW_UNSELECTALL, OnZoneFlowUnselectAll)

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

	// ID_TOOLS_NEWDOMAIN
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWDOMAIN, OnUpdateToolsNewDomain)
	ON_COMMAND(ID_TOOLS_NEWDOMAIN, OnToolsNewDomain)

	// ID_TOOLS_NEWDRAIN
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEWDRAIN, OnUpdateToolsNewDrain)
	ON_COMMAND(ID_TOOLS_NEWDRAIN, OnToolsNewDrain)

	// ID_TOOLS_COLORS
	ON_COMMAND(ID_TOOLS_COLORS, &CWPhastDoc::OnToolsColors)

	// ID_HELP_USER_GUIDE_PDF
	ON_COMMAND(ID_HELP_USER_GUIDE_PDF, &CWPhastDoc::OnHelpUserGuidePdf)
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
, m_pModel(0)
, ProjectionMode(PT_PERSPECTIVE)
, m_pMapActor(0)
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
, CoordinateMode(GridMode)
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

	// grid actor
	//
	this->m_pGridActor = CGridActor::New();
	ASSERT(this->m_pGridActor);
	this->m_pGridActor->AddObserver(CGridActor::DeleteGridLineEvent, this->GridCallbackCommand);
	this->m_pGridActor->AddObserver(CGridActor::InsertGridLineEvent, this->GridCallbackCommand);
	this->m_pGridActor->AddObserver(CGridActor::MoveGridLineEvent,   this->GridCallbackCommand);
	this->m_pGridActor->AddObserver(CGridActor::RotateGridEvent,     this->GridCallbackCommand);
	this->m_pGridActor->SetScale(1, 1, 1);
	this->m_pGridActor->SetPickable(0);

	// colors
	//
	this->SetDisplayColors(this->DisplayColors);
}

///////////////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////////////
#define ASSERT_DELETE_SET_NULL_MACRO(POINTER) \
	ASSERT(POINTER); \
	if (POINTER) { \
		delete POINTER; \
		POINTER = 0; \
	}
///////////////////////////////////////////////////////////////////////////////////////

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

	PDH_RAW_COUNTER start;
	pdh_status = PdhCollectQueryData(hquery);
	if (pdh_status == ERROR_SUCCESS)
	{
		DWORD dwType;
		pdh_status = PdhGetRawCounterValue(hcounter, &dwType, &start);
		if (pdh_status == ERROR_SUCCESS)
		{
			TRACE("Before serialize %lld %lld\n", start.TimeStamp, start.FirstValue);
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
			// store release date
			char *date = RELEASE_DATE;
			CGlobal::HDFSerializeStringOrNull(bStoring, wphast_id, "RELEASE_DATE", &date);

			// store svn release
			char *rev = REVISION;
			CGlobal::HDFSerializeStringOrNull(bStoring, wphast_id, "REVISION", &rev);

			// store flowonly
			ASSERT(this->m_pModel);
			this->m_pModel->m_title.Serialize(bStoring, wphast_id);
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

		// pull in the external files
		//
		std::map<CString, CString> notused;
		this->SerializeFiles(bStoring, pFile, notused);
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
		CDelayRedraw tree(pWndTreeCtrl);
		CDelayRedraw bar(pWndTreeCtrl ? pWndTreeCtrl->GetParent() : 0);

		// delay redrawing box properties window
		//
		CDelayRedraw box(this->GetBoxPropertiesDialogBar());

		// delay rendering the scene
		//
		CDelayRedraw scene(0, this);

		// This fixes the BUG that occurred when creating a new document
		// and then opening an existing document causing a crash in 
		// CWPhastView::Select() called from CWPhastView::Update the
		// CWPhastView::CurrentProp had probably been deleted
		this->ClearSelection();

		CHDFMirrorFile* pFile = (CHDFMirrorFile*)ar.GetFile();
		ASSERT(pFile->GetHID() > 0);
		if (pFile->GetHID() < 0) return;

		// push out the external files (if missing)
		ASSERT(this->GetOriginal2New().size() == 0);
		this->GetOriginal2New().clear();
		this->SerializeFiles(bStoring, pFile, this->GetOriginal2New());

		// Check for missing files
		std::string errors;
		if (this->ValidateData_sourceFiles(pFile, this->GetOriginal2New(), errors))
		{
			// Update StatusBar
			//
			if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
			{
				CString status;
				status.LoadString(AFX_IDS_IDLEMESSAGE);
				pWnd->SetWindowText(status);
			}
			::AfxMessageBox(errors.c_str());
			::AfxThrowUserException();
		}

		// Open the "/WPhast" group
		//
		hid_t wphast_id = ::H5Gopen(pFile->GetHID(), szWPhast);
		ASSERT(wphast_id > 0);
		if (wphast_id > 0)
		{
			std::istringstream iss("");
			CPhastInput* pInput = CPhastInput::New(iss, "Serialize", false);
			try
			{
				// Note: can't call this->New(...) here since the defaults are unknown
				// until this->SerializeMedia and this->SerializeIC are called

				// load model (flowonly freeSurface steadyFlow)
				ASSERT(this->m_pModel);
				this->m_pModel->m_title.Serialize(bStoring, wphast_id);
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
				CSiteMap3 siteMap3 = this->m_pMapActor->GetSiteMap3();
				if (siteMap3.FileName.empty())
				{
					this->m_pMapActor->Delete();
					this->m_pMapActor = 0;
				}
				else
				{
					if (this->SiteMap3.FileName.empty())
					{
						this->m_pMapActor->SetSiteMap3(siteMap3);
					}
					else
					{
						// SiteMap files were recreated so reset
						// pathnames
						this->m_pMapActor->SetSiteMap3(this->SiteMap3);
					}
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
			}
			catch (int)
			{
				::AfxMessageBox(pInput->GetErrorMsg());
				pInput->Delete();
				::AfxThrowUserException();
			}

			pInput->Delete();

			// close WPhast group
			status = ::H5Gclose(wphast_id);
			ASSERT(status >= 0);
		}
		else
		{
			::AfxMessageBox("Unable to open file");
			::AfxThrowUserException();
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

		// display if files were created
		if (::AfxGetMainWnd() && ::AfxGetMainWnd()->IsWindowVisible())
		{
			if (this->GetOriginal2New().size())
			{
				CFileRecreateDialog dlg;
				dlg.SetOriginal2New(this->GetOriginal2New());
				dlg.DoModal();
			}
		}
	}

#if defined(__CPPUNIT__)
	PDH_RAW_COUNTER finish;
	pdh_status = PdhCollectQueryData(hquery);
	if (pdh_status == ERROR_SUCCESS)
	{
		DWORD dwType;
		pdh_status = PdhGetRawCounterValue(hcounter, &dwType, &finish);
		if (pdh_status == ERROR_SUCCESS)
		{
			TRACE("After serialize %lld %lld\n", finish.TimeStamp, finish.FirstValue);
			TRACE("Time %g\n", (finish.TimeStamp.dwLowDateTime - start.TimeStamp.dwLowDateTime)/1e7);
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
				// read old format (this .p4w file was written by a version older than
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
			std::list< std::string > listNames;
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

	if (CPropertyTreeControlBar* pTree = static_cast<CPropertyTreeControlBar*>(pFrame->GetControlBar(IDW_CONTROLBAR_TREE)))
	{
		ASSERT_KINDOF(CPropertyTreeControlBar, pTree);
		ASSERT_VALID(pTree);
		return pTree;
	}
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
	if (bOk)
	{
		if (this->GetOriginal2New().size())
		{
			bool modified = false;
			std::map< CString, CString >::const_iterator cit = this->GetOriginal2New().begin();
			for (; cit != this->GetOriginal2New().end(); ++cit)
			{
				if (cit->first.CompareNoCase(cit->second) != 0)
				{
					modified = true;
					break;
				}
			}
			if (modified)
			{
				this->SetModifiedFlag();
			}
		}
		this->PathsRelativeToAbsolute(lpszPathName);
		this->GetOriginal2New().clear();
	}

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
			pView->UpdateWindow();
			this->UpdateAllViews(0);
		}
	}

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
	this->m_pGridActor->AddObserver(CGridActor::RotateGridEvent,     this->GridCallbackCommand);
	this->m_pGridActor->SetScale(1, 1, 1);
	this->m_pGridActor->SetPickable(0);

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

	if (this->m_pMapActor)
	{
		this->m_pMapActor->Delete();
		this->m_pMapActor = 0;
	}
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

	// cleanup phastinput data
	//
	Clear_NNInterpolatorList();
	FakeFiledata::Clear_fake_file_data_list();
	Clear_file_data_map();
	Clear_KDtreeList();

	// reset mode
	this->CoordinateMode = CWPhastDoc::GridMode;

	// reset import file name
	this->ImportPathName.Empty();

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
	BOOL b = (this->m_pimpl->m_vectorActionsIndex > 0);
	b = b && (this->m_pGridSheet->GetSafeHwnd() == 0);
	b = b && (this->m_pGeometrySheet->GetSafeHwnd() == 0);
	pCmdUI->Enable(b);
}

void CWPhastDoc::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	BOOL b = (this->m_pimpl->m_vectorActionsIndex < this->m_pimpl->m_vectorActions.size());
	b = b && (this->m_pGridSheet->GetSafeHwnd() == 0);
	b = b && (this->m_pGeometrySheet->GetSafeHwnd() == 0);
	pCmdUI->Enable(b);
}

void CWPhastDoc::OnEditUndo()
{
	CWaitCursor wait;
	--(this->m_pimpl->m_vectorActionsIndex);
	ASSERT(this->m_pimpl->m_vectorActionsIndex >= 0);
	this->m_pimpl->m_vectorActions.at(this->m_pimpl->m_vectorActionsIndex)->UnExecute();
	this->SetModifiedFlag(this->m_pimpl->m_vectorActionsIndex != this->m_pimpl->m_lastSaveIndex);
}

void CWPhastDoc::OnEditRedo() // 57644
{
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

	if (this->NewDrainActor)
	{
		this->NewDrainActor->SetScale(scale);
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
	this->UpdateAllViews(0);
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

void CWPhastDoc::SetCTitle(const CTitle &t)
{
	this->m_pModel->m_title = t;
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetCTitle(&this->m_pModel->m_title);
	}
}

CTitle CWPhastDoc::GetCTitle(void)const
{
	return this->m_pModel->m_title;
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
		CSiteMap3 siteMap3 = this->m_pMapActor->GetSiteMap3();
		siteMap3.Angle     = keyword.m_grid_angle;
		siteMap3.Origin[0] = keyword.m_grid_origin[0];
		siteMap3.Origin[1] = keyword.m_grid_origin[1];

		double pt[3] = {0.0, 0.0, 0.0};
		CGrid z = keyword.m_grid[2];
		z.Setup();
		pt[2] = z.coord[0] - .01 *(z.coord[z.count_coord - 1] - z.coord[0]);

		vtkTransform *trans = vtkTransform::New();
		const CUnits& units = this->GetUnits();
		trans->Scale(
			units.horizontal.input_to_si,
			units.horizontal.input_to_si,
			units.vertical.input_to_si);
		trans->TransformPoint(pt, pt);
		siteMap3.Origin[2] = pt[2];
		trans->Delete();

		this->m_pMapActor->SetSiteMap3(siteMap3);
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

	// Update default zones etc.
	//
	this->UpdateGridDomain();
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
	CString str;
	str.Format(szZoneFormat, this->GetNextZoneNumber());
	return str;
}

CString CWPhastDoc::GetNextWedgeName(void)
{
	CString str;
	str.Format(szWedgeFormat, this->GetNextWedgeNumber());
	return str;
}

CString CWPhastDoc::GetNextPrismName(void)
{
	CString str;
	str.Format(szPrismFormat, this->GetNextPrismNumber());
	return str;
}

CString CWPhastDoc::GetNextDomainName(void)
{
	CString str;
	str.Format(szDomainFormat, this->GetNextDomainNumber());
	return str;
}

CString CWPhastDoc::GetNextNullName(void)
{
	CString str;
	str.Format(szNullFormat, this->GetNextNullNumber());
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
	this->GetUsedZoneFlowRatesNumbers(nums);
	if (nums.rbegin() != nums.rend())
	{
		return (*nums.rbegin()) + 1;
	}
	else
	{
		return 1;
	}
}

int CWPhastDoc::GetNextDomainNumber(void)const
{
	std::set<int> nums;
	this->GetUsedDomainNumbers(nums);
	if (nums.rbegin() != nums.rend())
	{
		return (*nums.rbegin()) + 1;
	}
	else
	{
		return 1;
	}
}

int CWPhastDoc::GetNextNullNumber(void)const
{
	std::set<int> nums;
	this->GetUsedNullNumbers(nums);
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

void CWPhastDoc::GetUsedZoneFlowRatesNumbers(std::set<int>& usedNums)const
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

void CWPhastDoc::GetUsedDomainNumbers(std::set<int>& usedNums)const
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
					if (pZoneActor->GetPolyhedronType() == Polyhedron::GRID_DOMAIN)
					{
						// store used n_user numbers
						//
						std::pair< std::set<int>::iterator, bool > pr;
						CString str = pZoneActor->GetName();
						if (str.Find(szDomainFind) == 0)
						{
							int n = ::strtol((const char*)str + 6, &ptr, 10);
							pr = usedNums.insert(n);
							ASSERT(pr.second); // duplicate?
						}
					}
				}
			}
		}
	}
}

void CWPhastDoc::GetUsedNullNumbers(std::set<int>& usedNums)const
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
					if (pZoneActor->GetPolyhedronType() == Polyhedron::NONE)
					{
						// store used n_user numbers
						//
						std::pair< std::set<int>::iterator, bool > pr;
						CString str = pZoneActor->GetName();
						if (str.Find(szNullFind) == 0)
						{
							int n = ::strtol((const char*)str + 6, &ptr, 10);
							pr = usedNums.insert(n);
							ASSERT(pr.second); // duplicate?
						}
					}
				}
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
						CGlobal::PathsRelativeToAbsolute(lpszPathName, this, headIC);
						pICHeadZoneActor->SetData(headIC);
					}
					if (CZoneFlowRateZoneActor *pZoneFlowActor = CZoneFlowRateZoneActor::SafeDownCast(pZoneActor))
					{
						Zone_budget zb = pZoneFlowActor->GetData();
						CGlobal::PathsRelativeToAbsolute(lpszPathName, this, zb);
						pZoneFlowActor->SetData(zb);
					}
					if (pZoneActor->GetPolyhedronType() == Polyhedron::PRISM)
					{
						if (Prism *p = dynamic_cast<Prism*>(pZoneActor->GetPolyhedron()))
						{
							std::string filename;
							filename = p->perimeter.Get_file_name();
							if (filename.length() > 0)
							{
								if (this->GetOriginal2New().size())
								{
									// get new filename
									std::map<CString, CString>::iterator it = this->GetOriginal2New().find(filename.c_str());
									if (it != this->GetOriginal2New().end())
									{
										filename = it->second;
									}
								}
								p->perimeter.Set_file_name(this->GetAbsolutePath(lpszPathName, filename));
							}
							filename = p->top.Get_file_name();
							if (filename.length() > 0)
							{
								if (this->GetOriginal2New().size())
								{
									// get new filename
									std::map<CString, CString>::iterator it = this->GetOriginal2New().find(filename.c_str());
									if (it != this->GetOriginal2New().end())
									{
										filename = it->second;
									}
								}
								p->top.Set_file_name(this->GetAbsolutePath(lpszPathName, filename));
							}
							filename = p->bottom.Get_file_name();
							if (filename.length() > 0)
							{
								if (this->GetOriginal2New().size())
								{
									// get new filename
									std::map<CString, CString>::iterator it = this->GetOriginal2New().find(filename.c_str());
									if (it != this->GetOriginal2New().end())
									{
										filename = it->second;
									}
								}
								p->bottom.Set_file_name(this->GetAbsolutePath(lpszPathName, filename));
							}
						}
					}
				}
			}
		}
	}

	std::map< std::string, Filedata * > temp;
	std::map< std::string, Filedata * >::iterator it = Filedata::file_data_map.begin();
	for (; it != Filedata::file_data_map.end(); ++it)
	{
		std::string relative(it->first);
		std::map<CString, CString>::iterator oit = this->GetOriginal2New().find(it->first.c_str());
		if (oit != this->GetOriginal2New().end())
		{
			relative = oit->second;
		}
		std::string absolute = this->GetAbsolutePath(lpszPathName, relative);
		temp.insert(std::map< std::string, Filedata * >::value_type(absolute, (*it).second));		
	}
	Filedata::file_data_map.swap(temp);
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
					if (CZoneFlowRateZoneActor *pZoneFlowActor = CZoneFlowRateZoneActor::SafeDownCast(pZoneActor))
					{
						Zone_budget zb = pZoneFlowActor->GetData();
						CGlobal::PathsAbsoluteToRelative(lpszPathName, this, zb);

						CTreeCtrl *pTreeCtrl = this->GetPropertyTreeControlBar()->GetTreeCtrl();
						CAction *pAction = new CSetZoneBudgetAction(pZoneFlowActor, pTreeCtrl, zb);
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
		std::string cp_src_path(src_path);
		std::replace(cp_src_path.begin(), cp_src_path.end(), '/', '\\');
		VERIFY(::PathCanonicalize(szOut, cp_src_path.c_str()));
		cp_src_path = szOut;
		VERIFY(::PathRelativePathTo(szOut, lpszPathName, FILE_ATTRIBUTE_NORMAL, cp_src_path.c_str(), FILE_ATTRIBUTE_NORMAL));
		if (::strlen(szOut) > 2 && szOut[0] == '.' && szOut[1] == '\\')
		{
			return std::string(szOut+2);
		}
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
	// Need to change directory in order to serialize external
	// files with relative paths (especially when using automation)
	CSaveCurrentDirectory save(lpszPathName);

	// Add your specialized code here and/or call the base class
	this->m_pimpl->m_lastSaveIndex = this->m_pimpl->m_vectorActionsIndex;

	this->PathsAbsoluteToRelative(lpszPathName);
	BOOL bOk = CDocument::OnSaveDocument(lpszPathName);
	this->PathsRelativeToAbsolute(lpszPathName);
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

	CDocument::OnCloseDocument();
}

void CWPhastDoc::OnFileImport()
{
	CString newName;
	if (!DoPromptFileName(newName, IDS_IMPORT_PHAST_TRANS_442, //AFX_IDS_OPENFILE,
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
		CDelayRedraw del1(this->GetPropertyTreeControlBar(), pDoc);
		CDelayRedraw del3(this->GetBoxPropertiesDialogBar());
		if (!this->DoImport(szPath))
		{
			::AfxMessageBox("An error occurred during the import", MB_OK);
			this->SetModifiedFlag(FALSE);
		}
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

	VERIFY(::_tsplitpath_s(lpszPathName, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

	// change directory in case there's a sitemap
	//
	TCHAR szNewDir[MAX_PATH];
	VERIFY(::_tmakepath_s(szNewDir, _MAX_PATH, szDrive, szDir, NULL, NULL) == 0);


	BOOL b = save.SetCurrentDirectory(szNewDir);

	// remove .trans if it exists
	//
	CString strPrefix(szFName);
	{
		TCHAR szDrive[_MAX_DRIVE];
		TCHAR szDir[_MAX_DIR];
		TCHAR szFName[_MAX_FNAME];
		TCHAR szExt[_MAX_EXT];
		VERIFY(::_tsplitpath_s(strPrefix, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

		if (::_tcsicmp(szExt, _T(".trans")) == 0)
		{
			strPrefix = szFName;
		}
	}

	// get title
	//
	TCHAR szTitle[MAX_PATH];
	VERIFY(::_tmakepath_s(szTitle, _MAX_PATH, NULL, NULL, szFName, szExt) == 0);


	// get fullpath
	//
	TCHAR szFullpath[MAX_PATH];
	VERIFY(::_tfullpath(szFullpath, lpszPathName, MAX_PATH) != NULL);

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

	ASSERT(Prism::prism_list.size() == 0);
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

		CNewModel model;
		model.m_gridKeyword = CGridKeyword(::grid, ::snap, ::axes, ::print_input_xy, ::grid_origin, ::grid_angle);

		//
		// store original polyhs 
		// SaveCoorSystem/RestoreCoorSystem is required to keep Prisms from automatically converted to GRID coordinates
		//

		// MEDIA zones
		for (int i = 0; i < ::count_grid_elt_zones; ++i)
		{
			const struct grid_elt* grid_elt_ptr = ::grid_elt_zones[i];
			if (dynamic_cast<Domain*>(grid_elt_ptr->polyh))
			{
				grid_elt_map[grid_elt_ptr] = 0;
			}
			else
			{
				grid_elt_map[grid_elt_ptr] = grid_elt_ptr->polyh ? grid_elt_ptr->polyh->clone() : 0;
			}
		}
		// BC zones
		for (int i = 0; i < ::count_bc; ++i)
		{
			const struct BC* bc_ptr = ::bc[i];
			if (dynamic_cast<Domain*>(bc_ptr->polyh))
			{
				bc_map[bc_ptr] = 0;
			}
			else
			{
				bc_map[bc_ptr] = bc_ptr->polyh ? bc_ptr->polyh->clone() : 0;
			}
		}
		// Zone_budget zones
		std::map<int, Zone_budget*>::iterator zit = Zone_budget::zone_budget_map.begin();
		for (; zit != Zone_budget::zone_budget_map.end(); ++zit)
		{
			if (dynamic_cast<Domain*>(zit->second->Get_polyh()))
			{
				zb_map[zit->second] = 0;
			}
			else
			{
				zb_map[zit->second] = zit->second->Get_polyh() ? zit->second->Get_polyh()->clone() : 0;
			}
		}
		// IC zones
		for (int i = 0; i < ::count_head_ic; ++i)
		{
			const struct Head_ic* head_ic_ptr = ::head_ic[i];
			if (dynamic_cast<Domain*>(head_ic_ptr->polyh))
			{
				head_ic_map[head_ic_ptr] = 0;
			}
			else
			{
				head_ic_map[head_ic_ptr] = head_ic_ptr->polyh ? head_ic_ptr->polyh->clone() : 0;
			}
		}
		// CHEMISTRY_IC
		for (int i = 0; i < ::count_chem_ic; ++i)
		{
			const struct chem_ic* chem_ic_ptr = ::chem_ic[i];
			if (dynamic_cast<Domain*>(chem_ic_ptr->polyh))
			{
				chem_ic_map[chem_ic_ptr] = 0;
			}
			else
			{
				chem_ic_map[chem_ic_ptr] = chem_ic_ptr->polyh ? chem_ic_ptr->polyh->clone() : 0;
			}
		}

		pInput->Accumulate(false);

		::OutputDebugString("Finished loading trans.dat file\n");
		if (pInput->GetErrorCount() != 0)
		{
			// goto ImportError;
			// throw "Input file contains errors.";
			throw pInput->GetErrorCount();
		}

		// start loading data

		// TITLE
		//
		CTitle t(::title_x);

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
		ASSERT(!gridElt.RemovePropZones());

		// HEAD_IC
		//
		CHeadIC headIC = CHeadIC::Full();
		::GetDefaultHeadIC(&headIC);
		ASSERT(!headIC.RemovePropZones());

		// CHEMISTRY_IC
		//
		CChemIC chemIC = CChemIC::Full();
		::GetDefaultChemIC(&chemIC);
		ASSERT(!chemIC.RemovePropZones());

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
		model.m_title          = t;
		model.m_flowOnly       = flowOnly;
		model.m_freeSurface    = (::free_surface != 0);
		model.m_steadyFlow     = steadyFlow;
		model.m_units          = CUnits(::units);
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
			}

			// store pre-translated polyh
			CGridElt data(*grid_elt_ptr);
			data.RemovePropZones();

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
			data.RemovePropZones();

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
			{
				if (zb_map[it->second] || it->second->Get_polyh())
				{
					data.Set_polyh(zb_map[it->second] ? zb_map[it->second]->clone() : it->second->Get_polyh()->clone());
				}
				else
				{
					data.Set_polyh(new NullPolyhedron());
				}

				// not undoable
				std::auto_ptr< CZoneCreateAction<CZoneFlowRateZoneActor> > pAction(
					new CZoneCreateAction<CZoneFlowRateZoneActor>(
						this,
						data.Get_polyh(),
						::grid_origin,
						::grid_angle,
						data.Get_description().c_str()
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
			data.RemovePropZones();

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
								chemIC.RemovePropZones();

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
			data.RemovePropZones();

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
		this->PathsRelativeToAbsolute(lpszPathName);

		if (::AfxGetMainWnd() && ::AfxGetMainWnd()->IsWindowVisible())
		{
			// display warnings if any
			if (pInput->GetWarningCount() != 0)
			{
				CImportWarningDialog dlg;
				dlg.WarningMessages = pInput->GetWarningMsg();
				dlg.DoModal();
			}
		}
	}
	catch (int)
	{
		if (::AfxGetMainWnd() && ::AfxGetMainWnd()->IsWindowVisible())
		{
			CImportErrorDialog dlg;
			dlg.m_lpszErrorMessages = pInput->GetErrorMsg();
			dlg.DoModal();
		}

		// load a default model
		CNewModel model;
		this->New(model);
	}
	catch (const char * error)
	{
		if (::AfxGetMainWnd() && ::AfxGetMainWnd()->IsWindowVisible())
		{
			::AfxMessageBox(error, MB_OK|MB_ICONEXCLAMATION);
		}

		// load a default model
		CNewModel model;
		this->New(model);
		bReturnValue = TRUE;
	}
#if !defined(_DEBUG)
	catch (...)
	{
		if (::AfxGetMainWnd() && ::AfxGetMainWnd()->IsWindowVisible())
		{
			::AfxMessageBox("An unknown error occurred during import", MB_OK|MB_ICONEXCLAMATION);
		}

		// load a default model
		CNewModel model;
		this->New(model);
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

	// store path
	this->ImportPathName = lpszPathName;

	return bReturnValue;
}

void CWPhastDoc::OnFileExport()
{
	CString newName;
	newName = this->GetTitle();

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];

	VERIFY(::_tsplitpath_s(newName.GetBuffer(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);
	newName = szFName;
	if (::_tcsicmp(szExt, _T(".p4w")) != 0)
	{
		newName += szExt;
	}
	newName += _T(".trans.dat");

	if (!DoPromptFileName(newName, IDS_EXPORT_PHAST_TRANS_136,
	  OFN_HIDEREADONLY, FALSE))
		return; // save cancelled

	// modified from: CDocument* CDocManager::OpenDocumentFile
	TCHAR szPath[_MAX_PATH];
	ASSERT(lstrlen(newName) < sizeof(szPath)/sizeof(szPath[0]));
	TCHAR szTemp[_MAX_PATH];
	if (newName[0] == _T('\"'))
		newName = newName.Mid(1);
	lstrcpyn(szTemp, newName, _MAX_PATH);
	LPTSTR lpszLast = _tcsrchr(szTemp, _T('\"'));
	if (lpszLast != NULL)
		*lpszLast = 0;
	SrcFullPath(szPath, szTemp);
	TCHAR szLinkName[_MAX_PATH];
	if (SrcResolveShortcut(AfxGetMainWnd(), szPath, szLinkName, _MAX_PATH))
		lstrcpy(szPath, szLinkName);

	CString strPath(szPath);
	if (strPath.Right(10).CompareNoCase(_T(".trans.dat")) != 0)
	{
		strPath += _T(".trans.dat");
	}

	this->PathsAbsoluteToRelative(strPath);
	if (!this->DoExport(strPath))
	{
		::AfxMessageBox(_T("An error occurred during the export"), MB_OK);
	}
	this->PathsRelativeToAbsolute(strPath);
}

BOOL CWPhastDoc::DoExport(LPCTSTR lpszPathName)
{
	std::ofstream ofs;
	ofs.open(lpszPathName);
	if (!ofs.is_open())
	{
		CString str(_T("Unable to open \""));
		str += lpszPathName;
		str += _T("\" for writing.");
		::AfxMessageBox(str, MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	// preserve cd
	CSaveCurrentDirectory save;

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	VERIFY(::_tsplitpath_s(lpszPathName, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

	TCHAR szDirectory[_MAX_DIR];
	VERIFY(::_tmakepath_s(szDirectory, _MAX_DIR, szDrive, szDir, NULL, NULL) == 0);
	save.SetCurrentDirectory(szDirectory);

	CString strPrefix(szFName);
	if (strPrefix.Right(6).CompareNoCase(_T(".trans")) == 0)
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

	// TITLE
	os << this->m_pModel->m_title;

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
							if (CPointConnectorActor *pPointConnectorActor = CPointConnectorActor::SafeDownCast(prop3D))
							{
								pPointConnectorActor->SetUnits(units);
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
		pView->ResetSelection();
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
	Domain domain(&zone);

	// default media
	//
	CZoneCreateAction<CMediaZoneActor>* pMediaAction = new CZoneCreateAction<CMediaZoneActor>(
		this,
		"Default",
		&domain,
		model.m_gridKeyword.m_grid_origin,
		model.m_gridKeyword.m_grid_angle,
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
		model.m_gridKeyword.m_grid_origin,
		model.m_gridKeyword.m_grid_angle,
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
		model.m_gridKeyword.m_grid_origin,
		model.m_gridKeyword.m_grid_angle,
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

	if (model.HasSiteMap3())
	{
		ASSERT(this->m_pMapActor == NULL);
		this->m_pMapActor = CMapActor::New();   // Note pixel(0,0) is the same size as all other pixels
		this->m_pMapActor->SetSiteMap3(model.GetSiteMap3());
		this->m_pMapActor->SetPickable(0);
		this->GetPropCollection()->AddItem(this->m_pMapActor);
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
	VERIFY(::_tsplitpath_s(this->GetPathName(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

	TCHAR szOutDir[_MAX_PATH];
	VERIFY(::_tmakepath_s(szOutDir, _MAX_PATH, szDrive, szDir, NULL, NULL) == 0);
	scd.SetCurrentDirectoryA(szOutDir);

	TCHAR szPhastTmp[_MAX_PATH];
	VERIFY(::_tmakepath_s(szPhastTmp, _MAX_PATH, szDrive, szDir, _T("Phast"), _T(".tmp")) == 0);

	CString strPrefix(szFName);

	std::ostringstream oss;
	this->PathsAbsoluteToRelative(szPhastTmp);
	this->WriteTransDat(oss);
	this->PathsRelativeToAbsolute(szPhastTmp);

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
		::AfxMessageBox(_T("An unknown error occurred during the run."), MB_OK|MB_ICONEXCLAMATION);
	}
	pInput->Delete();

	if (bWritePhastTmpOK)
	{
		wait.Restore();

		TCHAR szPhastTmpDir[_MAX_PATH];
		VERIFY(::_tmakepath_s(szPhastTmpDir, _MAX_PATH, szDrive, szDir, _T(""), _T("")) == 0);

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
		if (POSITION pos = this->GetFirstViewPosition())
		{
			if (CWPhastView *pView = dynamic_cast<CWPhastView*>(this->GetNextView(pos)))
			{					
				if (pView->GetInteractor())
				{
					if (pView->GetInteractor()->GetEnableRender())
					{
						this->Notify(0, WPN_SELCHANGED, 0, pZoneActor);
					}
				}
			}
		}
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
		////if (::AfxGetMainWnd()->IsWindowVisible())
		{
			// must always call this otherwise there may be problems
			// with serializing prisms (parts lose reference counts)
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

// COMMENT: {1/25/2011 5:08:26 PM}	// set radius
// COMMENT: {1/25/2011 5:08:26 PM}	//
// COMMENT: {1/25/2011 5:08:26 PM}	pRiverActor->ScaleFromBounds(this->GetGridBounds());

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
		pRiverActor->ScaleFromBounds(this->GetGridBounds(), pView->GetRenderer());
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
	pRiverActor->AddObserver(CRiverActor::DeletePointEvent,    this->RiverCallbackCommand);

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

	if (CRiverActor* riverActor = CRiverActor::SafeDownCast(caller))
	{
		CWPhastDoc* self = reinterpret_cast<CWPhastDoc*>(clientdata);

		switch (eid)
		{
		case CRiverActor::StartMovePointEvent:
			{
				ASSERT(self->RiverMovePointAction == 0);
				double* pos = riverActor->GetCurrentPointPosition();
				self->RiverMovePointAction = new CPointConnectorMovePointAction<CRiverActor>(riverActor, self, riverActor->GetCurrentPointId(), pos[0], pos[1]);
			}
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
			{
				ASSERT(self->RiverMovePointAction != 0);
				double* pos = riverActor->GetCurrentPointPosition();
				self->RiverMovePointAction->SetPoint(pos[0], pos[1]);
				self->Execute(self->RiverMovePointAction);
				self->RiverMovePointAction = 0;
			}
			break;

		case CRiverActor::InsertPointEvent:
			{
				double* pos = riverActor->GetCurrentPointPosition();
				vtkIdType id = riverActor->GetCurrentPointId() - 1;
				CPointConnectorInsertPointAction<CRiverActor, CRiverPoint>* pRiverInsertPointAction =
					new CPointConnectorInsertPointAction<CRiverActor, CRiverPoint>(riverActor, self, id, pos[0], pos[1], pos[2], true);
				self->Execute(pRiverInsertPointAction);
			}
			break;

		case CRiverActor::DeletePointEvent:
			{
				ASSERT(calldata);
				vtkIdType point = *((vtkIdType*)calldata);
				if (point == 0 || point == riverActor->GetPointCount() - 1)
				{
					::AfxMessageBox("The first and the last river points cannot be deleted.");
				}
				else
				{
					self->Execute(new CPointConnectorDeletePointAction<CRiverActor, CRiverPoint>(riverActor, self, point));
				}
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
								pZone->SetGridAngle(gridKeyword.m_grid_angle);
								pZone->SetGridOrigin(gridKeyword.m_grid_origin);
							}
							if (CWellActor *pWellActor = CWellActor::SafeDownCast(prop3D))
							{
								pWellActor->SetGridKeyword(gridKeyword);
							}
							if (CPointConnectorActor *pPointConnectorActor = CPointConnectorActor::SafeDownCast(prop3D))
							{
								pPointConnectorActor->SetGridKeyword(gridKeyword);
							}
						}
					}
				}
			}
		}
	}
}

void CWPhastDoc::RotateGrid(const CGridKeyword& gridKeyword)
{
	// 2/17/2011
	// TODO Clean this up.
	// This replaces a call to SetGridKeyword skipping the call to
	// CGridActor::Setup (for speed reasons)

	////this->ResizeGrid(gridKeyword);
	{
		if (this->m_pMapActor)
		{
			CSiteMap3 siteMap3 = this->m_pMapActor->GetSiteMap3();
			siteMap3.Angle     = gridKeyword.m_grid_angle;
			siteMap3.Origin[0] = gridKeyword.m_grid_origin[0];
			siteMap3.Origin[1] = gridKeyword.m_grid_origin[1];

			double pt[3] = {0.0, 0.0, 0.0};
			CGrid z = gridKeyword.m_grid[2];
			z.Setup();
			pt[2] = z.coord[0] - .01 *(z.coord[z.count_coord - 1] - z.coord[0]);

			vtkTransform *trans = vtkTransform::New();
			const CUnits& units = this->GetUnits();
			trans->Scale(
				units.horizontal.input_to_si,
				units.horizontal.input_to_si,
				units.vertical.input_to_si);
			trans->TransformPoint(pt, pt);
			siteMap3.Origin[2] = pt[2];
			trans->Delete();

			this->m_pMapActor->SetSiteMap3(siteMap3);
		}

		// reset the grid
		//
		this->m_pGridActor->SetGridKeyword(gridKeyword, this->GetUnits(), false);
		this->m_pGridActor->SetPickable(0);
		this->m_pPropCollection->AddItem(this->m_pGridActor);
		if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
		{
			pTree->SetGridActor(this->m_pGridActor);
		}

		// Update default zones etc.
		//
		////this->UpdateGridDomain();    Domains don't change on a grid rotation
	}

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
								pZone->SetGridAngle(gridKeyword.m_grid_angle);
								pZone->SetGridOrigin(gridKeyword.m_grid_origin);
							}
							if (CWellActor *pWellActor = CWellActor::SafeDownCast(prop3D))
							{
								pWellActor->SetGridKeyword(gridKeyword);
							}
							if (CPointConnectorActor *pPointConnectorActor = CPointConnectorActor::SafeDownCast(prop3D))
							{
								pPointConnectorActor->SetGridKeyword(gridKeyword);
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
		case CGridActor::RotateGridEvent:
			{
				CGridKeyword gk = *(CGridKeyword*)calldata;
				CGridRotateAction* pGridRotateAction = new CGridRotateAction(self, grid, gk);
				self->Execute(pGridRotateAction);
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

		// setup domain
		this->GetDefaultZone(::domain);
		CGridKeyword k = this->GetGridKeyword();

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
							if (dynamic_cast<Prism*>(pZone->GetPolyhedron()))
							{
								// if any parts of the prism are Data_source::NONE
								// the polyhedron must be updated

								bool bNeedToResetPoly = false;
								Prism p(*dynamic_cast<Prism*>(pZone->GetPolyhedron()));

								if (p.top.Get_user_source_type() == Data_source::NONE)
								{
									bNeedToResetPoly = true;
									p.top.Set_source_type(Data_source::NONE);
									p.top.Get_points().clear();
								}
								if (p.perimeter.Get_user_source_type() == Data_source::NONE)
								{
									bNeedToResetPoly = true;
									p.perimeter.Set_source_type(Data_source::NONE);
									p.perimeter.Get_points().clear();
								}
								if (p.bottom.Get_user_source_type() == Data_source::NONE)
								{
									bNeedToResetPoly = true;
									p.bottom.Set_source_type(Data_source::NONE);
									p.bottom.Get_points().clear();
								}
								if (bNeedToResetPoly)
								{
									pZone->SetPolyhedron(&p, this->GetUnits(), k.m_grid_origin, k.m_grid_angle);
								}
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
				ASSERT(FALSE); // otherwise need to check to reset polyhedron
			}
		}
	}

	CGrid x, y, z;
	this->GetGrid(x, y, z);
	z.Setup();

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
				if (CPointConnectorActor* pActor = CPointConnectorActor::SafeDownCast(pProp))
				{
					pActor->SetZ(z.coord[z.count_coord - 1]);
				}
			}
		}
	}

	// update drains
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyDrains())
	{
		if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
		{
			vtkProp *pProp;
			vtkCollectionSimpleIterator csi;
			pPropCollection->InitTraversal(csi);
			for (; (pProp = pPropCollection->GetNextProp(csi)); )
			{
				if (CPointConnectorActor* pActor = CPointConnectorActor::SafeDownCast(pProp))
				{
					pActor->SetZ(z.coord[z.count_coord - 1]);
				}
			}
		}
	}


	// update wells
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyWells())
	{
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
					pWellActor->SetZAxis(z, this->GetUnits());

				}
			}
		}
	}

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
		pTree->SetICCheck(BST_UNCHECKED);
		pTree->SetBCCheck(BST_UNCHECKED);
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
#if defined(CANCEL_WHEN_GRID_UNCHECKED)
			if (this->GridElementsSelector && !this->ModifyGridSheet)
			{
				this->EndModifyGrid();
			}
#endif
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
							if (CPointConnectorActor *pPointConnectorActor = CPointConnectorActor::SafeDownCast(prop3D))
							{
								pPointConnectorActor->SetRadius(0.008 * size);
								ASSERT(this->NewDrainActor != pPointConnectorActor);
							}
						}
					}
				}
			}
		}
	}

	if (this->NewDrainActor)
	{
		this->NewDrainActor->SetRadius(0.008 * size);
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
			this->NewZoneWidget->SetCursorColor(this->ZoneCursorColor);
			this->NewZoneWidget->SetInteractor(pView->GetInteractor());
			this->NewZoneWidget->SetProp3D(this->GetGridActor());

			// add listener callback
			this->NewZoneCallbackCommand = vtkCallbackCommand::New();
			this->NewZoneCallbackCommand->SetClientData(this);
			this->NewZoneCallbackCommand->SetCallback(CWPhastDoc::NewZoneListener);
			this->NewZoneWidget->AddObserver(vtkCommand::EndInteractionEvent, this->NewZoneCallbackCommand);

			// set up for gridkeyword
			CGridKeyword gridKeyword;
			this->GetGridKeyword(gridKeyword);
			const CUnits& units = this->GetUnits();
			this->NewZoneWidget->SetGridKeyword(gridKeyword, units);
			this->NewZoneWidget->SetScale(this->GetScale()[0], this->GetScale()[1], this->GetScale()[2]);
			this->NewZoneWidget->SetCoordinateMode(this->GetCoordinateMode());

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
			// get type of zone
			//
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

			bool bFreeSufurface = self->GetFreeSurface();
			leakyProps.SetFreeSurface(bFreeSufurface);

			// set used zone flow rate numbers
			std::set<int> usedZoneFlowRatesNumbers;
			self->GetUsedZoneFlowRatesNumbers(usedZoneFlowRatesNumbers);
			zoneFlowRateProps.SetUsedZoneFlowRates(usedZoneFlowRatesNumbers);

			// set units
			mediaProps.SetUnits(self->GetUnits());
			fluxProps.SetUnits(self->GetUnits());
			leakyProps.SetUnits(self->GetUnits());
			specifiedProps.SetUnits(self->GetUnits());
			headProps.SetUnits(self->GetUnits());
			chemICProps.SetUnits(self->GetUnits());
			//zoneFlowRateProps.SetUnits(self->GetUnits());

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
					elt.polyh = self->NewZoneWidget->GetCube();
					CMediaZoneActor::Create(self, elt, mediaProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_FLUX)
				{
					CBC bc;
					fluxProps.GetProperties(bc);
					bc.polyh = self->NewZoneWidget->GetCube();
					CBCZoneActor::Create(self, bc, fluxProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_LEAKY)
				{
					CBC bc;
					leakyProps.GetProperties(bc);
					bc.polyh = self->NewZoneWidget->GetCube();
					CBCZoneActor::Create(self, bc, leakyProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_SPECIFIED)
				{
					CBC bc;
					specifiedProps.GetProperties(bc);
					bc.polyh = self->NewZoneWidget->GetCube();
					CBCZoneActor::Create(self, bc, specifiedProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_IC_HEAD)
				{
					CHeadIC headic;
					headProps.GetProperties(headic);
					headic.polyh = self->NewZoneWidget->GetCube();
					CICHeadZoneActor::Create(self, headic, headProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_IC_CHEM)
				{
					CChemIC chemIC;
					chemICProps.GetProperties(chemIC);
					chemIC.polyh = self->NewZoneWidget->GetCube();
					CICChemZoneActor::Create(self, chemIC, chemICProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_FLOW_RATE)
				{
					Zone_budget zone_budget;
					zoneFlowRateProps.GetProperties(zone_budget);
					zone_budget.Set_polyh(self->NewZoneWidget->GetCube());
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

	// zone cursors
	this->ZoneCursorColor[0] = (double)GetRValue(this->DisplayColors.crZoneCursor)/255.;
	this->ZoneCursorColor[1] = (double)GetGValue(this->DisplayColors.crZoneCursor)/255.;
	this->ZoneCursorColor[2] = (double)GetBValue(this->DisplayColors.crZoneCursor)/255.;

	if (this->NewPrismWidget)
	{
		this->NewPrismWidget->SetCursorColor(this->ZoneCursorColor);
	}
	if (this->NewWedgeWidget)
	{
		this->NewWedgeWidget->SetCursorColor(this->ZoneCursorColor);
	}
	if (this->NewZoneWidget)
	{
		this->NewZoneWidget->SetCursorColor(this->ZoneCursorColor);
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
			if (this->NewWedgeWidget = CNewWedgeWidget::New())
			{			
				this->NewWedgeWidget->SetCursorColor(this->ZoneCursorColor);
				this->NewWedgeWidget->SetInteractor(pView->GetInteractor());
				this->NewWedgeWidget->SetProp3D(this->GetGridActor());

				// add listener callback
				this->NewWedgeCallbackCommand = vtkCallbackCommand::New();
				this->NewWedgeCallbackCommand->SetClientData(this);
				this->NewWedgeCallbackCommand->SetCallback(CWPhastDoc::NewWedgeListener);
				this->NewWedgeWidget->AddObserver(vtkCommand::EndInteractionEvent, this->NewWedgeCallbackCommand);

				// set up for gridkeyword
				CGridKeyword gridKeyword;
				this->GetGridKeyword(gridKeyword);
				const CUnits& units = this->GetUnits();
				this->NewWedgeWidget->SetGridKeyword(gridKeyword, units);
				this->NewWedgeWidget->SetScale(this->GetScale()[0], this->GetScale()[1], this->GetScale()[2]);
				this->NewWedgeWidget->SetCoordinateMode(this->GetCoordinateMode());

				// enable widget
				this->NewWedgeWidget->SetEnabled(1);
			}
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

			bool bFreeSufurface = self->GetFreeSurface();
			leakyProps.SetFreeSurface(bFreeSufurface);

			// set used zone flow rate numbers
			std::set<int> usedZoneFlowRatesNumbers;
			self->GetUsedZoneFlowRatesNumbers(usedZoneFlowRatesNumbers);
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
					elt.polyh = self->NewWedgeWidget->GetWedge();
					CMediaZoneActor::Create(self, elt, mediaProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_FLUX)
				{
					CBC bc;
					fluxProps.GetProperties(bc);
					bc.polyh = self->NewWedgeWidget->GetWedge();
					CBCZoneActor::Create(self, bc, fluxProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_LEAKY)
				{
					CBC bc;
					leakyProps.GetProperties(bc);
					bc.polyh = self->NewWedgeWidget->GetWedge();
					CBCZoneActor::Create(self, bc, leakyProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_BC_SPECIFIED)
				{
					CBC bc;
					specifiedProps.GetProperties(bc);
					bc.polyh = self->NewWedgeWidget->GetWedge();
					CBCZoneActor::Create(self, bc, specifiedProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_IC_HEAD)
				{
					CHeadIC headic;
					headProps.GetProperties(headic);
					headic.polyh = self->NewWedgeWidget->GetWedge();
					CICHeadZoneActor::Create(self, headic, headProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_IC_CHEM)
				{
					CChemIC chemIC;
					chemICProps.GetProperties(chemIC);
					chemIC.polyh = self->NewWedgeWidget->GetWedge();
					CICChemZoneActor::Create(self, chemIC, chemICProps.GetDesc());
				}
				else if (newZone.GetType() == ID_ZONE_TYPE_FLOW_RATE)
				{
					Zone_budget zone_budget;
					zoneFlowRateProps.GetProperties(zone_budget);
					zone_budget.Set_polyh(self->NewWedgeWidget->GetWedge());
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
					::AfxMessageBox("An unknown error occurred while loading perimeter file.", MB_OK|MB_ICONEXCLAMATION);
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

					bool bFreeSufurface = this->GetFreeSurface();
					leakyProps.SetFreeSurface(bFreeSufurface);

					// set used zone flow rate numbers
					std::set<int> usedZoneFlowRatesNumbers;
					this->GetUsedZoneFlowRatesNumbers(usedZoneFlowRatesNumbers);
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
			this->NewPrismWidget->SetCursorColor(this->ZoneCursorColor);
			this->NewPrismWidget->SetInteractor(pView->GetInteractor());
			this->NewPrismWidget->SetProp3D(this->GetGridActor());
			this->NewPrismWidget->AddObserver(vtkCommand::EndInteractionEvent, this->NewPrismCallbackCommand);

			// set up for gridkeyword
			CGridKeyword gridKeyword;
			this->GetGridKeyword(gridKeyword);
			const CUnits& units = this->GetUnits();
			this->NewPrismWidget->SetGridKeyword(gridKeyword, units);
			this->NewPrismWidget->SetScale(this->GetScale()[0], this->GetScale()[1], this->GetScale()[2]);
			this->NewPrismWidget->SetCoordinateMode(this->GetCoordinateMode());

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
			vtkPoints *points = self->NewPrismWidget->GetPoints();
			if (points->GetNumberOfPoints() < 6)
			{
				::AfxMessageBox(_T("Perimeter must be defined by at least 3 points."));

				// Note: cannot call EndNewPrism here
				self->NewPrismWidget->SetInteractor(0);
				return;
			}

			// setup domain (reqd before call to Prism::Tidy)
			self->GetDefaultZone(::domain);

			if (Prism *p = self->NewPrismWidget->GetPrism())
			{
				// DO NOT CALL Tidy here
				//p->Tidy();

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

				bool bFreeSufurface = self->GetFreeSurface();
				leakyProps.SetFreeSurface(bFreeSufurface);

				// set used zone flow rate numbers
				std::set<int> usedZoneFlowRatesNumbers;
				self->GetUsedZoneFlowRatesNumbers(usedZoneFlowRatesNumbers);
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

void CWPhastDoc::OnUpdateToolsNewDomain(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

void CWPhastDoc::OnToolsNewDomain()
{
	// calc zone
	CZone zone;
	this->GetDefaultZone(zone);

	// get type of zone
	//
	ETSLayoutPropertySheet        sheet("Domain Wizard", NULL, 0, NULL, false);

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

	bool bFreeSufurface = this->GetFreeSurface();
	leakyProps.SetFreeSurface(bFreeSufurface);

	// set used zone flow rate numbers
	std::set<int> usedZoneFlowRatesNumbers;
	this->GetUsedZoneFlowRatesNumbers(usedZoneFlowRatesNumbers);
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
			elt.polyh = new Domain(&zone);
			CMediaZoneActor::Create(this, elt, mediaProps.GetDesc());
		}
		else if (newZone.GetType() == ID_ZONE_TYPE_BC_FLUX)
		{
			CBC bc;
			fluxProps.GetProperties(bc);
			bc.polyh = new Domain(&zone);
			CBCZoneActor::Create(this, bc, fluxProps.GetDesc());
		}
		else if (newZone.GetType() == ID_ZONE_TYPE_BC_LEAKY)
		{
			CBC bc;
			leakyProps.GetProperties(bc);
			bc.polyh = new Domain(&zone);
			CBCZoneActor::Create(this, bc, leakyProps.GetDesc());
		}
		else if (newZone.GetType() == ID_ZONE_TYPE_BC_SPECIFIED)
		{
			CBC bc;
			specifiedProps.GetProperties(bc);
			bc.polyh = new Domain(&zone);
			CBCZoneActor::Create(this, bc, specifiedProps.GetDesc());
		}
		else if (newZone.GetType() == ID_ZONE_TYPE_IC_HEAD)
		{
			CHeadIC headic;
			headProps.GetProperties(headic);
			headic.polyh = new Domain(&zone);
			CICHeadZoneActor::Create(this, headic, headProps.GetDesc());
		}
		else if (newZone.GetType() == ID_ZONE_TYPE_IC_CHEM)
		{
			CChemIC chemIC;
			chemICProps.GetProperties(chemIC);
			chemIC.polyh = new Domain(&zone);
			CICChemZoneActor::Create(this, chemIC, chemICProps.GetDesc());
		}
		else if (newZone.GetType() == ID_ZONE_TYPE_FLOW_RATE)
		{
			Zone_budget zone_budget;
			zoneFlowRateProps.GetProperties(zone_budget);
			zone_budget.Set_polyh(new Domain(&zone));
			CZoneFlowRateZoneActor::Create(this, zone_budget);
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
			this->NewDrainActor->SetCoordinateMode(this->GetCoordinateMode());
			this->NewDrainActor->AddObserver(CRiverActor::EndNewEvent, this->NewDrainCallbackCommand);
			this->NewDrainActor->AddObserver(CRiverActor::CancelNewEvent, this->NewDrainCallbackCommand);

			// scale
			double* scale = this->GetScale();
			this->NewDrainActor->SetScale(scale[0], scale[1], scale[2]);

			this->NewDrainActor->ScaleFromBounds(this->GetGridBounds(), pView->GetRenderer());

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

				ASSERT(drain.coordinate_system == PHAST_Transform::GRID); // new drain default
				if (this->GetCoordinateMode() == CWPhastDoc::MapMode)
				{
					const CUnits& units = this->GetUnits();
					const CGridKeyword& gridKeyword = this->GetGridKeyword();
					PHAST_Transform map2grid(
						gridKeyword.m_grid_origin[0],
						gridKeyword.m_grid_origin[1],
						gridKeyword.m_grid_origin[2],
						gridKeyword.m_grid_angle,
						units.map_horizontal.input_to_si/units.horizontal.input_to_si,
						units.map_horizontal.input_to_si/units.horizontal.input_to_si,
						units.map_vertical.input_to_si/units.vertical.input_to_si
						);

					drain.coordinate_system = PHAST_Transform::MAP;

					std::list<CRiverPoint>::iterator it = drain.m_listPoints.begin();
					for (; it != drain.m_listPoints.end(); ++it)
					{
						Point p(
							it->x_user,
							it->y_user,
							0.0
							);

						map2grid.Inverse_transform(p);
						it->x_user = p.x();
						it->y_user = p.y();
					}
				}

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

// COMMENT: {1/25/2011 5:10:43 PM}	// set radius
// COMMENT: {1/25/2011 5:10:43 PM}	//
// COMMENT: {1/25/2011 5:10:43 PM}	pDrainActor->ScaleFromBounds(this->GetGridBounds());

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
		pDrainActor->ScaleFromBounds(this->GetGridBounds(), pView->GetRenderer());
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
	pDrainActor->AddObserver(CDrainActor::DeletePointEvent,    this->DrainCallbackCommand);

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
				if (self->DrainMovePointAction->GetActor()->GetCoordinateSystem() == PHAST_Transform::MAP)
				{
					::_sntprintf(buffer, 80, "%g[%s] x %g[%s]",
						self->DrainMovePointAction->GetActor()->GetCurrentPointPosition()[0],
						units.map_horizontal.defined ? units.map_horizontal.input : units.map_horizontal.si,
						self->DrainMovePointAction->GetActor()->GetCurrentPointPosition()[1],
						units.map_horizontal.defined ? units.map_horizontal.input : units.horizontal.si
						);
				}
				else
				{
					::_sntprintf(buffer, 80, "%g[%s] x %g[%s]",
						self->DrainMovePointAction->GetActor()->GetCurrentPointPosition()[0],
						units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
						self->DrainMovePointAction->GetActor()->GetCurrentPointPosition()[1],
						units.horizontal.defined ? units.horizontal.input : units.horizontal.si
						);
				}
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

		case CDrainActor::DeletePointEvent:
			{
				ASSERT(calldata);
				vtkIdType point = *((vtkIdType*)calldata);
				if (point == 0 || point == drainActor->GetPointCount() - 1)
				{
					::AfxMessageBox("The first and the last drain points cannot be deleted.");
				}
				else
				{
					self->Execute(new CPointConnectorDeletePointAction<CDrainActor, CRiverPoint>(drainActor, self, point));
				}
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

CWPhastDoc::CoordinateState CWPhastDoc::GetCoordinateMode()const
{
	return this->CoordinateMode;
}

void CWPhastDoc::SetCoordinateMode(CWPhastDoc::CoordinateState mode)
{
	this->CoordinateMode = mode;

	if (this->NewZoneWidget && this->NewZoneWidget->GetEnabled())
	{
		this->NewZoneWidget->SetCoordinateMode(mode);
	}
	if (this->NewWedgeWidget && this->NewWedgeWidget->GetEnabled())
	{
		this->NewWedgeWidget->SetCoordinateMode(mode);
	}
	if (this->NewPrismWidget && this->NewPrismWidget->GetEnabled())
	{
		this->NewPrismWidget->SetCoordinateMode(mode);
	}
	if (this->NewDrainActor && this->NewDrainActor->GetEnabled())
	{
		this->NewDrainActor->SetCoordinateMode(mode);
	}

	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		pView->SetCoordinateMode(mode);
		pView->GetInteractor()->Render();
	}
}

void CWPhastDoc::SetMapMode(void)
{
	this->SetCoordinateMode(CWPhastDoc::MapMode);
}

void CWPhastDoc::SetGridMode(void)
{
	this->SetCoordinateMode(CWPhastDoc::GridMode);
}

void CWPhastDoc::OnUpdateGridMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(this->CoordinateMode == CWPhastDoc::GridMode);
	pCmdUI->Enable(TRUE);
}

void CWPhastDoc::OnUpdateMapMode(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(this->CoordinateMode == CWPhastDoc::MapMode);
	pCmdUI->Enable(TRUE);
}

void CWPhastDoc::OnSetGridMode()
{
	this->SetCoordinateMode(CWPhastDoc::GridMode);
}

void CWPhastDoc::OnSetMapMode()
{
	this->SetCoordinateMode(CWPhastDoc::MapMode);
}

CString CWPhastDoc::GetDefaultPathName()const
{
	CString path(this->GetPathName());
	if (path.IsEmpty())
	{
		return this->ImportPathName;
	}
	return path;
}

int CWPhastDoc::ValidateData_sourceFiles(CHDFMirrorFile* file, std::map<CString, CString> &orig2new, std::string &errors)
{
	ASSERT(file->GetHID() > 0);
	if (file->GetHID() < 0)
	{
		errors = "Unable to open " + file->GetFilePath() + ", the file may be corrupted.";
		return 1;
	}

	std::map<CString, CString> path2fileMap;
	if (this->GetMapOfData_sourceFiles(file->GetHID(), szWPhast, path2fileMap) != 0)
	{
		errors = "Unable to read " + file->GetFilePath() + ", the file may be corrupted.";
		return 1;
	}

	// make a unique list of all files
	std::set<CString> files;
	std::map<CString, CString>::const_iterator cit = path2fileMap.begin();
	for (; cit != path2fileMap.end(); ++cit)
	{
		std::map<CString, CString>::iterator o2nit = orig2new.find(cit->second);
		if (o2nit == orig2new.end())
		{
			files.insert(cit->second);
		}
		else
		{
			files.insert(o2nit->second);
		}
	}

	// make a list of missing files
	std::set<CString> missing;
	std::set<CString>::iterator it = files.begin();
	for (; it != files.end(); ++it)
	{
		if (::_stricmp(ATL::ATLPath::FindExtension(*it), ".shp") == 0)
		{
			TCHAR path[MAX_PATH];
			::strcpy(path, *it);

			bool bShp = true;
			bool bShx = true;
			bool bDbf = true;
			ATL::ATLPath::RenameExtension(path, ".shp");
			if (ATL::ATLPath::FileExists(path))
			{
				bShp = false;
			}
			ATL::ATLPath::RenameExtension(path, ".shx");
			if (ATL::ATLPath::FileExists(path))
			{
				bShx = false;
			}
			ATL::ATLPath::RenameExtension(path, ".dbf");
			if (ATL::ATLPath::FileExists(path))
			{
				bDbf = false;
			}

			if (bShp)
			{
				if (bShx)
				{
					if (bDbf)
					{
						ATL::ATLPath::RenameExtension(path, ".[shp,shx,dbf]");
					}
					else
					{
						ATL::ATLPath::RenameExtension(path, ".[shp,shx]");
					}
					missing.insert(path);
				}
				else
				{
					if (bDbf)
					{
						ATL::ATLPath::RenameExtension(path, ".[shp,dbf]");
					}
					else
					{
						ATL::ATLPath::RenameExtension(path, ".shp");
					}
					missing.insert(path);
				}
			}
			else if (bShx)
			{
				if (bDbf)
				{
					ATL::ATLPath::RenameExtension(path, ".[shx,dbf]");
				}
				else
				{
					ATL::ATLPath::RenameExtension(path, ".shx");
				}
				missing.insert(path);
			}
			else if (bDbf)
			{
				ATL::ATLPath::RenameExtension(path, ".dbf");
				missing.insert(path);
			}
		}
		else
		{
			if (!ATL::ATLPath::FileExists(*it))
			{
				missing.insert((*it));
			}
		}
	}

	// report missing files
	if (missing.size())
	{
		errors = "The following files could not be found:\n";
		std::set< CString >::const_iterator cit = missing.begin();
		for (; cit != missing.end(); ++cit)
		{
			errors += (*cit) + "\n";
		}
		return (int)missing.size();
	}

	// no missing files
	return 0;
}

int CWPhastDoc::GetListOfData_sourceFiles(hid_t loc_id, std::set< CString > &files)
{
	// the pair is used to combine hdf_path and path2fileMap
	// into a single variable that can be passed as a cookie
	// to the H5Ginterate callback
	//
	std::pair< CString, std::map<CString, CString> > apair;

	CString &hdf_path = apair.first;
	std::map<CString, CString> &path2fileMap = apair.second;

	// hdf_path holds the current (group) path to the given group
	// always starts with /WPhast/
	//
	hdf_path = "/";
	hdf_path += szWPhast;
	herr_t status = ::H5Giterate(loc_id, szWPhast, NULL, CWPhastDoc::H5GIterateStatic, (void *)&apair);
	ASSERT(status >= 0);
	if (status < 0)
	{
		return EXIT_FAILURE;
	}

	// make a unique list of all files
	std::map<CString, CString>::const_iterator cit = path2fileMap.begin();
	for (; cit != path2fileMap.end(); ++cit)
	{
		files.insert((*cit).second);
	}

	return EXIT_SUCCESS;
}

int CWPhastDoc::GetMapOfData_sourceFiles(hid_t loc_id, const char *name, std::map<CString, CString> &rpath2fileMap)
{
	// the pair is used to combine hdf_path and path2fileMap
	// into a single variable that can be passed as a cookie
	// to the H5Ginterate callback
	//
	std::pair< CString, std::map<CString, CString> > apair;

	CString &hdf_path = apair.first;
	std::map<CString, CString> &path2fileMap = apair.second;

	// hdf_path holds the current (group) path to the given group
	// always starts with /WPhast/
	//
	hdf_path = "/";
	hdf_path += name;
	herr_t status = ::H5Giterate(loc_id, name, NULL, CWPhastDoc::H5GIterateStatic, (void *)&apair);
	ASSERT(status >= 0);
	if (status < 0)
	{
		return EXIT_FAILURE;
	}

	rpath2fileMap.swap(path2fileMap);
	return EXIT_SUCCESS;
}

int CWPhastDoc::GetMapOfExternalFiles(hid_t loc_id, const char *name, std::map<CString, CString> &rpath2fileMap)
{
	// the pair is used to combine hdf_path and path2fileMap
	// into a single variable that can be passed as a cookie
	// to the H5Ginterate callback
	//
	std::pair< CString, std::map<CString, CString> > apair;

	CString &hdf_path = apair.first;
	std::map<CString, CString> &path2fileMap = apair.second;

	// hdf_path holds the current (group) path to the given group
	// always starts with /WPhast/
	//
	hdf_path = "/";
	hdf_path += name;
	herr_t status = ::H5Giterate(loc_id, name, NULL, CWPhastDoc::H5GIterateStatic2, (void *)&apair);
	ASSERT(status >= 0);
	if (status < 0)
	{
		return EXIT_FAILURE;
	}

	rpath2fileMap.swap(path2fileMap);
	return EXIT_SUCCESS;
}

void CWPhastDoc::SerializeFiles(bool bStoring, CHDFMirrorFile* file, std::map<CString, CString> &orig2new)
{
	this->SiteMap3.FileName      = "";
	this->SiteMap3.WorldFileName = "";

	herr_t status;

	ASSERT(file);
	if (!file)
	{
		return;
	}

	hid_t loc_id = file->GetHID();
	ASSERT(loc_id > 0);
	if (loc_id < 0)
	{
		return;
	}

	if (bStoring)
	{
		// Create the "/Files" group
		//
		hid_t files_id = ::H5Gcreate(loc_id, szFiles, 0);
		ASSERT(files_id > 0);
		if (files_id > 0)
		{
			std::map<CString, CString> path2fileMap;
			VERIFY(this->GetMapOfData_sourceFiles(loc_id, szWPhast, path2fileMap) == 0);
			VERIFY(this->GetMapOfSiteMapFiles(loc_id, szWPhast, path2fileMap) == 0);
			{
				// make unique list of files
				std::set<CString> ufiles;
				std::map<CString, CString>::const_iterator cit = path2fileMap.begin();
				for (; cit != path2fileMap.end(); ++cit)
				{
					ufiles.insert(cit->second);
					TRACE("%s = %s\n", cit->first, cit->second);
				}

				// split path "C:\Users\charlton\Documents\Visual Studio 2005\Projects\WPhast-trunk\Bugs\extents.1.p4w"
				TCHAR szDrive[_MAX_DRIVE];
				TCHAR szDir[_MAX_DIR];
				TCHAR szFName[_MAX_FNAME];
				TCHAR szExt[_MAX_EXT];
				VERIFY(::_tsplitpath_s(file->GetFilePath(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

				// chem.dat file (extents.1.chem.dat)
				CString newName;
				newName.Format("%s.chem.dat", szFName);

				// wphast file working directory
				TCHAR cwd[MAX_PATH];
				VERIFY(::_tmakepath_s(cwd, MAX_PATH, szDrive, szDir, NULL, NULL) == 0);

				// set in CWPhastDoc::OnSaveDocument
				TCHAR curdir[MAX_PATH];
				::GetCurrentDirectory(MAX_PATH, curdir);
				ATL::ATLPath::AddBackslash(curdir);
				ASSERT(::_stricmp(cwd, curdir) == 0);

				// add *.chem.dat if it exists
				if (ATL::ATLPath::FileExists(newName))
				{
					ufiles.insert(newName);
				}

				// always add phast.dat if it exists
				if (ATL::ATLPath::FileExists("phast.dat"))
				{
					ufiles.insert("phast.dat");
				}

				size_t i = 0;
				std::string md5;
				FILETIME ftWrite;
				std::set<CString>::iterator it = ufiles.begin();
				for (; it != ufiles.end(); ++it)
				{
					ostringstream oss;
					oss << "file" << i++;
					std::string rel_path(*it);
					status = CGlobal::HDFFileEx(bStoring, files_id, oss.str().c_str(), rel_path, md5, ftWrite);
					ASSERT(status >= 0);
					TRACE("%s = %s\n", oss.str().c_str(), rel_path.c_str());
				}
			}
			// close the /Files group
			status = ::H5Gclose(files_id);
			ASSERT(status >= 0);
		}
	}
	else
	{
		// make listing of missing files
		// 
		// path2fileMap["hdf_path"]     = "relative_path or fullpath if different drive or network share"
		// contains all external files
		// --------------------------------------------------------------------------------------------
		// path2fileMap["/Files/file0"] = "..\CapeCodPhast\ArcData\ModelExtent.shp"
		// path2fileMap["/Files/file1"] = "D:\Documents and Settings\charlton\ModelExtent.shp"
		// path2fileMap["/Files/file2"] = "\\srv2rcolkr\charlton\programs\phreeqc\phastpp-trunk\examples\capecod\ArcData\ModelOutline.shp"
		//
		// missingMap["hdf_path"]       = "relative_path or fullpath if different drive or network share"
		// contains missing external files
		// --------------------------------------------------------------------------------------------
		// missingMap["/Files/file0"]   = "..\CapeCodPhast\ArcData\ModelExtent.shp"
		// missingMap["/Files/file1"]   = "D:\Documents and Settings\charlton\ModelExtent.shp"
		// missingMap["/Files/file2"]   = "\\srv2rcolkr\charlton\programs\phreeqc\phastpp-trunk\examples\capecod\ArcData\ModelOutline.shp"
		//
		std::set<CString> unique;
		std::map<CString, CString> path2fileMap;
		std::map<CString, CString> missingMap;

		this->GetMapOfExternalFiles(loc_id, szFiles, path2fileMap);
		this->CheckSiteMaps(file, path2fileMap);

		if (path2fileMap.size())
		{
			std::map<CString, CString>::iterator it = path2fileMap.begin();
			for (; it != path2fileMap.end(); ++it)
			{
				TRACE("%s\n", it->second);
				if (ATL::ATLPath::FileExists(it->second))
				{
					VERIFY(unique.insert(it->second).second);
				}
				else
				{
					VERIFY(missingMap.insert(std::map<CString, CString>::value_type(it->first, it->second)).second);
				}
			}
		}

		if (missingMap.size() == 0) return; // nothing to do

		// CWD
		TCHAR cwd[MAX_PATH];
		VERIFY(::GetCurrentDirectory(MAX_PATH, cwd));
		const TCHAR* const szCwd = cwd;

		// set new missingMap paths
		// missingMap["hdf_path"]     = "new_path"
		// --------------------------------------------------------------------------------------------
		// missingMap["/Files/file0"] = "CapeCodPhast\ArcData\ModelExtent.shp"
		// missingMap["/Files/file1"] = "..\CapeCodPhast\ArcData\ModelExtent.shp"
		// missingMap["/Files/file2"] = "D:\Documents and Settings\charlton\ModelExtent.shp"
		// missingMap["/Files/file3"] = "\\srv2rcolkr\charlton\programs\phreeqc\phastpp-trunk\examples\capecod\ArcData\ModelOutline.shp"
		// becomes
		// missingMap["/Files/file0"] = "CapeCodPhast\ArcData\ModelExtent.shp"
		// missingMap["/Files/file1"] = "CapeCodPhast\ArcData\ModelExtent.1.shp"
		// missingMap["/Files/file2"] = "D\ModelExtent.shp"
		// missingMap["/Files/file3"] = "srv2rcolkr\charlton\ModelOutline.shp"
		//
		// create orig2new
		// orig2new["orig_path"] = "new_path"
		// --------------------------------------------------------------------------------------------
		// orig2new["CapeCodPhast\ArcData\ModelExtent.shp"]   =   "CapeCodPhast\ArcData\ModelExtent.shp"
		// orig2new["..\CapeCodPhast\ArcData\ModelExtent.shp"]   =   "CapeCodPhast\ArcData\ModelExtent.1.shp"
		// orig2new["D:\Documents and Settings\charlton\ModelExtent.shp"]   =   "D\ModelExtent.shp"
		// orig2new["\\srv2rcolkr\charlton\programs\phreeqc\phastpp-trunk\examples\capecod\ArcData\ModelOutline.shp"]   =   "srv2rcolkr\charlton\ModelOutline.shp"
		//
		// create unique -- contains list of existing files and files that
		// will exist
		// --------------------------------------------------------------------------------------------
		// unique["CapeCodPhast\ArcData\ModelExtent.shp"]
		// unique["CapeCodPhast\ArcData\ModelExtent.1.shp"]
		// unique["CapeCodPhast\ArcData\ModelExtent.2.shp"]
		//
		std::map<CString, CString>::iterator mit = missingMap.begin();
		for (; mit != missingMap.end(); ++mit)
		{
			CString orig_name(mit->second);
			CString new_name;
			TCHAR szDrive[_MAX_DRIVE];
			TCHAR szDir[_MAX_DIR];
			TCHAR szFName[_MAX_FNAME];
			TCHAR szExt[_MAX_EXT];
			do
			{
				// remove '..' from missing files if path doesn't exist
				std::string r(mit->second);
				if (r.find("..") == 0 && r.size() > 3)
				{
					mit->second = r.substr(3).c_str();
					new_name = mit->second;
				}
				else
				{
					break;
				}
			}
			while (true);

			// check if file is on a different drive or network share
			if (!ATL::ATLPath::IsRelative(mit->second))
			{
				// file is on another drive or a network share

				// FullPath
				TCHAR szFullPath[MAX_PATH];
				::strcpy_s(szFullPath, MAX_PATH, szCwd);

				VERIFY(::_tsplitpath_s(mit->second, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

				if (ATL::ATLPath::IsUNC(mit->second))
				{
					//
					// \\srv2rcolkr\charlton\programs\phreeqc\phastpp-trunk\examples\capecod\ArcData\ModelOutline.shp
					// becomes
					// <CWD>\srv2rcolkr\charlton\ModelOutline.shp
					//

					CString drive(szDrive);
					ASSERT(drive.IsEmpty());

					// \\srv2rcolkr\charlton (the share name)
					TCHAR root[MAX_PATH];
					::strcpy_s(root, MAX_PATH, mit->second);
					ATL::ATLPath::StripToRoot(root);
					ASSERT(::strlen(root) > 2 && root[0] == '\\' && root[1] == '\\');
					TRACE("%s\n", root);

					// <CWD>\srv2rcolkr\charlton
					VERIFY(ATL::ATLPath::Append(ATL::ATLPath::AddBackslash(szFullPath), root+2));
					TRACE("%s\n", szFullPath);

					// ModelOutline.shp
					TCHAR fn[MAX_PATH];
					::strcpy_s(fn, MAX_PATH, mit->second);
					ATL::ATLPath::StripPath(fn);
					TRACE("%s\n", fn);

					// <CWD>\srv2rcolkr\charlton\ModelOutline.shp
					VERIFY(ATL::ATLPath::Append(ATL::ATLPath::AddBackslash(szFullPath), fn));
					TRACE("%s\n", szFullPath);

					// final new name
					// srv2rcolkr\charlton\ModelOutline.shp
					TCHAR szRelative[MAX_PATH];
					VERIFY(ATL::ATLPath::RelativePathTo(szRelative, szCwd, FILE_ATTRIBUTE_DIRECTORY, szFullPath, FILE_ATTRIBUTE_NORMAL));
					if (::strlen(szRelative) > 2 && szRelative[0] == '.' && szRelative[1] == '\\')
					{
						new_name = szRelative+2;
					}
					else
					{
						new_name = szRelative;
					}
					TRACE("%s\n", new_name);
				}
				else
				{
					//
					// D:\Documents and Settings\charlton\ModelExtent.shp
					// becomes
					// <CWD>\D\ModelExtent.shp
					//

					// D (drive)
					CString drive(szDrive);
					ASSERT(drive.GetLength() == 2 && drive[1] == ':');
					TRACE("%s\n", (const char*)drive.Mid(0, 1));

					// <CWD>\D
					VERIFY(ATL::ATLPath::Append(ATL::ATLPath::AddBackslash(szFullPath), drive.Mid(0, 1)));
					TRACE("%s\n", szFullPath);

					// ModelExtent.shp
					TCHAR fn[MAX_PATH];
					::strcpy_s(fn, MAX_PATH, mit->second);
					ATL::ATLPath::StripPath(fn);
					TRACE("%s\n", fn);

					// <CWD>\D\ModelExtent.shp
					VERIFY(ATL::ATLPath::Append(ATL::ATLPath::AddBackslash(szFullPath), fn));
					TRACE("%s\n", szFullPath);

					// final new name
					// srv2rcolkr\charlton\ModelOutline.shp
					TCHAR szRelative[MAX_PATH];
					VERIFY(ATL::ATLPath::RelativePathTo(szRelative, szCwd, FILE_ATTRIBUTE_DIRECTORY, szFullPath, FILE_ATTRIBUTE_NORMAL));
					if (::strlen(szRelative) > 2 && szRelative[0] == '.' && szRelative[1] == '\\')
					{
						new_name = szRelative+2;
					}
					else
					{
						new_name = szRelative;
					}
					TRACE("%s\n", new_name);
				}
			}

			if (new_name.IsEmpty())
			{
				ASSERT(ATL::ATLPath::IsRelative(orig_name));
				if (unique.find(orig_name) != unique.end())
				{
					// orig_name is going to exist after serializing
					// so must rename

					VERIFY(::_tsplitpath_s(orig_name, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

					BOOL bExists;
					TCHAR szRelative[MAX_PATH];
					if (::_stricmp(szExt, "shp") == 0)
					{
						///TCHAR szRelativeSHP[MAX_PATH];
						TCHAR szRelativeSHX[MAX_PATH];
						TCHAR szRelativeDBF[MAX_PATH];
						int i = 0;
						do
						{
							new_name.Format("%s.%d", szFName, ++i);		
							VERIFY(::_tmakepath_s(szRelative,    MAX_PATH, szDrive, szDir, new_name, "shp") == 0);
							VERIFY(::_tmakepath_s(szRelativeSHX, MAX_PATH, szDrive, szDir, new_name, "shx") == 0);
							VERIFY(::_tmakepath_s(szRelativeDBF, MAX_PATH, szDrive, szDir, new_name, "dbf") == 0);
							bExists = ATL::ATLPath::FileExists(szRelative)
								|| ATL::ATLPath::FileExists(szRelativeSHX)
								|| ATL::ATLPath::FileExists(szRelativeDBF);
						}
						while (bExists || unique.find(szRelative) != unique.end());
					}
					else
					{
						int i = 0;
						do
						{
							new_name.Format("%s.%d", szFName, ++i);		
							VERIFY(::_tmakepath_s(szRelative, MAX_PATH, szDrive, szDir, new_name, szExt) == 0);
							bExists = ATL::ATLPath::FileExists(szRelative);
						}
						while (bExists || unique.find(szRelative) != unique.end());
					}
					
					// check and strip '.\' from beginning
					if (::strlen(szRelative) > 2 && szRelative[0] == '.' && szRelative[1] == '\\')
					{
						new_name = szRelative+2;
					}
					else
					{
						new_name = szRelative;
					}
					TRACE("%s\n", new_name);

					// add to list of files
					ASSERT(ATL::ATLPath::IsRelative(new_name));
					VERIFY(unique.insert(new_name).second);

					// swap new_names in orig2new
					std::map<CString, CString>::iterator search = orig2new.begin();
					for (; search != orig2new.end(); ++search)
					{
						if (search->second.Compare(orig_name) == 0)
						{
							search->second = new_name;
							break;
						}
					}
					// swap new_names in missingMap
					search = missingMap.begin();
					for (; search != missingMap.end(); ++search)
					{
						if (search->second.Compare(orig_name) == 0)
						{
							search->second = new_name;
							break;
						}
					}

					// map original name to new name
					VERIFY(orig2new.insert(std::map<CString, CString>::value_type(orig_name, orig_name)).second);
				}
				else
				{
					// add to list of files
					VERIFY(unique.insert(orig_name).second);
					ASSERT(mit->second.Compare(orig_name) == 0);

					// map original name to new name
					VERIFY(orig2new.insert(std::map<CString, CString>::value_type(orig_name, orig_name)).second);
				}
			}
			else
			{
				if (unique.find(new_name) != unique.end() || ATL::ATLPath::FileExists(new_name))
				{
					// new_name already exists or is going to exist after serializing
					// so must rename file

					VERIFY(::_tsplitpath_s(new_name, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

					BOOL bExists;
					TCHAR szRelative[MAX_PATH];
					if (::_stricmp(szExt, "shp") == 0)
					{
						///TCHAR szRelativeSHP[MAX_PATH];
						TCHAR szRelativeSHX[MAX_PATH];
						TCHAR szRelativeDBF[MAX_PATH];
						int i = 0;
						do
						{
							new_name.Format("%s.%d", szFName, ++i);		
							VERIFY(::_tmakepath_s(szRelative,    MAX_PATH, szDrive, szDir, new_name, "shp") == 0);
							VERIFY(::_tmakepath_s(szRelativeSHX, MAX_PATH, szDrive, szDir, new_name, "shx") == 0);
							VERIFY(::_tmakepath_s(szRelativeDBF, MAX_PATH, szDrive, szDir, new_name, "dbf") == 0);
							bExists = ATL::ATLPath::FileExists(szRelative)
								|| ATL::ATLPath::FileExists(szRelativeSHX)
								|| ATL::ATLPath::FileExists(szRelativeDBF);
						}
						while (bExists || unique.find(szRelative) != unique.end());
					}
					else
					{
						int i = 0;
						do
						{
							new_name.Format("%s.%d", szFName, ++i);		
							VERIFY(::_tmakepath_s(szRelative, MAX_PATH, szDrive, szDir, new_name, szExt) == 0);
							bExists = ATL::ATLPath::FileExists(szRelative);
						}
						while (bExists || unique.find(szRelative) != unique.end());
					}

					// make new_name relative to CWD
					if (::strlen(szRelative) > 2 && szRelative[0] == '.' && szRelative[1] == '\\')
					{
						new_name = szRelative+2;
					}
					else
					{
						new_name = szRelative;
					}
					TRACE("%s\n", new_name);
					ASSERT(ATL::ATLPath::IsRelative(new_name));
				}
				// add to list of files
				ASSERT(ATL::ATLPath::IsRelative(new_name));
				VERIFY(unique.insert(new_name).second);

				// map original name to new name
				VERIFY(orig2new.insert(std::map<CString, CString>::value_type(orig_name, new_name)).second);
				mit->second = new_name;
			}
		}

		// create directories that don't exist
		// SHCreateDirectoryEx creates all paths including missing children
		std::map<CString, CString>::iterator it = missingMap.begin();
		for (; it != missingMap.end(); ++it)
		{
			if (it->first.Find("/SHX/") != -1) continue;
			if (it->first.Find("/DBF/") != -1) continue;

			ASSERT(std::string(it->second).find("..") != 0);
			TCHAR szDrive[_MAX_DRIVE];
			TCHAR szDir[_MAX_DIR];
			TCHAR szFName[_MAX_FNAME];
			TCHAR szExt[_MAX_EXT];

			VERIFY(::_tsplitpath_s(it->second, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);
			std::string d = CGlobal::FullPath(szDir);
			if (!d.empty() && !ATL::ATLPath::IsDirectory(d.c_str()))
			{
				int n = ::SHCreateDirectoryEx(0, d.c_str(), 0);
				if (n != ERROR_SUCCESS)
				{
					ASSERT(FALSE);
				}
			}
		}

		// Open the "/Files" group
		hid_t files_id = ::H5Gopen(loc_id, szFiles);
		ASSERT(files_id > 0);
		if (files_id > 0)
		{
			// write missing files
			std::map<CString, CString>::iterator it = missingMap.begin();
			for (; it != missingMap.end(); ++it)
			{
				if (it->first.Find("/SHX/")           != -1) continue;
				if (it->first.Find("/DBF/")           != -1) continue;
				if (it->first.Find("/WPhast/SiteMap") != -1)
				{
					this->CreateSiteMapFiles(loc_id, it->first, it->second);
					continue;					
				}

				std::string md5;
				FILETIME ftWrite;
				std::string rel_path(it->second);
				ASSERT(ATL::ATLPath::IsRelative(it->second));
				status = CGlobal::HDFFileEx(bStoring, files_id, it->first, rel_path, md5, ftWrite);
				ASSERT(status >= 0);
			}

			// close the /Files group
			status = ::H5Gclose(files_id);
			ASSERT(status >= 0);
		}
		else
		{
			// This is for backwards compatibility
			// if here /Files group doesn't exist and only
			// Sitemap files are created
			std::map<CString, CString>::iterator it = missingMap.begin();
			for (; it != missingMap.end(); ++it)
			{
				if (it->first.Find("/WPhast/SiteMap") != -1)
				{
					this->CreateSiteMapFiles(loc_id, it->first, it->second);
				}
				else
				{
					ASSERT(FALSE);
				}
			}
		}
	}
}

herr_t CWPhastDoc::H5GIterateStatic(hid_t loc_id, const char *name, void *cookie)
{
	// this is called recursively to build the pathways to all of
	// the "file_name" datasets (see ValidateData_sources)

	static const char szFileName[] = "file_name";

	ASSERT(cookie);
	std::pair< CString, std::map<CString, CString> > *ppair =
		static_cast< std::pair< CString, std::map<CString, CString> > *>(cookie);

	CString &hdf_path = ppair->first;
	std::map<CString, CString> &path2fileMap = ppair->second;

    H5G_stat_t statbuf;
	herr_t status = ::H5Gget_objinfo(loc_id, name, FALSE, &statbuf);
	ASSERT(status >= 0);

	switch (statbuf.type)
	{
	case H5G_GROUP:
		hdf_path += "/";
		hdf_path += name;
		status = ::H5Giterate(loc_id, name, NULL, CWPhastDoc::H5GIterateStatic, (void *)cookie);
		hdf_path = hdf_path.Left(hdf_path.GetLength() - (int)::strlen(name) - 1);
		break;
	case H5G_DATASET:
		if (::strcmp(szFileName, name) == 0)
		{
			std::string filename;
			status = CGlobal::HDFSerializeString(false, loc_id, szFileName, filename);
			ASSERT(status >= 0);

			CString path(hdf_path);
			path += "/";
			path += name;
			path2fileMap.insert(std::map<CString, CString>::value_type(path, CString(filename.c_str()))); 
		}
		break;
	case H5G_TYPE:
		break;
	default:
		break;
	}

	return 0; // continue iterating
}

herr_t CWPhastDoc::H5GIterateStatic2(hid_t loc_id, const char *name, void *cookie)
{
	// this is called recursively to build the pathways to all of
	// the "RelativePath" datasets (see ValidateData_sources)
	// skips over DBF and SHX groups

	static const char szRelativePath[] = "RelativePath";

	ASSERT(cookie);
	std::pair< CString, std::map<CString, CString> > *ppair =
		static_cast< std::pair< CString, std::map<CString, CString> > *>(cookie);

	CString &hdf_path = ppair->first;
	std::map<CString, CString> &path2fileMap = ppair->second;

    H5G_stat_t statbuf;
	herr_t status = ::H5Gget_objinfo(loc_id, name, FALSE, &statbuf);
	ASSERT(status >= 0);

	switch (statbuf.type)
	{
	case H5G_GROUP:
		hdf_path += "/";
		hdf_path += name;
		if (::strcmp(name, "DBF") && ::strcmp(name, "SHX"))
		{
			status = ::H5Giterate(loc_id, name, NULL, CWPhastDoc::H5GIterateStatic2, (void *)cookie);
		}
		hdf_path = hdf_path.Left(hdf_path.GetLength() - (int)::strlen(name) - 1);
		break;
	case H5G_DATASET:
		if (::strcmp(szRelativePath, name) == 0)
		{
			std::string filename;
			status = CGlobal::HDFSerializeString(false, loc_id, szRelativePath, filename);
			ASSERT(status >= 0);

			CString path(hdf_path);
			path2fileMap.insert(std::map<CString, CString>::value_type(path, CString(filename.c_str()))); 
		}
		break;
	case H5G_TYPE:
		break;
	default:
		break;
	}

	return 0; // continue iterating
}

std::map<CString, CString>& CWPhastDoc::GetOriginal2New(void)
{
	return this->Original2New;
}

void CWPhastDoc::OnUpdateDrainsHideAll(CCmdUI *pCmdUI)
{
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyDrains())
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

void CWPhastDoc::OnDrainsHideAll()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetNodeCheck(pTree->GetDrainsNode(), BST_UNCHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateDrainsShowSelected(CCmdUI *pCmdUI)
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		switch (pTree->GetNodeCheck(pTree->GetDrainsNode()))
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

void CWPhastDoc::OnDrainsShowSelected()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetNodeCheck(pTree->GetDrainsNode(), BST_CHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnDrainsSelectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode node = pTree->GetDrainsNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_CHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnDrainsUnselectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode node = pTree->GetDrainsNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_UNCHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateZoneFlowHideAll(CCmdUI *pCmdUI)
{
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyZoneFlowRates())
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

void CWPhastDoc::OnZoneFlowHideAll()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetNodeCheck(pTree->GetZoneFlowRatesNode(), BST_UNCHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateZoneFlowShowSelected(CCmdUI *pCmdUI)
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		switch (pTree->GetNodeCheck(pTree->GetZoneFlowRatesNode()))
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

void CWPhastDoc::OnZoneFlowShowSelected()
{
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetNodeCheck(pTree->GetZoneFlowRatesNode(), BST_CHECKED);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnZoneFlowSelectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode node = pTree->GetZoneFlowRatesNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_CHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnZoneFlowUnselectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode node = pTree->GetZoneFlowRatesNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_UNCHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::CheckSiteMaps(CHDFMirrorFile* file, std::map<CString, CString> &path2fileMap)
{
	static const char szSiteMap[]       = "/WPhast/SiteMap";
	static const char szSiteMap2[]      = "/WPhast/SiteMap2";
	static const char szFileName[]      = "FileName";
	static const char szWorldFileName[] = "WorldFileName";

	hid_t loc_id = file->GetHID();
	ASSERT(loc_id > 0);
	if (loc_id < 0)
	{
		return;
	}

	CString hdf, path;
	CString whdf, wpath;
	int version = 0;
	hid_t sitemap_id = 0;

	// Open the szSiteMap2 group
	sitemap_id = ::H5Gopen(loc_id, szSiteMap2);
	if (sitemap_id < 0)
	{
		version = 1;
		// Open the szSiteMap group
		sitemap_id = ::H5Gopen(loc_id, szSiteMap);
	}
	else
	{
		version = 2;
	}

	if (sitemap_id > 0)
	{
		switch (version)
		{
		case 1:
			{
				CSiteMap sitemap;
				sitemap.Serialize(false, sitemap_id);

				hdf = szSiteMap;
				hdf += "/";
				whdf = hdf;

				hdf += szFileName;
				whdf += szWorldFileName;

				path = sitemap.m_fileName.c_str();
				wpath = CGlobal::GetWorldFileName(path);
			}
			break;
		case 2:
			{
				CSiteMap2 sitemap2;
				sitemap2.Serialize(false, sitemap_id);

				hdf = szSiteMap2;
				hdf += "/";
				whdf = hdf;

				hdf += szFileName;
				whdf += szWorldFileName;

				path = sitemap2.FileName.c_str();
				wpath = CGlobal::GetWorldFileName(path);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		// close the szSiteMap group
		herr_t status = ::H5Gclose(sitemap_id);
		ASSERT(status >= 0);

		// get relative filenames
		std::string filename(path);
		if (filename.length() > 0)
		{
			filename = this->GetRelativePath(file->GetFilePath(), filename);
			path2fileMap[hdf] = filename.c_str();
		}
	}
}

int CWPhastDoc::GetMapOfSiteMapFiles(hid_t loc_id, const char *name, std::map<CString, CString> &rpath2fileMap)
{
	if (this->m_pMapActor)
	{
		CSiteMap3 sitemap3 = this->m_pMapActor->GetSiteMap3();
		rpath2fileMap["/WPhast/SiteMap3/FileName"]      = sitemap3.FileName.c_str();
		rpath2fileMap["/WPhast/SiteMap3/WorldFileName"] = sitemap3.WorldFileName.c_str();
	}
	return EXIT_SUCCESS;
}

void CWPhastDoc::SiteMapPathsRelativeToAbsolute(LPCTSTR lpszPathName)
{
	if (this->m_pMapActor)
	{
		CSiteMap3 siteMap3 = this->m_pMapActor->GetSiteMap3();
		std::string filename = this->GetRelativePath(lpszPathName, siteMap3.FileName);
		std::string wfilename = this->GetRelativePath(lpszPathName, siteMap3.WorldFileName);

		if (this->GetOriginal2New().size())
		{
			std::map<CString, CString>::iterator it = this->GetOriginal2New().find(filename.c_str());
			if (it != this->GetOriginal2New().end())
			{
				filename = it->second;
			}
		}
		if (filename.length() > 0)
		{
			this->m_pMapActor->UpdateFileName(this->GetAbsolutePath(lpszPathName, filename).c_str());
		}
		if (wfilename.length() > 0)
		{
			this->m_pMapActor->UpdateWorldFileName(this->GetAbsolutePath(lpszPathName, wfilename).c_str());
		}
	}
}

void CWPhastDoc::SiteMapPathsAbsoluteToRelative(LPCTSTR lpszPathName)
{
	if (this->m_pMapActor)
	{
		CSiteMap3 siteMap3 = this->m_pMapActor->GetSiteMap3();
		std::string filename = siteMap3.FileName;
		std::string wfilename = siteMap3.WorldFileName;

		if (filename.length() > 0)
		{
			this->m_pMapActor->UpdateFileName(this->GetRelativePath(lpszPathName, filename).c_str());
		}
		if (wfilename.length() > 0)
		{
			this->m_pMapActor->UpdateWorldFileName(this->GetRelativePath(lpszPathName, wfilename).c_str());
		}
	}
}

void CWPhastDoc::PathsRelativeToAbsolute(LPCTSTR lpszPathName)
{
	this->DataSourcePathsRelativeToAbsolute(lpszPathName);
	this->SiteMapPathsRelativeToAbsolute(lpszPathName);
}

void CWPhastDoc::PathsAbsoluteToRelative(LPCTSTR lpszPathName)
{
	this->DataSourcePathsAbsoluteToRelative(lpszPathName);
	this->SiteMapPathsAbsoluteToRelative(lpszPathName);
}

void CWPhastDoc::CreateSiteMapFiles(hid_t loc_id, CString hdf, CString filename)
{
	// This is only used for backwards compatibility
	//

	if (hdf.Find("/WPhast/SiteMap2") != -1)
	{
		hid_t sitemap_id = ::H5Gopen(loc_id, "/WPhast/SiteMap2");
		if (sitemap_id > 0)
		{
			try 
			{
				herr_t status = CGlobal::HDFSerializeBinaryFile(false, sitemap_id, "Image", filename);
				ASSERT(status >= 0);

				CSiteMap2 sitemap2;
				sitemap2.Serialize(false, sitemap_id);

				CString strWorldFileName = CGlobal::GetWorldFileName(filename);
				CGlobal::WriteWorldFile(strWorldFileName, sitemap2.GetWorldTransform());

				this->SiteMap3.FileName      = filename;
				this->SiteMap3.WorldFileName = strWorldFileName;
				this->SiteMap3.SetWorldTransform(sitemap2.GetWorldTransform());

				this->SiteMap3.Angle = sitemap2.Angle;
				this->SiteMap3.Origin[0] = sitemap2.Origin[0];
				this->SiteMap3.Origin[1] = sitemap2.Origin[1];
				this->SiteMap3.Origin[2] = sitemap2.Origin[2];
			}
			catch(...)
			{
			}

			// close the "/WPhast/SiteMap2" group
			herr_t status = ::H5Gclose(sitemap_id);
			ASSERT(status >= 0);

		}
	}
	else if (hdf.Find("/WPhast/SiteMap") != -1)
	{
		hid_t sitemap_id = ::H5Gopen(loc_id, "/WPhast/SiteMap");
		if (sitemap_id > 0)
		{
			try 
			{
				herr_t status = CGlobal::HDFSerializeBinaryFile(false, sitemap_id, "Image", filename);
				ASSERT(status >= 0);

				CSiteMap sitemap;
				sitemap.Serialize(false, sitemap_id);

				CString strWorldFileName = CGlobal::GetWorldFileName(filename);
				CGlobal::WriteWorldFile(strWorldFileName, sitemap.GetWorldTransform());

				this->SiteMap3.FileName      = filename;
				this->SiteMap3.WorldFileName = strWorldFileName;
				this->SiteMap3.SetWorldTransform(sitemap.GetWorldTransform());

				this->SiteMap3.Angle = sitemap.m_angle;
				this->SiteMap3.Origin[0] = sitemap.m_placement[0];
				this->SiteMap3.Origin[1] = sitemap.m_placement[1];
				this->SiteMap3.Origin[2] = sitemap.m_placement[2];
			}
			catch(...)
			{
			}

			// close the "/WPhast/SiteMap" group
			herr_t status = ::H5Gclose(sitemap_id);
			ASSERT(status >= 0);
		}
	}
}

void CWPhastDoc::OnHelpUserGuidePdf()
{
	TCHAR szPath[MAX_PATH];
	if (!::GetModuleFileName(0, szPath, MAX_PATH))
	{
		TRACE("GetModuleFileName failed (%d)\n", ::GetLastError()); 
		return;
	}

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];

	VERIFY(::_tsplitpath_s(szPath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);
	VERIFY(::_tmakepath_s(szPath, _MAX_DIR, szDrive, szDir, NULL, NULL) == 0);
	VERIFY(::PathAppend(szPath, "..\\doc\\phast2-TM6-A35.pdf"));

	TCHAR szOut[MAX_PATH];
	VERIFY(::PathCanonicalize(szOut, szPath));

	if (!::ShellExecute(0, "open", szOut, 0, 0, 0))
	{
		TRACE("ShellExecute failed (%d)\n", ::GetLastError()); 
		char buffer[2048];
		::sprintf_s(buffer, sizeof(buffer), "Unable to open \"%s\"", szPath);
		::AfxMessageBox(buffer);
	}
}
