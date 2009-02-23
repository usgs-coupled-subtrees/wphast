#pragma once

#include <vtk3DWidget.h>

#include <list>
#include <vector>

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class vtkProperty;
class CZoneActor;

class CPrismWidget : public vtk3DWidget
{
public:
	// Description:
	// Instantiate the object.
	static CPrismWidget *New();

	vtkTypeRevisionMacro(CPrismWidget, vtk3DWidget);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Methods that satisfy the superclass' API.
	virtual void SetEnabled(int);
	virtual void PlaceWidget(vtkFloatingPointType bounds[6]);
	virtual void SetProp3D(vtkProp3D*);

	int State;
	enum WidgetState
	{
		Start=0,
		Moving,
		Outside,
		Deleting,
	};

	enum EventIds
	{
		InsertPointEvent = vtkCommand::UserEvent + 700,
		DeletePointEvent,
	};

	vtkPoints* GetPoints(void);

	vtkPoints* GetInsertPoints(void);
	vtkPoints* GetDeletePoints(void);

	double* GetPickPoint(void) {return this->PickPoint;};

#ifdef WIN32
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
#endif

protected:
	CPrismWidget(void);
	~CPrismWidget(void);

	// Handles the events
	static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* callData);

	// ProcessEvents() dispatches to these methods.
	void OnMouseMove(void);
	void OnLeftButtonDown(void);
	void OnLeftButtonUp(void);
// COMMENT: {8/25/2008 9:52:36 PM}	void OnMiddleButtonDown(void);
// COMMENT: {8/25/2008 9:52:36 PM}	void OnMiddleButtonUp(void);
	void OnRightButtonDown(void);
	void OnRightButtonUp(void);

	virtual void SizeHandles(void);

	// misc events
	void OnModifiedEvent(vtkObject* caller, void* callData);

protected:
	CZoneActor *ZoneActor;

	vtkProperty *HandleProperty;
	vtkProperty *SelectedProperty;
	vtkProperty *SelectedHandleProperty;

	// balls
	std::vector<vtkSphereSource*>   SphereSources;
	std::vector<vtkPolyDataMapper*> PolyDataMappers;
	std::vector<vtkActor*>          Actors;

	//
	vtkActor *CurrentHandle[2];
	vtkIdType CurrentId[2];

	//
	double ScaledZPlane;
	double WorldSIPoint[4];
	double PickPoint[3];

	//
	vtkCellPicker *HandlePicker;
	vtkCellPicker *LinePicker;

	// ghost point
	vtkSphereSource*   GhostSphereSource;
	vtkPolyDataMapper* GhostPolyDataMapper;
	vtkActor*          GhostActor;

	// lines
	std::vector<vtkLineSource*>      LineSources;
	std::vector<vtkTubeFilter*>      TubeFilters;
	std::vector<vtkPolyDataMapper*>  LineMappers;
	std::vector<vtkActor*>           LineActors;
	size_t                           LineIndex;
	int                              TubeVisibility;
	int                              HandleHover;


	// transforms
	vtkTransform *TransformScale;

	void PositionHandles(void);
	void FreeHandles(void);
	void CreateHandles(void);
	void AddHandleActors(void);
	void RemoveHandleActors(void);

private:
	CPrismWidget(const CPrismWidget&);  // Not implemented.
	void operator=(const CPrismWidget&);   // Not implemented.

};
