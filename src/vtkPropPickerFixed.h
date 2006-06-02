/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkPropPickerFixed.h,v $
  Language:  C++
  Date:      $Date: 2003/06/26 02:49:59 $
  Version:   $Revision: 1.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPropPickerFixed - pick an actor/prop using graphics hardware
// .SECTION Description
// vtkPropPickerFixed is used to pick an actor/prop given a selection
// point (in display coordinates) and a renderer. This class uses
// graphics hardware/rendering system to pick rapidly (as compared
// to using ray casting as does vtkCellPicker and vtkPointPicker).
// This class determines the actor/prop and pick position in world
// coordinates; point and cell ids are not determined.

// .SECTION See Also 
// vtkPicker vtkWorldPointPicker vtkCellPicker vtkPointPicker 

#ifndef __vtkPropPickerFixed_h
#define __vtkPropPickerFixed_h

#include <vtkAbstractPropPicker.h>

class vtkProp;
class vtkWorldPointPicker;

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif 

class /*VTK_RENDERING_EXPORT*/ vtkPropPickerFixed : public vtkAbstractPropPicker
{
public:
  static vtkPropPickerFixed *New();

  vtkTypeRevisionMacro(vtkPropPickerFixed,vtkAbstractPropPicker);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Perform the pick and set the PickedProp ivar. If something is picked, a
  // 1 is returned, otherwise 0 is returned.  Use the GetProp() method
  // to get the instance of vtkProp that was picked.  Props are picked from
  // the renderers list of pickable Props.
  int PickProp(vtkFloatingPointType selectionX, vtkFloatingPointType selectionY, vtkRenderer *renderer);  

  // Description:
  // Perform a pick from the user-provided list of vtkProps and not from the
  // list of vtkProps that the render maintains.
  int PickProp(vtkFloatingPointType selectionX, vtkFloatingPointType selectionY, vtkRenderer *renderer, 
               vtkPropCollection* pickfrom);  

  // Description:
  // Overide superclasses' Pick() method.
  int Pick(vtkFloatingPointType selectionX, vtkFloatingPointType selectionY, vtkFloatingPointType selectionZ, 
           vtkRenderer *renderer);  
  int Pick(vtkFloatingPointType selectionPt[3], vtkRenderer *renderer)
    { return this->Pick( selectionPt[0], 
                         selectionPt[1], selectionPt[2], renderer); };  

protected:
  vtkPropPickerFixed();
  ~vtkPropPickerFixed();

  void Initialize();
  
  vtkPropCollection* PickFromProps;
  
  // Used to get x-y-z pick position
  vtkWorldPointPicker *WorldPointPicker;
private:
  vtkPropPickerFixed(const vtkPropPickerFixed&);  // Not implemented.
  void operator=(const vtkPropPickerFixed&);  // Not implemented.
};

#endif


