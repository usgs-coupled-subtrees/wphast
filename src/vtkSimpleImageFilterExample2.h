/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSimpleImageFilterExample2.h,v $
  Language:  C++
  Date:      $Date: 2002/01/22 15:33:47 $
  Version:   $Revision: 1.11 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSimpleImageFilterExample2 - Simple example of an image-image filter.
// .SECTION Description
// This is an example of a simple image-image filter. It copies it's input
// to it's output (point by point). It shows how templates can be used
// to support various data types.
// .SECTION See also
// vtkSimpleImageFilterExample2

#ifndef __vtkSimpleImageFilterExample_h
#define __vtkSimpleImageFilterExample_h

#include <vtkSimpleImageToImageFilter.h>

class vtkSimpleImageFilterExample2 : public vtkSimpleImageToImageFilter
{
public:
  static vtkSimpleImageFilterExample2 *New();
  vtkTypeRevisionMacro(vtkSimpleImageFilterExample2,vtkSimpleImageToImageFilter);

protected:

// COMMENT: {8/12/2009 5:15:14 PM}  vtkSimpleImageFilterExample2() {};
  vtkSimpleImageFilterExample2();
  ~vtkSimpleImageFilterExample2() {};

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output);
  void MakeDataSuitable(vtkImageData* input, vtkImageData* output, unsigned char *inPtr, unsigned char *outPtr);

  //{{ from vtkImageActor
  int           Interpolate;
  vtkImageData* Input;
  int           DisplayExtent[6];
  float         Bounds[6];
  //}} from vtkImageActor

  //{{ from vtkOpenGLImageActor
  vtkTimeStamp   LoadTime;
  long          Index;
  vtkRenderWindow *RenderWindow;   // RenderWindow used for previous render
  float Coords[12];
  float TCoords[8];
  //}} from vtkOpenGLImageActor

  // These are called by the superclass.
  // You might have to override ExecuteInformation
  virtual void ExecuteInformation();
// COMMENT: {8/12/2009 9:51:46 PM}  virtual void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);

  // You don't have to touch this unless you have a good reason.
  //virtual void ExecuteData(vtkDataObject *output);
  // In the simplest case, this is the only method you need to define.
  //virtual void SimpleExecute(vtkImageData* input, vtkImageData* output) = 0;

private:
  vtkSimpleImageFilterExample2(const vtkSimpleImageFilterExample2&);  // Not implemented.
  void operator=(const vtkSimpleImageFilterExample2&);  // Not implemented.
};

#endif







