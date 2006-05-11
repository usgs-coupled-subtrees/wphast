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
// COMMENT: {9/13/2005 3:19:19 PM}		return;
	}

	this->EventCallbackCommand->SetAbortFlag(1);
	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	this->Interactor->Render();
}

void vtkBoxWidgetEx::SizeHandles()
{
#ifdef USE_ORIGINAL
	float radius = this->vtk3DWidget::SizeHandles(1.5);
	for(int i=0; i<7; i++)
	{
		this->HandleGeometry[i]->SetRadius(radius);
	}
#else
	if (!this->Interactor) return;
	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];
	this->CurrentRenderer = this->Interactor->FindPokedRenderer(X, Y);

	double radius = this->SizeHandles(1.5);
	TRACE("xxx vtkBoxWidgetEx::SizeHandles radius = %g\n", radius);
	for(int i=0; i<7; i++)
	{
		this->HandleGeometry[i]->SetRadius(radius);
	}
#endif
// COMMENT: {5/10/2006 10:59:10 PM}	//{{{
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 10:54:31 PM}	//{{
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 10:54:31 PM}	if (!this->Interactor) return;
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 10:54:31 PM}	int X = this->Interactor->GetEventPosition()[0];
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 10:54:31 PM}	int Y = this->Interactor->GetEventPosition()[1];
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 10:54:31 PM}
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 10:54:31 PM}	this->CurrentRenderer = this->Interactor->FindPokedRenderer(X, Y);
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 10:54:31 PM}	//}}
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}	if (this->CurrentRenderer)
// COMMENT: {5/10/2006 10:59:10 PM}	{
// COMMENT: {5/10/2006 10:59:10 PM}		int i;
// COMMENT: {5/10/2006 10:59:10 PM}		double radius, z;
// COMMENT: {5/10/2006 10:59:10 PM}		double windowLowerLeft[4], windowUpperRight[4];
// COMMENT: {5/10/2006 10:59:10 PM}		float *viewport = this->CurrentRenderer->GetViewport();
// COMMENT: {5/10/2006 10:59:10 PM}		int *winSize = this->CurrentRenderer->GetRenderWindow()->GetSize();
// COMMENT: {5/10/2006 10:59:10 PM}		double focalPoint[4];
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}		// get the focal point in world coordinates
// COMMENT: {5/10/2006 10:59:10 PM}		//
// COMMENT: {5/10/2006 10:59:10 PM}		vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
// COMMENT: {5/10/2006 10:59:10 PM}		vtkFloatingPointType cameraFP[4];
// COMMENT: {5/10/2006 10:59:10 PM}		camera->GetFocalPoint((vtkFloatingPointType*)cameraFP); cameraFP[3] = 1.0;
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}		this->ComputeWorldToDisplay(cameraFP[0],
// COMMENT: {5/10/2006 10:59:10 PM}									cameraFP[1],
// COMMENT: {5/10/2006 10:59:10 PM}									cameraFP[2], focalPoint);
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}		z = focalPoint[2];
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}		double x = winSize[0] * viewport[0];
// COMMENT: {5/10/2006 10:59:10 PM}		double y = winSize[1] * viewport[1];
// COMMENT: {5/10/2006 10:59:10 PM}		this->ComputeDisplayToWorld(x,y,z,windowLowerLeft);
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}		x = winSize[0] * viewport[2];
// COMMENT: {5/10/2006 10:59:10 PM}		y = winSize[1] * viewport[3];
// COMMENT: {5/10/2006 10:59:10 PM}		this->ComputeDisplayToWorld(x,y,z,windowUpperRight);
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}		for (radius=0.0, i=0; i<3; i++)
// COMMENT: {5/10/2006 10:59:10 PM}		{
// COMMENT: {5/10/2006 10:59:10 PM}			radius += (windowUpperRight[i] - windowLowerLeft[i]) *
// COMMENT: {5/10/2006 10:59:10 PM}				(windowUpperRight[i] - windowLowerLeft[i]);
// COMMENT: {5/10/2006 10:59:10 PM}		}
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}		//radius = ::sqrt(radius) * 0.008;
// COMMENT: {5/10/2006 10:59:10 PM}		radius = ::sqrt(radius) * 0.0149999997;
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}		for(int i=0; i<7; i++)
// COMMENT: {5/10/2006 10:59:10 PM}		{
// COMMENT: {5/10/2006 10:59:10 PM}			this->HandleGeometry[i]->SetRadius(radius);
// COMMENT: {5/10/2006 10:59:10 PM}		}
// COMMENT: {5/10/2006 10:59:10 PM}		TRACE("vtkBoxWidgetEx::SizeHandles radius = %g\n", radius);
// COMMENT: {5/10/2006 10:59:10 PM}	}
// COMMENT: {5/10/2006 10:59:10 PM}	//}}}
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 9:47:38 PM}	for(int i=0; i<7; i++)
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 9:47:38 PM}	{
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 9:47:38 PM}		this->HandleGeometry[i]->SetRadius(radius);
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 9:47:38 PM}	}
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 9:47:38 PM}	TRACE("vtkBoxWidgetEx::SizeHandles radius = %g\n", radius);
// COMMENT: {5/10/2006 10:59:10 PM}
// COMMENT: {5/10/2006 10:59:10 PM}// COMMENT: {5/10/2006 9:37:55 PM}	if (this->Interactor) this->Interactor->Render();
}

double vtkBoxWidgetEx::SizeHandles(double factor)
{
	int i;
	vtkRenderer *renderer;

#ifdef SKIP_ORIGINAL
	if ( !this->ValidPick || !(renderer=this->CurrentRenderer) ||
		!renderer->GetActiveCamera() )
#else
	if (!(renderer = this->CurrentRenderer) || !renderer->GetActiveCamera())
#endif
	{
		return (this->HandleSize * factor * this->InitialLength);
	}
	else
	{
		double radius, z;
		double windowLowerLeft[4], windowUpperRight[4];
		float *viewport = renderer->GetViewport();
		int *winSize = renderer->GetRenderWindow()->GetSize();
		double focalPoint[4];

#ifdef SKIP_ORIGINAL
		this->ComputeWorldToDisplay(this->LastPickPosition[0],
			this->LastPickPosition[1],
			this->LastPickPosition[2], focalPoint);
#else
		// get the focal point in world coordinates
		//
		vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
		double cameraFP[4];
		camera->GetFocalPoint(cameraFP); cameraFP[3] = 1.0;

		this->ComputeWorldToDisplay(cameraFP[0],
			cameraFP[1],
			cameraFP[2], focalPoint);
#endif
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

