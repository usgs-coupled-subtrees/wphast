#pragma once

#include <vtkPolyDataSource.h>

class srcWedgeSource : public vtkPolyDataSource 
{
public:
  static srcWedgeSource *New();
  vtkTypeRevisionMacro(srcWedgeSource,vtkPolyDataSource);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the length of the cube in the x-direction.
  vtkSetClampMacro(XLength,float,0.0,VTK_LARGE_FLOAT);
  vtkGetMacro(XLength,float);

  // Description:
  // Set the length of the cube in the y-direction.
  vtkSetClampMacro(YLength,float,0.0,VTK_LARGE_FLOAT);
  vtkGetMacro(YLength,float);

  // Description:
  // Set the length of the cube in the z-direction.
  vtkSetClampMacro(ZLength,float,0.0,VTK_LARGE_FLOAT);
  vtkGetMacro(ZLength,float);

  // Description:
  // Set the center of the cube.
  vtkSetVector3Macro(Center,float);
  vtkGetVectorMacro(Center,float,3);

  // Description:
  // Convenience method allows creation of cube by specifying bounding box.
  void SetBounds(float xMin, float xMax,
                 float yMin, float yMax,
                 float zMin, float zMax);
  void SetBounds(float bounds[6]);

  //
  //
  enum ChopType {
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

  enum ChopType GetChopType()const;
  void SetChopType(enum ChopType t);

protected:
  srcWedgeSource(float xL=1.0, float yL=1.0, float zL=1.0);
  ~srcWedgeSource() {};

  void Execute();
  float XLength;
  float YLength;
  float ZLength;
  float Center[3];
  

// COMMENT: {3/5/2008 9:12:34 PM}#define CHOP_MAX_X_MAX_Y
// COMMENT: {3/5/2008 9:12:34 PM}#define CHOP_MAX_X_MIN_Y
// COMMENT: {3/5/2008 9:12:34 PM}#define CHOP_MIN_X_MIN_Y
// COMMENT: {3/5/2008 9:12:34 PM}
// COMMENT: {3/5/2008 9:12:34 PM}#define CHOP_MIN_X_MAX_Z
// COMMENT: {3/5/2008 9:12:34 PM}#define CHOP_MAX_X_MAX_Z
// COMMENT: {3/5/2008 9:12:34 PM}#define CHOP_MAX_X_MIN_Z
// COMMENT: {3/5/2008 9:12:34 PM}#define CHOP_MIN_X_MIN_Z
// COMMENT: {3/5/2008 9:12:34 PM}
// COMMENT: {3/5/2008 9:12:34 PM}#define CHOP_MAX_Y_MIN_Z
// COMMENT: {3/5/2008 9:12:34 PM}#define CHOP_MAX_Y_MAX_Z
// COMMENT: {3/5/2008 9:12:34 PM}#define CHOP_MIN_Y_MAX_Z
// COMMENT: {3/5/2008 9:12:34 PM}#define CHOP_MIN_Y_MIN_Z

private:
  srcWedgeSource(const srcWedgeSource&);  // Not implemented.
  void operator=(const srcWedgeSource&);  // Not implemented.
};


