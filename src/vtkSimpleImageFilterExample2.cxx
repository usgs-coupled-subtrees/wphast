/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSimpleImageFilterExample2.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "stdafx.h"
#include "vtkSimpleImageFilterExample2.h"

#include <vtkImageData.h>
#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkSimpleImageFilterExample2, "$Revision: 1.10 $");
vtkStandardNewMacro(vtkSimpleImageFilterExample2);

// The switch statement in Execute will call this method with
// the appropriate input type (IT). Note that this example assumes
// that the output data type is the same as the input data type.
// This is not always the case.
template <class IT>
void vtkSimpleImageFilterExampleExecute(vtkImageData* input,
                                        vtkImageData* output,
                                        IT* inPtr, IT* outPtr)
{
  int dims[3];
  input->GetDimensions(dims);
  if (input->GetScalarType() != output->GetScalarType())
    {
    vtkGenericWarningMacro(<< "Execute: input ScalarType, " << input->GetScalarType()
    << ", must match out ScalarType " << output->GetScalarType());
	ASSERT(FALSE);
    return;
    }

  //{{
  vtkDataArray *scalars = input->GetPointData()->GetScalars();

  // make sure scalars are non null
  if (!scalars) 
    {
    vtkGenericWarningMacro(<< "No scalar values found for vtkSimpleImageFilterExample2 input!");
	ASSERT(FALSE);
    return;
    }

  int bytesPerPixel = scalars->GetNumberOfComponents();
  int size = dims[0]*dims[1]*dims[2]*bytesPerPixel;
  //}}
// COMMENT: {8/12/2009 4:17:55 PM}  int size = dims[0]*dims[1]*dims[2];

  for(int i=0; i<size; i++)
    {
    outPtr[i] = inPtr[i];
    }
}

vtkSimpleImageFilterExample2::vtkSimpleImageFilterExample2()
: vtkSimpleImageToImageFilter()
{
  //{{ from vtkImageActor
  this->Input = NULL;
  this->Interpolate = 1;
  this->DisplayExtent[0] = -1;
  this->DisplayExtent[1] = 0;
  this->DisplayExtent[2] = 0;
  this->DisplayExtent[3] = 0;
  this->DisplayExtent[4] = 0;
  this->DisplayExtent[5] = 0;  

  this->Bounds[0] = this->Bounds[2] = this->Bounds[4] = -1.0;
  this->Bounds[1] = this->Bounds[3] = this->Bounds[5] = 1.0;
  //}} from vtkImageActor

  //{{ from vtkOpenGLImageActor
  this->Index = 0;
  this->RenderWindow = 0;
  //}} from vtkOpenGLImageActor
}

void vtkSimpleImageFilterExample2::SimpleExecute(vtkImageData* input,
                                                vtkImageData* output)
{

  void* inPtr = input->GetScalarPointer();
  void* outPtr = output->GetScalarPointer();

// COMMENT: {8/12/2009 5:27:31 PM}  switch(output->GetScalarType())
// COMMENT: {8/12/2009 5:27:31 PM}    {
// COMMENT: {8/12/2009 5:27:31 PM}    // This is simple a #define for a big case list. It handles
// COMMENT: {8/12/2009 5:27:31 PM}    // all data types vtk can handle.
// COMMENT: {8/12/2009 5:27:31 PM}    vtkTemplateMacro4(vtkSimpleImageFilterExampleExecute, input, output,
// COMMENT: {8/12/2009 5:27:31 PM}                      (VTK_TT *)(inPtr), (VTK_TT *)(outPtr));
// COMMENT: {8/12/2009 5:27:31 PM}    default:
// COMMENT: {8/12/2009 5:27:31 PM}      vtkGenericWarningMacro("Execute: Unknown input ScalarType");
// COMMENT: {8/12/2009 5:27:31 PM}      return;
// COMMENT: {8/12/2009 5:27:31 PM}    }
  //{{
  this->MakeDataSuitable(input, output, (unsigned char*)inPtr, (unsigned char*)outPtr);
  //}}
}

//----------------------------------------------------------------------------
// Computes any global image information associated with regions.
void vtkSimpleImageFilterExample2::ExecuteInformation()
{
	//{{
	this->vtkSimpleImageToImageFilter::ExecuteInformation();
	//}}
	vtkImageData *inData = this->GetInput();
	vtkImageData *outData = this->GetOutput();
	int xsize;
	int ysize;
// COMMENT: {8/12/2009 10:57:37 PM}  int release;

// COMMENT: {8/12/2009 10:03:36 PM}  float *spacing;
// COMMENT: {8/12/2009 10:03:36 PM}  int idx;
// COMMENT: {8/12/2009 10:03:36 PM}  int *inExt;
// COMMENT: {8/12/2009 10:03:36 PM}  float outSpacing[3];
// COMMENT: {8/12/2009 10:03:36 PM}  int outExt[6];
// COMMENT: {8/12/2009 10:03:36 PM}  
// COMMENT: {8/12/2009 10:03:36 PM}  inExt = inData->GetWholeExtent();
// COMMENT: {8/12/2009 10:03:36 PM}  spacing = inData->GetSpacing();
// COMMENT: {8/12/2009 10:03:36 PM}  for (idx = 0; idx < 3; idx++)
// COMMENT: {8/12/2009 10:03:36 PM}    {
// COMMENT: {8/12/2009 10:03:36 PM}    // Scale the output extent
// COMMENT: {8/12/2009 10:03:36 PM}    outExt[idx*2] = inExt[idx*2] * this->MagnificationFactors[idx];
// COMMENT: {8/12/2009 10:03:36 PM}    outExt[idx*2+1] = outExt[idx*2] + 
// COMMENT: {8/12/2009 10:03:36 PM}      (inExt[idx*2+1] - inExt[idx*2] + 1)*this->MagnificationFactors[idx] - 1;
// COMMENT: {8/12/2009 10:03:36 PM}    
// COMMENT: {8/12/2009 10:03:36 PM}    // Change the data spacing
// COMMENT: {8/12/2009 10:03:36 PM}    outSpacing[idx] = spacing[idx] / (float)(this->MagnificationFactors[idx]);
// COMMENT: {8/12/2009 10:03:36 PM}    }
// COMMENT: {8/12/2009 10:03:36 PM}  
// COMMENT: {8/12/2009 10:03:36 PM}  outData->SetWholeExtent(outExt);
// COMMENT: {8/12/2009 10:03:36 PM}  outData->SetSpacing(outSpacing);

	int *inExt = inData->GetWholeExtent(); // this->DisplayExtent

	int contiguous = 0;
	unsigned short xs,ys;
	int powOfTwo = 0;
	int numComp = inData->GetNumberOfScalarComponents();
	int xdim, ydim;

	// it must be a power of two and contiguous
	// find the two used dimensions
	// this assumes a 2D image, no lines here folk
	if (inExt[0] != inExt[1])
	{
		xdim = 0;
		if (inExt[2] != inExt[3])
		{
			ydim = 1;
		}
		else
		{
			ydim = 2;
		}
	}
	else
	{
		xdim = 1;
		ydim = 2;
	}
  
	float *spacing = inData->GetSpacing();
	float *origin = inData->GetOrigin();

// COMMENT: {8/12/2009 10:18:49 PM}	// compute the world coordinates
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[0] = inExt[0]*spacing[0] + origin[0];
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[1] = inExt[2]*spacing[1] + origin[1];
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[2] = inExt[4]*spacing[2] + origin[2];
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[3] = inExt[1]*spacing[0] + origin[0];
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[4] = 
// COMMENT: {8/12/2009 10:18:49 PM}		inExt[2 + (xdim == 1)]*spacing[1] + origin[1];
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[5] = inExt[4]*spacing[2] + origin[2];
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[6] = inExt[1]*spacing[0] + origin[0];
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[7] = inExt[3]*spacing[1] + origin[1];
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[8] = inExt[5]*spacing[2] + origin[2];
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[9] = inExt[0]*spacing[0] + origin[0];
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[10] = 
// COMMENT: {8/12/2009 10:18:49 PM}		inExt[2 + (ydim == 1)]*spacing[1] + origin[1];
// COMMENT: {8/12/2009 10:18:49 PM}	this->Coords[11] = inExt[5]*spacing[2] + origin[2];
  
	// now contiguous would require that xdim = 0 and ydim = 1
	// OR xextent = 1 pixel and xdim = 1 and ydim = 2 
	// OR xdim = 0 and ydim = 2 and yextent = i pixel
	int *ext = inData->GetExtent();
	if ((xdim ==0 && ydim == 1)||
		(ext[0] == ext[1] && xdim == 1) ||
		(ext[2] == ext[3] && xdim == 0 && ydim == 2))
	{
		contiguous = 1;
	}
      
	// if contiguous is it a pow of 2
	if (contiguous)
	{
		xsize = ext[xdim*2+1] - ext[xdim*2] + 1;
		// xsize and ysize must be a power of 2 in OpenGL
		xs = (unsigned short)xsize;
		while (!(xs & 0x01))
		{
			xs = xs >> 1;
		}
		if (xs == 1)
		{
			powOfTwo = 1;
		}
	}

	if (contiguous && powOfTwo)
	{
		// can we make y a power of two also ?
		ysize = inExt[ydim*2+1] - inExt[ydim*2] + 1;
		ys = (unsigned short)ysize;
		while (!(ys & 0x01))
		{
			ys = ys >> 1;
		}
		// yes it is a power of two already
		if (ys == 1)
		{
// COMMENT: {8/12/2009 10:56:48 PM}			release = 0;
// COMMENT: {8/12/2009 10:20:50 PM}			this->TCoords[0] = (inExt[xdim*2] - ext[xdim*2] + 0.5)/xsize;
// COMMENT: {8/12/2009 10:20:50 PM}			this->TCoords[1] = 0.5/ysize;  
// COMMENT: {8/12/2009 10:20:50 PM}			this->TCoords[2] = (inExt[xdim*2+1] - ext[xdim*2] + 0.5)/xsize;
// COMMENT: {8/12/2009 10:20:50 PM}			this->TCoords[3] = this->TCoords[1];  
// COMMENT: {8/12/2009 10:20:50 PM}			this->TCoords[4] = this->TCoords[2];
// COMMENT: {8/12/2009 10:20:50 PM}			this->TCoords[5] = 1.0 - 0.5/ysize;  
// COMMENT: {8/12/2009 10:20:50 PM}			this->TCoords[6] = this->TCoords[0];
// COMMENT: {8/12/2009 10:20:50 PM}			this->TCoords[7] = this->TCoords[5];  
			// data ok
			return;
		}
	}

	// find the target size
	xsize = 1;
	while (xsize < inExt[xdim*2+1] - inExt[xdim*2] + 1)
	{
		xsize *= 2;
	}
	ysize = 1;
	while (ysize < inExt[ydim*2+1] - inExt[ydim*2] + 1)
	{
		ysize *= 2;
	}


	int outExt[6];
	outExt[0] = inExt[0];
	outExt[1] = xsize - 1;
	outExt[2] = inExt[2];
	outExt[3] = ysize - 1;
	outExt[4] = inExt[4];
	outExt[5] = inExt[5];
	outData->SetWholeExtent(outExt);
}

//
// modified from
// unsigned char *vtkOpenGLImageActor::MakeDataSuitable(int &xsize, int &ysize,
//                                                     int &release)
//
void vtkSimpleImageFilterExample2::MakeDataSuitable(vtkImageData* input, vtkImageData* output, unsigned char *inPtr, unsigned char *outPtr)
{
  //{{
  // if the display extent has not been set, then compute one
  int *wExtent = input->GetWholeExtent();
  if (this->DisplayExtent[0] == -1)
    {
    this->DisplayExtent[0] = wExtent[0];
    this->DisplayExtent[1] = wExtent[1];
    this->DisplayExtent[2] = wExtent[2];
    this->DisplayExtent[3] = wExtent[3];
    this->DisplayExtent[4] = wExtent[4];
    this->DisplayExtent[5] = wExtent[4];
    }
  //}}
  //{{
  this->Input = input;
  int xsize;
  int ysize;
  int release;
  //}}
  int contiguous = 0;
  unsigned short xs,ys;
  int powOfTwo = 0;
  int numComp = this->Input->GetNumberOfScalarComponents();
  int xdim, ydim;
  
  // it must be a power of two and contiguous
  // find the two used dimensions
  // this assumes a 2D image, no lines here folk
  if (this->DisplayExtent[0] != this->DisplayExtent[1])
    {
    xdim = 0;
    if (this->DisplayExtent[2] != this->DisplayExtent[3])
      {
      ydim = 1;
      }
    else
      {
      ydim = 2;
      }
    }
  else
    {
    xdim = 1;
    ydim = 2;
    }
  
  float *spacing = this->Input->GetSpacing();
  float *origin = this->Input->GetOrigin();
  
  // compute the world coordinates
  this->Coords[0] = this->DisplayExtent[0]*spacing[0] + origin[0];
  this->Coords[1] = this->DisplayExtent[2]*spacing[1] + origin[1];
  this->Coords[2] = this->DisplayExtent[4]*spacing[2] + origin[2];
  this->Coords[3] = this->DisplayExtent[1]*spacing[0] + origin[0];
  this->Coords[4] = 
    this->DisplayExtent[2 + (xdim == 1)]*spacing[1] + origin[1];
  this->Coords[5] = this->DisplayExtent[4]*spacing[2] + origin[2];
  this->Coords[6] = this->DisplayExtent[1]*spacing[0] + origin[0];
  this->Coords[7] = this->DisplayExtent[3]*spacing[1] + origin[1];
  this->Coords[8] = this->DisplayExtent[5]*spacing[2] + origin[2];
  this->Coords[9] = this->DisplayExtent[0]*spacing[0] + origin[0];
  this->Coords[10] = 
    this->DisplayExtent[2 + (ydim == 1)]*spacing[1] + origin[1];
  this->Coords[11] = this->DisplayExtent[5]*spacing[2] + origin[2];
  
  // now contiguous would require that xdim = 0 and ydim = 1
  // OR xextent = 1 pixel and xdim = 1 and ydim = 2 
  // OR xdim = 0 and ydim = 2 and yextent = i pixel
  int *ext = this->Input->GetExtent();
  if ((xdim ==0 && ydim == 1)||
      (ext[0] == ext[1] && xdim == 1) ||
      (ext[2] == ext[3] && xdim == 0 && ydim == 2))
    {
    contiguous = 1;
    }
      
  // if contiguous is it a pow of 2
  if (contiguous)
    {
    xsize = ext[xdim*2+1] - ext[xdim*2] + 1;
    // xsize and ysize must be a power of 2 in OpenGL
    xs = (unsigned short)xsize;
    while (!(xs & 0x01))
      {
      xs = xs >> 1;
      }
    if (xs == 1)
      {
      powOfTwo = 1;
      }
    }
  
  if (contiguous && powOfTwo)
    {
    // can we make y a power of two also ?
    ysize = this->DisplayExtent[ydim*2+1] - this->DisplayExtent[ydim*2] + 1;
    ys = (unsigned short)ysize;
    while (!(ys & 0x01))
      {
      ys = ys >> 1;
      }
    // yes it is a power of two already
    if (ys == 1)
      {
      release = 0;
      this->TCoords[0] = (this->DisplayExtent[xdim*2] - ext[xdim*2] + 0.5)/xsize;
      this->TCoords[1] = 0.5/ysize;  
      this->TCoords[2] = (this->DisplayExtent[xdim*2+1] - ext[xdim*2] + 0.5)/xsize;
      this->TCoords[3] = this->TCoords[1];  
      this->TCoords[4] = this->TCoords[2];
      this->TCoords[5] = 1.0 - 0.5/ysize;  
      this->TCoords[6] = this->TCoords[0];
      this->TCoords[7] = this->TCoords[5];  
// COMMENT: {8/12/2009 5:08:29 PM}      return (unsigned char *)
// COMMENT: {8/12/2009 5:08:29 PM}        this->Input->GetScalarPointerForExtent(this->DisplayExtent);
	  // data ok
////{{{{
  switch(output->GetScalarType())
    {
    // This is simple a #define for a big case list. It handles
    // all data types vtk can handle.
    vtkTemplateMacro4(vtkSimpleImageFilterExampleExecute, input, output,
                      (VTK_TT *)(inPtr), (VTK_TT *)(outPtr));
    default:
      vtkGenericWarningMacro("Execute: Unknown input ScalarType");
      return;
    }
////}}}}
	  return;
      }
    }
  
  // if we made it here then we must copy the data and possibly pad 
  // it as well
  release = 1;
  // find the target size
  xsize = 1;
  while (xsize < 
         this->DisplayExtent[xdim*2+1] - this->DisplayExtent[xdim*2] + 1)
    {
    xsize *= 2;
    }
  ysize = 1;
  while (ysize < 
         this->DisplayExtent[ydim*2+1] - this->DisplayExtent[ydim*2] + 1)
    {
    ysize *= 2;
    }
  
  // compute the tcoords
  this->TCoords[0] = 0.5/xsize;
  this->TCoords[1] = 0.5/ysize;  
  this->TCoords[2] = (this->DisplayExtent[xdim*2+1] - this->DisplayExtent[xdim*2] + 0.5)/xsize;
  this->TCoords[3] = this->TCoords[1];  
  this->TCoords[4] = this->TCoords[2];
  this->TCoords[5] = (this->DisplayExtent[ydim*2+1] - this->DisplayExtent[ydim*2] + 0.5)/ysize;  
  this->TCoords[6] = this->TCoords[0];
  this->TCoords[7] = this->TCoords[5];  

  // allocate the memory
  unsigned char *res = new unsigned char [ysize*xsize*numComp];
  
  // copy the input data to the memory
  int inIncX, inIncY, inIncZ;
  int idxZ, idxY, idxR;
  unsigned char *inPtr2 = (unsigned char *)
    this->Input->GetScalarPointerForExtent(this->DisplayExtent);
  this->Input->GetContinuousIncrements(this->DisplayExtent, 
                                       inIncX, inIncY, inIncZ);
  int rowLength = numComp*(this->DisplayExtent[1] -this->DisplayExtent[0] +1);
  unsigned char *outPtr2 = res;
  //{{
  unsigned char *res2 = outPtr;
  //}}
  int outIncY, outIncZ;
  if (ydim == 2)
    {
    if (xdim == 0)
      {
      outIncZ = numComp * 
        (xsize - (this->DisplayExtent[1] - this->DisplayExtent[0] + 1));
      }
    else
      {
      outIncZ = numComp * 
        (xsize - (this->DisplayExtent[3] - this->DisplayExtent[2] + 1));
      }
    outIncY = 0;
    }
  else
    {
    outIncY = numComp * 
      (xsize - (this->DisplayExtent[1] - this->DisplayExtent[0] + 1));
    outIncZ = 0;    
    }
  
// COMMENT: {8/12/2009 11:01:50 PM}  //{{
// COMMENT: {8/12/2009 11:01:50 PM}  // TODO need to update WholeExtent and Spacing
// COMMENT: {8/12/2009 11:01:50 PM}  output->SetWholeExtent(0, xsize, 0, ysize, 0, 0);
// COMMENT: {8/12/2009 11:01:50 PM}  output->SetExtent(0, xsize, 0, ysize, 0, 0);
// COMMENT: {8/12/2009 11:01:50 PM}  output->SetSpacing(spacing);
// COMMENT: {8/12/2009 11:01:50 PM}  output->AllocateScalars();
// COMMENT: {8/12/2009 11:01:50 PM}  //}}
      
  for (idxZ = this->DisplayExtent[4]; idxZ <= this->DisplayExtent[5]; idxZ++)
    {
    for (idxY = this->DisplayExtent[2]; idxY <= this->DisplayExtent[3]; idxY++)
      {
      for (idxR = 0; idxR < rowLength; idxR++)
        {
        // Pixel operation
        *outPtr = *inPtr2;
        outPtr++;
		//{{
		*outPtr2 = *inPtr2;
		outPtr2++;
		//}}
        inPtr2++;
        }
      outPtr += outIncY;
	  //{{
	  outPtr2 += outIncY;
	  //}}
      inPtr2 += inIncY;
      }
    outPtr += outIncZ;
	//{{
	outPtr2 += outIncZ;
	//}}
    inPtr2 += inIncZ;
    }
  
// COMMENT: {8/12/2009 5:00:04 PM}  return res;
////{{{{
// COMMENT: {8/12/2009 5:51:36 PM}  //{{
// COMMENT: {8/12/2009 5:51:36 PM}  // TODO need to update WholeExtent and Spacing
// COMMENT: {8/12/2009 5:51:36 PM}  output->SetWholeExtent(0, xsize, 0, ysize, 0, 0);
// COMMENT: {8/12/2009 5:51:36 PM}  output->SetSpacing(spacing);
// COMMENT: {8/12/2009 5:51:36 PM}  //}}
////}}}}
  delete [] res;
}
