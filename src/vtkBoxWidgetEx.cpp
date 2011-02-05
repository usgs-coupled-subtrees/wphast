#include "StdAfx.h"
#include "vtkBoxWidgetEx.h"

vtkCxxRevisionMacro(vtkBoxWidgetEx, "$Revision: 493 $");
vtkStandardNewMacro(vtkBoxWidgetEx);


vtkBoxWidgetEx::vtkBoxWidgetEx(void)
{
	this->EventCallbackCommand->SetCallback(vtkBoxWidgetEx::ProcessEvents);
	this->HexPickerEnabled = 1;

	this->HandleSize = 0.008 / 1.5;
}

vtkBoxWidgetEx::~vtkBoxWidgetEx(void)
{
}

// modified to add handling of vtkCommand::ModifiedEvent
void vtkBoxWidgetEx::SetEnabled(int enabling)
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
    i->AddObserver(vtkCommand::MiddleButtonPressEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::MiddleButtonReleaseEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::RightButtonPressEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::RightButtonReleaseEvent, 
                   this->EventCallbackCommand, this->Priority);
	//{{
    i->AddObserver(vtkCommand::ModifiedEvent, 
                   this->EventCallbackCommand, this->Priority);
	//}}

    // Add the various actors
    // Add the outline
    this->CurrentRenderer->AddActor(this->HexActor);
    this->CurrentRenderer->AddActor(this->HexOutline);
    this->HexActor->SetProperty(this->OutlineProperty);
    this->HexOutline->SetProperty(this->OutlineProperty);

    // Add the hex face
    this->CurrentRenderer->AddActor(this->HexFace);
    this->HexFace->SetProperty(this->FaceProperty);

    // turn on the handles
    for (int j=0; j<7; j++)
      {
      this->CurrentRenderer->AddActor(this->Handle[j]);
      this->Handle[j]->SetProperty(this->HandleProperty);
      }

    this->InvokeEvent(vtkCommand::EnableEvent,NULL);
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

    // turn off the outline
    this->CurrentRenderer->RemoveActor(this->HexActor);
    this->CurrentRenderer->RemoveActor(this->HexOutline);

    // turn off the hex face
    this->CurrentRenderer->RemoveActor(this->HexFace);

    // turn off the handles
    for (int i=0; i<7; i++)
      {
      this->CurrentRenderer->RemoveActor(this->Handle[i]);
      }

    this->CurrentHandle = NULL;
    this->InvokeEvent(vtkCommand::DisableEvent,NULL);
    this->CurrentRenderer = NULL;
    }
  
  this->Interactor->Render();
}

void vtkBoxWidgetEx::ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata)
{
	vtkBoxWidgetEx* self = reinterpret_cast<vtkBoxWidgetEx *>( clientdata );

	//okay, let's do the right thing
	switch(event)
	{
	case vtkCommand::LeftButtonPressEvent:
		self->OnLeftButtonDown();
		break;
	case vtkCommand::LeftButtonReleaseEvent:
		self->OnLeftButtonUp();
		break;
// COMMENT: {8/13/2008 5:47:41 PM}	case vtkCommand::MiddleButtonPressEvent:
// COMMENT: {8/13/2008 5:47:41 PM}		self->OnMiddleButtonDown();
// COMMENT: {8/13/2008 5:47:41 PM}		break;
// COMMENT: {8/13/2008 5:47:41 PM}	case vtkCommand::MiddleButtonReleaseEvent:
// COMMENT: {8/13/2008 5:47:41 PM}		self->OnMiddleButtonUp();
// COMMENT: {8/13/2008 5:47:41 PM}		break;
// COMMENT: {8/13/2008 5:47:48 PM}	case vtkCommand::RightButtonPressEvent:
// COMMENT: {8/13/2008 5:47:48 PM}		self->OnRightButtonDown();
// COMMENT: {8/13/2008 5:47:48 PM}		break;
// COMMENT: {8/13/2008 5:47:48 PM}	case vtkCommand::RightButtonReleaseEvent:
// COMMENT: {8/13/2008 5:47:48 PM}		self->OnRightButtonUp();
// COMMENT: {8/13/2008 5:47:48 PM}		break;
	case vtkCommand::MouseMoveEvent:
		self->OnMouseMove();
// COMMENT: {7/22/2009 9:16:09 PM}		//{{
// COMMENT: {7/22/2009 9:16:09 PM}		self->SizeHandles();
// COMMENT: {7/22/2009 9:16:09 PM}		//}}
		break;
	case vtkCommand::ModifiedEvent:
		self->SizeHandles();
		break;
	}
}

void vtkBoxWidgetEx::OnLeftButtonDown()
{
	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	// Okay, we can process this. Try to pick handles first;
	// if no handles picked, then pick the bounding box.
	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if ( ren != this->CurrentRenderer )
	{
		this->State = vtkBoxWidget::Outside;
		return;
	}

	vtkAssemblyPath *path;
	this->HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
	path = this->HandlePicker->GetPath();
	if ( path != NULL )
	{
		this->State = vtkBoxWidget::Moving;
		this->HighlightFace(
			this->HighlightHandle(path->GetFirstNode()->GetViewProp()));
		this->HandlePicker->GetPickPosition(this->LastPickPosition);
		this->ValidPick = 1;
	}
	else
	{
		this->HexPicker->Pick(X,Y,0.0,this->CurrentRenderer);
		path = this->HexPicker->GetPath();
		if ( path != NULL )
		{
			this->State = vtkBoxWidget::Moving;
			this->HexPicker->GetPickPosition(this->LastPickPosition);
			this->ValidPick = 1;
			if ( !this->Interactor->GetShiftKey() )
			{
				if (!this->HexPickerEnabled)
				{
					this->HighlightFace(this->HighlightHandle(NULL));
					this->State = vtkBoxWidget::Outside;
					return;
				}
				this->HighlightHandle(NULL);
				this->HighlightFace(this->HexPicker->GetCellId());
			}
			else
			{
				this->CurrentHandle = this->Handle[6];
				this->HighlightOutline(1);
			}
		}
		else
		{
			this->HighlightFace(this->HighlightHandle(NULL));
			this->State = vtkBoxWidget::Outside;
			return;
		}
	}

	this->EventCallbackCommand->SetAbortFlag(1);
	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	this->Interactor->Render();
}

//
// Modifications from vtkBoxWidget::SizeHandles()
// 1. Determine CurrentRenderer if NULL
// 2. Use vtkBoxWidgetEx::SizeHandles(double factor) instead
//    of vtk3DWidget::SizeHandles(float factor)
//
void vtkBoxWidgetEx::SizeHandles()
{
	if (!this->Interactor) return;

	if (!this->CurrentRenderer)
	{
		int X = this->Interactor->GetEventPosition()[0];
		int Y = this->Interactor->GetEventPosition()[1];
		this->CurrentRenderer = this->Interactor->FindPokedRenderer(X, Y);
	}

	double radius = this->SizeHandles(1.5);
	for(int i=0; i<7; i++)
	{
		this->HandleGeometry[i]->SetRadius(radius);
	}
}

//
// Modifications from vtk3DWidget::SizeHandles(float)
// 1. Use ActiveCamera's FocalPoint instead of last pick location
// 2. Some variables were changed from float to double
//
double vtkBoxWidgetEx::SizeHandles(double factor)
{
	TRACE("%s, in\n", __FUNCTION__);

	int i;
	vtkRenderer *renderer;

	if (!(renderer = this->CurrentRenderer) || !renderer->GetActiveCamera())
	{
		return (this->HandleSize * factor * this->InitialLength);
	}
	else
	{
		double radius, z;
		double windowLowerLeft[4], windowUpperRight[4];
		double *viewport = renderer->GetViewport();
		int *winSize = renderer->GetRenderWindow()->GetSize();
		double focalPoint[4];

		// get the focal point in world coordinates
		//
		vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
		double cameraFP[4];
		camera->GetFocalPoint(cameraFP); cameraFP[3] = 1.0;

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

		return (::sqrt(radius) * factor * this->HandleSize);
	}
}

void vtkBoxWidgetEx::RotateZ(float angle)
{
// COMMENT: {11/6/2008 2:08:25 PM}	double *pts = ((vtkDoubleArray *)this->Points->GetData())->GetPointer(0);
// COMMENT: {11/6/2008 2:08:25 PM}	double *center = ((vtkDoubleArray *)this->Points->GetData())->GetPointer(3*14);
// COMMENT: {12/14/2010 4:06:34 PM}	double m[16];
// COMMENT: {12/14/2010 4:06:34 PM}	vtkMatrix4x4 *m4x4 = this->Transform->GetMatrix();
// COMMENT: {12/14/2010 4:06:34 PM}	vtkMatrix4x4::DeepCopy(m, m4x4);
// COMMENT: {12/14/2010 4:06:34 PM}
// COMMENT: {12/14/2010 4:06:34 PM}	TRACE("\nvtkBoxWidgetEx::RotateZ before rotate:\n");
// COMMENT: {12/14/2010 4:06:34 PM}	TRACE("%g %g %g %g\n", m[0],  m[1],  m[2],  m[3]);
// COMMENT: {12/14/2010 4:06:34 PM}	TRACE("%g %g %g %g\n", m[4],  m[5],  m[6],  m[7]);
// COMMENT: {12/14/2010 4:06:34 PM}	TRACE("%g %g %g %g\n", m[8],  m[9],  m[10], m[11]);
// COMMENT: {12/14/2010 4:06:34 PM}	TRACE("%g %g %g %g\n", m[12], m[13], m[14], m[15]);

	int i;

	//Manipulate the transform to reflect the rotation
	this->Transform->Identity();
	this->Transform->RotateZ(angle);

	//Set the corners
	vtkPoints *newPts = vtkPoints::New(VTK_DOUBLE);
	this->Transform->TransformPoints(this->Points, newPts);

	//{{
	TRACE("\nthis->Points:\n");
	for (i = 0; i < 8; ++i)
	{
		double *d = this->Points->GetPoint(i);
		TRACE("  Point(%d) = %g, %g, %g\n", i, d[0], d[1], d[2]);
	}
	TRACE("\nnewPts:\n");
	for (i = 0; i < 8; ++i)
	{
		double *d = newPts->GetPoint(i);
		TRACE("  Point(%d) = %g, %g, %g\n", i, d[0], d[1], d[2]);
	}
	//}}

	for (i = 0; i < 8; ++i)
	{
		this->Points->SetPoint(i, newPts->GetPoint(i));
	}
	newPts->Delete();
	this->PositionHandles();
} 

void vtkBoxWidgetEx::SetOrientation(float x, float y, float z)
{
// COMMENT: {12/13/2010 4:00:26 PM}	//Manipulate the transform to reflect the rotation
// COMMENT: {12/13/2010 4:00:26 PM}
// COMMENT: {12/13/2010 4:00:26 PM}	this->Transform->Identity();
// COMMENT: {12/13/2010 4:00:26 PM}	this->Transform->PreMultiply();
// COMMENT: {12/13/2010 4:00:26 PM}	this->Transform->RotateZ(z);
// COMMENT: {12/13/2010 4:00:26 PM}	this->Transform->RotateX(x);
// COMMENT: {12/13/2010 4:00:26 PM}	this->Transform->RotateY(y);
// COMMENT: {12/13/2010 4:00:26 PM}
// COMMENT: {12/13/2010 4:00:26 PM}	// Set the corners
// COMMENT: {12/13/2010 4:00:26 PM}	vtkPoints *newPts = vtkPoints::New(VTK_DOUBLE);
// COMMENT: {12/13/2010 4:00:26 PM}	this->Transform->TransformPoints(this->Points, newPts);
// COMMENT: {12/13/2010 4:00:26 PM}
// COMMENT: {12/13/2010 4:00:26 PM}	for (int i = 0; i < 8; ++i)
// COMMENT: {12/13/2010 4:00:26 PM}	{
// COMMENT: {12/13/2010 4:00:26 PM}		this->Points->SetPoint(i, newPts->GetPoint(i));
// COMMENT: {12/13/2010 4:00:26 PM}	}
// COMMENT: {12/13/2010 4:00:26 PM}	newPts->Delete();
// COMMENT: {12/13/2010 4:00:26 PM}	this->PositionHandles();
} 
