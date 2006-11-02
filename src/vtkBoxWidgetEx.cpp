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
		vtkFloatingPointType *viewport = renderer->GetViewport();
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

