#include "StdAfx.h"
#include "vtkPlaneWidget2.h"

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

//{{
#include <vtkArrowSource.h>
//}}
#include "resource.h"

vtkCxxRevisionMacro(vtkPlaneWidget2, "");
vtkStandardNewMacro(vtkPlaneWidget2);

vtkPlaneWidget2::vtkPlaneWidget2(void)
{
	this->EventCallbackCommand->SetCallback(vtkPlaneWidget2::ProcessEvents);

	this->ConeActor->PickableOff();
	this->LineActor->PickableOff();
	this->ConeActor2->PickableOff();
	this->LineActor2->PickableOff();

	this->PlaneProperty->SetColor(0, 0, 0);
	this->PlaneProperty->SetLineWidth(2);

	this->SelectedPlaneProperty->SetAmbient(1.0);
	this->SelectedPlaneProperty->SetColor(1, 0, 1);
	this->SelectedPlaneProperty->SetLineWidth(3);

	// setup axes
	this->AxisSource = vtkArrowSource::New();
	this->AxisSource->SetTipResolution(16);
	this->AxisSource->SetShaftResolution(16);			
// COMMENT: {2/27/2004 4:20:38 PM}	this->AxisSource->SetShaftRadius(0.05);

	this->AxisMapper = vtkPolyDataMapper::New();
	this->AxisMapper->SetInput(AxisSource->GetOutput());

	this->XAxisActor = vtkActor::New();
	this->XAxisActor->SetMapper(AxisMapper);
	this->XAxisActor->GetProperty()->SetColor(1, 0, 0);

	this->YAxisActor = vtkActor::New();
	this->YAxisActor->SetMapper(AxisMapper);
	this->YAxisActor->GetProperty()->SetColor(0, 1, 0);

	float* pt1 = this->GetPoint1();
	for (int i = 0; i < 3; ++i) {
		this->m_InvisablePosX[i] = pt1[i];
	}

#if defined(_DEBUG)
	this->m_VisHandleGeometry = vtkSphereSource::New();
	this->m_VisHandleGeometry->SetThetaResolution(16);
	this->m_VisHandleGeometry->SetPhiResolution(8);

	this->m_VisHandleMapper = vtkPolyDataMapper::New();
	this->m_VisHandleMapper->SetInput(this->m_VisHandleGeometry->GetOutput());

	this->m_VisHandle = vtkActor::New();
	this->m_VisHandle->SetMapper(this->m_VisHandleMapper);
	this->m_VisHandle->GetProperty()->SetColor(1, 0, 1);

	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX);
#endif
}

vtkPlaneWidget2::~vtkPlaneWidget2(void)
{
	this->XAxisActor->Delete();
	this->YAxisActor->Delete();
	this->AxisMapper->Delete();
	this->AxisSource->Delete();
#if defined(_DEBUG)
	this->m_VisHandleGeometry->Delete();
	this->m_VisHandleMapper->Delete();
	this->m_VisHandle->Delete();
#endif
}

void vtkPlaneWidget2::SetEnabled(int enabling)
{
	vtkRenderer *pSaveCurrentRenderer = this->CurrentRenderer;

	Superclass::SetEnabled(enabling);
	/// this->Interactor->RemoveObserver(this->EventCallbackCommand);

	if (enabling) {
		//
		// Remove Normal Vector handles
		//

		// turn off the normal vector
		this->CurrentRenderer->RemoveActor(this->LineActor);
		this->CurrentRenderer->RemoveActor(this->ConeActor);
		this->CurrentRenderer->RemoveActor(this->LineActor2);
		this->CurrentRenderer->RemoveActor(this->ConeActor2);

		// add axes
		this->CurrentRenderer->AddActor(this->XAxisActor);
		this->CurrentRenderer->AddActor(this->YAxisActor);
		this->SizeHandles();

#ifdef _DEBUG
		this->CurrentRenderer->AddActor(this->m_VisHandle);
#endif

		this->Interactor->Render();
		//{{
		this->SizeHandles();
		//}}
	}
	else {
		// remove axes
		if (pSaveCurrentRenderer)
		{
			pSaveCurrentRenderer->RemoveActor(this->XAxisActor);
			pSaveCurrentRenderer->RemoveActor(this->YAxisActor);
#ifdef _DEBUG
			pSaveCurrentRenderer->RemoveActor(this->m_VisHandle);
#endif
		}
		this->Interactor->Render();
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
		if (self->Interactor->GetControlKey()) {
			self->OnCtrlLeftButtonDown();
		}
		else {
			self->OnLeftButtonDown();
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
  if ( ren != this->CurrentRenderer )
    {
    this->State = vtkPlaneWidget::Outside;
    return;
    }
  
  // Okay, we can process this. Try to pick handles first;
  // if no handles picked, then try to pick the plane.
  vtkAssemblyPath *path;
  this->HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->HandlePicker->GetPath();
  if ( path != NULL )
    {
    this->State = vtkPlaneWidget2::Spinning;
    this->HighlightHandle(path->GetFirstNode()->GetProp());
    }
  else
    {
    this->PlanePicker->Pick(X,Y,0.0,this->CurrentRenderer);
    path = this->PlanePicker->GetPath();
    if ( path != NULL )
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
  if ( this->State == vtkPlaneWidget::Outside || 
       this->State == vtkPlaneWidget::Start )
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
  if ( !camera )
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
		//{{
		float* o = this->GetOrigin();
		float prevO[3];
		for (int i = 0; i < 3; ++i) {
			prevO[i] = o[i];
		}
		//}}
        this->MoveOrigin(prevPickPoint, pickPoint);
		//{{
		this->m_InvisablePosX[0] += o[0] - prevO[0];
		this->m_InvisablePosX[1] += o[1] - prevO[1];
		//}}
		this->SizeHandles();
        }
      else if ( this->CurrentHandle == this->Handle[1] )
        {
		this->Substate = MovingMovePoint1;
		//{{
		float* o = this->GetOrigin();
		float prevO[3];
		for (int i = 0; i < 3; ++i) {
			prevO[i] = o[i];
		}
		//}}
        this->MovePoint1(prevPickPoint, pickPoint);
		//{{
		this->m_InvisablePosX[0] += o[0] - prevO[0];
		this->m_InvisablePosX[1] += o[1] - prevO[1];
		//}}
		this->SizeHandles();
        }
      else if ( this->CurrentHandle == this->Handle[2] )
        {
		this->Substate = MovingMovePoint2;
		//{{
		float* o = this->GetOrigin();
		float prevO[3];
		for (int i = 0; i < 3; ++i) {
			prevO[i] = o[i];
		}
		//}}
        this->MovePoint2(prevPickPoint, pickPoint);
		//{{
		this->m_InvisablePosX[0] += o[0] - prevO[0];
		this->m_InvisablePosX[1] += o[1] - prevO[1];
// COMMENT: {3/16/2004 5:41:57 PM}		this->m_InvisablePosX[0] += pickPoint[0] - prevPickPoint[0];
// COMMENT: {3/16/2004 5:41:57 PM}		this->m_InvisablePosX[1] += pickPoint[1] - prevPickPoint[1];
		//}}
		this->SizeHandles();
        }
      else if ( this->CurrentHandle == this->Handle[3] )
        {
		this->Substate = MovingMovePoint3;
        this->MovePoint3(prevPickPoint, pickPoint);
        }
      }
    else //must be moving the plane
      {
	  this->Substate = MovingTranslate;

      this->Translate(prevPickPoint, pickPoint);
      //{{
      this->m_InvisablePosX[0] += pickPoint[0] - prevPickPoint[0];
      this->m_InvisablePosX[1] += pickPoint[1] - prevPickPoint[1];
	  //}}
      this->SizeHandles();
      }
    }
  else if ( this->State == vtkPlaneWidget::Scaling )
    {
    //{{
	float* o = this->GetOrigin();
	float prevO[3];
	for (int i = 0; i < 3; ++i) {
		prevO[i] = o[i];
	}
	//}}
	////{{{{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_SCALE_OBJ));
	////}}}}
    this->Scale(prevPickPoint, pickPoint, X, Y);
    //{{
    this->m_InvisablePosX[0] += o[0] - prevO[0];
    this->m_InvisablePosX[1] += o[1] - prevO[1];
	//}}
	this->SizeHandles();
    }
  else if ( this->State == vtkPlaneWidget::Pushing )
    {
    this->Push(prevPickPoint, pickPoint);
	this->SizeHandles();
    }
  else if ( this->State == vtkPlaneWidget::Rotating )
    {
    camera->GetViewPlaneNormal(vpn);
    this->Rotate(X, Y, prevPickPoint, pickPoint, vpn);
	this->SizeHandles();
    }
  else if ( vtkPlaneWidget2::Spinning )
    {
	  this->Spin();
	  this->SizeHandles();
    }

  // Interact, if desired
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkCommand::InteractionEvent,NULL);

//{{
  if (this->CurrentRenderer) this->CurrentRenderer->ResetCameraClippingRange();
//}}
#if defined(_DEBUG)
	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX);
#endif
  
  this->Interactor->Render();
}

void vtkPlaneWidget2::Spin(void)
{
	vtkRenderWindowInteractor *rwi = this->Interactor;

	float *o = this->PlaneSource->GetOrigin();
	float *pt1 = this->PlaneSource->GetPoint1();
	float *pt2 = this->PlaneSource->GetPoint2();
	float *center = this->PlaneSource->GetCenter();


	float disp_obj_center[3];

	this->ComputeWorldToDisplay(center[0], center[1], center[2], 
		disp_obj_center);
	
	double newAngle = 
		atan2((double)rwi->GetEventPosition()[1] - (double)disp_obj_center[1],
		(double)rwi->GetEventPosition()[0] - (double)disp_obj_center[0]);
	
	double oldAngle = 
		atan2((double)rwi->GetLastEventPosition()[1] - (double)disp_obj_center[1],
		(double)rwi->GetLastEventPosition()[0] - (double)disp_obj_center[0]);
	
	newAngle *= vtkMath::RadiansToDegrees();
	oldAngle *= vtkMath::RadiansToDegrees();

	double angle = newAngle - oldAngle;
	cerr << "angle = " << angle << "\n";

// COMMENT: {1/29/2004 9:41:02 PM}	//****  THIS NEEDS WORK
// COMMENT: {1/29/2004 9:41:02 PM}	// SEEMS TO WORK COUNTERCLOCKWISE BUT NOT CLOCKWISE
// COMMENT: {1/29/2004 9:41:02 PM}	const int snap = 5;
// COMMENT: {1/29/2004 9:41:02 PM}	angle = int((angle + snap / 2.0) / snap) * snap;
// COMMENT: {1/29/2004 9:41:02 PM}	cerr << "snap angle = " << angle << "\n";
// COMMENT: {1/29/2004 9:41:02 PM}	//****/


	//Manipulate the transform to reflect the rotation
	this->Transform->Identity();
	this->Transform->Translate(center[0],center[1],center[2]);
	this->Transform->RotateZ(angle);
	this->Transform->Translate(-center[0],-center[1],-center[2]);


	//Set the corners
	float oNew[3], pt1New[3], pt2New[3];
	this->Transform->TransformPoint(o,oNew);
	this->Transform->TransformPoint(pt1,pt1New);
	this->Transform->TransformPoint(pt2,pt2New);
	
	float inv[3];
	this->Transform->TransformPoint(this->m_InvisablePosX, inv);
	for (int i=0; i < 3; ++i) {
		this->m_InvisablePosX[i] = inv[i];
	}
#if defined(_DEBUG)
	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX);
#endif

	this->PlaneSource->SetOrigin(oNew);
	this->PlaneSource->SetPoint1(pt1New);
	this->PlaneSource->SetPoint2(pt2New);
	this->PlaneSource->Update();

	this->PositionHandles();
}

double vtkPlaneWidget2::GetDeltaX(void)
{
	float *o = this->PlaneSource->GetOrigin();
	float *pt1 = this->PlaneSource->GetPoint1();
	double distance = sqrt(vtkMath::Distance2BetweenPoints(o, pt1));

	//
	// Unrotate in order to determine sign
	this->Transform->Identity();
	this->Transform->RotateZ(-this->GetAngle());

	float oNew[3], pt1New[3];
	this->Transform->TransformPoint(o, oNew);
	this->Transform->TransformPoint(pt1, pt1New);
	if (pt1New[0] > oNew[0]) {
		return distance;
	}
	else if (pt1New[0] < oNew[0]) {
		return -distance;
	}
	else {
		/// ASSERT(FALSE);
		return 0;
	}
}

void vtkPlaneWidget2::SetDeltaX(double dx)
{
	float new_pt1[3];

	float *o = this->PlaneSource->GetOrigin();
	float *pt1 = this->PlaneSource->GetPoint1();
	float *pt2 = this->PlaneSource->GetPoint2();
	double radians = atan2(pt1[1] - o[1], pt1[0] - o[0]);

	//BUGBUG entering a value when length is negative
	//causes a flip of entered sign
	new_pt1[0] = o[0] + (dx * cos(radians));
	new_pt1[1] = o[1] + (dx * sin(radians));
	new_pt1[2] = 0.0;

	// check validity
	float v1[3], v2[3];
	for (int i = 0; i < 3; ++i)
	{
		v1[i] = new_pt1[i] - o[i];
		v2[i] = pt2[i] - o[i];
	}
	float n[3];
	vtkMath::Cross(v1, v2, n);
	if ( vtkMath::Normalize(n) == 0.0 ) return;

	// if here ok to change
	this->PlaneSource->SetPoint1(new_pt1);
	this->PositionHandles();
	this->SizeHandles();
}

double vtkPlaneWidget2::GetDeltaY(void)
{
	float *o = this->PlaneSource->GetOrigin();
	float *pt2 = this->PlaneSource->GetPoint2();
	double distance = sqrt(vtkMath::Distance2BetweenPoints(o, pt2));

	//
	// Unrotate in order to determine sign
	this->Transform->Identity();
	this->Transform->RotateZ(-this->GetAngle());

	// Set the corners
	float oNew[3], pt2New[3];
	this->Transform->TransformPoint(o, oNew);
	this->Transform->TransformPoint(pt2, pt2New);
	if (pt2New[1] > oNew[1]) {
		return distance;
	}
	else if (pt2New[1] < oNew[1]) {
		return -distance;
	}
	else {
		///ASSERT(FALSE);
		return 0;
	}
}

void vtkPlaneWidget2::SetDeltaY(double dy)
{
	float new_pt2[3];

	float *o = this->PlaneSource->GetOrigin();
	float *pt1 = this->PlaneSource->GetPoint1();
	float *pt2 = this->PlaneSource->GetPoint2();
	double radians = atan2(pt2[1] - o[1], pt2[0] - o[0]);

	//BUGBUG entering a value when length is negative
	//causes a flip of entered sign
	new_pt2[0] = o[0] + (dy * cos(radians));
	new_pt2[1] = o[1] + (dy * sin(radians));
	new_pt2[2] = 0.0;

	// check validity
	float v1[3], v2[3];
	for (int i = 0; i < 3; ++i)
	{
		v1[i] = pt1[i] - o[i];
		v2[i] = new_pt2[i] - o[i];
	}
	float n[3];
	vtkMath::Cross(v1, v2, n);
	if ( vtkMath::Normalize(n) == 0.0 ) {
		return; // Bad plane coordinate system
	}

	this->PlaneSource->SetPoint2(new_pt2);
	this->PositionHandles();
	this->SizeHandles();
}

double vtkPlaneWidget2::GetAngle(void)
{
	float *o = this->PlaneSource->GetOrigin();
	//float *pt1 = this->PlaneSource->GetPoint1();

	return  vtkMath::DoubleRadiansToDegrees() * atan2(this->m_InvisablePosX[1] - o[1], this->m_InvisablePosX[0] - o[0]);
}

void vtkPlaneWidget2::SetAngle(double angle)
{
	float *o = this->PlaneSource->GetOrigin();
	float *pt1 = this->PlaneSource->GetPoint1();
	float *pt2 = this->PlaneSource->GetPoint2();
	float *center = this->PlaneSource->GetCenter();

	//Manipulate the transform to reflect the rotation
	this->Transform->Identity();
	this->Transform->Translate(center[0],center[1],center[2]);
	this->Transform->RotateZ(angle - this->GetAngle());
	this->Transform->Translate(-center[0],-center[1],-center[2]);

	//Set the corners
	float oNew[3], pt1New[3], pt2New[3];
	this->Transform->TransformPoint(o,oNew);
	this->Transform->TransformPoint(pt1,pt1New);
	this->Transform->TransformPoint(pt2,pt2New);
	//{{
	this->Transform->TransformPoint(this->m_InvisablePosX, this->m_InvisablePosX);
#if defined(_DEBUG)
	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX);
#endif
	//}}

	this->PlaneSource->SetOrigin(oNew);
	this->PlaneSource->SetPoint1(pt1New);
	this->PlaneSource->SetPoint2(pt2New);
	this->PlaneSource->Update();

	this->PositionHandles();
	//{{
	this->SizeHandles();
	//}}
}

float* vtkPlaneWidget2::GetModelOrigin(void)
{
	return this->GetOrigin();
}

void vtkPlaneWidget2::SetModelOrigin(float x, float y)
{
	float *o = this->PlaneSource->GetOrigin();
	float *pt1 = this->PlaneSource->GetPoint1();
	float *pt2 = this->PlaneSource->GetPoint2();

	//Manipulate the transform to reflect the rotation
	this->Transform->Identity();
	this->Transform->Translate(x - o[0], y - o[1], o[2]);
	//{{
	this->m_InvisablePosX[0] += x - o[0];
	this->m_InvisablePosX[1] += y - o[1];
#if defined(_DEBUG)
	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX);
#endif
	//}}

	//Set the corners
	float oNew[3], pt1New[3], pt2New[3];
	this->Transform->TransformPoint(o,oNew);
	this->Transform->TransformPoint(pt1,pt1New);
	this->Transform->TransformPoint(pt2,pt2New);

	this->PlaneSource->SetOrigin(oNew);
	this->PlaneSource->SetPoint1(pt1New);
	this->PlaneSource->SetPoint2(pt2New);
	this->PlaneSource->Update();

	this->PositionHandles();
	this->SizeHandles();
}

void vtkPlaneWidget2::SizeHandles(void)
{
	Superclass::SizeHandles();

	if (this->CurrentRenderer)
	{
		if (this->XAxisActor)
		{
			float *o = this->PlaneSource->GetOrigin();
			this->XAxisActor->SetPosition(o);
			this->YAxisActor->SetPosition(o);
			float scale = this->vtk3DWidget::SizeHandles(10.0);

			this->XAxisActor->SetScale(scale);
			this->YAxisActor->SetScale(scale);
			
			this->XAxisActor->SetOrientation(0, 0, this->GetAngle());
			this->YAxisActor->SetOrientation(0, 0, this->GetAngle() + 90);
#ifdef _DEBUG
			float radius = this->vtk3DWidget::SizeHandles(1.25);
			this->m_VisHandleGeometry->SetRadius(radius);
#endif
		}
	}
}

void vtkPlaneWidget2::PlaceWidget(void)
{
#ifdef _DEBUG
	afxDump << "before vtkPlaneWidget2::PlaceWidget\n";
	ostrstream oss;

	this->PrintSelf(oss, 4);

	oss << ends;
	afxDump << oss.str() << "\n";
	oss.rdbuf()->freeze(false); // this must be called after str() to avoid memory leak
#endif

    this->Superclass::PlaceWidget();

	float* pt1 = this->GetPoint1();
	for (int i = 0 ; i < 3; ++i) {
		this->m_InvisablePosX[i] = pt1[i];
	}
#if defined(_DEBUG)
	this->m_VisHandleGeometry->SetCenter(this->m_InvisablePosX);
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
