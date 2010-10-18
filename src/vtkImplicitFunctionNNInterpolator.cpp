#include "StdAfx.h"
#include "vtkImplicitFunctionNNInterpolator.h"

#include "vtkCell.h"
#include "vtkDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
//{{
#include <vtkPoints.h>
#include <assert.h>
#include <sstream>
#include "srcinput/NNInterpolator/NNInterpolator.h"
//}}

vtkCxxRevisionMacro(vtkImplicitFunctionNNInterpolator, "$Revision: 1.21 $");
vtkStandardNewMacro(vtkImplicitFunctionNNInterpolator);
vtkCxxSetObjectMacro(vtkImplicitFunctionNNInterpolator,DataSet,vtkDataSet);

// Construct an vtkImplicitFunctionNNInterpolator with no initial dataset; the OutValue
// set to a large negative number; and the OutGradient set to (0,0,1).
vtkImplicitFunctionNNInterpolator::vtkImplicitFunctionNNInterpolator()
{
  this->DataSet = NULL;
  this->Interpolator = NULL;

  this->OutValue = -VTK_LARGE_FLOAT;

  this->OutGradient[0] = 0.0;
  this->OutGradient[1] = 0.0;
  this->OutGradient[2] = 1.0;

  this->Weights = NULL;
  this->Size = 0;
}

vtkImplicitFunctionNNInterpolator::~vtkImplicitFunctionNNInterpolator()
{
  this->SetDataSet(NULL);
  if ( this->Weights )
    {
    delete [] this->Weights;
    }
}

// Evaluate the implicit function. This returns the interpolated scalar value
// at x[3].
float vtkImplicitFunctionNNInterpolator::EvaluateFunction(float x[3])
{
  vtkDataArray *scalars;
  vtkCell *cell;
  vtkIdType id;
  int subId, i, numPts;
  float pcoords[3], s;

// COMMENT: {6/6/2008 4:25:38 PM}  //{{
// COMMENT: {6/6/2008 4:25:38 PM}  char buffer[250];
// COMMENT: {6/6/2008 4:25:38 PM}// COMMENT: {5/14/2008 10:19:11 PM}  sprintf(buffer, "Evaluating vtkImplicitFunctionNNInterpolator::EvaluateFunction(%g, %g, %g) = ", x[0], x[1], x[2]);
// COMMENT: {6/6/2008 4:25:38 PM}  sprintf(buffer, "%g,%g,%g,", x[0], x[1], x[2]);
// COMMENT: {6/6/2008 4:25:38 PM}  ::OutputDebugStringA(buffer);
// COMMENT: {6/6/2008 4:25:38 PM}  //}}

  //{{
  //::OutputDebugStringA("\n");
  //return x[2] - 0.2;
  //}}

// COMMENT: {5/5/2008 8:45:26 PM}  if ( this->DataSet->GetMaxCellSize() > this->Size )
// COMMENT: {5/5/2008 8:45:26 PM}    {
// COMMENT: {5/5/2008 8:45:26 PM}    if ( this->Weights )
// COMMENT: {5/5/2008 8:45:26 PM}      {
// COMMENT: {5/5/2008 8:45:26 PM}      delete [] this->Weights;
// COMMENT: {5/5/2008 8:45:26 PM}      }
// COMMENT: {5/5/2008 8:45:26 PM}    this->Weights = new float[this->DataSet->GetMaxCellSize()];
// COMMENT: {5/5/2008 8:45:26 PM}    this->Size = this->DataSet->GetMaxCellSize();
// COMMENT: {5/5/2008 8:45:26 PM}    }

// COMMENT: {5/5/2008 8:45:56 PM}  // See if a dataset has been specified
// COMMENT: {5/5/2008 8:45:56 PM}  if ( !this->DataSet /* || 
// COMMENT: {5/5/2008 8:45:56 PM}       !(scalars = this->DataSet->GetPointData()->GetScalars()) */ )
// COMMENT: {5/5/2008 8:45:56 PM}    {
// COMMENT: {5/5/2008 8:45:56 PM}    vtkErrorMacro(<<"Can't evaluate dataset!");
// COMMENT: {5/5/2008 8:45:56 PM}    //{{
// COMMENT: {5/5/2008 8:45:56 PM}    sprintf(buffer, "%g\n", this->OutValue);
// COMMENT: {5/5/2008 8:45:56 PM}    ::OutputDebugStringA(buffer);
// COMMENT: {5/5/2008 8:45:56 PM}    //}}
// COMMENT: {5/5/2008 8:45:56 PM}    return this->OutValue;
// COMMENT: {5/5/2008 8:45:56 PM}    }

// COMMENT: {5/5/2008 8:46:17 PM}  vtkIdType cellId;
// COMMENT: {5/5/2008 8:46:17 PM}  vtkIdType numCells = this->DataSet->GetNumberOfCells();
// COMMENT: {5/5/2008 8:46:17 PM}  float p1[3];
// COMMENT: {5/5/2008 8:46:17 PM}  float p2[3];
// COMMENT: {5/5/2008 8:46:17 PM}  float t;
// COMMENT: {5/5/2008 8:46:17 PM}  float g[3];
// COMMENT: {5/5/2008 8:46:17 PM}
// COMMENT: {5/5/2008 8:46:17 PM}  p1[0] = x[0]; p1[1] = x[1]; p1[2] = 100;
// COMMENT: {5/5/2008 8:46:17 PM}  p2[0] = x[0]; p2[1] = x[1]; p2[2] = -100;

  // See if a dataset has been specified
  if ( !this->Interpolator )
    {
    vtkErrorMacro(<<"No Interpolator set!");
// COMMENT: {5/7/2008 5:57:20 PM}    //{{
// COMMENT: {5/7/2008 5:57:20 PM}    sprintf(buffer, "%g\n", this->OutValue);
// COMMENT: {5/7/2008 5:57:20 PM}    ::OutputDebugStringA(buffer);
// COMMENT: {5/7/2008 5:57:20 PM}    //}}
    return this->OutValue;
    }

  Point pt(x[0], x[1], x[2], x[2]);
  float val = this->Interpolator->interpolate(pt);
  //return (val - x[2]);

// COMMENT: {6/6/2008 4:26:03 PM}  sprintf(buffer, "%g\n", x[2] - val);
// COMMENT: {6/6/2008 4:26:03 PM}  ::OutputDebugStringA(buffer);
  return (x[2] - val);

// COMMENT: {5/5/2008 8:28:40 PM}  for (cellId = 0; cellId < numCells; ++cellId)
// COMMENT: {5/5/2008 8:28:40 PM}  {
// COMMENT: {5/5/2008 8:28:40 PM}    cell = this->DataSet->GetCell(cellId);
// COMMENT: {5/5/2008 8:28:40 PM}	assert(cell->GetNumberOfEdges() == 3);
// COMMENT: {5/5/2008 8:28:40 PM}	assert(cell->GetNumberOfFaces() == 0);
// COMMENT: {5/5/2008 8:28:40 PM}	assert(cell->GetNumberOfPoints() == 3);
// COMMENT: {5/5/2008 8:28:40 PM}	//{{
// COMMENT: {5/5/2008 8:28:40 PM}	std::ostringstream oss;
// COMMENT: {5/5/2008 8:28:40 PM}	cell->PrintSelf(oss, vtkIndent(1));
// COMMENT: {5/5/2008 8:28:40 PM}	::OutputDebugStringA(oss.str().c_str());
// COMMENT: {5/5/2008 8:28:40 PM}	std::ostringstream oss2;
// COMMENT: {5/5/2008 8:28:40 PM}	cell->GetPoints()->PrintSelf(oss2, vtkIndent(2));
// COMMENT: {5/5/2008 8:28:40 PM}	::OutputDebugStringA(oss2.str().c_str());
// COMMENT: {5/5/2008 8:28:40 PM}	//}}
// COMMENT: {5/5/2008 8:28:40 PM}	if (cell->IntersectWithLine(p1, p2, 1e-6, t, g, pcoords, subId))
// COMMENT: {5/5/2008 8:28:40 PM}	{
// COMMENT: {5/5/2008 8:28:40 PM}		break;
// COMMENT: {5/5/2008 8:28:40 PM}	}
// COMMENT: {5/5/2008 8:28:40 PM}  }
// COMMENT: {5/5/2008 8:28:40 PM}  if (cellId < numCells)
// COMMENT: {5/5/2008 8:28:40 PM}  {
// COMMENT: {5/5/2008 8:28:40 PM}	  // cell found
// COMMENT: {5/5/2008 8:28:40 PM}  }

// COMMENT: {5/5/2008 3:28:49 PM}  // Find the cell that contains xyz and get it
// COMMENT: {5/5/2008 3:28:49 PM}  cell = this->DataSet->FindAndGetCell(x,NULL,-1,0.0,subId,pcoords,this->Weights);

// COMMENT: {5/5/2008 3:28:43 PM}  if (cell)
// COMMENT: {5/5/2008 3:28:43 PM}    { // Interpolate the point data
// COMMENT: {5/5/2008 3:28:43 PM}    numPts = cell->GetNumberOfPoints();
// COMMENT: {5/5/2008 3:28:43 PM}    for (s=0.0, i=0; i < numPts; i++)
// COMMENT: {5/5/2008 3:28:43 PM}      {
// COMMENT: {5/5/2008 3:28:43 PM}      id = cell->PointIds->GetId(i);
// COMMENT: {5/5/2008 3:28:43 PM}      s += scalars->GetComponent(id,0) * this->Weights[i];
// COMMENT: {5/5/2008 3:28:43 PM}      }
// COMMENT: {5/5/2008 3:28:43 PM}    //{{
// COMMENT: {5/5/2008 3:28:43 PM}    sprintf(buffer, "%g\n", s);
// COMMENT: {5/5/2008 3:28:43 PM}    ::OutputDebugStringA(buffer);
// COMMENT: {5/5/2008 3:28:43 PM}    //}}
// COMMENT: {5/5/2008 3:28:43 PM}    return s;
// COMMENT: {5/5/2008 3:28:43 PM}    }
// COMMENT: {5/5/2008 3:28:43 PM}  else
// COMMENT: {5/5/2008 3:28:43 PM}    { // use outside value
// COMMENT: {5/5/2008 3:28:43 PM}    //{{
// COMMENT: {5/5/2008 3:28:43 PM}	id = this->DataSet->FindPoint(x);
// COMMENT: {5/5/2008 3:28:43 PM}	if (id >= 0)
// COMMENT: {5/5/2008 3:28:43 PM}	{
// COMMENT: {5/5/2008 3:28:43 PM}		float *pt = this->DataSet->GetPoint(id);
// COMMENT: {5/5/2008 3:28:43 PM}		sprintf(buffer, "%g\n", x[2]-pt[2]);
// COMMENT: {5/5/2008 3:28:43 PM}		::OutputDebugStringA(buffer);
// COMMENT: {5/5/2008 3:28:43 PM}		return x[2]-pt[2];
// COMMENT: {5/5/2008 3:28:43 PM}	}
// COMMENT: {5/5/2008 3:28:43 PM}	sprintf(buffer, "%g\n", this->OutValue);
// COMMENT: {5/5/2008 3:28:43 PM}	::OutputDebugStringA(buffer);
// COMMENT: {5/5/2008 3:28:43 PM}    //}}
// COMMENT: {5/5/2008 3:28:43 PM}    return this->OutValue;
// COMMENT: {5/5/2008 3:28:43 PM}    }
}

unsigned long vtkImplicitFunctionNNInterpolator::GetMTime()
{
  unsigned long mTime=this->vtkImplicitFunction::GetMTime();
  unsigned long DataSetMTime;

  if ( this->DataSet != NULL )
    {
    this->DataSet->Update ();
    DataSetMTime = this->DataSet->GetMTime();
    mTime = ( DataSetMTime > mTime ? DataSetMTime : mTime );
    }

  return mTime;
}


// Evaluate implicit function gradient.
void vtkImplicitFunctionNNInterpolator::EvaluateGradient(float x[3], float n[3])
{
	x[0] = 1;
	x[1] = 1;
	x[2] = 1;

// COMMENT: {5/12/2008 2:32:34 PM}  vtkDataArray *scalars;
// COMMENT: {5/12/2008 2:32:34 PM}  vtkCell *cell;
// COMMENT: {5/12/2008 2:32:34 PM}  vtkIdType id;
// COMMENT: {5/12/2008 2:32:34 PM}  int subId, i, numPts;
// COMMENT: {5/12/2008 2:32:34 PM}  float pcoords[3];
// COMMENT: {5/12/2008 2:32:34 PM}
// COMMENT: {5/12/2008 2:32:34 PM}  if ( this->DataSet->GetMaxCellSize() > this->Size )
// COMMENT: {5/12/2008 2:32:34 PM}    {
// COMMENT: {5/12/2008 2:32:34 PM}    if ( this->Weights )
// COMMENT: {5/12/2008 2:32:34 PM}      {
// COMMENT: {5/12/2008 2:32:34 PM}      delete [] this->Weights;
// COMMENT: {5/12/2008 2:32:34 PM}      }
// COMMENT: {5/12/2008 2:32:34 PM}    this->Weights = new float[this->DataSet->GetMaxCellSize()];
// COMMENT: {5/12/2008 2:32:34 PM}    this->Size = this->DataSet->GetMaxCellSize();
// COMMENT: {5/12/2008 2:32:34 PM}    }
// COMMENT: {5/12/2008 2:32:34 PM}
// COMMENT: {5/12/2008 2:32:34 PM}  // See if a dataset has been specified
// COMMENT: {5/12/2008 2:32:34 PM}  if ( !this->DataSet || 
// COMMENT: {5/12/2008 2:32:34 PM}       !(scalars = this->DataSet->GetPointData()->GetScalars()) )
// COMMENT: {5/12/2008 2:32:34 PM}    {
// COMMENT: {5/12/2008 2:32:34 PM}    vtkErrorMacro(<<"Can't evaluate gradient!");
// COMMENT: {5/12/2008 2:32:34 PM}    for ( i=0; i < 3; i++ )
// COMMENT: {5/12/2008 2:32:34 PM}      {
// COMMENT: {5/12/2008 2:32:34 PM}      n[i] = this->OutGradient[i];
// COMMENT: {5/12/2008 2:32:34 PM}      }
// COMMENT: {5/12/2008 2:32:34 PM}    return;
// COMMENT: {5/12/2008 2:32:34 PM}    }
// COMMENT: {5/12/2008 2:32:34 PM}
// COMMENT: {5/12/2008 2:32:34 PM}  // Find the cell that contains xyz and get it
// COMMENT: {5/12/2008 2:32:34 PM}  cell = this->DataSet->FindAndGetCell(x,NULL,-1,0.0,subId,pcoords,this->Weights);
// COMMENT: {5/12/2008 2:32:34 PM}
// COMMENT: {5/12/2008 2:32:34 PM}  if (cell)
// COMMENT: {5/12/2008 2:32:34 PM}    { // Interpolate the point data
// COMMENT: {5/12/2008 2:32:34 PM}    numPts = cell->GetNumberOfPoints();
// COMMENT: {5/12/2008 2:32:34 PM}
// COMMENT: {5/12/2008 2:32:34 PM}    for ( i=0; i < numPts; i++ ) //Weights used to hold scalar values
// COMMENT: {5/12/2008 2:32:34 PM}      {
// COMMENT: {5/12/2008 2:32:34 PM}      id = cell->PointIds->GetId(i);
// COMMENT: {5/12/2008 2:32:34 PM}      this->Weights[i] = scalars->GetComponent(id,0);
// COMMENT: {5/12/2008 2:32:34 PM}      }
// COMMENT: {5/12/2008 2:32:34 PM}    cell->Derivatives(subId, pcoords, this->Weights, 1, n);
// COMMENT: {5/12/2008 2:32:34 PM}    }
// COMMENT: {5/12/2008 2:32:34 PM}
// COMMENT: {5/12/2008 2:32:34 PM}  else
// COMMENT: {5/12/2008 2:32:34 PM}    { // use outside value
// COMMENT: {5/12/2008 2:32:34 PM}    for ( i=0; i < 3; i++ )
// COMMENT: {5/12/2008 2:32:34 PM}      {
// COMMENT: {5/12/2008 2:32:34 PM}      n[i] = this->OutGradient[i];
// COMMENT: {5/12/2008 2:32:34 PM}      }
// COMMENT: {5/12/2008 2:32:34 PM}    }
}

void vtkImplicitFunctionNNInterpolator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Out Value: " << this->OutValue << "\n";
  os << indent << "Out Gradient: (" << this->OutGradient[0] << ", " 
     << this->OutGradient[1] << ", " << this->OutGradient[2] << ")\n";

  if ( this->DataSet )
    {
    os << indent << "Data Set: " << this->DataSet << "\n";
    }
  else
    {
    os << indent << "Data Set: (none)\n";
    }
}
