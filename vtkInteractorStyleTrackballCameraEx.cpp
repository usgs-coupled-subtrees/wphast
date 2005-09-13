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

#include <vtkPropAssembly.h>



vtkCxxRevisionMacro(vtkInteractorStyleTrackballCameraEx, "$Revision$");
vtkStandardNewMacro(vtkInteractorStyleTrackballCameraEx);


vtkInteractorStyleTrackballCameraEx::vtkInteractorStyleTrackballCameraEx(void)
: PickWithMouse(0)
, LastProp(0)
{
}

vtkInteractorStyleTrackballCameraEx::~vtkInteractorStyleTrackballCameraEx(void)
{
}

void vtkInteractorStyleTrackballCameraEx::Delete(void)
{
	if (this->Interactor)
	{
		// HACK
		// If this isn't here a reference count proble occurs
		//
		vtkPropCollection *pickProps = this->Interactor->GetPicker()->GetPickList();
		ASSERT(pickProps);
		pickProps->RemoveAllItems();
	}

	this->Superclass::Delete();
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
	if (!this->PickedRenderer)
	{
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

//----------------------------------------------------------------------------
void vtkInteractorStyleTrackballCameraEx::OnChar() 
{
  vtkRenderWindowInteractor *rwi = this->Interactor;

  switch (rwi->GetKeyCode()) 
    {
    case 'm' :
    case 'M' :
      if (this->AnimState == VTKIS_ANIM_OFF) 
        {
        this->StartAnimate();
        }
      else 
        {
        this->StopAnimate();
        }
      break;

// COMMENT: {9/7/2005 9:46:57 PM}    case 'Q' :
// COMMENT: {9/7/2005 9:46:57 PM}    case 'q' :
// COMMENT: {9/7/2005 9:46:57 PM}    case 'e' :
// COMMENT: {9/7/2005 9:46:57 PM}    case 'E' :
// COMMENT: {9/7/2005 9:46:57 PM}      rwi->ExitCallback();
// COMMENT: {9/7/2005 9:46:57 PM}      break;

    case 'f' :      
    case 'F' :
      {
      this->AnimState = VTKIS_ANIM_ON;
      vtkAssemblyPath *path = NULL;
      this->FindPokedRenderer(rwi->GetEventPosition()[0],
                              rwi->GetEventPosition()[1]);
      rwi->GetPicker()->Pick(rwi->GetEventPosition()[0],
                             rwi->GetEventPosition()[1], 
                             0.0, 
                             this->CurrentRenderer);
      vtkAbstractPropPicker *picker;
      if ((picker=vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker())))
        {
        path = picker->GetPath();
        }
      if (path != NULL)
        {
        rwi->FlyTo(this->CurrentRenderer, picker->GetPickPosition());
        }
      this->AnimState = VTKIS_ANIM_OFF;
      }
      break;

    case 'u' :
    case 'U' :
      rwi->UserCallback();
      break;

    case 'r' :
    case 'R' :
      this->FindPokedRenderer(rwi->GetEventPosition()[0], 
                              rwi->GetEventPosition()[1]);
      this->CurrentRenderer->ResetCamera();
      rwi->Render();
      break;

    case 'w' :
    case 'W' :
      {
      vtkActorCollection *ac;
      vtkActor *anActor, *aPart;
      vtkAssemblyPath *path;
      this->FindPokedRenderer(rwi->GetEventPosition()[0],
                              rwi->GetEventPosition()[1]);
      ac = this->CurrentRenderer->GetActors();
      for (ac->InitTraversal(); (anActor = ac->GetNextItem()); ) 
        {
        for (anActor->InitPathTraversal(); (path=anActor->GetNextPath()); ) 
          {
          aPart=(vtkActor *)path->GetLastNode()->GetProp();
          aPart->GetProperty()->SetRepresentationToWireframe();
          }
        }
      rwi->Render();
      }
      break;

    case 's' :
    case 'S' :
      {
      vtkActorCollection *ac;
      vtkActor *anActor, *aPart;
      vtkAssemblyPath *path;
      this->FindPokedRenderer(rwi->GetEventPosition()[0],
                              rwi->GetEventPosition()[1]);
      ac = this->CurrentRenderer->GetActors();
      for (ac->InitTraversal(); (anActor = ac->GetNextItem()); ) 
        {
        for (anActor->InitPathTraversal(); (path=anActor->GetNextPath()); ) 
          {
          aPart=(vtkActor *)path->GetLastNode()->GetProp();
          aPart->GetProperty()->SetRepresentationToSurface();
          }
        }
      rwi->Render();
      }
      break;

    case 'l' :
    case 'L' :
      {
      int val = vtkTextProperty::GetGlobalAntiAliasing();
      // Cycle through global anti-aliasing control
      if (val == VTK_TEXT_GLOBAL_ANTIALIASING_ALL)
        {
        val = VTK_TEXT_GLOBAL_ANTIALIASING_SOME;
        }
      else
        {
        val++;
        }
      vtkTextProperty::SetGlobalAntiAliasing(val);
      rwi->Render();
      }
      break;

    case '3' :
      if (rwi->GetRenderWindow()->GetStereoRender()) 
        {
        rwi->GetRenderWindow()->StereoRenderOff();
        }
      else 
        {
        rwi->GetRenderWindow()->StereoRenderOn();
        }
      rwi->Render();
      break;

    case 'p' :
    case 'P' :
      if (this->State == VTKIS_NONE) 
        {
        vtkAssemblyPath *path=NULL;
        this->FindPokedRenderer(rwi->GetEventPosition()[0],
                                rwi->GetEventPosition()[1]);
        rwi->StartPickCallback();
        rwi->GetPicker()->Pick(rwi->GetEventPosition()[0],
                               rwi->GetEventPosition()[1], 
                               0.0, 
                               this->CurrentRenderer);
        vtkAbstractPropPicker *picker;
        if ( (picker=vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker())) )
          {
          path = picker->GetPath();
          }
        if ( path == NULL )
          {
          this->HighlightProp(NULL);
          this->PropPicked = 0;
          }
        else
          {
          this->HighlightProp(path->GetFirstNode()->GetProp());
          this->PropPicked = 1;
          }
        rwi->EndPickCallback();
        }
      break;
    }
}

void vtkInteractorStyleTrackballCameraEx::OnLeftButtonDown()
{
	if (!this->PickWithMouse)
	{
		this->Superclass::OnLeftButtonDown();
	}
	else
	{
		vtkRenderWindowInteractor *rwi = this->Interactor;
		if (this->State == VTKIS_NONE) 
		{
			vtkAssemblyPath *path=NULL;
			this->FindPokedRenderer(rwi->GetEventPosition()[0],
				rwi->GetEventPosition()[1]);
			rwi->StartPickCallback();
			//{{
			this->LastEventPosition[0] = rwi->GetEventPosition()[0];
			this->LastEventPosition[1] = rwi->GetEventPosition()[1];
			//}}
			rwi->GetPicker()->Pick(rwi->GetEventPosition()[0],
				rwi->GetEventPosition()[1], 
				0.0, 
				this->CurrentRenderer);
			vtkAbstractPropPicker *picker;
			if ( (picker=vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker())) )
			{
				path = picker->GetPath();
			}
			if ( path == NULL )
			{
				this->LastProp = NULL;
				this->HighlightProp(NULL);
				this->PropPicked = 0;
			}
			else
			{
				if (path->GetNumberOfItems() == 3)
				{
					path->InitTraversal();
					vtkProp* pPropAssembly = path->GetNextNode()->GetProp();
					ASSERT(pPropAssembly->IsA("vtkPropAssembly"));
					this->LastProp = path->GetNextNode()->GetProp();
				}
				else
				{
					this->LastProp = path->GetLastNode()->GetProp();
				}

				ASSERT(
					this->LastProp->IsA("CZoneActor")
					||
					this->LastProp->IsA("CWellActor")
					||
					this->LastProp->IsA("CRiverActor")
					);

				ASSERT(this->LastProp);
				this->HighlightProp(this->LastProp);
				this->PropPicked = 1;
			}
			rwi->EndPickCallback();
		}
	}
}

void vtkInteractorStyleTrackballCameraEx::OnLeftButtonUp()
{
	if (!this->PickWithMouse)
	{
		this->Superclass::OnLeftButtonUp();
	}
}

void vtkInteractorStyleTrackballCameraEx::OnKeyPress()
{
	if (!this->Interactor) return;

	if (this->State == VTKIS_NONE) 
	{
		char* keysym = this->Interactor->GetKeySym();		
		vtkRenderWindowInteractor *rwi = this->Interactor;
		if (::strcmp(keysym, "Tab") == 0 && this->LastProp)
		{
			vtkAssemblyPath *path=NULL;
			rwi->StartPickCallback();

			vtkPropCollection *props = rwi->GetPicker()->GetPickList();
			ASSERT(props);
			if (props->GetNumberOfItems() == 0)
			{
				// this is the first time Tab has been pressed
				vtkProp *prop;
				props = this->CurrentRenderer->GetProps();
				for ( props->InitTraversal(); (prop = props->GetNextProp()); )
				{
					if (vtkPropAssembly *pPropAssembly = vtkPropAssembly::SafeDownCast(prop))
					{
						if (vtkPropCollection *pPropCollection = pPropAssembly->GetParts())
						{
							pPropCollection->InitTraversal();
							for (int i = 0; i < pPropCollection->GetNumberOfItems(); ++i)
							{
								if (vtkProp* p = pPropCollection->GetNextProp())
								{
									rwi->GetPicker()->AddPickList(p);
									ASSERT(
										p->IsA("CZoneActor")
										||
										p->IsA("CWellActor")
										||
										p->IsA("CRiverActor")
										);
								}
							}
						}
					}
				}
				rwi->GetPicker()->PickFromListOn();
			}
			rwi->GetPicker()->DeletePickList(this->LastProp);

			rwi->GetPicker()->Pick(this->LastEventPosition[0],
				this->LastEventPosition[1], 
				0.0, 
				this->CurrentRenderer);
			vtkAbstractPropPicker *picker;
			if ( (picker=vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker())) )
			{
				path = picker->GetPath();
				if ( path == NULL )
				{
					vtkPropCollection *props = rwi->GetPicker()->GetPickList();
					ASSERT(props);
					props->RemoveAllItems();
					rwi->GetPicker()->PickFromListOff();
					rwi->GetPicker()->Pick(this->LastEventPosition[0],
						this->LastEventPosition[1], 
						0.0, 
						this->CurrentRenderer);
					path = picker->GetPath();
				}
			}
			if ( path == NULL )
			{
				ASSERT(FALSE);
				this->LastProp = NULL;
				this->HighlightProp(NULL);
				this->PropPicked = 0;
			}
			else
			{
				if (path->GetNumberOfItems() == 3)
				{
					path->InitTraversal();
					vtkProp* pPropAssembly = path->GetNextNode()->GetProp();
					ASSERT(pPropAssembly->IsA("vtkPropAssembly"));
					this->LastProp = path->GetNextNode()->GetProp();
				}
				else
				{
					this->LastProp = path->GetLastNode()->GetProp();
				}

				ASSERT(
					this->LastProp->IsA("CZoneActor")
					||
					this->LastProp->IsA("CWellActor")
					||
					this->LastProp->IsA("CRiverActor")
					);

				ASSERT(this->LastProp);
				this->HighlightProp(this->LastProp);
				this->PropPicked = 1;
			}
			rwi->EndPickCallback();
		}
		this->EventCallbackCommand->SetAbortFlag(1);
		this->Interactor->Render();
	}
}
