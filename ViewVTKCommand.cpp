#include "StdAfx.h"

#define USE_ZMAX

#include "ViewVTKCommand.h"
#include "resource.h"

#include "WPhastDoc.h"
#include "WPhastView.h"
#include "BoxPropertiesDialogBar.h"
#include "ZoneActor.h"
#include "MediaZoneActor.h"
#include "BCZoneActor.h"
#include "ICZoneActor.h"

#include "WellActor.h"
#include "Well.h"
#include "WellPropertyPage.h"

#include "RiverActor.h"
#include "River.h"
//#include "RiverPropertyPage.h"

#include "ZoneResizeAction.h"
#include "NewZonePropertyPage.h"
#include "MediaPropertyPage.h"
#include "BCFluxPropertyPage.h"
#include "BCLeakyPropertyPage.h"
#include "BCLeakyPropertyPage2.h"
#include "BCSpecifiedPropertyPage.h"
#include "ICHeadPropertyPage.h"
#include "ChemICPropertyPage.h"

#include "WellCreateAction.h"
#include "WellSetPositionAction.h"

#include "Units.h"

#include <vtkWin32RenderWindowInteractor.h>

#include <vtkBoxWidget.h>
#include "vtkPointWidget2.h"


#include <vtkAbstractPropPicker.h>
#include <vtkProp3D.h>
#include <vtkAssembly.h>
#include <vtkPropAssembly.h>

#include <vtkTransform.h>
#include <vtkMapper.h>
#include <vtkDataSet.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkPolyDataMapper.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkCylinderSource.h>
#include "Global.h"

#include "MainFrm.h"

#include <afxmt.h>
static CCriticalSection  critSect;

CViewVTKCommand::CViewVTKCommand(CWPhastView* pView)
: m_pView(pView)
, m_pAction(0)
{
	ASSERT_VALID(pView);
}

CViewVTKCommand::~CViewVTKCommand()
{
}

CViewVTKCommand *CViewVTKCommand::New(CWPhastView *pView)
{
	return new CViewVTKCommand(pView);
}

void CViewVTKCommand::Execute(vtkObject *caller, unsigned long eventId, void *callData)
{
	switch ( eventId ) 
	{
		case vtkCommand::StartInteractionEvent:
			this->OnStartInteractionEvent(caller, callData);
			break;

		case vtkCommand::EndInteractionEvent:
			this->OnEndInteractionEvent(caller, callData);
			break;

		case vtkCommand::EndPickEvent:
			this->OnEndPickEvent(caller, callData);
			break;

		case vtkCommand::InteractionEvent:
			this->OnInteractionEvent(caller, callData);
			break;

		case vtkCommand::MouseMoveEvent:
			this->OnMouseMoveEvent(caller, callData);
			break;

		case vtkCommand::LeftButtonReleaseEvent:
			this->OnLeftButtonReleaseEvent(caller, callData);
			break;

		case vtkCommand::LeftButtonPressEvent:
			this->OnLeftButtonPressEvent(caller, callData);
			break;

		case vtkCommand::KeyPressEvent:
			this->OnKeyPressEvent(caller, callData);
			break;

		case vtkCommand::ModifiedEvent:
			this->OnModifiedEvent(caller, callData);
			break;

		default:
			break;
	}

}

void CViewVTKCommand::OnStartInteractionEvent(vtkObject* caller, void* callData)
{
	if (vtkPointWidget2 *widget = vtkPointWidget2::SafeDownCast(caller))
	{
		if (CWellActor *pWellActor = CWellActor::SafeDownCast(widget->GetProp3D()))
		{
			ASSERT(this->m_pAction == 0);
			this->m_pAction = 
				new CWellSetPositionAction(pWellActor, this->m_pView->GetDocument(), 0.0, 0.0);
		}
	}
}

void CViewVTKCommand::OnEndInteractionEvent(vtkObject* caller, void* callData)
{
	if (vtkBoxWidget *widget = vtkBoxWidget::SafeDownCast(caller)) {
		if (CZoneActor *pZone = CZoneActor::SafeDownCast(widget->GetProp3D())) {
			pZone->Resize(this->m_pView);
		}
	}

	if (vtkPointWidget2 *widget = vtkPointWidget2::SafeDownCast(caller))
	{
		if (CWellActor *pWellActor = CWellActor::SafeDownCast(widget->GetProp3D()))
		{
			// Units
			const CUnits& units = this->m_pView->GetDocument()->GetUnits();

			// Scale
			const vtkFloatingPointType* scale = this->m_pView->GetDocument()->GetScale();

			double x = widget->GetPosition()[0] / scale[0] / units.horizontal.input_to_si;
			double y = widget->GetPosition()[1] / scale[1] / units.horizontal.input_to_si;
			((CWellSetPositionAction*)this->m_pAction)->SetPosition(x, y);
			this->m_pView->GetDocument()->Execute(this->m_pAction);
			this->m_pAction = 0;
		}
	}
}

void CViewVTKCommand::OnEndPickEvent(vtkObject* caller, void* callData)
{
	TRACE("vtkCommand::EndPickEvent\n");
	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( this->m_pView->GetRenderWindowInteractor()->GetPicker() ))
	{
		if (vtkAssemblyPath *path = picker->GetPath())
		{
#if defined(_DEBUG)
			{
				ostrstream oss;
				path->PrintSelf(oss, 4);
				oss << ends;
				TRACE("\n");
				afxDump << "vtkAssemblyPath{{\n" << oss.str() << "vtkAssemblyPath}}\n";
				oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak

				ASSERT(path->GetFirstNode()->GetProp()->IsA("vtkPropAssembly"));

				if (path->GetNumberOfItems() == 3)
				{
					// should be river
					//
					path->InitTraversal();
					vtkProp* pPropAssembly = path->GetNextNode()->GetProp();
					ASSERT(pPropAssembly->IsA("vtkPropAssembly"));
					vtkProp* pRiverActor = path->GetNextNode()->GetProp();
					ASSERT(pRiverActor->IsA("CRiverActor"));
					if (CRiverActor *pRiver = CRiverActor::SafeDownCast(pRiverActor))
					{
						this->m_pView->GetDocument()->Select(pRiver);
					}
				}
				else
				{
					ASSERT(path->GetNumberOfItems() == 2); // the vtkPropAssembly and the CZoneActor or CWellActor

					// this will change as additional pickable types are added
					//
					ASSERT(
						path->GetLastNode()->GetProp()->IsA("CZoneActor")
						||
						path->GetLastNode()->GetProp()->IsA("CWellActor")
						);

					// check if zone
					//
					if (CZoneActor *pZone = CZoneActor::SafeDownCast(path->GetLastNode()->GetProp()))
					{
						pZone->Select(this->m_pView);
					}

					// check if well
					//
					if (CWellActor *pWell = CWellActor::SafeDownCast(path->GetLastNode()->GetProp()))
					{
						this->m_pView->GetDocument()->Select(pWell);
					}

				}
			}
#endif

#ifdef _DEBUG
			int* xy = this->m_pView->GetRenderWindowInteractor()->GetEventPosition();
			TRACE("EventPosition(%d, %d)\n", xy[0], xy[1]);
#endif
		}
// COMMENT: {7/12/2004 9:21:26 PM}		if (vtkProp *pProp = picker->GetProp())
// COMMENT: {7/12/2004 9:21:26 PM}		{
// COMMENT: {7/12/2004 9:21:26 PM}			if (vtkPropAssembly *pPropAssembly = vtkPropAssembly::SafeDownCast(pProp)) {
// COMMENT: {7/12/2004 9:21:26 PM}				if (vtkAssemblyPath *path = picker->GetPath()) {
// COMMENT: {7/12/2004 9:21:26 PM}					if (CZoneActor *pZone = CZoneActor::SafeDownCast(path->GetLastNode()->GetProp())) {
// COMMENT: {7/12/2004 9:21:26 PM}						pZone->Select(this->m_pView);
// COMMENT: {7/12/2004 9:21:26 PM}					}
// COMMENT: {7/12/2004 9:21:26 PM}				}
// COMMENT: {7/12/2004 9:21:26 PM}			}
// COMMENT: {7/12/2004 9:21:26 PM}		}
// COMMENT: {7/12/2004 9:21:26 PM}		if (vtkProp3D *pProp3D = picker->GetProp3D())
// COMMENT: {7/12/2004 9:21:26 PM}		{
// COMMENT: {7/12/2004 9:21:26 PM}			// check for assembly
// COMMENT: {7/12/2004 9:21:26 PM}			//
// COMMENT: {7/12/2004 9:21:26 PM}			if (vtkAssembly *pAssembly = vtkAssembly::SafeDownCast(pProp3D)) {
// COMMENT: {7/12/2004 9:21:26 PM}				ASSERT(FALSE);
// COMMENT: {7/12/2004 9:21:26 PM}				if (vtkAssemblyPath *path = picker->GetPath()) {
// COMMENT: {7/12/2004 9:21:26 PM}					pProp3D = (vtkProp3D *)path->GetLastNode()->GetProp();
// COMMENT: {7/12/2004 9:21:26 PM}				}
// COMMENT: {7/12/2004 9:21:26 PM}			}
// COMMENT: {7/12/2004 9:21:26 PM}
// COMMENT: {7/12/2004 9:21:26 PM}			// select zone
// COMMENT: {7/12/2004 9:21:26 PM}			//
// COMMENT: {7/12/2004 9:21:26 PM}			if (CZoneActor *pZone = CZoneActor::SafeDownCast(pProp3D)) {
// COMMENT: {7/12/2004 9:21:26 PM}				pZone->Select(this->m_pView);
// COMMENT: {7/12/2004 9:21:26 PM}			}
// COMMENT: {7/12/2004 9:21:26 PM}
// COMMENT: {7/12/2004 9:21:26 PM}#ifdef _DEBUG
// COMMENT: {7/12/2004 9:21:26 PM}			int* xy = this->m_pView->GetRenderWindowInteractor()->GetEventPosition();
// COMMENT: {7/12/2004 9:21:26 PM}			TRACE("EventPosition(%d, %d)\n", xy[0], xy[1]);
// COMMENT: {7/12/2004 9:21:26 PM}#endif
// COMMENT: {7/12/2004 9:21:26 PM}		}
		else
		{
			// Hide BoxWidget
			//
			this->m_pView->GetBoxWidget()->SetEnabled(0);
		}


		// render
		//
		this->m_pView->GetDocument()->UpdateAllViews(0);
	}
}

void CViewVTKCommand::OnInteractionEvent(vtkObject* caller, void* callData)
{
	if (vtkPointWidget2 *widget = vtkPointWidget2::SafeDownCast(caller))
	{
		if (CWellActor *pWellActor = CWellActor::SafeDownCast(widget->GetProp3D()))
		{
			// Units
			const CUnits& units = this->m_pView->GetDocument()->GetUnits();

			// Scale
			const vtkFloatingPointType* scale = pWellActor->GetScale();

			CWellSchedule well = pWellActor->GetWell();
			well.x = widget->GetPosition()[0] / scale[0] / units.horizontal.input_to_si;
			well.y = widget->GetPosition()[1] / scale[1] / units.horizontal.input_to_si;
			pWellActor->SetWell(well, units);

			///double x = widget->GetPosition()[0] / scale[0] / units.horizontal.input_to_si;
			///double y = widget->GetPosition()[1] / scale[1] / units.horizontal.input_to_si;
			///pWellActor->SetPosition(x, y);
		}
	}

	if (vtkBoxWidget *widget = vtkBoxWidget::SafeDownCast(caller))
	{
		// Apply transform
		//
		if (vtkActor *actor = vtkActor::SafeDownCast(widget->GetProp3D()))
		{
			vtkMapper *mapper = actor->GetMapper();
			if (vtkDataSet *dataset = mapper->GetInput())
			{
				if (vtkSource *source = dataset->GetSource())
				{
					if (vtkCubeSource* cube = vtkCubeSource::SafeDownCast(source))
					{
						vtkTransform *t = vtkTransform::New();
						widget->GetTransform(t);
#ifdef _DEBUG
						vtkFloatingPointType* pos = widget->GetProp3D()->GetPosition();
						ASSERT(pos[0] == 0.0);
						ASSERT(pos[1] == 0.0);
						ASSERT(pos[2] == 0.0);
#endif
						vtkFloatingPointType* scale = actor->GetScale();
						vtkFloatingPointType* center = cube->GetCenter();
						widget->GetProp3D()->SetPosition(center[0]*scale[0], center[1]*scale[1], center[2]*scale[2]);
						widget->GetTransform(t);
#if (0)
#ifdef _DEBUG
						{
							afxDump << "widget->GetTransform(t)\n";
							ostrstream oss;

							t->PrintSelf(oss, 4);

							oss << ends;
							afxDump << oss.str() << "\n";
							oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
						}
#endif
#endif
						widget->GetProp3D()->SetPosition(0, 0, 0);
						if (CGlobal::IsValidTransform(t)) {
							widget->GetProp3D()->SetUserTransform(t);
						}
						t->Delete();
					}
				}
			}
		}

		// Update StatusBar
		//
		if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
			vtkFloatingPointType bounds[6];
			widget->GetProp3D()->GetBounds(bounds);
			vtkFloatingPointType* scale = widget->GetProp3D()->GetScale();
			TCHAR buffer[80];
			const CUnits& units = this->m_pView->GetDocument()->GetUnits();
			::_sntprintf(buffer, 80, "%g[%s] x %g[%s] x %g[%s])",
				fabs(bounds[1] - bounds[0]) / scale[0] / units.horizontal.input_to_si,
				units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
				fabs(bounds[3] - bounds[2]) / scale[1] / units.horizontal.input_to_si,
				units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
				fabs(bounds[5] - bounds[4]) / scale[2] / units.vertical.input_to_si,
				units.vertical.defined ? units.vertical.input : units.vertical.si
				);
			pWnd->SetWindowText(buffer);
		}

		// Update BoxPropertiesDialogBar
		//
		if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) ) {
			// pBar->Set(this->m_pView, widget->GetProp3D());
			pBar->Set(0, 0, this->m_pView->GetDocument()->GetUnits());
			/// pBar->Set(this->m_pView, widget->GetProp3D(), this->m_pView->GetDocument()->GetUnits());
		}
	}
}

void CViewVTKCommand::Update()
{
	// Modified from 
	// int vtkPicker::Pick(float selectionX, float selectionY, float selectionZ,
	//                     vtkRenderer *renderer)
	int i;

	// get the position of the mouse cursor in screen/window coordinates
	// (pixel)
	vtkRenderer *renderer = this->m_pView->GetRenderer();
	int* pos = this->m_pView->GetRenderWindowInteractor()->GetEventPosition();

	// get the focal point in world coordinates
	//
	vtkCamera *camera = renderer->GetActiveCamera();	
	vtkFloatingPointType cameraFP[4];
	camera->GetFocalPoint((vtkFloatingPointType*)cameraFP); cameraFP[3] = 1.0;

	// Convert the focal point coordinates to display (or screen) coordinates.
	//
	renderer->SetWorldPoint(cameraFP);
	renderer->WorldToDisplay();
	vtkFloatingPointType *displayCoords = renderer->GetDisplayPoint();

	// Convert the selection point into world coordinates.
	//
	renderer->SetDisplayPoint(pos[0], pos[1], displayCoords[2]);
	renderer->DisplayToWorld();
	vtkFloatingPointType *worldCoords = renderer->GetWorldPoint();
	if ( worldCoords[3] == 0.0 ) {
		ASSERT(FALSE);
		return;
	}
	vtkFloatingPointType PickPosition[3];
	for (i = 0; i < 3; ++i) {
		PickPosition[i] = worldCoords[i] / worldCoords[3];
	}

	vtkFloatingPointType* bounds = this->m_pView->GetDocument()->GetGridBounds();
	vtkFloatingPointType zMin = bounds[4];
	vtkFloatingPointType zMax = bounds[5];
#ifdef USE_ZMAX
	vtkFloatingPointType zPos = zMax;
#else
	vtkFloatingPointType zPos = zMin;
#endif

	if ( camera->GetParallelProjection() )
	{
		double* cameraDOP = camera->GetDirectionOfProjection();
		// double t = -PickPosition[2] / cameraDOP[2];
		double t = (zPos - PickPosition[2]) / cameraDOP[2];
		for (i = 0; i < 2; ++i) {
			this->m_WorldPointXYPlane[i] = PickPosition[i] + t * cameraDOP[i];
		}
	}
	else
	{
		double *cameraPos = camera->GetPosition();
		// double t = -cameraPos[2] / ( PickPosition[2] - cameraPos[2] );
		double t = (zPos - cameraPos[2]) / ( PickPosition[2] - cameraPos[2] );
		for (i = 0; i < 2; ++i) {
			this->m_WorldPointXYPlane[i] = cameraPos[i] + t * ( PickPosition[i] - cameraPos[i] );
		}
	}
	this->m_WorldPointXYPlane[2] = zPos;

	// SCALE
	//
	vtkFloatingPointType* scale = this->m_pView->GetDocument()->GetScale();

	// UNITS
	const CUnits& units = this->m_pView->GetDocument()->GetUnits();
	const char* xy = units.horizontal.defined ? units.horizontal.input : units.horizontal.si;
	const char* z = units.vertical.defined ? units.vertical.input : units.vertical.si;

	// TODO this->m_WorldPointMinZPlane;

	((CMainFrame*)::AfxGetMainWnd())->UpdateXYZ(
		this->m_WorldPointXYPlane[0]/scale[0]/units.horizontal.input_to_si,
		this->m_WorldPointXYPlane[1]/scale[1]/units.horizontal.input_to_si,
		// zMin/scale[2]/units.vertical.input_to_si,
		this->m_WorldPointXYPlane[2]/scale[2]/units.vertical.input_to_si,
		xy,
		z);
}

void CViewVTKCommand::OnLeftButtonPressEvent(vtkObject* caller, void* callData)
{
	TRACE("OnLeftButtonPressEvent\n");

// COMMENT: {6/22/2005 7:34:29 PM}	if (this->m_pView->CreatingNewRiver())
// COMMENT: {6/22/2005 7:34:29 PM}	{
// COMMENT: {6/22/2005 7:34:29 PM}		this->m_pView->m_pCursor3DActor->SetPosition(this->m_WorldPointXYPlane[0], this->m_WorldPointXYPlane[1], this->m_WorldPointXYPlane[2]);
// COMMENT: {6/22/2005 7:34:29 PM}		this->m_pView->m_Renderer->ResetCameraClippingRange();
// COMMENT: {6/22/2005 7:34:29 PM}		this->m_pView->m_RenderWindowInteractor->Render();
// COMMENT: {6/22/2005 7:34:29 PM}
// COMMENT: {6/22/2005 7:34:29 PM}		// update m_WorldPointXYPlane and save starting point
// COMMENT: {6/22/2005 7:34:29 PM}		//
// COMMENT: {6/22/2005 7:34:29 PM}		Update();
// COMMENT: {6/22/2005 7:34:29 PM}		for (int i = 0; i < 3; ++i) {
// COMMENT: {6/22/2005 7:34:29 PM}			this->m_BeginPoint[i] = this->m_WorldPointXYPlane[i];
// COMMENT: {6/22/2005 7:34:29 PM}		}
// COMMENT: {6/22/2005 7:34:29 PM}	}

	if (this->m_pView->CreatingNewWell()) {
		this->m_pView->m_pCursor3DActor->SetPosition(this->m_WorldPointXYPlane[0], this->m_WorldPointXYPlane[1], this->m_WorldPointXYPlane[2]);
		this->m_pView->m_Renderer->ResetCameraClippingRange();
		this->m_pView->m_RenderWindowInteractor->Render();

		// update m_WorldPointXYPlane and save starting point
		//
		Update();
		for (int i = 0; i < 3; ++i) {
			this->m_BeginPoint[i] = this->m_WorldPointXYPlane[i];
		}
	}

	if (this->m_pView->CreatingNewZone()) {

		// m_pView->StartNewZone should have already been called
		//
		ASSERT(this->m_pView->m_pNewCube);
		ASSERT(this->m_pView->m_pNewCubeMapper);
		ASSERT(this->m_pView->m_pNewCubeActor);

		// update m_WorldPointXYPlane and save starting point
		//
		Update();
		for (int i = 0; i < 3; ++i) {
			this->m_BeginPoint[i] = this->m_WorldPointXYPlane[i];
		}

		// place cube actor
		//
		// this->m_pView->m_pNewCubeActor->SetPosition(this->m_BeginPoint[0], this->m_BeginPoint[1], 0);
		this->m_pView->m_pNewCubeActor->SetPosition(this->m_BeginPoint[0], this->m_BeginPoint[1], this->m_BeginPoint[2]);

		// turn on cube actor
		//
		ASSERT(this->m_pView->m_pNewCubeActor->GetVisibility() == 0);
		this->m_pView->m_pNewCubeActor->VisibilityOn();

		// render
		//
		this->m_pView->GetDocument()->UpdateAllViews(NULL);
	}
}

void CViewVTKCommand::OnLeftButtonReleaseEvent(vtkObject* caller, void* callData)
{
	TRACE("OnLeftButtonReleaseEvent\n");

// COMMENT: {6/22/2005 6:27:15 PM}	if (this->m_pView->CreatingNewRiver())
// COMMENT: {6/22/2005 6:27:15 PM}	{
// COMMENT: {6/22/2005 6:27:15 PM}		vtkFloatingPointType* scale = this->m_pView->GetDocument()->GetScale();
// COMMENT: {6/22/2005 6:27:15 PM}
// COMMENT: {6/22/2005 6:27:15 PM}		this->m_pView->m_pRiverActor->InsertNextPoint(
// COMMENT: {6/22/2005 6:27:15 PM}			this->m_BeginPoint[0] / scale[0] / this->m_pView->GetDocument()->GetUnits().horizontal.input_to_si,
// COMMENT: {6/22/2005 6:27:15 PM}			this->m_BeginPoint[1] / scale[1] / this->m_pView->GetDocument()->GetUnits().horizontal.input_to_si,
// COMMENT: {6/22/2005 6:27:15 PM}			this->m_pView->m_pRiverActor->GetZ()
// COMMENT: {6/22/2005 6:27:15 PM}			);
// COMMENT: {6/22/2005 6:27:15 PM}
// COMMENT: {6/22/2005 6:27:15 PM}		this->m_pView->m_Renderer->Render();
// COMMENT: {6/22/2005 6:27:15 PM}
// COMMENT: {6/22/2005 6:27:15 PM}		/// this->m_pView->CancelNewRiver();
// COMMENT: {6/22/2005 6:27:15 PM}	}

	if (this->m_pView->CreatingNewWell())
	{

		vtkFloatingPointType* bounds = this->m_pView->GetDocument()->GetGridBounds();
		vtkFloatingPointType zMin = bounds[4];
		vtkFloatingPointType zMax = bounds[5];

		// set radius
		//
		vtkFloatingPointType defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
		this->m_pView->m_pWellActor->SetDefaultTubeDiameter(defaultAxesSize * 0.17);

		// set scale
		//
		vtkFloatingPointType* scale = this->m_pView->GetDocument()->GetScale();
		this->m_pView->m_pWellActor->SetScale(scale);


		// set position
		//
		CWellSchedule well;
		well.n_user    = this->m_pView->GetDocument()->GetNextWellNumber();
		well.x_defined = TRUE;
		well.y_defined = TRUE;
		well.x         = this->m_BeginPoint[0] / scale[0] / this->m_pView->GetDocument()->GetUnits().horizontal.input_to_si;
		well.y         = this->m_BeginPoint[1] / scale[1] / this->m_pView->GetDocument()->GetUnits().horizontal.input_to_si;
		this->m_pView->m_pWellActor->SetWell(well, this->m_pView->GetDocument()->GetUnits());

		// set height
		//
		CGrid grid[3];
		this->m_pView->GetDocument()->GetGrid(grid[0], grid[1], grid[2]);
		this->m_pView->m_pWellActor->SetZAxis(grid[2], this->m_pView->GetDocument()->GetUnits());


		
// COMMENT: {8/13/2004 6:21:43 PM}		this->m_pView->m_pWellActor->SetHeight(zMax - zMin);
// COMMENT: {8/13/2004 6:21:43 PM}		this->m_pView->m_pWellActor->CappingOn();
// COMMENT: {8/13/2004 6:21:43 PM}		this->m_pView->m_pWellActor->SetResolution(20);
		

// COMMENT: {8/13/2004 6:22:57 PM}		this->m_pView->m_pWellActor->SetPosition(
// COMMENT: {8/13/2004 6:22:57 PM}			this->m_BeginPoint[0],
// COMMENT: {8/13/2004 6:22:57 PM}			this->m_BeginPoint[1],
// COMMENT: {8/13/2004 6:22:57 PM}			(zMax + zMin) / 2);


		this->m_pView->m_pWellActor->VisibilityOn();

		// render
		//
		this->m_pView->m_Renderer->AddProp(this->m_pView->m_pWellActor);
		this->m_pView->m_Renderer->Render();

		// display well properties
		//
		CPropertySheet sheet(_T("Define well properties"), 0);

		std::set<int> wellNums;
		this->m_pView->GetDocument()->GetUsedWellNumbers(wellNums);

		CWellPropertyPage page;
		page.SetProperties(well);
		page.SetGrid(grid[2]);
		page.SetUnits(this->m_pView->GetDocument()->GetUnits());
		page.SetUsedWellNumbers(wellNums);

		sheet.AddPage(&page);

		if (sheet.DoModal() == IDOK)
		{
			this->m_pView->EndNewWell();
			page.GetProperties(well);
			CWellCreateAction *pAction = new CWellCreateAction(this->m_pView->GetDocument(), well);
			this->m_pView->GetDocument()->Execute(pAction);
		}
		else
		{
			this->m_pView->CancelNewWell();
		}
	}


	if (this->m_pView->CreatingNewZone()) {

		// m_pView->StartNewZone should have already been called
		//
		ASSERT(this->m_pView->m_pNewCube);
		ASSERT(this->m_pView->m_pNewCubeMapper);
		ASSERT(this->m_pView->m_pNewCubeActor);

		// OnLeftButtonPressEvent called?
		//
		ASSERT(this->m_pView->m_pNewCubeActor->GetVisibility() != 0);

		// update cube
		//
        Update();
		this->m_pView->m_pNewCube->SetXLength( fabs(this->m_WorldPointXYPlane[0] - this->m_BeginPoint[0]) );
		this->m_pView->m_pNewCube->SetYLength( fabs(this->m_WorldPointXYPlane[1] - this->m_BeginPoint[1]) );
		this->m_pView->m_pNewCubeActor->SetPosition(
			(this->m_WorldPointXYPlane[0] + this->m_BeginPoint[0]) / 2.0,
			(this->m_WorldPointXYPlane[1] + this->m_BeginPoint[1]) / 2.0,
// COMMENT: {7/8/2004 8:04:56 PM}			this->m_pView->m_pNewCube->GetZLength() / 2.0);
#if defined(USE_ZMAX)
			(2 * this->m_WorldPointXYPlane[2] - this->m_pView->m_pNewCube->GetZLength()) / 2.0);
#else
			(2 * this->m_WorldPointXYPlane[2] + this->m_pView->m_pNewCube->GetZLength()) / 2.0);
#endif

		
		// get bounds before calling EndNewZone
		//
		vtkFloatingPointType scaled_meters[6];
		this->m_pView->m_pNewCubeActor->GetBounds(scaled_meters);
// COMMENT: {5/27/2004 8:36:31 PM}		this->m_pView->EndNewZone();

		// Note: the Interactor must be set before Execute
		// otherwise the selection will not be set
		//
#ifdef _DEBUG
		if (vtkInteractorStyle* style = vtkInteractorStyle::SafeDownCast(this->m_pView->m_pInteractorStyle)) {
			if (vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast(style)) {
				style = switcher->GetCurrentStyle();
			}
// COMMENT: {5/27/2004 8:39:28 PM}			ASSERT(style->GetInteractor() != 0);
		}
#endif
		//{{
		// get type of zone
		//
		CPropertySheet sheet("Zone Wizard");
		CNewZonePropertyPage     newZone;
		CMediaPropertyPage       mediaProps(IDS_MEDIA_PROPS_WIZ_135);
		CBCFluxPropertyPage      fluxProps;
		CBCLeakyPropertyPage     leakyProps;
		CBCSpecifiedPropertyPage specifiedProps;
		CICHeadPropertyPage      icHeadProps;
		CChemICPropertyPage      chemICProps;

		fluxProps.SetStressPeriod(1);
		leakyProps.SetStressPeriod(1);
		specifiedProps.SetStressPeriod(1);

		sheet.AddPage(&newZone);
		sheet.AddPage(&mediaProps);
		sheet.AddPage(&fluxProps);		
		sheet.AddPage(&leakyProps);		
		sheet.AddPage(&specifiedProps);		
		sheet.AddPage(&icHeadProps);		
		sheet.AddPage(&chemICProps);		
		sheet.SetWizardMode();

		if (sheet.DoModal() == ID_WIZFINISH) {
			///{{{
			this->m_pView->EndNewZone();
			///}}}

			// create new zone
			//

			// determine absolute zone
			vtkFloatingPointType* scale = this->m_pView->GetDocument()->GetScale();
			CZone absZone;
			const CUnits& units = this->m_pView->GetDocument()->GetUnits();
			absZone.x1 = scaled_meters[0] / scale[0] / units.horizontal.input_to_si;
			absZone.x2 = scaled_meters[1] / scale[0] / units.horizontal.input_to_si;
			absZone.y1 = scaled_meters[2] / scale[1] / units.horizontal.input_to_si;
			absZone.y2 = scaled_meters[3] / scale[1] / units.horizontal.input_to_si;
			absZone.z1 = scaled_meters[4] / scale[2] / units.vertical.input_to_si;
			absZone.z2 = scaled_meters[5] / scale[2] / units.vertical.input_to_si;

			CWPhastDoc* pDoc = this->m_pView->GetDocument();

			// ID_ZONE_TYPE_MEDIA
			if (newZone.GetType() == ID_ZONE_TYPE_MEDIA) {
				CGridElt elt;
				mediaProps.GetProperties(elt);
				CMediaZoneActor::Create(pDoc, absZone, elt);
			}
			// ID_ZONE_TYPE_BC_FLUX
			else if (newZone.GetType() == ID_ZONE_TYPE_BC_FLUX) {
				CBC bc;
				fluxProps.GetProperties(bc);
				CBCZoneActor::Create(pDoc, absZone, bc);
			}
			// ID_ZONE_TYPE_BC_LEAKY
			else if (newZone.GetType() == ID_ZONE_TYPE_BC_LEAKY) {
				CBC bc;
				leakyProps.GetProperties(bc);
				CBCZoneActor::Create(pDoc, absZone, bc);
			}
			// ID_ZONE_TYPE_BC_SPECIFIED
			else if (newZone.GetType() == ID_ZONE_TYPE_BC_SPECIFIED)
			{
				CBC bc;
				specifiedProps.GetProperties(bc);
				CBCZoneActor::Create(pDoc, absZone, bc);
			}
			// ID_ZONE_TYPE_IC_HEAD
			else if (newZone.GetType() == ID_ZONE_TYPE_IC_HEAD) {
				CHeadIC headic;
				icHeadProps.GetProperties(headic);
				CICZoneActor::Create(pDoc, absZone, headic);
			}
			// ID_ZONE_TYPE_IC_CHEM
			else if (newZone.GetType() == ID_ZONE_TYPE_IC_CHEM) {
				CChemIC chemIC;
				chemICProps.GetProperties(chemIC);
				CICZoneActor::Create(pDoc, absZone, chemIC);
			}
		}
		else {
			this->m_pView->CancelNewZone();
		}

		// render
		this->m_pView->GetDocument()->UpdateAllViews(NULL);
	}
}

void CViewVTKCommand::OnMouseMoveEvent(vtkObject* caller, void* callData)
{
	Update();

	if (this->m_pView->CreatingNewWell() || this->m_pView->CreatingNewRiver())
	{
		this->m_pView->m_pCursor3DActor->SetPosition(this->m_WorldPointXYPlane[0], this->m_WorldPointXYPlane[1], this->m_WorldPointXYPlane[2]);
		this->m_pView->m_Renderer->ResetCameraClippingRange();
		this->m_pView->m_RenderWindowInteractor->Render();
	}


	if (this->m_pView->CreatingNewZone()) {
// COMMENT: {7/8/2004 7:10:31 PM}		this->m_pView->m_pCursor3DActor->SetPosition(this->m_WorldPointXYPlane[0], this->m_WorldPointXYPlane[1], 0.0);
		this->m_pView->m_pCursor3DActor->SetPosition(this->m_WorldPointXYPlane[0], this->m_WorldPointXYPlane[1], this->m_WorldPointXYPlane[2]);

		if (this->m_pView->m_pCursor3DActor->GetVisibility() && this->m_pView->m_pNewCubeActor->GetVisibility()) {
			TRACE("OnMouseMoveEvent\n");
			this->m_pView->m_pNewCube->SetXLength( fabs(this->m_WorldPointXYPlane[0] - this->m_BeginPoint[0]) );
			this->m_pView->m_pNewCube->SetYLength( fabs(this->m_WorldPointXYPlane[1] - this->m_BeginPoint[1]) );

			this->m_pView->m_pNewCubeActor->SetPosition((this->m_WorldPointXYPlane[0] + this->m_BeginPoint[0]) / 2.0,
				(this->m_WorldPointXYPlane[1] + this->m_BeginPoint[1]) / 2.0,
// COMMENT: {7/8/2004 7:15:02 PM}				this->m_pView->m_pNewCube->GetZLength() / 2.0);
#ifdef USE_ZMAX
				(2 * this->m_WorldPointXYPlane[2] - this->m_pView->m_pNewCube->GetZLength()) / 2.0);
#else
				(2 * this->m_WorldPointXYPlane[2] + this->m_pView->m_pNewCube->GetZLength()) / 2.0);
#endif


			vtkFloatingPointType* scale = this->m_pView->GetDocument()->GetScale();
			if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
				static TCHAR buffer[80];
				const CUnits& units = this->m_pView->GetDocument()->GetUnits();
				::_sntprintf(buffer, 80, "%g[%s] x %g[%s]",
					fabs(this->m_WorldPointXYPlane[0] - this->m_BeginPoint[0]) / scale[0] / units.horizontal.input_to_si,
					units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
					fabs(this->m_WorldPointXYPlane[1] - this->m_BeginPoint[1]) / scale[1] / units.horizontal.input_to_si,
					units.horizontal.defined ? units.horizontal.input : units.horizontal.si
					);
				pWnd->SetWindowText(buffer);
			}
		}

		this->m_pView->m_Renderer->ResetCameraClippingRange();
		this->m_pView->m_RenderWindowInteractor->Render();
	}
}

void CViewVTKCommand::OnKeyPressEvent(vtkObject* caller, void* callData)
{
	char* keysym = this->m_pView->m_RenderWindowInteractor->GetKeySym();
	
	if (this->m_pView->CreatingNewZone() && ::strcmp(keysym, "Escape") == 0) {
		this->m_pView->CancelNewZone();
	}
}

void CViewVTKCommand::OnModifiedEvent(vtkObject* caller, void* callData)
{
#if defined(_DEBUG)
	if (caller)
	{
		ostrstream oss;
		caller->PrintSelf(oss, 4);
		oss << ends;
		afxDump << oss.str() << "\n";
		oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
	}
#endif

	ASSERT(caller && vtkCamera::SafeDownCast(caller));
	if (vtkBoxWidget *pWidget = this->m_pView->GetBoxWidget())
	{
		if (pWidget->GetEnabled())
		{
			// this implicitly calls the protected method pWidget->SizeHandles();
			pWidget->PlaceWidget();
			pWidget->Modified();
		}
	}
}
