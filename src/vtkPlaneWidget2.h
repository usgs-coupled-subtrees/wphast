#pragma once

#include <vtkPlaneWidget.h>

class vtkArrowSource;

class vtkPlaneWidget2 : public vtkPlaneWidget  
{

public:
	vtkTypeRevisionMacro(vtkPlaneWidget2, vtkPlaneWidget);
	static vtkPlaneWidget2 *New();

	double GetDeltaX(void);
	void SetDeltaX(double dx);

	double GetDeltaY(void);
	void SetDeltaY(double dy);

	double GetAngle(void); // degrees
	void SetAngle(double angle); // degrees

	double GetRadians(void);
	void SetRadians(double radians);
	
	double* GetModelOrigin(void); // degrees
	void SetModelOrigin(float x, float y);

	void SetResolution(int index, int n);
	int GetResolution(int index)const;

	int GetXResolution(void)const;
	void SetXResolution(int nx);

	int GetYResolution(void)const;
	void SetYResolution(int ny);

	// states
	enum WidgetState2
	{
		Start    = vtkPlaneWidget::Start,
		Moving   = vtkPlaneWidget::Moving,
		Scaling  = vtkPlaneWidget::Scaling,
		Pushing  = vtkPlaneWidget::Pushing,
		Rotating = vtkPlaneWidget::Rotating,
		Outside  = vtkPlaneWidget::Outside,
		// additional states
		Spinning = 200
	};
	int GetState(void)const { return State; }

	// sub-states
	enum WidgetSubState
	{
		MovingMoveOrigin,
		MovingMovePoint1,
		MovingMovePoint2,
		MovingMovePoint3,
		MovingTranslate
	} Substate;
	int GetSubstate(void)const { return Substate; }


	// overrides
	virtual void SetEnabled(int);
	virtual void SizeHandles(void);

	virtual void PlaceWidget(void);

protected:
	vtkPlaneWidget2(void);
	virtual ~vtkPlaneWidget2(void);

	//handles the events
	static void ProcessEvents(vtkObject* vtkNotUsed(object), 
							unsigned long event,
							void* clientdata, 
							void* vtkNotUsed(calldata));

	// ProcessEvents() dispatches to these methods.
	void OnCtrlLeftButtonDown(void);
	void OnMouseMove(void);

	// Methods to manipulate the plane
	void Spin(void);

	// axes
	vtkArrowSource    *AxisSource;
	vtkPolyDataMapper *AxisMapper;
	vtkActor          *XAxisActor;
	vtkActor          *YAxisActor;
	double            Angle;
	double            GridOrigin[3];
	double            Deltas[3];
	double            m_InvisablePosX[3];

// COMMENT: {4/18/2006 11:11:02 PM}	// axes triangle
// COMMENT: {4/18/2006 11:11:02 PM}	vtkPoints         *TrianglePoints;
// COMMENT: {4/18/2006 11:11:02 PM}	vtkCellArray      *TriangleCellArray;
// COMMENT: {4/18/2006 11:11:02 PM}	vtkPolyData       *TrianglePolyData;
// COMMENT: {4/18/2006 11:11:02 PM}	vtkPolyDataMapper *TriangleMapper;
// COMMENT: {4/18/2006 11:11:02 PM}	vtkActor          *TriangleActor;

#if defined(_DEBUG)
	vtkActor          *m_VisHandle;
	vtkPolyDataMapper *m_VisHandleMapper;
	vtkSphereSource   *m_VisHandleGeometry;
#endif

private:
	vtkPlaneWidget2(const vtkPlaneWidget2&);  // Not implemented.
	void operator=(const vtkPlaneWidget2&);  // Not implemented.

};
