#pragma once

#include <vtkPolyDataSource.h>
#include "srcinput/wedge.h"

class srcWedgeSource : public vtkPolyDataSource 
{
public:
  static srcWedgeSource *New();
  vtkTypeRevisionMacro(srcWedgeSource,vtkPolyDataSource);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the length of the cube in the x-direction.
  vtkSetClampMacro(XLength,double,0.0,VTK_DOUBLE_MAX);
  vtkGetMacro(XLength,double);

  // Description:
  // Set the length of the cube in the y-direction.
  vtkSetClampMacro(YLength,double,0.0,VTK_DOUBLE_MAX);
  vtkGetMacro(YLength,double);

  // Description:
  // Set the length of the cube in the z-direction.
  vtkSetClampMacro(ZLength,double,0.0,VTK_DOUBLE_MAX);
  vtkGetMacro(ZLength,double);

  // Description:
  // Set the center of the cube.
  vtkSetVector3Macro(Center,double);
  vtkGetVectorMacro(Center,double,3);

  // Description:
  // Convenience method allows creation of cube by specifying bounding box.
  void SetBounds(double xMin, double xMax,
                 double yMin, double yMax,
                 double zMin, double zMax);
  void SetBounds(double bounds[6]);

  //
  //
  typedef enum tagChopType {
	  CHOP_NONE,
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
  } ChopType;

  /*
	typedef enum tagWedgeOrientation {
		X1,
		X2,
		X3,
		X4,
		Y1,
		Y2,
		Y3,
		Y4,
		Z1,
		Z2,
		Z3,
		Z4,
		WEDGE_ERROR,
	} WedgeOrientation;
	*/


  ChopType GetChopType()const;
  void SetChopType(ChopType t);

  static Wedge::WEDGE_ORIENTATION ConvertChopType(srcWedgeSource::ChopType t);
  static ChopType ConvertWedgeOrientation(Wedge::WEDGE_ORIENTATION o);
  static std::string GetWedgeOrientationString(Wedge::WEDGE_ORIENTATION o);
  static std::string GetWedgeOrientationString(srcWedgeSource::ChopType ct);


protected:
  srcWedgeSource(double xL=1.0, double yL=1.0, double zL=1.0, srcWedgeSource::ChopType n = srcWedgeSource::CHOP_NONE);

  ~srcWedgeSource() {};

  void Execute();
  double XLength;
  double YLength;
  double ZLength;
  double Center[3];

  ChopType chopType;

private:
  srcWedgeSource(const srcWedgeSource&);  // Not implemented.
  void operator=(const srcWedgeSource&);  // Not implemented.
};


