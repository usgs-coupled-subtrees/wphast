#include "StdAfx.h"
#include "vtkInteractorStyleTrackballCameraEx.h"

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCallbackCommand.h>
#include <vtkCellPicker.h>
#include <vtkMath.h> 
#include <vtkObjectFactory.h>
#include <vtkOldStyleCallbackCommand.h>
#include <vtkOutlineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>



vtkCxxRevisionMacro(vtkInteractorStyleTrackballCameraEx, "$Revision$");
vtkStandardNewMacro(vtkInteractorStyleTrackballCameraEx);


vtkInteractorStyleTrackballCameraEx::vtkInteractorStyleTrackballCameraEx(void)
{
}

vtkInteractorStyleTrackballCameraEx::~vtkInteractorStyleTrackballCameraEx(void)
{
}

//----------------------------------------------------------------------------
// When pick action successfully selects a vtkProp3Dactor, this method
// highlights the vtkProp3D appropriately. Currently this is done by placing a
// bounding box around the vtkProp3D.
void vtkInteractorStyleTrackballCameraEx::HighlightProp3D(vtkProp3D *prop3D) 
{
  if ( ! this->OutlineActor ) 
    {
    // have to defer creation to get right type
    this->OutlineActor = vtkActor::New();
    this->OutlineActor->PickableOff();
    this->OutlineActor->DragableOff();
    this->OutlineActor->SetMapper(this->OutlineMapper);
    this->OutlineActor->GetProperty()->SetColor(this->PickColor);
    this->OutlineActor->GetProperty()->SetAmbient(1.0);
    this->OutlineActor->GetProperty()->SetDiffuse(0.0);
    }
  
  //no prop picked now 
  if ( ! prop3D) 
    {
    //was there previously?
    if (this->PickedRenderer != NULL) 
      {
      this->PickedRenderer->RemoveActor(this->OutlineActor);
      this->PickedRenderer = NULL;
      }
    }
  //prop picked now 
  else
    {
	//{{SRC
	// This is added in order to allow highlights without
	// actually picking using the mouse+p combination
	// This may cause problems if there is more than 1 renderer
	if (!this->PickedRenderer) {
		this->FindPokedRenderer(0, 0);
	}
	//{
	//BUGBUG
	// Click new zone button
	// attempt to edit an existing zone
	// this->Interactor is NULL
	// NEED TO CANCEL EDIT ???
	ASSERT(this->Interactor);
	if (!this->Interactor) return; // HACKHACK
	//}
	//}}SRC
    //check if picked in different renderer to previous pick
    if (this->CurrentRenderer != this->PickedRenderer)
      {
      if (this->PickedRenderer != NULL)
        {
        this->PickedRenderer->RemoveActor(this->OutlineActor);
        }
      this->CurrentRenderer->AddActor(this->OutlineActor);
      this->PickedRenderer = this->CurrentRenderer;      
      }
    this->Outline->SetBounds(prop3D->GetBounds());
    }
}
