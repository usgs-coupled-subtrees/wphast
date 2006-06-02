/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkPointWidget2.cxx,v $
  Language:  C++
  Date:      $Date: 2004/08/31 02:21:49 $
  Version:   $Revision$

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "StdAfx.h"
#include "vtkPointWidget2.h"

#include "vtkActor.h"
#include "vtkAssemblyNode.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkPointWidget2, "$Revision$");
vtkStandardNewMacro(vtkPointWidget2);

vtkPointWidget2::vtkPointWidget2()
{
  this->State = vtkPointWidget2::Start;
  this->EventCallbackCommand->SetCallback(vtkPointWidget2::ProcessEvents);
  
  // Represent the line
  this->Cursor3D = vtkCursor3D::New();
  this->Mapper = vtkPolyDataMapper::New();
  this->Mapper->SetInput(this->Cursor3D->GetOutput());
  this->Actor = vtkActor::New();
  this->Actor->SetMapper(this->Mapper);

  // Define the point coordinates
  vtkFloatingPointType bounds[6];
  bounds[0] = -0.5;
  bounds[1] = 0.5;
  bounds[2] = -0.5;
  bounds[3] = 0.5;
  bounds[4] = -0.5;
  bounds[5] = 0.5;

  // Initial creation of the widget, serves to initialize it
  this->PlaceWidget(bounds);

  //Manage the picking stuff
  this->CursorPicker = vtkCellPicker::New();
  this->CursorPicker->PickFromListOn();
  this->CursorPicker->AddPickList(this->Actor);
  this->CursorPicker->SetTolerance(0.005); //need some fluff

  // Set up the initial properties
  this->CreateDefaultProperties();

  // Constraints not set
  this->ConstraintAxis = -1;

  // Override superclass'
  this->PlaceFactor = 1.0;
  
  // The size of the hot spot
  this->HotSpotSize = 0.05;
  this->WaitingForMotion = 0;
}

vtkPointWidget2::~vtkPointWidget2()
{
  this->Actor->Delete();
  this->Mapper->Delete();
  this->Cursor3D->Delete();

  this->CursorPicker->Delete();

  this->Property->Delete();
  this->SelectedProperty->Delete();
}

void vtkPointWidget2::SetEnabled(int enabling)
{
  if ( ! this->Interactor )
    {
    vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
    return;
    }

  if ( enabling ) //-----------------------------------------------------------
    {
    vtkDebugMacro(<<"Enabling point widget");

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

    // listen for the following events
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

    // Add the line
    this->CurrentRenderer->AddActor(this->Actor);
    this->Actor->SetProperty(this->Property);
    this->Cursor3D->Update();

    this->InvokeEvent(vtkCommand::EnableEvent,NULL);
    }
  
  else //disabling----------------------------------------------------------
    {
    vtkDebugMacro(<<"Disabling point widget");

    if ( ! this->Enabled ) //already disabled, just return
      {
      return;
      }
    
    this->Enabled = 0;

    // don't listen for events any more
    this->Interactor->RemoveObserver(this->EventCallbackCommand);

    // turn off the line
    this->CurrentRenderer->RemoveActor(this->Actor);

    this->InvokeEvent(vtkCommand::DisableEvent,NULL);
    this->CurrentRenderer = NULL;
    }

  this->Interactor->Render();
}

void vtkPointWidget2::ProcessEvents(vtkObject* vtkNotUsed(object), 
                                   unsigned long event,
                                   void* clientdata, 
                                   void* vtkNotUsed(calldata))
{
  vtkPointWidget2* self = reinterpret_cast<vtkPointWidget2 *>( clientdata );

  //okay, let's do the right thing
  switch(event)
    {
    case vtkCommand::LeftButtonPressEvent:
      self->OnLeftButtonDown();
      break;
    case vtkCommand::LeftButtonReleaseEvent:
      self->OnLeftButtonUp();
      break;
    case vtkCommand::MiddleButtonPressEvent:
      /// self->OnMiddleButtonDown();
      break;
    case vtkCommand::MiddleButtonReleaseEvent:
      /// self->OnMiddleButtonUp();
      break;
    case vtkCommand::RightButtonPressEvent:
      /// self->OnRightButtonDown();
      break;
    case vtkCommand::RightButtonReleaseEvent:
      /// self->OnRightButtonUp();
      break;
    case vtkCommand::MouseMoveEvent:
      self->OnMouseMove();
      break;
    }
}

void vtkPointWidget2::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if ( this->Property )
    {
    os << indent << "Property: " << this->Property << "\n";
    }
  else
    {
    os << indent << "Property: (none)\n";
    }
  if ( this->SelectedProperty )
    {
    os << indent << "Selected Property: " << this->SelectedProperty << "\n";
    }
  else
    {
    os << indent << "Selected Property: (none)\n";
    }

  vtkFloatingPointType *pos = this->Cursor3D->GetFocalPoint();
  os << indent << "Position: (" << pos[0] << ", "
               << pos[1] << ", " << pos[2] << ")\n";

  os << indent << "Outline: " << (this->GetOutline() ? "On\n" : "Off\n");
  os << indent << "XShadows: " << (this->GetXShadows() ? "On\n" : "Off\n");
  os << indent << "YShadows: " << (this->GetYShadows() ? "On\n" : "Off\n");
  os << indent << "ZShadows: " << (this->GetZShadows() ? "On\n" : "Off\n");

  os << indent << "Translation Mode: " 
     << (this->Cursor3D->GetTranslationMode() ? "On\n" : "Off\n");

  os << indent << "Hot Spot Size: " << this->HotSpotSize << "\n";
}

void vtkPointWidget2::Highlight(int highlight)
{
  if ( highlight )
    {
    this->Actor->SetProperty(this->SelectedProperty);
    this->CursorPicker->GetPickPosition(this->LastPickPosition);
    this->ValidPick = 1;
    }
  else
    {
    this->Actor->SetProperty(this->Property);
    }
}

int vtkPointWidget2::DetermineConstraintAxis(int constraint, double *x)
{
  // Look for trivial cases
  if ( ! this->Interactor->GetShiftKey() )
    {
    return -1;
    }
  else if ( constraint >= 0 && constraint < 3 )
    {
    return constraint;
    }
  
  // Okay, figure out constraint. First see if the choice is
  // outside the hot spot
  if ( ! this->WaitingForMotion )
    {
    vtkFloatingPointType p[3], d2, tol;
    this->CursorPicker->GetPickPosition(p);
    d2 = vtkMath::Distance2BetweenPoints(p,this->LastPickPosition);
    tol = this->HotSpotSize*this->InitialLength;
    if ( d2 > (tol*tol) )
      {
      this->WaitingForMotion = 0;
      return this->CursorPicker->GetCellId();
      }
    else
      {
      this->WaitingForMotion = 1;
      this->WaitCount = 0;
      return -1;
      }
    }
  else if ( this->WaitingForMotion && x ) 
    {
    vtkFloatingPointType v[3];
    this->WaitingForMotion = 0;
    v[0] = fabs(x[0] - this->LastPickPosition[0]);
    v[1] = fabs(x[1] - this->LastPickPosition[1]);
    v[2] = fabs(x[2] - this->LastPickPosition[2]);
    return ( v[0]>v[1] ? (v[0]>v[2]?0:2) : (v[1]>v[2]?1:2));
    }
  else
    {
    return -1;
    }
}

void vtkPointWidget2::OnLeftButtonDown()
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
  if ( ren != this->CurrentRenderer )
    {
    this->State = vtkPointWidget2::Outside;
    return;
    }
  
  vtkAssemblyPath *path;
  this->CursorPicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->CursorPicker->GetPath();
  if ( path != NULL )
    {
    this->State = vtkPointWidget2::Moving;
    this->Highlight(1);
    this->ConstraintAxis = this->DetermineConstraintAxis(-1,NULL);
    }
  else
    {
    this->State = vtkPointWidget2::Outside;
    this->Highlight(0);
    this->ConstraintAxis = -1;
    return;
    }
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  this->Interactor->Render();
}

void vtkPointWidget2::OnLeftButtonUp()
{
  if ( this->State == vtkPointWidget2::Outside ||
       this->State == vtkPointWidget2::Start )
    {
    return;
    }

  this->State = vtkPointWidget2::Start;
  this->Highlight(0);

  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  this->Interactor->Render();
}

void vtkPointWidget2::OnMiddleButtonDown()
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
  if ( ren != this->CurrentRenderer )
    {
    this->State = vtkPointWidget2::Outside;
    return;
    }
  
  // Okay, we can process this.
  vtkAssemblyPath *path;
  this->CursorPicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->CursorPicker->GetPath();
  if ( path != NULL )
    {
    this->State = vtkPointWidget2::Translating;
    this->Highlight(1);
    this->ConstraintAxis = this->DetermineConstraintAxis(-1,NULL);
    }
  else
    {
    this->State = vtkPointWidget2::Outside;
    this->ConstraintAxis = -1;
    return;
    }
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  this->Interactor->Render();
}

void vtkPointWidget2::OnMiddleButtonUp()
{
  if ( this->State == vtkPointWidget2::Outside ||
       this->State == vtkPointWidget2::Start )
    {
    return;
    }

  this->State = vtkPointWidget2::Start;
  this->Highlight(0);
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  this->Interactor->Render();
}

void vtkPointWidget2::OnRightButtonDown()
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
  if ( ren != this->CurrentRenderer )
    {
    this->State = vtkPointWidget2::Outside;
    return;
    }
  
  // Okay, we can process this. Pick the cursor.
  vtkAssemblyPath *path;
  this->CursorPicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->CursorPicker->GetPath();
  if ( path != NULL )
    {
    this->State = vtkPointWidget2::Scaling;
    int idx = this->CursorPicker->GetCellId();
    if ( idx >= 0 && idx < 3 )
      {
      this->ConstraintAxis = idx;
      }
    this->Highlight(1);
    }
  else
    {
    this->State = vtkPointWidget2::Outside;
    this->ConstraintAxis = -1;
    return;
    }
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  this->Interactor->Render();
}

void vtkPointWidget2::OnRightButtonUp()
{
  if ( this->State == vtkPointWidget2::Outside ||
       this->State == vtkPointWidget2::Start )
    {
    return;
    }

  this->State = vtkPointWidget2::Start;
  this->Highlight(0);
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  this->Interactor->Render();
}

void vtkPointWidget2::OnMouseMove()
{
  // See whether we're active
  if ( this->State == vtkPointWidget2::Outside || 
       this->State == vtkPointWidget2::Start )
    {
    return;
    }
  
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Do different things depending on state
  // Calculations everybody does
  double focalPoint[4], pickPoint[4], prevPickPoint[4];
  double z;

  vtkRenderer *renderer = this->Interactor->FindPokedRenderer(X,Y);
  vtkCamera *camera = renderer->GetActiveCamera();
  if ( !camera )
    {
    return;
    }

  // Compute the two points defining the motion vector
  this->ComputeWorldToDisplay(this->LastPickPosition[0], this->LastPickPosition[1],
                              this->LastPickPosition[2], focalPoint);
  z = focalPoint[2];
  this->ComputeDisplayToWorld(double(this->Interactor->GetLastEventPosition()[0]),double(this->Interactor->GetLastEventPosition()[1]),
                              z, prevPickPoint);
  this->ComputeDisplayToWorld(double(X), double(Y), z, pickPoint);
// COMMENT: {8/30/2004 5:34:55 PM}	//{{
// COMMENT: {8/30/2004 5:34:55 PM}	TRACE("focalPoint = (%g, %g, %g)\n", focalPoint[0], focalPoint[1], focalPoint[2]);
// COMMENT: {8/30/2004 5:34:55 PM}	TRACE("prevPickPoint = (%g, %g, %g)\n", prevPickPoint[0], prevPickPoint[1], prevPickPoint[2]);
// COMMENT: {8/30/2004 5:34:55 PM}	TRACE("pickPoint = (%g, %g, %g)\n", pickPoint[0], pickPoint[1], pickPoint[2]);
// COMMENT: {8/30/2004 5:34:55 PM}
// COMMENT: {8/30/2004 5:34:55 PM}
// COMMENT: {8/30/2004 5:34:55 PM}  	float cameraFP[4];
// COMMENT: {8/30/2004 5:34:55 PM}	camera->GetFocalPoint(cameraFP);
// COMMENT: {8/30/2004 5:34:55 PM}	cameraFP[3] = 1.0;
// COMMENT: {8/30/2004 5:34:55 PM}	TRACE("cameraFP = (%g, %g, %g)\n", cameraFP[0], cameraFP[1], cameraFP[2]);
// COMMENT: {8/30/2004 5:34:55 PM}
// COMMENT: {8/30/2004 5:34:55 PM}	// Convert the focal point coordinates to display (or screen) coordinates.
// COMMENT: {8/30/2004 5:34:55 PM}	//
// COMMENT: {8/30/2004 5:34:55 PM}	renderer->SetWorldPoint(cameraFP);
// COMMENT: {8/30/2004 5:34:55 PM}	renderer->WorldToDisplay();
// COMMENT: {8/30/2004 5:34:55 PM}	float *displayCoords = renderer->GetDisplayPoint();
// COMMENT: {8/30/2004 5:34:55 PM}
// COMMENT: {8/30/2004 5:34:55 PM}	int* pos = this->Interactor->GetEventPosition();
// COMMENT: {8/30/2004 5:34:55 PM}	int* lastPos = this->Interactor->GetLastEventPosition();
// COMMENT: {8/30/2004 5:34:55 PM}
// COMMENT: {8/30/2004 5:34:55 PM}	float *worldCoords;
// COMMENT: {8/30/2004 5:34:55 PM}
// COMMENT: {8/30/2004 5:34:55 PM}	// Convert the selection point into world coordinates.
// COMMENT: {8/30/2004 5:34:55 PM}	//
// COMMENT: {8/30/2004 5:34:55 PM}	renderer->SetDisplayPoint(pos[0], pos[1], displayCoords[2]);
// COMMENT: {8/30/2004 5:34:55 PM}	renderer->DisplayToWorld();
// COMMENT: {8/30/2004 5:34:55 PM}	worldCoords = renderer->GetWorldPoint();
// COMMENT: {8/30/2004 5:34:55 PM}	if ( worldCoords[3] == 0.0 )
// COMMENT: {8/30/2004 5:34:55 PM}	{
// COMMENT: {8/30/2004 5:34:55 PM}		ASSERT(FALSE);
// COMMENT: {8/30/2004 5:34:55 PM}	}
// COMMENT: {8/30/2004 5:34:55 PM}	float PickPosition[3];
// COMMENT: {8/30/2004 5:34:55 PM}	for (int i = 0; i < 3; ++i)
// COMMENT: {8/30/2004 5:34:55 PM}	{
// COMMENT: {8/30/2004 5:34:55 PM}		PickPosition[i] = worldCoords[i] / worldCoords[3];
// COMMENT: {8/30/2004 5:34:55 PM}	}
// COMMENT: {8/30/2004 5:34:55 PM}
// COMMENT: {8/30/2004 5:34:55 PM}	////{{{
// COMMENT: {8/30/2004 5:34:55 PM}	renderer->SetDisplayPoint(lastPos[0], lastPos[1], displayCoords[2]);
// COMMENT: {8/30/2004 5:34:55 PM}	renderer->DisplayToWorld();
// COMMENT: {8/30/2004 5:34:55 PM}	worldCoords = renderer->GetWorldPoint();
// COMMENT: {8/30/2004 5:34:55 PM}	if ( worldCoords[3] == 0.0 )
// COMMENT: {8/30/2004 5:34:55 PM}	{
// COMMENT: {8/30/2004 5:34:55 PM}		ASSERT(FALSE);
// COMMENT: {8/30/2004 5:34:55 PM}	}
// COMMENT: {8/30/2004 5:34:55 PM}	float PrevPickPosition[3];
// COMMENT: {8/30/2004 5:34:55 PM}	for (int i = 0; i < 3; ++i)
// COMMENT: {8/30/2004 5:34:55 PM}	{
// COMMENT: {8/30/2004 5:34:55 PM}		PrevPickPosition[i] = worldCoords[i] / worldCoords[3];
// COMMENT: {8/30/2004 5:34:55 PM}	}
// COMMENT: {8/30/2004 5:34:55 PM}	////}}}
// COMMENT: {8/30/2004 5:34:55 PM}
// COMMENT: {8/30/2004 5:34:55 PM}	float zPos = 0.0;
// COMMENT: {8/30/2004 5:34:55 PM}	double MypickPoint[3];
// COMMENT: {8/30/2004 5:34:55 PM}	double MyprevPickPoint[3];
// COMMENT: {8/30/2004 5:34:55 PM}	if ( camera->GetParallelProjection() )
// COMMENT: {8/30/2004 5:34:55 PM}	{
// COMMENT: {8/30/2004 5:34:55 PM}		double* cameraDOP = camera->GetDirectionOfProjection();
// COMMENT: {8/30/2004 5:34:55 PM}		double t = (zPos - PickPosition[2]) / cameraDOP[2];
// COMMENT: {8/30/2004 5:34:55 PM}		for (i = 0; i < 2; ++i) {
// COMMENT: {8/30/2004 5:34:55 PM}			MypickPoint[i] = PickPosition[i] + t * cameraDOP[i];
// COMMENT: {8/30/2004 5:34:55 PM}			MyprevPickPoint[i] = PrevPickPosition[i] + t * cameraDOP[i];
// COMMENT: {8/30/2004 5:34:55 PM}		}
// COMMENT: {8/30/2004 5:34:55 PM}	}
// COMMENT: {8/30/2004 5:34:55 PM}	else
// COMMENT: {8/30/2004 5:34:55 PM}	{
// COMMENT: {8/30/2004 5:34:55 PM}		double *cameraPos = camera->GetPosition();
// COMMENT: {8/30/2004 5:34:55 PM}		double t = (zPos - cameraPos[2]) / ( PickPosition[2] - cameraPos[2] );
// COMMENT: {8/30/2004 5:34:55 PM}		for (i = 0; i < 2; ++i) {
// COMMENT: {8/30/2004 5:34:55 PM}			MypickPoint[i] = cameraPos[i] + t * ( PickPosition[i] - cameraPos[i] );
// COMMENT: {8/30/2004 5:34:55 PM}			MyprevPickPoint[i] = cameraPos[i] + t * ( PrevPickPosition[i] - cameraPos[i] );
// COMMENT: {8/30/2004 5:34:55 PM}		}
// COMMENT: {8/30/2004 5:34:55 PM}	}
// COMMENT: {8/30/2004 5:34:55 PM}	MypickPoint[2] = zPos;
// COMMENT: {8/30/2004 5:34:55 PM}	TRACE("MypickPoint = (%g, %g, %g)\n", MypickPoint[0], MypickPoint[1], MypickPoint[2]);
// COMMENT: {8/30/2004 5:34:55 PM}	TRACE("MyprevPickPoint = (%g, %g, %g)\n", MyprevPickPoint[0], MyprevPickPoint[1], MyprevPickPoint[2]);
// COMMENT: {8/30/2004 5:34:55 PM}
// COMMENT: {8/30/2004 5:34:55 PM}	////{{
// COMMENT: {8/30/2004 5:34:55 PM}	for (int i = 0; i < 3; ++i)
// COMMENT: {8/30/2004 5:34:55 PM}	{
// COMMENT: {8/30/2004 5:34:55 PM}		pickPoint[i] = MypickPoint[i];
// COMMENT: {8/30/2004 5:34:55 PM}		prevPickPoint[i] = MyprevPickPoint[i];
// COMMENT: {8/30/2004 5:34:55 PM}	}
// COMMENT: {8/30/2004 5:34:55 PM}	////}}
// COMMENT: {8/30/2004 5:34:55 PM}	//}}

  // Process the motion
  if ( this->State == vtkPointWidget2::Moving )
    {
    if ( !this->WaitingForMotion || this->WaitCount++ > 3 )
      {
      this->ConstraintAxis = 
        this->DetermineConstraintAxis(this->ConstraintAxis,pickPoint);
      this->MoveFocus(prevPickPoint, pickPoint);
      }
    else
      {
      return; //avoid the extra render
      }
    }

  else if ( this->State == vtkPointWidget2::Scaling )
    {
    this->Scale(prevPickPoint, pickPoint, X, Y);
    }

  else if ( this->State == vtkPointWidget2::Translating )
    {
    if ( !this->WaitingForMotion || this->WaitCount++ > 3 )
      {
      this->ConstraintAxis = 
        this->DetermineConstraintAxis(this->ConstraintAxis,pickPoint);
      this->Translate(prevPickPoint, pickPoint);
      }
    else
      {
      return; //avoid the extra render
      }
    }

  // Interact, if desired
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkCommand::InteractionEvent,NULL);
  this->Interactor->Render();
}

void vtkPointWidget2::MoveFocus(double *p1, double *p2)
{
  //Get the motion vector
  double v[3];
  v[0] = p2[0] - p1[0];
  v[1] = p2[1] - p1[1];
  v[2] = p2[2] - p1[2];

	//{{
	// constrain z
	v[2] = 0.0;
	//}}


  vtkFloatingPointType focus[3];
  this->Cursor3D->GetFocalPoint(focus);
  if ( this->ConstraintAxis >= 0 )
    {
    focus[this->ConstraintAxis] += v[this->ConstraintAxis];
    }
  else
    {
    focus[0] += v[0];
    focus[1] += v[1];
    focus[2] += v[2];
    }
  
  this->Cursor3D->SetFocalPoint(focus);
}

// Translate everything
void vtkPointWidget2::Translate(double *p1, double *p2)
{
  //Get the motion vector
  double v[3];
  v[0] = p2[0] - p1[0];
  v[1] = p2[1] - p1[1];
  v[2] = p2[2] - p1[2];
  
  vtkFloatingPointType *bounds = this->Cursor3D->GetModelBounds();
  vtkFloatingPointType *pos = this->Cursor3D->GetFocalPoint();
  vtkFloatingPointType newBounds[6], newFocus[3];
  int i;

  if ( this->ConstraintAxis >= 0 )
    {//move along axis
    for (i=0; i<3; i++)
      {
      if ( i != this->ConstraintAxis )
        {
        v[i] = 0.0;
        }
      }
    }

	//{{
	// constrain z
	v[2] = 0.0;
	//}}
  
  for (i=0; i<3; i++)
    {
    newBounds[2*i] = bounds[2*i] + v[i];
    newBounds[2*i+1] = bounds[2*i+1] + v[i];
    newFocus[i] = pos[i] + v[i];
    }
  
  this->Cursor3D->SetModelBounds(newBounds);
  this->Cursor3D->SetFocalPoint(newFocus);
}

void vtkPointWidget2::Scale(double *p1, double *p2, int vtkNotUsed(X), int Y)
{
  //Get the motion vector
  double v[3];
  v[0] = p2[0] - p1[0];
  v[1] = p2[1] - p1[1];
  v[2] = p2[2] - p1[2];

  //int res = this->Cursor3D->GetResolution();
  vtkFloatingPointType *bounds = this->Cursor3D->GetModelBounds();
  vtkFloatingPointType *focus = this->Cursor3D->GetFocalPoint();

  // Compute the scale factor
  vtkFloatingPointType sf = vtkMath::Norm(v) / 
    sqrt( (bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
          (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
          (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));

  if ( Y > this->Interactor->GetLastEventPosition()[1] )
    {
    sf = 1.0 + sf;
    }
  else
    {
    sf = 1.0 - sf;
    }
  
  // Move the end points
  vtkFloatingPointType newBounds[6];
  for (int i=0; i<3; i++)
    {
    newBounds[2*i] = sf * (bounds[2*i] - focus[i]) + focus[i];
    newBounds[2*i+1] = sf * (bounds[2*i+1] - focus[i]) + focus[i];
    }

  this->Cursor3D->SetModelBounds(newBounds);
  this->Cursor3D->Update();
}

void vtkPointWidget2::CreateDefaultProperties()
{
  this->Property = vtkProperty::New();
  this->Property->SetAmbient(1.0);
  this->Property->SetAmbientColor(1.0,1.0,1.0);
  this->Property->SetLineWidth(0.5);

  this->SelectedProperty = vtkProperty::New();
  this->SelectedProperty->SetAmbient(1.0);
  this->SelectedProperty->SetAmbientColor(0.0,1.0,0.0);
  this->SelectedProperty->SetLineWidth(2.0);
}

void vtkPointWidget2::PlaceWidget(vtkFloatingPointType bds[6])
{
  int i;
  vtkFloatingPointType bounds[6], center[3];

  this->AdjustBounds(bds, bounds, center);
  
  this->Cursor3D->SetModelBounds(bounds);
  this->Cursor3D->SetFocalPoint(center);
  this->Cursor3D->Update();

  for (i=0; i<6; i++)
    {
    this->InitialBounds[i] = bounds[i];
    }
  this->InitialLength = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
                             (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
                             (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));

}

void vtkPointWidget2::GetPolyData(vtkPolyData *pd)
{
  this->Cursor3D->Update(); 
  pd->DeepCopy(this->Cursor3D->GetFocus()); 
}
