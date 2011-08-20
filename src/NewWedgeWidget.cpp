#include "StdAfx.h"
#include "NewWedgeWidget.h"

#include "Global.h"    // CGlobal::ComputeRadius
#include "GridActor.h"
#include "Utilities.h"
#include "Resource.h"  // IDC_NULL
#include "srcWedgeSource.h"

#if defined(USE_INTRINSIC)
#pragma intrinsic(fabs) // using this inlines fabs and is ~ 4x faster
#define FABS(x) fabs(x)
#else
#define FABS(x) ((x < 0) ? -x : x) // this is slightly slower than the intrinsic
#endif

vtkCxxRevisionMacro(CNewWedgeWidget, "$Revision$");
vtkStandardNewMacro(CNewWedgeWidget);


CNewWedgeWidget::CNewWedgeWidget(void)
: State(CNewWedgeWidget::Start)
, Cursor3D(0)
, Cursor3DMapper(0)
, Cursor3DActor(0)
, WedgeSource(0)
, OutlineMapper(0)
, OutlineActor(0)
, GridAngle(0)
, CoordinateMode(CWPhastDoc::GridMode)
{
	for (int i = 0; i < 3; ++i)
	{
		this->GridOrigin[i]    = 0;
		this->GeometryScale[i] = 1;
	}

	ASSERT(this->EventCallbackCommand);
	this->EventCallbackCommand->SetCallback(CNewWedgeWidget::ProcessEvents);

	// 3D Cursor
	//
	this->Cursor3D = vtkCursor3D::New();
	this->Cursor3D->XShadowsOff();
	this->Cursor3D->YShadowsOff();
	this->Cursor3D->ZShadowsOff();
	this->Cursor3D->OutlineOff();

	this->Cursor3DMapper = vtkPolyDataMapper::New();
	this->Cursor3DMapper->SetInput(this->Cursor3D->GetOutput());
	
	this->Cursor3DActor = vtkActor::New();
	this->Cursor3DActor->SetMapper(this->Cursor3DMapper);
	this->Cursor3DActor->SetPosition(0, 0, 0);
	this->Cursor3DActor->GetProperty()->SetColor(1, 1, 1); // white

	// Outline
	//
	this->WedgeSource = srcWedgeSource::New();

	this->OutlineMapper = vtkPolyDataMapper::New();
	this->OutlineMapper->SetInput( this->WedgeSource->GetOutput() );

	this->OutlineActor = vtkActor::New();
	this->OutlineActor->SetMapper( this->OutlineMapper );
	this->OutlineActor->SetVisibility(0);
    this->OutlineActor->GetProperty()->SetColor(1, 0, 0);

	this->OutlineActor->GetProperty()->SetRepresentationToWireframe();
	this->OutlineActor->GetProperty()->SetAmbient(1.0); // this makes the back edges easier to see
}

CNewWedgeWidget::~CNewWedgeWidget(void)
{
	this->EventCallbackCommand->SetCallback(0);

	this->Cursor3DActor->SetVisibility(0);
	this->OutlineActor->SetVisibility(0);
	if (this->CurrentRenderer)
	{
		this->CurrentRenderer->RemoveActor(this->Cursor3DActor);
		this->CurrentRenderer->RemoveActor(this->OutlineActor);
		if (this->Interactor)
		{
			this->Interactor->Render();
		}
	}

	this->Cursor3D->Delete();
	this->Cursor3DMapper->Delete();
	this->Cursor3DActor->Delete();

	this->WedgeSource->Delete();
	this->OutlineMapper->Delete();
	this->OutlineActor->Delete();
}

void CNewWedgeWidget::SetEnabled(int enabling)
{
	if ( ! this->Interactor )
	{
		vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
		return;
	}

	if ( enabling ) //------------------------------------------------------------
	{
		vtkDebugMacro(<<"Enabling widget");

		if ( this->Enabled ) //already enabled, just return
		{
			return;
		}

		if ( ! this->CurrentRenderer )
		{
			this->CurrentRenderer = this->Interactor->FindPokedRenderer(
				this->Interactor->GetLastEventPosition()[0],
				this->Interactor->GetLastEventPosition()[1]);
			if (this->CurrentRenderer == NULL)
			{
				return;
			}
		}

		this->Enabled = 1;

		// listen to the following events
		vtkRenderWindowInteractor *i = this->Interactor;
		i->AddObserver(vtkCommand::MouseMoveEvent,
            this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::LeftButtonPressEvent, 
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::LeftButtonReleaseEvent, 
			this->EventCallbackCommand, this->Priority);
		//{{
		i->AddObserver(vtkCommand::KeyPressEvent, 
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::KeyReleaseEvent, 
			this->EventCallbackCommand, this->Priority);
		//}}


		this->CurrentRenderer->AddActor(this->Cursor3DActor);
		this->CurrentRenderer->AddActor(this->OutlineActor);

		this->InvokeEvent(vtkCommand::EnableEvent, NULL);

	}

	else //disabling-------------------------------------------------------------
	{
		vtkDebugMacro(<<"Disabling widget");

		if ( ! this->Enabled ) //already disabled, just return
		{
			return;
		}

		this->Enabled = 0;

		if (this->CurrentRenderer)
		{
			this->CurrentRenderer->RemoveActor(this->Cursor3DActor);
			this->CurrentRenderer->RemoveActor(this->OutlineActor);
		}

		// don't listen for events any more
		this->Interactor->RemoveObserver(this->EventCallbackCommand);
		this->InvokeEvent(vtkCommand::DisableEvent, NULL);
	}

	this->Interactor->Render();
}

void CNewWedgeWidget::PlaceWidget(double bounds[6])
{
	// set bounds for the outline
	//
	this->WedgeSource->SetBounds(bounds);
	this->OutlineActor->SetVisibility(1);
}

void CNewWedgeWidget::ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata)
{
	CNewWedgeWidget* self = reinterpret_cast<CNewWedgeWidget *>(clientdata);

	if (!self || !self->Prop3D || !self->Interactor) return;

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = self->Interactor->FindPokedRenderer(X, Y);
	if (ren != self->CurrentRenderer)
	{
		return;
	}

	switch(event)
	{
	case vtkCommand::LeftButtonPressEvent:
		self->OnLeftButtonDown();
		break;
	case vtkCommand::LeftButtonReleaseEvent:
		self->OnLeftButtonUp();
		break;
	case vtkCommand::MouseMoveEvent:
		self->OnMouseMove();
		break;
	case vtkCommand::KeyPressEvent:
		self->OnMouseMove();
		break;
	case vtkCommand::KeyReleaseEvent:
		self->OnMouseMove();
		break;
	}
}

void CNewWedgeWidget::OnMouseMove()
{
	TRACE("CNewWedgeWidget::OnMouseMove\n");

	if (!this->CurrentRenderer) return;
	if (!this->Prop3D) return;

	// determine most likely plane by finding
	// largest component vector
	//
	double max = 0.0;
	double viewPlaneNormal[3];
	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	camera->GetViewPlaneNormal(viewPlaneNormal);
	for (int i = 0; i < 3; ++i)
	{
		// Note: fabs() is ~4x slower
		//
		if (max < FABS(viewPlaneNormal[i]))
		{
			this->FixedCoord = i;
			if (viewPlaneNormal[i] < 0.0)
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

	double bounds[6];
	this->Prop3D->GetBounds(bounds);
	CUtilities::GetWorldPointAtFixedPlane(this->Interactor, this->CurrentRenderer, this->FixedCoord, bounds[this->FixedPlane], this->FixedPlanePoint);

	double dim = 0.016 * CGlobal::ComputeRadius(this->CurrentRenderer);
	this->Cursor3D->SetModelBounds(-dim, dim, -dim, dim, -dim, dim);

	this->Cursor3DActor->SetPosition(this->FixedPlanePoint);
	this->CurrentRenderer->ResetCameraClippingRange();

	if (this->State == CNewWedgeWidget::Selecting)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (i != this->FixedCoord)
			{
				bounds[2 * i]     = min(this->StartPoint[i], this->FixedPlanePoint[i]);
				bounds[2 * i + 1] = max(this->StartPoint[i], this->FixedPlanePoint[i]);
			}
		}

		// world to grid
		//
		vtkTransform *grid2world = vtkTransform::New();
		grid2world->Scale(
			this->GeometryScale[0],
			this->GeometryScale[1],
			this->GeometryScale[2]
			);

		double pt1[3];
		grid2world->GetInverse()->TransformPoint(this->StartPoint, pt1);

		double pt2[3];
		grid2world->GetInverse()->TransformPoint(this->FixedPlanePoint, pt2);

		// grid to map
		//
		vtkTransform *map2grid = vtkTransform::New();
		map2grid->Scale(
			this->Units.map_horizontal.input_to_si,
			this->Units.map_horizontal.input_to_si,
			this->Units.map_vertical.input_to_si
			);
		map2grid->RotateZ(-this->GridAngle);
		map2grid->Translate(-this->GridOrigin[0], -this->GridOrigin[1], -this->GridOrigin[2]);

		map2grid->GetInverse()->TransformPoint(pt1, pt1);
		map2grid->GetInverse()->TransformPoint(pt2, pt2);

		double min[3];
		double max[3];
		for (int i = 0; i < 3; ++i)
		{
			if (i == this->FixedCoord)
			{
				// world (scaled meters) to map
				//
				double pt[3] = {0, 0, 0};

				pt[i] = min(bounds[2 * i], bounds[2 * i + 1]);
				grid2world->GetInverse()->TransformPoint(pt, pt);
				map2grid->GetInverse()->TransformPoint(pt, pt);
				min[i] = pt[i];

				pt[i] = max(bounds[2 * i], bounds[2 * i + 1]);
				grid2world->GetInverse()->TransformPoint(pt, pt);
				map2grid->GetInverse()->TransformPoint(pt, pt);
				max[i] = pt[i];

			}
			else
			{
				min[i] = min(pt1[i], pt2[i]);
				max[i] = max(pt1[i], pt2[i]);
			}
		}
		this->MapZone.x1 = min[0];
		this->MapZone.x2 = max[0];
		this->MapZone.y1 = min[1];
		this->MapZone.y2 = max[1];
		this->MapZone.z1 = min[2];
		this->MapZone.z2 = max[2];

		// world to map
		double mapStartPoint[3];
		double mapFixedPlanePoint[3];

		if (this->CoordinateMode == CWPhastDoc::MapMode)
		{
			bounds[0] = min[0];
			bounds[1] = max[0];
			bounds[2] = min[1];
			bounds[3] = max[1];
			bounds[4] = min[2];
			bounds[5] = max[2];

			// world to map
			//
			grid2world->GetInverse()->TransformPoint(this->StartPoint, mapStartPoint);
			map2grid->GetInverse()->TransformPoint(mapStartPoint, mapStartPoint);

			grid2world->GetInverse()->TransformPoint(this->FixedPlanePoint, mapFixedPlanePoint);
			map2grid->GetInverse()->TransformPoint(mapFixedPlanePoint, mapFixedPlanePoint);
		}
		else if (this->CoordinateMode == CWPhastDoc::GridMode)
		{
			for (int i = 0; i < 3; ++i)
			{
				mapStartPoint[i]      = this->StartPoint[i];
				mapFixedPlanePoint[i] = this->FixedPlanePoint[i];
			}
		}

		grid2world->Delete();
		map2grid->Delete();

		enum srcWedgeSource::tagChopType t;
		bool flip = (this->Interactor->GetShiftKey() != 0);
		if (this->FixedCoord == 0)
		{
/*
            *                         *
            ***                     ***
            *****                 *****
            *******             *******
            *********         *********
            ***********     ***********
            ************* *************
            ************* *************
            ***********     ***********
            *********         *********
            *******             *******
            *****                 *****
            ***                     ***
            *                         *
*/
			TRACE("FixedCoord == X-axis\n");
			if (mapStartPoint[1] < mapFixedPlanePoint[1])
			{
				if (mapStartPoint[2] < mapFixedPlanePoint[2])
				{
					// +y+z
					t = flip ? srcWedgeSource::CHOP_MIN_Y_MAX_Z : srcWedgeSource::CHOP_MAX_Y_MIN_Z;
				}
				else
				{
					// +y-z
					t = flip ? srcWedgeSource::CHOP_MIN_Y_MIN_Z : srcWedgeSource::CHOP_MAX_Y_MAX_Z;
				}
			}
			else
			{
				if (mapStartPoint[2] < mapFixedPlanePoint[2])
				{
					// -y+z
					t = flip ? srcWedgeSource::CHOP_MAX_Y_MAX_Z : srcWedgeSource::CHOP_MIN_Y_MIN_Z;
				}
				else
				{
					// -y-z
					t = flip ? srcWedgeSource::CHOP_MAX_Y_MIN_Z : srcWedgeSource::CHOP_MIN_Y_MAX_Z;
				}
			}
		}
		else if (this->FixedCoord == 1)
		{
			TRACE("FixedCoord == Y-axis\n");
			if (mapStartPoint[0] < mapFixedPlanePoint[0])
			{
				if (mapStartPoint[2] < mapFixedPlanePoint[2])
				{
					// +x+z
					t = flip ? srcWedgeSource::CHOP_MAX_X_MIN_Z : srcWedgeSource::CHOP_MIN_X_MAX_Z;
				}
				else
				{
					// +x-z
					t = flip ? srcWedgeSource::CHOP_MAX_X_MAX_Z : srcWedgeSource::CHOP_MIN_X_MIN_Z;
				}
			}
			else
			{
				if (mapStartPoint[2] < mapFixedPlanePoint[2])
				{
					// -x+z
					t = flip ? srcWedgeSource::CHOP_MIN_X_MIN_Z : srcWedgeSource::CHOP_MAX_X_MAX_Z;
				}
				else
				{
					// -x-z
					t = flip ? srcWedgeSource::CHOP_MIN_X_MAX_Z : srcWedgeSource::CHOP_MAX_X_MIN_Z;
				}
			}
			this->WedgeSource->SetChopType(t);
		}
		else if (this->FixedCoord == 2)
		{
			TRACE("FixedCoord == Z-axis\n");
			if (mapStartPoint[0] < mapFixedPlanePoint[0])
			{
				if (mapStartPoint[1] < mapFixedPlanePoint[1])
				{
					// +x+y
					t = flip ? srcWedgeSource::CHOP_MAX_X_MIN_Y : srcWedgeSource::CHOP_MIN_X_MAX_Y;
				}
				else
				{
					// +x-y
					t = flip ? srcWedgeSource::CHOP_MAX_X_MAX_Y : srcWedgeSource::CHOP_MIN_X_MIN_Y;
				}
			}
			else
			{
				if (mapStartPoint[1] < mapFixedPlanePoint[1])
				{
					// -x+y
					t = flip ? srcWedgeSource::CHOP_MIN_X_MIN_Y : srcWedgeSource::CHOP_MAX_X_MAX_Y;
				}
				else
				{
					// -x-y
					t = flip ? srcWedgeSource::CHOP_MIN_X_MAX_Y : srcWedgeSource::CHOP_MAX_X_MIN_Y;
				}
			}
		}

		this->WedgeSource->SetChopType(t);
		this->WedgeSource->SetBounds(bounds);

		// validate actor
		ASSERT(this->OutlineActor->GetPosition()[0]         == 0.0 && this->OutlineActor->GetPosition()[1]         == 0.0 && this->OutlineActor->GetPosition()[2]         == 0.0);
		ASSERT(this->OutlineActor->GetOrigin()[0]           == 0.0 && this->OutlineActor->GetOrigin()[1]           == 0.0 && this->OutlineActor->GetOrigin()[2]           == 0.0);
		ASSERT(this->OutlineActor->GetOrientation()[0]      == 0.0 && this->OutlineActor->GetOrientation()[1]      == 0.0 && this->OutlineActor->GetOrientation()[2]      == 0.0);
		ASSERT(this->OutlineActor->vtkProp3D::GetScale()[0] == 1.0 && this->OutlineActor->vtkProp3D::GetScale()[1] == 1.0 && this->OutlineActor->vtkProp3D::GetScale()[2] == 1.0);

		if (this->CoordinateMode == CWPhastDoc::MapMode)
		{
			vtkTransform *user = vtkTransform::New();
			user->Identity();
			user->Scale(
				this->GeometryScale[0] * this->Units.map_horizontal.input_to_si,
				this->GeometryScale[1] * this->Units.map_horizontal.input_to_si,
				this->GeometryScale[2] * this->Units.map_vertical.input_to_si);
			user->RotateZ(-this->GridAngle);
			user->Translate(-this->GridOrigin[0], -this->GridOrigin[1], -this->GridOrigin[2]);
			this->OutlineActor->SetUserTransform(user);
			user->Delete();
		}
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
	}

	this->Interactor->Render();
}

void CNewWedgeWidget::OnLeftButtonDown()
{
	TRACE("CNewWedgeWidget::OnLeftButtonDown\n");

	if (this->State != CNewWedgeWidget::Start)
	{
		// sticky keys hack
		TRACE("Warning: (this->State != CNewWedgeWidget::Start) in CNewWedgeWidget::OnLeftButtonDown\n");
		this->EventCallbackCommand->SetAbortFlag(1);
		return; 
	}
	ASSERT(this->State == CNewWedgeWidget::Start);

	// set state
	//
	this->State = CNewWedgeWidget::Selecting;

	// get point of intersection of axis=FixedCoord with a value of bounds[this->FixedPlane]
	// FixedPlane       FixedCoord
	//    0 => xmin        0 => x
	//    1 => xmax        1 => y
	//    2 => ymin        2 => z
	//    3 => ymax
	//    4 => zmin
	//    5 => zmax
	//
	double bounds[6];
	this->Prop3D->GetBounds(bounds);
	CUtilities::GetWorldPointAtFixedPlane(this->Interactor, this->CurrentRenderer, this->FixedCoord, bounds[this->FixedPlane], this->StartPoint);

	// set bounds for the outline
	//
	for (int i = 0; i < 3; ++i)
	{
		if (i != this->FixedCoord)
		{
			bounds[2 * i]     = this->StartPoint[i];
			bounds[2 * i + 1] = this->StartPoint[i];
		}
	}
	this->WedgeSource->SetBounds(bounds);
	this->OutlineActor->SetVisibility(1);


	// stop forwarding event
	//
	this->EventCallbackCommand->SetAbortFlag(1);
	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	this->Interactor->Render();
}

void CNewWedgeWidget::OnLeftButtonUp()
{
	TRACE("CNewWedgeWidget::OnLeftButtonUp\n");
	if ( this->State == CNewWedgeWidget::Selecting )
	{
		double bounds[6];
		this->Prop3D->GetBounds(bounds);
		CUtilities::GetWorldPointAtFixedPlane(this->Interactor, this->CurrentRenderer, this->FixedCoord, bounds[this->FixedPlane], this->FixedPlanePoint);
		for (int i = 0; i < 3; ++i)
		{
			if (i != this->FixedCoord)
			{
				bounds[2 * i]     = min(this->StartPoint[i], this->FixedPlanePoint[i]);
				bounds[2 * i + 1] = max(this->StartPoint[i], this->FixedPlanePoint[i]);
			}
		}

		// world to grid
		//
		vtkTransform *grid2world = vtkTransform::New();
		grid2world->Scale(
			this->GeometryScale[0],
			this->GeometryScale[1],
			this->GeometryScale[2]
			);

		double pt1[3];
		grid2world->GetInverse()->TransformPoint(this->StartPoint, pt1);

		double pt2[3];
		grid2world->GetInverse()->TransformPoint(this->FixedPlanePoint, pt2);

		// grid to map
		//
		vtkTransform *map2grid = vtkTransform::New();
		map2grid->Scale(
			this->Units.map_horizontal.input_to_si,
			this->Units.map_horizontal.input_to_si,
			this->Units.map_vertical.input_to_si
			);
		map2grid->RotateZ(-this->GridAngle);
		map2grid->Translate(-this->GridOrigin[0], -this->GridOrigin[1], -this->GridOrigin[2]);

		map2grid->GetInverse()->TransformPoint(pt1, pt1);
		map2grid->GetInverse()->TransformPoint(pt2, pt2);

		double min[3];
		double max[3];
		for (int i = 0; i < 3; ++i)
		{
			if (i == this->FixedCoord)
			{
				// world (scaled meters) to map
				//
				double pt[3] = {0, 0, 0};

				pt[i] = min(bounds[2 * i], bounds[2 * i + 1]);
				grid2world->GetInverse()->TransformPoint(pt, pt);
				map2grid->GetInverse()->TransformPoint(pt, pt);
				min[i] = pt[i];

				pt[i] = max(bounds[2 * i], bounds[2 * i + 1]);
				grid2world->GetInverse()->TransformPoint(pt, pt);
				map2grid->GetInverse()->TransformPoint(pt, pt);
				max[i] = pt[i];

			}
			else
			{
				min[i] = min(pt1[i], pt2[i]);
				max[i] = max(pt1[i], pt2[i]);
			}
		}
		this->MapZone.x1 = min[0];
		this->MapZone.x2 = max[0];
		this->MapZone.y1 = min[1];
		this->MapZone.y2 = max[1];
		this->MapZone.z1 = min[2];
		this->MapZone.z2 = max[2];

		// world to map
		double mapStartPoint[3];
		double mapFixedPlanePoint[3];

		if (this->CoordinateMode == CWPhastDoc::MapMode)
		{
			bounds[0] = min[0];
			bounds[1] = max[0];
			bounds[2] = min[1];
			bounds[3] = max[1];
			bounds[4] = min[2];
			bounds[5] = max[2];

			// world to map
			//
			grid2world->GetInverse()->TransformPoint(this->StartPoint, mapStartPoint);
			map2grid->GetInverse()->TransformPoint(mapStartPoint, mapStartPoint);

			grid2world->GetInverse()->TransformPoint(this->FixedPlanePoint, mapFixedPlanePoint);
			map2grid->GetInverse()->TransformPoint(mapFixedPlanePoint, mapFixedPlanePoint);
		}
		else if (this->CoordinateMode == CWPhastDoc::GridMode)
		{
			for (int i = 0; i < 3; ++i)
			{
				mapStartPoint[i]      = this->StartPoint[i];
				mapFixedPlanePoint[i] = this->FixedPlanePoint[i];
			}
		}

		grid2world->Delete();
		map2grid->Delete();

		enum srcWedgeSource::tagChopType t;
		bool flip = (this->Interactor->GetShiftKey() != 0);
		if (this->FixedCoord == 0)
		{
/*
            *                         *
            ***                     ***
            *****                 *****
            *******             *******
            *********         *********
            ***********     ***********
            ************* *************
            ************* *************
            ***********     ***********
            *********         *********
            *******             *******
            *****                 *****
            ***                     ***
            *                         *
*/
			TRACE("FixedCoord == X-axis\n");
			if (mapStartPoint[1] < mapFixedPlanePoint[1])
			{
				if (mapStartPoint[2] < mapFixedPlanePoint[2])
				{
					// +y+z
					t = flip ? srcWedgeSource::CHOP_MIN_Y_MAX_Z : srcWedgeSource::CHOP_MAX_Y_MIN_Z;
				}
				else
				{
					// +y-z
					t = flip ? srcWedgeSource::CHOP_MIN_Y_MIN_Z : srcWedgeSource::CHOP_MAX_Y_MAX_Z;
				}
			}
			else
			{
				if (mapStartPoint[2] < mapFixedPlanePoint[2])
				{
					// -y+z
					t = flip ? srcWedgeSource::CHOP_MAX_Y_MAX_Z : srcWedgeSource::CHOP_MIN_Y_MIN_Z;
				}
				else
				{
					// -y-z
					t = flip ? srcWedgeSource::CHOP_MAX_Y_MIN_Z : srcWedgeSource::CHOP_MIN_Y_MAX_Z;
				}
			}
		}
		else if (this->FixedCoord == 1)
		{
			TRACE("FixedCoord == Y-axis\n");
			if (mapStartPoint[0] < mapFixedPlanePoint[0])
			{
				if (mapStartPoint[2] < mapFixedPlanePoint[2])
				{
					// +x+z
					t = flip ? srcWedgeSource::CHOP_MAX_X_MIN_Z : srcWedgeSource::CHOP_MIN_X_MAX_Z;
				}
				else
				{
					// +x-z
					t = flip ? srcWedgeSource::CHOP_MAX_X_MAX_Z : srcWedgeSource::CHOP_MIN_X_MIN_Z;
				}
			}
			else
			{
				if (mapStartPoint[2] < mapFixedPlanePoint[2])
				{
					// -x+z
					t = flip ? srcWedgeSource::CHOP_MIN_X_MIN_Z : srcWedgeSource::CHOP_MAX_X_MAX_Z;
				}
				else
				{
					// -x-z
					t = flip ? srcWedgeSource::CHOP_MIN_X_MAX_Z : srcWedgeSource::CHOP_MAX_X_MIN_Z;
				}
			}
			this->WedgeSource->SetChopType(t);
		}
		else if (this->FixedCoord == 2)
		{
			TRACE("FixedCoord == Z-axis\n");
			if (mapStartPoint[0] < mapFixedPlanePoint[0])
			{
				if (mapStartPoint[1] < mapFixedPlanePoint[1])
				{
					// +x+y
					t = flip ? srcWedgeSource::CHOP_MAX_X_MIN_Y : srcWedgeSource::CHOP_MIN_X_MAX_Y;
				}
				else
				{
					// +x-y
					t = flip ? srcWedgeSource::CHOP_MAX_X_MAX_Y : srcWedgeSource::CHOP_MIN_X_MIN_Y;
				}
			}
			else
			{
				if (mapStartPoint[1] < mapFixedPlanePoint[1])
				{
					// -x+y
					t = flip ? srcWedgeSource::CHOP_MIN_X_MIN_Y : srcWedgeSource::CHOP_MAX_X_MAX_Y;
				}
				else
				{
					// -x-y
					t = flip ? srcWedgeSource::CHOP_MIN_X_MAX_Y : srcWedgeSource::CHOP_MAX_X_MIN_Y;
				}
			}
		}

		this->WedgeSource->SetChopType(t);
		this->WedgeSource->SetBounds(bounds);

		// validate actor
		ASSERT(this->OutlineActor->GetPosition()[0]         == 0.0 && this->OutlineActor->GetPosition()[1]         == 0.0 && this->OutlineActor->GetPosition()[2]         == 0.0);
		ASSERT(this->OutlineActor->GetOrigin()[0]           == 0.0 && this->OutlineActor->GetOrigin()[1]           == 0.0 && this->OutlineActor->GetOrigin()[2]           == 0.0);
		ASSERT(this->OutlineActor->GetOrientation()[0]      == 0.0 && this->OutlineActor->GetOrientation()[1]      == 0.0 && this->OutlineActor->GetOrientation()[2]      == 0.0);
		ASSERT(this->OutlineActor->vtkProp3D::GetScale()[0] == 1.0 && this->OutlineActor->vtkProp3D::GetScale()[1] == 1.0 && this->OutlineActor->vtkProp3D::GetScale()[2] == 1.0);

		vtkTransform *user = vtkTransform::New();
		user->Identity();

		if (this->CoordinateMode == CWPhastDoc::MapMode)
		{
			vtkTransform *user = vtkTransform::New();
			user->Identity();
			user->Scale(
				this->GeometryScale[0] * this->Units.map_horizontal.input_to_si,
				this->GeometryScale[1] * this->Units.map_horizontal.input_to_si,
				this->GeometryScale[2] * this->Units.map_vertical.input_to_si);
			user->RotateZ(-this->GridAngle);
			user->Translate(-this->GridOrigin[0], -this->GridOrigin[1], -this->GridOrigin[2]);
			this->OutlineActor->SetUserTransform(user);
			user->Delete();
		}

		// stop forwarding event
		//
		this->State = CNewWedgeWidget::Start;
		this->EventCallbackCommand->SetAbortFlag(1);
		this->EndInteraction();
		this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
		if (this->Interactor)
		{
			this->Interactor->Render();
		}
	}
}

#ifdef WIN32
BOOL CNewWedgeWidget::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (this->Enabled)
	{
		::SetCursor(::AfxGetApp()->LoadCursor(IDC_NULL));
		return TRUE;
	}
	return FALSE;
}
#endif // WIN32

enum srcWedgeSource::tagChopType CNewWedgeWidget::GetChopType()const
{
	return this->WedgeSource->GetChopType();
}

void CNewWedgeWidget::SetChopType(enum srcWedgeSource::tagChopType t)
{
	this->WedgeSource->SetChopType(t);
}

void CNewWedgeWidget::SetGridKeyword(const CGridKeyword& gridKeyword, const CUnits& units)
{
	// set up for transform
	this->GridAngle     = gridKeyword.m_grid_angle;
	this->GridOrigin[0] = gridKeyword.m_grid_origin[0];
	this->GridOrigin[1] = gridKeyword.m_grid_origin[1];
	this->GridOrigin[2] = gridKeyword.m_grid_origin[2];
	this->Units         = units;
}

void CNewWedgeWidget::SetScale(double x, double y, double z)
{
	this->GeometryScale[0] = x;
	this->GeometryScale[1] = y;
	this->GeometryScale[2] = z;
}

void CNewWedgeWidget::SetCoordinateMode(CWPhastDoc::CoordinateState mode)
{
	this->CoordinateMode = mode;

	if (mode == CWPhastDoc::GridMode)
	{
		this->Cursor3DActor->SetOrientation(0, 0, 0);
	}
	else if (mode == CWPhastDoc::MapMode)
	{
		this->Cursor3DActor->SetOrientation(0, 0, -this->GridAngle);
	}
	else
	{
		ASSERT(FALSE);
	}
}

Wedge* CNewWedgeWidget::GetWedge()
{
	if (this->CoordinateMode == CWPhastDoc::GridMode)
	{
		double scaled_meters[6];
		this->GetBounds(scaled_meters);

		// calc zone
		CZone zone;
		zone.x1 = scaled_meters[0] / this->GeometryScale[0] / this->Units.horizontal.input_to_si;
		zone.x2 = scaled_meters[1] / this->GeometryScale[0] / this->Units.horizontal.input_to_si;
		zone.y1 = scaled_meters[2] / this->GeometryScale[1] / this->Units.horizontal.input_to_si;
		zone.y2 = scaled_meters[3] / this->GeometryScale[1] / this->Units.horizontal.input_to_si;
		zone.z1 = scaled_meters[4] / this->GeometryScale[2] / this->Units.vertical.input_to_si;
		zone.z2 = scaled_meters[5] / this->GeometryScale[2] / this->Units.vertical.input_to_si;

		TRACE("x(%g-%g) y(%g-%g) z(%g-%g)\n", zone.x1, zone.x2, zone.y1, zone.y2, zone.z1, zone.z2);
		return new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(this->GetChopType()), PHAST_Transform::GRID);
	}
	else if (this->CoordinateMode == CWPhastDoc::MapMode)
	{
		TRACE("x(%g-%g) y(%g-%g) z(%g-%g)\n", this->MapZone.x1, this->MapZone.x2, this->MapZone.y1, this->MapZone.y2, this->MapZone.z1, this->MapZone.z2);
		return new Wedge(&this->MapZone, srcWedgeSource::GetWedgeOrientationString(this->GetChopType()), PHAST_Transform::MAP);
	}
	else
	{
		ASSERT(FALSE);
		CZone zone;
		return new Wedge(&zone, srcWedgeSource::GetWedgeOrientationString(this->GetChopType()), PHAST_Transform::GRID);
	}
}

void CNewWedgeWidget::SetCursorColor(double r, double g, double b)
{
	this->Cursor3DActor->GetProperty()->SetColor(r, g, b);
}