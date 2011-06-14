#pragma once

#include <vtk3DWidget.h>
#include "srcWedgeSource.h"
#include "WPhastDoc.h"
#include "Units.h"
#include "Zone.h"

class vtkCursor3D;
class vtkPolyDataMapper;
class vtkActor;
class Wedge;

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
	virtual void PlaceWidget(double bounds[6]);

	double* GetBounds(void) { return this->OutlineActor->GetBounds(); }
	void GetBounds(double bounds[6]) { this->OutlineActor->GetBounds(bounds); }

	enum srcWedgeSource::tagChopType GetChopType()const;

	void SetChopType(enum srcWedgeSource::tagChopType t);

	void SetGridKeyword(const CGridKeyword& gridKeyword, const CUnits& units);
	void SetCoordinateMode(CWPhastDoc::CoordinateState mode);
	void SetScale(double x, double y, double z);

	Wedge* GetWedge();

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
	double                StartPoint[3];

	// Mouse state
	double                FixedPlanePoint[3];
	int                   FixedPlane;  // [0-5]
	int                   FixedCoord;  // [0-2]

	// coordinate system vars
	double                       GridOrigin[3];
	double                       GridAngle;
	double                       GeometryScale[3];
	CUnits                       Units;
	CWPhastDoc::CoordinateState  CoordinateMode;
	CZone                        MapZone;

private:
	CNewWedgeWidget(const CNewWedgeWidget&);  // Not implemented.
	void operator=(const CNewWedgeWidget&);   // Not implemented.

};
