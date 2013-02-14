#include "StdAfx.h"
#include "GridElementsSelector.h"

#include "GridActor.h"
#include "WPhastDoc.h"
#include "Utilities.h"


#if defined(USE_INTRINSIC)
#pragma intrinsic(fabs) // using this inlines fabs and is ~ 4x faster
#define FABS(x) fabs(x)
#else
#define FABS(x) ((x < 0) ? -x : x) // this is slightly slower than the intrinsic
#endif

vtkCxxRevisionMacro(CGridElementsSelector, "$Revision: 1.28 $");
vtkStandardNewMacro(CGridElementsSelector);

CGridElementsSelector::CGridElementsSelector(void)
: GridActor(0)
, Source(0)
, Mapper(0)
, Actor(0)
, OutlineSource(0)
, OutlineMapper(0)
, OutlineActor(0)
, State(CGridElementsSelector::Start)
, SelectElementIntersection(1)
{
	this->EventCallbackCommand->SetCallback(CGridElementsSelector::ProcessEvents);
	this->SetKeyPressActivation(0);

	// element selection
	//
	this->Source = vtkCubeSource::New();
	this->Mapper = vtkPolyDataMapper::New();
	this->Mapper->SetResolveCoincidentTopologyToPolygonOffset();

	this->Mapper->SetInput( this->Source->GetOutput() );
	this->Actor = vtkActor::New();
	this->Actor->SetMapper( this->Mapper );
	this->Actor->SetVisibility(0);
    this->Actor->GetProperty()->SetColor(1, 0, 0);

	// outline
	//
	this->OutlineSource = vtkOutlineSource::New();

	this->OutlineMapper = vtkPolyDataMapper::New();
	this->OutlineMapper->SetInput( this->OutlineSource->GetOutput() );

	this->OutlineActor = vtkActor::New();
	this->OutlineActor->SetMapper( this->OutlineMapper );
	this->OutlineActor->SetVisibility(0);
    this->OutlineActor->GetProperty()->SetColor(1, 0, 0);
}

CGridElementsSelector::~CGridElementsSelector(void)
{
	this->EventCallbackCommand->SetCallback(0);
	this->KeyPressCallbackCommand->SetCallback(0);

	if (this->CurrentRenderer)
	{
		this->CurrentRenderer->RemoveActor(this->Actor);
		this->CurrentRenderer->RemoveActor(this->OutlineActor);
		if (this->Interactor)
		{
			this->Interactor->Render();
		}
	}
	this->Source->Delete();
	this->Mapper->Delete();
	this->Actor->Delete();

	this->OutlineSource->Delete();
	this->OutlineMapper->Delete();
	this->OutlineActor->Delete();
	// may want to call this->SetEnabled(0);
}

void CGridElementsSelector::SetEnabled(int enabling)
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
			this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(
				this->Interactor->GetLastEventPosition()[0],
				this->Interactor->GetLastEventPosition()[1]));
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

		this->CurrentRenderer->AddActor(this->Actor);
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

		// don't listen for events any more
		this->Interactor->RemoveObserver(this->EventCallbackCommand);

		if (this->CurrentRenderer)
		{
			this->CurrentRenderer->RemoveActor(this->Actor);
			this->CurrentRenderer->RemoveActor(this->OutlineActor);
		}

		this->InvokeEvent(vtkCommand::DisableEvent, NULL);
		this->SetCurrentRenderer(NULL);
	}

	this->Interactor->Render();
}

void CGridElementsSelector::PlaceWidget(double bounds[6])
{
	// set bounds for the outline
	//
	this->OutlineSource->SetBounds(bounds);
	this->OutlineActor->SetVisibility(1);

	// set bounds for the highlighted elements
	//
	this->Source->SetBounds(bounds);
	this->Actor->SetVisibility(1);
}

void CGridElementsSelector::ProcessEvents(vtkObject* vtkNotUsed(object), 
								 unsigned long event,
								 void* clientdata, 
								 void* vtkNotUsed(calldata))
{
	CGridElementsSelector* self = reinterpret_cast<CGridElementsSelector *>( clientdata );

	if ( !self || !self->GridActor ) return;

	if ( !self->Interactor ) return;

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = self->Interactor->FindPokedRenderer( X, Y );
	if ( ren != self->CurrentRenderer )
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

void CGridElementsSelector::OnMouseMove(void)
{
	if ( this->State == CGridElementsSelector::Selecting )
	{
		double bounds[6];
		this->GridActor->GetBounds(bounds);

		double endPoint[3];
		CUtilities::GetWorldPointAtFixedPlane(this->Interactor, this->CurrentRenderer, this->FixedCoord, bounds[this->FixedPlane], endPoint);

		for (int i = 0; i < 3; ++i)
		{
			if (i != this->FixedCoord)
			{
				bounds[2 * i]     = min(this->StartPoint[i], endPoint[i]);
				bounds[2 * i + 1] = max(this->StartPoint[i], endPoint[i]);
			}
		}
		this->OutlineSource->SetBounds(bounds);

		this->GridActor->GetBounds(bounds);		
		double *scale = this->GridActor->GetScale();
		const CUnits& units = this->GridActor->GetUnits();
		for (int i = 0; i < 3; ++i)
		{
			if (i == this->FixedCoord)
			{
				this->Min[i] = 0;
				this->Max[i] = this->GridKeyword.m_grid[i].count_coord - 1;
			}
			else
			{
				if (this->SelectElementIntersection)
				{
					// select intersected elements
					if (i == 2)
					{
						this->Min[i] = this->GridKeyword.m_grid[i].upper_bound(this->OutlineSource->GetBounds()[2 * i] / scale[i] / units.vertical.input_to_si) - 1;
						if (this->Min[i] < 0)
						{
							this->Min[i] = 0;
						}
						this->Max[i] = this->GridKeyword.m_grid[i].lower_bound(this->OutlineSource->GetBounds()[2 * i + 1] / scale[i] / units.vertical.input_to_si);
					}
					else
					{
						this->Min[i] = this->GridKeyword.m_grid[i].upper_bound(this->OutlineSource->GetBounds()[2 * i] / scale[i] / units.horizontal.input_to_si) - 1;
						if (this->Min[i] < 0)
						{
							this->Min[i] = 0;
						}
						this->Max[i] = this->GridKeyword.m_grid[i].lower_bound(this->OutlineSource->GetBounds()[2 * i + 1] / scale[i] / units.horizontal.input_to_si);
					}
				}
				else
				{
					// only select interior elements
					if (i == 2)
					{
						this->Min[i] = this->GridKeyword.m_grid[i].lower_bound(this->OutlineSource->GetBounds()[2 * i] / scale[i] / units.vertical.input_to_si);
						this->Max[i] = this->GridKeyword.m_grid[i].upper_bound(this->OutlineSource->GetBounds()[2 * i + 1] / scale[i] / units.vertical.input_to_si) - 1;
					}
					else
					{
						this->Min[i] = this->GridKeyword.m_grid[i].lower_bound(this->OutlineSource->GetBounds()[2 * i] / scale[i] / units.horizontal.input_to_si);
						this->Max[i] = this->GridKeyword.m_grid[i].upper_bound(this->OutlineSource->GetBounds()[2 * i + 1] / scale[i] / units.horizontal.input_to_si) - 1;
					}
				}
				ASSERT( 0 <= this->Min[i] && this->Min[i] < this->GridKeyword.m_grid[i].count_coord );
				ASSERT( 0 <= this->Max[i] && this->Max[i] < this->GridKeyword.m_grid[i].count_coord );
				if (i == 2)
				{
					bounds[2 * i]     = this->GridKeyword.m_grid[i].coord[this->Min[i]] * scale[i] * units.vertical.input_to_si;
					bounds[2 * i + 1] = this->GridKeyword.m_grid[i].coord[this->Max[i]] * scale[i] * units.vertical.input_to_si;
				}
				else
				{
					bounds[2 * i]     = this->GridKeyword.m_grid[i].coord[this->Min[i]] * scale[i] * units.horizontal.input_to_si;
					bounds[2 * i + 1] = this->GridKeyword.m_grid[i].coord[this->Max[i]] * scale[i] * units.horizontal.input_to_si;
				}
			}
		}

		TRACE("CGridElementsSelector::OnMouseMove\n");
		TRACE("X (%d, %d) (%g, %g)\n", this->Min[0], this->Max[0], bounds[0], bounds[1]);
		TRACE("Y (%d, %d) (%g, %g)\n", this->Min[1], this->Max[1], bounds[2], bounds[3]);
		TRACE("Z (%d, %d) (%g, %g)\n", this->Min[2], this->Max[2], bounds[4], bounds[5]);

		this->Source->SetBounds(bounds);
		this->Actor->SetVisibility(1);

		this->EventCallbackCommand->SetAbortFlag(1);
		this->Interactor->Render();
	}
}

void CGridElementsSelector::OnLeftButtonDown(void)
{
	ASSERT ( this->State == CGridElementsSelector::Start );

	// set state
	//
	this->State = CGridElementsSelector::Selecting;

	// setup grids (for faster usage in mouse move)
	//
	this->GridKeyword = this->GridActor->m_gridKeyword;
	for (int i = 0; i < 3; ++i)
	{
		this->GridKeyword.m_grid[i].Setup();
	}

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


	// get point of intersection of axis=FixedCoord with a value of bounds[this->FixedPlane]
	// FixedPlane       FixedCoord
	//    0 => xmin        0 => x
	//    1 => xmax        1 => y
	//    2 => ymin        2 => z
	//    3 => ymax
	//    4 => zmin
	//    5 => zmax
	//
	double* bounds = this->GridActor->GetBounds();
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

	if (this->SelectElementIntersection)
	{
		// set bounds for the highlighted elements
		//
		double *scale = this->GridActor->GetScale();
		for (int i = 0; i < 3; ++i)
		{
			if (i == this->FixedCoord)
			{
				this->Min[i] = 0;
				this->Max[i] = this->GridKeyword.m_grid[i].count_coord - 1;
			}
			else
			{
				this->Min[i] = this->GridKeyword.m_grid[i].upper_bound(this->StartPoint[i] / scale[i]) - 1;
				if (this->Min[i] < 0)
				{
					this->Min[i] = 0;
				}
				this->Max[i] = this->GridKeyword.m_grid[i].lower_bound(this->StartPoint[i] / scale[i]);
			}
			ASSERT( 0 <= this->Min[i] && this->Min[i] < this->GridKeyword.m_grid[i].count_coord );
			ASSERT( 0 <= this->Max[i] && this->Max[i] < this->GridKeyword.m_grid[i].count_coord );
			bounds[2 * i]     = this->GridKeyword.m_grid[i].coord[this->Min[i]] * scale[i];
			bounds[2 * i + 1] = this->GridKeyword.m_grid[i].coord[this->Max[i]] * scale[i];
		}
		this->Source->SetBounds(bounds);
		this->Actor->SetVisibility(1);
	}

	// stop forwarding event
	//
	this->EventCallbackCommand->SetAbortFlag(1);
	this->Interactor->Render();
}

void CGridElementsSelector::OnLeftButtonUp(void)
{
	if ( this->State == CGridElementsSelector::Selecting )
	{
		this->State = CGridElementsSelector::Start;
		this->EventCallbackCommand->SetAbortFlag(1);
		this->OutlineActor->SetVisibility(0);
		this->Interactor->Render();
		TRACE("X(%d, %d)\n", this->Min[0], this->Max[0]);
		TRACE("Y(%d, %d)\n", this->Min[1], this->Max[1]);
		TRACE("Z(%d, %d)\n", this->Min[2], this->Max[2]);
		this->Document->ModifyGrid(this->GridActor, this);
	}
}

void CGridElementsSelector::SetIBounds(int imin, int imax, int jmin, int jmax, int kmin, int kmax)
{
	int ibounds[6];
	ibounds[0] = imin;
	ibounds[1] = imax;
	ibounds[2] = jmin;
	ibounds[3] = jmax;
	ibounds[4] = kmin;
	ibounds[5] = kmax;
	this->SetIBounds(ibounds);
}

void CGridElementsSelector::SetIBounds(int ibounds[6])
{
	const CUnits& units = this->GridActor->GetUnits();
	double *scale = this->GridActor->GetScale();
	double bounds[6];
	for (int i = 0; i < 3; ++i)
	{
		if (ibounds[2*i] < ibounds[2*i + 1])
		{
			this->Min[i] = ibounds[2*i];
			this->Max[i] = ibounds[2*i + 1];
		}
		else
		{
			this->Min[i] = ibounds[2*i + 1];
			this->Max[i] = ibounds[2*i];
		}
		if (this->Min[i] < 0)
		{
			this->Min[i] = 0;
		}
		if (this->GridKeyword.m_grid[i].count_coord <= this->Max[i])
		{
			this->Max[i] = this->GridKeyword.m_grid[i].count_coord - 1;
		}

		if (i == 2)
		{
			bounds[2 * i]     = this->GridKeyword.m_grid[i].coord[this->Min[i]] * scale[i] * units.vertical.input_to_si;
			bounds[2 * i + 1] = this->GridKeyword.m_grid[i].coord[this->Max[i]] * scale[i] * units.vertical.input_to_si;
		}
		else
		{
			bounds[2 * i]     = this->GridKeyword.m_grid[i].coord[this->Min[i]] * scale[i] * units.horizontal.input_to_si;
			bounds[2 * i + 1] = this->GridKeyword.m_grid[i].coord[this->Max[i]] * scale[i] * units.horizontal.input_to_si;
		}
	}
	this->PlaceWidget(bounds);
	if (this->Interactor) this->Interactor->Render();
}

void CGridElementsSelector::GetIBounds(int &imin, int &imax, int &jmin, int &jmax, int &kmin, int &kmax)
{
	imin = this->Min[0];
	imax = this->Max[0];
	jmin = this->Min[1];
	jmax = this->Max[1];
	kmin = this->Min[2];
	kmax = this->Max[2];
}

void CGridElementsSelector::GetIBounds(int ibounds[6])
{
	for (int i = 0; i < 3; ++i)
	{
		ibounds[2*i]     = this->Min[i];
		ibounds[2*i + 1] = this->Max[i];
	}
}

void CGridElementsSelector::UpdateGrid(void)
{
	this->GridKeyword = this->GridActor->m_gridKeyword;
	for (int i = 0; i < 3; ++i)
	{
		this->GridKeyword.m_grid[i].Setup();
	}
}
