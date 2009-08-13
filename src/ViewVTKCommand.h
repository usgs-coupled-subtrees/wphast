#pragma once
#include <vtkCommand.h>

class CWPhastView;
class CAction;

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CViewVTKCommand : public vtkCommand
{
public:
	static CViewVTKCommand *New(CWPhastView *pView);

	virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData);
	void Update();
	void Update2();

protected:
	// interactor events
	void OnStartInteractionEvent(vtkObject* caller, void* callData);
	void OnEndInteractionEvent(vtkObject* caller, void* callData);
	void OnInteractionEvent(vtkObject* caller, void* callData);

	// pick events
	void OnEndPickEvent(vtkObject* caller, void* callData);

	// mouse events
	void OnLeftButtonReleaseEvent(vtkObject* caller, void* callData);
	void OnLeftButtonPressEvent(vtkObject* caller, void* callData);
	void OnMouseMoveEvent(vtkObject* caller, void* callData);

	// keyboard events
	void OnKeyPressEvent(vtkObject* caller, void* callData);

	// misc events
	void OnModifiedEvent(vtkObject* caller, void* callData);

	//
	void ComputeDisplayToWorld(double x, double y, double z, double worldPt[4]);
	void ComputeWorldToDisplay(double x, double y, double z, double displayPt[4]);

protected:
	CViewVTKCommand(CWPhastView* pView);
	virtual ~CViewVTKCommand();

	CWPhastView* m_pView;
	double m_WorldPointXYPlane[4];
	double m_WorldPointMinZPlane[4];
	double m_BeginPoint[4];

	CAction* m_pAction;

	vtkFloatingPointType FixedPlanePoint[3];
	int                  FixedPlane;  // [0-5]
	int                  FixedCoord;  // [0-2]
};
