#include "StdAfx.h"
#include "GridLineSelector.h"

#include "GridActor.h"

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
{
	this->EventCallbackCommand->SetCallback(CGridLineSelector::ProcessEvents);
	this->SetKeyPressActivation(0);

	// selection
	this->Source = vtkCubeSource::New();
	this->Mapper = vtkPolyDataMapper::New();
	this->Mapper->SetResolveCoincidentTopologyToPolygonOffset();

	this->Mapper->SetInput( this->Source->GetOutput() );
	this->Actor = vtkActor::New();
	this->Actor->SetMapper( this->Mapper );
	this->Actor->SetVisibility(0);

	// outline
	///this->OutlineSource = vtkCubeSource::New();
	this->OutlineSource = vtkOutlineSource::New();

	this->OutlineMapper = vtkPolyDataMapper::New();
	///this->OutlineMapper->SetResolveCoincidentTopologyToPolygonOffset();
	this->OutlineMapper->SetInput( this->OutlineSource->GetOutput() );

	this->OutlineActor = vtkActor::New();
	this->OutlineActor->SetMapper( this->OutlineMapper );
	this->OutlineActor->SetVisibility(0);
    ///this->OutlineActor->GetProperty()->SetRepresentationToWireframe();
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
// COMMENT: {8/22/2005 5:23:49 PM}		i->AddObserver(vtkCommand::MiddleButtonPressEvent, 
// COMMENT: {8/22/2005 5:23:49 PM}			this->EventCallbackCommand, this->Priority);
// COMMENT: {8/22/2005 5:23:49 PM}		i->AddObserver(vtkCommand::MiddleButtonReleaseEvent, 
// COMMENT: {8/22/2005 5:23:49 PM}			this->EventCallbackCommand, this->Priority);
// COMMENT: {8/22/2005 5:23:49 PM}		i->AddObserver(vtkCommand::RightButtonPressEvent, 
// COMMENT: {8/22/2005 5:23:49 PM}			this->EventCallbackCommand, this->Priority);
// COMMENT: {8/22/2005 5:23:49 PM}		i->AddObserver(vtkCommand::RightButtonReleaseEvent, 
// COMMENT: {8/22/2005 5:23:49 PM}			this->EventCallbackCommand, this->Priority);

// COMMENT: {8/22/2005 3:39:29 PM}		// Add the various actors
// COMMENT: {8/22/2005 3:39:29 PM}		// Add the outline
// COMMENT: {8/22/2005 3:39:29 PM}		this->CurrentRenderer->AddActor(this->HexActor);
// COMMENT: {8/22/2005 3:39:29 PM}		this->CurrentRenderer->AddActor(this->HexOutline);
// COMMENT: {8/22/2005 3:39:29 PM}		this->HexActor->SetProperty(this->OutlineProperty);
// COMMENT: {8/22/2005 3:39:29 PM}		this->HexOutline->SetProperty(this->OutlineProperty);
// COMMENT: {8/22/2005 3:39:29 PM}
// COMMENT: {8/22/2005 3:39:29 PM}		// Add the hex face
// COMMENT: {8/22/2005 3:39:29 PM}		this->CurrentRenderer->AddActor(this->HexFace);
// COMMENT: {8/22/2005 3:39:29 PM}		this->HexFace->SetProperty(this->FaceProperty);

// COMMENT: {8/22/2005 3:38:21 PM}		// turn on the handles
// COMMENT: {8/22/2005 3:38:21 PM}		for (int j=0; j<7; j++)
// COMMENT: {8/22/2005 3:38:21 PM}		{
// COMMENT: {8/22/2005 3:38:21 PM}			this->CurrentRenderer->AddActor(this->Handle[j]);
// COMMENT: {8/22/2005 3:38:21 PM}			this->Handle[j]->SetProperty(this->HandleProperty);
// COMMENT: {8/22/2005 3:38:21 PM}		}
// COMMENT: {8/22/2005 3:38:21 PM}
		this->CurrentRenderer->AddActor(this->Actor);
		this->CurrentRenderer->AddActor(this->OutlineActor);
// COMMENT: {8/22/2005 3:38:21 PM}		this->InvokeEvent(vtkCommand::EnableEvent,NULL);
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

// COMMENT: {8/22/2005 3:38:03 PM}		// turn off the outline
// COMMENT: {8/22/2005 3:38:03 PM}		this->CurrentRenderer->RemoveActor(this->HexActor);
// COMMENT: {8/22/2005 3:38:03 PM}		this->CurrentRenderer->RemoveActor(this->HexOutline);
// COMMENT: {8/22/2005 3:38:03 PM}
// COMMENT: {8/22/2005 3:38:03 PM}		// turn off the hex face
// COMMENT: {8/22/2005 3:38:03 PM}		this->CurrentRenderer->RemoveActor(this->HexFace);
// COMMENT: {8/22/2005 3:38:03 PM}
// COMMENT: {8/22/2005 3:37:53 PM}		// turn off the handles
// COMMENT: {8/22/2005 3:37:53 PM}		for (int i=0; i<7; i++)
// COMMENT: {8/22/2005 3:37:53 PM}		{
// COMMENT: {8/22/2005 3:37:53 PM}			this->CurrentRenderer->RemoveActor(this->Handle[i]);
// COMMENT: {8/22/2005 3:37:53 PM}		}
// COMMENT: {8/22/2005 3:37:53 PM}
// COMMENT: {8/22/2005 3:37:53 PM}		this->CurrentHandle = NULL;
// COMMENT: {8/22/2005 3:37:53 PM}		this->InvokeEvent(vtkCommand::DisableEvent,NULL);

// COMMENT: {8/24/2005 5:18:57 PM}		this->CurrentRenderer->RemoveActor(this->Actor);
// COMMENT: {8/24/2005 5:18:57 PM}		this->CurrentRenderer->RemoveActor(this->OutlineActor);

// COMMENT: {8/24/2005 5:27:28 PM}		this->CurrentRenderer = NULL;
	}

	this->Interactor->Render();
}

void CGridLineSelector::PlaceWidget(float bds[6])
{
// COMMENT: {8/22/2005 3:40:52 PM}  int i;
// COMMENT: {8/22/2005 3:40:52 PM}  double bounds[6], center[3];
// COMMENT: {8/22/2005 3:40:52 PM}  
// COMMENT: {8/22/2005 3:40:52 PM}  this->AdjustBounds(bds,bounds,center);
// COMMENT: {8/22/2005 3:40:52 PM}  
// COMMENT: {8/22/2005 3:40:52 PM}  this->Points->SetPoint(0, bounds[0], bounds[2], bounds[4]);
// COMMENT: {8/22/2005 3:40:52 PM}  this->Points->SetPoint(1, bounds[1], bounds[2], bounds[4]);
// COMMENT: {8/22/2005 3:40:52 PM}  this->Points->SetPoint(2, bounds[1], bounds[3], bounds[4]);
// COMMENT: {8/22/2005 3:40:52 PM}  this->Points->SetPoint(3, bounds[0], bounds[3], bounds[4]);
// COMMENT: {8/22/2005 3:40:52 PM}  this->Points->SetPoint(4, bounds[0], bounds[2], bounds[5]);
// COMMENT: {8/22/2005 3:40:52 PM}  this->Points->SetPoint(5, bounds[1], bounds[2], bounds[5]);
// COMMENT: {8/22/2005 3:40:52 PM}  this->Points->SetPoint(6, bounds[1], bounds[3], bounds[5]);
// COMMENT: {8/22/2005 3:40:52 PM}  this->Points->SetPoint(7, bounds[0], bounds[3], bounds[5]);
// COMMENT: {8/22/2005 3:40:52 PM}
// COMMENT: {8/22/2005 3:40:52 PM}  for (i=0; i<6; i++)
// COMMENT: {8/22/2005 3:40:52 PM}    {
// COMMENT: {8/22/2005 3:40:52 PM}    this->InitialBounds[i] = bounds[i];
// COMMENT: {8/22/2005 3:40:52 PM}    }
// COMMENT: {8/22/2005 3:40:52 PM}  this->InitialLength = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
// COMMENT: {8/22/2005 3:40:52 PM}                             (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
// COMMENT: {8/22/2005 3:40:52 PM}                             (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));
// COMMENT: {8/22/2005 3:40:52 PM}
// COMMENT: {8/22/2005 3:40:52 PM}  this->PositionHandles();
// COMMENT: {8/22/2005 3:40:52 PM}  this->ComputeNormals();
// COMMENT: {8/22/2005 3:40:52 PM}  this->SizeHandles();
}

void GetXYatZ(vtkRenderWindowInteractor *interactor, vtkRenderer *renderer, vtkFloatingPointType zPos, vtkFloatingPointType point[3])
{
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
		double t = (zPos - pickPosition[2]) / cameraDOP[2];
		for (i = 0; i < 2; ++i)
		{
			point[i] = pickPosition[i] + t * cameraDOP[i];
		}
	}
	else
	{
		double *cameraPos = camera->GetPosition();
		// double t = -cameraPos[2] / ( PickPosition[2] - cameraPos[2] );
		double t = (zPos - cameraPos[2]) / ( pickPosition[2] - cameraPos[2] );
		for (i = 0; i < 2; ++i) {
			point[i] = cameraPos[i] + t * ( pickPosition[i] - cameraPos[i] );
		}
	}
	point[2] = zPos;

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

			::GetXYatZ(self->Interactor, self->CurrentRenderer, bounds[5], zMaxPosition);
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

void CGridLineSelector::OnMouseMove(void)
{
	if ( this->State == CGridLineSelector::Selecting )
	{
		vtkFloatingPointType bounds[6];
		this->GridActor->GetBounds(bounds);

		vtkFloatingPointType endPoint[3];
		::GetXYatZ(this->Interactor, this->CurrentRenderer, bounds[5], endPoint);

		bounds[0] = min(this->StartPoint[0], endPoint[0]);
		bounds[1] = max(StartPoint[0], endPoint[0]);
		bounds[2] = min(StartPoint[1], endPoint[1]);
		bounds[3] = max(StartPoint[1], endPoint[1]);
		this->OutlineSource->SetBounds(bounds);

		//{{
		int X = this->Interactor->GetEventPosition()[0];
		int Y = this->Interactor->GetEventPosition()[1];

		if (this->GridActor->PlanePicker->Pick(X, Y, 0, this->CurrentRenderer))
		{
			vtkFloatingPointType* bounds = this->GridActor->GetBounds();

// #define SELECT_WITH_INTERSECTION
#if defined(SELECT_WITH_INTERSECTION)
			// intersection 
			//
			this->Min[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[0]);
			this->Min[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[2]);
			this->Max[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[1]) + 1;
			this->Max[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[3]) + 1;
#else
// COMMENT: {8/24/2005 5:03:57 PM}			// interior only
// COMMENT: {8/24/2005 5:03:57 PM}			//
// COMMENT: {8/24/2005 5:03:57 PM}			this->Min[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 5:03:57 PM}			this->Min[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[2]);
// COMMENT: {8/24/2005 5:03:57 PM}			this->Max[0] = this->GridKeyword.m_grid[0].lower_bound(this->OutlineSource->GetBounds()[1]) - 1;
// COMMENT: {8/24/2005 5:03:57 PM}			this->Max[1] = this->GridKeyword.m_grid[1].lower_bound(this->OutlineSource->GetBounds()[3]) - 1;
			this->Min[0] = this->GridKeyword.m_grid[0].lower_bound(this->OutlineSource->GetBounds()[0]);
			this->Min[1] = this->GridKeyword.m_grid[1].lower_bound(this->OutlineSource->GetBounds()[2]);

			this->Max[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[1]) - 1;
			this->Max[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[3]) - 1;
#endif
			TRACE("Hit Plane\n");
			TRACE("X (%d, %d)\n", this->Min[0], this->Max[0]);
			TRACE("Y (%d, %d)\n", this->Min[1], this->Max[1]);

			this->Source->SetBounds(
				this->GridKeyword.m_grid[0].coord[this->Min[0]],
				this->GridKeyword.m_grid[0].coord[this->Max[0]],
				this->GridKeyword.m_grid[1].coord[this->Min[1]],
				this->GridKeyword.m_grid[1].coord[this->Max[1]],
				bounds[4],
				bounds[5]
				);
			this->Actor->SetVisibility(1);
		}
		else
		{
			vtkFloatingPointType* bounds = this->GridActor->GetBounds();

#if defined(SELECT_WITH_INTERSECTION)
			// intersection 
			//
			this->Min[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[0]);
			this->Min[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[2]);
			this->Max[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[1]) + 1;
			this->Max[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[3]) + 1;
#else
// COMMENT: {8/24/2005 4:47:47 PM}			// interior only
// COMMENT: {8/24/2005 4:47:47 PM}			//
// COMMENT: {8/24/2005 4:47:47 PM}			this->Min[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 4:47:47 PM}			this->Min[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[2]);
// COMMENT: {8/24/2005 4:47:47 PM}			this->Max[0] = this->GridKeyword.m_grid[0].lower_bound(this->OutlineSource->GetBounds()[1]) - 1;
// COMMENT: {8/24/2005 4:47:47 PM}			this->Max[1] = this->GridKeyword.m_grid[1].lower_bound(this->OutlineSource->GetBounds()[3]) - 1;
			// interior only
			//
// COMMENT: {8/24/2005 5:00:13 PM}			this->Min[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[0]);
// COMMENT: {8/24/2005 5:00:13 PM}			this->Min[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[2]);

			this->Min[0] = this->GridKeyword.m_grid[0].lower_bound(this->OutlineSource->GetBounds()[0]);
			this->Min[1] = this->GridKeyword.m_grid[1].lower_bound(this->OutlineSource->GetBounds()[2]);

			this->Max[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[1]) - 1;
			this->Max[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[3]) - 1;
#endif
			TRACE("No Plane\n");
			TRACE("X (%d, %d)\n", this->Min[0], this->Max[0]);
			TRACE("Y (%d, %d)\n", this->Min[1], this->Max[1]);

			this->Source->SetBounds(
				this->GridKeyword.m_grid[0].coord[this->Min[0]],
				this->GridKeyword.m_grid[0].coord[this->Max[0]],
				this->GridKeyword.m_grid[1].coord[this->Min[1]],
				this->GridKeyword.m_grid[1].coord[this->Max[1]],
				bounds[4],
				bounds[5]
				);
			this->Actor->SetVisibility(1);
		}

		this->EventCallbackCommand->SetAbortFlag(1);
		this->Interactor->Render();
	}
}

void CGridLineSelector::OnLeftButtonDown(void)
{
	ASSERT ( this->State == CGridLineSelector::Start );
	if ( this->State == CGridLineSelector::Start )
	{
		//{{
		vtkFloatingPointType viewPlaneNormal[3];
		vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
		camera->GetViewPlaneNormal(viewPlaneNormal);
		//}}

		int imax;
		int fixed[3];
		double max = 0.0;
		this->GridKeyword = this->GridActor->m_gridKeyword;
		for (int i = 0; i < 3; ++i)
		{
			this->GridKeyword.m_grid[i].Setup();
			fixed[i] = 0;
			if (max < ::fabs(viewPlaneNormal[i]))
			{
				max = ::fabs(viewPlaneNormal[i]);
				imax = i;
			}
		}
		fixed[imax] = 1;
		if (fixed[0]) return;
		if (fixed[1]) return;

		this->State = CGridLineSelector::Selecting;

		vtkFloatingPointType* bounds = this->GridActor->GetBounds();

		::GetXYatZ(this->Interactor, this->CurrentRenderer, bounds[5], this->StartPoint);

		//bounds[0] = bounds[1] = this->StartPoint[0];
		//bounds[2] = bounds[3] = this->StartPoint[1];
		///this->OutlineSource->SetBounds(bounds);
		this->OutlineSource->SetBounds(
			this->StartPoint[0],
			this->StartPoint[0],
			this->StartPoint[1],
			this->StartPoint[1],
			bounds[4],
			bounds[5]
            );
		this->OutlineActor->SetVisibility(1);

		//{{
		int X = this->Interactor->GetEventPosition()[0];
		int Y = this->Interactor->GetEventPosition()[1];

		if (this->GridActor->PlanePicker->Pick(X, Y, 0, this->CurrentRenderer))
		{
			vtkFloatingPointType* bounds = this->GridActor->GetBounds();

#if defined(SELECT_WITH_INTERSECTION)
			// intersection 
			//
			this->Min[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[0]);
			this->Min[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[2]);
			this->Max[0] = this->GridKeyword.m_grid[0].Element(this->OutlineSource->GetBounds()[1]) + 1;
			this->Max[1] = this->GridKeyword.m_grid[1].Element(this->OutlineSource->GetBounds()[3]) + 1;
#else
			// interior only
			//
			this->Min[0] = this->GridKeyword.m_grid[0].upper_bound(this->OutlineSource->GetBounds()[0]);
			this->Min[1] = this->GridKeyword.m_grid[1].upper_bound(this->OutlineSource->GetBounds()[2]);
			this->Max[0] = this->GridKeyword.m_grid[0].lower_bound(this->OutlineSource->GetBounds()[1]) - 1;
			this->Max[1] = this->GridKeyword.m_grid[1].lower_bound(this->OutlineSource->GetBounds()[3]) - 1;
#endif


			//if (xstart != -1)
			{
				this->Source->SetBounds(
					this->GridKeyword.m_grid[0].coord[this->Min[0]],
					this->GridKeyword.m_grid[0].coord[this->Max[0]],
					this->GridKeyword.m_grid[1].coord[this->Min[1]],
					this->GridKeyword.m_grid[1].coord[this->Max[1]],
					bounds[4],
					bounds[5]
					);
				this->Actor->SetVisibility(1);
				this->Interactor->Render();
			}


// COMMENT: {8/23/2005 6:27:32 PM}			vtkFloatingPointType bounds[6];
// COMMENT: {8/23/2005 6:27:32 PM}			this->GridActor->GetBounds(bounds);
// COMMENT: {8/23/2005 6:27:32 PM}
// COMMENT: {8/23/2005 6:27:32 PM}			CGrid grid = this->GridActor->m_gridKeyword.m_grid[0];
// COMMENT: {8/23/2005 6:27:32 PM}			grid.Setup();
// COMMENT: {8/23/2005 6:27:32 PM}
// COMMENT: {8/23/2005 6:27:32 PM}			this->GridActor->PlanePicker->GetPickPosition(zMaxPosition);
// COMMENT: {8/23/2005 6:27:32 PM}			start_element = grid.Element(zMaxPosition[0]);
// COMMENT: {8/23/2005 6:27:32 PM}			//{{
// COMMENT: {8/23/2005 6:27:32 PM}			if (start_element == -1)
// COMMENT: {8/23/2005 6:27:32 PM}			{
// COMMENT: {8/23/2005 6:27:32 PM}				start_element = grid.Closest(zMaxPosition[0]);
// COMMENT: {8/23/2005 6:27:32 PM}				if (start_element != 0)
// COMMENT: {8/23/2005 6:27:32 PM}				{
// COMMENT: {8/23/2005 6:27:32 PM}					--start_element;
// COMMENT: {8/23/2005 6:27:32 PM}				}
// COMMENT: {8/23/2005 6:27:32 PM}			}
// COMMENT: {8/23/2005 6:27:32 PM}			//}}
// COMMENT: {8/23/2005 6:27:32 PM}			if (start_element != -1)
// COMMENT: {8/23/2005 6:27:32 PM}			{
// COMMENT: {8/23/2005 6:27:32 PM}				bounds[0] = grid.coord[start_element];
// COMMENT: {8/23/2005 6:27:32 PM}				bounds[1] = grid.coord[start_element+1];
// COMMENT: {8/23/2005 6:27:32 PM}				self->Source->SetBounds(bounds);
// COMMENT: {8/23/2005 6:27:32 PM}				self->Actor->SetVisibility(1);
// COMMENT: {8/23/2005 6:27:32 PM}				self->State = CGridLineSelector::Selecting;
// COMMENT: {8/23/2005 6:27:32 PM}				self->EventCallbackCommand->SetAbortFlag(1);
// COMMENT: {8/23/2005 6:27:32 PM}				self->Interactor->Render();
// COMMENT: {8/23/2005 6:27:32 PM}			}
// COMMENT: {8/23/2005 6:27:32 PM}			else
// COMMENT: {8/23/2005 6:27:32 PM}			{
// COMMENT: {8/23/2005 6:27:32 PM}				ASSERT(FALSE);
// COMMENT: {8/23/2005 6:27:32 PM}			}
		}
		else
		{
		}
		//}}


		this->EventCallbackCommand->SetAbortFlag(1);
		this->Interactor->Render();
	}
}

void CGridLineSelector::OnLeftButtonUp(void)
{
	if ( this->State == CGridLineSelector::Selecting )
	{
		this->State = CGridLineSelector::Start;
		this->EventCallbackCommand->SetAbortFlag(1);
// COMMENT: {8/24/2005 5:59:58 PM}		this->SetEnabled(0);
		this->Interactor->Render();
	}
}
