#pragma once

#include "vtkImplicitFunction.h"

class vtkDataSet;
class NNInterpolator;

class vtkImplicitFunctionNNInterpolator : public vtkImplicitFunction
{
public:
  vtkTypeRevisionMacro(vtkImplicitFunctionNNInterpolator,vtkImplicitFunction);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description
  // Construct an vtkImplicitDataSet with no initial dataset; the OutValue
  // set to a large negative number; and the OutGradient set to (0,0,1).
  static vtkImplicitFunctionNNInterpolator *New();
  // Description:
  // Return the MTime also considering the DataSet dependency.
  unsigned long GetMTime();

  // Description
  // Evaluate the implicit function. This returns the interpolated scalar value
  // at x[3].
  float EvaluateFunction(float x[3]);
  float EvaluateFunction(float x, float y, float z)
    {return this->vtkImplicitFunction::EvaluateFunction(x, y, z); } ;

  // Description
  // Evaluate implicit function gradient.
  void EvaluateGradient(float x[3], float n[3]);

  // Description:
  // Set / get the dataset used for the implicit function evaluation.
  virtual void SetDataSet(vtkDataSet*);
  vtkGetObjectMacro(DataSet,vtkDataSet);

  // Description:
  // Set / get the function value to use for points outside of the dataset.
  vtkSetMacro(OutValue,float);
  vtkGetMacro(OutValue,float);

  // Description:
  // Set / get the function gradient to use for points outside of the dataset.
  vtkSetVector3Macro(OutGradient,float);
  vtkGetVector3Macro(OutGradient,float);

  vtkSetMacro(Interpolator,NNInterpolator*);
  vtkGetMacro(Interpolator,NNInterpolator*);

protected:
  vtkImplicitFunctionNNInterpolator();
  ~vtkImplicitFunctionNNInterpolator();

  vtkDataSet *DataSet;
  float OutValue;
  float OutGradient[3];

  NNInterpolator* Interpolator;

  float *Weights; //used to compute interpolation weights
  int Size; //keeps track of length of weights array

private:
  vtkImplicitFunctionNNInterpolator(const vtkImplicitFunctionNNInterpolator&);  // Not implemented.
  void operator=(const vtkImplicitFunctionNNInterpolator&);  // Not implemented.
};
