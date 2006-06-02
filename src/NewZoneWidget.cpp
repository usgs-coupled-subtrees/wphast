#include "StdAfx.h"
#include "NewZoneWidget.h"

#include "GridActor.h"
#include "Utilities.h"
#include "Resource.h"  // IDC_NULL

#if defined(USE_INTRINSIC)
#pragma intrinsic(fabs) // using this inlines fabs and is ~ 4x faster
#define FABS(x) fabs(x)
#else
#define FABS(x) ((x < 0) ? -x : x) // this is slightly slower than the intrinsic
#endif

vtkCxxRevisionMacro(CNewZoneWidget, "$Revision$");
vtkStandardNewMacro(CNewZoneWidget);


CNewZoneWidget::CNewZoneWidget(void)
: State(CNewZoneWidget::Start)
, Cursor3D(0)
, Cursor3DMapper(0)
, Cursor3DActor(0)
, OutlineSource(0)
, OutlineMapper(0)
, OutlineActor(0)
{
	ASSERT(this->EventCallbackCommand);
	this->EventCallbackCommand->SetCallback(CNewZoneWidget::ProcessEvents);

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
	this->OutlineSource = vtkOutlineSource::New();

	this->OutlineMapper = vtkPolyDataMapper::New();
	this->OutlineMapper->SetInput( this->OutlineSource->GetOutput() );

	this->OutlineActor = vtkActor::New();
	this->OutlineActor->SetMapper( this->OutlineMapper );
	this->OutlineActor->SetVisibility(0);
    this->OutlineActor->GetProperty()->SetColor(1, 0, 0);
}

CNewZoneWidget::~CNewZoneWidget(void)
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

	this->OutlineSource->Delete();
	this->OutlineMapper->Delete();
	this->OutlineActor->Delete();
}

void CNewZoneWidget::SetEnabled(int enabling)
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

void CNewZoneWidget::PlaceWidget(vtkFloatingPointType bounds[6])
{
	// set bounds for the outline
	//
	this->OutlineSource->SetBounds(bounds);
	this->OutlineActor->SetVisibility(1);
}

void CNewZoneWidget::ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata)
{
	CNewZoneWidget* self = reinterpret_cast<CNewZoneWidget *>(clientdata);

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
	}
}

void CNewZoneWidget::OnMouseMove()
{
	TRACE("CNewZoneWidget::OnMouseMove\n");

	if (!this->CurrentRenderer) return;
	if (!this->Prop3D) return;

	// determine most likely plane by finding
	// largest component vector
	//
	double max = 0.0;
	vtkFloatingPointType viewPlaneNormal[3];
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

	///vtkFloatingPointType* bounds = this->GridActor->GetBounds();
	vtkFloatingPointType* bounds = this->Prop3D->GetBounds();
	CUtilities::GetWorldPointAtFixedPlane(this->Interactor, this->CurrentRenderer, this->FixedCoord, bounds[this->FixedPlane], this->FixedPlanePoint);

	vtkFloatingPointType dim = (bounds[1] - bounds[0]) / 20.0;
	this->Cursor3D->SetModelBounds(-dim, dim, -dim, dim, -dim, dim);

	this->Cursor3DActor->SetPosition(this->FixedPlanePoint);
	this->CurrentRenderer->ResetCameraClippingRange();

	if (this->State == CNewZoneWidget::Selecting)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (i != this->FixedCoord)
			{
				bounds[2 * i]     = min(this->StartPoint[i], this->FixedPlanePoint[i]);
				bounds[2 * i + 1] = max(this->StartPoint[i], this->FixedPlanePoint[i]);
			}
		}
		this->OutlineSource->SetBounds(bounds);
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
	}

	this->Interactor->Render();
}

void CNewZoneWidget::OnLeftButtonDown()
{
	TRACE("CNewZoneWidget::OnLeftButtonDown\n");

	ASSERT(this->State == CNewZoneWidget::Start);

	// set state
	//
	this->State = CNewZoneWidget::Selecting;

	// get point of intersection of axis=FixedCoord with a value of bounds[this->FixedPlane]
	// FixedPlane       FixedCoord
	//    0 => xmin        0 => x
	//    1 => xmax        1 => y
	//    2 => ymin        2 => z
	//    3 => ymax
	//    4 => zmin
	//    5 => zmax
	//
	vtkFloatingPointType* bounds = this->Prop3D->GetBounds();
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
	this->OutlineSource->SetBounds(bounds);
	this->OutlineActor->SetVisibility(1);


	// stop forwarding event
	//
	this->EventCallbackCommand->SetAbortFlag(1);
	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	this->Interactor->Render();
}

void CNewZoneWidget::OnLeftButtonUp()
{
	TRACE("CNewZoneWidget::OnLeftButtonUp\n");
	if ( this->State == CNewZoneWidget::Selecting )
	{
		vtkFloatingPointType* bounds = this->Prop3D->GetBounds();
		CUtilities::GetWorldPointAtFixedPlane(this->Interactor, this->CurrentRenderer, this->FixedCoord, bounds[this->FixedPlane], this->FixedPlanePoint);
		for (int i = 0; i < 3; ++i)
		{
			if (i != this->FixedCoord)
			{
				bounds[2 * i]     = min(this->StartPoint[i], this->FixedPlanePoint[i]);
				bounds[2 * i + 1] = max(this->StartPoint[i], this->FixedPlanePoint[i]);
			}
		}
		this->OutlineSource->SetBounds(bounds);

		// stop forwarding event
		//
		this->State = CNewZoneWidget::Start;
		this->EventCallbackCommand->SetAbortFlag(1);
		this->EndInteraction();
		this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
		this->Interactor->Render();
	}
}

#ifdef WIN32
BOOL CNewZoneWidget::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (this->Enabled)
	{
		::SetCursor(::AfxGetApp()->LoadCursor(IDC_NULL));
		return TRUE;
	}
	return FALSE;
}
#endif // WIN32
