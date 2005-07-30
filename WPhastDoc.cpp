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
#include <vtkPolyDataMapper.h>
#include <vtkLODActor.h>
#include <vtkProperty.h>

#include <vtkAxes.h>
#include <vtkTubeFilter.h>
#include <vtkAssembly.h>
#include <vtkPropAssembly.h>
#include <vtkProp3DCollection.h>

#include <vector>
#include <list>
#include <fstream>  // std::ifstream
#include <sstream>  // std::ostringstream std::istringstream

#include "Action.h"
#include "ZoneActor.h"
#include "MediaZoneActor.h"
#include "BCZoneActor.h"
#include "ICZoneActor.h"

#include "ISerial.h"
#include "DelayRedraw.h"

////#include "ZoneLODActor.h"
#include "GridLODActor.h"
#include "AxesActor.h"
#include "Global.h"

#include "ImportErrorDialog.h"
#include "RunDialog.h"

#include "PropertyTreeControlBar.h"
#include "BoxPropertiesDialogBar.h"
#include "TimeControlPropertyPage.h"

#include "ModelessPropertySheet.h"
#include "ScalePropertyPage.h"

#include "GridPropertyPage2.h"

#include <vtkBoxWidget.h>

#include <vtkAbstractPropPicker.h>
#include <vtkWin32RenderWindowInteractor.h>

// #include "CreateZoneAction.h"
#include "ZoneCreateAction.h"
#include "AddStressPeriodAction.h"
#include "WellCreateAction.h"
#include "RiverCreateAction.h"
#include "RiverMovePointAction.h"
#include "RiverInsertPointAction.h"

#include "Unit.h"
#include "Units.h"
#include "NewModel.h"
#include "FlowOnly.h"
#include "SteadyFlow.h"
#include "FreeSurface.h"

#include "HeadIC.h"
#include "TimeControl2.h"
#include "MapActor.h"
#include "MapImageActor.h"
#include "WorldTransform.h"
#include "SiteMap.h"
#include "NewModel.h"

#include "WellActor.h"
#include "RiverActor.h"

extern void GetDefaultMedia(struct grid_elt* p_grid_elt);
extern void GetDefaultHeadIC(struct head_ic* p_head_ic);
extern void GetDefaultChemIC(struct chem_ic* p_chem_ic);

extern "C" {
int error_msg (const char *err_str, const int stop);
}

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


struct WPhastDocImpl {
	// Note: see http://www.gotw.ca/publications/mill04.htm
	std::vector<CAction*> m_vectorActions;
	std::vector<CAction*>::size_type m_vectorActionsIndex;
	std::vector<CAction*>::size_type m_lastSaveIndex;
	std::vector<CZone*> m_vectorDefaultZones;
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
, m_pGridLODActor(0)
, m_pAxesActor(0)
, m_pGeometrySheet(0)
, m_pScalePage(0)
, m_pUnits(0)
, m_pModel(0) // , m_pFlowOnly(0)
, m_ProjectionMode(PT_PERSPECTIVE)
, m_pMapActor(0)
, m_pPropAssemblyMedia(0)
, m_pPropAssemblyBC(0)
, m_pPropAssemblyIC(0)
, m_pPropAssemblyWells(0)
, m_pPropAssemblyRivers(0)
, RiverCallbackCommand(0)
, RiverMovePointAction(0)
, m_pGridSheet(0)
, m_pGridPage(0)
{
#if defined(WPHAST_AUTOMATION)
	EnableAutomation();

	AfxOleLockApp();
#endif

	ASSERT(this->m_pimpl == 0);
	this->m_pimpl = new WPhastDocImpl();
	this->m_pimpl->m_vectorActionsIndex = 0;
	this->m_pimpl->m_lastSaveIndex = -1;

// COMMENT: {7/28/2005 2:48:16 PM}	// create the grid
// COMMENT: {7/28/2005 2:48:16 PM}	//
// COMMENT: {7/28/2005 2:48:16 PM}	ASSERT(this->m_pGridLODActor == 0);
// COMMENT: {7/28/2005 2:48:16 PM}	this->m_pGridLODActor = CGridLODActor::New();
// COMMENT: {7/28/2005 2:48:16 PM}	this->m_pGridLODActor->GetProperty()->SetColor(1.0, 0.8, 0.6);

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
	this->m_pPropAssemblyMedia  = vtkPropAssembly::New();
	this->m_pPropAssemblyIC     = vtkPropAssembly::New();
	this->m_pPropAssemblyBC     = vtkPropAssembly::New();
	this->m_pPropAssemblyWells  = vtkPropAssembly::New();
	this->m_pPropAssemblyRivers = vtkPropAssembly::New();
	this->m_pPropCollection->AddItem(this->m_pPropAssemblyMedia);
	this->m_pPropCollection->AddItem(this->m_pPropAssemblyIC);
	this->m_pPropCollection->AddItem(this->m_pPropAssemblyBC);
	this->m_pPropCollection->AddItem(this->m_pPropAssemblyWells);
	this->m_pPropCollection->AddItem(this->m_pPropAssemblyRivers);

	// create the axes
	//
	this->m_pAxesActor = CAxesActor::New();

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
// COMMENT: {7/28/2005 2:54:07 PM}	this->New(CNewModel::Default());

	// river event listener
	//
	this->RiverCallbackCommand = vtkCallbackCommand::New();
	this->RiverCallbackCommand->SetClientData(this);
	this->RiverCallbackCommand->SetCallback(CWPhastDoc::RiverListener);
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

 	ASSERT(this->m_pGridLODActor);
	if (this->m_pGridLODActor)
	{
		this->m_pGridLODActor->Delete();
		this->m_pGridLODActor = 0;
	}

	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyMedia);
	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyIC);
	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyBC);
	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyWells);
	CLEANUP_ASSEMBLY_MACRO(this->m_pPropAssemblyRivers);	

	ASSERT(this->m_pAxesActor);
	if (this->m_pAxesActor)
	{
		this->m_pAxesActor->Delete();
		this->m_pAxesActor = 0;
	}

	ASSERT(this->m_pScalePage == 0);     // should be deleted in CWPhastDoc::OnCloseDocument()
	ASSERT(this->m_pGeometrySheet == 0); // should be deleted in CWPhastDoc::OnCloseDocument()

	ASSERT_DELETE_SET_NULL_MACRO(this->m_pUnits);
	ASSERT_DELETE_SET_NULL_MACRO(this->m_pModel);
// COMMENT: {5/5/2005 3:51:23 PM}	ASSERT_DELETE_SET_NULL_MACRO(this->m_pPrintFreq);

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
}

BOOL CWPhastDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// add reinitialization code here
	// (SDI documents will reuse this document)
	TRACE("CWPhastDoc::OnNewDocument()\n");
	this->SetModifiedFlag(TRUE);

	return TRUE;
}

// CWPhastDoc serialization

void CWPhastDoc::Serialize(CArchive& ar)
{
	static const char szWPhast[] = "WPhast";
	herr_t status;

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

			// store units
			ASSERT(this->m_pUnits);
			this->m_pUnits->Serialize(bStoring, wphast_id);

			// store grid
			this->m_pGridLODActor->Serialize(bStoring, wphast_id);

			// store axes
			//this->m_pAxesActor->Serialize(bStoring, wphast_id);  // not implemented

			// store site map
			if (this->m_pMapActor) this->m_pMapActor->Serialize(bStoring, wphast_id);

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

			// store PRINT_FREQUENCY
			this->m_pModel->m_printFreq.Serialize(bStoring, wphast_id);

			// store time control
			this->m_pModel->m_timeControl2.Serialize(bStoring, wphast_id);

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
		CDelayRedraw delayRender(::AfxGetMainWnd()->GetActiveWindow());

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
			this->m_pGridLODActor->Serialize(bStoring, wphast_id);

			// set grid
			CGrid x, y, z;
			this->m_pGridLODActor->GetGrid(x, y, z);
			this->ResizeGrid(x, y, z);

			// load axes
			//this->m_pAxesActor->Serialize(bStoring, wphast_id); // not implemented
			this->GetPropCollection()->AddItem(this->m_pAxesActor);

			// load site map
			ASSERT(this->m_pMapActor == NULL);
			this->m_pMapActor = CMapActor::New();
			this->m_pMapActor->Serialize(bStoring, wphast_id);
			CSiteMap siteMap = this->m_pMapActor->GetSiteMap();
			if (siteMap.m_fileName.empty())
			{
				this->m_pMapActor->Delete();
				this->m_pMapActor = 0;
			}
			else
			{
				this->m_pMapActor->SetPickable(0);
				this->GetPropCollection()->AddItem(this->m_pMapActor);
			}

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

			// load PRINT_FREQUENCY
			this->m_pModel->m_printFreq.Serialize(bStoring, wphast_id);
			// update properties bar
			if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
			{
				pTree->SetPrintFrequency(&this->m_pModel->m_printFreq);
			}

			// load time control
			this->m_pModel->m_timeControl2.Serialize(bStoring, wphast_id);			

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
		vtkFloatingPointType* scale = this->m_pGridLODActor->GetScale();
		this->SetScale(scale[0], scale[1], scale[2]);

		// TODO: eventually the camera position will be stored in the HDF file
		this->ResetCamera();
	}

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
	static const char szMedia[] = "Media";
	static const char szZones[] = "Zones";

	hid_t media_id;
	hid_t zone_id;
	herr_t status;

	if (bStoring)
	{
		// Create the szMedia group
		media_id = ::H5Gcreate(loc_id, szMedia, 0); // always created even if empty
		ASSERT(media_id > 0);
		if (media_id > 0)
		{
			CTreeCtrlNode nodeMedia = this->GetPropertyTreeControlBar()->GetMediaNode();
			int nCount = nodeMedia.GetChildCount();
			std::list<LPCTSTR> listNames;
			std::list<CMediaZoneActor*> listZones;
			for (int i = 0; i < nCount; ++i)
			{
				if (CMediaZoneActor *pZone = CMediaZoneActor::SafeDownCast((vtkObject*)nodeMedia.GetChildAt(i).GetData()))
				{
					listZones.push_back(pZone);
					ASSERT(pZone->GetName());
					listNames.push_back(pZone->GetName());
				}
			}

			if (listNames.size() > 0)
			{
				CGlobal::WriteList(media_id, szZones, listNames);
				std::list<CMediaZoneActor*>::iterator iter = listZones.begin();
				for (; iter != listZones.end(); ++iter)
				{
					// create zone group
					zone_id = ::H5Gcreate(media_id, (*iter)->GetName(), 0);
					ASSERT(zone_id > 0);
					if (zone_id > 0)
					{
						// serialize grid_elt
						(*iter)->Serialize(true, zone_id, this->GetUnits());

						// close the szZones group
						status = ::H5Gclose(zone_id);
						ASSERT(status >= 0);
					}
				}
			}
			// close the szMedia group
			status = ::H5Gclose(media_id);
			ASSERT(status >= 0);
		}
	}
	else
	{
		// Loading
		//

		// Open the szMedia group
		//
		media_id = ::H5Gopen(loc_id, szMedia);
		ASSERT(media_id > 0);
		if (media_id > 0)
		{
			std::list<std::string> listNames;
			CGlobal::ReadList(media_id, szZones, listNames);
			std::list<std::string>::iterator iter = listNames.begin();
			for (; iter != listNames.end(); ++iter)
			{
				// open zone group
				zone_id = ::H5Gopen(media_id, (*iter).c_str());
				ASSERT(zone_id > 0);
				if (zone_id > 0)
				{
					CMediaZoneActor* pZone = CMediaZoneActor::New();

					// load grid_elt
					pZone->SetName((*iter).c_str());
					pZone->Serialize(false, zone_id, this->GetUnits());
					this->Add(pZone);
					pZone->Delete(); // ok ref counted

					// close the szZones group
					status = ::H5Gclose(zone_id);
					ASSERT(status >= 0);
				}
			}

			// close the szMedia group
			status = ::H5Gclose(media_id);
			ASSERT(status >= 0);
		}
	}
}

// COMMENT: {4/11/2005 1:31:43 PM}void CWPhastDoc::SerializeBC(bool bStoring, hid_t loc_id, int nStressPeriod)
// COMMENT: {4/11/2005 1:30:56 PM}{
// COMMENT: {4/11/2005 1:30:56 PM}	static const char szBC[]    = "BC";
// COMMENT: {4/11/2005 1:30:56 PM}	static const char szZones[] = "Zones";
// COMMENT: {4/11/2005 1:30:56 PM}
// COMMENT: {4/11/2005 1:30:56 PM}	hid_t bc_id;
// COMMENT: {4/11/2005 1:30:56 PM}	hid_t zone_id;
// COMMENT: {4/11/2005 1:30:56 PM}	herr_t status;
// COMMENT: {4/11/2005 1:30:56 PM}
// COMMENT: {4/11/2005 1:30:56 PM}	if (bStoring)
// COMMENT: {4/11/2005 1:30:56 PM}	{
// COMMENT: {4/11/2005 1:30:56 PM}		// add storing code here
// COMMENT: {4/11/2005 1:30:56 PM}
// COMMENT: {4/11/2005 1:30:56 PM}		// Create the szBC group
// COMMENT: {4/11/2005 1:30:56 PM}		bc_id = ::H5Gcreate(loc_id, szBC, 0); // always created even if empty
// COMMENT: {4/11/2005 1:30:56 PM}		ASSERT(bc_id > 0);
// COMMENT: {4/11/2005 1:30:56 PM}		if (bc_id > 0)
// COMMENT: {4/11/2005 1:30:56 PM}		{
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:28 PM}			CTreeCtrlNode nodeBC = this->GetPropertyTreeControlBar()->GetBCNode(nStressPeriod);
// COMMENT: {4/11/2005 1:30:56 PM}			CTreeCtrlNode nodeBC = this->GetPropertyTreeControlBar()->GetBCNode(nStressPeriod);
// COMMENT: {4/11/2005 1:30:56 PM}			int nCount = nodeBC.GetChildCount();
// COMMENT: {4/11/2005 1:30:56 PM}			std::list<LPCTSTR> listNames;
// COMMENT: {4/11/2005 1:30:56 PM}			std::list<CBCZoneActor*> listZones;
// COMMENT: {4/11/2005 1:30:56 PM}			for (int i = 0; i < nCount; ++i)
// COMMENT: {4/11/2005 1:30:56 PM}			{
// COMMENT: {4/11/2005 1:30:56 PM}				if (CBCZoneActor *pZone = CBCZoneActor::SafeDownCast((vtkObject*)nodeBC.GetChildAt(i).GetData()))
// COMMENT: {4/11/2005 1:30:56 PM}				{
// COMMENT: {4/11/2005 1:30:56 PM}					listZones.push_back(pZone);
// COMMENT: {4/11/2005 1:30:56 PM}					ASSERT(pZone->GetName());
// COMMENT: {4/11/2005 1:30:56 PM}					listNames.push_back(pZone->GetName());
// COMMENT: {4/11/2005 1:30:56 PM}				}
// COMMENT: {4/11/2005 1:30:56 PM}				else ASSERT(FALSE);
// COMMENT: {4/11/2005 1:30:56 PM}			}
// COMMENT: {4/11/2005 1:30:56 PM}			if (listNames.size() > 0)
// COMMENT: {4/11/2005 1:30:56 PM}			{
// COMMENT: {4/11/2005 1:30:56 PM}				CGlobal::WriteList(bc_id, szZones, listNames);
// COMMENT: {4/11/2005 1:30:56 PM}				std::list<CBCZoneActor*>::iterator iter = listZones.begin();
// COMMENT: {4/11/2005 1:30:56 PM}				for (; iter != listZones.end(); ++iter)
// COMMENT: {4/11/2005 1:30:56 PM}				{
// COMMENT: {4/11/2005 1:30:56 PM}					// create zone group
// COMMENT: {4/11/2005 1:30:56 PM}					zone_id = ::H5Gcreate(bc_id, (*iter)->GetName(), 0);
// COMMENT: {4/11/2005 1:30:56 PM}					ASSERT(zone_id > 0);
// COMMENT: {4/11/2005 1:30:56 PM}					if (zone_id > 0)
// COMMENT: {4/11/2005 1:30:56 PM}					{
// COMMENT: {4/11/2005 1:30:56 PM}						// serialize bc
// COMMENT: {4/11/2005 1:30:56 PM}						if (nStressPeriod == 1)
// COMMENT: {4/11/2005 1:30:56 PM}						{
// COMMENT: {4/11/2005 1:30:56 PM}							(*iter)->Serialize(bStoring, zone_id, this->GetUnits());
// COMMENT: {4/11/2005 1:30:56 PM}						}
// COMMENT: {4/11/2005 1:30:56 PM}						else
// COMMENT: {4/11/2005 1:30:56 PM}						{
// COMMENT: {4/11/2005 1:30:56 PM}							(*iter)->SerializeStressPeriod(nStressPeriod, bStoring, zone_id);
// COMMENT: {4/11/2005 1:30:56 PM}						}
// COMMENT: {4/11/2005 1:30:56 PM}
// COMMENT: {4/11/2005 1:30:56 PM}						// close the szZones group
// COMMENT: {4/11/2005 1:30:56 PM}						status = ::H5Gclose(zone_id);
// COMMENT: {4/11/2005 1:30:56 PM}						ASSERT(status >= 0);
// COMMENT: {4/11/2005 1:30:56 PM}					}
// COMMENT: {4/11/2005 1:30:56 PM}				}
// COMMENT: {4/11/2005 1:30:56 PM}			}
// COMMENT: {4/11/2005 1:30:56 PM}			// close the szMedia group
// COMMENT: {4/11/2005 1:30:56 PM}			status = ::H5Gclose(bc_id);
// COMMENT: {4/11/2005 1:30:56 PM}			ASSERT(status >= 0);
// COMMENT: {4/11/2005 1:30:56 PM}		}
// COMMENT: {4/11/2005 1:30:56 PM}	}
// COMMENT: {4/11/2005 1:30:56 PM}	else
// COMMENT: {4/11/2005 1:30:56 PM}	{
// COMMENT: {4/11/2005 1:30:56 PM}		// add loading code here
// COMMENT: {4/11/2005 1:30:56 PM}
// COMMENT: {4/11/2005 1:30:56 PM}		// Open the szBC group
// COMMENT: {4/11/2005 1:30:56 PM}		bc_id = ::H5Gopen(loc_id, szBC);
// COMMENT: {4/11/2005 1:30:56 PM}		ASSERT(bc_id > 0);
// COMMENT: {4/11/2005 1:30:56 PM}		if (bc_id > 0)
// COMMENT: {4/11/2005 1:30:56 PM}		{
// COMMENT: {4/11/2005 1:30:56 PM}			std::list<std::string> listNames;
// COMMENT: {4/11/2005 1:30:56 PM}			CGlobal::ReadList(bc_id, szZones, listNames);
// COMMENT: {4/11/2005 1:30:56 PM}			std::list<std::string>::iterator iter = listNames.begin();
// COMMENT: {4/11/2005 1:30:56 PM}			for (; iter != listNames.end(); ++iter)
// COMMENT: {4/11/2005 1:30:56 PM}			{
// COMMENT: {4/11/2005 1:30:56 PM}				// open zone group
// COMMENT: {4/11/2005 1:30:56 PM}				zone_id = ::H5Gopen(bc_id, (*iter).c_str());
// COMMENT: {4/11/2005 1:30:56 PM}				ASSERT(zone_id > 0);
// COMMENT: {4/11/2005 1:30:56 PM}				if (zone_id > 0)
// COMMENT: {4/11/2005 1:30:56 PM}				{
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:50 PM}					if (nStressPeriod == 1)
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:50 PM}					{
// COMMENT: {4/11/2005 1:30:56 PM}						CBCZoneActor* pZone = CBCZoneActor::New();
// COMMENT: {4/11/2005 1:30:56 PM}
// COMMENT: {4/11/2005 1:30:56 PM}						// load bc
// COMMENT: {4/11/2005 1:30:56 PM}						pZone->SetName((*iter).c_str());
// COMMENT: {4/11/2005 1:30:56 PM}						pZone->Serialize(bStoring, zone_id, this->GetUnits());
// COMMENT: {4/11/2005 1:30:56 PM}						this->Add(pZone);
// COMMENT: {4/11/2005 1:30:56 PM}						pZone->Delete(); // ok ref counted
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}					}
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}					else
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}					{
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}						CTreeCtrlNode nodeBC = this->GetPropertyTreeControlBar()->GetBCNode(1);
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}						int nCount = nodeBC.GetChildCount();
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}						for (int i = 0; i < nCount; ++i)
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}						{
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}							if (CBCZoneActor *pZone = CBCZoneActor::SafeDownCast((vtkObject*)nodeBC.GetChildAt(i).GetData()))
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}							{
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}								if (::strcmp(pZone->GetName(), (*iter).c_str()) == 0)
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}								{
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}									// load bc for stress period
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}									pZone->SerializeStressPeriod(nStressPeriod, bStoring, zone_id);
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}									pZone->UpdateTree(this->GetPropertyTreeControlBar(), nStressPeriod);
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}									break;
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}								}
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}							}
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}						}
// COMMENT: {4/11/2005 1:30:56 PM}// COMMENT: {4/11/2005 1:13:57 PM}					}
// COMMENT: {4/11/2005 1:30:56 PM}
// COMMENT: {4/11/2005 1:30:56 PM}					// close the szZones group
// COMMENT: {4/11/2005 1:30:56 PM}					status = ::H5Gclose(zone_id);
// COMMENT: {4/11/2005 1:30:56 PM}					ASSERT(status >= 0);
// COMMENT: {4/11/2005 1:30:56 PM}				}
// COMMENT: {4/11/2005 1:30:56 PM}			}
// COMMENT: {4/11/2005 1:30:56 PM}
// COMMENT: {4/11/2005 1:30:56 PM}			// close the szBC group
// COMMENT: {4/11/2005 1:30:56 PM}			status = ::H5Gclose(bc_id);
// COMMENT: {4/11/2005 1:30:56 PM}			ASSERT(status >= 0);
// COMMENT: {4/11/2005 1:30:56 PM}		}
// COMMENT: {4/11/2005 1:30:56 PM}	}
// COMMENT: {4/11/2005 1:30:56 PM}}
void CWPhastDoc::SerializeBC(bool bStoring, hid_t loc_id)
{
	static const char szBC[]    = "BC";
	static const char szZones[] = "Zones";

	hid_t bc_id;
	hid_t zone_id;
	herr_t status;

	if (bStoring)
	{
		// add storing code here

		// Create the szBC group
		bc_id = ::H5Gcreate(loc_id, szBC, 0); // always created even if empty
		ASSERT(bc_id > 0);
		if (bc_id > 0)
		{
			CTreeCtrlNode nodeBC = this->GetPropertyTreeControlBar()->GetBCNode();
			int nCount = nodeBC.GetChildCount();
			std::list<LPCTSTR> listNames;
			std::list<CBCZoneActor*> listZones;
			for (int i = 0; i < nCount; ++i)
			{
				if (CBCZoneActor *pZone = CBCZoneActor::SafeDownCast((vtkObject*)nodeBC.GetChildAt(i).GetData()))
				{
					listZones.push_back(pZone);
					ASSERT(pZone->GetName());
					listNames.push_back(pZone->GetName());
				}
				else ASSERT(FALSE);
			}
			if (listNames.size() > 0)
			{
				CGlobal::WriteList(bc_id, szZones, listNames);
				std::list<CBCZoneActor*>::iterator iter = listZones.begin();
				for (; iter != listZones.end(); ++iter)
				{
					// create zone group
					zone_id = ::H5Gcreate(bc_id, (*iter)->GetName(), 0);
					ASSERT(zone_id > 0);
					if (zone_id > 0)
					{
						(*iter)->Serialize(bStoring, zone_id, this->GetUnits());

						// close the szZones group
						status = ::H5Gclose(zone_id);
						ASSERT(status >= 0);
					}
				}
			}
			// close the szMedia group
			status = ::H5Gclose(bc_id);
			ASSERT(status >= 0);
		}
	}
	else
	{
		// add loading code here

		// Open the szBC group
		bc_id = ::H5Gopen(loc_id, szBC);
		ASSERT(bc_id > 0);
		if (bc_id > 0)
		{
			std::list<std::string> listNames;
			CGlobal::ReadList(bc_id, szZones, listNames);
			std::list<std::string>::iterator iter = listNames.begin();
			for (; iter != listNames.end(); ++iter)
			{
				// open zone group
				zone_id = ::H5Gopen(bc_id, (*iter).c_str());
				ASSERT(zone_id > 0);
				if (zone_id > 0)
				{
					CBCZoneActor* pZone = CBCZoneActor::New();

					// load bc
					pZone->SetName((*iter).c_str());
					pZone->Serialize(bStoring, zone_id, this->GetUnits());
					this->Add(pZone);
					pZone->Delete(); // ok ref counted

					// close the szZones group
					status = ::H5Gclose(zone_id);
					ASSERT(status >= 0);
				}
			}

			// close the szBC group
			status = ::H5Gclose(bc_id);
			ASSERT(status >= 0);
		}
	}
}

void CWPhastDoc::SerializeWells(bool bStoring, hid_t loc_id)
{
	static const char szWells[] = "Wells";

	hid_t wells_id;
	herr_t status;

	if (bStoring)
	{
		// Create the wells group
		wells_id = ::H5Gcreate(loc_id, szWells, 0); // always created even if empty
		ASSERT(wells_id > 0);
		if (wells_id > 0)
		{
			// create list of wells and well names
			//
			CTreeCtrlNode nodeWells = this->GetPropertyTreeControlBar()->GetWellsNode();
			int nCount = nodeWells.GetChildCount();
			std::list<LPCTSTR> listSerialNames;
			std::list<CWellActor*> listWells;
			for (int i = 0; i < nCount; ++i)
			{
				if (CWellActor *pWellActor = CWellActor::SafeDownCast((vtkObject*)nodeWells.GetChildAt(i).GetData()))
				{
					listWells.push_back(pWellActor);
					listSerialNames.push_back(pWellActor->GetSerialName());
				}
				else ASSERT(FALSE);
			}

			if (listSerialNames.size() > 0)
			{
				// store well names
				//
				CGlobal::WriteList(wells_id, szWells, listSerialNames);

				// store each well
				//
				std::list<CWellActor*>::iterator iter = listWells.begin();
				for (; iter != listWells.end(); ++iter)
				{
					hid_t well_id = ::H5Gcreate(wells_id, (*iter)->GetSerialName(), 0);
					ASSERT(well_id > 0);
					if (well_id > 0)
					{
						// store well
						(*iter)->Serialize(true, well_id, this->GetUnits());

						// close the well group
						status = ::H5Gclose(well_id);
						ASSERT(status >= 0);
					}
				}
			}

			// close the wells group
			//
			status = ::H5Gclose(wells_id);
			ASSERT(status >= 0);
		}
	}
	else
	{
		// Open the wells group
		//
		wells_id = ::H5Gopen(loc_id, szWells);
		/// ASSERT(wells_id > 0);
		if (wells_id > 0)
		{
			// read in the list of well names
			//
			std::list<std::string> listSerialNames;
			CGlobal::ReadList(wells_id, szWells, listSerialNames);
			std::list<std::string>::iterator iter = listSerialNames.begin();
			for (; iter != listSerialNames.end(); ++iter)
			{
				hid_t well_id = ::H5Gopen(wells_id, (*iter).c_str());
				ASSERT(well_id > 0);
				if (well_id > 0)
				{
					CWellActor *pWellActor = CWellActor::New();
					pWellActor->Serialize(bStoring, well_id, this->GetUnits());

					this->Add(pWellActor);
					pWellActor->Delete(); // ok ref counted

					// close the well group
					//
					status = ::H5Gclose(well_id);
					ASSERT(status >= 0);
				}
			}
			// close the wells group
			// 
			status = ::H5Gclose(wells_id);
			ASSERT(status >= 0);
		}
	}
}

void CWPhastDoc::SerializeRivers(bool bStoring, hid_t loc_id)
{
	static const char szRivers[] = "Rivers";

	hid_t rivers_id;
	herr_t status;

	if (bStoring)
	{
		// Create the rivers group
		rivers_id = ::H5Gcreate(loc_id, szRivers, 0); // always created even if empty
		ASSERT(rivers_id > 0);
		if (rivers_id > 0)
		{
			// create list of rivers and river names
			//
			CTreeCtrlNode nodeRivers = this->GetPropertyTreeControlBar()->GetRiversNode();
			int nCount = nodeRivers.GetChildCount();
			std::list<LPCTSTR> listSerialNames;
			std::list<CRiverActor*> listRivers;
			for (int i = 0; i < nCount; ++i)
			{
				if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast((vtkObject*)nodeRivers.GetChildAt(i).GetData()))
				{
					listRivers.push_back(pRiverActor);
					listSerialNames.push_back(pRiverActor->GetSerialName());
				}
				else ASSERT(FALSE);
			}

			if (listSerialNames.size() > 0)
			{
				// store river names
				//
				CGlobal::WriteList(rivers_id, szRivers, listSerialNames);

				// store each river
				//
				std::list<CRiverActor*>::iterator iter = listRivers.begin();
				for (; iter != listRivers.end(); ++iter)
				{
					hid_t riv_id = ::H5Gcreate(rivers_id, (*iter)->GetSerialName(), 0);
					ASSERT(riv_id > 0);
					if (riv_id > 0)
					{
						// store river
						(*iter)->Serialize(true, riv_id, this->GetUnits());

						// close the river group
						status = ::H5Gclose(riv_id);
						ASSERT(status >= 0);
					}
				}
			}

			// close the rivers group
			//
			status = ::H5Gclose(rivers_id);
			ASSERT(status >= 0);
		}
	}
	else
	{
		// Open the rivers group
		//
		rivers_id = ::H5Gopen(loc_id, szRivers);
		/// ASSERT(rivers_id > 0);
		if (rivers_id > 0)
		{
			// read in the list of river names
			//
			std::list<std::string> listSerialNames;
			CGlobal::ReadList(rivers_id, szRivers, listSerialNames);
			std::list<std::string>::iterator iter = listSerialNames.begin();
			for (; iter != listSerialNames.end(); ++iter)
			{
				hid_t riv_id = ::H5Gopen(rivers_id, (*iter).c_str());
				ASSERT(riv_id > 0);
				if (riv_id > 0)
				{
					CRiverActor *pRiverActor = CRiverActor::New();
					pRiverActor->Serialize(bStoring, riv_id, this->GetUnits());

					this->Add(pRiverActor);
					pRiverActor->Delete(); // ok ref counted

					// close the river group
					//
					status = ::H5Gclose(riv_id);
					ASSERT(status >= 0);
				}
			}
			// close the rivers group
			// 
			status = ::H5Gclose(rivers_id);
			ASSERT(status >= 0);
		}
	}
}

void CWPhastDoc::SerializeIC(bool bStoring, hid_t loc_id)
{
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
			CTreeCtrlNode nodeIC = this->GetPropertyTreeControlBar()->GetICNode();
			int nCount = nodeIC.GetChildCount();
			std::list<LPCTSTR> listNames;
			std::list<CICZoneActor*> listZones;
			for (int i = 0; i < nCount; ++i)
			{
				if (CICZoneActor *pZone = CICZoneActor::SafeDownCast((vtkObject*)nodeIC.GetChildAt(i).GetData()))
				{
					listZones.push_back(pZone);
					ASSERT(pZone->GetName());
					listNames.push_back(pZone->GetSerialName());
				}
				else ASSERT(FALSE);
			}
			if (listNames.size() > 0)
			{
				CGlobal::WriteList(ic_id, szZones, listNames);
				std::list<CICZoneActor*>::iterator iter = listZones.begin();
				for (; iter != listZones.end(); ++iter)
				{
					// create zone group
					zone_id = ::H5Gcreate(ic_id, (*iter)->GetSerialName(), 0);
					ASSERT(zone_id > 0);
					if (zone_id > 0)
					{
						// store head_ic
						(*iter)->Serialize(true, zone_id, this->GetUnits());

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
	else
	{
		// add loading code here

		// Open the szIC group
		ic_id = ::H5Gopen(loc_id, szIC);
		ASSERT(ic_id > 0);
		if (ic_id > 0)
		{
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
					CICZoneActor* pZone = CICZoneActor::New();

					// load head_ic/chem_ic
					pZone->SetSerialName((*iter).c_str());
					pZone->Serialize(false, zone_id, this->GetUnits());
					this->Add(pZone);
					pZone->Delete(); // ok ref counted

					// close the szZones group
					status = ::H5Gclose(zone_id);
					ASSERT(status >= 0);
				}
			}

			// close the szIC group
			status = ::H5Gclose(ic_id);
			ASSERT(status >= 0);
		}
	}
}

void CWPhastDoc::SerializeStressPeriods(bool bStoring, hid_t loc_id)
{
	ASSERT(FALSE);
// COMMENT: {4/8/2005 6:56:13 PM}	static const char szStressPeriods[]      = "StressPeriods";
// COMMENT: {4/8/2005 6:56:13 PM}	static const char szStressPeriodFormat[] = "Stress Period %d";
// COMMENT: {4/8/2005 6:56:13 PM}
// COMMENT: {4/8/2005 6:56:13 PM}	hid_t sp_id;
// COMMENT: {4/8/2005 6:56:13 PM}	hid_t period_id;
// COMMENT: {4/8/2005 6:56:13 PM}	herr_t status;
// COMMENT: {4/8/2005 6:56:13 PM}	int nStressPeriodCount;
// COMMENT: {4/8/2005 6:56:13 PM}	CString str;
// COMMENT: {4/8/2005 6:56:13 PM}
// COMMENT: {4/8/2005 6:56:13 PM}	if (bStoring)
// COMMENT: {4/8/2005 6:56:13 PM}	{
// COMMENT: {4/8/2005 6:56:13 PM}		// add storing code here
// COMMENT: {4/8/2005 6:56:13 PM}
// COMMENT: {4/8/2005 6:56:13 PM}		// Create the szStressPeriods group
// COMMENT: {4/8/2005 6:56:13 PM}		sp_id = ::H5Gcreate(loc_id, szStressPeriods, 0); // always created even if empty
// COMMENT: {4/8/2005 6:56:13 PM}		ASSERT(sp_id > 0);
// COMMENT: {4/8/2005 6:56:13 PM}		if (sp_id > 0)
// COMMENT: {4/8/2005 6:56:13 PM}		{
// COMMENT: {4/8/2005 6:56:13 PM}			nStressPeriodCount = this->GetPropertyTreeControlBar()->GetStressPeriodCount();
// COMMENT: {4/8/2005 6:56:13 PM}
// COMMENT: {4/8/2005 6:56:13 PM}			std::vector<std::string> vec_strings;
// COMMENT: {4/8/2005 6:56:13 PM}			vec_strings.reserve(nStressPeriodCount);
// COMMENT: {4/8/2005 6:56:13 PM}			std::list<LPCTSTR> listNames;
// COMMENT: {4/8/2005 6:56:13 PM}			for (int i = 2; i <= nStressPeriodCount; ++i)
// COMMENT: {4/8/2005 6:56:13 PM}			{
// COMMENT: {4/8/2005 6:56:13 PM}				str.Format(szStressPeriodFormat, i);
// COMMENT: {4/8/2005 6:56:13 PM}				vec_strings.push_back((LPCTSTR)str);
// COMMENT: {4/8/2005 6:56:13 PM}				listNames.push_back(vec_strings.back().c_str());
// COMMENT: {4/8/2005 6:56:13 PM}			}
// COMMENT: {4/8/2005 6:56:13 PM}			if (listNames.size() > 0)
// COMMENT: {4/8/2005 6:56:13 PM}			{
// COMMENT: {4/8/2005 6:56:13 PM}				// store list of stress periods
// COMMENT: {4/8/2005 6:56:13 PM}				CGlobal::WriteList(sp_id, szStressPeriods, listNames);
// COMMENT: {4/8/2005 6:56:13 PM}
// COMMENT: {4/8/2005 6:56:13 PM}				// store each stress period
// COMMENT: {4/8/2005 6:56:13 PM}				std::list<LPCTSTR>::iterator iter = listNames.begin();
// COMMENT: {4/8/2005 6:56:13 PM}				for (int i = 2; iter != listNames.end(); ++iter, ++i)
// COMMENT: {4/8/2005 6:56:13 PM}				{
// COMMENT: {4/8/2005 6:56:13 PM}					// create stress period group
// COMMENT: {4/8/2005 6:56:13 PM}					period_id = ::H5Gcreate(sp_id, (*iter), 0);
// COMMENT: {4/8/2005 6:56:13 PM}					ASSERT(period_id > 0);
// COMMENT: {4/8/2005 6:56:13 PM}					if (period_id > 0)
// COMMENT: {4/8/2005 6:56:13 PM}					{
// COMMENT: {4/8/2005 6:56:13 PM}						// store BCs
// COMMENT: {4/8/2005 6:56:13 PM}						this->SerializeBC(bStoring, period_id, i);
// COMMENT: {4/8/2005 6:56:13 PM}
// COMMENT: {4/8/2005 6:56:13 PM}						// store time_control 
// COMMENT: {4/8/2005 6:56:13 PM}						this->SerializeTimeControl(bStoring, period_id, i);
// COMMENT: {4/8/2005 6:56:13 PM}
// COMMENT: {4/8/2005 6:56:13 PM}						// close the stress period group group
// COMMENT: {4/8/2005 6:56:13 PM}						status = ::H5Gclose(period_id);
// COMMENT: {4/8/2005 6:56:13 PM}						ASSERT(status >= 0);
// COMMENT: {4/8/2005 6:56:13 PM}					}
// COMMENT: {4/8/2005 6:56:13 PM}				}
// COMMENT: {4/8/2005 6:56:13 PM}			}
// COMMENT: {4/8/2005 6:56:13 PM}			// close the szStressPeriods group
// COMMENT: {4/8/2005 6:56:13 PM}			status = ::H5Gclose(sp_id);
// COMMENT: {4/8/2005 6:56:13 PM}			ASSERT(status >= 0);
// COMMENT: {4/8/2005 6:56:13 PM}		}
// COMMENT: {4/8/2005 6:56:13 PM}	}
// COMMENT: {4/8/2005 6:56:13 PM}	else
// COMMENT: {4/8/2005 6:56:13 PM}	{
// COMMENT: {4/8/2005 6:56:13 PM}		// add loading code here
// COMMENT: {4/8/2005 6:56:13 PM}
// COMMENT: {4/8/2005 6:56:13 PM}		// Open the szStressPeriods group
// COMMENT: {4/8/2005 6:56:13 PM}		sp_id = ::H5Gopen(loc_id, szStressPeriods);
// COMMENT: {4/8/2005 6:56:13 PM}		ASSERT(sp_id > 0);
// COMMENT: {4/8/2005 6:56:13 PM}		if (sp_id > 0)
// COMMENT: {4/8/2005 6:56:13 PM}		{
// COMMENT: {4/8/2005 6:56:13 PM}			std::list<std::string> listNames;
// COMMENT: {4/8/2005 6:56:13 PM}			CGlobal::ReadList(sp_id, szStressPeriods, listNames);
// COMMENT: {4/8/2005 6:56:13 PM}			std::list<std::string>::iterator iter = listNames.begin();
// COMMENT: {4/8/2005 6:56:13 PM}			for (int i = 2; iter != listNames.end(); ++iter, ++i)
// COMMENT: {4/8/2005 6:56:13 PM}			{
// COMMENT: {4/8/2005 6:56:13 PM}				// open zone group
// COMMENT: {4/8/2005 6:56:13 PM}				period_id = ::H5Gopen(sp_id, (*iter).c_str());
// COMMENT: {4/8/2005 6:56:13 PM}				ASSERT(period_id > 0);
// COMMENT: {4/8/2005 6:56:13 PM}				if (period_id > 0)
// COMMENT: {4/8/2005 6:56:13 PM}				{
// COMMENT: {4/8/2005 6:56:13 PM}					// load time_control 
// COMMENT: {4/8/2005 6:56:13 PM}					this->SerializeTimeControl(bStoring, period_id, i);
// COMMENT: {4/8/2005 6:56:13 PM}
// COMMENT: {4/8/2005 6:56:13 PM}					// load BCs
// COMMENT: {4/8/2005 6:56:13 PM}					this->SerializeBC(bStoring, period_id, i);
// COMMENT: {4/8/2005 6:56:13 PM}
// COMMENT: {4/8/2005 6:56:13 PM}					// close the stress period group group
// COMMENT: {4/8/2005 6:56:13 PM}					status = ::H5Gclose(period_id);
// COMMENT: {4/8/2005 6:56:13 PM}					ASSERT(status >= 0);
// COMMENT: {4/8/2005 6:56:13 PM}				}
// COMMENT: {4/8/2005 6:56:13 PM}			}
// COMMENT: {4/8/2005 6:56:13 PM}			// close the szStressPeriods group
// COMMENT: {4/8/2005 6:56:13 PM}			status = ::H5Gclose(sp_id);
// COMMENT: {4/8/2005 6:56:13 PM}			ASSERT(status >= 0);
// COMMENT: {4/8/2005 6:56:13 PM}		}
// COMMENT: {4/8/2005 6:56:13 PM}	}
}

// COMMENT: {4/11/2005 1:34:09 PM}void CWPhastDoc::SerializeTimeControl(bool bStoring, hid_t loc_id, int nStressPeriod)
// COMMENT: {4/11/2005 1:34:09 PM}{
// COMMENT: {4/11/2005 1:34:09 PM}	ASSERT(nStressPeriod > 1); // use this->m_pTimeControl->Serialize instead
// COMMENT: {4/11/2005 1:34:09 PM}	if (bStoring)
// COMMENT: {4/11/2005 1:34:09 PM}	{
// COMMENT: {4/11/2005 1:34:09 PM}		// store time_control 
// COMMENT: {4/11/2005 1:34:09 PM}		CTreeCtrlNode nodeTC = this->GetPropertyTreeControlBar()->GetTimeControlNode(nStressPeriod);
// COMMENT: {4/11/2005 1:34:09 PM}		ASSERT(nodeTC.GetData());
// COMMENT: {4/11/2005 1:34:09 PM}		if (CTimeControl* pTimeControl = (CTimeControl*)(nodeTC.GetData())) {
// COMMENT: {4/11/2005 1:34:09 PM}			pTimeControl->Serialize(bStoring, loc_id);
// COMMENT: {4/11/2005 1:34:09 PM}		}
// COMMENT: {4/11/2005 1:34:09 PM}	}
// COMMENT: {4/11/2005 1:34:09 PM}	else
// COMMENT: {4/11/2005 1:34:09 PM}	{
// COMMENT: {4/11/2005 1:34:09 PM}		// load time_control 
// COMMENT: {4/11/2005 1:34:09 PM}		CTimeControl tc;
// COMMENT: {4/11/2005 1:34:09 PM}		tc.Serialize(bStoring, loc_id);
// COMMENT: {4/11/2005 1:34:09 PM}// COMMENT: {4/8/2005 6:52:58 PM}		this->AddStressPeriod(tc);
// COMMENT: {4/11/2005 1:34:09 PM}	}
// COMMENT: {4/11/2005 1:34:09 PM}}

// CWPhastDoc diagnostics

#ifdef _DEBUG
void CWPhastDoc::AssertValid() const
{
	CDocument::AssertValid();
	ASSERT(this->m_pAxesActor != 0);
// COMMENT: {7/15/2004 1:15:38 PM}	ASSERT(this->m_pGeometrySheet != 0);
// COMMENT: {7/28/2005 2:55:59 PM}	ASSERT(this->m_pGridLODActor != 0);
	ASSERT(this->m_pimpl != 0);
	ASSERT(this->m_pPropCollection != 0);
	ASSERT(this->m_pRemovedPropCollection != 0);
// COMMENT: {7/15/2004 1:15:34 PM}	ASSERT(this->m_pScalePage != 0);
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
	ASSERT(FALSE);
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
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	return TRUE;
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

void CWPhastDoc::InitDocument()
{
	TRACE("CWPhastDoc::InitDocument()\n");

	//// create pimpl
	////
	//ASSERT(this->m_pimpl == 0);
	//this->m_pimpl = new WPhastDocImpl();
	//this->m_pimpl->m_vectorActionsIndex = 0;

	// create the list
	//
	// ASSERT(this->m_pPropCollection == 0);
	// this->m_pPropCollection = vtkPropCollection::New();

	////// create the grid
	//////
	////ASSERT(this->m_pGridLODActor == 0);
	////this->m_pGridLODActor = CGridLODActor::New();
	////this->m_pGridLODActor->GetProperty()->SetColor(1.0, 0.8, 0.6);

	//////// create the axes
	////////
	//////ASSERT(this->m_pAxes == 0);
	//////ASSERT(this->m_pAxesTubeFilter == 0);
	//////ASSERT(this->m_pAxesPolyDataMapper == 0);
	//////ASSERT(this->m_pAxesActor == 0);
	//////this->m_pAxes = vtkAxes::New();
	//////this->m_pAxes->SetOrigin(0, 0, 0);
	//////this->m_pAxesTubeFilter = vtkTubeFilter::New();
	//////this->m_pAxesTubeFilter->SetInput(this->m_pAxes->GetOutput());
	//////this->m_pAxesTubeFilter->SetNumberOfSides(10);
	//////this->m_pAxesPolyDataMapper = vtkPolyDataMapper::New();
	//////this->m_pAxesPolyDataMapper->SetInput(this->m_pAxesTubeFilter->GetOutput());
	//////this->m_pAxesActor = vtkActor::New();
	//////this->m_pAxesActor->SetMapper(this->m_pAxesPolyDataMapper);
	//////this->m_pAxesActor->SetPickable(0);

	//if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar()) {
	//	pTree->InitDocument();
	//}
	//else {
	//	ASSERT(FALSE);
	//}

	/****
	// set scale
	//
	for (int i = 0; i < 3; ++i) {
		this->m_Scale[i] = 1.0;
	}

	// Update scale page
	//
	this->m_pScalePage->m_XScale = m_Scale[0];
	this->m_pScalePage->m_YScale = m_Scale[1];
	this->m_pScalePage->m_ZScale = m_Scale[2];
	if (this->m_pScalePage->GetSafeHwnd()) {
		this->m_pScalePage->UpdateData(FALSE);
	}
	****/

	// properties
	//
	this->m_nNextZone = 1;
}

///////////////////////////////////////////////////////////////////////////////////////
#define CLEAR_PROP_ASSEMBLY_MACRO(PROP_ASSEMBLY_PTR) \
do { \
	ASSERT(PROP_ASSEMBLY_PTR); \
	if (PROP_ASSEMBLY_PTR) { \
		if (vtkPropCollection *pPropCollection = PROP_ASSEMBLY_PTR->GetParts()) { \
			pPropCollection->RemoveAllItems(); \
		} \
	} \
} while(0)
///////////////////////////////////////////////////////////////////////////////////////

void CWPhastDoc::DeleteContents()
{
	// Add your specialized code here and/or call the base class
	TRACE("CWPhastDoc::DeleteContents()\n");

	ASSERT(this->m_pAxesActor);

	// init next zone number
	this->m_nNextZone = 1;

	// reset model
	ASSERT(this->m_pModel);
	delete this->m_pModel;
	this->m_pModel = new CNewModel();
	this->m_pModel->m_printFreq = CPrintFreq::NewDefaults();

	// reset units
	ASSERT(this->m_pUnits);
	delete this->m_pUnits;
	this->m_pUnits = new CUnits();

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

	//{{
	// create the grid
	//
	if (this->m_pGridLODActor)
	{
		this->m_pGridLODActor->Delete();
	}
	this->m_pGridLODActor = CGridLODActor::New();
	this->m_pGridLODActor->GetProperty()->SetColor(1.0, 0.8, 0.6);
	//}}
	ASSERT(this->m_pGridLODActor);
	this->m_pGridLODActor->SetScale(1, 1, 1);


	// update views
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL)
	{
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		ASSERT(this->m_pPropCollection);
		if (this->m_pPropCollection)
		{
			// remove all props
			this->m_pPropCollection->InitTraversal();
			for (int i = 0; i < this->m_pPropCollection->GetNumberOfItems(); ++i)
			{
				vtkProp* prop = this->m_pPropCollection->GetNextProp();
				pView->GetRenderer()->GetProps()->RemoveItem(prop);
			}
		}

		// all vtkProps must release before the Renderer is destroyed
		// ie when an import fails
		this->m_pGridLODActor->ReleaseGraphicsResources(pView->GetRenderer()->GetVTKWindow());
		this->m_pAxesActor->ReleaseGraphicsResources(pView->GetRenderer()->GetVTKWindow());		

		pView->DeleteContents();
	}

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

	// Turn-all on by default
	//
	this->m_pPropAssemblyMedia->SetVisibility(1);
	this->m_pPropAssemblyIC->SetVisibility(1);
	this->m_pPropAssemblyBC->SetVisibility(1);
	this->m_pPropAssemblyWells->SetVisibility(1);
	this->m_pPropAssemblyRivers->SetVisibility(1);

	// reset scale
	//
	
// COMMENT: {7/28/2005 4:38:34 PM}	//{{
// COMMENT: {7/28/2005 4:38:34 PM}	// create the grid
// COMMENT: {7/28/2005 4:38:34 PM}	//
// COMMENT: {7/28/2005 4:38:34 PM}	if (this->m_pGridLODActor)
// COMMENT: {7/28/2005 4:38:34 PM}	{
// COMMENT: {7/28/2005 4:38:34 PM}		this->m_pGridLODActor->Delete();
// COMMENT: {7/28/2005 4:38:34 PM}	}
// COMMENT: {7/28/2005 4:38:34 PM}	this->m_pGridLODActor = CGridLODActor::New();
// COMMENT: {7/28/2005 4:38:34 PM}	this->m_pGridLODActor->GetProperty()->SetColor(1.0, 0.8, 0.6);
// COMMENT: {7/28/2005 4:38:34 PM}	//}}
// COMMENT: {7/28/2005 4:38:34 PM}	ASSERT(this->m_pGridLODActor);
// COMMENT: {7/28/2005 4:38:34 PM}	this->m_pGridLODActor->SetScale(1, 1, 1);

	if (this->m_pMapActor)
	{
		this->m_pMapActor->Delete();
		this->m_pMapActor = 0;
	}

	// update geometry property sheet
	// Note: can't call this->SetScale(1.0f, 1.0f, 1.0f);
	// Since the this->m_pGridLODActor contains no data for
	// this->m_pGridLODActor->GetBounds
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

	this->UpdateAllViews(0);

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
	pCmdUI->Enable(this->m_pimpl->m_vectorActionsIndex > 0);
}

void CWPhastDoc::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(this->m_pimpl->m_vectorActionsIndex < this->m_pimpl->m_vectorActions.size());
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

void CWPhastDoc::SetScale(vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z)
{
	ASSERT(x != 0);
	ASSERT(y != 0);
	ASSERT(z != 0);

	vtkFloatingPointType scale[3];
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
	this->m_pGridLODActor->SetScale(scale);

	// reset the axes
	//
	vtkFloatingPointType bounds[6];
	this->m_pGridLODActor->GetBounds(bounds);
	float defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
	this->m_pAxesActor->SetDefaultPositions(bounds);
	this->m_pAxesActor->SetDefaultSize(defaultAxesSize);
	this->m_pAxesActor->SetDefaultTubeDiameter(defaultAxesSize * 0.1);

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
		pCollection->InitTraversal();
		for (int i = 0; i < pCollection->GetNumberOfItems(); ++i)
		{
			vtkProp* prop = pCollection->GetNextProp();
			if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop))
			{
				pZone->SetScale(scale);
			}
			if (vtkAssembly *pAssembly = vtkAssembly::SafeDownCast(prop))
			{
				ASSERT(FALSE); // no longer using vtkAssembly ???
				pAssembly->SetScale(scale);
			}
			if (vtkPropAssembly *pPropAssembly = vtkPropAssembly::SafeDownCast(prop))
			{
				if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
				{
					vtkProp *pProp;
					pPropCollection->InitTraversal();
					for (; (pProp = pPropCollection->GetNextProp()); )
					{
						if (vtkProp3D *prop3D = vtkProp3D::SafeDownCast(pProp))
						{
							prop3D->SetScale(scale);
							if (CWellActor *pWellActor = CWellActor::SafeDownCast(prop3D))
							{
// COMMENT: {7/12/2005 3:21:11 PM}								pWellActor->SetDefaultTubeDiameter(defaultAxesSize * 0.17 / sqrt(scale[0] * scale[1]));
								pWellActor->SetDefaultTubeDiameter(defaultAxesSize * 0.17);
								// pWellActor->SetRadius(defaultAxesSize * 0.085);
							}
							if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(prop3D))
							{
								pRiverActor->SetRadius(defaultAxesSize * 0.085);
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

		// resize the selection bounding box
		//
		if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( pView->GetRenderWindowInteractor()->GetPicker() ))
		{
			if (vtkProp3D* prop = picker->GetProp3D())
			{
				if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop))
				{
					pZone->Select(pView);
				}
				else
				{
					pView->HighlightProp3D(prop); // doesn't immediately render
				}
			}
		}

		// resize the well interactor
		//

		// resize the Box Widget
		//
		if (pView->GetBoxWidget()->GetProp3D())
		{
			pView->GetBoxWidget()->PlaceWidget();
		}

		// reset the camera
		//
		pView->ResetCamera();
	}
	//{{
	this->Notify(0, WPN_SCALE_CHANGED, 0, 0);
	//}}
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

// COMMENT: {6/3/2004 5:10:37 PM}const CFlowOnly& CWPhastDoc::GetFlowOnly(void)const
// COMMENT: {6/3/2004 5:10:37 PM}{
// COMMENT: {6/3/2004 5:10:37 PM}	return this->m_pModel->m_flowOnly;
// COMMENT: {6/3/2004 5:10:37 PM}}

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

//void CWPhastDoc::SetGrid(CGrid* x, CGrid* y, CGrid* z)
//{
//	ASSERT(this->m_pGridLODActor);
//	ASSERT(this->m_pAxes);
//	ASSERT(this->m_pAxesTubeFilter);
//	ASSERT(this->m_pAxesActor);
//	ASSERT(this->m_pPropCollection);
//	ASSERT(this->m_pimpl);
//	ASSERT(this->m_pUnits);
//	ASSERT(this->m_pFlowOnly);
//
//	// set flowonly
//	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar()) {
//		pTree->SetFlowOnly(this->m_pFlowOnly);
//	}
//
//	// set the units
//	//
//	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar()) {
//		pTree->SetUnits(this->m_pUnits);
//	}
//
//	// set the grid
//	//
//	this->m_pGridLODActor->SetGrid(x, y, z, this->m_pUnits);
//	this->m_pGridLODActor->SetPickable(0);
//	this->m_pPropCollection->AddItem(this->m_pGridLODActor);
//	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar()) {
//		pTree->SetGridLODActor(this->m_pGridLODActor);
//	}
//
//	// set the axes
//	//
//	float bounds[6];
//	this->m_pGridLODActor->GetBounds(bounds);
//	this->m_pAxes->SetScaleFactor((bounds[1] - bounds[0])/5);
//	this->m_pAxesTubeFilter->SetRadius(this->m_pAxes->GetScaleFactor()/25.0);
//	this->m_pPropCollection->AddItem(this->m_pAxesActor);
//
//	// refresh screen
//	//
//	this->UpdateAllViews(0);
//}

void CWPhastDoc::ResizeGrid(const CGrid& x, const CGrid&  y, const CGrid&  z)
{
	ASSERT(this->m_pGridLODActor);
	ASSERT(this->m_pAxesActor);
	ASSERT(this->m_pPropCollection);
	ASSERT(this->m_pimpl);
	ASSERT(this->m_pUnits);
	ASSERT(this->m_pModel);

	// reset the grid
	//
	this->m_pGridLODActor->SetGrid(x, y, z, *this->m_pUnits);
	this->m_pGridLODActor->SetPickable(0);
	this->m_pPropCollection->AddItem(this->m_pGridLODActor);
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetGridLODActor(this->m_pGridLODActor);
	}

	// reset the axes
	//
	vtkFloatingPointType bounds[6];
	this->m_pGridLODActor->GetBounds(bounds);
	float defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
	this->m_pAxesActor->SetDefaultPositions(bounds);
	this->m_pAxesActor->SetDefaultSize(defaultAxesSize);
	this->m_pAxesActor->SetDefaultTubeDiameter(defaultAxesSize * 0.1);

	// update default zones
	//
	if (vtkPropCollection* pCollection = this->GetPropCollection())
	{
		CZone bounds;
		this->m_pGridLODActor->GetDefaultZone(bounds);
		pCollection->InitTraversal();
		for (int i = 0; i < pCollection->GetNumberOfItems(); ++i)
		{
			vtkProp* prop = pCollection->GetNextProp();
			if (vtkPropAssembly *pPropAssembly = vtkPropAssembly::SafeDownCast(prop))
			{
				if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
				{
					pPropCollection->InitTraversal();
					for (int i = 0; i < pPropCollection->GetNumberOfItems(); ++i)
					{
						vtkProp* prop = pPropCollection->GetNextProp();
						if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop))
						{
							if (pZone->GetDefault())
							{
								pZone->SetBounds(bounds, *this->m_pUnits);
							}
						}
					}
				}
			}
			if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop))
			{
				if (pZone->GetDefault())
				{
					pZone->SetBounds(bounds, *this->m_pUnits);
				}
			}
		}
	}

#ifdef SAVE_2003_10_24_2030
	// This used to resize the highlight box and BoxWidget if 
	// the current zone was a default zone.  Since default zones
	// are no longer pickable this code has become obsolete.
	//
	// Note. That this code caused an exception in Release builds.
	// To reproduce:
	// 1. New... (accept all defaults)
	// 2. create a zone
	// 3. double-click on grid in tree (Press OK or Apply)
	// This problem didn't occur when debugging the Release
	// build but did occur when 'Start Without Debugging'
	// and attaching to the process Debug->Processes...

	// for all views
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL) {
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);

		// resize the selection bounding box
		//
		if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( pView->GetRenderWindowInteractor()->GetPicker() )) {
			if (vtkProp3D* prop = picker->GetProp3D()) {
				if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop)) {
					pZone->Pick(pView->GetRenderer(), pView->GetRenderWindowInteractor());
				}
			}
		}

		// resize the Box Widget
		//
		if (pView->GetBoxWidget()->GetProp3D()) {
			pView->GetBoxWidget()->PlaceWidget();
		}
	}
#endif


	// refresh screen
	//
	this->UpdateAllViews(0);
}


void CWPhastDoc::AddDefaultZone(CZone* pZone)
{
	if (pZone) {
		this->m_pimpl->m_vectorDefaultZones.push_back(pZone);
	}
}

vtkFloatingPointType* CWPhastDoc::GetScale()
{
	return this->m_pGridLODActor->GetScale();
}

void CWPhastDoc::GetScale(vtkFloatingPointType data[3])
{
	this->m_pGridLODActor->GetScale(data);
}

vtkFloatingPointType* CWPhastDoc::GetGridBounds()
{
	if (this->m_pGridLODActor) {
		return this->m_pGridLODActor->GetBounds();
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
// COMMENT: {3/9/2004 8:21:04 PM}	CPropertySheet sheet;
// COMMENT: {3/9/2004 8:21:04 PM}	CScalePropertyPage page;
// COMMENT: {3/9/2004 8:21:04 PM}	page.m_pDoc = this;
// COMMENT: {3/9/2004 8:21:04 PM}	sheet.AddPage(&page);
// COMMENT: {3/9/2004 8:21:04 PM}
// COMMENT: {3/9/2004 8:21:04 PM}	CTimeControlPropertyPage page2;
// COMMENT: {3/9/2004 8:21:04 PM}	sheet.AddPage(&page2);
// COMMENT: {3/9/2004 8:21:04 PM}
// COMMENT: {3/9/2004 8:21:04 PM}
// COMMENT: {3/9/2004 8:21:04 PM}	INT_PTR id = sheet.DoModal();
// COMMENT: {3/9/2004 8:21:04 PM}#ifdef _DEBUG
// COMMENT: {3/9/2004 8:21:04 PM}	switch(id) {
// COMMENT: {3/9/2004 8:21:04 PM}		case IDOK:
// COMMENT: {3/9/2004 8:21:04 PM}			::AfxMessageBox("Ok pressed");
// COMMENT: {3/9/2004 8:21:04 PM}			break;
// COMMENT: {3/9/2004 8:21:04 PM}		case IDCANCEL:
// COMMENT: {3/9/2004 8:21:04 PM}			::AfxMessageBox("Cancel pressed");
// COMMENT: {3/9/2004 8:21:04 PM}			break;
// COMMENT: {3/9/2004 8:21:04 PM}		default:
// COMMENT: {3/9/2004 8:21:04 PM}			::AfxMessageBox("Unknown pressed");
// COMMENT: {3/9/2004 8:21:04 PM}			break;
// COMMENT: {3/9/2004 8:21:04 PM}	}
// COMMENT: {3/9/2004 8:21:04 PM}#endif

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
	while (pos != NULL) {
		CWPhastView *pView = (CWPhastView*) this->GetNextView(pos);
		ASSERT_VALID(pView);
		pView->ResetCamera();
	}
}

CString CWPhastDoc::GetNextZoneName(void)
{
	return this->GetPropertyTreeControlBar()->GetNextZoneName();
}

int CWPhastDoc::GetNextWellNumber(void)
{
// COMMENT: {6/14/2005 9:06:36 PM}	int nMax = 0;
// COMMENT: {6/14/2005 9:06:36 PM}	if (vtkPropCollection *pPropCollection = this->GetPropAssemblyWells()->GetParts())
// COMMENT: {6/14/2005 9:06:36 PM}	{
// COMMENT: {6/14/2005 9:06:36 PM}		vtkProp *pProp = 0;
// COMMENT: {6/14/2005 9:06:36 PM}		pPropCollection->InitTraversal();
// COMMENT: {6/14/2005 9:06:36 PM}		for (; (pProp = pPropCollection->GetNextProp()); )
// COMMENT: {6/14/2005 9:06:36 PM}		{
// COMMENT: {6/14/2005 9:06:36 PM}			if (CWellActor *pWellActor = CWellActor::SafeDownCast(pProp))
// COMMENT: {6/14/2005 9:06:36 PM}			{
// COMMENT: {6/14/2005 9:06:36 PM}				CWellSchedule well = pWellActor->GetWell();
// COMMENT: {6/14/2005 9:06:36 PM}				nMax = max(nMax, well.n_user);
// COMMENT: {6/14/2005 9:06:36 PM}			}
// COMMENT: {6/14/2005 9:06:36 PM}		}
// COMMENT: {6/14/2005 9:06:36 PM}	}
// COMMENT: {6/14/2005 9:06:36 PM}	return (nMax + 1);
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
	//std::set<int> usedWellNumbers;
	usedNums.clear();
	if (vtkPropCollection *pPropCollection = this->GetPropAssemblyWells()->GetParts())
	{
		vtkProp *pProp = 0;
		pPropCollection->InitTraversal();
		for (; (pProp = pPropCollection->GetNextProp()); )
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
		pPropCollection->InitTraversal();
		for (; (pProp = pPropCollection->GetNextProp()); )
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

BOOL CWPhastDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// Add your specialized code here and/or call the base class
	this->m_pimpl->m_lastSaveIndex = this->m_pimpl->m_vectorActionsIndex;

	return CDocument::OnSaveDocument(lpszPathName);
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

// COMMENT: {7/19/2005 8:13:02 PM}	// Grid
// COMMENT: {7/19/2005 8:13:02 PM}	//
// COMMENT: {7/19/2005 8:13:02 PM}	if (this->m_pGridPage)
// COMMENT: {7/19/2005 8:13:02 PM}	{
// COMMENT: {7/19/2005 8:13:02 PM}		this->m_pGridPage->DestroyWindow();
// COMMENT: {7/19/2005 8:13:02 PM}		delete this->m_pGridPage;
// COMMENT: {7/19/2005 8:13:02 PM}		this->m_pGridPage = 0;
// COMMENT: {7/19/2005 8:13:02 PM}	}
// COMMENT: {7/19/2005 8:13:02 PM}	if (this->m_pGridSheet)
// COMMENT: {7/19/2005 8:13:02 PM}	{
// COMMENT: {7/19/2005 8:13:02 PM}		this->m_pGridSheet->DestroyWindow();
// COMMENT: {7/19/2005 8:13:02 PM}		delete this->m_pGridSheet;
// COMMENT: {7/19/2005 8:13:02 PM}		this->m_pGridSheet = 0;
// COMMENT: {7/19/2005 8:13:02 PM}	}

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

	CDocument* pDoc = this->GetDocTemplate()->OpenDocumentFile(NULL);
	if (pDoc) {
		ASSERT(pDoc == this);
		if (!this->DoImport(szPath)) {
			::AfxMessageBox("An error occured during the import", MB_OK);
			this->SetModifiedFlag(FALSE);
		}
	}
	this->ResetCamera();
}

BOOL CWPhastDoc::DoImport(LPCTSTR lpszPathName)
{
	// Return Value
	// Nonzero if the file was successfully loaded; otherwise 0.
	CWaitCursor wait;

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];

	::_tsplitpath(lpszPathName, szDrive, szDir, szFName, szExt);

	// remove .trans if exists
	//
	CString strPrefix(szFName);
	strPrefix.MakeLower();
	strPrefix.Replace(_T(".trans"), _T(""));

	std::ifstream ifs;
	ifs.open(lpszPathName);
	if (!ifs.is_open())
	{
		return FALSE;
	}


	CPhastInput* pInput = CPhastInput::New(ifs, strPrefix);
	if (!pInput) return FALSE;


	// delay redrawing treectrl
	//
	CWnd *pWnd = 0;
	if (CPropertyTreeControlBar *pPropertyTreeControlBar = this->GetPropertyTreeControlBar())
	{
		pWnd = pPropertyTreeControlBar->GetTreeCtrl();
	}
	CDelayRedraw delayTree(pWnd);

	try
	{
		CPrintFreq printFreq;
		printFreq.InitSync(pInput); // must be called before Load()

		pInput->Load();
// COMMENT: {7/14/2005 7:44:09 PM}		// check for errors
// COMMENT: {7/14/2005 7:44:09 PM}		for (int i = 0; i < 3; ++i)
// COMMENT: {7/14/2005 7:44:09 PM}		{
// COMMENT: {7/14/2005 7:44:09 PM}			if (grid[i].uniform != TRUE)
// COMMENT: {7/14/2005 7:44:09 PM}			{
// COMMENT: {7/14/2005 7:44:09 PM}				::error_msg("Currently only uniform grids are supported.", CONTINUE);
// COMMENT: {7/14/2005 7:44:09 PM}				++input_error;
// COMMENT: {7/14/2005 7:44:09 PM}			}
// COMMENT: {7/14/2005 7:44:09 PM}		}
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

		// MEDIA
		//
		CGridElt gridElt = CGridElt::Full();
		::GetDefaultMedia(&gridElt);

		// HEAD_IC
		//
		CHeadIC headIC = CHeadIC::Full();
		::GetDefaultHeadIC(&headIC);

		// CHEMISTRY_IC
		//
		CChemIC chemIC = CChemIC::Full();
		::GetDefaultChemIC(&chemIC);

		// PRINT_FREQUENCY
		//
		printFreq.SyncWithSrcInput();
#ifdef _DEBUG
		std::ostringstream oss;
		oss << printFreq;
		TRACE("%s\n", oss.str().c_str());
#endif

// COMMENT: {2/23/2005 1:07:32 PM}		// TIME_CONTROL
// COMMENT: {2/23/2005 1:07:32 PM}		//
// COMMENT: {2/23/2005 1:07:32 PM}		CTimeControl timeControl(time_step, time_end);
// COMMENT: {2/23/2005 1:07:32 PM}		Ctime timeBegin(time_end);
// COMMENT: {2/23/2005 1:07:32 PM}		timeBegin.SetValue(0.0);
		// TIME_CONTROL
		//
		CTimeControl2 timeControl2(::time_step, ::time_end, ::count_time_end);


		// create new document
		//
		CNewModel model;
		model.m_flowOnly       = flowOnly;
		model.m_freeSurface    = (::free_surface != 0);
		model.m_steadyFlow     = (::steady_flow != 0);  // TODO later there'll be additional member vars
		model.m_units          = ::units;
		//model.m_grid[0]        = ::grid[0];
		//model.m_grid[1]        = ::grid[1];
		//model.m_grid[2]        = ::grid[2];
		//model.m_snap           = ::snap;
		//model.m_print_input_xy = (::print_input_xy != 0);
		//model.m_axes[0]        = ::axes[0];
		//model.m_axes[1]        = ::axes[1];
		//model.m_axes[2]        = ::axes[2];
		model.m_gridKeyword    = CGridKeyword(::grid, ::snap, ::axes, ::print_input_xy);
		model.m_media          = gridElt;
		model.m_headIC         = headIC;
		model.m_chemIC         = chemIC;
		model.m_printFreq      = printFreq;
		model.m_timeControl2   = timeControl2;

		this->New(model);

		// MEDIA zones
		//
		for (int i = 0; i < ::count_grid_elt_zones; ++i)
		{
			// not undoable
			const struct grid_elt* grid_elt_ptr = ::grid_elt_zones[i];

			if (i == 0)
			{
				// if the first zone is equivalent to the default gridElt
				// don't add it

				// TODO: check for equivalance to default gridElt
			}

			CZoneCreateAction<CMediaZoneActor>* pAction = new CZoneCreateAction<CMediaZoneActor>(
				this,
				this->GetNextZoneName(),
				grid_elt_ptr->zone->x1,
				grid_elt_ptr->zone->x2,
				grid_elt_ptr->zone->y1,
				grid_elt_ptr->zone->y2,
				grid_elt_ptr->zone->z1,
				grid_elt_ptr->zone->z2
				);
			pAction->GetZoneActor()->SetGridElt(*grid_elt_ptr);
			pAction->Execute();
			delete pAction;
		}

		// BC Zones
		//
		std::list<CBCZoneActor*> listBCZoneActors;
		for (int i = 0; i < ::count_bc; ++i)
		{
			const struct bc* bc_ptr = ::bc[i];

			// not undoable
			//
			CZoneCreateAction<CBCZoneActor>* pAction = new CZoneCreateAction<CBCZoneActor>(
				this,
				this->GetNextZoneName(),
				bc_ptr->zone->x1,
				bc_ptr->zone->x2,
				bc_ptr->zone->y1,
				bc_ptr->zone->y2,
				bc_ptr->zone->z1,
				bc_ptr->zone->z2
				);
			pAction->GetZoneActor()->SetBC(*bc_ptr);
			pAction->Execute();
			listBCZoneActors.push_back(pAction->GetZoneActor());
			delete pAction;
		}

		// Wells
		//
		for (int i = 0; i < ::count_wells; ++i)
		{
			const Well* well_ptr = &::wells[i];			
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
			CRiverCreateAction *pAction = new CRiverCreateAction(this, river);
			pAction->Execute();
			delete pAction;
		}

		// IC
		//
		for (int i = 0; i < ::count_head_ic; ++i)
		{
			const struct head_ic* head_ic_ptr = ::head_ic[i];

			if (i == 0)
			{
				// if the first zone is equivalent to the default headIC
				// don't add it

				// TODO: check for equivalance to default headIC
			}

			CZoneCreateAction<CICZoneActor>* pAction = new CZoneCreateAction<CICZoneActor>(
				this,
				this->GetNextZoneName(),
				head_ic_ptr->zone->x1,
				head_ic_ptr->zone->x2,
				head_ic_ptr->zone->y1,
				head_ic_ptr->zone->y2,
				head_ic_ptr->zone->z1,
				head_ic_ptr->zone->z2
				);
			pAction->GetZoneActor()->SetHeadIC(*head_ic_ptr);
			pAction->Execute();
			delete pAction;
		}

		// CHEMISTRY_IC
		//
		for (int i = 0; i < ::count_chem_ic; ++i)
		{
			const struct chem_ic* chem_ic_ptr = ::chem_ic[i];

			if (i == 0)
			{
				// if the first zone is equivalent to the default hechemICadIC
				// don't add it

				// TODO: check for equivalance to default chemIC
			}

			CZoneCreateAction<CICZoneActor>* pAction = new CZoneCreateAction<CICZoneActor>(
				this,
				this->GetNextZoneName(),
				chem_ic_ptr->zone->x1,
				chem_ic_ptr->zone->x2,
				chem_ic_ptr->zone->y1,
				chem_ic_ptr->zone->y2,
				chem_ic_ptr->zone->z1,
				chem_ic_ptr->zone->z2
				);
			pAction->GetZoneActor()->SetChemIC(*chem_ic_ptr);
			pAction->Execute();
			delete pAction;
		}

		// load the rest of the stress periods
		//
		
		int last_count_bc = ::count_bc;
		int nStressPeriod = 2;
// COMMENT: {2/23/2005 1:08:05 PM}		timeBegin = time_end;
// COMMENT: {3/31/2005 6:23:23 PM}		while(pInput->LoadNextStressPeriod())
// COMMENT: {3/31/2005 6:23:23 PM}		{
// COMMENT: {3/31/2005 6:23:23 PM}			if (pInput->GetErrorCount() != 0)
// COMMENT: {3/31/2005 6:23:23 PM}			{
// COMMENT: {3/31/2005 6:23:23 PM}				throw pInput->GetErrorCount();
// COMMENT: {3/31/2005 6:23:23 PM}			}
// COMMENT: {3/31/2005 6:23:23 PM}// COMMENT: {2/23/2005 1:09:36 PM}			CTreeCtrlNode spNode = this->AddStressPeriod(CTimeControl(::time_step, ::time_end));
// COMMENT: {3/31/2005 6:23:23 PM}
// COMMENT: {3/31/2005 6:23:23 PM}			// Wells
// COMMENT: {3/31/2005 6:23:23 PM}			//
// COMMENT: {3/31/2005 6:23:23 PM}			CTreeCtrlNode nodeWells = this->GetPropertyTreeControlBar()->GetWellsNode();
// COMMENT: {3/31/2005 6:23:23 PM}			ASSERT(nodeWells.GetChildCount() == ::count_wells);
// COMMENT: {3/31/2005 6:23:23 PM}			for (int i = 0; i < count_wells; ++i)
// COMMENT: {3/31/2005 6:23:23 PM}			{
// COMMENT: {3/31/2005 6:23:23 PM}				const Well* well_ptr = &::wells[i];			
// COMMENT: {3/31/2005 6:23:23 PM}				if (CWellActor *pWellActor = CWellActor::SafeDownCast((vtkObject*)nodeWells.GetChildAt(i).GetData()))
// COMMENT: {3/31/2005 6:23:23 PM}				{
// COMMENT: {3/31/2005 6:23:23 PM}// COMMENT: {2/23/2005 1:09:25 PM}					ASSERT(well_ptr->n_user == pWellActor->GetWell().n_user);
// COMMENT: {3/31/2005 6:23:23 PM}// COMMENT: {2/23/2005 1:09:25 PM}					CWellRate rate(*well_ptr);
// COMMENT: {3/31/2005 6:23:23 PM}// COMMENT: {2/23/2005 1:09:25 PM}					pWellActor->Insert(timeBegin, rate);
// COMMENT: {3/31/2005 6:23:23 PM}				}
// COMMENT: {3/31/2005 6:23:23 PM}				else ASSERT(FALSE);
// COMMENT: {3/31/2005 6:23:23 PM}			}
// COMMENT: {3/31/2005 6:23:23 PM}
// COMMENT: {3/31/2005 6:23:23 PM}			// search for matching bcs
// COMMENT: {3/31/2005 6:23:23 PM}			// for (int nbc = count_bc - 1; nbc >= last_count_bc; --nbc) {
// COMMENT: {3/31/2005 6:23:23 PM}			for (int nbc = last_count_bc; nbc < ::count_bc; ++nbc)
// COMMENT: {3/31/2005 6:23:23 PM}			{
// COMMENT: {3/31/2005 6:23:23 PM}				const struct bc* bc_ptr = ::bc[nbc];
// COMMENT: {3/31/2005 6:23:23 PM}				std::list<CBCZoneActor*>::reverse_iterator rIterBC = listBCZoneActors.rbegin();
// COMMENT: {3/31/2005 6:23:23 PM}				for (; rIterBC != listBCZoneActors.rend(); ++rIterBC)
// COMMENT: {3/31/2005 6:23:23 PM}				{
// COMMENT: {3/31/2005 6:23:23 PM}					CZone zone(*(*rIterBC)->GetBC().zone);
// COMMENT: {3/31/2005 6:23:23 PM}					if (zone == *(bc_ptr->zone))
// COMMENT: {3/31/2005 6:23:23 PM}					{
// COMMENT: {3/31/2005 6:23:23 PM}						CBC bc((*rIterBC)->GetBC());
// COMMENT: {3/31/2005 6:23:23 PM}						if (bc.bc_type == bc_ptr->bc_type)
// COMMENT: {3/31/2005 6:23:23 PM}						{
// COMMENT: {3/31/2005 6:23:23 PM}							ASSERT((*rIterBC)->GetStressPeriodCount() == nStressPeriod);
// COMMENT: {3/31/2005 6:23:23 PM}							(*rIterBC)->SetBC(*bc_ptr, nStressPeriod);
// COMMENT: {3/31/2005 6:23:23 PM}							break;
// COMMENT: {3/31/2005 6:23:23 PM}						}
// COMMENT: {3/31/2005 6:23:23 PM}					}
// COMMENT: {3/31/2005 6:23:23 PM}				}
// COMMENT: {3/31/2005 6:23:23 PM}			}
// COMMENT: {3/31/2005 6:23:23 PM}
// COMMENT: {3/31/2005 6:23:23 PM}			// TODO PRINT_FREQUENCY
// COMMENT: {3/31/2005 6:23:23 PM}
// COMMENT: {3/31/2005 6:23:23 PM}			if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
// COMMENT: {3/31/2005 6:23:23 PM}			{
// COMMENT: {3/31/2005 6:23:23 PM}				// update stress period
// COMMENT: {3/31/2005 6:23:23 PM}				std::list<CBCZoneActor*>::iterator iterBC = listBCZoneActors.begin();
// COMMENT: {3/31/2005 6:23:23 PM}				for (; iterBC != listBCZoneActors.end(); ++iterBC)
// COMMENT: {3/31/2005 6:23:23 PM}				{
// COMMENT: {3/31/2005 6:23:23 PM}					(*iterBC)->UpdateTree(pTree, nStressPeriod);
// COMMENT: {3/31/2005 6:23:23 PM}				}
// COMMENT: {3/31/2005 6:23:23 PM}			}
// COMMENT: {3/31/2005 6:23:23 PM}
// COMMENT: {3/31/2005 6:23:23 PM}			++nStressPeriod;
// COMMENT: {3/31/2005 6:23:23 PM}			last_count_bc = ::count_bc;
// COMMENT: {3/31/2005 6:23:23 PM}// COMMENT: {2/23/2005 1:09:44 PM}			timeBegin = time_end;
// COMMENT: {3/31/2005 6:23:23 PM}		}
	}
	catch (int)
	{
		CImportErrorDialog dlg;
		dlg.m_lpszErrorMessages = pInput->GetErrorMsg();
		dlg.DoModal();
		pInput->Delete();
		return FALSE;
	}
	catch (const char * error)
	{
		::AfxMessageBox(error, MB_OK|MB_ICONEXCLAMATION);
		pInput->Delete();
		return FALSE;
	}
	catch (...)
	{
		::AfxMessageBox("An unknown error occured during import", MB_OK|MB_ICONEXCLAMATION);
		pInput->Delete();
		return FALSE;
	}

	pInput->Delete();
	this->SetTitle(strPrefix);
	this->SetModifiedFlag(TRUE);
	return TRUE;

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
	newName = this->GetTitle() + ".trans.dat";
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
	if (strPath.Find(_T(".trans.dat")) == -1) {
		strPath += ".trans.dat";
	}

	if (!this->DoExport(strPath)) {
		::AfxMessageBox("An error occured during the export", MB_OK);
		this->SetModifiedFlag(FALSE);
	}

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

	CString path = this->GetPathName();
	if (path.IsEmpty())
	{
		ofs << "# Exported from WPhast(Unsaved File)\n";
	}
	else
	{
		ofs << "# Exported from WPhast(" << (LPCTSTR)path <<  ")\n";
	}

	return this->WriteTransDat(ofs);
}

BOOL CWPhastDoc::WriteTransDat(std::ostream& os)
{
	int nCount;

// COMMENT: {4/11/2005 1:53:16 PM}#ifndef _DEBUG
	// FLOW_ONLY
	os << this->m_pModel->m_flowOnly;

	// STEADY_FLOW
	os << this->m_pModel->m_steadyFlow;

	// FREE_SURFACE
	os << this->m_pModel->m_freeSurface;

	// UNITS
	os << this->GetUnits();

	CGrid xyz[3];
	this->m_pGridLODActor->GetGrid(xyz[0], xyz[1], xyz[2]);

	// GRID
	os << "GRID\n";
	for (int i = 0; i < 3; ++i)
	{
		os << xyz[i];
	}

	// MEDIA
	CTreeCtrlNode nodeMedia = this->GetPropertyTreeControlBar()->GetMediaNode();
	nCount = nodeMedia.GetChildCount();
	if (nCount > 0) {
		os << "MEDIA\n";
	}
	for (int i = 0; i < nCount; ++i) {
		if (CMediaZoneActor *pZone = CMediaZoneActor::SafeDownCast((vtkObject*)nodeMedia.GetChildAt(i).GetData())) {
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
	CTreeCtrlNode nodeIC = this->GetPropertyTreeControlBar()->GetICNode();
	nCount = nodeIC.GetChildCount();
	for (int i = 0; i < nCount; ++i) {
		if (CICZoneActor *pZone = CICZoneActor::SafeDownCast((vtkObject*)nodeIC.GetChildAt(i).GetData())) {
			// TODO would be better to have CBCZoneActor write itself
			switch (pZone->GetType())
			{
			case CICZoneActor::IC_HEAD:
				os << pZone->GetHeadIC();
				break;
			case CICZoneActor::IC_CHEM:
				os << pZone->GetChemIC();
				break;
			default:
				ASSERT(FALSE);
			}
		}
	}
// COMMENT: {4/11/2005 1:53:20 PM}#endif

	std::multimap<Ctime, ISerial*> series;

// COMMENT: {4/19/2005 2:09:54 PM}	// Wells
// COMMENT: {4/19/2005 2:09:54 PM}	CTreeCtrlNode nodeWells = this->GetPropertyTreeControlBar()->GetWellsNode();
// COMMENT: {4/19/2005 2:09:54 PM}	nCount = nodeWells.GetChildCount();
// COMMENT: {4/19/2005 2:09:54 PM}	for (int i = 0; i < nCount; ++i)
// COMMENT: {4/19/2005 2:09:54 PM}	{
// COMMENT: {4/19/2005 2:09:54 PM}		if (CWellActor *pWellActor = CWellActor::SafeDownCast((vtkObject*)nodeWells.GetChildAt(i).GetData()))
// COMMENT: {4/19/2005 2:09:54 PM}		{
// COMMENT: {4/19/2005 2:09:54 PM}			std::map<Ctime, CWellRate> map = pWellActor->GetWell().GetMap();
// COMMENT: {4/19/2005 2:09:54 PM}			ASSERT(map.size() > 0);
// COMMENT: {4/19/2005 2:09:54 PM}			if (map.size() > 0)
// COMMENT: {4/19/2005 2:09:54 PM}			{
// COMMENT: {4/19/2005 2:09:54 PM}				std::map<Ctime, CWellRate>::const_iterator iter = map.begin();
// COMMENT: {4/19/2005 2:09:54 PM}				for (; iter != map.end(); ++iter)
// COMMENT: {4/19/2005 2:09:54 PM}				{
// COMMENT: {4/19/2005 2:09:54 PM}					series.insert(std::multimap<Ctime, ISerial*>::value_type((*iter).first, pWellActor));
// COMMENT: {4/19/2005 2:09:54 PM}				}
// COMMENT: {4/19/2005 2:09:54 PM}			}
// COMMENT: {4/19/2005 2:09:54 PM}		}
// COMMENT: {4/19/2005 2:09:54 PM}	}
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

	os << this->GetPrintFrequency();

	os << this->GetTimeControl2();


// COMMENT: {4/8/2005 6:57:04 PM}	// Additional stress periods
// COMMENT: {4/8/2005 6:57:04 PM}	int nStressPeriods = this->GetPropertyTreeControlBar()->GetStressPeriodCount();
// COMMENT: {4/8/2005 6:57:04 PM}	int nSimulation = 2;
// COMMENT: {4/8/2005 6:57:04 PM}	{
// COMMENT: {4/8/2005 6:57:04 PM}		int i = 1;
// COMMENT: {4/8/2005 6:57:04 PM}		std::multimap<Ctime, ISerial*>::iterator seriesIter = series.begin();
// COMMENT: {4/8/2005 6:57:04 PM}		CTimeControl* pTimeControl = 0;
// COMMENT: {4/8/2005 6:57:04 PM}		while (i <= nStressPeriods)
// COMMENT: {4/8/2005 6:57:04 PM}		{
// COMMENT: {4/8/2005 6:57:04 PM}			// TIME_CONTROL 
// COMMENT: {4/8/2005 6:57:04 PM}			CTreeCtrlNode nodeTC = this->GetPropertyTreeControlBar()->GetTimeControlNode(i);
// COMMENT: {4/8/2005 6:57:04 PM}			ASSERT(nodeTC.GetData());
// COMMENT: {4/8/2005 6:57:04 PM}			pTimeControl = (CTimeControl*)(nodeTC.GetData());
// COMMENT: {4/8/2005 6:57:04 PM}			ASSERT(pTimeControl);
// COMMENT: {4/8/2005 6:57:04 PM}
// COMMENT: {4/8/2005 6:57:04 PM}			if (((*seriesIter).first < pTimeControl->GetTimeEnd()))
// COMMENT: {4/8/2005 6:57:04 PM}			{
// COMMENT: {4/8/2005 6:57:04 PM}				Ctime timeToOutput((*seriesIter).first);
// COMMENT: {4/8/2005 6:57:04 PM}				for (; seriesIter != series.end(); ++seriesIter)
// COMMENT: {4/8/2005 6:57:04 PM}				{
// COMMENT: {4/8/2005 6:57:04 PM}					if (timeToOutput < (*seriesIter).first)
// COMMENT: {4/8/2005 6:57:04 PM}					{
// COMMENT: {4/8/2005 6:57:04 PM}						if ((*seriesIter).first < pTimeControl->GetTimeEnd())
// COMMENT: {4/8/2005 6:57:04 PM}						{
// COMMENT: {4/8/2005 6:57:04 PM}							CTimeControl tc(pTimeControl->GetTimeStep(), (*seriesIter).first);
// COMMENT: {4/8/2005 6:57:04 PM}							os << tc;
// COMMENT: {4/8/2005 6:57:04 PM}							os << "END\n";
// COMMENT: {4/8/2005 6:57:04 PM}							os.flush();
// COMMENT: {4/8/2005 6:57:04 PM}						}
// COMMENT: {4/8/2005 6:57:04 PM}						else
// COMMENT: {4/8/2005 6:57:04 PM}						{
// COMMENT: {4/8/2005 6:57:04 PM}							os << (*pTimeControl);
// COMMENT: {4/8/2005 6:57:04 PM}							os << "END\n";
// COMMENT: {4/8/2005 6:57:04 PM}							os.flush();
// COMMENT: {4/8/2005 6:57:04 PM}							++i;
// COMMENT: {4/8/2005 6:57:04 PM}						}
// COMMENT: {4/8/2005 6:57:04 PM}						break;
// COMMENT: {4/8/2005 6:57:04 PM}					}
// COMMENT: {4/8/2005 6:57:04 PM}#ifdef _DEBUG
// COMMENT: {4/8/2005 6:57:04 PM}					os << "#### for " << (*seriesIter).first.value << " " << (*seriesIter).first.input << "\n";
// COMMENT: {4/8/2005 6:57:04 PM}#endif
// COMMENT: {4/8/2005 6:57:04 PM}					(*seriesIter).second->Output(os, (*seriesIter).first);
// COMMENT: {4/8/2005 6:57:04 PM}					os.flush();
// COMMENT: {4/8/2005 6:57:04 PM}				}
// COMMENT: {4/8/2005 6:57:04 PM}			}
// COMMENT: {4/8/2005 6:57:04 PM}			else
// COMMENT: {4/8/2005 6:57:04 PM}			{
// COMMENT: {4/8/2005 6:57:04 PM}				// TIME_CONTROL 
// COMMENT: {4/8/2005 6:57:04 PM}				if (pTimeControl) {
// COMMENT: {4/8/2005 6:57:04 PM}					os << (*pTimeControl);
// COMMENT: {4/8/2005 6:57:04 PM}				}
// COMMENT: {4/8/2005 6:57:04 PM}
// COMMENT: {4/8/2005 6:57:04 PM}				// END
// COMMENT: {4/8/2005 6:57:04 PM}				os << "END\n";
// COMMENT: {4/8/2005 6:57:04 PM}				++i;
// COMMENT: {4/8/2005 6:57:04 PM}			}
// COMMENT: {4/8/2005 6:57:04 PM}		}
// COMMENT: {4/8/2005 6:57:04 PM}
// COMMENT: {4/8/2005 6:57:04 PM}		// Warn about any remaining time series beyond
// COMMENT: {4/8/2005 6:57:04 PM}		// the final time control
// COMMENT: {4/8/2005 6:57:04 PM}		//
// COMMENT: {4/8/2005 6:57:04 PM}		if (seriesIter != series.end())
// COMMENT: {4/8/2005 6:57:04 PM}		{
// COMMENT: {4/8/2005 6:57:04 PM}			// create set of unique ISerial's in order to 
// COMMENT: {4/8/2005 6:57:04 PM}			// only display warning once from each ISerial
// COMMENT: {4/8/2005 6:57:04 PM}			std::set<ISerial*> setSerial;
// COMMENT: {4/8/2005 6:57:04 PM}			for (; seriesIter != series.end(); ++seriesIter)
// COMMENT: {4/8/2005 6:57:04 PM}			{
// COMMENT: {4/8/2005 6:57:04 PM}				setSerial.insert((*seriesIter).second);
// COMMENT: {4/8/2005 6:57:04 PM}			}
// COMMENT: {4/8/2005 6:57:04 PM}			std::set<ISerial*>::iterator setIter = setSerial.begin();
// COMMENT: {4/8/2005 6:57:04 PM}			for (; setIter != setSerial.end(); ++setIter)
// COMMENT: {4/8/2005 6:57:04 PM}			{
// COMMENT: {4/8/2005 6:57:04 PM}				CString warning = (*setIter)->GetWarning(pTimeControl);
// COMMENT: {4/8/2005 6:57:04 PM}				::AfxMessageBox(warning, MB_OK);
// COMMENT: {4/8/2005 6:57:04 PM}			}
// COMMENT: {4/8/2005 6:57:04 PM}		}
// COMMENT: {4/8/2005 6:57:04 PM}	}

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

void CWPhastDoc::SetUnits(const CUnits& units)
{
	ASSERT(this->m_pUnits);
	(*this->m_pUnits) = units;

	// minimize length units for display purposes
	//
	if (this->m_pUnits->horizontal.defined == TRUE)
	{
		CString strLength(this->m_pUnits->horizontal.input);
		CGlobal::MinimizeLengthUnits(strLength);
		this->m_pUnits->horizontal.set_input(strLength);
	}
	if (this->m_pUnits->vertical.defined == TRUE)
	{
		CString strLength(this->m_pUnits->vertical.input);
		CGlobal::MinimizeLengthUnits(strLength);
		this->m_pUnits->horizontal.set_input(strLength);
	}

	// update properties bar
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetUnits(this->m_pUnits);
	}

	// update grid
	//
	CGrid x, y, z;
	this->m_pGridLODActor->GetGrid(x, y, z);
	this->ResizeGrid(x, y, z);

	// update zones
	//
	if (vtkPropCollection* pCollection = this->GetPropCollection())
	{
		pCollection->InitTraversal();
		for (int i = 0; i < pCollection->GetNumberOfItems(); ++i)
		{
			vtkProp* prop = pCollection->GetNextProp();
			if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop))
			{
				ASSERT(FALSE); // TODO 8/24/2004 Does this need to change (similar to SetScale)
				pZone->SetUnits(units);
			}
			if (CWellActor *pWell = CWellActor::SafeDownCast(prop))
			{
				ASSERT(FALSE);
			}
		}
	}

	//{{{{
	// set scale for all zones
	//
	if (vtkPropCollection* pCollection = this->GetPropCollection())
	{
		//{{
		vtkFloatingPointType *scale = this->GetScale();
		vtkFloatingPointType *bounds = this->GetGridBounds();
		vtkFloatingPointType defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
		//}}
		pCollection->InitTraversal();
		for (int i = 0; i < pCollection->GetNumberOfItems(); ++i)
		{
			vtkProp* prop = pCollection->GetNextProp();
			if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop))
			{
				ASSERT(FALSE);
				pZone->SetUnits(units);
			}
			if (vtkAssembly *pAssembly = vtkAssembly::SafeDownCast(prop))
			{
				ASSERT(FALSE); // no longer using vtkAssembly ???
			}
			if (vtkPropAssembly *pPropAssembly = vtkPropAssembly::SafeDownCast(prop))
			{
				if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
				{
					vtkProp *pProp;
					pPropCollection->InitTraversal();
					for (; (pProp = pPropCollection->GetNextProp()); )
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
								pWellActor->SetDefaultTubeDiameter(defaultAxesSize * 0.17 / sqrt(scale[0] * scale[1]));
								// pWellActor->SetRadius(defaultAxesSize * 0.085);
							}
							if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(prop3D))
							{
								pRiverActor->SetUnits(units);
								pRiverActor->SetRadius(defaultAxesSize * 0.085);
							}
						}
					}
				}
			}
		}
	}
	//}}}}

// COMMENT: {6/7/2005 6:11:55 PM}	// Update wells
// COMMENT: {6/7/2005 6:11:55 PM}	if (vtkPropCollection *pPropCollection = this->GetPropAssemblyWells()->GetParts())
// COMMENT: {6/7/2005 6:11:55 PM}	{
// COMMENT: {6/7/2005 6:11:55 PM}		// determine well diameter for displaying
// COMMENT: {6/7/2005 6:11:55 PM}		//
// COMMENT: {6/7/2005 6:11:55 PM}		float *scale = this->GetScale();
// COMMENT: {6/7/2005 6:11:55 PM}		float *bounds = this->GetGridBounds();
// COMMENT: {6/7/2005 6:11:55 PM}		float defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
// COMMENT: {6/7/2005 6:11:55 PM}		float defaultTubeDiameter = defaultAxesSize * 0.17 / sqrt(scale[0] * scale[1]);
// COMMENT: {6/7/2005 6:11:55 PM}
// COMMENT: {6/7/2005 6:11:55 PM}		vtkProp *pProp = 0;
// COMMENT: {6/7/2005 6:11:55 PM}		pPropCollection->InitTraversal();
// COMMENT: {6/7/2005 6:11:55 PM}		for (; (pProp = pPropCollection->GetNextProp()); )
// COMMENT: {6/7/2005 6:11:55 PM}		{
// COMMENT: {6/7/2005 6:11:55 PM}			if (CWellActor *pWellActor = CWellActor::SafeDownCast(pProp))
// COMMENT: {6/7/2005 6:11:55 PM}			{
// COMMENT: {6/7/2005 6:11:55 PM}				pWellActor->SetUnits(units);
// COMMENT: {6/7/2005 6:11:55 PM}				pWellActor->SetDefaultTubeDiameter(defaultTubeDiameter);
// COMMENT: {6/7/2005 6:11:55 PM}			}
// COMMENT: {6/7/2005 6:11:55 PM}		}
// COMMENT: {6/7/2005 6:11:55 PM}	}
// COMMENT: {6/7/2005 6:11:55 PM}
// COMMENT: {6/7/2005 6:11:55 PM}	// Update rivers
// COMMENT: {6/7/2005 6:11:55 PM}	if (vtkPropCollection *pPropCollection = this->GetPropAssemblyRivers()->GetParts())
// COMMENT: {6/7/2005 6:11:55 PM}	{
// COMMENT: {6/7/2005 6:11:55 PM}		// determine well diameter for displaying
// COMMENT: {6/7/2005 6:11:55 PM}		//
// COMMENT: {6/7/2005 6:11:55 PM}		float *bounds = this->GetGridBounds();
// COMMENT: {6/7/2005 6:11:55 PM}		float defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
// COMMENT: {6/7/2005 6:11:55 PM}
// COMMENT: {6/7/2005 6:11:55 PM}		vtkProp *pProp = 0;
// COMMENT: {6/7/2005 6:11:55 PM}		pPropCollection->InitTraversal();
// COMMENT: {6/7/2005 6:11:55 PM}		for (; (pProp = pPropCollection->GetNextProp()); )
// COMMENT: {6/7/2005 6:11:55 PM}		{
// COMMENT: {6/7/2005 6:11:55 PM}			if (CRiverActor *pRiverActor = CRiverActor::SafeDownCast(pProp))
// COMMENT: {6/7/2005 6:11:55 PM}			{
// COMMENT: {6/7/2005 6:11:55 PM}				pRiverActor->SetUnits(units);
// COMMENT: {6/7/2005 6:11:55 PM}				pRiverActor->SetRadius(defaultAxesSize * 0.085);
// COMMENT: {6/7/2005 6:11:55 PM}			}
// COMMENT: {6/7/2005 6:11:55 PM}		}
// COMMENT: {6/7/2005 6:11:55 PM}	}



	// for all views
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL) {
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);

		// resize the selection bounding box
		//
		if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( pView->GetRenderWindowInteractor()->GetPicker() )) {
			if (vtkProp3D* prop = picker->GetProp3D()) {
				if (CZoneActor *pZone = CZoneActor::SafeDownCast(prop)) {
// COMMENT: {6/10/2004 4:14:49 PM}					pZone->Pick(pView->GetRenderer(), pView->GetRenderWindowInteractor());
					pZone->Select(pView);
				}
			}
		}

		// resize the Box Widget
		//
		if (pView->GetBoxWidget()->GetProp3D()) {
			pView->GetBoxWidget()->PlaceWidget();
		}
	}
	this->UpdateAllViews(0);


	// Update BoxPropertiesDialogBar
	//
	if (CBoxPropertiesDialogBar* pBar = this->GetBoxPropertiesDialogBar()) {
		POSITION pos = this->GetFirstViewPosition();
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		pBar->Set(pView, pBar->GetProp3D(), this->GetUnits());
	}

}

// void CWPhastDoc::New(const CFlowOnly& flowOnly, const CUnits& units, const CGrid& x, const CGrid& y, const CGrid& z, const CGridElt& media, const CHeadIC& headIC, const CTimeControl& timeControl)
void CWPhastDoc::New(const CNewModel& model)
{
	ASSERT(this->m_pGridLODActor);
	ASSERT(this->m_pAxesActor);
	ASSERT(this->m_pPropCollection);
	ASSERT(this->m_pimpl);
	ASSERT(this->m_pUnits);

	// set FlowOnly
	// set SteadyFlow
	// set FreeSurface
	// set PrintFreq
	this->SetModel(model);
	///this->SetFlowOnly(model.m_flowOnly);

	// set the units
	//
	ASSERT(this->m_pUnits);
	*this->m_pUnits = model.m_units;
	// update properties bar
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar()) {
		pTree->SetUnits(this->m_pUnits);
	}

	// set the grid
	//
// COMMENT: {7/18/2005 8:45:06 PM}	this->m_pGridLODActor->SetGrid(model.m_gridKeyword.m_grid[0], model.m_gridKeyword.m_grid[1], model.m_gridKeyword.m_grid[2], *this->m_pUnits);
// COMMENT: {7/18/2005 8:45:06 PM}	this->m_pGridLODActor->SetSnap(model.m_gridKeyword.m_snap);
	this->m_pGridLODActor->SetGridKeyword(model.m_gridKeyword, this->GetUnits());
	this->m_pGridLODActor->SetPickable(0);
	this->m_pPropCollection->AddItem(this->m_pGridLODActor);
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar()) {
		pTree->SetGridLODActor(this->m_pGridLODActor);
	}

	// set the axes
	//
	///float bounds[6];
	///this->m_pGridLODActor->GetBounds(bounds);
	vtkFloatingPointType *bounds = this->GetGridBounds();
	vtkFloatingPointType defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
	this->m_pAxesActor->SetDefaultPositions(bounds);
	this->m_pAxesActor->SetDefaultSize(defaultAxesSize);
	this->m_pAxesActor->SetDefaultTubeDiameter(defaultAxesSize * 0.1);
	this->m_pPropCollection->AddItem(this->m_pAxesActor);

	// create the default zones
	//
	CZone zone;
	this->m_pGridLODActor->GetDefaultZone(zone);

	// default media
	//
	CZoneCreateAction<CMediaZoneActor>* pMediaAction = new CZoneCreateAction<CMediaZoneActor>(
		this,
		"Default",
		zone.x1,
		zone.x2,
		zone.y1,
		zone.y2,
		zone.z1,
		zone.z2
		);
	pMediaAction->GetZoneActor()->SetGridElt(model.m_media);
	pMediaAction->GetZoneActor()->SetDefault(true);
	pMediaAction->Execute();
	delete pMediaAction;


	// default head_ic
	CZoneCreateAction<CICZoneActor>* pICHeadAction = new CZoneCreateAction<CICZoneActor>(
		this,
		"Default",
		zone.x1,
		zone.x2,
		zone.y1,
		zone.y2,
		zone.z1,
		zone.z2
		);
	pICHeadAction->GetZoneActor()->SetHeadIC(model.m_headIC);
	pICHeadAction->GetZoneActor()->SetDefault(true);
	pICHeadAction->Execute();
	delete pICHeadAction;

	// CHEMISTRY_IC (default)
	CZoneCreateAction<CICZoneActor>* pChemICAction = new CZoneCreateAction<CICZoneActor>(
		this,
		"Default",
		zone.x1,
		zone.x2,
		zone.y1,
		zone.y2,
		zone.z1,
		zone.z2
		);
	pChemICAction->GetZoneActor()->SetChemIC(model.m_chemIC);
	pChemICAction->GetZoneActor()->SetDefault(true);
	pChemICAction->Execute();
	delete pChemICAction;

	// PRINT_FREQUENCY
	//
	// update properties bar
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetPrintFrequency(&this->m_pModel->m_printFreq);
	}

	// update properties bar
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		pTree->SetTimeControl2(&this->m_pModel->m_timeControl2);
	}

	if (model.HasSiteMap())
	{
		ASSERT(this->m_pMapActor == NULL);
		this->m_pMapActor = CMapActor::New();   // Note pixel(0,0) is the same size as all other pixels
		this->m_pMapActor->SetSiteMap(model.GetSiteMap());
		this->m_pMapActor->SetPickable(0);
		this->GetPropCollection()->AddItem(this->m_pMapActor);
	}

	// refresh screen
	//
	this->ResetCamera();
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnFileRun()
{
	CWaitCursor wait;
	// this is required to get the prefix
	//
	if (this->GetPathName().IsEmpty()) {
		if (!this->DoSave(NULL)) {
			return;
		}
	}

	std::ostringstream oss;
	this->WriteTransDat(oss);

	std::string str = oss.str();
	std::istringstream iss(str);

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];

	ASSERT(!this->GetPathName().IsEmpty());
	::_tsplitpath(this->GetPathName(), szDrive, szDir, szFName, szExt);

	TCHAR szPhastTmp[_MAX_PATH];
	::_tmakepath(szPhastTmp, szDrive, szDir, "Phast", ".tmp");

	CString strPrefix(szFName);


	CPhastInput* pInput = CPhastInput::New(iss, strPrefix);
	if (!pInput) return;

	// Update StatusBar
	//
	if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
		CString status(_T("Writing Phast.tmp..."));
		pWnd->SetWindowText(status);
	}

	bool bWritePhastTmpOK = false;
	try
	{
		pInput->WritePhastTmp(szPhastTmp);
		if (pInput->GetErrorCount() != 0) {
			// goto ImportError;
			// throw "Input file contains errors.";
			throw pInput->GetErrorCount();
		}
		bWritePhastTmpOK = true;
	}
	catch (int) {
		CImportErrorDialog dlg;
		dlg.m_lpszErrorMessages = pInput->GetErrorMsg();
		dlg.DoModal();
	}
	catch (const char * error) {
		::AfxMessageBox(error, MB_OK|MB_ICONEXCLAMATION);
	}
	catch (...) {
		::AfxMessageBox("An unknown error occured during import", MB_OK|MB_ICONEXCLAMATION);
	}
	pInput->Delete();

	if (bWritePhastTmpOK) {
		wait.Restore();

		TCHAR szPhastTmpDir[_MAX_PATH];
		::_tmakepath(szPhastTmpDir, szDrive, szDir, "", "");

		// Update StatusBar
		//
		if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
			CString status(_T("Running..."));
			pWnd->SetWindowText(status);
		}

		CRunDialog run;
		run.SetWorkingDirectory(szPhastTmpDir);
		run.DoModal();
	}
}

// COMMENT: {4/8/2005 6:51:09 PM}CTreeCtrlNode CWPhastDoc::AddStressPeriod(const CTimeControl& timeControl)
// COMMENT: {4/8/2005 6:51:09 PM}{
// COMMENT: {4/8/2005 6:51:09 PM}    CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar();
// COMMENT: {4/8/2005 6:51:09 PM}	ASSERT(pTree);
// COMMENT: {4/8/2005 6:51:09 PM}	CTreeCtrlNode node = pTree->AddStressPeriod(timeControl);
// COMMENT: {4/8/2005 6:51:09 PM}	return node;
// COMMENT: {4/8/2005 6:51:09 PM}}

// COMMENT: {4/8/2005 6:51:14 PM}void CWPhastDoc::RemoveStressPeriod(int nStressPeriod)
// COMMENT: {4/8/2005 6:51:14 PM}{
// COMMENT: {4/8/2005 6:51:14 PM}    CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar();
// COMMENT: {4/8/2005 6:51:14 PM}	ASSERT(pTree);
// COMMENT: {4/8/2005 6:51:14 PM}	pTree->RemoveStressPeriod(nStressPeriod);
// COMMENT: {4/8/2005 6:51:14 PM}}

// COMMENT: {4/8/2005 6:51:20 PM}int CWPhastDoc::GetStressPeriodCount(void)const
// COMMENT: {4/8/2005 6:51:20 PM}{
// COMMENT: {4/8/2005 6:51:20 PM}    CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar();
// COMMENT: {4/8/2005 6:51:20 PM}	ASSERT(pTree);
// COMMENT: {4/8/2005 6:51:20 PM}	return pTree->GetStressPeriodCount();
// COMMENT: {4/8/2005 6:51:20 PM}}

// COMMENT: {4/11/2005 1:22:32 PM}void CWPhastDoc::SetTimeControl(const CTimeControl& timeControl, int nStressPeriod)
// COMMENT: {4/11/2005 1:22:32 PM}{
// COMMENT: {4/11/2005 1:22:32 PM}	if (nStressPeriod == 1) {
// COMMENT: {4/11/2005 1:22:32 PM}		(*this->m_pTimeControl) = timeControl;
// COMMENT: {4/11/2005 1:22:32 PM}
// COMMENT: {4/11/2005 1:22:32 PM}		// update properties bar
// COMMENT: {4/11/2005 1:22:32 PM}		//
// COMMENT: {4/11/2005 1:22:32 PM}		if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar()) {
// COMMENT: {4/11/2005 1:22:32 PM}			pTree->SetTimeControl(this->m_pTimeControl);
// COMMENT: {4/11/2005 1:22:32 PM}		}
// COMMENT: {4/11/2005 1:22:32 PM}	}
// COMMENT: {4/11/2005 1:22:32 PM}	else {
// COMMENT: {4/11/2005 1:22:32 PM}		CTimeControl copy(timeControl);
// COMMENT: {4/11/2005 1:22:32 PM}		if (copy.GetTimeEndInput() == 0) {
// COMMENT: {4/11/2005 1:22:32 PM}			copy.SetTimeEndInput(this->GetUnits().time.c_str());
// COMMENT: {4/11/2005 1:22:32 PM}		}
// COMMENT: {4/11/2005 1:22:32 PM}		if (copy.GetTimeStepInput() == 0) {
// COMMENT: {4/11/2005 1:22:32 PM}			copy.SetTimeStepInput(this->GetUnits().time.c_str());
// COMMENT: {4/11/2005 1:22:32 PM}		}
// COMMENT: {4/11/2005 1:22:32 PM}
// COMMENT: {4/11/2005 1:22:32 PM}		// update properties bar
// COMMENT: {4/11/2005 1:22:32 PM}		//
// COMMENT: {4/11/2005 1:22:32 PM}		if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar()) {
// COMMENT: {4/11/2005 1:22:32 PM}			pTree->SetTimeControl(copy, nStressPeriod);
// COMMENT: {4/11/2005 1:22:32 PM}		}
// COMMENT: {4/11/2005 1:22:32 PM}	}
// COMMENT: {4/11/2005 1:22:32 PM}}

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

void CWPhastDoc::OnToolsNewStressPeriod(void)
{
// COMMENT: {4/8/2005 6:57:20 PM}	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar()) {
// COMMENT: {4/8/2005 6:57:20 PM}		CPropertySheet props(_T("New Simulation Period"));
// COMMENT: {4/8/2005 6:57:20 PM}		CTimeControlPropertyPage tcPage;
// COMMENT: {4/8/2005 6:57:20 PM}		tcPage.SetPreviousTimeStep(*pTree->GetTimeControl(pTree->GetStressPeriodCount()));
// COMMENT: {4/8/2005 6:57:20 PM}		props.AddPage(&tcPage);
// COMMENT: {4/8/2005 6:57:20 PM}
// COMMENT: {4/8/2005 6:57:20 PM}		if (props.DoModal() == IDOK) {
// COMMENT: {4/8/2005 6:57:20 PM}			CTimeControl tc;
// COMMENT: {4/8/2005 6:57:20 PM}			tcPage.GetProperties(tc);
// COMMENT: {4/8/2005 6:57:20 PM}			this->Execute(new CAddStressPeriodAction(this, tc));
// COMMENT: {4/8/2005 6:57:20 PM}		}
// COMMENT: {4/8/2005 6:57:20 PM}	}
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
	if (this->m_pAxesActor->GetVisibility())
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
	if (this->m_pAxesActor->GetVisibility())
	{
		this->m_pAxesActor->SetVisibility(0);
	}
	else
	{
		this->m_pAxesActor->SetVisibility(1);
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnUpdateSetprojectiontoParallel(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(this->m_ProjectionMode == PT_PARALLEL);
}

void CWPhastDoc::OnSetprojectiontoParallel()
{
	this->m_ProjectionMode = PT_PARALLEL;
	// update views
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL) {
		CWPhastView *pView = (CWPhastView*) GetNextView(pos);
		ASSERT_VALID(pView);
		pView->ParallelProjectionOn();
	}
	UpdateAllViews(NULL);
}

void CWPhastDoc::OnUpdateSetprojectiontoPerspective(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(this->m_ProjectionMode == PT_PERSPECTIVE);
}

void CWPhastDoc::OnSetprojectiontoPerspective()
{
	this->m_ProjectionMode = PT_PERSPECTIVE;

	// update views
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL) {
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
	if (this->m_pGridLODActor->GetVisibility())
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

	if (this->m_pGridLODActor->GetVisibility())
	{
		this->m_pGridLODActor->SetVisibility(0);
		// if (pTree) pTree->SetNodeCheck(pTree->GetGridNode(), BST_UNCHECKED);
		if (pTree) pTree->GetGridNode().SetState(INDEXTOSTATEIMAGEMASK(BST_UNCHECKED + 1), TVIS_STATEIMAGEMASK);

	}
	else
	{
		this->m_pGridLODActor->SetVisibility(1);
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
		CTreeCtrlNode node = pTree->GetICNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_CHECKED);
		}
	}
	this->UpdateAllViews(0);
}

void CWPhastDoc::OnICZonesUnselectAll()
{
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar())
	{
		CTreeCtrlNode node = pTree->GetICNode();
		int nCount = node.GetChildCount();
		for (int i = 0; i < nCount; ++i)
		{
			pTree->SetNodeCheck(node.GetChildAt(i), BST_UNCHECKED);
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
	// foreach view
	//
	POSITION pos = this->GetFirstViewPosition();
	while (pos != NULL) {
		CView *pView = this->GetNextView(pos);
		if (CWPhastView *pWPhastView = static_cast<CWPhastView*>(pView)) {
			ASSERT_KINDOF(CWPhastView, pWPhastView);
			ASSERT_VALID(pWPhastView);
			pWPhastView->ClearSelection();
		}
	}

	// clear selected river points
	//
	vtkPropCollection *pPropCollection = this->GetPropAssemblyRivers()->GetParts();
	pPropCollection->InitTraversal();
	vtkProp* pProp = pPropCollection->GetNextProp();
	for (; pProp; pProp = pPropCollection->GetNextProp())
	{
		if (CRiverActor *pActor = CRiverActor::SafeDownCast(pProp))
		{
			pActor->ClearSelection();
			pActor->SetEnabled(0);
		}
	}
}

void CWPhastDoc::Add(CZoneActor *pZoneActor)
{
	this->InternalAdd(pZoneActor, true);
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

void CWPhastDoc::InternalAdd(CZoneActor *pZoneActor, bool bAdd)
{
	ASSERT(pZoneActor);
	if (pZoneActor == NULL) return;

	// set scale
	//
	vtkFloatingPointType *scale = this->GetScale();
	pZoneActor->SetScale(scale[0], scale[1], scale[2]);


	// add to document
	//
	pZoneActor->Add(this);

	if (!bAdd) {
		// see InternalDelete
		//
		ASSERT(this->m_pRemovedPropCollection);
		ASSERT(this->m_pRemovedPropCollection->IsItemPresent(pZoneActor));
		this->m_pRemovedPropCollection->RemoveItem(pZoneActor);
	}

	// add to property tree
	//
	if (CPropertyTreeControlBar *pPropertyTreeControlBar = this->GetPropertyTreeControlBar()) {
		if (bAdd) {
			pPropertyTreeControlBar->AddZone(pZoneActor);
		}
		else {
			pZoneActor->UnRemove(pPropertyTreeControlBar);
		}
	}

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

			// set selection bounding box
			//
			pZoneActor->Select(pWPhastView);

			// Update BoxPropertiesDialogBar
			//
			if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) ) {
				pBar->Set(pWPhastView, pZoneActor, this->GetUnits());
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

	if (bDelete) {
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
			if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) ) {
				pBar->Set(pWPhastView, NULL, this->GetUnits());
			}
		}
	}

	// remove from property tree
	//
	if (CPropertyTreeControlBar* pTree = this->GetPropertyTreeControlBar()) {
		pTree->RemoveZone(pZoneActor);
	}

	// render
	//
	this->UpdateAllViews(0);
}

void CWPhastDoc::Add(CWellActor *pWellActor)
{
	ASSERT(pWellActor);
	if (!pWellActor) return;
	ASSERT(pWellActor->GetPickable());

	// set scale
	//
	vtkFloatingPointType *scale = this->GetScale();
	pWellActor->SetScale(scale[0], scale[1], scale[2]);

	// set height
	//
	CGrid xyz[3];
	this->GetGrid(xyz[0], xyz[1], xyz[2]);
	pWellActor->SetZAxis(xyz[2], this->GetUnits());

	// set radius
	//
	vtkFloatingPointType *bounds = this->GetGridBounds();
	float defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
	pWellActor->SetDefaultTubeDiameter(defaultAxesSize * 0.17 / sqrt(scale[0] * scale[1]));


	// add to well assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyWells())
	{
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(pWellActor));
		pPropAssembly->AddPart(pWellActor);
		if (!this->GetPropCollection()->IsItemPresent(pPropAssembly))
		{
			this->GetPropCollection()->AddItem(pPropAssembly);
		}
	}

	// add to property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pWellActor->Add(pTree);
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
	vtkFloatingPointType *scale = this->GetScale();
	vtkFloatingPointType *wellscale = pWellActor->GetScale();
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
	vtkFloatingPointType *scale = this->GetScale();
	vtkFloatingPointType *wellscale = pWellActor->GetScale();
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

void CWPhastDoc::Select(CWellActor *pWellActor)
{	
	this->Notify(0, WPN_SELCHANGED, 0, pWellActor);
}

void CWPhastDoc::Select(CRiverActor *pRiverActor)
{
	this->Notify(0, WPN_SELCHANGED, 0, pRiverActor);
}

void CWPhastDoc::GetGrid(CGrid& x, CGrid& y, CGrid& z)const
{
	this->m_pGridLODActor->GetGrid(x, y, z);
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

	// TODO: Add your property handler code here
#if defined(_DEBUG)
	afxDump << (COleVariant)newVal;
	ASSERT(newVal.vt == VT_BOOL);
#endif

	if (newVal.boolVal)
	{
		::AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOW);
	}
	else
	{
		::AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);
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

	// TODO: Add your dispatch handler code here

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
		this->ResetCamera();
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

void CWPhastDoc::Add(CRiverActor *pRiverActor)
{
	ASSERT(pRiverActor);
	if (!pRiverActor) return;

	// set scale
	//
	vtkFloatingPointType *scale = this->GetScale();
	pRiverActor->SetScale(scale[0], scale[1], scale[2]);

	// set radius
	//
	pRiverActor->ScaleFromBounds(this->GetGridBounds());

	// set z
	//
// COMMENT: {6/22/2005 5:50:09 PM}	vtkFloatingPointType *bounds = this->GetGridBounds();
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
		pRiverActor->SetInteractor(pView->GetRenderWindowInteractor());
		pRiverActor->SetEnabled(0);
	}	

	// add to river assembly
	//
	if (vtkPropAssembly *pPropAssembly = this->GetPropAssemblyRivers())
	{
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(pRiverActor));
		pPropAssembly->AddPart(pRiverActor);
		if (!this->GetPropCollection()->IsItemPresent(pPropAssembly))
		{
			this->GetPropCollection()->AddItem(pPropAssembly);
		}
	}

	// add to property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pRiverActor->Add(pTree);
	}

	// add listeners
	//
	pRiverActor->AddObserver(CRiverActor::StartMovePointEvent, RiverCallbackCommand);
	pRiverActor->AddObserver(CRiverActor::MovingPointEvent, RiverCallbackCommand);
	pRiverActor->AddObserver(CRiverActor::EndMovePointEvent, RiverCallbackCommand);
	pRiverActor->AddObserver(CRiverActor::InsertPointEvent, RiverCallbackCommand);	

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
		// VTK HACK
		// This is req'd because ReleaseGraphicsResources is not called when
		// vtkPropAssembly::RemovePart(vtkProp *prop) is called
		this->ReleaseGraphicsResources(pRiverActor);
	}

	// remove from property tree
	//
	if (CPropertyTreeControlBar *pTree = this->GetPropertyTreeControlBar())
	{
		pRiverActor->UnAdd(pTree);
	}

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
	vtkFloatingPointType *scale = this->GetScale();
	vtkFloatingPointType *riverscale = pRiverActor->GetScale();
// COMMENT: {7/12/2005 3:42:06 PM}	ASSERT(riverscale[0] == scale[0]);
// COMMENT: {7/12/2005 3:42:06 PM}	ASSERT(riverscale[1] == scale[1]);
// COMMENT: {7/12/2005 3:42:06 PM}	ASSERT(riverscale[2] == scale[2]);

	// make sure pWellActor ref count doesn't go to zero
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
	vtkFloatingPointType *scale = this->GetScale();
	vtkFloatingPointType *riverscale = pRiverActor->GetScale();
// COMMENT: {7/12/2005 3:41:44 PM}	ASSERT(riverscale[0] == scale[0]);
// COMMENT: {7/12/2005 3:41:44 PM}	ASSERT(riverscale[1] == scale[1]);
// COMMENT: {7/12/2005 3:41:44 PM}	ASSERT(riverscale[2] == scale[2]);

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
			self->RiverMovePointAction = new CRiverMovePointAction(river, self, river->GetCurrentPointId(), river->GetCurrentPointPosition()[0], river->GetCurrentPointPosition()[1]);
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
					self->RiverMovePointAction->GetRiverActor()->GetCurrentPointPosition()[0] / units.horizontal.input_to_si,
					units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
					self->RiverMovePointAction->GetRiverActor()->GetCurrentPointPosition()[1] / units.horizontal.input_to_si,
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
				CRiverInsertPointAction* pRiverInsertPointAction = new CRiverInsertPointAction(river, id, pos[0], pos[1], pos[2], true);
				self->Execute(pRiverInsertPointAction);
			}
			break;
		}
	}
}

void CWPhastDoc::SetGridKeyword(const CGridKeyword& gridKeyword)
{
	this->m_pGridLODActor->SetGridKeyword(gridKeyword, this->GetUnits());
	this->ResizeGrid(gridKeyword.m_grid[0], gridKeyword.m_grid[1], gridKeyword.m_grid[2]);
}

void CWPhastDoc::Edit(CGridLODActor* pGridLODActor)
{
	if (!this->m_pGridSheet)
	{
		this->m_pGridSheet = new CModelessPropertySheet("Grid");
	}
	if (!this->m_pGridPage)
	{
		this->m_pGridPage  = new CGridPropertyPage2();
		this->m_pGridSheet->AddPage(this->m_pGridPage);
	}
	
	CGridKeyword gridKeyword;
	pGridLODActor->GetGridKeyword(gridKeyword);
	this->m_pGridPage->SetProperties(gridKeyword);
	this->m_pGridPage->SetUnits(this->GetUnits());

	//{{
	this->m_pGridPage->m_pDoc = this;
	this->m_pGridPage->m_pActor = this->m_pGridLODActor;
	//}}

	this->m_pGridSheet->Create(::AfxGetApp()->m_pMainWnd);
}

void CWPhastDoc::Update(IObserver* pSender, LPARAM lHint, CObject* pHint, vtkObject* pObject)
{
}

vtkActor* CWPhastDoc::GetGridActor(void)
{
	return this->m_pGridLODActor;
}
