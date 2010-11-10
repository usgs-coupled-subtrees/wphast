#include "StdAfx.h"
#include "vtkPlaneWidget2.h"

#include <sstream>

#include <vtkActor.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellPicker.h>
#include <vtkConeSource.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkLineSource.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkPlanes.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
#include <vtkArrowSource.h>

#include "resource.h"

vtkCxxRevisionMacro(vtkPlaneWidget2, "$Revision$");
vtkStandardNewMacro(vtkPlaneWidget2);

vtkPlaneWidget2::vtkPlaneWidget2(void)
{
	this->EventCallbackCommand->SetCallback(vtkPlaneWidget2::ProcessEvents);

	this->ConeActor->PickableOff();
	this->LineActor->PickableOff();
	this->ConeActor2->PickableOff();
	this->LineActor2->PickableOff();
	this->PlaneActor->PickableOff();

	//this->PlaneProperty->SetColor(0, 0, 0);
	this->PlaneProperty->SetColor(0, 0, 1);
	this->PlaneProperty->SetLineWidth(1);

	this->SelectedPlaneProperty->SetAmbient(1.0);
	this->SelectedPlaneProperty->SetColor(1, 0, 1);
	this->SelectedPlaneProperty->SetLineWidth(2);

	// setup axes
	this->AxisSource = vtkArrowSource::New();
	this->AxisSource->SetTipResolution(16);
	this->AxisSource->SetShaftResolution(16);

	this->AxisMapper = vtkPolyDataMapper::New();
	this->AxisMapper->SetInput(AxisSource->GetOutput());

	this->XAxisActor = vtkActor::New();
	this->XAxisActor->SetMapper(AxisMapper);
	this->XAxisActor->GetProperty()->SetColor(1, 0, 0);

	this->YAxisActor = vtkActor::New();
	this->YAxisActor->SetMapper(AxisMapper);
	this->YAxisActor->GetProperty()->SetColor(0, 1, 0);

	double* pt1 = this->GetPoint1();
	for (int i = 0; i < 3; ++i)
	{
		this->m_InvisablePosX[i] = pt1[i];
	}

	// Add arrow heads to pick list in order to allow
	// drag/drop/rotate grid
	this->PlanePicker->AddPickList(this->XAxisActor);
	this->PlanePicker->AddPickList(this->YAxisActor);

#if defined(_DEBUG)
	this->m_VisHandleGeometry = vtkSphereSource::New();
	this->m_VisHandleGeometry->SetThetaResolution(16);
	this->m_VisHandleGeometry->SetPhiResolution(8);

	this->m_VisHandleMapper = vtkPolyDataMapper::New();
	this->m_VisHandleMapper->SetInput(this->m_VisHandleGeometry->GetOutput());

	this->m_VisHandle = vtkActor::New();
	this->m_VisHandle->SetMapper(this->m_VisHandleMapper);
	this->m_VisHandle->GetProperty()->SetColor(1, 0, 1);

	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX[0], this->m_InvisablePosX[1], this->m_InvisablePosX[2]);
#endif

}

vtkPlaneWidget2::~vtkPlaneWidget2(void)
{
	if (this->XAxisActor) this->XAxisActor->Delete();
	if (this->YAxisActor) this->YAxisActor->Delete();
	if (this->AxisMapper) this->AxisMapper->Delete();
	if (this->AxisSource) this->AxisSource->Delete();

#if defined(_DEBUG)
	if (this->m_VisHandleGeometry) this->m_VisHandleGeometry->Delete();
	if (this->m_VisHandleMapper)   this->m_VisHandleMapper->Delete();
	if (this->m_VisHandle)         this->m_VisHandle->Delete();
#endif
}

void vtkPlaneWidget2::SetEnabled(int enabling)
{
	if (enabling)
	{
		// called before adding axes
		Superclass::SetEnabled(enabling);

		// turn off the normal vector
		this->CurrentRenderer->RemoveActor(this->LineActor);
		this->CurrentRenderer->RemoveActor(this->ConeActor);
		this->CurrentRenderer->RemoveActor(this->LineActor2);
		this->CurrentRenderer->RemoveActor(this->ConeActor2);

		// add axes
		this->CurrentRenderer->AddActor(this->XAxisActor);
		this->CurrentRenderer->AddActor(this->YAxisActor);

#ifdef _DEBUG
		if (this->m_VisHandle)
		{
			this->CurrentRenderer->AddActor(this->m_VisHandle);
		}
#endif

		this->Interactor->Render();
		this->SizeHandles();
	}
	else
	{
		// remove axes
		if (this->CurrentRenderer)
		{
			ASSERT(this->XAxisActor->GetNumberOfConsumers()       != 0);
			ASSERT(this->YAxisActor->GetNumberOfConsumers()       != 0);

			this->CurrentRenderer->RemoveActor(this->XAxisActor);
			this->CurrentRenderer->RemoveActor(this->YAxisActor);

			ASSERT(this->XAxisActor->GetNumberOfConsumers()       == 0);
			ASSERT(this->YAxisActor->GetNumberOfConsumers()       == 0);

#ifdef _DEBUG
			ASSERT(this->m_VisHandle->GetNumberOfConsumers()      != 0);
			this->CurrentRenderer->RemoveActor(this->m_VisHandle);
			ASSERT(this->m_VisHandle->GetNumberOfConsumers()      == 0);
#endif
		}
		this->Interactor->Render();

		// must be called after removing axes (this->CurrentRender gets freed)
		Superclass::SetEnabled(enabling);
	}
}

void vtkPlaneWidget2::ProcessEvents(vtkObject* vtkNotUsed(object),
									unsigned long event,
									void* clientdata,
									void* vtkNotUsed(calldata))
{
	vtkPlaneWidget2* self = reinterpret_cast<vtkPlaneWidget2 *>( clientdata );

	switch(event)
    {
    case vtkCommand::LeftButtonPressEvent:
		if (self->Interactor->GetControlKey())
		{
			self->OnCtrlLeftButtonDown();
		}
		else
		{
			self->OnLeftButtonDown();
			if (self->State == vtkPlaneWidget::Moving)
			{
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEALL)));
			}
		}
		break;
    case vtkCommand::LeftButtonReleaseEvent:
		self->OnLeftButtonUp();
		break;
	case vtkCommand::MiddleButtonPressEvent:
		self->OnLeftButtonDown();
		break;
	case vtkCommand::MiddleButtonReleaseEvent:
		self->OnLeftButtonUp();
		break;
    case vtkCommand::RightButtonPressEvent:
		self->OnRightButtonDown();
		if (self->State == vtkPlaneWidget::Scaling)
		{
			::SetCursor(AfxGetApp()->LoadCursor(IDC_SCALE_OBJ));
		}
		break;
    case vtkCommand::RightButtonReleaseEvent:
		self->OnRightButtonUp();
		break;
    case vtkCommand::MouseMoveEvent:
		self->OnMouseMove();
		break;
    }
}

void vtkPlaneWidget2::OnCtrlLeftButtonDown(void)
{
	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	// Okay, make sure that the pick is in the current renderer
	vtkRenderer *ren = this->Interactor->FindPokedRenderer(X,Y);
	if (ren != this->CurrentRenderer)
	{
		this->State = vtkPlaneWidget::Outside;
		return;
	}

	// Okay, we can process this. Try to pick handles first;
	// if no handles picked, then try to pick the plane.
	vtkAssemblyPath *path;
	this->HandlePicker->Pick(X, Y, 0.0, this->CurrentRenderer);
	path = this->HandlePicker->GetPath();
	if ( path != NULL )
	{
		this->State = vtkPlaneWidget2::Spinning;
		this->HighlightHandle(path->GetFirstNode()->GetViewProp());
	}
	else
	{
		this->PlanePicker->Pick(X, Y, 0.0, this->CurrentRenderer);
		path = this->PlanePicker->GetPath();
		if (path != NULL)
		{
			this->State = vtkPlaneWidget2::Spinning;
			this->HighlightPlane(1);
		}
		else
		{
			this->State = vtkPlaneWidget::Outside;
			this->HighlightHandle(NULL);
			return;
		}
	}

	this->EventCallbackCommand->SetAbortFlag(1);
	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
	this->Interactor->Render();
}

void vtkPlaneWidget2::OnMouseMove(void)
{
	// See whether we're active
	if (this->State == vtkPlaneWidget::Outside ||
		this->State == vtkPlaneWidget::Start)
	{
		return;
	}

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	// Do different things depending on state
	// Calculations everybody does
	double focalPoint[4], pickPoint[4], prevPickPoint[4];
	double z, vpn[3];

	vtkRenderer *renderer = this->Interactor->FindPokedRenderer(X,Y);
	vtkCamera *camera = renderer->GetActiveCamera();
	if (!camera)
	{
		return;
	}

	// Compute the two points defining the motion vector
	this->ComputeWorldToDisplay(this->LastPickPosition[0], this->LastPickPosition[1],
		this->LastPickPosition[2], focalPoint);
	z = focalPoint[2];
	this->ComputeDisplayToWorld(double(this->Interactor->GetLastEventPosition()[0]),
		double(this->Interactor->GetLastEventPosition()[1]),
		z, prevPickPoint);
	this->ComputeDisplayToWorld(double(X), double(Y), z, pickPoint);

	// Process the motion
	if ( this->State == vtkPlaneWidget::Moving )
	{
		// Okay to process
		if ( this->CurrentHandle )
		{
			if ( this->CurrentHandle == this->Handle[0] )
			{
				this->Substate = MovingMoveOrigin;
				double* o = this->GetOrigin();
				double prevO[3];
				for (int i = 0; i < 3; ++i)
				{
					prevO[i] = o[i];
				}
				if (!(pickPoint[0] == prevPickPoint[0] && pickPoint[1] == prevPickPoint[1] && pickPoint[2] == prevPickPoint[2]))
				{
					this->MoveOrigin(prevPickPoint, pickPoint);
					ASSERT(!::_isnan(this->GetOrigin()[0]));
					this->m_InvisablePosX[0] += o[0] - prevO[0];
					this->m_InvisablePosX[1] += o[1] - prevO[1];
					this->SizeHandles();
				}
				else
				{
					TRACE("MouseMove equal 0\n");
				}
			}
			else if ( this->CurrentHandle == this->Handle[1] )
			{
				this->Substate = MovingMovePoint1;
				double* o = this->GetOrigin();
				double prevO[3];
				for (int i = 0; i < 3; ++i)
				{
					prevO[i] = o[i];
				}
				if (!(pickPoint[0] == prevPickPoint[0] && pickPoint[1] == prevPickPoint[1] && pickPoint[2] == prevPickPoint[2]))
				{
					this->MovePoint1(prevPickPoint, pickPoint);
					ASSERT(!::_isnan(this->GetOrigin()[0]));
					this->m_InvisablePosX[0] += o[0] - prevO[0];
					this->m_InvisablePosX[1] += o[1] - prevO[1];
					this->SizeHandles();
				}
				else
				{
					TRACE("MouseMove equal 1\n");
				}
			}
			else if ( this->CurrentHandle == this->Handle[2] )
			{
				this->Substate = MovingMovePoint2;
				double* o = this->GetOrigin();
				double prevO[3];
				for (int i = 0; i < 3; ++i)
				{
					prevO[i] = o[i];
				}
				if (!(pickPoint[0] == prevPickPoint[0] && pickPoint[1] == prevPickPoint[1] && pickPoint[2] == prevPickPoint[2]))
				{
					this->MovePoint2(prevPickPoint, pickPoint);
					ASSERT(!::_isnan(this->GetOrigin()[0]));
					this->m_InvisablePosX[0] += o[0] - prevO[0];
					this->m_InvisablePosX[1] += o[1] - prevO[1];
					this->SizeHandles();
				}
				else
				{
					TRACE("MouseMove equal 2\n");
				}
			}
			else if ( this->CurrentHandle == this->Handle[3] )
			{
				this->Substate = MovingMovePoint3;
				if (!(pickPoint[0] == prevPickPoint[0] && pickPoint[1] == prevPickPoint[1] && pickPoint[2] == prevPickPoint[2]))
				{
					this->MovePoint3(prevPickPoint, pickPoint);
					ASSERT(!::_isnan(this->GetOrigin()[0]));
				}
				else
				{
					TRACE("MouseMove equal 3\n");
				}
			}
		}
		else // must be moving the plane
		{
			this->Substate = MovingTranslate;
			this->Translate(prevPickPoint, pickPoint);
			ASSERT(!::_isnan(this->GetOrigin()[0]));
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEALL)));
			this->m_InvisablePosX[0] += pickPoint[0] - prevPickPoint[0];
			this->m_InvisablePosX[1] += pickPoint[1] - prevPickPoint[1];
			this->SizeHandles();
		}
	}
	else if (this->State == vtkPlaneWidget::Scaling)
	{
		double* o = this->GetOrigin();
		double prevO[3];
		for (int i = 0; i < 3; ++i)
		{
			prevO[i] = o[i];
		}
		::SetCursor(AfxGetApp()->LoadCursor(IDC_SCALE_OBJ));
		this->Scale(prevPickPoint, pickPoint, X, Y);
		ASSERT(!::_isnan(this->GetOrigin()[0]));
		this->m_InvisablePosX[0] += o[0] - prevO[0];
		this->m_InvisablePosX[1] += o[1] - prevO[1];
		this->SizeHandles();
	}
	else if (this->State == vtkPlaneWidget::Pushing)
	{
		this->Push(prevPickPoint, pickPoint);
		ASSERT(!::_isnan(this->GetOrigin()[0]));
		this->SizeHandles();
	}
	else if (this->State == vtkPlaneWidget::Rotating)
	{
		camera->GetViewPlaneNormal(vpn);
		this->Rotate(X, Y, prevPickPoint, pickPoint, vpn);
		ASSERT(!::_isnan(this->GetOrigin()[0]));
		this->SizeHandles();
	}
	else if (vtkPlaneWidget2::Spinning)
	{
		this->Spin();
		ASSERT(!::_isnan(this->GetOrigin()[0]));
		this->SizeHandles();
	}

	// Interact, if desired
	this->EventCallbackCommand->SetAbortFlag(1);
	this->InvokeEvent(vtkCommand::InteractionEvent,NULL);

	if (this->CurrentRenderer) this->CurrentRenderer->ResetCameraClippingRange();

#if defined(_DEBUG)
	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX[0], this->m_InvisablePosX[1], this->m_InvisablePosX[2]);
#endif

	this->Interactor->Render();
}

void vtkPlaneWidget2::Spin(void)
{
	vtkRenderWindowInteractor *rwi = this->Interactor;

	double *o = this->PlaneSource->GetOrigin();
	double *pt1 = this->PlaneSource->GetPoint1();
	double *pt2 = this->PlaneSource->GetPoint2();
	double *center = this->PlaneSource->GetCenter();

	double disp_obj_center[3];

	this->ComputeWorldToDisplay(center[0], center[1], center[2],
		disp_obj_center);

	double newAngle =
		atan2((double)rwi->GetEventPosition()[1] - (double)disp_obj_center[1],
		(double)rwi->GetEventPosition()[0] - (double)disp_obj_center[0]);

	double oldAngle =
		atan2((double)rwi->GetLastEventPosition()[1] - (double)disp_obj_center[1],
		(double)rwi->GetLastEventPosition()[0] - (double)disp_obj_center[0]);

	newAngle = vtkMath::DegreesFromRadians(newAngle);
	oldAngle = vtkMath::DegreesFromRadians(oldAngle);

	double angle = newAngle - oldAngle;
	//cerr << "angle = " << angle << "\n";
	TRACE("angle = %g\n", angle);

// COMMENT: {1/29/2004 9:41:02 PM}	//****  THIS NEEDS WORK
// COMMENT: {1/29/2004 9:41:02 PM}	// SEEMS TO WORK COUNTERCLOCKWISE BUT NOT CLOCKWISE
// COMMENT: {1/29/2004 9:41:02 PM}	const int snap = 5;
// COMMENT: {1/29/2004 9:41:02 PM}	angle = int((angle + snap / 2.0) / snap) * snap;
// COMMENT: {1/29/2004 9:41:02 PM}	cerr << "snap angle = " << angle << "\n";
// COMMENT: {1/29/2004 9:41:02 PM}	//****/

	// Manipulate the transform to reflect the rotation
	this->Transform->Identity();
	this->Transform->Translate(center[0], center[1], center[2]);
	this->Transform->RotateZ(angle);
	this->Transform->Translate(-center[0], -center[1], -center[2]);

	// Set the corners
	double oNew[3], pt1New[3], pt2New[3];
	this->Transform->TransformPoint(o, oNew);
	this->Transform->TransformPoint(pt1, pt1New);
	this->Transform->TransformPoint(pt2, pt2New);

	this->Transform->TransformPoint(this->m_InvisablePosX, this->m_InvisablePosX);
#if defined(_DEBUG)
	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX[0], this->m_InvisablePosX[1], this->m_InvisablePosX[2]);
#endif

	ASSERT(!::_isnan(oNew[0]) && !::_isnan(oNew[1]) && !::_isnan(oNew[2]) && !::_isnan(pt1New[0]) && !::_isnan(pt1New[1]) && !::_isnan(pt1New[2]) && !::_isnan(pt2New[0]) && !::_isnan(pt2New[1]) && !::_isnan(pt2New[2]));
	this->PlaneSource->SetOrigin(oNew);
	this->PlaneSource->SetPoint1(pt1New);
	this->PlaneSource->SetPoint2(pt2New);
	this->PlaneSource->Update();

	this->PositionHandles();
}

double vtkPlaneWidget2::GetDeltaX(void)
{
	double *o = this->PlaneSource->GetOrigin();
	double *pt1 = this->PlaneSource->GetPoint1();
	double distance = sqrt(vtkMath::Distance2BetweenPoints(o, pt1));

	// Unrotate in order to determine sign
	this->Transform->Identity();
	this->Transform->RotateZ(-this->GetAngle());

	double oNew[3], pt1New[3];
	this->Transform->TransformPoint(o, oNew);
	this->Transform->TransformPoint(pt1, pt1New);
	if (pt1New[0] > oNew[0])
	{
		return distance;
	}
	else if (pt1New[0] < oNew[0])
	{
		return -distance;
	}
	else
	{
		return 0;
	}
}

void vtkPlaneWidget2::SetDeltaX(double dx)
{
	double *o = this->PlaneSource->GetOrigin();
	double *pt1 = this->PlaneSource->GetPoint1();
	double *pt2 = this->PlaneSource->GetPoint2();
	double radians = atan2(pt1[1] - o[1], pt1[0] - o[0]);

	//BUGBUG entering a value when length is negative
	//causes a flip of entered sign
	double new_pt1[3];
	new_pt1[0] = o[0] + (dx * cos(radians));
	new_pt1[1] = o[1] + (dx * sin(radians));
	new_pt1[2] = 0.0;

	// check validity
	double v1[3], v2[3];
	for (int i = 0; i < 3; ++i)
	{
		v1[i] = new_pt1[i] - o[i];
		v2[i] = pt2[i] - o[i];
	}
	double n[3];
	vtkMath::Cross(v1, v2, n);
	if ( vtkMath::Normalize(n) == 0.0 )
	{
		return;
	}

	// if here ok to change
	ASSERT(!::_isnan(new_pt1[0]) && !::_isnan(new_pt1[1]) && !::_isnan(new_pt1[2]));
	this->PlaneSource->SetPoint1(new_pt1);
	this->PositionHandles();
	this->SizeHandles();
}

double vtkPlaneWidget2::GetDeltaY(void)
{
	double *o = this->PlaneSource->GetOrigin();
	double *pt2 = this->PlaneSource->GetPoint2();
	double distance = sqrt(vtkMath::Distance2BetweenPoints(o, pt2));

	// Unrotate in order to determine sign
	this->Transform->Identity();
	this->Transform->RotateZ(-this->GetAngle());

	// Set the corners
	double oNew[3], pt2New[3];
	this->Transform->TransformPoint(o, oNew);
	this->Transform->TransformPoint(pt2, pt2New);
	if (pt2New[1] > oNew[1])
	{
		return distance;
	}
	else if (pt2New[1] < oNew[1])
	{
		return -distance;
	}
	else
	{
		return 0;
	}
}

void vtkPlaneWidget2::SetDeltaY(double dy)
{
	double *o = this->PlaneSource->GetOrigin();
	double *pt1 = this->PlaneSource->GetPoint1();
	double *pt2 = this->PlaneSource->GetPoint2();
	double radians = atan2(pt2[1] - o[1], pt2[0] - o[0]);

	//BUGBUG entering a value when length is negative
	//causes a flip of entered sign
	double new_pt2[3];
	new_pt2[0] = o[0] + (dy * cos(radians));
	new_pt2[1] = o[1] + (dy * sin(radians));
	new_pt2[2] = 0.0;

	// check validity
	double v1[3], v2[3];
	for (int i = 0; i < 3; ++i)
	{
		v1[i] = pt1[i] - o[i];
		v2[i] = new_pt2[i] - o[i];
	}
	double n[3];
	vtkMath::Cross(v1, v2, n);
	if ( vtkMath::Normalize(n) == 0.0 )
	{
		return; // Bad plane coordinate system
	}

	ASSERT(!::_isnan(new_pt2[0]) && !::_isnan(new_pt2[1]) && !::_isnan(new_pt2[2]));
	this->PlaneSource->SetPoint2(new_pt2);
	this->PositionHandles();
	this->SizeHandles();
}

double vtkPlaneWidget2::GetRadians(void)
{
	double *origin = this->PlaneSource->GetOrigin();
	//{{
	double o[3];
	o[0] = origin[0];
	o[1] = origin[1];
	o[2] = origin[2];
	//}}
	return atan2(this->m_InvisablePosX[1] - o[1], this->m_InvisablePosX[0] - o[0]);
}

void vtkPlaneWidget2::SetRadians(double radians)
{
	this->SetAngle(vtkMath::DegreesFromRadians(radians));
}

double vtkPlaneWidget2::GetAngle(void)
{
	return vtkMath::DegreesFromRadians(this->GetRadians());
}

void vtkPlaneWidget2::SetAngle(double angle)
{
	double *o = this->PlaneSource->GetOrigin();
	double *pt1 = this->PlaneSource->GetPoint1();
	double *pt2 = this->PlaneSource->GetPoint2();
	double *center = this->PlaneSource->GetCenter();

	// Manipulate the transform to reflect the rotation
	this->Transform->Identity();
	this->Transform->Translate(o[0], o[1], o[2]);
	this->Transform->RotateZ(angle - this->GetAngle());
	this->Transform->Translate(-o[0], -o[1], -o[2]);

	// Set the corners
	double oNew[3], pt1New[3], pt2New[3];
	this->Transform->TransformPoint(o, oNew);
	this->Transform->TransformPoint(pt1, pt1New);
	this->Transform->TransformPoint(pt2, pt2New);
	this->Transform->TransformPoint(this->m_InvisablePosX, this->m_InvisablePosX);
#if defined(_DEBUG)
	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX[0], this->m_InvisablePosX[1], this->m_InvisablePosX[2]);
#endif

	ASSERT(!::_isnan(oNew[0]) && !::_isnan(oNew[1]) && !::_isnan(oNew[2]) && !::_isnan(pt1New[0]) && !::_isnan(pt1New[1]) && !::_isnan(pt1New[2]) && !::_isnan(pt2New[0]) && !::_isnan(pt2New[1]) && !::_isnan(pt2New[2]));
	this->PlaneSource->SetOrigin(oNew);
	this->PlaneSource->SetPoint1(pt1New);
	this->PlaneSource->SetPoint2(pt2New);
	this->PlaneSource->Update();

	this->PositionHandles();
	this->SizeHandles();
}

double* vtkPlaneWidget2::GetModelOrigin(void)
{
	return this->GetOrigin();
}

void vtkPlaneWidget2::SetModelOrigin(float x, float y)
{
	double *o = this->PlaneSource->GetOrigin();
	double *pt1 = this->PlaneSource->GetPoint1();
	double *pt2 = this->PlaneSource->GetPoint2();

	//Manipulate the transform to reflect the rotation
	this->Transform->Identity();
	this->Transform->Translate(x - o[0], y - o[1], o[2]);

	this->m_InvisablePosX[0] += x - o[0];
	this->m_InvisablePosX[1] += y - o[1];
#if defined(_DEBUG)
	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX[0], this->m_InvisablePosX[1], this->m_InvisablePosX[2]);
#endif

	//Set the corners
	double oNew[3], pt1New[3], pt2New[3];
	this->Transform->TransformPoint(o,oNew);
	this->Transform->TransformPoint(pt1,pt1New);
	this->Transform->TransformPoint(pt2,pt2New);

	ASSERT(!::_isnan(oNew[0]) && !::_isnan(oNew[1]) && !::_isnan(oNew[2]) && !::_isnan(pt1New[0]) && !::_isnan(pt1New[1]) && !::_isnan(pt1New[2]) && !::_isnan(pt2New[0]) && !::_isnan(pt2New[1]) && !::_isnan(pt2New[2]));
	this->PlaneSource->SetOrigin(oNew);
	this->PlaneSource->SetPoint1(pt1New);
	this->PlaneSource->SetPoint2(pt2New);
	this->PlaneSource->Update();

	this->PositionHandles();
	this->SizeHandles();
}

void vtkPlaneWidget2::SizeHandles(void)
{
	if (!this->ValidPick)
	{
		// this allows the handles to be sized w/o having to
		// interact(mouse-click) with the widget
		this->PlaneSource->GetOrigin(this->LastPickPosition);
		this->ValidPick = 1;
	}

	double radius = this->vtk3DWidget::SizeHandles(.10);
	for(int i = 0; i < 4; ++i)
	{
		this->HandleGeometry[i]->SetRadius(radius);
	}

	if (this->CurrentRenderer)
	{
		double *o = this->PlaneSource->GetOrigin();
		double scale = this->vtk3DWidget::SizeHandles(1.0);
		if (this->XAxisActor)
		{
			this->XAxisActor->SetPosition(o);
			this->XAxisActor->SetScale(scale);
			this->XAxisActor->SetOrientation(0, 0, this->GetAngle());
		}
		if (this->YAxisActor)
		{
			this->YAxisActor->SetPosition(o);
			this->YAxisActor->SetScale(scale);
			this->YAxisActor->SetOrientation(0, 0, this->GetAngle() + 90);
		}
#ifdef _DEBUG
		if (this->m_VisHandleGeometry)
		{
			this->m_VisHandleGeometry->SetRadius(radius);
		}
#endif

	}
}

void vtkPlaneWidget2::PlaceWidget(void)
{
#ifdef _DEBUG
	afxDump << "before vtkPlaneWidget2::PlaceWidget\n";
	ostrstream oss;

	this->PrintSelf(oss, vtkIndent(4));

	oss << ends;
	afxDump << oss.str() << "\n";
	oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
#endif

    this->Superclass::PlaceWidget();

	//{{
	double *o = this->GetOrigin();
	this->GridOrigin[0] = o[0];
	this->GridOrigin[1] = o[1];
	this->GridOrigin[2] = o[2];

	double *p1 = this->GetPoint1();
	this->Deltas[0] = p1[0] - o[0];

	double *p2 = this->GetPoint2();
	this->Deltas[1] = p2[1] - o[1];

	this->Deltas[2] = 0.;
	//}}

	double* pt1 = this->GetPoint1();
	for (int i = 0 ; i < 3; ++i) {
		this->m_InvisablePosX[i] = pt1[i];
	}
#if defined(_DEBUG)
	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX[0], this->m_InvisablePosX[1], this->m_InvisablePosX[2]);
#endif
}

int vtkPlaneWidget2::GetResolution(int index)const
{
	switch (index) {
		case 0:
			return this->PlaneSource->GetXResolution();
			break;
		case 1:
			return this->PlaneSource->GetYResolution();
			break;
		default:
			ASSERT(FALSE);
	}
	return 0;
}

void vtkPlaneWidget2::SetResolution(int index, int n)
{
	switch (index) {
		case 0:
			this->PlaneSource->SetXResolution(n);
			break;
		case 1:
			this->PlaneSource->SetYResolution(n);
			break;
		case 2:
			// no-op
			break;
		default:
			ASSERT(FALSE);
	}
}

int vtkPlaneWidget2::GetXResolution(void)const
{
	return this->PlaneSource->GetXResolution();
}

void vtkPlaneWidget2::SetXResolution(int nx)
{
	this->PlaneSource->SetXResolution(nx);
}

int vtkPlaneWidget2::GetYResolution(void)const
{
	return this->PlaneSource->GetYResolution();
}

void vtkPlaneWidget2::SetYResolution(int nx)
{
	this->PlaneSource->SetYResolution(nx);
}
