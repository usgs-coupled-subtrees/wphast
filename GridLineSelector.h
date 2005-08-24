#pragma once
//#include <vtkInteractorObserver.h>
#include <vtk3DWidget.h>
#include <vtkBoxWidget.h>

class CGridActor;

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CGridLineSelector : public vtk3DWidget
{
public:
	// Description:
	// Instantiate the object.
	static CGridLineSelector *New();

	vtkTypeRevisionMacro(CGridLineSelector, vtk3DWidget);
	//void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Methods that satisfy the superclass' API.
	virtual void SetEnabled(int);
	virtual void PlaceWidget(vtkFloatingPointType bounds[6]);
	void PlaceWidget()
	{this->Superclass::PlaceWidget();}
	void PlaceWidget(vtkFloatingPointType xmin, vtkFloatingPointType xmax, vtkFloatingPointType ymin, vtkFloatingPointType ymax, 
		vtkFloatingPointType zmin, vtkFloatingPointType zmax)
	{this->Superclass::PlaceWidget(xmin,xmax,ymin,ymax,zmin,zmax);}

	//
	vtkSetMacro(GridActor,CGridActor*);
	vtkGetMacro(GridActor,CGridActor*);

protected:
	CGridLineSelector(void);
	//virtual ~CGridLineSelector(void);
	~CGridLineSelector(void);

	int State;
	enum WidgetState
	{
		Start=0,
		Selecting,
	};

	//
	CGridActor  *GridActor;

	// element selection
	vtkCubeSource     *Source;
	vtkPolyDataMapper *Mapper;
	vtkActor          *Actor;

	// wireframe outline
	vtkActor             *OutlineActor;
	vtkPolyDataMapper    *OutlineMapper;
	vtkOutlineSource     *OutlineSource;
	vtkFloatingPointType  StartPoint[3];

	// Handles the events
	static void ProcessEvents(vtkObject* object, 
		unsigned long event,
		void* clientdata, 
		void* calldata);

	// ProcessEvents() dispatches to these methods.
	void OnMouseMove();
	void OnLeftButtonDown();
	void OnLeftButtonUp();
// COMMENT: {8/22/2005 3:46:36 PM}	void OnMiddleButtonDown();
// COMMENT: {8/22/2005 3:46:36 PM}	void OnMiddleButtonUp();
// COMMENT: {8/22/2005 3:46:36 PM}	void OnRightButtonDown();
// COMMENT: {8/22/2005 3:46:36 PM}	void OnRightButtonUp();

private:
	CGridLineSelector(const CGridLineSelector&);  // Not implemented.
	void operator=(const CGridLineSelector&);     // Not implemented.

};
