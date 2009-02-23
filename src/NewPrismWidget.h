#pragma once

#include <vtk3DWidget.h>

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

class vtkCursor3D;
class vtkPolyDataMapper;
class vtkActor;
class vtkPoints;
class vtkCellArray;
class vtkPolyData;

class CNewPrismWidget : public vtk3DWidget
{
public:
	// Description:
	// Instantiate the object.
	static CNewPrismWidget *New();

	vtkTypeRevisionMacro(CNewPrismWidget, vtk3DWidget);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Methods that satisfy the superclass' API.
	virtual void SetEnabled(int);
	virtual void PlaceWidget(vtkFloatingPointType bounds[6]);
	vtkPoints* GetPoints(void) { return Points; };

// COMMENT: {5/21/2008 8:58:23 PM}	vtkFloatingPointType* GetBounds(void) { return this->OutlineActor->GetBounds(); }
// COMMENT: {5/21/2008 8:58:23 PM}	void GetBounds(vtkFloatingPointType bounds[6]) { this->OutlineActor->GetBounds(bounds); }

#ifdef WIN32
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
#endif

protected:
	CNewPrismWidget(void);
	~CNewPrismWidget(void);

	// Handles the events
	static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata);

	// ProcessEvents() dispatches to these methods.
	void OnMouseMove();
	void OnLeftButtonDown();
	void OnLeftButtonUp();
	void OnKeyPress(void);


	// 3D Cursor
	vtkCursor3D           *Cursor3D;
	vtkPolyDataMapper     *Cursor3DMapper;
	vtkActor              *Cursor3DActor;

	int State;
	enum WidgetState
	{
		Start=0,
		CreatingPrism,
	};

	// wireframe outline
	vtkActor             *OutlineActor;
	vtkPolyDataMapper    *OutlineMapper;
// COMMENT: {5/21/2008 8:58:01 PM}	srcWedgeSource       *WedgeSource;
	vtkIdType             LastPointId;
	vtkIdType             LastCellId;
	vtkPoints            *Points;
	vtkCellArray         *CellArray;
	vtkPolyData          *PolyData;
	vtkFloatingPointType  StartPoint[3];

	// Mouse state
	vtkFloatingPointType  FixedPlanePoint[3];
	int                   FixedPlane;  // [0-5]
	int                   FixedCoord;  // [0-2]

private:
	CNewPrismWidget(const CNewPrismWidget&);  // Not implemented.
	void operator=(const CNewPrismWidget&);   // Not implemented.

};
