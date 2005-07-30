#include "StdAfx.h"
#include "GridLineWidget.h"

vtkCxxRevisionMacro(CGridLineWidget, "$Revision: 1.23 $");
vtkStandardNewMacro(CGridLineWidget);


CGridLineWidget::CGridLineWidget(void)
{
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
    this->OutlineActor->SetProperty(this->EdgesProperty);

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
