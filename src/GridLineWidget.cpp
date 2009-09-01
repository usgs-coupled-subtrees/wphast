#include "StdAfx.h"
#include "GridLineWidget.h"

vtkCxxRevisionMacro(CGridLineWidget, "$Revision: 1.23 $");
vtkStandardNewMacro(CGridLineWidget);


CGridLineWidget::CGridLineWidget(void)
{
  this->EventCallbackCommand->SetCallback(CGridLineWidget::ProcessEvents);
  this->Picker->AddPickList(this->EdgesActor);
}

CGridLineWidget::~CGridLineWidget(void)
{
}

void CGridLineWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

void CGridLineWidget::SetEnabled(int enabling)
{
  if ( ! this->Interactor )
    {
    vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
    return;
    }

  if ( enabling ) //------------------------------------------------------------
    {
    vtkDebugMacro(<<"Enabling plane widget");

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

// COMMENT: {7/29/2005 6:07:08 PM}    // add the outline
// COMMENT: {7/29/2005 6:07:08 PM}    this->CurrentRenderer->AddActor(this->OutlineActor);
// COMMENT: {7/29/2005 6:07:08 PM}    this->OutlineActor->SetProperty(this->OutlineProperty);

    // add the edges
    this->CurrentRenderer->AddActor(this->EdgesActor);
    this->EdgesActor->SetProperty(this->EdgesProperty);

// COMMENT: {7/29/2005 6:13:09 PM}    // add the normal vector
// COMMENT: {7/29/2005 6:13:09 PM}    this->CurrentRenderer->AddActor(this->LineActor);
// COMMENT: {7/29/2005 6:13:09 PM}    this->LineActor->SetProperty(this->NormalProperty);
// COMMENT: {7/29/2005 6:13:09 PM}    this->CurrentRenderer->AddActor(this->ConeActor);
// COMMENT: {7/29/2005 6:13:09 PM}    this->ConeActor->SetProperty(this->NormalProperty);

// COMMENT: {7/29/2005 6:16:22 PM}    this->CurrentRenderer->AddActor(this->LineActor2);
// COMMENT: {7/29/2005 6:16:22 PM}    this->LineActor2->SetProperty(this->NormalProperty);
// COMMENT: {7/29/2005 6:16:22 PM}    this->CurrentRenderer->AddActor(this->ConeActor2);
// COMMENT: {7/29/2005 6:16:22 PM}    this->ConeActor2->SetProperty(this->NormalProperty);
    
// COMMENT: {7/29/2005 6:14:50 PM}    // add the origin handle
// COMMENT: {7/29/2005 6:14:50 PM}    this->CurrentRenderer->AddActor(this->SphereActor);
// COMMENT: {7/29/2005 6:14:50 PM}    this->SphereActor->SetProperty(this->NormalProperty);

    // add the plane (if desired)
    if ( this->DrawPlane )
      {
      this->CurrentRenderer->AddActor(this->CutActor);
      }
    this->CutActor->SetProperty(this->PlaneProperty);

    this->UpdateRepresentation();
#ifndef USE_ORIGINAL
	if (!this->ValidPick)
	{
		this->LastPickPosition[0] = this->InitialPickPosition[0];
		this->LastPickPosition[1] = this->InitialPickPosition[1];
		this->LastPickPosition[2] = this->InitialPickPosition[2];
		this->ValidPick = 1;
	}
#endif
    this->SizeHandles();
    this->InvokeEvent(vtkCommand::EnableEvent,NULL);
    }
  
  else //disabling----------------------------------------------------------
    {
    vtkDebugMacro(<<"Disabling plane widget");

    if ( ! this->Enabled ) //already disabled, just return
      {
      return;
      }
    
    this->Enabled = 0;

    // don't listen for events any more
    this->Interactor->RemoveObserver(this->EventCallbackCommand);

    // turn off the various actors
// COMMENT: {7/29/2005 6:07:16 PM}    this->CurrentRenderer->RemoveActor(this->OutlineActor);
    this->CurrentRenderer->RemoveActor(this->EdgesActor);
// COMMENT: {7/29/2005 6:13:28 PM}    this->CurrentRenderer->RemoveActor(this->LineActor);
// COMMENT: {7/29/2005 6:13:28 PM}    this->CurrentRenderer->RemoveActor(this->ConeActor);
// COMMENT: {7/29/2005 6:16:10 PM}    this->CurrentRenderer->RemoveActor(this->LineActor2);
// COMMENT: {7/29/2005 6:16:10 PM}    this->CurrentRenderer->RemoveActor(this->ConeActor2);
// COMMENT: {7/29/2005 6:14:35 PM}    this->CurrentRenderer->RemoveActor(this->SphereActor);
    this->CurrentRenderer->RemoveActor(this->CutActor);

    this->InvokeEvent(vtkCommand::DisableEvent,NULL);
    this->SetCurrentRenderer(NULL);
    }

  this->Interactor->Render();
}

void CGridLineWidget::ProcessEvents(vtkObject* vtkNotUsed(object), 
                                           unsigned long event,
                                           void* clientdata, 
                                           void* vtkNotUsed(calldata))
{
  CGridLineWidget* self = 
    reinterpret_cast<CGridLineWidget *>( clientdata );

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
      self->OnMiddleButtonDown();
      break;
    case vtkCommand::MiddleButtonReleaseEvent:
      self->OnMiddleButtonUp();
      break;
    case vtkCommand::RightButtonPressEvent:
      self->OnRightButtonDown();
      break;
    case vtkCommand::RightButtonReleaseEvent:
      self->OnRightButtonUp();
      break;
    case vtkCommand::MouseMoveEvent:
      self->OnMouseMove();
      break;
    }
}

void CGridLineWidget::OnLeftButtonDown()
{
  // We're only here if we are enabled
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, we can process this. See if we've picked anything.
  // Make sure it's in the activated renderer
  vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
  if ( ren != this->CurrentRenderer )
    {
    this->State = vtkImplicitPlaneWidget::Outside;
    return;
    }
  
  vtkAssemblyPath *path;
  int n = this->Picker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->Picker->GetPath();

  if ( path == NULL ) //not picking this widget
    {
    this->HighlightPlane(0);
    this->HighlightNormal(0);
    this->HighlightOutline(0);
    this->State = vtkImplicitPlaneWidget::Outside;
    return;
    }

  vtkProp *prop = path->GetFirstNode()->GetViewProp();
  this->ValidPick = 1;
  this->Picker->GetPickPosition(this->LastPickPosition);
  if ( prop == this->ConeActor || prop == this->LineActor ||
       prop == this->ConeActor2 || prop == this->LineActor2 )
    {
    this->HighlightPlane(1);
    this->HighlightNormal(1);
    this->State = vtkImplicitPlaneWidget::Rotating;
    }
#ifdef USE_ORIGINAL
  else if ( prop == this->CutActor )
#else USE_ORIGINAL
  else if ( prop == this->CutActor || prop == this->EdgesActor )
#endif
    {
    this->HighlightPlane(1);
    this->State = vtkImplicitPlaneWidget::Pushing;
    }
  else if ( prop == this->SphereActor )
    {
    this->HighlightNormal(1);
    this->State = vtkImplicitPlaneWidget::MovingOrigin;
    }
  else
    {
    if ( this->OutlineTranslation )
      {
      this->HighlightOutline(1);
      this->State = vtkImplicitPlaneWidget::MovingOutline;
      }
    }
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  this->Interactor->Render();
}

void CGridLineWidget::SetInitialPickPosition(double pos[3])
{
	for (int i = 0; i < 3; ++i)
	{
		this->InitialPickPosition[i] = pos[i];
	}
}
