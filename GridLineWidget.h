#pragma once
#include <vtkImplicitPlaneWidget.h>

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CGridLineWidget : public vtkImplicitPlaneWidget
{
public:
  // Description:
  // Instantiate the object.
  static CGridLineWidget *New();

  vtkTypeRevisionMacro(CGridLineWidget,vtkImplicitPlaneWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void SetEnabled(int);

  void SetInitialPickPosition(vtkFloatingPointType pos[3]);

protected:
	CGridLineWidget(void);
	~CGridLineWidget(void);

	//handles the events
	static void ProcessEvents(vtkObject* object, unsigned long event,
							void* clientdata, void* calldata);

	// ProcessEvents() dispatches to these methods.
	void OnLeftButtonDown();
	void OnLeftButtonUp();
	//void OnMiddleButtonDown();
	//void OnMiddleButtonUp();
	//void OnRightButtonDown();
	//void OnRightButtonUp();
	//void OnMouseMove();

	vtkFloatingPointType InitialPickPosition[3];

private:
  CGridLineWidget(const CGridLineWidget&);  //Not implemented
  void operator=(const CGridLineWidget&);  //Not implemented
};
