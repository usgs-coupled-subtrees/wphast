#include "StdAfx.h"

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

#include "DrainActor.h"

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
#include "GridKeyword.h"

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

#define USE_INTRINSIC

#if defined(USE_INTRINSIC)
#pragma intrinsic(fabs) // using this inlines fabs and is ~ 4x faster
#define FABS(x) fabs(x)
#else
#define FABS(x) ((x < 0) ? -x : x) // this is slightly slower than the intrinsic
#endif

CViewVTKCommand::CViewVTKCommand(CWPhastView* pView)
: m_pView(pView)
, m_pAction(0)
, Transform(0)
{
	ASSERT_VALID(pView);
	this->Transform = vtkTransform::New();
}

CViewVTKCommand::~CViewVTKCommand()
{
	if (this->Transform)
	{
		this->Transform->Delete();
	}
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
			// TODO add functionality to change coor sys on the fly
			PHAST_Transform::COORDINATE_SYSTEM cs = pWellActor->GetWell().xy_coordinate_system_user;
			this->m_pAction =
				new CWellSetPositionAction(pWellActor, this->m_pView->GetDocument(), 0.0, 0.0, cs);
		}
	}
}

void CViewVTKCommand::OnEndInteractionEvent(vtkObject* caller, void* callData)
{
	if (vtkBoxWidget *widget = vtkBoxWidget::SafeDownCast(caller))
	{
		if (CZoneActor *pZone = CZoneActor::SafeDownCast(widget->GetProp3D()))
		{
			//{{
			pZone->SetUserTransform(0);
			//}}
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
			const double* scale = this->m_pView->GetDocument()->GetScale();

			// Coor Sys
			PHAST_Transform::COORDINATE_SYSTEM cs = PHAST_Transform::GRID;

			if (this->Transform)
			{
				this->Transform->Identity();
				if (this->m_pView->GetDocument()->GetCoordinateMode() == CWPhastDoc::MapMode)
				{
					const CGridKeyword& gridKeyword = this->m_pView->GetDocument()->GetGridKeyword();
					this->Transform->Scale(
						scale[0] * units.map_horizontal.input_to_si,
						scale[1] * units.map_horizontal.input_to_si,
						scale[2] * units.map_vertical.input_to_si);
					this->Transform->RotateZ(-gridKeyword.m_grid_angle);
					this->Transform->Translate(-gridKeyword.m_grid_origin[0], -gridKeyword.m_grid_origin[1], -gridKeyword.m_grid_origin[2]);
					cs = PHAST_Transform::MAP;
				}
				else
				{
					ASSERT(this->m_pView->GetDocument()->GetCoordinateMode() == CWPhastDoc::GridMode);
					this->Transform->Scale(
						scale[0] * units.horizontal.input_to_si,
						scale[1] * units.horizontal.input_to_si,
						scale[2] * units.vertical.input_to_si);
				}
				this->Transform->Inverse();

				double pt[3];
				pt[0] = widget->GetPosition()[0];
				pt[1] = widget->GetPosition()[1];
				pt[2] = 0;

				this->Transform->TransformPoint(pt, pt);

				((CWellSetPositionAction*)this->m_pAction)->SetPosition(pt[0], pt[1], cs);
				this->m_pView->GetDocument()->Execute(this->m_pAction);
				this->m_pAction = 0;
			}
			else
			{
				ASSERT(FALSE);
			}
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
				path->PrintSelf(oss, vtkIndent(4));
				oss << ends;
				TRACE("\n");
				afxDump << "vtkAssemblyPath{{\n" << oss.str() << "vtkAssemblyPath}}\n";
				oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
#endif

			//{{
			if (path->GetFirstNode()->GetViewProp()->IsA("vtkPropAssembly"))
			{
			//}}

				ASSERT(path->GetFirstNode()->GetViewProp()->IsA("vtkPropAssembly"));

				if (path->GetNumberOfItems() == 3)
				{
					// should be river
					//
					vtkCollectionSimpleIterator csi;
					path->InitTraversal(csi);
					vtkProp* pPropAssembly = path->GetNextNode(csi)->GetViewProp();
					ASSERT(pPropAssembly->IsA("vtkPropAssembly"));
					vtkProp* pRiverActor = path->GetNextNode()->GetViewProp();
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
						path->GetLastNode()->GetViewProp()->IsA("CZoneActor")
						||
						path->GetLastNode()->GetViewProp()->IsA("CWellActor")
						);

					// check if zone
					//
					if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(path->GetLastNode()->GetViewProp()))
					{
						///pZone->Select(this->m_pView);
						this->m_pView->GetDocument()->Select(pZoneActor);
					}

					// check if well
					//
					if (CWellActor *pWell = CWellActor::SafeDownCast(path->GetLastNode()->GetViewProp()))
					{
						this->m_pView->GetDocument()->Select(pWell);
					}

				}
			//{{
			}
			else
			{
				vtkProp *prop = path->GetFirstNode()->GetViewProp();

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
	Update2();

	if (vtkPointWidget2 *widget = vtkPointWidget2::SafeDownCast(caller))
	{
		if (CWellActor *pWellActor = CWellActor::SafeDownCast(widget->GetProp3D()))
		{
			// Units
			const CUnits& units = this->m_pView->GetDocument()->GetUnits();

			// GridKeyword
			CGridKeyword gridKeyword;
			this->m_pView->GetDocument()->GetGridKeyword(gridKeyword);

			// Scale
			const double* scale = pWellActor->GetScale();

			if (this->Transform)
			{
				this->Transform->Identity();
				if (pWellActor->GetWell().xy_coordinate_system_user == PHAST_Transform::MAP)
				{
					this->Transform->Scale(
						scale[0] * units.map_horizontal.input_to_si,
						scale[1] * units.map_horizontal.input_to_si,
						scale[2] * units.map_vertical.input_to_si);
					this->Transform->RotateZ(-gridKeyword.m_grid_angle);
					this->Transform->Translate(-gridKeyword.m_grid_origin[0], -gridKeyword.m_grid_origin[1], -gridKeyword.m_grid_origin[2]);
				}
				else
				{
					this->Transform->Scale(
						scale[0] * units.horizontal.input_to_si,
						scale[1] * units.horizontal.input_to_si,
						scale[2] * units.vertical.input_to_si);
				}
				this->Transform->Inverse();

				double wp[3];
				wp[0] = widget->GetPosition()[0];
				wp[1] = widget->GetPosition()[1];
				wp[2] = 0;
				this->Transform->TransformPoint(wp, wp);

				CWellSchedule well = pWellActor->GetWell();
				well.x_user = wp[0];
				well.y_user = wp[1];
				pWellActor->SetWell(well, units, gridKeyword);
			}
			else
			{
				ASSERT(FALSE);
			}

			// Update BoxPropertiesDialogBar
			//
			if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) )
			{
				// TODO for now just disable CBoxPropertiesDialogBar
				//pBar->Set(0, widget->GetProp3D(), this->m_pView->GetDocument()->GetUnits());
				pBar->Set(0, 0, this->m_pView->GetDocument()->GetUnits());
			}
		}
	}

	if (vtkBoxWidget *widget = vtkBoxWidget::SafeDownCast(caller))
	{
		// Apply transform
		//
		if (CZoneActor *zoneActor = CZoneActor::SafeDownCast(widget->GetProp3D()))
		{
			Polyhedron *poly = zoneActor->GetPolyhedron();
			if (Cube *cube = dynamic_cast<Cube*>(zoneActor->GetPolyhedron()))
			{
				vtkTransform *t = vtkTransform::New();
				widget->GetTransform(t);

				if (CGlobal::IsValidTransform(t))
				{
					widget->GetProp3D()->SetUserTransform(t);
				}
				t->Delete();
			}
		}

		// Update StatusBar
		//
		if (CWnd* pWnd = ((CFrameWnd*)::AfxGetMainWnd())->GetMessageBar())
		{
			// TODO this doesn't handle map coordinates
			//
// COMMENT: {1/12/2011 6:04:01 PM}			double bounds[6];
// COMMENT: {1/12/2011 6:04:01 PM}			widget->GetProp3D()->GetBounds(bounds);
// COMMENT: {1/12/2011 6:04:01 PM}			double* scale = widget->GetProp3D()->GetScale();
// COMMENT: {1/12/2011 6:04:01 PM}			TCHAR buffer[80];
// COMMENT: {1/12/2011 6:04:01 PM}			const CUnits& units = this->m_pView->GetDocument()->GetUnits();
// COMMENT: {1/12/2011 6:04:01 PM}			::_sntprintf(buffer, 80, "%g[%s] x %g[%s] x %g[%s])",
// COMMENT: {1/12/2011 6:04:01 PM}				fabs(bounds[1] - bounds[0]) / scale[0] / units.horizontal.input_to_si,
// COMMENT: {1/12/2011 6:04:01 PM}				units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
// COMMENT: {1/12/2011 6:04:01 PM}				fabs(bounds[3] - bounds[2]) / scale[1] / units.horizontal.input_to_si,
// COMMENT: {1/12/2011 6:04:01 PM}				units.horizontal.defined ? units.horizontal.input : units.horizontal.si,
// COMMENT: {1/12/2011 6:04:01 PM}				fabs(bounds[5] - bounds[4]) / scale[2] / units.vertical.input_to_si,
// COMMENT: {1/12/2011 6:04:01 PM}				units.vertical.defined ? units.vertical.input : units.vertical.si
// COMMENT: {1/12/2011 6:04:01 PM}				);
// COMMENT: {1/12/2011 6:04:01 PM}			TRACE("%s\n", buffer);
// COMMENT: {1/12/2011 6:04:01 PM}			pWnd->SetWindowText(buffer);
		}

		// Update BoxPropertiesDialogBar
		//
		if (CBoxPropertiesDialogBar* pBar = static_cast<CBoxPropertiesDialogBar*>(  ((CFrameWnd*)::AfxGetMainWnd())->GetControlBar(IDW_CONTROLBAR_BOXPROPS) ) )
		{
			// TODO for now just disable CBoxPropertiesDialogBar
			//pBar->Set(0, widget->GetProp3D(), this->m_pView->GetDocument()->GetUnits());
			pBar->Set(0, 0, this->m_pView->GetDocument()->GetUnits());
		}
	}
}

void CViewVTKCommand::Update2()
{
	// Modified from
	// int vtkPicker::Pick(float selectionX, float selectionY, float selectionZ,
	//                     vtkRenderer *renderer)
	int i;

	// get the position of the mouse cursor in screen/window coordinates
	// (pixel)
	vtkRenderer *renderer = this->m_pView->GetRenderer();
	int* pos = this->m_pView->GetInteractor()->GetEventPosition();

	// get the focal point in world coordinates
	//
	vtkCamera *camera = renderer->GetActiveCamera();
	double cameraFP[4];
	camera->GetFocalPoint(cameraFP); cameraFP[3] = 1.0;

	// Convert the focal point coordinates to display (or screen) coordinates.
	//
	renderer->SetWorldPoint(cameraFP[0], cameraFP[1], cameraFP[2], cameraFP[3]);
	renderer->WorldToDisplay();
	double displayCoords[4];
	renderer->GetDisplayPoint(displayCoords);

	// Convert the selection point into world coordinates.
	//
	renderer->SetDisplayPoint(pos[0], pos[1], displayCoords[2]);
	renderer->DisplayToWorld();
	double worldCoords[4];
	renderer->GetWorldPoint(worldCoords);
	if ( worldCoords[3] == 0.0 )
	{
		ASSERT(FALSE);
		return;
	}
	double PickPosition[3];
	for (i = 0; i < 3; ++i)
	{
		PickPosition[i] = worldCoords[i] / worldCoords[3];
	}

	double* bounds = this->m_pView->GetDocument()->GetGridBounds();
	double zMin = bounds[4];
	double zMax = bounds[5];
	double zPos = zMax;

	if ( camera->GetParallelProjection() )
	{
		double* cameraDOP = camera->GetDirectionOfProjection();
		// double t = -PickPosition[2] / cameraDOP[2];
		double t = (zPos - PickPosition[2]) / cameraDOP[2];
		for (i = 0; i < 2; ++i)
		{
			this->m_WorldPointXYPlane[i] = PickPosition[i] + t * cameraDOP[i];
		}
	}
	else
	{
		double *cameraPos = camera->GetPosition();
		// double t = -cameraPos[2] / ( PickPosition[2] - cameraPos[2] );
		double t = (zPos - cameraPos[2]) / ( PickPosition[2] - cameraPos[2] );
		for (i = 0; i < 2; ++i)
		{
			this->m_WorldPointXYPlane[i] = cameraPos[i] + t * ( PickPosition[i] - cameraPos[i] );
		}
	}
	this->m_WorldPointXYPlane[2] = zPos;

	// SCALE
	//
	double scale[3];
	scale[0] = this->m_pView->GetDocument()->GetScale()[0];
	scale[1] = this->m_pView->GetDocument()->GetScale()[1];
	scale[2] = this->m_pView->GetDocument()->GetScale()[2];

	// UNITS
	const CUnits& units = this->m_pView->GetDocument()->GetUnits();
	const char* xy_grid = units.horizontal.defined ? units.horizontal.input : units.horizontal.si;
	const char* z_grid = units.vertical.defined ? units.vertical.input : units.vertical.si;

	const char* xy_map = units.map_horizontal.defined ? units.map_horizontal.input : units.map_horizontal.si;
	const char* z_map = units.map_vertical.defined ? units.map_vertical.input : units.map_vertical.si;

	// determine most likely plane by finding
	// largest component vector
	//
	double max = 0.0;
	double viewPlaneNormal[3];
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
	CUtilities::GetWorldPointAtFixedPlane(this->m_pView->GetInteractor(), renderer, this->FixedCoord, bounds[this->FixedPlane], this->FixedPlanePoint);

	// Update grid coordinates
	//
	this->Transform->Identity();
	this->Transform->Scale(
		scale[0] * units.horizontal.input_to_si,
		scale[1] * units.horizontal.input_to_si,
		scale[2] * units.vertical.input_to_si);
	this->Transform->Inverse();

	this->Transform->TransformPoint(this->FixedPlanePoint, this->GridPoint);

	((CMainFrame*)::AfxGetMainWnd())->UpdateGrid(
		this->GridPoint[0],
		this->GridPoint[1],
		this->GridPoint[2],
		xy_grid,
		z_grid
		);


	// Update map coordinates
	//
	CGridKeyword gridk;
	this->m_pView->GetDocument()->GetGridKeyword(gridk);

	this->Transform->Identity();
	this->Transform->Scale(
		scale[0] * units.map_horizontal.input_to_si,
		scale[1] * units.map_horizontal.input_to_si,
		scale[2] * units.map_vertical.input_to_si);
	this->Transform->RotateZ(-gridk.m_grid_angle);
	this->Transform->Translate(-gridk.m_grid_origin[0], -gridk.m_grid_origin[1], -gridk.m_grid_origin[2]);
	this->Transform->Inverse();

	this->Transform->TransformPoint(this->FixedPlanePoint, this->MapPoint);

	((CMainFrame*)::AfxGetMainWnd())->UpdateMap(
		this->MapPoint[0],
		this->MapPoint[1],
		this->MapPoint[2],
		xy_map,
		z_map
		);
}

void CViewVTKCommand::ComputeDisplayToWorld(double x, double y, double z, double worldPt[4])
{
	CGlobal::ComputeDisplayToWorld(this->m_pView->GetRenderer(), x, y, z, worldPt);
}

void CViewVTKCommand::ComputeWorldToDisplay(double x, double y, double z, double displayPt[4])
{
	CGlobal::ComputeWorldToDisplay(this->m_pView->GetRenderer(), x, y, z, displayPt);
}

void CViewVTKCommand::OnLeftButtonPressEvent(vtkObject* caller, void* callData)
{
	TRACE("CViewVTKCommand::OnLeftButtonPressEvent\n");

	if (::GetAsyncKeyState(VK_SHIFT) < 0 && ::GetAsyncKeyState(VK_LBUTTON))
	{
		TRACE("Panning (early out of CViewVTKCommand::OnLeftButtonPressEvent)\n");
		return;
	}

	if (this->m_pView->CreatingNewWell())
	{
		this->m_pView->Cursor3DActor->SetPosition(this->m_WorldPointXYPlane[0], this->m_WorldPointXYPlane[1], this->m_WorldPointXYPlane[2]);
		this->m_pView->GetRenderer()->ResetCameraClippingRange();
		this->m_pView->GetInteractor()->Render();

		// stop forwarding to Interactor
		//
		this->SetAbortFlag(1);

		// update m_WorldPointXYPlane and save starting point
		//
		this->Update2();
		for (int i = 0; i < 4; ++i)
		{
			this->m_BeginPoint[i] = this->m_WorldPointXYPlane[i];
		}
	}
}

void CViewVTKCommand::OnLeftButtonReleaseEvent(vtkObject* caller, void* callData)
{
	TRACE("OnLeftButtonReleaseEvent\n");

	if (this->m_pView->CreatingNewWell())
	{
		double* bounds = this->m_pView->GetDocument()->GetGridBounds();
		double zMin = bounds[4];
		double zMax = bounds[5];

		// set radius
		//
		double defaultAxesSize = (bounds[1]-bounds[0] + bounds[3]-bounds[2] + bounds[5]-bounds[4])/12;
		this->m_pView->WellActor->SetDefaultTubeDiameter(defaultAxesSize * 0.17);

		// set scale
		//
		double* scale = this->m_pView->GetDocument()->GetScale();
		this->m_pView->WellActor->SetScale(scale);

		// set well
		//
		CWellSchedule well;
		well.n_user         = this->m_pView->GetDocument()->GetNextWellNumber();
		well.x_user_defined = TRUE;
		well.y_user_defined = TRUE;

		CGridKeyword gridKeyword;
		this->m_pView->GetDocument()->GetGridKeyword(gridKeyword);
		const CUnits& units = this->m_pView->GetDocument()->GetUnits();

		double pt[3];
		ASSERT(this->Transform);
		if (this->Transform)
		{
			this->Transform->Identity();
			if (this->m_pView->GetDocument()->GetCoordinateMode() == CWPhastDoc::MapMode)
			{
				this->Transform->Scale(
					scale[0] * units.map_horizontal.input_to_si,
					scale[1] * units.map_horizontal.input_to_si,
					scale[2] * units.map_vertical.input_to_si);
				this->Transform->RotateZ(-gridKeyword.m_grid_angle);
				this->Transform->Translate(-gridKeyword.m_grid_origin[0], -gridKeyword.m_grid_origin[1], -gridKeyword.m_grid_origin[2]);
				well.xy_coordinate_system_user = PHAST_Transform::MAP;
			}
			else
			{
				this->Transform->Scale(
					scale[0] * units.horizontal.input_to_si,
					scale[1] * units.horizontal.input_to_si,
					scale[2] * units.vertical.input_to_si);
				well.xy_coordinate_system_user = PHAST_Transform::GRID;
			}
			this->Transform->Inverse();
			this->Transform->TransformPoint(this->m_BeginPoint, pt);
		}

		well.x_user = pt[0];
		well.y_user = pt[1];
		this->m_pView->WellActor->SetWell(well, this->m_pView->GetDocument()->GetUnits(), this->m_pView->GetDocument()->GetGridKeyword());

		// set height
		//
		this->m_pView->WellActor->SetZAxis(gridKeyword.m_grid[2], this->m_pView->GetDocument()->GetUnits());
		this->m_pView->WellActor->VisibilityOn();

		// render
		//
		this->m_pView->GetRenderer()->AddViewProp(this->m_pView->WellActor);
		this->m_pView->GetRenderer()->Render();

		// display well properties
		//
		CPropertySheet sheet(_T("Define well properties"), 0);

		std::set<int> wellNums;
		this->m_pView->GetDocument()->GetUsedWellNumbers(wellNums);

		CWellPropertyPage page;
		page.SetProperties(well);
		page.SetGridKeyword(gridKeyword);
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
}

void CViewVTKCommand::OnMouseMoveEvent(vtkObject* caller, void* callData)
{
	Update2();

	if (this->m_pView->CreatingNewWell())
	{
		this->m_pView->Cursor3DActor->SetPosition(this->m_WorldPointXYPlane[0], this->m_WorldPointXYPlane[1], this->m_WorldPointXYPlane[2]);
		this->m_pView->GetRenderer()->ResetCameraClippingRange();
		this->m_pView->GetInteractor()->Render();
	}
}

void CViewVTKCommand::OnKeyPressEvent(vtkObject* caller, void* callData)
{
	char* keysym = this->m_pView->GetInteractor()->GetKeySym();

	if (this->m_pView->CreatingNewWell())
	{
		if (::_stricmp(keysym, "m") == 0)
		{
			ASSERT(FALSE); // Should now be caught by Accelerator
			this->m_pView->GetDocument()->SetMapMode();
		}
		if (::_stricmp(keysym, "g") == 0)
		{
			ASSERT(FALSE); // Should now be caught by Accelerator
			this->m_pView->GetDocument()->SetGridMode();
		}
	}

	if (::strcmp(keysym, "Escape") == 0)
	{
		ASSERT(FALSE); // Should now be caught by Accelerator
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
		caller->PrintSelf(oss, vtkIndent(4));
		oss << ends;
		///afxDump << oss.str() << "\n";
		TRACE("%s\n", oss.str());
		oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
	}
#endif

	if (vtkRenderer *renderer =  this->m_pView->GetRenderer())
	{
		double radius = CGlobal::ComputeRadius(renderer);
		TRACE("CViewVTKCommand::OnModifiedEvent radius = %g\n", radius);
		this->m_pView->SizeHandles(radius);
	}
}

void CViewVTKCommand::OnEndPickEvent(vtkObject* caller, void* callData)
{
	if (vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast( this->m_pView->GetInteractor()->GetPicker() ))
	{
		if (vtkAssemblyPath *path = picker->GetPath())
		{
#if defined(_DEBUG)
			ostrstream oss;
			path->PrintSelf(oss, vtkIndent(4));
			oss << ends;
			TRACE("\n");
			afxDump << "vtkAssemblyPath{{\n" << oss.str() << "vtkAssemblyPath}}\n";
			oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
#endif
			if (path->GetFirstNode()->GetViewProp()->IsA("vtkPropAssembly"))
			{
				TRACE("path->GetNumberOfItems() = %d\n", path->GetNumberOfItems());
				if (path->GetNumberOfItems() == 3)
				{
					// should be river Or CZoneActor
					//
					vtkCollectionSimpleIterator csi;
					path->InitTraversal(csi);
					vtkProp* pPropAssembly = path->GetNextNode(csi)->GetViewProp();
					ASSERT(pPropAssembly->IsA("vtkPropAssembly"));

					vtkProp* pActor = path->GetNextNode(csi)->GetViewProp();
					if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(pActor))
					{
						this->m_pView->GetDocument()->Select(pZoneActor);
					}
					if (CRiverActor *pRiver = CRiverActor::SafeDownCast(pActor))
					{
						this->m_pView->GetDocument()->Select(pRiver);
					}
					if (CDrainActor *pDrain = CDrainActor::SafeDownCast(pActor))
					{
						this->m_pView->GetDocument()->Select(pDrain);
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
						path->GetLastNode()->GetViewProp()->IsA("CZoneActor")
						||
						path->GetLastNode()->GetViewProp()->IsA("CWellActor")
						);

					// check if zone
					//
					if (CZoneActor *pZoneActor = CZoneActor::SafeDownCast(path->GetLastNode()->GetViewProp()))
					{
						this->m_pView->GetDocument()->Select(pZoneActor);
					}

					// check if well
					//
					if (CWellActor *pWell = CWellActor::SafeDownCast(path->GetLastNode()->GetViewProp()))
					{
						this->m_pView->GetDocument()->Select(pWell);
					}
				}
			}
			else
			{
				vtkProp *prop = path->GetFirstNode()->GetViewProp();

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
			int* xy = this->m_pView->GetInteractor()->GetEventPosition();
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
