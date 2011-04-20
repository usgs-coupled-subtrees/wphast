#pragma once

#include <vtk3DWidget.h>

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
	virtual void PlaceWidget(double bounds[6]);
	vtkPoints* GetPoints(void) { return Points; };

#if defined(_MFC_VER)
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

	void CancelNew(void);
	void EndNew(void);

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
	vtkIdType             LastPointId;
	vtkIdType             LastCellId;
	vtkPoints            *Points;
	vtkCellArray         *CellArray;
	vtkPolyData          *PolyData;
	double                StartPoint[3];

	// Mouse state
	double                FixedPlanePoint[3];
	int                   FixedPlane;  // [0-5]
	int                   FixedCoord;  // [0-2]

	// Double click parameters
	int   NumberOfClicks;
#if defined(_MFC_VER)
	RECT  RectClick;
	DWORD TimeLastClick;
#endif

private:
	CNewPrismWidget(const CNewPrismWidget&);  // Not implemented.
	void operator=(const CNewPrismWidget&);   // Not implemented.

};
