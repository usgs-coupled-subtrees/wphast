#pragma once

#include <vtk3DWidget.h>
#include "srcWedgeSource.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class vtkCursor3D;
class vtkPolyDataMapper;
class vtkActor;

class CNewWedgeWidget : public vtk3DWidget
{
public:
	// Description:
	// Instantiate the object.
	static CNewWedgeWidget *New();

	vtkTypeRevisionMacro(CNewWedgeWidget, vtk3DWidget);

	// Description:
	// Methods that satisfy the superclass' API.
	virtual void SetEnabled(int);
	virtual void PlaceWidget(vtkFloatingPointType bounds[6]);

	vtkFloatingPointType* GetBounds(void) { return this->OutlineActor->GetBounds(); }
	void GetBounds(vtkFloatingPointType bounds[6]) { this->OutlineActor->GetBounds(bounds); }

	enum srcWedgeSource::tagChopType GetChopType()const;

	void SetChopType(enum srcWedgeSource::tagChopType t);


#ifdef WIN32
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
#endif

protected:
	CNewWedgeWidget(void);
	~CNewWedgeWidget(void);

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
	srcWedgeSource       *WedgeSource;
	vtkFloatingPointType  StartPoint[3];

	// Mouse state
	vtkFloatingPointType  FixedPlanePoint[3];
	int                   FixedPlane;  // [0-5]
	int                   FixedCoord;  // [0-2]

private:
	CNewWedgeWidget(const CNewWedgeWidget&);  // Not implemented.
	void operator=(const CNewWedgeWidget&);   // Not implemented.

};
