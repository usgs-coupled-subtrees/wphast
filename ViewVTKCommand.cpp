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

#include "BCFluxPropertyPage.h"
#include "BCFluxPropertyPage2.h"

#include "BCLeakyPropertyPage.h"
#include "BCLeakyPropertyPage2.h"

#include "BCSpecifiedPropertyPage.h"
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


	if (this->m_pView->CreatingNewZone())
	{

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
        this->Update();
		this->m_pView->m_pNewCube->SetXLength( ::fabs(this->m_WorldPointXYPlane[0] - this->m_BeginPoint[0]) );
		this->m_pView->m_pNewCube->SetYLength( ::fabs(this->m_WorldPointXYPlane[1] - this->m_BeginPoint[1]) );
		this->m_pView->m_pNewCubeActor->SetPosition(
			(this->m_WorldPointXYPlane[0] + this->m_BeginPoint[0]) / 2.0,
			(this->m_WorldPointXYPlane[1] + this->m_BeginPoint[1]) / 2.0,
#if defined(USE_ZMAX)
			(2 * this->m_WorldPointXYPlane[2] - this->m_pView->m_pNewCube->GetZLength()) / 2.0);
#else
			(2 * this->m_WorldPointXYPlane[2] + this->m_pView->m_pNewCube->GetZLength()) / 2.0);
#endif


		// get bounds before calling EndNewZone
		//
		vtkFloatingPointType scaled_meters[6];
		this->m_pView->m_pNewCubeActor->GetBounds(scaled_meters);

		// get type of zone
		//
		ETSLayoutPropertySheet        sheet("Zone Wizard", NULL, 0, NULL, false);

		CNewZonePropertyPage          newZone;
#if defined(__CPPUNIT__)
		///TMediaPropertyPage<CPropertyPage> mediaProps(IDS_MEDIA_PROPS_WIZ_135);
		///CMediaPropertyPage            mediaProps(IDS_MEDIA_PROPS_WIZ_135);
		CMediaSpreadPropertyPage      mediaProps;
#else
		///CMediaPropertyPage            mediaProps(IDS_MEDIA_PROPS_WIZ_135);
		CMediaSpreadPropertyPage      mediaProps;
#endif
		CBCFluxPropertyPage2          fluxProps;
		CBCLeakyPropertyPage2         leakyProps;
		CBCSpecifiedHeadPropertyPage  specifiedProps;
		CICHeadSpreadPropertyPage     icHeadProps;
		CChemICSpreadPropertyPage     chemICProps;

		// CChemICSpreadPropertyPage only needs the flowonly flag when the zone is a
		// default zone
		//
		bool bFlowOnly = this->m_pView->GetDocument()->GetFlowOnly();

		fluxProps.SetFlowOnly(bFlowOnly);
		leakyProps.SetFlowOnly(bFlowOnly);
		specifiedProps.SetFlowOnly(bFlowOnly);

		sheet.AddPage(&newZone);
		sheet.AddPage(&mediaProps);
		sheet.AddPage(&fluxProps);
		sheet.AddPage(&leakyProps);
		sheet.AddPage(&specifiedProps);
		sheet.AddPage(&icHeadProps);
		sheet.AddPage(&chemICProps);

		sheet.SetWizardMode();

		if (sheet.DoModal() == ID_WIZFINISH)
		{
			this->m_pView->EndNewZone();

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

			if (newZone.GetType() == ID_ZONE_TYPE_MEDIA)
			{
				CGridElt elt;
				mediaProps.GetProperties(elt);
				CMediaZoneActor::Create(pDoc, absZone, elt, mediaProps.GetDesc());
			}
			else if (newZone.GetType() == ID_ZONE_TYPE_BC_FLUX)
			{
				CBC bc;
				fluxProps.GetProperties(bc);
				CBCZoneActor::Create(pDoc, absZone, bc, fluxProps.GetDesc());
			}
			else if (newZone.GetType() == ID_ZONE_TYPE_BC_LEAKY)
			{
				CBC bc;
				leakyProps.GetProperties(bc);
				CBCZoneActor::Create(pDoc, absZone, bc, leakyProps.GetDesc());
			}
			else if (newZone.GetType() == ID_ZONE_TYPE_BC_SPECIFIED)
			{
				CBC bc;
				specifiedProps.GetProperties(bc);
				CBCZoneActor::Create(pDoc, absZone, bc, specifiedProps.GetDesc());
			}
			else if (newZone.GetType() == ID_ZONE_TYPE_IC_HEAD)
			{
				CHeadIC headic;
				icHeadProps.GetProperties(headic);
				CICHeadZoneActor::Create(pDoc, absZone, headic, icHeadProps.GetDesc());
			}
			else if (newZone.GetType() == ID_ZONE_TYPE_IC_CHEM)
			{
				CChemIC chemIC;
				chemICProps.GetProperties(chemIC);
				CICChemZoneActor::Create(pDoc, absZone, chemIC, chemICProps.GetDesc());
			}
		}
		else
		{
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

	if (::strcmp(keysym, "Escape") == 0)
	{
		this->m_pView->CancelMode();
	}
}

void CViewVTKCommand::OnModifiedEvent(vtkObject* caller, void* callData)
{
	if (!this->m_pView) return;

#if defined(_DEBUG)
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

	///{{{
	if (vtkRenderer *renderer =  this->m_pView->GetRenderer())
	{
		int i;
		double radius, z;
		double windowLowerLeft[4], windowUpperRight[4];
		float *viewport = renderer->GetViewport();
		int *winSize = renderer->GetRenderWindow()->GetSize();
		double focalPoint[4];

// COMMENT: {5/5/2006 5:33:00 PM}		this->ComputeWorldToDisplay(this->m_WorldPointXYPlane[0],
// COMMENT: {5/5/2006 5:33:00 PM}									this->m_WorldPointXYPlane[1],
// COMMENT: {5/5/2006 5:33:00 PM}									this->m_WorldPointXYPlane[2], focalPoint);

		// get the focal point in world coordinates
		//
		vtkCamera *camera = renderer->GetActiveCamera();	
		vtkFloatingPointType cameraFP[4];
		camera->GetFocalPoint((vtkFloatingPointType*)cameraFP); cameraFP[3] = 1.0;


		this->ComputeWorldToDisplay(cameraFP[0],
									cameraFP[1],
									cameraFP[2], focalPoint);

		/****
		this->ComputeWorldToDisplay(this->FixedPlanePoint[0],
									this->FixedPlanePoint[1],
									this->FixedPlanePoint[2], focalPoint);
		****/

// COMMENT: {5/4/2006 10:08:45 PM}		//{{
// COMMENT: {5/4/2006 10:08:45 PM}		double xyz[3];
// COMMENT: {5/4/2006 10:08:45 PM}		xyz[this->FixedCoord] = focalPoint[this->FixedCoord];
// COMMENT: {5/4/2006 10:08:45 PM}
// COMMENT: {5/4/2006 10:08:45 PM}		switch(this->FixedCoord)
// COMMENT: {5/4/2006 10:08:45 PM}		{
// COMMENT: {5/4/2006 10:08:45 PM}		case 0:
// COMMENT: {5/4/2006 10:08:45 PM}			xyz[0] = winSize[0] * viewport[0];
// COMMENT: {5/4/2006 10:08:45 PM}			xyz[1] = winSize[1] * viewport[1];
// COMMENT: {5/4/2006 10:08:45 PM}			break;
// COMMENT: {5/4/2006 10:08:45 PM}		case 1:
// COMMENT: {5/4/2006 10:08:45 PM}			break;
// COMMENT: {5/4/2006 10:08:45 PM}		case 2:
// COMMENT: {5/4/2006 10:08:45 PM}			xyz[0] = winSize[0] * viewport[0];
// COMMENT: {5/4/2006 10:08:45 PM}			xyz[1] = winSize[1] * viewport[1];
// COMMENT: {5/4/2006 10:08:45 PM}			break;
// COMMENT: {5/4/2006 10:08:45 PM}		}
// COMMENT: {5/4/2006 10:08:45 PM}		//}}
		z = focalPoint[2];

		double x = winSize[0] * viewport[0];
		double y = winSize[1] * viewport[1];
		this->ComputeDisplayToWorld(x,y,z,windowLowerLeft);
#ifdef _DEBUG
		TRACE("windowLowerLeft = %g, %g, %g\n", windowLowerLeft[0], windowLowerLeft[1], windowLowerLeft[2]);
#endif

		x = winSize[0] * viewport[2];
		y = winSize[1] * viewport[3];
		this->ComputeDisplayToWorld(x,y,z,windowUpperRight);
#ifdef _DEBUG
		TRACE("windowUpperRight = %g, %g, %g\n", windowUpperRight[0], windowUpperRight[1], windowUpperRight[2]);
		TRACE("dx^2 = %g\n", ::pow(windowUpperRight[0] - windowLowerLeft[0] , 2.0));
		TRACE("dy^2 = %g\n", ::pow(windowUpperRight[1] - windowLowerLeft[1] , 2.0));
		TRACE("dz^2 = %g\n", ::pow(windowUpperRight[2] - windowLowerLeft[2] , 2.0));
#endif

		for (radius=0.0, i=0; i<3; i++)
		{
			radius += (windowUpperRight[i] - windowLowerLeft[i]) *
				(windowUpperRight[i] - windowLowerLeft[i]);
		}

		this->m_pView->SizeHandles(::sqrt(radius));
	}
	///}}}
}
