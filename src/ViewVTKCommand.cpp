#include "StdAfx.h"

#define USE_ZMAX

#include "ViewVTKCommand.h"
#include "resource.h"

#include "WPhastDoc.h"
#include "WPhastView.h"
#include "BoxPropertiesDialogBar.h"
#include "ZoneActor.h"
#include "MediaZoneActor.h"
#include "MediaSpreadPropertyPage.h"
#include "BCZoneActor.h"
#include "ICZoneActor.h"
#include "ICHeadZoneActor.h"
#include "ICChemZoneActor.h"

#include "WellActor.h"
#include "Well.h"
#include "WellPropertyPage.h"

#include "RiverActor.h"
#include "River.h"
//#include "RiverPropertyPage.h"

#include "ZoneResizeAction.h"
#include "NewZonePropertyPage.h"
#include "MediaPropertyPage.h"

#include "BCFluxPropertyPage2.h"

#include "BCLeakyPropertyPage2.h"

#include "BCSpecifiedHeadPropertyPage.h"

#include "ICHeadPropertyPage.h"
#include "ICHeadSpreadPropertyPage.h"
#include "ChemICPropertyPage.h"
#include "ChemICSpreadPropertyPage.h"

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
// COMMENT: {3/5/2008 4:27:04 PM}#ifdef USE_WEDGE
// COMMENT: {3/5/2008 4:27:04 PM}#include "MyCubeSource.h"
// COMMENT: {3/5/2008 4:27:04 PM}#endif
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkPolyDataMapper.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkCylinderSource.h>
#include "Global.h"
#include "FlowOnly.h"

#include "MainFrm.h"

#include "Utilities.h"

#include <afxmt.h>
static CCriticalSection  critSect;

#if defined(USE_INTRINSIC)
#pragma intrinsic(fabs) // using this inlines fabs and is ~ 4x faster
#define FABS(x) fabs(x)
#else
#define FABS(x) ((x < 0) ? -x : x) // this is slightly slower than the intrinsic
#endif

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
			ASSERT(callData == NULL);
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

#ifdef SKIP
void CViewVTKCommand::OnEndPickEvent(vtkObject* caller, void* callData)
{
	TRACE("vtkCommand::EndPickEvent\n");
	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( this->m_pView->GetRenderWindowInteractor()->GetPicker() ))
	{
		if (vtkAssemblyPath *path = picker->GetPath())
		{
			{
#if defined(_DEBUG)
				ostrstream oss;
				path->PrintSelf(oss, 4);
				oss << ends;
				TRACE("\n");
				afxDump << "vtkAssemblyPath{{\n" << oss.str() << "vtkAssemblyPath}}\n";
				oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
#endif

			//{{
			if (path->GetFirstNode()->GetProp()->IsA("vtkPropAssembly"))
			{
			//}}

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
					if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(path->GetLastNode()->GetProp()))
					{
						///pZone->Select(this->m_pView);
						this->m_pView->GetDocument()->Select(pZoneActor);
					}

					// check if well
					//
					if (CWellActor *pWell = CWellActor::SafeDownCast(path->GetLastNode()->GetProp()))
					{
						this->m_pView->GetDocument()->Select(pWell);
					}

				}
			//{{
			}
			else
			{
				vtkProp *prop = path->GetFirstNode()->GetProp();

				// check if zone
				//
				if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(prop))
				{
					this->m_pView->GetDocument()->Select(pZoneActor);
				}

				// check if river
				//
				else if (CRiverActor *pRiver = CRiverActor::SafeDownCast(prop))
				{
					this->m_pView->GetDocument()->Select(pRiver);
				}

				// check if well
				//
				else if (CWellActor *pWell = CWellActor::SafeDownCast(prop))
				{
					this->m_pView->GetDocument()->Select(pWell);
				}

				else
				{
					ASSERT(FALSE);
				}
			}
			//}}
			}


#ifdef _DEBUG
			int* xy = this->m_pView->GetRenderWindowInteractor()->GetEventPosition();
			TRACE("EventPosition(%d, %d)\n", xy[0], xy[1]);
#endif
		}
		else
		{
			// Hide BoxWidget
			//
			//this->m_pView->GetBoxWidget()->SetEnabled(0);
			this->m_pView->GetDocument()->ClearSelection();
		}

		// render
		//
		this->m_pView->GetDocument()->UpdateAllViews(0);
	}
}
#endif

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

//{{
	// determine most likely plane by finding
	// largest component vector
	//
	double max = 0.0;
	vtkFloatingPointType viewPlaneNormal[3];
	camera->GetViewPlaneNormal(viewPlaneNormal);
	for (int i = 0; i < 3; ++i)
	{
		// Note: fabs() is ~4x slower
		//
		if ( max < FABS(viewPlaneNormal[i]) )
		{
			this->FixedCoord = i;
			if ( viewPlaneNormal[i] < 0.0 )
			{
				max = -viewPlaneNormal[i];
				this->FixedPlane = 2 * i;
			}
			else
			{
				max = viewPlaneNormal[i];
				this->FixedPlane = 2 * i + 1;
			}
		}
	}
	ASSERT( 0 <= this->FixedPlane && this->FixedPlane < 6 );
	ASSERT( 0 <= this->FixedCoord && this->FixedCoord < 3 );

	// get point of intersection of axis=this->FixedCoord with a value of bounds[this->this->FixedPlane]
	// this->FixedPlane       this->FixedCoord
	//    0 => xmin        0 => x
	//    1 => xmax        1 => y
	//    2 => ymin        2 => z
	//    3 => ymax
	//    4 => zmin
	//    5 => zmax
	//
	CUtilities::GetWorldPointAtFixedPlane(this->m_pView->GetRenderWindowInteractor(), renderer, this->FixedCoord, bounds[this->FixedPlane], this->FixedPlanePoint);

	((CMainFrame*)::AfxGetMainWnd())->UpdateXYZ(
		this->FixedPlanePoint[0]/scale[0]/units.horizontal.input_to_si,
		this->FixedPlanePoint[1]/scale[1]/units.horizontal.input_to_si,
		this->FixedPlanePoint[2]/scale[2]/units.vertical.input_to_si,
		xy,
		z);
	return;
//}}

	((CMainFrame*)::AfxGetMainWnd())->UpdateXYZ(
		this->m_WorldPointXYPlane[0]/scale[0]/units.horizontal.input_to_si,
		this->m_WorldPointXYPlane[1]/scale[1]/units.horizontal.input_to_si,
		// zMin/scale[2]/units.vertical.input_to_si,
		this->m_WorldPointXYPlane[2]/scale[2]/units.vertical.input_to_si,
		xy,
		z);
}

void CViewVTKCommand::ComputeDisplayToWorld(double x, double y, double z, double worldPt[4])
{
	vtkRenderer *renderer = this->m_pView->GetRenderer();
	if (!renderer)
	{
		return;
	}

	renderer->SetDisplayPoint(x, y, z);
	renderer->DisplayToWorld();
	renderer->GetWorldPoint(worldPt);
	if (worldPt[3])
	{
		worldPt[0] /= worldPt[3];
		worldPt[1] /= worldPt[3];
		worldPt[2] /= worldPt[3];
		worldPt[3] = 1.0;
	}
}

void CViewVTKCommand::ComputeWorldToDisplay(double x, double y, double z, double displayPt[4])
{
	vtkRenderer *renderer = this->m_pView->GetRenderer();
	if (!renderer)
	{
		return;
	}

	renderer->SetWorldPoint(x, y, z, 1.0);
	renderer->WorldToDisplay();
	renderer->GetDisplayPoint(displayPt);
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

// COMMENT: {3/5/2008 4:41:27 PM}	if (this->m_pView->CreatingNewZone()) {
// COMMENT: {3/5/2008 4:41:27 PM}		ASSERT(FALSE); // DEAD ???
// COMMENT: {3/5/2008 4:41:27 PM}
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		// m_pView->StartNewZone should have already been called
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		//
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		ASSERT(this->m_pView->m_pNewCube);
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		ASSERT(this->m_pView->m_pNewCubeMapper);
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		ASSERT(this->m_pView->m_pNewCubeActor);
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		// update m_WorldPointXYPlane and save starting point
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		//
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		Update();
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		for (int i = 0; i < 3; ++i) {
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}			this->m_BeginPoint[i] = this->m_WorldPointXYPlane[i];
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		}
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		// place cube actor
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		//
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		// this->m_pView->m_pNewCubeActor->SetPosition(this->m_BeginPoint[0], this->m_BeginPoint[1], 0);
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		this->m_pView->m_pNewCubeActor->SetPosition(this->m_BeginPoint[0], this->m_BeginPoint[1], this->m_BeginPoint[2]);
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		// turn on cube actor
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		//
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		ASSERT(this->m_pView->m_pNewCubeActor->GetVisibility() == 0);
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		this->m_pView->m_pNewCubeActor->VisibilityOn();
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		// render
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		//
// COMMENT: {3/5/2008 4:41:27 PM}// COMMENT: {3/5/2008 4:32:01 PM}		this->m_pView->GetDocument()->UpdateAllViews(NULL);
// COMMENT: {3/5/2008 4:41:27 PM}	}
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
		page.SetFlowOnly(this->m_pView->GetDocument()->GetFlowOnly());

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


// COMMENT: {3/5/2008 4:42:08 PM}	if (this->m_pView->CreatingNewZone())
// COMMENT: {3/5/2008 4:42:08 PM}	{
// COMMENT: {3/5/2008 4:42:08 PM}		ASSERT(FALSE);
// COMMENT: {3/5/2008 4:42:08 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		// m_pView->StartNewZone should have already been called
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		//
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		ASSERT(this->m_pView->m_pNewCube);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		ASSERT(this->m_pView->m_pNewCubeMapper);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		ASSERT(this->m_pView->m_pNewCubeActor);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		// OnLeftButtonPressEvent called?
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		//
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		ASSERT(this->m_pView->m_pNewCubeActor->GetVisibility() != 0);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		// update cube
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		//
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}        this->Update();
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		this->m_pView->m_pNewCube->SetXLength( ::fabs(this->m_WorldPointXYPlane[0] - this->m_BeginPoint[0]) );
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		this->m_pView->m_pNewCube->SetYLength( ::fabs(this->m_WorldPointXYPlane[1] - this->m_BeginPoint[1]) );
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		this->m_pView->m_pNewCubeActor->SetPosition(
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			(this->m_WorldPointXYPlane[0] + this->m_BeginPoint[0]) / 2.0,
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			(this->m_WorldPointXYPlane[1] + this->m_BeginPoint[1]) / 2.0,
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}#if defined(USE_ZMAX)
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			(2 * this->m_WorldPointXYPlane[2] - this->m_pView->m_pNewCube->GetZLength()) / 2.0);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}#else
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			(2 * this->m_WorldPointXYPlane[2] + this->m_pView->m_pNewCube->GetZLength()) / 2.0);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}#endif
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		// get bounds before calling EndNewZone
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		//
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		vtkFloatingPointType scaled_meters[6];
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		this->m_pView->m_pNewCubeActor->GetBounds(scaled_meters);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		// get type of zone
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		//
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		ETSLayoutPropertySheet        sheet("Zone Wizard", NULL, 0, NULL, false);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		CNewZonePropertyPage          newZone;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}#if defined(__CPPUNIT__)
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		///TMediaPropertyPage<CPropertyPage> mediaProps(IDS_MEDIA_PROPS_WIZ_135);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		///CMediaPropertyPage            mediaProps(IDS_MEDIA_PROPS_WIZ_135);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		CMediaSpreadPropertyPage      mediaProps;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}#else
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		///CMediaPropertyPage            mediaProps(IDS_MEDIA_PROPS_WIZ_135);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		CMediaSpreadPropertyPage      mediaProps;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}#endif
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		CBCFluxPropertyPage2          fluxProps;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		CBCLeakyPropertyPage2         leakyProps;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		CBCSpecifiedHeadPropertyPage  specifiedProps;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		CICHeadSpreadPropertyPage     icHeadProps;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		CChemICSpreadPropertyPage     chemICProps;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		// CChemICSpreadPropertyPage only needs the flowonly flag when the zone is a
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		// default zone
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		//
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		bool bFlowOnly = this->m_pView->GetDocument()->GetFlowOnly();
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		fluxProps.SetFlowOnly(bFlowOnly);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		leakyProps.SetFlowOnly(bFlowOnly);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		specifiedProps.SetFlowOnly(bFlowOnly);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		sheet.AddPage(&newZone);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		sheet.AddPage(&mediaProps);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		sheet.AddPage(&fluxProps);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		sheet.AddPage(&leakyProps);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		sheet.AddPage(&specifiedProps);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		sheet.AddPage(&icHeadProps);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		sheet.AddPage(&chemICProps);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		sheet.SetWizardMode();
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		if (sheet.DoModal() == ID_WIZFINISH)
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		{
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			this->m_pView->EndNewZone();
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			// create new zone
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			//
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			// determine absolute zone
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			vtkFloatingPointType* scale = this->m_pView->GetDocument()->GetScale();
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			CZone absZone;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			const CUnits& units = this->m_pView->GetDocument()->GetUnits();
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			absZone.x1 = scaled_meters[0] / scale[0] / units.horizontal.input_to_si;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			absZone.x2 = scaled_meters[1] / scale[0] / units.horizontal.input_to_si;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			absZone.y1 = scaled_meters[2] / scale[1] / units.horizontal.input_to_si;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			absZone.y2 = scaled_meters[3] / scale[1] / units.horizontal.input_to_si;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			absZone.z1 = scaled_meters[4] / scale[2] / units.vertical.input_to_si;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			absZone.z2 = scaled_meters[5] / scale[2] / units.vertical.input_to_si;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			CWPhastDoc* pDoc = this->m_pView->GetDocument();
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			if (newZone.GetType() == ID_ZONE_TYPE_MEDIA)
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			{
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CGridElt elt;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				mediaProps.GetProperties(elt);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CMediaZoneActor::Create(pDoc, absZone, elt, mediaProps.GetDesc());
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			else if (newZone.GetType() == ID_ZONE_TYPE_BC_FLUX)
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			{
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CBC bc;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				fluxProps.GetProperties(bc);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CBCZoneActor::Create(pDoc, absZone, bc, fluxProps.GetDesc());
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			else if (newZone.GetType() == ID_ZONE_TYPE_BC_LEAKY)
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			{
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CBC bc;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				leakyProps.GetProperties(bc);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CBCZoneActor::Create(pDoc, absZone, bc, leakyProps.GetDesc());
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			else if (newZone.GetType() == ID_ZONE_TYPE_BC_SPECIFIED)
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			{
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CBC bc;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				specifiedProps.GetProperties(bc);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CBCZoneActor::Create(pDoc, absZone, bc, specifiedProps.GetDesc());
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			else if (newZone.GetType() == ID_ZONE_TYPE_IC_HEAD)
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			{
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CHeadIC headic;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				icHeadProps.GetProperties(headic);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CICHeadZoneActor::Create(pDoc, absZone, headic, icHeadProps.GetDesc());
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			else if (newZone.GetType() == ID_ZONE_TYPE_IC_CHEM)
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			{
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CChemIC chemIC;
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				chemICProps.GetProperties(chemIC);
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}				CICChemZoneActor::Create(pDoc, absZone, chemIC, chemICProps.GetDesc());
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		else
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		{
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}			this->m_pView->CancelNewZone();
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		// render
// COMMENT: {3/5/2008 4:42:08 PM}// COMMENT: {3/5/2008 4:33:01 PM}		this->m_pView->GetDocument()->UpdateAllViews(NULL);
// COMMENT: {3/5/2008 4:42:08 PM}	}
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


// COMMENT: {3/5/2008 4:40:06 PM}	if (this->m_pView->CreatingNewZone()) {
// COMMENT: {3/5/2008 4:40:06 PM}		ASSERT(FALSE);
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}// COMMENT: {7/8/2004 7:10:31 PM}		this->m_pView->m_pCursor3DActor->SetPosition(this->m_WorldPointXYPlane[0], this->m_WorldPointXYPlane[1], 0.0);
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}		this->m_pView->m_pCursor3DActor->SetPosition(this->m_WorldPointXYPlane[0], this->m_WorldPointXYPlane[1], this->m_WorldPointXYPlane[2]);
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}		if (this->m_pView->m_pCursor3DActor->GetVisibility() && this->m_pView->m_pNewCubeActor->GetVisibility()) {
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}			TRACE("OnMouseMoveEvent\n");
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}			ASSERT(FALSE); // {{ DEAD CODE ???
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}			this->m_pView->m_pNewCube->SetXLength( fabs(this->m_WorldPointXYPlane[0] - this->m_BeginPoint[0]) );
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}			this->m_pView->m_pNewCube->SetYLength( fabs(this->m_WorldPointXYPlane[1] - this->m_BeginPoint[1]) );
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}			this->m_pView->m_pNewCubeActor->SetPosition((this->m_WorldPointXYPlane[0] + this->m_BeginPoint[0]) / 2.0,
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}				(this->m_WorldPointXYPlane[1] + this->m_BeginPoint[1]) / 2.0,
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}// COMMENT: {7/8/2004 7:15:02 PM}				this->m_pView->m_pNewCube->GetZLength() / 2.0);
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}#ifdef USE_ZMAX
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}				(2 * this->m_WorldPointXYPlane[2] - this->m_pView->m_pNewCube->GetZLength()) / 2.0);
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}#else
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}				(2 * this->m_WorldPointXYPlane[2] + this->m_pView->m_pNewCube->GetZLength()) / 2.0);
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}#endif
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}			ASSERT(FALSE); // }} DEAD CODE ???
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}			vtkFloatingPointType* scale = this->m_pView->GetDocument()->GetScale();
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}			if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar()) {
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}				static TCHAR buffer[80];
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}				const CUnits& units = this->m_pView->GetDocument()->GetUnits();
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}				::_sntprintf(buffer, 80, "%g[%s] x %g[%s]",
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}					fabs(this->m_WorldPointXYPlane[0] - this->m_BeginPoint[0]) / scale[0] / units.horizontal.input_to_si,
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}					units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}					fabs(this->m_WorldPointXYPlane[1] - this->m_BeginPoint[1]) / scale[1] / units.horizontal.input_to_si,
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}					units.horizontal.defined ? units.horizontal.input : units.horizontal.si
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}					);
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}				pWnd->SetWindowText(buffer);
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}			}
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}		}
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}		this->m_pView->m_Renderer->ResetCameraClippingRange();
// COMMENT: {3/5/2008 4:40:06 PM}// COMMENT: {3/5/2008 4:33:30 PM}		this->m_pView->m_RenderWindowInteractor->Render();
// COMMENT: {3/5/2008 4:40:06 PM}	}
}

void CViewVTKCommand::OnKeyPressEvent(vtkObject* caller, void* callData)
{
	char* keysym = this->m_pView->m_RenderWindowInteractor->GetKeySym();

	if (::strcmp(keysym, "Escape") == 0)
	{
		this->m_pView->CancelMode();
	}
}

void CViewVTKCommand::OnModifiedEvent(vtkObject* caller, void* callData)
{
	if (!this->m_pView) return;

#if defined(_DEBUG) && defined(SKIP)
	if (caller)
	{
		ostrstream oss;
		caller->PrintSelf(oss, 4);
		oss << ends;
		///afxDump << oss.str() << "\n";
		TRACE("%s\n", oss.str());
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

	if (vtkRenderer *renderer =  this->m_pView->GetRenderer())
	{
		int i;
		double radius, z;
		double windowLowerLeft[4], windowUpperRight[4];
		vtkFloatingPointType *viewport = renderer->GetViewport();
		int *winSize = renderer->GetRenderWindow()->GetSize();
		double focalPoint[4];

		// get the focal point in world coordinates
		//
		vtkCamera *camera = renderer->GetActiveCamera();	
		vtkFloatingPointType cameraFP[4];
		camera->GetFocalPoint((vtkFloatingPointType*)cameraFP); cameraFP[3] = 1.0;

		this->ComputeWorldToDisplay(cameraFP[0],
									cameraFP[1],
									cameraFP[2], focalPoint);

		z = focalPoint[2];

		double x = winSize[0] * viewport[0];
		double y = winSize[1] * viewport[1];
		this->ComputeDisplayToWorld(x,y,z,windowLowerLeft);

		x = winSize[0] * viewport[2];
		y = winSize[1] * viewport[3];
		this->ComputeDisplayToWorld(x,y,z,windowUpperRight);

		for (radius=0.0, i=0; i<3; i++)
		{
			radius += (windowUpperRight[i] - windowLowerLeft[i]) *
				(windowUpperRight[i] - windowLowerLeft[i]);
		}

		TRACE("CViewVTKCommand::OnModifiedEvent radius = %g\n", ::sqrt(radius));
		this->m_pView->SizeHandles(::sqrt(radius));
	}
}

void CViewVTKCommand::OnEndPickEvent(vtkObject* caller, void* callData)
{
	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( this->m_pView->GetRenderWindowInteractor()->GetPicker() ))
	{
		if (vtkAssemblyPath *path = picker->GetPath())
		{
#if defined(_DEBUG)
			ostrstream oss;
			path->PrintSelf(oss, 4);
			oss << ends;
			TRACE("\n");
			afxDump << "vtkAssemblyPath{{\n" << oss.str() << "vtkAssemblyPath}}\n";
			oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
#endif
			if (path->GetFirstNode()->GetProp()->IsA("vtkPropAssembly"))
			{
				TRACE("path->GetNumberOfItems() = %d\n", path->GetNumberOfItems());
				if (path->GetNumberOfItems() == 3)
				{
					// should be river Or CZoneActor
					//
					path->InitTraversal();
					vtkProp* pPropAssembly = path->GetNextNode()->GetProp();
					ASSERT(pPropAssembly->IsA("vtkPropAssembly"));

					vtkProp* pActor = path->GetNextNode()->GetProp();
					if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(pActor))
					{
						this->m_pView->GetDocument()->Select(pZoneActor);
					}
					if (CRiverActor *pRiver = CRiverActor::SafeDownCast(pActor))
					{
						this->m_pView->GetDocument()->Select(pRiver);
					}
					if (CWellActor *pWell = CWellActor::SafeDownCast(pActor))
					{
						ASSERT(FALSE);  //	NEVER???
						this->m_pView->GetDocument()->Select(pWell);
					}
				}
				else
				{
					ASSERT( path->GetNumberOfItems() == 2 ); // the vtkPropAssembly and the CZoneActor or CWellActor

					// this will change as additional pickable types are added
					//
					ASSERT(
						path->GetLastNode()->GetProp()->IsA("CZoneActor")
						||
						path->GetLastNode()->GetProp()->IsA("CWellActor")
						);

					// check if zone
					//
					if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(path->GetLastNode()->GetProp()))
					{
						this->m_pView->GetDocument()->Select(pZoneActor);
					}

					// check if well
					//
					if (CWellActor *pWell = CWellActor::SafeDownCast(path->GetLastNode()->GetProp()))
					{
						this->m_pView->GetDocument()->Select(pWell);
					}
				}
			}
			else
			{
				vtkProp *prop = path->GetFirstNode()->GetProp();

				// check if zone
				//
				if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(prop))
				{
					this->m_pView->GetDocument()->Select(pZoneActor);
				}
				// check if river
				//
				else if (CRiverActor *pRiver = CRiverActor::SafeDownCast(prop))
				{
					this->m_pView->GetDocument()->Select(pRiver);
				}
				// check if well
				//
				else if (CWellActor *pWell = CWellActor::SafeDownCast(prop))
				{
					this->m_pView->GetDocument()->Select(pWell);
				}
				else
				{
					ASSERT(FALSE);
				}
			}

#ifdef _DEBUG
			int* xy = this->m_pView->GetRenderWindowInteractor()->GetEventPosition();
			TRACE("EventPosition(%d, %d)\n", xy[0], xy[1]);
#endif
		}
		else
		{
			// Hide BoxWidget
			//
			this->m_pView->GetDocument()->ClearSelection();
		}

		// render
		//
		this->m_pView->GetDocument()->UpdateAllViews(0);
	}
}
