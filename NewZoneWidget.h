#pragma once

#include <vtk3DWidget.h>

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class CGridActor;
class vtkCursor3D;
class vtkPolyDataMapper;
class vtkActor;

class CNewZoneWidget : public vtk3DWidget
{
public:
	// Description:
	// Instantiate the object.
	static CNewZoneWidget *New();

	vtkTypeRevisionMacro(CNewZoneWidget, vtk3DWidget);

	// Description:
	// Methods that satisfy the superclass' API.
	virtual void SetEnabled(int);
	virtual void PlaceWidget(vtkFloatingPointType bounds[6]);

	vtkFloatingPointType* GetBounds(void) { return this->OutlineActor->GetBounds(); }
	void GetBounds(vtkFloatingPointType bounds[6]) { this->OutlineActor->GetBounds(bounds); }

// COMMENT: {5/10/2006 4:23:30 PM}	vtkSetMacro(GridActor,CGridActor*);

#ifdef WIN32
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
#endif

protected:
	CNewZoneWidget(void);
	//virtual ~CGridElementsSelector(void);
	~CNewZoneWidget(void);

	// Handles the events
	static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata);

	// ProcessEvents() dispatches to these methods.
	void OnMouseMove();
	void OnLeftButtonDown();
	void OnLeftButtonUp();

	int State;
	enum WidgetState
	{
		Start=0,
		Selecting,
	};

	// 3D Cursor
	vtkCursor3D           *Cursor3D;
	vtkPolyDataMapper     *Cursor3DMapper;
	vtkActor              *Cursor3DActor;

	// wireframe outline
	vtkActor             *OutlineActor;
	vtkPolyDataMapper    *OutlineMapper;
	vtkOutlineSource     *OutlineSource;
	vtkFloatingPointType  StartPoint[3];

	// Mouse state
	vtkFloatingPointType  FixedPlanePoint[3];
	int                   FixedPlane;  // [0-5]
	int                   FixedCoord;  // [0-2]


	//
// COMMENT: {5/10/2006 4:23:08 PM}	CGridActor  *GridActor;

// COMMENT: {5/8/2006 7:11:45 PM}	// 
// COMMENT: {5/8/2006 7:11:45 PM}	CWPhastDoc  *Document;

private:
	CNewZoneWidget(const CNewZoneWidget&);  // Not implemented.
	void operator=(const CNewZoneWidget&);   // Not implemented.

};
