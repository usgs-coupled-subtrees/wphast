#include "StdAfx.h"
#include "GridLineSelector.h"

#include "GridActor.h"

#if defined(USE_INTRINSIC)
#pragma intrinsic(fabs) // using this inlines fabs and is ~ 4x faster
#define FABS(x) fabs(x)
#else
#define FABS(x) ((x < 0) ? -x : x) // this is slightly slower than the intrinsic
#endif

vtkCxxRevisionMacro(CGridLineSelector, "$Revision: 1.28 $");
vtkStandardNewMacro(CGridLineSelector);

CGridLineSelector::CGridLineSelector(void)
: GridActor(0)
, Source(0)
, Mapper(0)
, Actor(0)
, OutlineSource(0)
, OutlineMapper(0)
, OutlineActor(0)
, State(CGridLineSelector::Start)
, bSelectElementIntersection(false)
{
	this->EventCallbackCommand->SetCallback(CGridLineSelector::ProcessEvents);
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

CGridLineSelector::~CGridLineSelector(void)
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
}

void CGridLineSelector::SetEnabled(int enabling)
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
		i->AddObserver(vtkCommand::MouseMoveEvent, this->EventCallbackCommand, 
			this->Priority);
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
	}

	this->Interactor->Render();
}

void CGridLineSelector::PlaceWidget(float bds[6])
{

}

//void GetXYatZ(vtkRenderWindowInteractor *interactor, vtkRenderer *renderer, vtkFloatingPointType value, vtkFloatingPointType point[3])
void GetPlaneCoordinates(vtkRenderWindowInteractor *interactor, vtkRenderer *renderer, int fixed, vtkFloatingPointType value, vtkFloatingPointType point[3])
{
	// fixed 
	//   0 => X
	//   1 => Y
	//   2 => Z
	//
	ASSERT(0 <= fixed && fixed < 3);

	int i;

	// get the position of the mouse cursor in screen/window coordinates
	// (pixel)
	int* pos = interactor->GetEventPosition();

	//vtkRenderer *Renderer = this->m_pView->GetRenderer();

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
	if ( worldCoords[3] == 0.0 )
	{
		ASSERT(FALSE);
		return;
	}
	vtkFloatingPointType pickPosition[3];
	for (i = 0; i < 3; ++i)
	{
		pickPosition[i] = worldCoords[i] / worldCoords[3];
	}

	if ( camera->GetParallelProjection() )
	{
		double* cameraDOP = camera->GetDirectionOfProjection();
		double t = (value - pickPosition[fixed]) / cameraDOP[fixed];
		for (i = 0; i < 3; ++i)
		{
			if ( i != fixed )
			{
				point[i] = pickPosition[i] + t * cameraDOP[i];
			}
		}
	}
	else
	{
		double *cameraPos = camera->GetPosition();
		// double t = -cameraPos[2] / ( PickPosition[2] - cameraPos[2] );
		double t = (value - cameraPos[fixed]) / ( pickPosition[fixed] - cameraPos[fixed] );
		for (i = 0; i < 3; ++i)
		{
			if ( i != fixed )
			{
				point[i] = cameraPos[i] + t * ( pickPosition[i] - cameraPos[i] );
			}
		}
	}
	point[fixed] = value;

// COMMENT: {8/23/2005 2:30:52 PM}	// SCALE
// COMMENT: {8/23/2005 2:30:52 PM}	//
// COMMENT: {8/23/2005 2:30:52 PM}	vtkFloatingPointType* scale = this->m_pView->GetDocument()->GetScale();
// COMMENT: {8/23/2005 2:30:52 PM}
// COMMENT: {8/23/2005 2:30:52 PM}	// UNITS
// COMMENT: {8/23/2005 2:30:52 PM}	const CUnits& units = this->m_pView->GetDocument()->GetUnits();
// COMMENT: {8/23/2005 2:30:52 PM}	const char* xy = units.horizontal.defined ? units.horizontal.input : units.horizontal.si;
// COMMENT: {8/23/2005 2:30:52 PM}	const char* z = units.vertical.defined ? units.vertical.input : units.vertical.si;
// COMMENT: {8/23/2005 2:30:52 PM}
// COMMENT: {8/23/2005 2:30:52 PM}	// TODO this->m_WorldPointMinZPlane;
// COMMENT: {8/23/2005 2:30:52 PM}
// COMMENT: {8/23/2005 2:30:52 PM}	((CMainFrame*)::AfxGetMainWnd())->UpdateXYZ(
// COMMENT: {8/23/2005 2:30:52 PM}		this->m_WorldPointXYPlane[0]/scale[0]/units.horizontal.input_to_si,
// COMMENT: {8/23/2005 2:30:52 PM}		this->m_WorldPointXYPlane[1]/scale[1]/units.horizontal.input_to_si,
// COMMENT: {8/23/2005 2:30:52 PM}		// zMin/scale[2]/units.vertical.input_to_si,
// COMMENT: {8/23/2005 2:30:52 PM}		this->m_WorldPointXYPlane[2]/scale[2]/units.vertical.input_to_si,
// COMMENT: {8/23/2005 2:30:52 PM}		xy,
// COMMENT: {8/23/2005 2:30:52 PM}		z);
}
//}}

void CGridLineSelector::ProcessEvents(vtkObject* vtkNotUsed(object), 
								 unsigned long event,
								 void* clientdata, 
								 void* vtkNotUsed(calldata))
{
	CGridLineSelector* self = reinterpret_cast<CGridLineSelector *>( clientdata );
	if ( !self || !self->GridActor ) return;

	if ( !self->Interactor ) return;

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	vtkRenderer *ren = self->Interactor->FindPokedRenderer( X, Y );
	if ( ren != self->CurrentRenderer )
	{
		return;
	}

	static int start_element;
	static vtkFloatingPointType zMaxPosition[3];
	static CGrid grid = self->GridActor->m_gridKeyword.m_grid[0];
	static vtkFloatingPointType outlineBounds[6];

	static vtkFloatingPointType startPoint[3];
	static vtkFloatingPointType endPoint[3];


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
	return;


	switch(event)
	{
	case vtkCommand::LeftButtonPressEvent:
		//self->OnLeftButtonDown();
		if (self->GridActor->PlanePicker->Pick(X, Y, 0, self->CurrentRenderer))
		{
			//self->State = CGridLineSelector::Selecting;
			//TRACE("CGridLineSelector::LeftButtonPressEvent\n");
			//self->EventCallbackCommand->SetAbortFlag(1);
			vtkFloatingPointType bounds[6];
			self->GridActor->GetBounds(bounds);

			CGrid grid = self->GridActor->m_gridKeyword.m_grid[0];
			grid.Setup();

			//vtkFloatingPointType pos[3];
			self->GridActor->PlanePicker->GetPickPosition(zMaxPosition);
			start_element = grid.Element(zMaxPosition[0]);
			//{{
			if (start_element == -1)
			{
				start_element = grid.Closest(zMaxPosition[0]);
				if (start_element != 0)
				{
					--start_element;
				}
			}
			//}}
			if (start_element != -1)
			{
				bounds[0] = grid.coord[start_element];
				bounds[1] = grid.coord[start_element+1];
				self->Source->SetBounds(bounds);
				self->Actor->SetVisibility(1);
				self->State = CGridLineSelector::Selecting;
				self->EventCallbackCommand->SetAbortFlag(1);
				self->Interactor->Render();
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		else
		{
			vtkFloatingPointType bounds[6];
			self->GridActor->GetBounds(bounds);

			////::GetXYatZ(self->Interactor, self->CurrentRenderer, bounds[5], zMaxPosition);
			self->Actor->SetVisibility(0);

			CGrid grid = self->GridActor->m_gridKeyword.m_grid[0];
			grid.Setup();
			start_element = grid.Element(zMaxPosition[0]);
			//{{
			if (start_element == -1)
			{
				start_element = grid.Closest(zMaxPosition[0]);
				if (start_element != 0)
				{
					--start_element;
				}
			}
			//}}
			if (start_element != -1)
			{
				self->State = CGridLineSelector::Selecting;
				self->EventCallbackCommand->SetAbortFlag(1);
				self->Interactor->Render();
			}
			//self->State = CGridLineSelector::Start;
		}
		break;
	case vtkCommand::LeftButtonReleaseEvent:
		//self->OnLeftButtonUp();
		if ( self->State == CGridLineSelector::Selecting )
		{
			TRACE("CGridLineSelector::LeftButtonReleaseEvent\n");
			self->EventCallbackCommand->SetAbortFlag(1);
			self->State = CGridLineSelector::Start;
		}
		break;
// COMMENT: {8/22/2005 3:47:40 PM}	case vtkCommand::MiddleButtonPressEvent:
// COMMENT: {8/22/2005 3:47:40 PM}		self->OnMiddleButtonDown();
// COMMENT: {8/22/2005 3:47:40 PM}		break;
// COMMENT: {8/22/2005 3:47:40 PM}	case vtkCommand::MiddleButtonReleaseEvent:
// COMMENT: {8/22/2005 3:47:40 PM}		self->OnMiddleButtonUp();
// COMMENT: {8/22/2005 3:47:40 PM}		break;
// COMMENT: {8/22/2005 3:47:40 PM}	case vtkCommand::RightButtonPressEvent:
// COMMENT: {8/22/2005 3:47:40 PM}		self->OnRightButtonDown();
// COMMENT: {8/22/2005 3:47:40 PM}		break;
// COMMENT: {8/22/2005 3:47:40 PM}	case vtkCommand::RightButtonReleaseEvent:
// COMMENT: {8/22/2005 3:47:40 PM}		self->OnRightButtonUp();
// COMMENT: {8/22/2005 3:47:40 PM}		break;
	case vtkCommand::MouseMoveEvent:
		//self->OnMouseMove();
		if ( self->State == CGridLineSelector::Selecting )
		{
			TRACE("CGridLineSelector::MouseMoveEvent\n");
			self->EventCallbackCommand->SetAbortFlag(1);

			if (self->GridActor->PlanePicker->Pick(X, Y, 0, self->CurrentRenderer))
			{
				vtkFloatingPointType bounds[6];
				self->GridActor->GetBounds(bounds);
				CGrid grid = self->GridActor->m_gridKeyword.m_grid[0];
				grid.Setup();

				vtkFloatingPointType pos[3];
				self->GridActor->PlanePicker->GetPickPosition(pos);
				int end_element = grid.Element(pos[0]);
				if (end_element != -1)
				{
					bounds[0] = grid.coord[ min( start_element , end_element ) ];
					bounds[1] = grid.coord[ max( start_element + 1 , end_element + 1) ];
					self->Source->SetBounds(bounds);
					self->Actor->SetVisibility(1);
					self->Interactor->Render();
				}
			}
			else
			{
				self->Actor->SetVisibility(0);
				self->Interactor->Render();
			}
		}
		break;
	}
}

// COMMENT: {8/24/2005 7:50:47 PM}void CGridLineSelector::OnMouseMove(void)
// COMMENT: {8/24/2005 7:50:47 PM}{
// COMMENT: {8/24/2005 7:50:47 PM}	if ( this->State == CGridLineSelector::Selecting )
// COMMENT: {8/24/2005 7:50:47 PM}	{
// COMMENT: {8/24/2005 7:50:47 PM}		vtkFloatingPointType bounds[6];
// COMMENT: {8/24/2005 7:50:47 PM}		this->GridActor->GetBounds(bounds);
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}		vtkFloatingPointType endPoint[3];
// COMMENT: {8/24/2005 7:50:47 PM}		//::GetXYatZ(this->Interactor, this->CurrentRenderer, bounds[5], endPoint);
// COMMENT: {8/24/2005 7:50:47 PM}		//::GetXYatZ(this->Interactor, this->CurrentRenderer, bounds[this->FixedPlane], endPoint);
// COMMENT: {8/24/2005 7:50:47 PM}		::GetPlaneCoordinates(this->Interactor, this->CurrentRenderer, this->FixedCoord, bounds[this->FixedPlane], endPoint);
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 7:30:22 PM}		bounds[0] = min(this->StartPoint[0], endPoint[0]);
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 7:30:22 PM}		bounds[1] = max(StartPoint[0], endPoint[0]);
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 7:30:22 PM}		bounds[2] = min(StartPoint[1], endPoint[1]);
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 7:30:22 PM}		bounds[3] = max(StartPoint[1], endPoint[1]);
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 7:30:22 PM}		this->OutlineSource->SetBounds(bounds);
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}		for (int i = 0; i < 3; ++i)
// COMMENT: {8/24/2005 7:50:47 PM}		{
// COMMENT: {8/24/2005 7:50:47 PM}			if (i != this->FixedCoord)
// COMMENT: {8/24/2005 7:50:47 PM}			{
// COMMENT: {8/24/2005 7:50:47 PM}				bounds[2 * i] = min(this->StartPoint[i], endPoint[i]);
// COMMENT: {8/24/2005 7:50:47 PM}				bounds[2 * i + 1] = max(this->StartPoint[i], endPoint[i]);
// COMMENT: {8/24/2005 7:50:47 PM}			}
// COMMENT: {8/24/2005 7:50:47 PM}		}
// COMMENT: {8/24/2005 7:50:47 PM}		this->OutlineSource->SetBounds(bounds);
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}		//{{
// COMMENT: {8/24/2005 7:50:47 PM}		int X = this->Interactor->GetEventPosition()[0];
// COMMENT: {8/24/2005 7:50:47 PM}		int Y = this->Interactor->GetEventPosition()[1];
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}		if (this->GridActor->PlanePicker->Pick(X, Y, 0, this->CurrentRenderer))
// COMMENT: {8/24/2005 7:50:47 PM}		{
// COMMENT: {8/24/2005 7:50:47 PM}			vtkFloatingPointType* bounds = this->GridActor->GetBounds();
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}// #define SELECT_WITH_INTERSECTION
// COMMENT: {8/24/2005 7:50:47 PM}#if defined(SELECT_WITH_INTERSECTION)
// COMMENT: {8/24/2005 7:50:47 PM}			// intersection 
// COMMENT: {8/24/2005 7:50:47 PM}			//
// COMMENT: {8/24/2005 7:50:47 PM}			this->Min[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 7:50:47 PM}			this->Min[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[2]);
// COMMENT: {8/24/2005 7:50:47 PM}			this->Max[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[1]) + 1;
// COMMENT: {8/24/2005 7:50:47 PM}			this->Max[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[3]) + 1;
// COMMENT: {8/24/2005 7:50:47 PM}#else
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 5:03:57 PM}			// interior only
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 5:03:57 PM}			//
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 5:03:57 PM}			this->Min[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 5:03:57 PM}			this->Min[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[2]);
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 5:03:57 PM}			this->Max[0] = this->GridKeyword.m_grid[0].lower_bound(this->OutlineSource->GetBounds()[1]) - 1;
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 5:03:57 PM}			this->Max[1] = this->GridKeyword.m_grid[1].lower_bound(this->OutlineSource->GetBounds()[3]) - 1;
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 7:16:30 PM}			this->Min[0] = this->GridKeyword.m_grid[0].lower_bound(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 7:16:30 PM}			this->Min[1] = this->GridKeyword.m_grid[1].lower_bound(this->OutlineSource->GetBounds()[2]);
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 7:16:30 PM}
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 7:16:30 PM}			this->Max[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[1]) - 1;
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 7:16:30 PM}			this->Max[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[3]) - 1;
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}			vtkFloatingPointType scale[6];
// COMMENT: {8/24/2005 7:50:47 PM}			for (int i = 0; i < 3; ++i)
// COMMENT: {8/24/2005 7:50:47 PM}			{
// COMMENT: {8/24/2005 7:50:47 PM}				if (i == this->FixedCoord)
// COMMENT: {8/24/2005 7:50:47 PM}				{
// COMMENT: {8/24/2005 7:50:47 PM}					this->Min[i] = 0;
// COMMENT: {8/24/2005 7:50:47 PM}					this->Max[i] = this->GridKeyword.m_grid[i].count_coord - 1;
// COMMENT: {8/24/2005 7:50:47 PM}					scale[i] = this->GridActor->GetScale()[i];
// COMMENT: {8/24/2005 7:50:47 PM}				}
// COMMENT: {8/24/2005 7:50:47 PM}				else
// COMMENT: {8/24/2005 7:50:47 PM}				{
// COMMENT: {8/24/2005 7:50:47 PM}					this->Min[i] = this->GridKeyword.m_grid[i].lower_bound(this->OutlineSource->GetBounds()[2 * i]);
// COMMENT: {8/24/2005 7:50:47 PM}					this->Max[i] = this->GridKeyword.m_grid[i].upper_bound(this->OutlineSource->GetBounds()[2 * i + 1]) - 1;
// COMMENT: {8/24/2005 7:50:47 PM}					scale[i] = 1;
// COMMENT: {8/24/2005 7:50:47 PM}				}
// COMMENT: {8/24/2005 7:50:47 PM}			}
// COMMENT: {8/24/2005 7:50:47 PM}#endif
// COMMENT: {8/24/2005 7:50:47 PM}			TRACE("Hit Plane\n");
// COMMENT: {8/24/2005 7:50:47 PM}			TRACE("X (%d, %d)\n", this->Min[0], this->Max[0]);
// COMMENT: {8/24/2005 7:50:47 PM}			TRACE("Y (%d, %d)\n", this->Min[1], this->Max[1]);
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}			this->Source->SetBounds(
// COMMENT: {8/24/2005 7:50:47 PM}				this->GridKeyword.m_grid[0].coord[this->Min[0]] * scale[0],
// COMMENT: {8/24/2005 7:50:47 PM}				this->GridKeyword.m_grid[0].coord[this->Max[0]] * scale[0],
// COMMENT: {8/24/2005 7:50:47 PM}				this->GridKeyword.m_grid[1].coord[this->Min[1]] * scale[1],
// COMMENT: {8/24/2005 7:50:47 PM}				this->GridKeyword.m_grid[1].coord[this->Max[1]] * scale[1],
// COMMENT: {8/24/2005 7:50:47 PM}				this->GridKeyword.m_grid[2].coord[this->Min[2]] * scale[2],
// COMMENT: {8/24/2005 7:50:47 PM}				this->GridKeyword.m_grid[2].coord[this->Max[2]] * scale[2]
// COMMENT: {8/24/2005 7:50:47 PM}				);
// COMMENT: {8/24/2005 7:50:47 PM}			this->Actor->SetVisibility(1);
// COMMENT: {8/24/2005 7:50:47 PM}		}
// COMMENT: {8/24/2005 7:50:47 PM}		else
// COMMENT: {8/24/2005 7:50:47 PM}		{
// COMMENT: {8/24/2005 7:50:47 PM}			vtkFloatingPointType* bounds = this->GridActor->GetBounds();
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}#if defined(SELECT_WITH_INTERSECTION)
// COMMENT: {8/24/2005 7:50:47 PM}			// intersection 
// COMMENT: {8/24/2005 7:50:47 PM}			//
// COMMENT: {8/24/2005 7:50:47 PM}			this->Min[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 7:50:47 PM}			this->Min[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[2]);
// COMMENT: {8/24/2005 7:50:47 PM}			this->Max[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[1]) + 1;
// COMMENT: {8/24/2005 7:50:47 PM}			this->Max[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[3]) + 1;
// COMMENT: {8/24/2005 7:50:47 PM}#else
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 4:47:47 PM}			// interior only
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 4:47:47 PM}			//
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 4:47:47 PM}			this->Min[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 4:47:47 PM}			this->Min[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[2]);
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 4:47:47 PM}			this->Max[0] = this->GridKeyword.m_grid[0].lower_bound(this->OutlineSource->GetBounds()[1]) - 1;
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 4:47:47 PM}			this->Max[1] = this->GridKeyword.m_grid[1].lower_bound(this->OutlineSource->GetBounds()[3]) - 1;
// COMMENT: {8/24/2005 7:50:47 PM}			// interior only
// COMMENT: {8/24/2005 7:50:47 PM}			//
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 5:00:13 PM}			this->Min[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 7:50:47 PM}// COMMENT: {8/24/2005 5:00:13 PM}			this->Min[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[2]);
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}			this->Min[0] = this->GridKeyword.m_grid[0].lower_bound(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 7:50:47 PM}			this->Min[1] = this->GridKeyword.m_grid[1].lower_bound(this->OutlineSource->GetBounds()[2]);
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}			this->Max[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[1]) - 1;
// COMMENT: {8/24/2005 7:50:47 PM}			this->Max[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[3]) - 1;
// COMMENT: {8/24/2005 7:50:47 PM}#endif
// COMMENT: {8/24/2005 7:50:47 PM}			TRACE("No Plane\n");
// COMMENT: {8/24/2005 7:50:47 PM}			TRACE("X (%d, %d)\n", this->Min[0], this->Max[0]);
// COMMENT: {8/24/2005 7:50:47 PM}			TRACE("Y (%d, %d)\n", this->Min[1], this->Max[1]);
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}			this->Source->SetBounds(
// COMMENT: {8/24/2005 7:50:47 PM}				this->GridKeyword.m_grid[0].coord[this->Min[0]],
// COMMENT: {8/24/2005 7:50:47 PM}				this->GridKeyword.m_grid[0].coord[this->Max[0]],
// COMMENT: {8/24/2005 7:50:47 PM}				this->GridKeyword.m_grid[1].coord[this->Min[1]],
// COMMENT: {8/24/2005 7:50:47 PM}				this->GridKeyword.m_grid[1].coord[this->Max[1]],
// COMMENT: {8/24/2005 7:50:47 PM}				bounds[4],
// COMMENT: {8/24/2005 7:50:47 PM}				bounds[5]
// COMMENT: {8/24/2005 7:50:47 PM}				);
// COMMENT: {8/24/2005 7:50:47 PM}			this->Actor->SetVisibility(1);
// COMMENT: {8/24/2005 7:50:47 PM}		}
// COMMENT: {8/24/2005 7:50:47 PM}
// COMMENT: {8/24/2005 7:50:47 PM}		this->EventCallbackCommand->SetAbortFlag(1);
// COMMENT: {8/24/2005 7:50:47 PM}		this->Interactor->Render();
// COMMENT: {8/24/2005 7:50:47 PM}	}
// COMMENT: {8/24/2005 7:50:47 PM}}

void CGridLineSelector::OnMouseMove(void)
{
	if ( this->State == CGridLineSelector::Selecting )
	{
		vtkFloatingPointType bounds[6];
		this->GridActor->GetBounds(bounds);

		vtkFloatingPointType endPoint[3];
		::GetPlaneCoordinates(this->Interactor, this->CurrentRenderer, this->FixedCoord, bounds[this->FixedPlane], endPoint);

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
		vtkFloatingPointType *scale = this->GridActor->GetScale();
		for (int i = 0; i < 3; ++i)
		{
			if (i == this->FixedCoord)
			{
				this->Min[i] = 0;
				this->Max[i] = this->GridKeyword.m_grid[i].count_coord - 1;
			}
			else
			{
				if (this->bSelectElementIntersection)
				{
					// select intersected elements
					this->Min[i] = this->GridKeyword.m_grid[i].upper_bound(this->OutlineSource->GetBounds()[2 * i] / scale[i]) - 1;
					if (this->Min[i] < 0)
					{
						this->Min[i] = 0;
					}
					this->Max[i] = this->GridKeyword.m_grid[i].lower_bound(this->OutlineSource->GetBounds()[2 * i + 1] / scale[i]);
				}
				else
				{
					// only select interior elements
					this->Min[i] = this->GridKeyword.m_grid[i].lower_bound(this->OutlineSource->GetBounds()[2 * i] / scale[i]);
					this->Max[i] = this->GridKeyword.m_grid[i].upper_bound(this->OutlineSource->GetBounds()[2 * i + 1] / scale[i]) - 1;
				}
				ASSERT( 0 <= this->Min[i] && this->Min[i] < this->GridKeyword.m_grid[i].count_coord );
				ASSERT( 0 <= this->Max[i] && this->Max[i] < this->GridKeyword.m_grid[i].count_coord );
				bounds[2 * i]     = this->GridKeyword.m_grid[i].coord[this->Min[i]] * scale[i];
				bounds[2 * i + 1] = this->GridKeyword.m_grid[i].coord[this->Max[i]] * scale[i];
			}
		}

		TRACE("CGridLineSelector::OnMouseMove\n");
		TRACE("X (%d, %d) (%g, %g)\n", this->Min[0], this->Max[0], bounds[0], bounds[1]);
		TRACE("Y (%d, %d) (%g, %g)\n", this->Min[1], this->Max[1], bounds[2], bounds[3]);
		TRACE("Z (%d, %d) (%g, %g)\n", this->Min[2], this->Max[2], bounds[4], bounds[5]);

		this->Source->SetBounds(bounds);
		this->Actor->SetVisibility(1);

		this->EventCallbackCommand->SetAbortFlag(1);
		this->Interactor->Render();
	}
}

void CGridLineSelector::OnLeftButtonDown(void)
{
	ASSERT ( this->State == CGridLineSelector::Start );

	// set state
	//
	this->State = CGridLineSelector::Selecting;

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
	vtkFloatingPointType viewPlaneNormal[3];
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
	vtkFloatingPointType* bounds = this->GridActor->GetBounds();
	::GetPlaneCoordinates(this->Interactor, this->CurrentRenderer, this->FixedCoord, bounds[this->FixedPlane], this->StartPoint);

	// set bounds for the Outline
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

	if (this->bSelectElementIntersection)
	{
		// set bounds for the highlighted elements
		//
		vtkFloatingPointType *scale = this->GridActor->GetScale();
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


// COMMENT: {8/24/2005 9:08:52 PM}void CGridLineSelector::OnLeftButtonDown(void)
// COMMENT: {8/24/2005 9:08:52 PM}{
// COMMENT: {8/24/2005 9:08:52 PM}	ASSERT ( this->State == CGridLineSelector::Start );
// COMMENT: {8/24/2005 9:08:52 PM}	if ( this->State == CGridLineSelector::Start )
// COMMENT: {8/24/2005 9:08:52 PM}	{
// COMMENT: {8/24/2005 9:08:52 PM}		//{{
// COMMENT: {8/24/2005 9:08:52 PM}		vtkFloatingPointType viewPlaneNormal[3];
// COMMENT: {8/24/2005 9:08:52 PM}		vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
// COMMENT: {8/24/2005 9:08:52 PM}		camera->GetViewPlaneNormal(viewPlaneNormal);
// COMMENT: {8/24/2005 9:08:52 PM}		//}}
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}		this->GridKeyword = this->GridActor->m_gridKeyword;
// COMMENT: {8/24/2005 9:08:52 PM}		for (int i = 0; i < 3; ++i)
// COMMENT: {8/24/2005 9:08:52 PM}		{
// COMMENT: {8/24/2005 9:08:52 PM}			this->GridKeyword.m_grid[i].Setup();
// COMMENT: {8/24/2005 9:08:52 PM}		}
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}		//{{
// COMMENT: {8/24/2005 9:08:52 PM}		///int imax;
// COMMENT: {8/24/2005 9:08:52 PM}		///int fixed[6];
// COMMENT: {8/24/2005 9:08:52 PM}		double max = 0.0;
// COMMENT: {8/24/2005 9:08:52 PM}		for (int i = 0; i < 3; ++i)
// COMMENT: {8/24/2005 9:08:52 PM}		{			
// COMMENT: {8/24/2005 9:08:52 PM}			///fixed[2 * i] = 0;
// COMMENT: {8/24/2005 9:08:52 PM}			///fixed[2 * i + 1] = 0;
// COMMENT: {8/24/2005 9:08:52 PM}			if (::fabs(max) < ::fabs(viewPlaneNormal[i]))
// COMMENT: {8/24/2005 9:08:52 PM}			{
// COMMENT: {8/24/2005 9:08:52 PM}				this->FixedCoord = i;
// COMMENT: {8/24/2005 9:08:52 PM}				max = viewPlaneNormal[i];
// COMMENT: {8/24/2005 9:08:52 PM}				if (max < 0.0)
// COMMENT: {8/24/2005 9:08:52 PM}				{
// COMMENT: {8/24/2005 9:08:52 PM}					this->FixedPlane = 2 * i;
// COMMENT: {8/24/2005 9:08:52 PM}				}
// COMMENT: {8/24/2005 9:08:52 PM}				else
// COMMENT: {8/24/2005 9:08:52 PM}				{
// COMMENT: {8/24/2005 9:08:52 PM}					this->FixedPlane = 2 * i + 1;
// COMMENT: {8/24/2005 9:08:52 PM}				}
// COMMENT: {8/24/2005 9:08:52 PM}			}
// COMMENT: {8/24/2005 9:08:52 PM}		}
// COMMENT: {8/24/2005 9:08:52 PM}		///fixed[this->FixedPlane] = 1;
// COMMENT: {8/24/2005 9:08:52 PM}		///if ( !fixed[4] && !fixed[5] ) return;
// COMMENT: {8/24/2005 9:08:52 PM}		///if ( !(this->FixedPlane != 4) && !(this->FixedPlane != 5) ) return;
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/24/2005 7:10:32 PM}		if (this->FixedCoord != 2) return;
// COMMENT: {8/24/2005 9:08:52 PM}		//}}
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}		this->State = CGridLineSelector::Selecting;
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}		vtkFloatingPointType* bounds = this->GridActor->GetBounds();
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}		///::GetXYatZ(this->Interactor, this->CurrentRenderer, bounds[this->FixedPlane], this->StartPoint);
// COMMENT: {8/24/2005 9:08:52 PM}		::GetPlaneCoordinates(this->Interactor, this->CurrentRenderer, this->FixedCoord, bounds[this->FixedPlane], this->StartPoint);
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}		//bounds[0] = bounds[1] = this->StartPoint[0];
// COMMENT: {8/24/2005 9:08:52 PM}		//bounds[2] = bounds[3] = this->StartPoint[1];
// COMMENT: {8/24/2005 9:08:52 PM}		///this->OutlineSource->SetBounds(bounds);
// COMMENT: {8/24/2005 9:08:52 PM}		this->OutlineSource->SetBounds(
// COMMENT: {8/24/2005 9:08:52 PM}			this->StartPoint[0],
// COMMENT: {8/24/2005 9:08:52 PM}			this->StartPoint[0],
// COMMENT: {8/24/2005 9:08:52 PM}			this->StartPoint[1],
// COMMENT: {8/24/2005 9:08:52 PM}			this->StartPoint[1],
// COMMENT: {8/24/2005 9:08:52 PM}			bounds[4],
// COMMENT: {8/24/2005 9:08:52 PM}			bounds[5]
// COMMENT: {8/24/2005 9:08:52 PM}            );
// COMMENT: {8/24/2005 9:08:52 PM}		this->OutlineActor->SetVisibility(1);
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}		//{{
// COMMENT: {8/24/2005 9:08:52 PM}		int X = this->Interactor->GetEventPosition()[0];
// COMMENT: {8/24/2005 9:08:52 PM}		int Y = this->Interactor->GetEventPosition()[1];
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}		if (this->GridActor->PlanePicker->Pick(X, Y, 0, this->CurrentRenderer))
// COMMENT: {8/24/2005 9:08:52 PM}		{
// COMMENT: {8/24/2005 9:08:52 PM}			vtkFloatingPointType* bounds = this->GridActor->GetBounds();
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}#if defined(SELECT_WITH_INTERSECTION)
// COMMENT: {8/24/2005 9:08:52 PM}			// intersection 
// COMMENT: {8/24/2005 9:08:52 PM}			//
// COMMENT: {8/24/2005 9:08:52 PM}			this->Min[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 9:08:52 PM}			this->Min[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[2]);
// COMMENT: {8/24/2005 9:08:52 PM}			this->Max[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[1]) + 1;
// COMMENT: {8/24/2005 9:08:52 PM}			this->Max[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[3]) + 1;
// COMMENT: {8/24/2005 9:08:52 PM}#else
// COMMENT: {8/24/2005 9:08:52 PM}			// interior only
// COMMENT: {8/24/2005 9:08:52 PM}			//
// COMMENT: {8/24/2005 9:08:52 PM}			this->Min[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 9:08:52 PM}			this->Min[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[2]);
// COMMENT: {8/24/2005 9:08:52 PM}			this->Max[0] = this->GridKeyword.m_grid[0].lower_bound(this->OutlineSource->GetBounds()[1]) - 1;
// COMMENT: {8/24/2005 9:08:52 PM}			this->Max[1] = this->GridKeyword.m_grid[1].lower_bound(this->OutlineSource->GetBounds()[3]) - 1;
// COMMENT: {8/24/2005 9:08:52 PM}#endif
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}			//if (xstart != -1)
// COMMENT: {8/24/2005 9:08:52 PM}			{
// COMMENT: {8/24/2005 9:08:52 PM}				this->Source->SetBounds(
// COMMENT: {8/24/2005 9:08:52 PM}					this->GridKeyword.m_grid[0].coord[this->Min[0]],
// COMMENT: {8/24/2005 9:08:52 PM}					this->GridKeyword.m_grid[0].coord[this->Max[0]],
// COMMENT: {8/24/2005 9:08:52 PM}					this->GridKeyword.m_grid[1].coord[this->Min[1]],
// COMMENT: {8/24/2005 9:08:52 PM}					this->GridKeyword.m_grid[1].coord[this->Max[1]],
// COMMENT: {8/24/2005 9:08:52 PM}					bounds[4],
// COMMENT: {8/24/2005 9:08:52 PM}					bounds[5]
// COMMENT: {8/24/2005 9:08:52 PM}					);
// COMMENT: {8/24/2005 9:08:52 PM}				this->Actor->SetVisibility(1);
// COMMENT: {8/24/2005 9:08:52 PM}				this->Interactor->Render();
// COMMENT: {8/24/2005 9:08:52 PM}			}
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			vtkFloatingPointType bounds[6];
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			this->GridActor->GetBounds(bounds);
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			CGrid grid = this->GridActor->m_gridKeyword.m_grid[0];
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			grid.Setup();
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			this->GridActor->PlanePicker->GetPickPosition(zMaxPosition);
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			start_element = grid.Element(zMaxPosition[0]);
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			//{{
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			if (start_element == -1)
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			{
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				start_element = grid.Closest(zMaxPosition[0]);
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				if (start_element != 0)
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				{
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}					--start_element;
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				}
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			}
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			//}}
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			if (start_element != -1)
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			{
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				bounds[0] = grid.coord[start_element];
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				bounds[1] = grid.coord[start_element+1];
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				self->Source->SetBounds(bounds);
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				self->Actor->SetVisibility(1);
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				self->State = CGridLineSelector::Selecting;
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				self->EventCallbackCommand->SetAbortFlag(1);
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				self->Interactor->Render();
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			}
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			else
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			{
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}				ASSERT(FALSE);
// COMMENT: {8/24/2005 9:08:52 PM}// COMMENT: {8/23/2005 6:27:32 PM}			}
// COMMENT: {8/24/2005 9:08:52 PM}		}
// COMMENT: {8/24/2005 9:08:52 PM}		else
// COMMENT: {8/24/2005 9:08:52 PM}		{
// COMMENT: {8/24/2005 9:08:52 PM}		}
// COMMENT: {8/24/2005 9:08:52 PM}		//}}
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}
// COMMENT: {8/24/2005 9:08:52 PM}		this->EventCallbackCommand->SetAbortFlag(1);
// COMMENT: {8/24/2005 9:08:52 PM}		this->Interactor->Render();
// COMMENT: {8/24/2005 9:08:52 PM}	}
// COMMENT: {8/24/2005 9:08:52 PM}}

void CGridLineSelector::OnLeftButtonUp(void)
{
	if ( this->State == CGridLineSelector::Selecting )
	{
		this->State = CGridLineSelector::Start;
		this->EventCallbackCommand->SetAbortFlag(1);
		this->SetEnabled(0);
		this->Interactor->Render();
	}
}
