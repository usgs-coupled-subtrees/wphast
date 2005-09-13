#include "StdAfx.h"
#include "vtkBoxWidgetEx.h"

vtkCxxRevisionMacro(vtkBoxWidgetEx, "$Revision: 493 $");
vtkStandardNewMacro(vtkBoxWidgetEx);


vtkBoxWidgetEx::vtkBoxWidgetEx(void)
{
	this->EventCallbackCommand->SetCallback(vtkBoxWidgetEx::ProcessEvents);
	this->HexPickerEnabled = 1;
}

vtkBoxWidgetEx::~vtkBoxWidgetEx(void)
{
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
			this->HighlightHandle(path->GetFirstNode()->GetProp()));
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
// COMMENT: {9/13/2005 3:19:19 PM}		return;
	}

	this->EventCallbackCommand->SetAbortFlag(1);
	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	this->Interactor->Render();
}
