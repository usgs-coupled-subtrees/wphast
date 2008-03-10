#include "stdafx.h"

#include "srcWedgeSource.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"

#include <math.h>

// COMMENT: {2/29/2008 8:04:04 PM}#define CHOP_MIN_X_MAX_Y
// COMMENT: {3/3/2008 2:42:26 PM}#define CHOP_MAX_X_MAX_Y
// COMMENT: {3/5/2008 9:44:58 PM}#define CHOP_MAX_X_MIN_Y
// COMMENT: {3/4/2008 8:58:29 PM}#define CHOP_MIN_X_MIN_Y

// COMMENT: {2/29/2008 9:17:56 PM}#define CHOP_MIN_X_MAX_Z
// COMMENT: {2/29/2008 9:29:38 PM}#define CHOP_MAX_X_MAX_Z
// COMMENT: {2/29/2008 9:39:14 PM}#define CHOP_MAX_X_MIN_Z
// COMMENT: {2/29/2008 10:23:55 PM}#define CHOP_MIN_X_MIN_Z

// COMMENT: {2/29/2008 10:33:34 PM}#define CHOP_MAX_Y_MIN_Z
// COMMENT: {2/29/2008 10:46:47 PM}#define CHOP_MAX_Y_MAX_Z
// COMMENT: {2/29/2008 11:16:27 PM}#define CHOP_MIN_Y_MAX_Z
// COMMENT: {3/3/2008 2:20:46 PM}#define CHOP_MIN_Y_MIN_Z

#define SKIP_NORMALS
#define SKIP_TCOORDS

vtkCxxRevisionMacro(srcWedgeSource, "$Revision: 1.48 $");
vtkStandardNewMacro(srcWedgeSource);

srcWedgeSource::srcWedgeSource(float xL, float yL, float zL)
: ChopType(CHOP_NONE)
{
  this->XLength = fabs(xL);
  this->YLength = fabs(yL);
  this->ZLength = fabs(zL);

  this->Center[0] = 0.0;
  this->Center[1] = 0.0;
  this->Center[2] = 0.0;
}

void srcWedgeSource::Execute()
{
  float x[3], n[3], tc[3];
  int numPolys=6, numPts=24;
// COMMENT: {8/23/2007 7:44:20 PM}  int numPolys=5, numPts=24;
  int i, j, k;
  vtkIdType pts[4];
  vtkPoints *newPoints; 
#ifdef SKIP_NORMALS
  vtkFloatArray *newNormals;
#endif // SKIP_NORMALS
#ifdef SKIP_TCOORDS
  vtkFloatArray *newTCoords; // CCS 7/27/98 Added for Texture Mapping
#endif // SKIP_TCOORDS
  vtkCellArray *newPolys;
  vtkPolyData *output = this->GetOutput();
  
  vtkDebugMacro(<<"Creating polygonal cube");
  TRACE("XLength = %g, YLength = %g, ZLength = %g\n", this->XLength, this->YLength, this->ZLength);
//
// Set things up; allocate memory
//
  newPoints = vtkPoints::New();
  newPoints->Allocate(numPts);
// COMMENT: {2/28/2008 7:53:39 PM}  //{{
// COMMENT: {2/28/2008 7:53:39 PM}  newPoints->Allocate(8);
// COMMENT: {2/28/2008 7:53:39 PM}  //}}
#ifdef SKIP_NORMALS
  newNormals = vtkFloatArray::New();
  newNormals->SetNumberOfComponents(3);
  newNormals->Allocate(numPts);
  newNormals->SetName("Normals");
#endif // SKIP_NORMALS
#ifdef SKIP_TCOORDS
  newTCoords = vtkFloatArray::New();
  newTCoords->SetNumberOfComponents(2);
  newTCoords->Allocate(numPts);
  newTCoords->SetName("TCoords");
#endif // SKIP_TCOORDS

  newPolys = vtkCellArray::New();
  newPolys->Allocate(newPolys->EstimateSize(numPolys,4));
//
// Generate points and normals
//
  numPts = 0;

  for (x[0]=this->Center[0]-this->XLength/2.0, n[0]=(-1.0), n[1]=n[2]=0.0, i=0; 
  i<2; i++, x[0]+=this->XLength, n[0]+=2.0)
    {
    for (x[1]=this->Center[1]-this->YLength/2.0, j=0; j<2; 
    j++, x[1]+=this->YLength)
      {
      tc[1] =  x[1] + 0.5;
      for (x[2]=this->Center[2]-this->ZLength/2.0, k=0; k<2; 
      k++, x[2]+=this->ZLength)
        {
/*
	  // fixed z
	  CHOP_MIN_X_MAX_Y,
	  CHOP_MAX_X_MAX_Y,
	  CHOP_MAX_X_MIN_Y,  
	  CHOP_MIN_X_MIN_Y,
	  // fixed y
	  CHOP_MIN_X_MAX_Z,
	  CHOP_MAX_X_MAX_Z,
	  CHOP_MAX_X_MIN_Z,
	  CHOP_MIN_X_MIN_Z,
	  // fixed x
	  CHOP_MAX_Y_MIN_Z,
	  CHOP_MAX_Y_MAX_Z,
	  CHOP_MIN_Y_MAX_Z,
	  CHOP_MIN_Y_MIN_Z,
*/
		/*
			   2---------6
			  /|        /|
			 / |       / |
			3--+------7  |
			|  0------+--4
			| /       | /
			|/        |/
			1---------5
		*/

		float save;
		switch (this->ChopType)
		{
		case CHOP_NONE:
			// do nothing
			break;
		//
		// Fixed-Z
		//
		case CHOP_MIN_X_MAX_Y:
			// pt2 becomes the same as pt0
			// pt3 becomes the same as pt1
			save = x[1];
			if (i==0 && j==1)
			{
				x[1] = this->Center[1]-this->YLength/2.0;
			}
			break;
		case CHOP_MAX_X_MAX_Y:
			// pt6 becomes the same as pt4
			// pt7 becomes the same as pt5
			save = x[1];
			if (i==1 && j==1)
			{
				x[1] = this->Center[1]-this->YLength/2.0;		
			}
			break;
		case CHOP_MAX_X_MIN_Y:
			// pt4 becomes the same as pt6
			// pt5 becomes the same as pt7
			save = x[1];
			if (i==1 && j==0)
			{
				x[1] = this->Center[1]+this->YLength/2.0;
			}
			break;
		case CHOP_MIN_X_MIN_Y:
			// pt0 becomes the same as pt2
			// pt1 becomes the same as pt3
			save = x[1];
			if (i==0 && j==0)
			{
				x[1] = this->Center[1]+this->YLength/2.0;		
			}
			break;
		//
		// Fixed-Y
		//
		case CHOP_MIN_X_MAX_Z:
			// pt1 becomes the same as pt5
			// pt3 becomes the same as pt7
			save = x[0];
			if (i==0 && k==1)
			{
				x[0] = this->Center[0]+this->XLength/2.0;
			}
			break;
		case CHOP_MAX_X_MAX_Z:
			// pt5 becomes the same as pt1
			// pt7 becomes the same as pt3
			save = x[0];
			if (i==1 && k==1)
			{
				x[0] = this->Center[0]-this->XLength/2.0;
			}
			break;
		case CHOP_MAX_X_MIN_Z:
			// pt4 becomes the same as pt0
			// pt6 becomes the same as pt2
			save = x[0];
			if (i==1 && k==0)
			{
				x[0] = this->Center[0]-this->XLength/2.0;
			}
			break;
		case CHOP_MIN_X_MIN_Z:
			// pt0 becomes the same as pt4
			// pt2 becomes the same as pt6
			save = x[0];
			if (i==0 && k==0)
			{
				x[0] = this->Center[0]+this->XLength/2.0;
			}
			break;
		//
		// Fixed-X
		//
		case CHOP_MAX_Y_MIN_Z:
			// pt2 becomes the same as pt3
			// pt6 becomes the same as pt7
			save = x[2];
			if (j==1 && k==0)
			{
				x[2] = this->Center[2]+this->ZLength/2.0;
			}
			break;
		case CHOP_MAX_Y_MAX_Z:
			// pt3 becomes the same as pt2
			// pt7 becomes the same as pt6
			save = x[2];
			if (j==1 && k==1)
			{
				x[2] = this->Center[2]-this->ZLength/2.0;
			}
			break;
		case CHOP_MIN_Y_MAX_Z:
			// pt1 becomes the same as pt0
			// pt5 becomes the same as pt4
			save = x[2];
			if (j==0 && k==1)
			{
				x[2] = this->Center[2]-this->ZLength/2.0;
			}
			break;
		case CHOP_MIN_Y_MIN_Z:
			// pt0 becomes the same as pt1
			// pt4 becomes the same as pt5
			save = x[2];
			if (j==0 && k==0)
			{
				x[2] = this->Center[2]+this->ZLength/2.0;
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}

        tc[0] = (x[2] + 0.5) * ( 1 - 2*i );
        newPoints->InsertNextPoint(x);
#ifdef SKIP_TCOORDS
        newTCoords->InsertNextTuple(tc);
#endif // SKIP_TCOORDS
#ifdef SKIP_NORMALS
        newNormals->InsertNextTuple(n);
#endif // SKIP_NORMALS

		switch (this->ChopType)
		{
		case CHOP_NONE:
			// do nothing
			break;
		//
		// Fixed-Z
		//
		case CHOP_MIN_X_MAX_Y:
			if (i==0 && j==1)
			{
				x[1] = save;
			}
			break;
		case CHOP_MAX_X_MAX_Y:
			if (i==1 && j==1)
			{
				x[1] = save;
			}
			break;
		case CHOP_MAX_X_MIN_Y:
			if (i==1 && j==0)
			{
				x[1] = save;
			}
			break;
		case CHOP_MIN_X_MIN_Y:
			if (i==0 && j==0)
			{
				x[1] = save;
			}
			break;
		//
		// Fixed-Y
		//
		case CHOP_MIN_X_MAX_Z:
			if (i==0 && k==1)
			{
				x[0] = save;
			}
			break;
		case CHOP_MAX_X_MAX_Z:
			if (i==1 && k==1)
			{
				x[0] = save;
			}
			break;
		case CHOP_MAX_X_MIN_Z:
			if (i==1 && k==0)
			{
				x[0] = save;
			}
			break;
		case CHOP_MIN_X_MIN_Z:
			if (i==0 && k==0)
			{
				x[0] = save;
			}
			break;
		//
		// Fixed-X
		//
		case CHOP_MAX_Y_MIN_Z:
			if (j==1 && k==0)
			{
				x[2] = save;
			}
			break;
		case CHOP_MAX_Y_MAX_Z:
			if (j==1 && k==1)
			{
				x[2] = save;
			}
			break;
		case CHOP_MIN_Y_MAX_Z:
			if (j==0 && k==1)
			{
				x[2] = save;
			}
			break;
		case CHOP_MIN_Y_MIN_Z:
			if (j==0 && k==0)
			{
				x[2] = save;
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
        }
      }
    }
  // purple
  pts[0] = 0; pts[1] = 1; pts[2] = 3; pts[3] = 2; 
  newPolys->InsertNextCell(4,pts);
  pts[0] = 4; pts[1] = 6; pts[2] = 7; pts[3] = 5; 
  newPolys->InsertNextCell(4,pts);

  for (x[1]=this->Center[1]-this->YLength/2.0, n[1]=(-1.0), n[0]=n[2]=0.0, i=0; 
  i<2; i++, x[1]+=this->YLength, n[1]+=2.0)
    {
    for (x[0]=this->Center[0]-this->XLength/2.0, j=0; j<2; 
    j++, x[0]+=this->XLength)
      {
      tc[0] = ( x[0] + 0.5 ) * ( 2*i - 1 );
      for (x[2]=this->Center[2]-this->ZLength/2.0, k=0; k<2; 
      k++, x[2]+=this->ZLength)
        {

		/*
			   12-------14
			  /|        /|
			 / |       / |
			13-+-----15  |
			|  8------+-10
			| /       | /
			|/        |/
			9--------11
		*/

		float save;
		switch (this->ChopType)
		{
		case CHOP_NONE:
			// do nothing
			break;
		//
		// Fixed-Z
		//
		case CHOP_MIN_X_MAX_Y:
			// pt12 becomes the same as pt14
			// pt13 becomes the same as pt15
			save = x[0];
			if (i==1 && j==0)
			{
				x[0] = this->Center[0]+this->XLength/2.0;
			}
			break;
		case CHOP_MAX_X_MAX_Y:
			// pt14 becomes the same as pt12
			// pt15 becomes the same as pt13
			save = x[0];
			if (i==1 && j==1)
			{
				x[0] = this->Center[0]-this->XLength/2.0;
			}
			break;
		case CHOP_MAX_X_MIN_Y:
			// pt10 becomes the same as pt8
			// pt11 becomes the same as pt9
			save = x[0];
			if(i==0 && j==1)
			{
				x[0] = this->Center[0]-this->XLength/2.0;
			}
			break;
		case CHOP_MIN_X_MIN_Y:
			// pt8 becomes the same as pt10
			// pt9 becomes the same as pt11
			save = x[0];
			if (i==0 && j==0)
			{
				x[0] = this->Center[0]+this->XLength/2.0;		
			}
			break;
		//
		// Fixed-Y
		//
		case CHOP_MIN_X_MAX_Z:
			// pt9 becomes the same as pt11
			// pt13 becomes the same as pt15
			save = x[0];
			if (j==0 && k==1)
			{
				x[0] = this->Center[0]+this->XLength/2.0;
			}
			break;
		case CHOP_MAX_X_MAX_Z:
			// pt11 becomes the same as pt9
			// pt15 becomes the same as pt13
			save = x[0];
			if (j==1 && k==1)
			{
				x[0] = this->Center[0]-this->XLength/2.0;
			}
			break;
		case CHOP_MAX_X_MIN_Z:
			// pt10 becomes the same as pt8
			// pt14 becomes the same as pt12
			save = x[0];
			if (j==1 && k==0)
			{
				x[0] = this->Center[0]-this->XLength/2.0;
			}
			break;
		case CHOP_MIN_X_MIN_Z:
			// pt8 becomes the same as pt10
			// pt12 becomes the same as pt14
			save = x[0];
			if (j==0 && k==0)
			{
				x[0] = this->Center[0]+this->XLength/2.0;
			}
			break;
		//
		// Fixed-X
		//
		case CHOP_MAX_Y_MIN_Z:
			// pt12 becomes the same as pt13
			// pt14 becomes the same as pt15
			save = x[2];
			if (i==1 && k==0)
			{
				x[2] = this->Center[2]+this->ZLength/2.0;
			}
			break;
		case CHOP_MAX_Y_MAX_Z:
			// pt13 becomes the same as pt12
			// pt15 becomes the same as pt14
			save = x[2];
			if (i==1 && k==1)
			{
				x[2] = this->Center[2]-this->ZLength/2.0;
			}
			break;
		case CHOP_MIN_Y_MAX_Z:
			// pt9 becomes the same as pt8
			// pt11 becomes the same as pt10
			save = x[2];
			if (i==0 && k==1)
			{
				x[2] = this->Center[2]-this->ZLength/2.0;
			}
			break;
		case CHOP_MIN_Y_MIN_Z:
			// pt8 becomes the same as pt9
			// pt10 becomes the same as pt11
			save = x[2];
			if (i==0 && k==0)
			{
				x[2] = this->Center[2]+this->ZLength/2.0;
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}

        tc[1] = ( x[2] + 0.5 ) * -1;
        newPoints->InsertNextPoint(x);
#ifdef SKIP_TCOORDS
        newTCoords->InsertNextTuple(tc);
#endif // SKIP_TCOORDS
#ifdef SKIP_NORMALS
        newNormals->InsertNextTuple(n);
#endif // SKIP_NORMALS

		switch (this->ChopType)
		{
		case CHOP_NONE:
			// do nothing
			break;
		//
		// Fixed-Z
		//
		case CHOP_MIN_X_MAX_Y:
			if (i==1 && j==0)
			{
				x[0] = save;
			}
			break;
		case CHOP_MAX_X_MAX_Y:
			if (i==1 && j==1)
			{
				x[0] = save;
			}
			break;
		case CHOP_MAX_X_MIN_Y:
			if(i==0 && j==1)
			{
				x[0] = save;
			}
			break;
		case CHOP_MIN_X_MIN_Y:
			if (i==0 && j==0)
			{
				x[0] = save;
			}
			break;
		//
		// Fixed-Y
		//
		case CHOP_MIN_X_MAX_Z:
			if (j==0 && k==1)
			{
				x[0] = save;
			}
			break;
		case CHOP_MAX_X_MAX_Z:
			if (j==1 && k==1)
			{
				x[0] = save;
			}
			break;
		case CHOP_MAX_X_MIN_Z:
			if (j==1 && k==0)
			{
				x[0] = save;
			}
			break;
		case CHOP_MIN_X_MIN_Z:
			if (j==0 && k==0)
			{
				x[0] = save;
			}
			break;
		//
		// Fixed-X
		//
		case CHOP_MAX_Y_MIN_Z:
			if (i==1 && k==0)
			{
				x[2] = save;
			}
			break;
		case CHOP_MAX_Y_MAX_Z:
			if (i==1 && k==1)
			{
				x[2] = save;
			}
			break;
		case CHOP_MIN_Y_MAX_Z:
			if (i==0 && k==1)
			{
				x[2] = save;
			}
			break;
		case CHOP_MIN_Y_MIN_Z:
			if (i==0 && k==0)
			{
				x[2] = save;
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
        }
      }
    }
  // blue
  pts[0] = 8; pts[1] = 10; pts[2] = 11; pts[3] = 9; 
  newPolys->InsertNextCell(4,pts);
  pts[0] = 12; pts[1] = 13; pts[2] = 15; pts[3] = 14; 
  newPolys->InsertNextCell(4,pts);

  for (x[2]=this->Center[2]-this->ZLength/2.0, n[2]=(-1.0), n[0]=n[1]=0.0, i=0; 
  i<2; i++, x[2]+=this->ZLength, n[2]+=2.0)
    {
    for (x[1]=this->Center[1]-this->YLength/2.0, j=0; j<2; 
    j++, x[1]+=this->YLength)
      {
      tc[1] = x[1] + 0.5;
      for (x[0]=this->Center[0]-this->XLength/2.0, k=0; k<2; 
      k++, x[0]+=this->XLength)
        {
		/*
			   18-------19
			  /|        /|
			 / |       / |
			22-+-----23  |
			|  16-----+-17
			| /       | /
			|/        |/
			20-------21
		*/
		// x=k y=j z=i 
		float save;
		switch (this->ChopType)
		{
		case CHOP_NONE:
			// do nothing
			break;
		//
		// Fixed-Z
		//
		case CHOP_MIN_X_MAX_Y:
			// pt18 becomes the same as pt19
			// pt22 becomes the same as pt23
			save = x[0];
			if (j==1 && k==0)
			{
				x[0] = this->Center[0]+this->XLength/2.0;
			}
			break;
		case CHOP_MAX_X_MAX_Y:
			// pt19 becomes the same as pt17
			// pt23 becomes the same as pt21
			save = x[1];
			if (j==1 && k==1)
			{
				x[1] = this->Center[1]-this->YLength/2.0;
			}
			break;
		case CHOP_MAX_X_MIN_Y:
			// pt17 becomes the same as pt19
			// pt21 becomes the same as pt23
			save = x[1];
			if(j==0 && k==1)
			{
				x[1] = this->Center[1]+this->YLength/2.0;
			}
			break;
		case CHOP_MIN_X_MIN_Y:
			// pt16 becomes the same as pt17
			// pt20 becomes the same as pt21
			save = x[0];
			if (j==0 && k==0)
			{
				x[0] = this->Center[0]+this->XLength/2.0;		
			}
			break;
		//
		// Fixed-Y
		//
		case CHOP_MIN_X_MAX_Z:
			// pt20 becomes the same as pt21
			// pt22 becomes the same as pt23
			save = x[0];
			if (i==1 && k==0)
			{
				x[0] = this->Center[0]+this->XLength/2.0;
			}
			break;
		case CHOP_MAX_X_MAX_Z:
			// pt21 becomes the same as pt20
			// pt23 becomes the same as pt22
			save = x[0];
			if (i==1 && k==1)
			{
				x[0] = this->Center[0]-this->XLength/2.0;
			}
			break;
		case CHOP_MAX_X_MIN_Z:
			// pt17 becomes the same as pt16
			// pt19 becomes the same as pt18
			save = x[0];
			if (i==0 && k==1)
			{
				x[0] = this->Center[0]-this->XLength/2.0;
			}
			break;
		case CHOP_MIN_X_MIN_Z:
			// pt16 becomes the same as pt17
			// pt18 becomes the same as pt19
			save = x[0];
			if (i==0 && k==0)
			{
				x[0] = this->Center[0]+this->XLength/2.0;
			}
			break;
		//
		// Fixed-X
		//
		case CHOP_MAX_Y_MIN_Z:
			// pt18 becomes the same as pt22
			// pt19 becomes the same as pt23
			save = x[2];
			if (i==0 && j==1)
			{
				x[2] = this->Center[2]+this->ZLength/2.0;
			}
			break;
		case CHOP_MAX_Y_MAX_Z:
			// pt22 becomes the same as pt18
			// pt23 becomes the same as pt19
			save = x[2];
			if (i==1 && j==1)
			{
				x[2] = this->Center[2]-this->ZLength/2.0;
			}
			break;
		case CHOP_MIN_Y_MAX_Z:
			// pt20 becomes the same as pt22
			// pt21 becomes the same as pt23
			save = x[1];
			if (i==1 && j==0)
			{
				x[1] = this->Center[1]+this->YLength/2.0;
			}
			break;
		case CHOP_MIN_Y_MIN_Z:
			// pt16 becomes the same as pt18
			// pt17 becomes the same as pt19
			save = x[1];
			if (i==0 && j==0)
			{
				x[1] = this->Center[1]+this->YLength/2.0;
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}

        tc[0] = ( x[0] + 0.5 ) * ( 2*i - 1 );
        newPoints->InsertNextPoint(x);
#ifdef SKIP_TCOORDS
        newTCoords->InsertNextTuple(tc);
#endif // SKIP_TCOORDS
#ifdef SKIP_NORMALS
        newNormals->InsertNextTuple(n);
#endif // SKIP_NORMALS

		switch (this->ChopType)
		{
		case CHOP_NONE:
			// do nothing
			break;
		//
		// Fixed-Z
		//
		case CHOP_MIN_X_MAX_Y:
			if (j==1 && k==0)
			{
				x[0] = save;
			}
			break;
		case CHOP_MAX_X_MAX_Y:
			if (j==1 && k==1)
			{
				x[1] = save;
			}
			break;
		case CHOP_MAX_X_MIN_Y:
			if(j==0 && k==1)
			{
				x[1] = save;
			}
			break;
		case CHOP_MIN_X_MIN_Y:
			if (j==0 && k==0)
			{
				x[0] = save;
			}
			break;
		//
		// Fixed-Y
		//
		case CHOP_MIN_X_MAX_Z:
			if (i==1 && k==0)
			{
				x[0] = save;
			}
			break;
		case CHOP_MAX_X_MAX_Z:
			if (i==1 && k==1)
			{
				x[0] = save;
			}
			break;
		case CHOP_MAX_X_MIN_Z:
			if (i==0 && k==1)
			{
				x[0] = save;
			}
			break;
		case CHOP_MIN_X_MIN_Z:
			if (i==0 && k==0)
			{
				x[0] = save;
			}
			break;
		//
		// Fixed-X
		//
		case CHOP_MAX_Y_MIN_Z:
			if (i==0 && j==1)
			{
				x[2] = save;
			}
			break;
		case CHOP_MAX_Y_MAX_Z:
			if (i==1 && j==1)
			{
				x[2] = save;
			}
			break;
		case CHOP_MIN_Y_MAX_Z:
			if (i==1 && j==0)
			{
				x[1] = save;
			}
			break;
		case CHOP_MIN_Y_MIN_Z:
			if (i==0 && j==0)
			{
				x[1] = save;
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
        }
      }
    }
  // orange
  pts[0] = 16; pts[1] = 18; pts[2] = 19; pts[3] = 17; 
  newPolys->InsertNextCell(4,pts);
  pts[0] = 20; pts[1] = 21; pts[2] = 23; pts[3] = 22; 
  newPolys->InsertNextCell(4,pts);
//
// Update ourselves and release memory
//
  output->SetPoints(newPoints);
  newPoints->Delete();

#ifdef SKIP_NORMALS
  output->GetPointData()->SetNormals(newNormals);
  newNormals->Delete();
#endif // SKIP_NORMALS

#ifdef SKIP_TCOORDS
  output->GetPointData()->SetTCoords(newTCoords);
  newTCoords->Delete();
#endif // SKIP_TCOORDS

  newPolys->Squeeze(); // since we've estimated size; reclaim some space
  output->SetPolys(newPolys);
  newPolys->Delete();
}

// Convenience method allows creation of cube by specifying bounding box.
void srcWedgeSource::SetBounds(float xMin, float xMax,
                              float yMin, float yMax,
                              float zMin, float zMax)
{
  float bounds[6];
  bounds[0] = xMin;
  bounds[1] = xMax;
  bounds[2] = yMin;
  bounds[3] = yMax;
  bounds[4] = zMin;
  bounds[5] = zMax;
  this->SetBounds (bounds);
}

void srcWedgeSource::SetBounds(float bounds[6])
{
  this->SetXLength(bounds[1]-bounds[0]);
  this->SetYLength(bounds[3]-bounds[2]);
  this->SetZLength(bounds[5]-bounds[4]);

  this->SetCenter((bounds[1]+bounds[0])/2.0, (bounds[3]+bounds[2])/2.0, 
                  (bounds[5]+bounds[4])/2.0);
}

void srcWedgeSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "X Length: " << this->XLength << "\n";
  os << indent << "Y Length: " << this->YLength << "\n";
  os << indent << "Z Length: " << this->ZLength << "\n";
  os << indent << "Center: (" << this->Center[0] << ", " 
               << this->Center[1] << ", " << this->Center[2] << ")\n";
  switch (this->ChopType)
  {
  case srcWedgeSource::CHOP_NONE:
	  os << indent << "CHOP_NONE" << "\n";
	  break;

  case srcWedgeSource::CHOP_MIN_X_MAX_Y:
	  os << indent << "CHOP_MIN_X_MAX_Y" << "\n";
	  break;
  case srcWedgeSource::CHOP_MAX_X_MAX_Y:
	  os << indent << "CHOP_MAX_X_MAX_Y" << "\n";
	  break;
  case srcWedgeSource::CHOP_MAX_X_MIN_Y:
	  os << indent << "CHOP_MAX_X_MIN_Y" << "\n";
	  break;
  case srcWedgeSource::CHOP_MIN_X_MIN_Y:
	  os << indent << "CHOP_MIN_X_MIN_Y" << "\n";
	  break;

  case srcWedgeSource::CHOP_MIN_X_MAX_Z:
	  os << indent << "CHOP_MIN_X_MAX_Z" << "\n";
	  break;
  case srcWedgeSource::CHOP_MAX_X_MAX_Z:
	  os << indent << "CHOP_MAX_X_MAX_Z" << "\n";
	  break;
  case srcWedgeSource::CHOP_MAX_X_MIN_Z:
	  os << indent << "CHOP_MAX_X_MIN_Z" << "\n";
	  break;
  case srcWedgeSource::CHOP_MIN_X_MIN_Z:
	  os << indent << "CHOP_MIN_X_MIN_Z" << "\n";
	  break;

  case srcWedgeSource::CHOP_MAX_Y_MIN_Z:
	  os << indent << "CHOP_MAX_Y_MIN_Z" << "\n";
	  break;
  case srcWedgeSource::CHOP_MAX_Y_MAX_Z:
	  os << indent << "CHOP_MAX_Y_MAX_Z" << "\n";
	  break;
  case srcWedgeSource::CHOP_MIN_Y_MAX_Z:
	  os << indent << "CHOP_MIN_Y_MAX_Z" << "\n";
	  break;
  case srcWedgeSource::CHOP_MIN_Y_MIN_Z:
	  os << indent << "CHOP_MIN_Y_MIN_Z" << "\n";
	  break;

  }
}

enum srcWedgeSource::ChopType srcWedgeSource::GetChopType()const
{
	return this->ChopType;
}

void srcWedgeSource::SetChopType(enum srcWedgeSource::ChopType t)
{
	if (this->ChopType != t)
	{
		this->ChopType = t;
		this->Modified();
	}
}
